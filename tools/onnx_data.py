import yaml
import sys, os, getopt
import numpy as np
from PIL import Image, ImageOps
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

import torch
import torchvision.transforms.functional as TF

import fpgaconvnet_optimiser.tools.graphs as graphs
import fpgaconvnet_optimiser.tools.layer_enum as layer_enum
from fpgaconvnet_optimiser.tools.layer_enum import LAYER_TYPE
import fpgaconvnet_optimiser.tools.onnx_helper as onnx_helper
import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2 as fpgaconvnet_pb2

try:
    from tools.array_init import array_init
except ModuleNotFoundError:
    from array_init import array_init

SCALE=256

def fixed_point(val,total_width=16,int_width=8):
    val = min(val,  2**(int_width-1))
    val = max(val, -2**(int_width-1))
    return FpBinary(int_bits=int_width,frac_bits=(total_width-int_width),signed=True,value=val)

def get_layer_from_partition(partition, layer_name): # Non ONNXData class version
    for layer in partition.layers:
        if layer.name == layer_name:
            return layer
    raise NameError("layer name not in partition")

def gen_layer_name(layer): # layer in protobuf form
    #macro issue is that layers that have numerical names cause compiler to error
    layer_type_str = str(fpgaconvnet_pb2.layer.layer_type.Name(layer.type))
    if layer.name.isnumeric(): # preprend with type to avoid macro issue
        return f'{layer_type_str}{layer.name}'
    else:
        return layer.name

def bin_to_fixed_point(filename, data_width=16, int_width=8):
    #get numpy data type from width
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

    #import the file
    bin_in = np.fromfile(filename, dtype=data_type)

    #TODO convert to fixed point with integer component of width int_width
    #TODO maybe convert to floating point?
    #for b in bin_in:
    #    print(b)

    return bin_in

def validate_output(args):
    #get fixed point tolerance
    errtol = fixed_point(args.error_tolerance, args.data_width) #TODO add int_width
    print(f"Error tolerance as fxpbin: {errtol}, as uint: {errtol.bits_to_signed()}")
    #load golden data, convert to float
    g_dat = bin_to_fixed_point(args.gold_path, args.data_width)
    #load actual data, convert to float
    a_dat = bin_to_fixed_point(args.actual_path, args.data_width)
    #compare values, check length, compute average error
    if len(g_dat) != len(a_dat):
        raise ValueError(f"Data length differs! Expected: {len(g_dat)} Actual: {len(a_dat)}")

    max_err=0.0
    for idx,(g,a) in enumerate(zip(g_dat, a_dat)):
        #print(idx," : ",g," : ",a)
        cmpr = (g-a) if (g > a) else (a-g)
        max_err = max(max_err,cmpr)
        if cmpr > errtol.bits_to_signed():
            raise ValueError(f"Difference greater than tolerance.\n \
                    Values g:{g} a:{a} @ index:{idx}")

    err_p = (max_err)/errtol.bits_to_signed()
    raw_err_mx = err_p*args.error_tolerance
    print("Made it to end with no errors exceeding tolerance! YAY")
    print("(Maximum error was ~{:.2f}% of tolerance, ~{:.5f})".format(100*err_p, raw_err_mx))

class ONNXData:
    def __init__(self, partition, model_path=None):
        # partitions
        self.partition = partition
        self.model = None
        if model_path:
            # model
            self.model = onnx_helper.load(model_path)
            self.model = onnx_helper.update_batch_size(self.model,self.partition.batch_size)

            # merge subgraphs into main graph
            self.merge_subgraphs()
            # add intermediate layers to outputs
            new_outputs=[]
            for node in self.model.graph.node:
                og_VIP = onnx_helper.get_ValueInfoProto_from_node(self.model.graph,node)
                VIP = onnx.ValueInfoProto()
                VIP.CopyFrom(og_VIP)
                new_outputs.append(VIP)
            # add input aswell to output
            input_VIP = onnx.ValueInfoProto()
            input_VIP.CopyFrom(self.model.graph.input[0])
            new_outputs.append(input_VIP)
            # extending model with intermediate outputs
            self.model.graph.output.extend(new_outputs)

            # remove bias
            self.remove_initializer_from_input()
            self.remove_bias()

            # verify model is onnx compliant after changes
            onnx.checker.check_model(self.model)
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

    def load_input(self,filepath):
        self.data = np.array(Image.open(filepath),dtype=np.float32)
        #"normalising", more like scaling, input to prevent saturation of quant data types
        data_max = np.amax(self.data)
        self.data = self.data / data_max
        if len(self.data.shape) == 2:
            self.data = np.expand_dims(self.data,axis=0)
        self.data = np.stack([self.data for _ in range(self.partition.batch_size)], axis=0 )

    #TODO version of load input that takes in folder full of images, maybe randomised
    def load_inputs(self,filepath):
        img_ls = os.listdir(filepath)
        img_ls.sort()

        np_ls=[]
        for s in range(self.partition.batch_size):
            current_path = os.path.join(filepath,img_ls[s])
            # load in the numpy array
            #img = np.array(Image.open(current_path),dtype=np.float32)
            img = np.load(current_path)
            # scale images
            data_max = np.amax(img)
            img = img / data_max
            if len(img.shape) == 2:
                img = np.expand_dims(img,axis=0)
            np_ls.append(img)
        self.data = np.concatenate(np_ls, axis=0 )
        print("Input data shape:",self.data.shape)

    def get_layer(self,layer_name):
        for layer in self.partition.layers:
            if layer.name == layer_name:
                return layer

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
            if layer.bias_path and layer.parameters.has_bias==0:
                initializer = onnx_helper.get_model_initializer(self.model, layer.bias_path, to_tensor=False)
                # TODO: seems like theres no bias initializer for inner product layer
                if not initializer:
                    continue
                zeroes = np.zeros(onnx.numpy_helper.to_array(initializer).shape).astype(np.float32)
                initializer_new = onnx.numpy_helper.from_array(zeroes,name=initializer.name)
                self.model.graph.initializer.remove(initializer)
                self.model.graph.initializer.extend([initializer_new])

    def merge_subgraphs(self):
        #try and merge the subgraph nodes into the main graph nodes
        subnodes=[]
        valinfs=[]
        ifnode_idxs=[]
        #outputs=[]
        for idx,node in enumerate(self.model.graph.node):
            #expand subgraphs
            name = onnx_helper._name(node)
            if layer_enum.from_onnx_op_type(node.op_type) == LAYER_TYPE.If:
                for subgraph in node.attribute:
                    for og_node in subgraph.g.node:
                        # copy VIP information to new class
                        og_VIP = onnx_helper.get_ValueInfoProto_from_node(subgraph.g,og_node)
                        VIP = onnx.ValueInfoProto()
                        VIP.CopyFrom(og_VIP)
                        # copy node information
                        new_node = onnx.NodeProto()
                        new_node.CopyFrom(og_node)
                        # build lists of fields from sub-graphs
                        subnodes.append(new_node)
                        valinfs.append(VIP)
                # append ifnode index for removal
                ifnode_idxs.append(idx)
                #remove exit associated with the ifnode
                for op in self.model.graph.output:
                    if op.name == node.output[0]:
                        self.model.graph.output.remove(op)
        #extend main graph
        self.model.graph.node.extend(subnodes)
        self.model.graph.value_info.extend(valinfs)
        #remove ifnodes
        for idx in reversed(ifnode_idxs):
            self.model.graph.node.pop(idx)

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

    def _fixed_point_stream_format(self, stream, streams=1, port_width=16, ports=1):
        # check it's only a 1D array
        assert len(stream.shape) == 1
        # check the stream is fixed-point
        # TODO
        # get width of fixed point data
        data_width = sum(stream[0].format)
        # check theres enough ports for the streams
        if streams > ports*(port_width/data_width):
            print(f"streams:{streams} ports:{ports} pw:{port_width} dw:{data_width}")
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
                # print(stream[i*streams+j].bits_to_signed() & ((2**data_width)-1), data_type(stream[i*streams+j].bits_to_signed()))
                stream_val = data_type( stream[i*streams+j].bits_to_signed() & ((2**data_width)-1) )
                bin_out[port_index][i] |= port_type( stream_val  << (data_width*j)%port_width )
        # return the formatted stream
        return bin_out

    def _fixed_point_stream_to_bin(self, stream, output_path, streams=1, port_width=64, ports=1):
        # get the formatted_stream
        bin_out = self._fixed_point_stream_format(stream, streams=streams, port_width=port_width, ports=ports)
        # get the port type
        port_type = bin_out.dtype
        # save to binary file
        for i in range(ports):
            bin_out[i].astype(port_type).tofile(f"{output_path}_{i}.bin".format(i=i))

    def _fixed_point_stream_to_dat(self, stream, output_path, streams=1, port_width=64, ports=1):
        # get the formatted_stream
        bin_out = self._fixed_point_stream_format(stream, streams=streams, port_width=port_width, ports=ports)
        # save to binary file
        for i in range(ports):
            with open(f"{output_path}_{i}.dat", 'w') as f:
                f.write("\n".join([str(j) for j in bin_out[i]]))

    def transform_featuremap(self, featuremap):
        # normalise
        #featuremap = self.normalise(featuremap) # TODO: remove
        # transform featuremap
        if featuremap.ndim == 1:
            return featuremap
        return np.moveaxis(featuremap, 1, -1)
        # TODO: handle 1D and 2D featuremaps

    def save_featuremap(self, featuremap, output_path, parallel_streams=1, to_yaml=False, to_bin=False, to_csv=False, to_dat=False):
        # yaml format
        if to_yaml:
            # save to yaml file
            with open(output_path+'.yaml', 'w') as f:
                print("fm shape:",featuremap.shape)
                yaml.dump({
                    "batch_size": featuremap.shape[0],
                    "rows"      : featuremap.shape[1],
                    "cols"      : featuremap.shape[2],
                    "channels"  : featuremap.shape[3],
                    "data"      : featuremap.reshape(-1).tolist() }, f)
        # get feature map stream
        stream = self._convert_fixed_port_stream(featuremap.reshape(-1))
        # binary format
        if to_bin:
            print("WARNING: port width set to 16 bits for bin")
            self._fixed_point_stream_to_bin(stream, output_path, streams=parallel_streams, port_width=16)
        # dat format
        if to_dat:
            print("WARNING: port width set to 16 bits for dat")
            self._fixed_point_stream_to_dat(stream, output_path, streams=parallel_streams, port_width=16)
        # csv format
        if to_csv:
            pass

    def save_featuremap_in_out(self, output_path, to_bin=False, to_csv=False, to_dat=False):
        # save input layer
        input_node = self.partition.input_node
        input_data = np.array( self.sess.run([input_node], { self.input_name : self.data } )[0] )
        input_data = self.transform_featuremap(input_data)
        input_streams = int(self.partition.layers[0].parameters.coarse_in)
        self.save_featuremap(input_data, os.path.join(output_path, onnx_helper._format_name(input_node)),
            parallel_streams=input_streams, to_yaml=False, to_bin=to_bin, to_csv=to_csv, to_dat=to_dat)
        # save output layer
        output_node = self.partition.output_node
        output_data = np.array( self.sess.run([output_node], { self.input_name : self.data } )[0], dtype=np.float64) #making sure data type works
        output_data = self.transform_featuremap(output_data)
        #print("Output tensor:\n",output_data)
        output_streams = int(self.partition.layers[-1].parameters.coarse_out)
        self.save_featuremap(output_data, os.path.join(output_path, onnx_helper._format_name(output_node)),
            parallel_streams=output_streams, to_yaml=False, to_bin=to_bin, to_csv=to_csv, to_dat=to_dat)

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    WEIGHTS
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

    @staticmethod
    def _transform_weights(weights_raw,wr_factor,coarse_in,coarse_out,coarse_group,groups):
        # parameters
        num_filters  = int(weights_raw.shape[0]/(groups*coarse_out*wr_factor))
        num_channels = int(weights_raw.shape[1]/coarse_in)
        k_size_x       = weights_raw.shape[2]
        k_size_y       = weights_raw.shape[3]
        # correct output shape for weights
        weights = np.ndarray(
            shape=(
                wr_factor,
                coarse_group,
                coarse_in,
                coarse_out,
                int(groups/coarse_group),
                num_channels,
                num_filters,
                k_size_x,k_size_y),dtype=float,order='C')

        # transform weights raw shape
        for index,_ in np.ndenumerate(weights):
            weights[index] = weights_raw[
                      index[4]*coarse_group*num_filters*wr_factor*coarse_out+index[1]*num_filters*wr_factor*coarse_out+index[6]*wr_factor*coarse_out+index[0]*coarse_out+index[3],
                      index[5]*coarse_in+index[2],
                      index[7],
                      index[8]]
        # merge channel and filter dimensions
        weights = np.reshape(weights,[wr_factor,coarse_in*coarse_group,coarse_out,int(groups/coarse_group)*num_channels*num_filters,k_size_x,k_size_y])
        # return transformed weights
        return weights

    def get_weights_convolution(self, layer, wr_factor=1):
        # get weights
        if self.model:
            weights_raw = onnx_helper.get_model_initializer(self.model, layer.weights_path)
        else:
            print(f"WARNING: no initializer found for {layer.name}, creating a random initializer")
            dim = [layer.parameters.filters*wr_factor,
                int(layer.parameters.channels_in / layer.parameters.groups),
                layer.parameters.kernel_size[0],
                layer.parameters.kernel_size[1]]

            # Initialise random data array
            weights_raw = np.ndarray(dim,dtype=float)
            # assign values
            for index,_ in np.ndenumerate(weights_raw):
                weights_raw[index] = random.uniform(-8, 8) #todo: consistent with weight_t

        # transform parameters
        coarse_in   = layer.parameters.coarse_in
        coarse_out  = layer.parameters.coarse_out
        coarse_group  = layer.parameters.coarse_group
        groups = layer.parameters.groups
        # return transformed weights
        return self._transform_weights(weights_raw,wr_factor,coarse_in,coarse_out,coarse_group,groups)

    def get_weights_inner_product(self, layer, wr_factor=1):
        # get weights
        weights_raw = onnx_helper.get_model_initializer(self.model, layer.weights_path)
        #print("weights reshape: ",weights_raw.shape)
        #print("HAS BIAS",layer.parameters.has_bias)
        if not (layer.parameters.has_bias == 1):
        #if layer.parameters.matmul_flag:
            print("MatMul ONNX Operation used, transposing")
            weights_raw = np.matrix.transpose(weights_raw)
            print("weights transpose: ",weights_raw.shape)
        # transform parameters
        coarse_in   = layer.parameters.coarse_in
        coarse_out  = layer.parameters.coarse_out
        filters     = layer.parameters.filters
        channels    = layer.parameters.channels_in
        rows        = layer.parameters.rows_in
        cols        = layer.parameters.cols_in
        #reshape for transforming
        weights_raw = np.reshape(weights_raw,(filters*wr_factor,channels,rows,cols))
        #print("weights reshape: ",weights_raw.shape)
        weights_raw = np.rollaxis(weights_raw,1,4) #input filters need to be FINAL axis
        #print("weights rollaxis: ",weights_raw.shape)
        weights_raw = np.reshape(weights_raw,(filters*wr_factor,rows*cols*channels,1,1))
        #print("weights reshape2: ",weights_raw.shape)
        # return transformed weights
        return self._transform_weights(weights_raw,wr_factor,coarse_in,coarse_out,1,1)

    def save_weights_layer(self,layer,wr_factor=1,output_path=None,to_yaml=False,to_bin=False,to_csv=False,to_dat=False):
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
            # get the bitwidth for the weights
            bitwidth = layer.parameters.weight_width
            # flatten the weights for binary and data formats
            weights_stream =  self._convert_fixed_port_stream(transformed_weights.reshape(-1), total_width=bitwidth, int_width=bitwidth//2)
            # bin format
            if to_bin:
                self._fixed_point_stream_to_bin(weights_stream, output_path=output_path, streams=1, port_width=64, ports=1)
            # dat format
            if to_dat:
                self._fixed_point_stream_to_dat(weights_stream, output_path=output_path, streams=1, port_width=64, ports=1)
        # return transformed weights
        return transformed_weights

    def save_weights_partition(self,output_path,to_yaml=False,to_bin=False,to_csv=False,to_dat=False):

        def _fix_identifier(name):
            if name[0].isdigit():
                return "n" + name
            else:
                return name
        weights = {}
        # iterate over layers in network
        for layer in self.partition.layers:
            layer_type_str = str(fpgaconvnet_pb2.layer.layer_type.Name(layer.type)) # REQUIRED EDIT
            layer_name = gen_layer_name(layer) # REQUIRED EDIT
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
                    layer_identifier = _fix_identifier(layer.name)
                    output_path_layer = os.path.join(output_path,f"{layer_identifier}_weights")
                # get layer info
                weights[layer.name] = self.save_weights_layer(layer,wr_factor=wr_factor,
                        output_path=output_path_layer,to_bin=to_bin,to_csv=to_csv,to_dat=to_dat)
        return weights

    """
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    BIASES
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    """

    @staticmethod
    def _transform_biases(biases_raw, filters, coarse_out,wr_factor=1):
        # parameters
        if wr_factor == -1:
            num_filters  = biases_raw.shape[0]//(coarse_out)
            biases = np.ndarray(
                shape=(
                    coarse_out,
                    num_filters
                    ), dtype=float, order='C')#order is row major

            # transform biases raw shape
            for index,_ in np.ndenumerate(biases):
                biases[index] = biases_raw[coarse_out*index[1]+index[0]]
        else:
            num_filters  = biases_raw.shape[0]//(coarse_out*wr_factor)
            biases = np.ndarray(
                shape=(
                    wr_factor,
                    coarse_out,
                    num_filters
                    ), dtype=float, order='C')#order is row major

            # transform biases raw shape
            for index,_ in np.ndenumerate(biases):
                biases[index] = biases_raw[coarse_out*wr_factor*index[2]+index[1]+index[0]]

        # return transformed biases
        return biases

    def get_biases_convolution(self, layer, wr_factor=-1):
        # get biases
        if self.model:
            biases_raw = onnx_helper.get_model_initializer(self.model, layer.bias_path)
        else:
            print(f"WARNING: no initializer found for {layer.name}")
            #, creating a random initializer") # FIXME - see get weights conv

        # transform parameters
        coarse_out  = layer.parameters.coarse_out
        filters = layer.parameters.filters
        # return transformed biases
        return self._transform_biases(biases_raw,filters,coarse_out,wr_factor)

    def get_biases_inner_product(self, layer,wr_factor=-1):
        # get biases
        biases_raw = onnx_helper.get_model_initializer(self.model, layer.bias_path)
        # transform parameters
        coarse_out  = layer.parameters.coarse_out
        filters     = layer.parameters.filters
        channels    = layer.parameters.channels_in
        rows        = layer.parameters.rows_in
        cols        = layer.parameters.cols_in
        return self._transform_biases(biases_raw,filters,coarse_out,wr_factor)

    def save_biases_layer(self,layer,wr_factor=-1,output_path=None,to_yaml=False,to_bin=False,to_csv=False,
                            to_dat=False):
        # get transformed biases
        if layer_enum.from_proto_layer_type(layer.type) == layer_enum.LAYER_TYPE.Convolution:
            transformed_biases = self.get_biases_convolution(layer,wr_factor)
        elif layer_enum.from_proto_layer_type(layer.type) == layer_enum.LAYER_TYPE.InnerProduct:
            transformed_biases = self.get_biases_inner_product(layer,wr_factor)
        else:
            raise TypeError
        # save biases
        if output_path:
            # csv format
            if to_csv:
                # get filepath name
                filepath = f'{output_path}.csv'
                # save to csv file
                with open(filepath, 'w') as f:
                    f.write(array_init(transformed_biases))

            # get the bitwidth for the biases
            bitwidth = layer.parameters.biases_width
            # flatten the biases for binary and data formats
            biases_stream =  self._convert_fixed_port_stream(transformed_biases.reshape(-1), total_width=bitwidth, int_width=bitwidth//2)
            # bin format
            if to_bin:
                self._fixed_point_stream_to_bin(biases_stream, output_path=output_path, streams=1, port_width=64, ports=1)
            # dat format
            if to_dat:
                self._fixed_point_stream_to_dat(biases_stream, output_path=output_path, streams=1, port_width=64, ports=1)
        # return transformed biases
        return transformed_biases
    def save_biases_partition(self,output_path,to_yaml=False,to_bin=False,to_csv=False,
                                to_dat=False):

        def _fix_identifier(name):
            if name[0].isdigit():
                return "n" + name
            else:
                return name
        biases = {}
        # iterate over layers in network
        for layer in self.partition.layers: # skip biases outside of partition
            layer_type_str = str(fpgaconvnet_pb2.layer.layer_type.Name(layer.type)) # REQUIRED EDIT
            layer_name = gen_layer_name(layer) # REQUIRED EDIT
            if layer_enum.from_proto_layer_type(layer.type) in \
                    [ layer_enum.LAYER_TYPE.Convolution, layer_enum.LAYER_TYPE.InnerProduct ]:
                # get weights reloading factor
                if layer.name == self.partition.weights_reloading_layer:
                    wr_factor = self.partition.weights_reloading_factor
                else:
                    wr_factor = -1
                # get output path
                output_path_layer = None
                if output_path:
                    layer_identifier = _fix_identifier(layer.name)
                    output_path_layer = os.path.join(output_path,f"{layer_identifier}_biases")
                if layer.parameters.has_bias: # skip layers with no bias
                    # get layer info
                    biases[layer.name] = self.save_biases_layer(layer,wr_factor,
                                                                output_path=output_path_layer,
                                                                to_bin=to_bin,to_csv=to_csv,
                                                                to_dat=to_dat)
        return biases

if __name__ == "__main__":
    bin_to_fixed_point()
