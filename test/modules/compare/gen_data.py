import os
import sys

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.Compare import Compare
from Data import Data

class CompareTB(Data):
    def __init__(self):
        Data.__init__(self,'compare')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        cmp = Compare(
                self.param['rows'], #not used in HLS but used in optimiser
                self.param['cols'],
                self.param['channels'],
                self.param['thr_val']
        )
        # data in
        data_max_in = self.gen_data([
            1,#self.param['rows'],
            #1, #self.param['cols'],
            #1, #self.param['channels']
        ])#generate 1 value
        data_thr_in = self.gen_data([
            1,#self.param['rows'],
            #1, #self.param['cols'],
            #1, #self.param['channels']
        ])#generate 1 value

        # data out
        data_out = cmp.functional_model(data_max_in, data_thr_in)
        # return data
        data = {
            'input_max' : data_max_in.reshape(-1).tolist(),
            'input_thr_mul' : data_thr_in.reshape(-1).tolist(),
            'thr_val'   : [cmp.threshold],
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        #TODO needs to be fixed on optimiser side
        model = {
            'latency'   : cmp.get_latency(),
            'resources' : cmp.rsc()
        }
        return data, model

if __name__ == '__main__':
    cmp_tb = CompareTB()
    cmp_tb.main(sys.argv[1:])

