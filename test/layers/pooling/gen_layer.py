import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.PoolingLayer import PoolingLayer
import generate.layers.pooling
from Layer import Layer

class PoolingLayerTB(Layer):
    def __init__(self):
        self.name = 'pooling_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = PoolingLayer(
            [
                self.param['channels_in'],
                self.param['rows_in'],
                self.param['cols_in']
            ],
            self.param['pool_type'],
            self.param['kernel_size'],
            self.param['stride'],
            self.param['pad'],
            self.param['coarse'],
            self.param['coarse']
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
        # data out
        data_out = layer.functional_model(copy.copy(data_in))[0]
        data_out = np.moveaxis(data_out,0,-1)

        print(data_in.shape)
        print(data_out.shape)

        print(data_in[0,0])
        print(data_in[0,1])
        print(data_in[1,0])
        print(data_in[1,1])
        print(data_out[0,0])

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
        generate.layers.pooling.gen_pooling_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    pooling_layer_tb = PoolingLayerTB()
    pooling_layer_tb.main(sys.argv[1:])
