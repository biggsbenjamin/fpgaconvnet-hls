import os
import sys

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))

from models.modules.Accum import Accum
from Data import Data

class AccumTB(Data):
    def __init__(self):     
        Data.__init__(self,'accum')
    
    # update stimulus generation
    def gen_stimulus(self):
        # add parameters
        self.param['channels_per_group']    = int(self.param['channels']/self.param['groups'])
        self.param['filters_per_group']     = int(self.param['filters'] /self.param['groups'])
        # Init Module
        accum = Accum(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['filters'],
            self.param['groups']
        )
        # load model coefficients
        accum.load_coef(
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/accum_static_coef.npy"),
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/accum_dynamic_coef.npy"),
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/accum_rsc_coef.npy"),
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['filters_per_group']
        ])
        # data out
        data_out = accum.functional_model(data_in)
        # return data
        data = {
            'input'  : data_in.reshape(-1).tolist(),
            'output' : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : accum.get_latency(),
            'resources' : accum.rsc()
        }
        return data, model

if __name__ == '__main__':
    accum_tb = AccumTB()
    accum_tb.main(sys.argv[1:])    
 
