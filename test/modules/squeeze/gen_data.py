import os
import sys
import math

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.Squeeze import Squeeze
from Data import Data

def lcm(a, b):
    return abs(a*b) // math.gcd(a, b)

class SqueezeTB(Data):
    def __init__(self):
        Data.__init__(self,'squeeze')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        squeeze = Squeeze(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['coarse_out'],
            self.param['coarse_in']
        )

        # add parameters
        self.param['data_width'] = squeeze.data_width
        self.param['data_int_width'] = squeeze.data_width//2

        self.param['buffer_size'] = lcm(self.param['coarse_in'], self.param['coarse_out'])

        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            int(self.param['channels']/self.param['coarse_out']),
            self.param['coarse_out']
        ])
        # data out
        data_out = squeeze.functional_model(data_in)
        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : squeeze.get_latency(),
            'resources' : squeeze.rsc()
        }
        return data, model

if __name__ == '__main__':
    squeeze_tb = SqueezeTB()
    squeeze_tb.main(sys.argv[1:])

