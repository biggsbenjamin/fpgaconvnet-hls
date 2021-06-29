import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.SplitLayer import SplitLayer
import generate.layers.split
from Layer import Layer

class SplitLayerTB(Layer):
    def __init__(self):
        self.name = 'split_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = SplitLayer(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['coarse'],
            self.param['ports_out']
        )
        layer.load_coef()

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # data out
        data_out = layer.functional_model(copy.copy(data_in))
        #data_out = np.moveaxis(data_out,0,-1)

        # add output dimensions
        self.param['rows_out']      = layer.rows_out(0)
        self.param['cols_out']      = layer.cols_out(0)
        self.param['channels_out']  = layer.channels_out(0)

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
        generate.layers.split.gen_split_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    split_layer_tb = SplitLayerTB()
    split_layer_tb.main(sys.argv[1:])
