import os
import sys

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.Squeeze import Squeeze
from Data import Data

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
        # load model coefficients
        squeeze.load_coef(
            os.path.join(os.getenv("FPGACONVNET_OPTIMISER"),"fpgaconvnet_optimiser/coefficients/squeeze_rsc_coef.npy"),
        )
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
 
