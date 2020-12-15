import sys
sys.path.append('../../..')
sys.path.append('..')

from models.modules.Auxiliary import Split
from Data import Data

class SplitTB(Data):
    def __init__(self):     
        Data.__init__(self,'split')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        split = Split(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['outputs'],
            self.param['coarse']
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            int(self.param['channels']/self.param['coarse']),
            self.param['coarse']
        ])
        # data out
        data_out = split.functional_model(data_in)
        # return data
        return {
            'data_in'  : data_in.reshape(-1).tolist(),
            'data_out' : data_out.reshape(-1).tolist(),
            'latency'  : split.get_latency()
        }

if __name__ == '__main__':
    split_tb = SplitTB()
    split_tb.main(sys.argv[1:])    
 
