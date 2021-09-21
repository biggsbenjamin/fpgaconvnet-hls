# import modules
import os
import shutil
import generate.modules.relu

relu_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "relu.hpp"

#define name        {name}
#define {NAME}_ID   {id}

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_COARSE       {coarse}

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_RELU_BATCH_SIZE   {batch_size}
#define {NAME}_RELU_ROWS         {rows}
#define {NAME}_RELU_COLS         {cols}
#define {NAME}_RELU_CHANNELS     {channels_per_module}
typedef ap_fixed<{datatype},{datatype}/2,AP_RND, AP_SAT>   {NAME}_RELU_t;
/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t({NAME}_RELU_t) in[{NAME}_COARSE],
    stream_t({NAME}_RELU_t) out[{NAME}_COARSE],
    int mode
);

#undef name
#endif
"""

relu_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t({NAME}_RELU_t) in[{NAME}_COARSE],
    stream_t({NAME}_RELU_t) out[{NAME}_COARSE],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW

    for(int coarseIndex=0;coarseIndex<{NAME}_COARSE;coarseIndex++)
    {{
#pragma HLS unroll

{relu}

    }}
}}

"""

def gen_relu_layer(name,param,src_path,header_path):

    # RELU MODULE INIT
    relu_param = {
        'input_t'       : "{name}_input_t".format(name=name),
        'output_t'      : "{name}_output_t".format(name=name)
    }
    relu = generate.modules.relu.gen_relu_module(
        name+"_relu",
        "in[coarseIndex]",
        "out[coarseIndex]",
        indent=8
    )

    # src
    relu_layer_src = relu_layer_template_src.format(
        name  =name,
        NAME  =name.upper(),
        buffer_depth=max(param['buffer_depth'],2),
        relu  =relu
    )

    # header
    print(param)
    relu_layer_header = relu_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows_in'],
        cols                =param['cols_in'],
        channels            =param['channels_in'],
        channels_per_module =int(param['channels_in']/param['coarse_in']),
        coarse              =param['coarse_in'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        datatype            =param['data_width']
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(relu_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(relu_layer_header)

    return
