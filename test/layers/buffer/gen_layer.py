import sys
import os
import numpy as np
import csv
import copy

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from fpgaconvnet_optimiser.models.layers.BufferLayer import BufferLayer
import generate.layers.buffer
from Layer import Layer

class BufferLayerTB(Layer):
    def __init__(self):
        self.name = 'buffer_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = BufferLayer(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['coarse'],
            self.param['ctrledge'],
            self.param['drop_mode'],
        )
        layer.load_coef()

        # data in
        data_in = self.gen_data([
            self.param['batch_size'],
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # data out
        ctrl_in = self.gen_ctrl(self.param['batch_size'])
        data_out = layer.functional_model(copy.copy(data_in), ctrl_in, self.param['batch_size'])
        #ctrl_in_COARSE = np.repeat(ctrl_in, self.param['coarse']) #dup ctrl signals for hw
        print(ctrl_in)

        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'ctrl_in'   : ctrl_in.reshape(-1).tolist(), #ctrl_in_COARSE.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : layer.get_latency(),
            'resources' : layer.resource()
        }
        return data, model

    # update layer generation
    def gen_layer(self,src_path,header_path):
        generate.layers.buffer.gen_buffer_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    buffer_layer_tb = BufferLayerTB()
    buffer_layer_tb.main(sys.argv[1:])
