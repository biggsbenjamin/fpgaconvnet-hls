import os
import sys
import numpy as np

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.Buffer import Buffer
from Data import Data

class BufferTB(Data):
    def __init__(self):
        Data.__init__(self,'buffer')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        buff = Buffer(
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['ctrledge'],
            self.param['drop_mode'],
        )
        # data in
        data_in = self.gen_data([
            self.param['batch_size'],
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
        ])
        ctrl_in = self.gen_ctrl(self.param['batch_size'])

        # data out
        data_out = []
        for b,ctrl in zip(data_in, ctrl_in):
            b_out = buff.functional_model(b, ctrl)
            if b_out is not None:
                data_out.append(b_out)
        data_out = np.asarray(data_out)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),
            'ctrl_in'   : ctrl_in.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : buff.get_latency(),
            'resources' : buff.rsc()
        }
        return data, model

if __name__ == '__main__':
    buffer_tb = BufferTB()
    buffer_tb.main(sys.argv[1:])

