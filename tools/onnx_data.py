import yaml
import caffe
import sys, os, getopt
import numpy as np
from PIL import Image
import copy
from types import FunctionType
import csv
import re
import random
from fpbinary import FpBinary
import math

import onnx
import onnxruntime
import onnx.numpy_helper

import tools.graphs as graphs
import tools.layer_enum as layer_enum
import tools.onnx_helper as onnx_helper

from tools.layer_enum import LAYER_TYPE
from tools.array_init import array_init

SCALE=256

def fixed_point(val,total_width=16,int_width=8):
    val = min(val,  2**(int_width-1))
    val = max(val, -2**(int_width-1))
    return FpBinary(int_bits=int_width,frac_bits=(total_width-int_width),signed=True,value=val)


class ONNXData:
    def __init__(self, partition, model_path):
        #self.data_in = np.array(Image.open(filepath),dtype=np.float32)
        # partitions
        self.partition = partition
        # model
        self.model = onnx_helper.load(model_path)
        self.model = onnx_helper.update_batch_size(self.model,self.partition.batch_size)
        # add intermediate layers to outputs
        for node in self.model.graph.node:
            layer_info = onnx.helper.ValueInfoProto()
            layer_info.name = node.output[0]
            self.model.graph.output.append(layer_info)
        # add input aswell to output
        layer_info = onnx.helper.ValueInfoProto()
        layer_info.name = self.model.graph.input[0].name
        self.model.graph.output.append(layer_info)
        # remove bias
        self.remove_initializer_from_input()
        self.remove_bias()
        # inference session
        self.sess = onnxruntime.InferenceSession(self.model.SerializeToString())
        # get input data
        self.input_name  = self.sess.get_inputs()[0].name
        self.input_shape = self.sess.get_inputs()[0].shape
        # get output data
        self.output_name  = self.sess.get_outputs()[0].name
        self.output_shape = self.sess.get_outputs()[0].shape
        # create random data input 
        self.data = np.random.uniform(0,1,self.input_shape).astype(np.float32)

    def get_type(self,layer):
        return self.net.layers[list(self.net._layer_names).index(layer)].type

    def gen_data(self,dim,data_range=[0,1],data_type=float):
        # Initialise random data array
        data = np.ndarray(dim,dtype=data_type)
        # assign values
        for index,_ in np.ndenumerate(data):
            data[index] = data_type(random.uniform(data_range[0],data_range[1]))
        return data

    def normalise(self,data,scale=SCALE):
        return np.true_divide(data,scale)
        #return np.multiply(np.subtract(np.true_divide(data,scale),0.5),2)

    def remove_initializer_from_input(self):
        inputs = self.model.graph.input
        name_to_input = {}
        for input in inputs:
            name_to_input[input.name] = input
        for initializer in self.model.graph.initializer:
            if initializer.name in name_to_input:
                inputs.remove(name_to_input[initializer.name])

    def remove_bias(self):
        for layer in self.partition.layers:
            if layer.bias_path:
                initializer = onnx_helper.get_model_initializer(self.model, layer.bias_path, to_tensor=False)
                # TODO: seems like theres no bias initializer for inner product layer
                if not initializer:
                    continue
                zeroes = np.zeros(onnx.numpy_helper.to_array(initializer).shape).astype(np.float32)
                initializer_new = onnx.numpy_helper.from_array(zeroes,name=initializer.name)
                self.model.graph.initializer.remove(initializer)
                self.model.graph.initializer.extend([initializer_new])

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    DATA
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

    def _convert_fixed_port_stream(self, stream_in, total_width=16, int_width=8):
        stream_out = np.ndarray(shape=stream_in.shape, dtype=FpBinary)
        for index,val in np.ndenumerate(stream_in):
            stream_out[index] = fixed_point(val,total_width=total_width,int_width=int_width)
        return stream_out

    def _fixed_point_stream_to_bin(self, stream, output_path=None, streams=1, port_width=64, ports=1):
        # check it's only a 1D array
        assert len(stream.shape) == 1
        # check the stream is fixed-point
        # TODO
        # get width of fixed point data
        data_width = sum(stream[0].format)
        # check theres enough ports for the streams
        if streams > ports*(port_width/data_width):
            raise ValueError
        # get port data type
        if   port_width == 8:
            port_type = np.uint8
        elif port_width == 16:
            port_type = np.uint16
        elif port_width == 32:
            port_type = np.uint32
        elif port_width == 64:
            port_type = np.uint64
        else:
            raise TypeError
        # get stream data type
        if   0  < data_width <= 8:
            data_type = np.uint8
        elif 8  < data_width <= 16:
            data_type = np.uint16
        elif 16 < data_width <= 32:
            data_type = np.uint32
        elif 32 < data_width <= 64:
            data_type = np.uint64
        else:
            raise TypeError
        # check streams are a factor of the stream shape
        if not stream.shape[0]%streams == 0:
            raise ValueError
        # get the size of the binary streas out
        size = int(stream.shape[0]/streams)
        # binary stream out
        bin_out = np.zeros([ports,size], dtype=port_type)
        # copy stream to binary stream out
        for i in range(size):
            for j in range(streams):
                port_index = math.floor((j*data_width)/port_width)
                stream_val = data_type( stream[i*streams+j].bits_to_signed() & ((2**data_width)-1) )
                bin_out[port_index][i] |= port_type( stream_val  << (data_width*j)%port_width )
        # save to binary file
        for i in range(ports):
            bin_out[i].astype(port_type).tofile(output_path+"_{i}.bin".format(i=i))
        # return binary stream
        return bin_out

    def transform_featuremap(self, featuremap):
        # normalise
        featuremap = self.normalise(featuremap) # TODO: remove
        # transform featuremap
        return np.moveaxis(featuremap, 1, -1)
        # TODO: handle 1D and 2D featuremaps

    def save_featuremap(self, featuremap, output_path, to_yaml=False, to_bin=False, to_csv=False):
        # yaml format
        if to_yaml:
            # save to yaml file
            with open(output_path+'.yaml', 'w') as f:
                yaml.dump({
                    "batch_size": featuremap.shape[0],
                    "rows"      : featuremap.shape[1],
                    "cols"      : featuremap.shape[2],
                    "channels"  : featuremap.shape[3],
                    "data"      : featuremap.reshape(-1).tolist() }, f)
        # binary format
        if to_bin:
            # get feature map stream
            stream = self._convert_fixed_port_stream(featuremap.reshape(-1))
            # save to binary file
            self._fixed_point_stream_to_bin(stream, output_path=output_path) # TODO: add port info
        # csv format
        if to_csv:
            pass

    def save_featuremap_in_out(self, output_path, to_bin=False, to_csv=False):
        # save input layer
        input_node = self.partition.input_node
        input_data = np.array( self.sess.run([input_node], { self.input_name : self.data } )[0] )
        input_data = self.transform_featuremap(input_data)
        self.save_featuremap(input_data, os.path.join(output_path, onnx_helper._format_name(input_node)), 
            to_yaml=False, to_bin=to_bin, to_csv=to_csv)
        # save output layer
        output_node = self.partition.output_node
        output_data = np.array( self.sess.run([output_node], { self.input_name : self.data } )[0] )
        output_data = self.transform_featuremap(output_data)
        print(output_data.shape)
        self.save_featuremap(output_data, os.path.join(output_path, onnx_helper._format_name(input_node)), 
            to_yaml=False, to_bin=to_bin, to_csv=to_csv)
        # save yaml data
        data = {
            "in"  : input_data.reshape(-1).tolist(),
            "out" : output_data.reshape(-1).tolist()
        }
        # yaml format
        if output_path:
            # save to yaml file
            with open(os.path.join(output_path,'data.yaml'), 'w') as f:
                yaml.dump(data, f)

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    WEIGHTS
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

    def _transform_weights(self,weights_raw,wr_factor=1,coarse_in=1,coarse_out=1):
        # parameters
        num_filters  = int(weights_raw.shape[0]/(coarse_out*wr_factor))
        num_channels = int(weights_raw.shape[1]/coarse_in)
        k_size       = weights_raw.shape[2]
        # correct output shape for weights
        weights = np.ndarray(
            shape=(
                wr_factor,
                coarse_in,
                coarse_out,
                num_channels,
                num_filters,
                k_size,k_size),dtype=float,order='C')
        # transform weights raw shape
        for index,_ in np.ndenumerate(weights):
            weights[index] = weights_raw[
                      index[4]*wr_factor*coarse_out+index[0]*coarse_out+index[2],
                      index[3]*coarse_in+index[1],
                      index[5],
                      index[6]]
        # merge channel and filter dimensions
        weights = np.reshape(weights,[wr_factor,coarse_in,coarse_out,num_channels*num_filters,k_size,k_size])
        # remove last two dimensions if kernel size is 1
        if k_size == 1:
            weights = weights[:,:,:,:,0,0]
        # return transformed weights
        return weights

    def get_weights_convolution(self, layer, wr_factor=1):
        # get weights
        weights_raw = onnx_helper.get_model_initializer(self.model, layer.weights_path)
        # transform parameters
        coarse_in   = layer.parameters.coarse_in
        coarse_out  = layer.parameters.coarse_out
        # return transformed weights
        return self._transform_weights(weights_raw,wr_factor,coarse_in,coarse_out)

    def get_weights_inner_product(self, layer, wr_factor=1):
        # get weights
        weights_raw = onnx_helper.get_model_initializer(self.model, layer.weights_path)
        # transform parameters
        coarse_in   = layer.parameters.coarse_in
        coarse_out  = layer.parameters.coarse_out
        filters     = layer.parameters.filters
        channels    = layer.parameters.channels_in
        rows        = layer.parameters.rows_in
        cols        = layer.parameters.cols_in
        #reshape for transforming
        weights_raw = np.reshape(weights_raw,(filters,channels,rows,cols))
        weights_raw = np.rollaxis(weights_raw,1,3)
        weights_raw = np.reshape(weights_raw,(filters,rows*cols*channels,1,1))
        # return transformed weights
        return self._transform_weights(weights_raw,wr_factor,coarse_in,coarse_out)

    def save_weights_layer(self,layer,wr_factor=1,output_path=None,to_yaml=False,to_bin=False,to_csv=False):
        # get transformed weights
        if layer_enum.from_proto_layer_type(layer.type) == layer_enum.LAYER_TYPE.Convolution:
            transformed_weights = self.get_weights_convolution(layer, wr_factor=wr_factor)
        elif layer_enum.from_proto_layer_type(layer.type) == layer_enum.LAYER_TYPE.InnerProduct:
            transformed_weights = self.get_weights_inner_product(layer, wr_factor=wr_factor)
        else:
            raise TypeError
        # save weights
        if output_path:
            # csv format
            if to_csv:
                # iterate over weights reloading factor
                for weights_reloading_index in range(wr_factor):
                    # get filepath name
                    filepath = f'{output_path}_{weights_reloading_index}.csv'
                    # save to csv file
                    with open(filepath, 'w') as f:
                        f.write(array_init(transformed_weights[weights_reloading_index]))
            # bin format
            if to_bin:
                # iterate over weights reloading factor
                for weights_reloading_index in range(wr_factor):
                    weights_stream =  self._convert_fixed_port_stream(transformed_weights[weights_reloading_index].reshape(-1), total_width=16, int_width=8)
                    self._fixed_point_stream_to_bin(weights_stream, output_path=f'{output_path}_{weights_reloading_index}', streams=1, port_width=64, ports=1)
        # return transformed weights
        return transformed_weights

    def save_weights_partition(self,output_path,to_yaml=False,to_bin=False,to_csv=False):
        weights = {}
        # iterate over layers in network
        for layer in self.partition.layers:
            # skip weights outside of partition
            if layer_enum.from_proto_layer_type(layer.type) in [ layer_enum.LAYER_TYPE.Convolution, layer_enum.LAYER_TYPE.InnerProduct ]:
                # get weights reloading factor
                if layer.name == self.partition.weights_reloading_layer:
                    wr_factor = self.partition.weights_reloading_factor
                else:
                    wr_factor = 1
                # get output path
                output_path_layer = None
                if output_path:
                    output_path_layer = os.path.join(output_path,f"{layer.name}_weights")
                # get layer info
                weights[layer.name] = self.save_weights_layer(layer,wr_factor=wr_factor,
                        output_path=output_path_layer,to_bin=to_bin,to_csv=to_csv)
        # yaml format
        if to_yaml:
            tmp = {}
            for layer in weights:
                tmp[layer] = weights[layer].reshape(-1).tolist()
            # save to yaml file
            with open(os.path.join(output_path,'weights.yaml'), 'w') as f:
                yaml.dump(tmp, f)
        # return weights
        return weights

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    WEIGHTS
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

    def save_batch_norm(self,node_info,output_path):
        # weights
        scale = {}
        shift = {}
        # iterate over each layer
        for node in node_info:
            if node_info[node]['type'] != LAYER_TYPE.BatchNorm:
               continue
            # scale layer
            scale_layer = node_info['hw'].scale_layer
            # get parameters
            mean  = self.net.params[layer][0].data
            var   = self.net.params[layer][1].data
            gamma = self.net.params[scale_layer][0].data
            beta  = self.net.params[scale_layer][1].data
            eps   = np.float_power(10,-5)
            # transform to 2 operations
            scale[node] = gamma / np.sqrt( var + eps )
            shift[node] = ( beta / scale[node] - mean )

        if output_path:
            # save scale coeffients
            with open(output_path + '/scale.yaml', 'w') as f:
                yaml.dump(scale, f)
            # save shift coeffients
            with open(output_path + '/shift.yaml', 'w') as f:
                yaml.dump(shift, f)

        return scale, shift

    def transform_batch_norm_coef(self,coef_raw,coarse=1):
        # parameters
        num_channels = int(coef_raw.shape[0]/coarse)

        # correct output shape for weights
        coef = np.ndarray(
            shape=(
                coarse,
                num_channels),dtype=float,order='C')

        # transform weights raw shape
        for index,_ in np.ndenumerate(coef):
            coef[index] = coef_raw[index[1]*coarse+index[0]]
            #coef[index] = coef_raw[index[0]*num_channels+index[1]]

        return coef

    def save_batch_norm_csv(self,output_path,net_info):
        # find layers with weights
        for layer in self.net.params:
            if self.get_type(layer) == 'BatchNorm':
                layer_index = list(self.net.params.keys()).index(layer)
                next_layer  = list(self.net.params.keys())[layer_index+1]
                mean  = self.net.params[layer][0].data
                var   = self.net.params[layer][1].data
                gamma = self.net.params[next_layer][0].data
                beta  = self.net.params[next_layer][1].data

                print(layer)

                eps = np.float_power(10,-5)
                # get coarse factor
                try:
                    coarse_in  = net_info[layer]['coarse_in']
                except KeyError:
                    coarse_in  = 1

                # channels
                num_channels = int(self.net.params[layer][0].shape[0]/coarse_in)

                # scale
                scale = np.ndarray([
                    num_channels,
                    coarse_in
                ],dtype=float)
                for index,_ in np.ndenumerate(scale):
                    i = index[0]*coarse_in+index[1]
                    scale[index] = gamma[i]/np.sqrt(var[i]+eps)
                # scale
                offset = np.ndarray([
                    num_channels,
                    coarse_in
                ],dtype=float)
                for index,_ in np.ndenumerate(offset):
                    i = index[0]*coarse_in+index[1]
                    offset[index] = beta[i] - mean[i]*gamma[i]/np.sqrt(var[i]+eps)
                # save csv
                filepath = '{path}/{layer}_scale.csv'.format( path=output_path,layer=layer.replace("/","_") )
                with open(filepath, 'w') as writeFile:
                    writer = csv.writer(writeFile)
                    writer.writerows([scale.reshape(-1).tolist()])
                filepath = '{path}/{layer}_offset.csv'.format( path=output_path,layer=layer.replace("/","_") )
                with open(filepath, 'w') as writeFile:
                    writer = csv.writer(writeFile)
                    writer.writerows([offset.reshape(-1).tolist()])

if __name__ == "__main__":
    caffe_data = CaffeData()
    caffe_data.load_net(
        'data/models/lenet.prototxt',
        'data/weights/lenet.caffemodel'
    )
    caffe_data.load_input(
        #'data/imagenet_0_vgg.jpg'
        'data/inputs/lenet_0.png'
    )
    print('run ...')
    caffe_data.run_net()
    print('save data ...')
    #caffe_data.save_data('.')
    print('save weight ...')
    #caffe_data.save_weight('.')
    print('save weight csv ...')
    #caffe_data.save_weight_csv('.',{})
