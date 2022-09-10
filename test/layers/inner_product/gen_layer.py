import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.InnerProductLayer import InnerProductLayer
import generate.layers.inner_product
from Layer import Layer
from tools.onnx_data import ONNXData
from tools.array_init import array_init

class InnerProductLayerTB(Layer):
    def __init__(self):
        self.name = 'inner_product_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = InnerProductLayer(
            self.param['filters'],
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            coarse_in=self.param['coarse_in'],
            coarse_out=self.param['coarse_out'],
            input_width=self.param["input_width"],
            output_width=self.param["output_width"],
            acc_width=self.param["acc_width"],
            weight_width=self.param["weight_width"],
            biases_width=self.param["biases_width"]

        )

        # data in
        data_in = self.gen_data([
            self.param['batch_size'],
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])
        #data_in = np.repeat(data_in[np.newaxis,...], self.param['batch_size'], axis=0)

        # weights
        weights = self.gen_data([
            self.param['filters'],
            self.param['rows_in']*self.param['cols_in']*self.param['channels_in']
        ],
        data_range=[-1,1]) #more realistic to have +ve and -ve weights
        # might also help truncation issue for large layers

        # biases
        biases = np.zeros(self.param['filters'])
        if self.param['has_bias'] == 1:
            biases = self.gen_data([
                self.param['filters']
            ],data_range=[-1,1])

        # data out
        data_out = layer.functional_model(copy.copy(data_in),weights,biases)
        # move channels to last dimension
        data_out = np.moveaxis(data_out,1,-1)

        # reshape weights - I guess to match conv weight format?
        weights = np.reshape(weights,
                (self.param['filters'],
            self.param['channels_in'],
            self.param['rows_in'],
            self.param['cols_in'],
            1,1))
        weights = np.rollaxis(weights,1,4)
        weights = np.reshape(weights,
                (self.param['filters'],
                    self.param['cols_in']*self.param['rows_in']*self.param['channels_in'],
                    1,1))

        # save weights
        weights = ONNXData._transform_weights(
            weights,
            1,
            self.param['coarse_in'],
            self.param['coarse_out'],
            1,
            1
        )

        # save biases
        biases = ONNXData._transform_biases(
            biases,
            self.param['filters'],
            self.param['coarse_out']
        )

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist(),
            'weights': array_init(weights[0]),
            'biases' : array_init(biases[0])
        }

        # resource and latency model
        model = {
            'latency'   : layer.latency(),
            'resources' : layer.resource()
        }

        return data, model

    # update layer generation
    def gen_layer(self,src_path,header_path):
        generate.layers.inner_product.gen_inner_product_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    inner_product_layer_tb = InnerProductLayerTB()
    inner_product_layer_tb.main(sys.argv[1:])
