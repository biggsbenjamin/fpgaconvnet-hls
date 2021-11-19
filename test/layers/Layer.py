import random
import yaml
import numpy as np
import json
import sys, getopt
import os

sys.path.append('..')

class Layer:
    # Initialise Parameters
    def __init__(self,name):
        # parameters
        self.param = {}
        # module name
        self.name = name

    # Fixed-point representation # TODO: remove
    def fixed_point(data,INT_WIDTH=4,FRAC_WIDTH=12):
        return int( int(data*(2**FRAC_WIDTH))&0xFFFF )

    # Data Generation
    def gen_data(self,dim,data_range=[0,1],data_type=float):
        random.seed(0)
        # Initialise random data array
        data = np.ndarray(dim,dtype=data_type)
        # assign values
        for index,_ in np.ndenumerate(data):
            data[index] = random.uniform(data_range[0],data_range[1])
        return data

    # Stimulus Generation
    # NOTE: need to create for each testbench
    def gen_stimulus(self):
        return {}
    # NOTE: need to create for each testbench
    def gen_layer(self,src_path,header_path):
        pass
    # Header File Generation
    def gen_tb_header(self,tb_path,data_path):
        # header top
        header = """#ifndef {NAME}_PARAM_HPP_
#define {NAME}_PARAM_HPP_

#define DATA_DIR "{data_path}"

#include "{name}.hpp"

#endif
""".format(NAME=self.name.upper(),name=self.name,data_path=data_path)
        # save to file
        with open(tb_path+"/{name}_param.hpp".format(name=self.name),"w") as f:
            f.write(header)

    def main(self,argv):

        config_path = ''
        output_path = ''
        header_path = ''

        try:
            opts,args = getopt.getopt(argv,"h:c:o:t:s:")
        except getopt.GetoptError:
            sys.exit(2)
        for opt,arg in opts:
            if opt in ('-c'):
                config_path = arg
            elif opt in ('-o'):
                output_path = arg
            elif opt in ('-h'):
                header_path = arg
            elif opt in ('-s'):
                src_path = arg
            elif opt in ('-t'):
                tb_path = arg

        # load config
        with open(config_path,'r') as f:
            config = json.load(f)

        # add parameters
        for key,val in config.items():
            self.param[key] = val

        # generate signals
        data, model = self.gen_stimulus()

        # save data as .dat files
        for filename in data:
            if filename in ['weights','biases']:
                with open(os.path.join(output_path,filename+".csv"), 'w') as f:
                    print("saving:",filename)
                    f.write(data[filename])
            else:
                with open(os.path.join(output_path,filename+".dat"), 'w') as f:
                    f.write("\n".join([str(i) for i in data[filename]]))

        # create layer
        self.gen_layer(src_path,header_path)

        # add parameters to yaml file
        for key,val in self.param.items():
            model[key] = val

        # save data
        with open(output_path+'/model.json', 'w') as f:
            json.dump(model, f)

        # save header file
        self.gen_tb_header(tb_path,output_path)
