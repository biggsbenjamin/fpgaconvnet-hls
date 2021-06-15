import os
import sys

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.ReduceMax import ReduceMax
from Data import Data

class ReduceMaxTB(Data):
    def __init__(self):
        Data.__init__(self,'reducemax')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        redmx = ReduceMax(
                self.param['rows'],
                self.param['cols'],
                self.param['channels']
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # data out
        data_out = redmx.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        #TODO needs to be fixed on optimiser side
        model = {
            'latency'   : redmx.get_latency(),
            'resources' : redmx.rsc()
        }
        return data, model

if __name__ == '__main__':
    redmx_tb = ReduceMaxTB()
    redmx_tb.main(sys.argv[1:])

