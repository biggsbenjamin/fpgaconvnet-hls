import sys
import os
import numpy as np

sys.path.append('..')
sys.path.append('../..')
sys.path.append('../../..')

from models.layers.ConcatLayer import ConcatLayer
import generate.layers.concat
from Layer import Layer

concat_top_template="""#include "concat_layer_tb.hpp"
#include "concat_layer.hpp"

void concat_layer_top(
{inputs},
  stream_t(data_t) out[CONCAT_LAYER_COARSE],
  int mode
)
{{
#pragma HLS DATAFLOW
#pragma HLS inline off
//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
//#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    concat_layer({args},out,mode);

}}
"""

concat_tb_header_template="""#ifndef CONCAT_LAYER_TB_HPP_
#define CONCAT_LAYER_TB_HPP_

#include "common.hpp"
#include "concat_layer.hpp"
#include "concat_layer_param.hpp"

void concat_layer_top(
{inputs},
  stream_t(data_t) out[CONCAT_LAYER_COARSE],
  int mode
);

#endif
"""

concat_tb_src_template="""#include "common_tb.hpp"
#include "concat_layer_tb.hpp"

int main()
{{
    int err = 0;
    std::string data_path = std::string(DATA_DIR)+"/data.yaml";

{inputs}

    stream_t(data_t) out[CONCAT_LAYER_COARSE];
    stream_t(data_t) out_correct[CONCAT_LAYER_COARSE];

    // test images
    static data_t test_out[CONCAT_LAYER_CHANNELS_OUT*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load output
    load_data<
        CONCAT_LAYER_CHANNELS_OUT*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_out",test_out);

    to_stream<
        CONCAT_LAYER_CHANNELS_OUT*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_out,out_correct);

{function}

    for(int i=0;i<CONCAT_LAYER_COARSE;i++)
    {{
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i]);
        printf("%s\\n",(err==0) ? "passed" : "failed");
    }}

    return err;
}}
"""

class ConcatLayerTB(Layer):
    def __init__(self):
        self.name = 'concat_layer'
        Layer.__init__(self,self.name)

    # update stimulus generation
    def gen_stimulus(self):
        # Init Module
        layer = ConcatLayer(
            [
                self.param['channels'],
                self.param['rows'],
                self.param['cols']
            ],
            self.param['inputs'],
            self.param['coarse'],
            self.param['coarse']
        )
        # data in
        data_in = [ self.gen_data([
            self.param['rows'],
            self.param['cols'],
            self.param['channels'][i]
        ]) for i in range(self.param['inputs']) ]

        # data out
        data_out = layer.functional_model(data_in)
        data_out = np.moveaxis(data_out,0,-1)
        
        # return data
        layer.load_coef()
        info =  {
            'data_out' : data_out.reshape(-1).tolist(),
            'latency'  : int(layer.get_latency()),
            'LUT'      : layer.resource()['LUT'],
            'FF'       : layer.resource()['FF'],
            'DSP'      : layer.resource()['DSP'],
            'BRAM'     : layer.resource()['BRAM']
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
    stream_t(data_t) in_{i}[CONCAT_LAYER_COARSE];
    static data_t test_in_{i}[CONCAT_LAYER_CHANNELS_{i}*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load input
    load_data<
        CONCAT_LAYER_CHANNELS_{i}*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_in_{i}",test_in_{i});

    // convert to streams
    to_stream<
        CONCAT_LAYER_CHANNELS_{i}*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_in_{i},in_{i});
""".format(i=i)
   
        function="    concat_layer_top({args},out,0);".format(args=", ".join( [ "in_{}".format(i) for i in range(self.param['inputs'])] ))

        # Test Bench Generation
        concat_tb_src = concat_tb_src_template.format(
            inputs  =inputs,
            function=function
        )
        with open("tb/concat_layer_tb.cpp","w") as f:
            f.write(concat_tb_src)
        
        concat_tb_header = concat_tb_header_template.format(
            inputs=",\n".join(["  stream_t(data_t) in_{}[CONCAT_LAYER_COARSE]".format(i) for i in range(self.param['inputs'])])
        )
        with open("tb/concat_layer_tb.hpp","w") as f:
            f.write(concat_tb_header)
    
        concat_top = concat_top_template.format(
            inputs=",\n".join(["  stream_t(data_t) in_{}[CONCAT_LAYER_COARSE]".format(i) for i in range(self.param['inputs'])]),
            args=", ".join( [ "in_{}".format(i) for i in range(self.param['inputs'])] )
        )
        with open("src/concat_layer_top.cpp","w") as f:
            f.write(concat_top)

        generate.layers.concat.gen_concat_layer(
            self.name,
            self.param,
            os.path.join(src_path,'{}.cpp'.format(self.name)),
            os.path.join(header_path,'{}.hpp'.format(self.name))
        )

if __name__ == '__main__':
    concat_layer_tb = ConcatLayerTB()
    concat_layer_tb.main(sys.argv[1:])
