# import modules
import os
import shutil 
import generate.modules.relu

relu_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

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

#define MODULE_NAME {NAME}_RELU
#define {NAME}_RELU_BATCH_SIZE   {batch_size}
#define {NAME}_RELU_ROWS         {rows}
#define {NAME}_RELU_COLS         {cols}
#define {NAME}_RELU_CHANNELS     {channels_per_module}
#include "{name}_relu.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t(data_t) in[{NAME}_COARSE],
    stream_t(data_t) out[{NAME}_COARSE],
    int mode
);

#undef name
#endif
"""

relu_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t(data_t) in[{NAME}_COARSE],
    stream_t(data_t) out[{NAME}_COARSE],
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
        name,
        relu_param,
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
        channels_out        =param['channels_out']
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(relu_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(relu_layer_header)

    # save modules 
    header_path = os.path.dirname(os.path.abspath(header_path))
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/relu.hpp') , os.path.join(header_path,"{name}_relu.hpp".format(name=name)) )
    
    #src_path = os.path.dirname(os.path.abspath(src_path))
    #shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'src/relu.cpp') , os.path.join(src_path ,"{name}_relu.cpp".format(name=name)) )

    return
