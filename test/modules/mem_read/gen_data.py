import sys
import numpy as np
import math

sys.path.append('..')

DMA_WIDTH=64
DATA_WIDTH=16

from Data import Data

class MemReadTB(Data):
    def __init__(self):     
        Data.__init__(self,'mem_read')

    # update stimulus generation
    def gen_stimulus(self):
        # data in & out 
        data = self.gen_data([
            self.param['rows_in']*self.param['cols_in']*self.param['channels_in'],
            self.param['streams_in']
        ])
        # latency 
        latency = self.param['rows_in']*self.param['cols_in']*self.param['channels_in']
        # return data
        return {
            'data_in'  : { 'data' : data.reshape(-1).tolist() },
            'data_out' : data.reshape(-1).tolist(),
            'latency'  : latency
        }

if __name__ == '__main__':
    mem_read_tb = MemReadTB()
    mem_read_tb.main(sys.argv[1:])    
 
