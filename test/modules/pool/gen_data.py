import os
import sys

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))

from models.modules.Pool import Pool
from Data import Data

class PoolTB(Data):
    def __init__(self):     
        Data.__init__(self,'pool')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        if self.param['pool_type'] == 0:
            pool_type = 'max'
        if self.param['pool_type'] == 1:
            pool_type = 'avg'
        pool = Pool(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['kernel_size'],
            pool_type
        )
        # load model coefficients
        pool.load_coef(
            os.path.join(os.getenv("FPGACONVNET_OPTIMISER"),"coefficients/pool_rsc_coef.npy"),
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
        data_out = pool.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),            
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : pool.get_latency(),
            'resources' : pool.rsc()
        }
        return data, model

if __name__ == '__main__':
    pool_tb = PoolTB()
    pool_tb.main(sys.argv[1:])    
 
