import sys
import os
import numpy as np
import csv
import copy 

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from models.layers.ConvolutionLayer import ConvolutionLayer
import generate.layers.convolution
from Layer import Layer
from tools.caffe_data import CaffeData
from tools.array_init import array_init

class ConvolutionLayerTB(Layer):
    def __init__(self):
        self.name = 'convolution_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = ConvolutionLayer(
            [
                self.param['channels_in'],
                self.param['rows_in'],
                self.param['cols_in']
            ],
            self.param['filters'],
            self.param['kernel_size'],
            self.param['stride'],
            self.param['groups'],
            self.param['pad'],
            self.param['coarse_in'],
            self.param['coarse_out'],
            self.param['fine']
        )
        layer.load_coef()
        
        self.param['pad_top']       = layer.pad_top
        self.param['pad_right']     = layer.pad_right
        self.param['pad_bottom']    = layer.pad_bottom
        self.param['pad_left']      = layer.pad_left

        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])
        # weights
        weights = self.gen_data([
            self.param['filters'],
            int(self.param['channels_in']/self.param['groups']),
            self.param['kernel_size'],
            self.param['kernel_size']
        ])
        bias     = np.zeros(self.param['filters'])
        # data out
        print(data_in.shape)
        data_out = layer.functional_model(copy.copy(data_in),weights,bias)[0]
        data_out = np.moveaxis(data_out,0,-1)
        print(data_out.shape)

        # save weights
        weights = CaffeData()._transform_weights(
            weights,
            1,
            self.param['coarse_in'],
            self.param['coarse_out']
        )
        with open('data/weights.csv', 'w') as f:
            f.write(array_init(weights[0]))

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : layer.get_latency(),
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
