import os
import sys

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))

from fpgaconvnet_optimiser.models.modules.Fork import Fork
from Data import Data

class ForkTB(Data):
    def __init__(self):     
        Data.__init__(self,'fork')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        fork = Fork(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['kernel_size'],
            self.param['coarse']
        )
        # load model coefficients
        fork.load_coef(
            os.path.join(os.getenv("FPGACONVNET_OPTIMISER"),"fpgaconvnet_optimiser/coefficients/fork_rsc_coef.npy"),
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'],
            self.param['kernel_size']
        ])
        # data out
        data_out = fork.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),            
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : fork.get_latency(),
            'resources' : fork.rsc()
        }
        return data, model

if __name__ == '__main__':
    fork_tb = ForkTB()
    fork_tb.main(sys.argv[1:])    
 
