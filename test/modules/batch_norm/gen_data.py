import os
import sys

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))

from models.modules.BatchNorm import BatchNorm
from Data import Data

class BatchNormTB(Data):
    def __init__(self):     
        Data.__init__(self,'batch_norm')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        batch_norm = BatchNorm(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ]
        )
        # load model coefficients
        batch_norm.load_coef(
            os.path.join(os.getenv("FPGACONVNET_OPTIMISER"),"coefficients/batch_norm_rsc_coef.npy"),
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # scale coefficients
        scale = self.gen_data([
            self.param['channels']
        ])
        # shift coefficients
        shift = self.gen_data([
            self.param['channels']
        ])
        # data out
        data_out = batch_norm.functional_model(data_in, scale, shift)
        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),            
            'scale'  : scale.reshape(-1).tolist(),
            'shift'  : shift.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : batch_norm.get_latency(),
            'resources' : batch_norm.rsc()
        }
        return data, model

if __name__ == '__main__':
    batch_norm_tb = BatchNormTB()
    batch_norm_tb.main(sys.argv[1:])    
 
