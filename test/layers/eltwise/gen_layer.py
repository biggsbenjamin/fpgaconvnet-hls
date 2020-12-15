import sys
import os
import numpy as np

sys.path.append('..')
sys.path.append('../..')
sys.path.append('../../..')

from models.layers.EltwiseLayer import EltwiseLayer
import generate.layers.eltwise
from Layer import Layer

eltwise_top_template="""#include "eltwise_layer_tb.hpp"
#include "eltwise_layer.hpp"

void eltwise_layer_top(
{inputs},
  hls::stream<data_t> out[ELTWISE_LAYER_COARSE],
  int mode
)
{{
#pragma HLS DATAFLOW

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
//#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    eltwise_layer({args},out,mode);

}}
"""

eltwise_tb_header_template="""#ifndef ELTWISE_LAYER_TB_HPP_
#define ELTWISE_LAYER_TB_HPP_

#include "common.hpp"
#include "eltwise_layer.hpp"
#include "eltwise_layer_param.hpp"

void eltwise_layer_top(
{inputs},
  hls::stream<data_t> out[ELTWISE_LAYER_COARSE],
  int mode
);

#endif
"""

eltwise_tb_src_template="""#include "common_tb.hpp"
#include "eltwise_layer_tb.hpp"

int main()
{{
    int err = 0;
    std::string data_path = std::string(DATA_DIR)+"/data.yaml";

{inputs}

    hls::stream<data_t> out[ELTWISE_LAYER_COARSE];
    hls::stream<data_t> out_correct[ELTWISE_LAYER_COARSE];

    // test images
    data_t test_out[CHANNELS_3D(ELTWISE_LAYER_CHANNELS_OUT,ELTWISE_LAYER_COARSE)*ELTWISE_LAYER_ROWS*ELTWISE_LAYER_COLS][ELTWISE_LAYER_COARSE];

    // load output
    load_data<
        CHANNELS_3D(ELTWISE_LAYER_CHANNELS_OUT,ELTWISE_LAYER_COARSE)*ELTWISE_LAYER_ROWS*ELTWISE_LAYER_COLS,
        ELTWISE_LAYER_COARSE
    >(data_path,"data_out",test_out);

    getArray2Stream<
        CHANNELS_3D(ELTWISE_LAYER_CHANNELS_OUT,ELTWISE_LAYER_COARSE)*ELTWISE_LAYER_ROWS*ELTWISE_LAYER_COLS,
        ELTWISE_LAYER_COARSE,
        data_t
    >(test_out,out_correct);

{function}

    for(int i=0;i<ELTWISE_LAYER_COARSE;i++)
    {{
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i],false);
        printf("%s\\n",(err==0) ? "passed" : "failed");
    }}

    return err;
}}
"""

class EltwiseLayerTB(Layer):
    def __init__(self):
        self.name = 'eltwise_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = EltwiseLayer(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['inputs'],
            coarse_in= self.param['coarse'],
            coarse_out=self.param['coarse']
        )
        # data in
        data_in = [ self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels']
        ]) for i in range(self.param['inputs']) ]

        # data out
        data_out = layer.functional_model(data_in)
        data_out = np.moveaxis(data_out,0,-1)
        
        info = {
            'data_out' : data_out.reshape(-1).tolist(),
            'latency'  : int(layer.get_latency())
        } 

        for i in range(self.param['inputs']):
            info['data_in_{}'.format(i)] = data_in[i].reshape(-1).tolist()

        # add output dimensions
        self.param['rows_out']      = layer.rows_out()
        self.param['cols_out']      = layer.cols_out()
        self.param['channels_out']  = layer.channels_out()


        # return data
        return info

    # update layer generation
    def gen_layer(self,src_path,header_path):
    
        inputs=""
        for i in range(self.param['inputs']):
            inputs+="""
    hls::stream<data_t> in_{i}[ELTWISE_LAYER_COARSE];
    data_t test_in_{i}[CHANNELS_3D(ELTWISE_LAYER_CHANNELS,ELTWISE_LAYER_COARSE)*ELTWISE_LAYER_ROWS*ELTWISE_LAYER_COLS][ELTWISE_LAYER_COARSE];

    // load input
    load_data<
        CHANNELS_3D(ELTWISE_LAYER_CHANNELS,ELTWISE_LAYER_COARSE)*ELTWISE_LAYER_ROWS*ELTWISE_LAYER_COLS,
        ELTWISE_LAYER_COARSE
    >(data_path,"data_in_{i}",test_in_{i});

    // convert to streams
    getArray2Stream<
        CHANNELS_3D(ELTWISE_LAYER_CHANNELS,ELTWISE_LAYER_COARSE)*ELTWISE_LAYER_ROWS*ELTWISE_LAYER_COLS,
        ELTWISE_LAYER_COARSE,
        data_t
    >(test_in_{i},in_{i});
""".format(i=i)
   
        function="    eltwise_layer_top({args},out,0);".format(args=", ".join( [ "in_{}".format(i) for i in range(self.param['inputs'])] ))

        # Test Bench Generation
        eltwise_tb_src = eltwise_tb_src_template.format(
            inputs  =inputs,
            function=function
        )
        with open("tb/eltwise_layer_tb.cpp","w") as f:
            f.write(eltwise_tb_src)
        
        eltwise_tb_header = eltwise_tb_header_template.format(
            inputs=",\n".join(["  hls::stream<data_t> in_{}[ELTWISE_LAYER_COARSE]".format(i) for i in range(self.param['inputs'])])
        )
        with open("tb/eltwise_layer_tb.hpp","w") as f:
            f.write(eltwise_tb_header)
    
        eltwise_top = eltwise_top_template.format(
            inputs=",\n".join(["  hls::stream<data_t> in_{}[ELTWISE_LAYER_COARSE]".format(i) for i in range(self.param['inputs'])]),
            args=", ".join( [ "in_{}".format(i) for i in range(self.param['inputs'])] )
        )
        with open("src/eltwise_layer_top.cpp","w") as f:
            f.write(eltwise_top)

        generate.layers.eltwise.gen_eltwise_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    eltwise_layer_tb = EltwiseLayerTB()
    eltwise_layer_tb.main(sys.argv[1:])
