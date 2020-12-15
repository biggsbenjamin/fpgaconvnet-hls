import sys
import os
import numpy as np
import csv

sys.path.append('..')
sys.path.append('../..')
sys.path.append('../../..')

from models.layers.SqueezeLayer import SqueezeLayer
import generate.layers.batch_norm
from Layer import Layer
from tools.caffe_data import CaffeData

class SqueezeLayerTB(Layer):
    def __init__(self):
        self.name = 'batch_norm_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = SqueezeLayer(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['coarse'],
            self.param['coarse']
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # batch norm coefficients
        gamma = self.gen_data([self.param['channels']])
        beta  = self.gen_data([self.param['channels']])
        eps   = np.float_power(10,-5)

        # data out
        data_out, mean, var = layer.functional_model(data_in,gamma,beta)
        data_out = np.moveaxis(data_out,0,-1)

        # get hardware coefficients
        scale = gamma / np.sqrt( var + eps )
        shift = np.divide(beta,scale) - mean
        
        # save scale 
        scale = CaffeData().transform_batch_norm_coef(
            scale,
            self.param['coarse']
        )
        with open('data/scale.csv', 'w') as f:
            writer = csv.writer(f)
            writer.writerows([scale.reshape(-1).tolist()])

        # save shift 
        shift = CaffeData().transform_batch_norm_coef(
            shift,
            self.param['coarse']
        )
        with open('data/shift.csv', 'w') as f:
            writer = csv.writer(f)
            writer.writerows([shift.reshape(-1).tolist()])

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()

        # return data
        print(layer.get_latency())
        layer.load_coef()
        return {
            'data_in'  : data_in.reshape(-1).tolist(),
            'data_out' : data_out.reshape(-1).tolist(),
            'latency'  : int(layer.get_latency()),
            'LUT'      : layer.resource()['LUT'],
            'FF'       : layer.resource()['FF'],
            'DSP'      : layer.resource()['DSP'],
            'BRAM'     : layer.resource()['BRAM']
        }

    # update layer generation
    def gen_layer(self,src_path,header_path):
        generate.layers.batch_norm.gen_batch_norm_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    batch_norm_layer_tb = SqueezeLayerTB()
    batch_norm_layer_tb.main(sys.argv[1:])
