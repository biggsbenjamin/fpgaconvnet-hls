import os
import sys

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))

from models.modules.Conv import Conv
from Data import Data

class ConvTB(Data):
    def __init__(self):     
        Data.__init__(self,'conv')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        self.param['interval'] = int((self.param['kernel_size']*self.param['kernel_size'])/self.param['fine'])
        conv = Conv(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['filters'],
            self.param['fine'],
            self.param['kernel_size'],
            self.param['group']
        )
        # load model coefficients
        conv.load_coef(
            os.path.join(os.getenv("FPGACONVNET_OPTIMISER"),"coefficients/conv_rsc_coef.npy"),
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'],
            self.param['kernel_size'],
            self.param['kernel_size']
        ])
        # weights 
        weights = self.gen_data([
            self.param['channels'],
            int(self.param['filters']/self.param['group']),
            self.param['kernel_size'],
            self.param['kernel_size']
        ])
        # data out
        data_out = conv.functional_model(data_in, weights)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),            
            'weights'   : weights.reshape(-1).tolist(),
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : conv.get_latency(),
            'resources' : conv.rsc()
        }
        return data, model

if __name__ == '__main__':
    conv_tb = ConvTB()
    conv_tb.main(sys.argv[1:])    
 
