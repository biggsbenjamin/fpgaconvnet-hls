import sys
import os
import numpy as np
import csv
import copy

sys.path.append("..")
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.ConvolutionLayer import ConvolutionLayer
import generate.layers.convolution
from Layer import Layer
from tools.onnx_data import ONNXData
from tools.array_init import array_init

class ConvolutionLayerTB(Layer):
    def __init__(self):
        self.name = 'convolution_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):

        # Init Module
        layer = ConvolutionLayer(
            self.param['filters'],
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            kernel_size=self.param['kernel_size'],
            stride=self.param['stride'],
            groups=self.param['groups'],
            pad=self.param['pad'],
            coarse_in=self.param['coarse_in'],
            coarse_out=self.param['coarse_out'],
            coarse_group=self.param['coarse_group'],
            fine=self.param['fine'],
            input_width=self.param["input_width"],
            output_width=self.param["output_width"],
            acc_width=self.param["acc_width"],
            weight_width=self.param["weight_width"],
            biases_width=self.param["biases_width"]
        )

        # update parameters
        self.param["kernel_size"] = layer.kernel_size
        self.param["stride"] = layer.stride
        self.param["pad_top"] = layer.pad_top
        self.param["pad_right"] = layer.pad_right
        self.param["pad_bottom"] = layer.pad_bottom
        self.param["pad_left"] = layer.pad_left

        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])

        # weights
        weights = self.gen_data([
            self.param['filters'],
            self.param['channels_in']//self.param['groups'],
            self.param['kernel_size'][0],
            self.param['kernel_size'][1]
        ],[-8,8]) #todo: consistent with weight_t

        # biases
        biases = np.zeros(self.param['filters'])
        if self.param['has_bias'] == 1:
            biases = self.gen_data([
                self.param['filters']
            ])

        # generate data out with functional model
        data_out = layer.functional_model(copy.copy(data_in),weights,biases)[0]
        data_out = np.moveaxis(data_out,0,-1)

        # save weights
        weights = ONNXData._transform_weights(
            weights,
            1,
            self.param['coarse_in'],
            self.param['coarse_out'],
            self.param['coarse_group'],
            self.param['groups']
        )
        #print(weights)
        with open('data/weights.csv', 'w') as f:
            f.write(array_init(weights[0]))

        if self.param['has_bias'] == 1:
            # save biases #FIXME finish placeholder fn
            biases = ONNXData._transform_biases(
                biases,
                self.param['filters'],
                self.param['coarse_out']
            )
            #print(biases) #FIXME save the biases
            with open('data/biases.csv', 'w') as f:
                f.write(array_init(biases))#[0]))


        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

        data_in = data_in.reshape(
            self.param['rows_in'],
            self.param['cols_in'],
            int(self.param['groups']/self.param['coarse_group']),
            self.param['coarse_group'],
            int(self.param['channels_in']/(self.param['groups']*self.param['coarse_in'])),
            self.param['coarse_in']
        )
        data_in = data_in.transpose((0,1,2,4,3,5))

        data_out = data_out.reshape(
            self.param['rows_out'],
            self.param['cols_out'],
            int(self.param['groups']/self.param['coarse_group']),
            self.param['coarse_group'],
            int(self.param['channels_out']/(self.param['groups']*self.param['coarse_out'])),
            self.param['coarse_out']
        )
        #print(data_out)
        data_out = data_out.transpose(0,1,2,4,3,5)

        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist(),
            'weights': array_init(weights[0]),
            'biases' : array_init(biases)
        }
        # resource and latency model
        model = {
            'latency'   : layer.latency(),
            'resources' : layer.resource()
        }
        return data, model

    # update layer generation
    def gen_layer(self,src_path,header_path):
        generate.layers.convolution.gen_convolution_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    convolution_layer_tb = ConvolutionLayerTB()
    convolution_layer_tb.main(sys.argv[1:])
