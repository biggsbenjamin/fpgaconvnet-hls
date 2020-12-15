import os
import sys
sys.path.append('../../..')
sys.path.append('..')

from models.modules.ReLU import ReLU
from Data import Data

class ReLUTB(Data):
    def __init__(self):     
        Data.__init__(self,'relu')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        relu = ReLU(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ]
        )
        # load model coefficients
        relu.load_coef(
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/relu_static_coef.npy"),
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/relu_dynamic_coef.npy"),
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/relu_rsc_coef.npy"),
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # data out
        data_out = relu.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),            
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : relu.get_latency(),
            'resources' : relu.rsc()
        }
        return data, model

if __name__ == '__main__':
    relu_tb = ReLUTB()
    relu_tb.main(sys.argv[1:])    
 
