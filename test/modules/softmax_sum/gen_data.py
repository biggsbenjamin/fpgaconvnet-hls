import os
import sys

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.SoftMaxSum import SoftMaxSum
from Data import Data

class SoftMaxSumTB(Data):
    def __init__(self):
        Data.__init__(self,'softmax_sum')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        sm_sum = SoftMaxSum(
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
        data_out = sm_sum.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        #TODO needs to be fixed on optimiser side
        model = {
            'latency'   : sm_sum.get_latency(),
            'resources' : sm_sum.rsc()
        }
        return data, model

if __name__ == '__main__':
    sm_sum_tb = SoftMaxSumTB()
    sm_sum_tb.main(sys.argv[1:])

