import os
import sys

sys.path.append('..')

from fpgaconvnet_optimiser.models.modules.Glue import Glue
from Data import Data

class GlueTB(Data):
    def __init__(self):     
        Data.__init__(self,'glue')

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        self.param['acc'] = "+".join([ "in[{i}][out_index].read()".format(i=i) for i in range(self.param['coarse_in']) ])
        glue = Glue(
            [
                1,
                self.param['rows'],
                self.param['cols']
            ],
            self.param['filters'],
            self.param['coarse_in'],
            self.param['coarse_out']
        )
        # load model coefficients
        glue.load_coef(
            os.path.join(os.getenv("FPGACONVNET_OPTIMISER"),"fpgaconvnet_optimiser/coefficients/glue_rsc_coef.npy"),
        )
        # data in
        data_in = self.gen_data([
            self.param['rows'],
            self.param['cols'],
            int(self.param['filters']/self.param['coarse_out']),
            self.param['coarse_in'],
            self.param['coarse_out']
        ])
        # data out
        data_out = glue.functional_model(data_in)
        # return data
        data = {
            'input'     : data_in.reshape(-1).tolist(),            
            'output'    : data_out.reshape(-1).tolist()
        }
        # resource and latency model
        model = {
            'latency'   : glue.get_latency(),
            'resources' : glue.rsc()
        }
        return data, model

if __name__ == '__main__':
    glue_tb = GlueTB()
    glue_tb.main(sys.argv[1:])    
 
