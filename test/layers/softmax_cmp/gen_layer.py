import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.SoftMaxCmpLayer import SoftMaxCmpLayer
import generate.layers.softmax_cmp
from Layer import Layer

class SoftMaxCmpLayerTB(Layer):
    def __init__(self):
        self.name = 'softmax_cmp_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = SoftMaxCmpLayer(
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in'],
            self.param['coarse_in'],
            self.param['coarse_out'],
            self.param['ctrl_edges'],
            self.param['threshold'],
            self.param['cond_type'],
        )
        layer.load_coef()

        # data in
        data_in = self.gen_data([
            self.param['rows_in'],
            self.param['cols_in'],
            self.param['channels_in']
        ])
        # data out TODO fix batch size used in layer functional model
        #data_out = layer.functional_model(copy.copy(data_in))[0]
        #data_out = np.moveaxis(data_out,0,-1)
        #print(data_in)
        data_out = layer.functional_model(copy.copy(data_in))

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
        generate.layers.softmax_cmp.gen_softmax_cmp_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    softmax_cmp_layer_tb = SoftMaxCmpLayerTB()
    softmax_cmp_layer_tb.main(sys.argv[1:])
