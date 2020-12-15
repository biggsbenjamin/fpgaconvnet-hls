import sys
import os
import numpy as np

sys.path.append('..')
sys.path.append('../..')
sys.path.append('../../..')

from models.layers.ReLULayer import ReLULayer
import generate.layers.relu
from Layer import Layer

class ReLULayerTB(Layer):
    def __init__(self):
        self.name = 'relu_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = ReLULayer(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['coarse'],
            self.param['coarse']
        )
        layer.load_coef()
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ],data_range=[-1,1])
        # data out
        data_out = layer.functional_model(data_in)
        data_out = np.moveaxis(data_out,0,-1)

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
        generate.layers.relu.gen_relu_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    relu_layer_tb = ReLULayerTB()
    relu_layer_tb.main(sys.argv[1:])
