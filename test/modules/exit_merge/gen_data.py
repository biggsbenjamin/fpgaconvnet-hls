import os
import sys
import numpy as np
import random

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.ExitMerge import ExitMerge
from Data import Data

class ExitMergeTB(Data):
    def __init__(self):
        Data.__init__(self,'exit_merge')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        exm = ExitMerge(
                self.param['rows'],
                self.param['cols'],
                self.param['channels'],
                self.param['exits']
        )
        # data in
        print("batch_size",self.param['batch_size'])

        #TODO make this work
        #currently gets a split of correct length
        #not sure how to test ordering
        b = self.param['batch_size']
        exs = self.param['exits']
        splits=[]
        while sum(splits) != b:
            splits = random.sample(range(0,b),exs)

        data_arr=[]
        data={}
        #FIXME currently only works for 2 exits due to tb
        for ex,sp in enumerate(splits):
            print(f"Exit {ex} gets size {sp}")
            data_in = self.gen_data([
                sp,
                self.param['rows'],
                self.param['cols'],
                self.param['channels'],
            ])
            data_arr.append(data_in)
            self.param[f'ex{ex}_in'] = sp
            # return data
            data[f'input{ex}'] = data_in.reshape(-1).tolist()

        # add output to return data
        data_out = np.concatenate(data_arr)
        data['output'] = data_out.reshape(-1).tolist()
        # resource and latency model
        #TODO needs to be fixed on optimiser side
        model = {
            'latency'   : exm.latency(),
            'resources' : exm.rsc()
        }
        return data, model

if __name__ == '__main__':
    exm_tb = ExitMergeTB()
    exm_tb.main(sys.argv[1:])

