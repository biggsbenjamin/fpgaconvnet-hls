import os
import sys

sys.path.append('..')
sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))

from models.modules.SlidingWindow import SlidingWindow
from Data import Data

class SlidingWindowTB(Data):
    def __init__(self):     
        Data.__init__(self,'sliding_window')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        sliding_window = SlidingWindow(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['kernel_size'],
            self.param['stride'],
            self.param['pad_top'],
            self.param['pad_right'],
            self.param['pad_bottom'],
            self.param['pad_left']
        )
        # load model coefficients
        sliding_window.load_coef(
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/sliding_window_static_coef.npy"),
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/sliding_window_dynamic_coef.npy"),
            os.path.join(os.getenv("FPGACONVNET_ROOT"),"data/model_coefficients/sliding_window_rsc_coef.npy"),
        )
        # output dimensions
        self.param['rows_out'] = sliding_window.rows_out()
        self.param['cols_out'] = sliding_window.cols_out()

        # data in
        data_in = self.gen_data([
            self.param['batch_size'],
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ])
        # data out
        data_out = sliding_window.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),            
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : sliding_window.get_latency(),
            'resources' : sliding_window.rsc()
        }
        return data, model
    
if __name__ == '__main__':
    sliding_window_tb = SlidingWindowTB()
    sliding_window_tb.main(sys.argv[1:])    
 
