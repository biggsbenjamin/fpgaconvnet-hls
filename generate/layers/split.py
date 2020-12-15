# import modules
import os
import shutil

import generate.modules.split

split_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#define name      {name}
#define {NAME}_ID {id}

#define {name}_input_t          data_t
#define {name}_output_t         data_t

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_COARSE       {coarse}
#define {NAME}_OUTPUTS      {outputs}

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

#define {NAME}_ROWS_OUT     {rows_out} 
#define {NAME}_COLS_OUT     {cols_out} 
#define {NAME}_CHANNELS_OUT {channels_out} 

// SLIDING WINDOW
#define MODULE_NAME {NAME}_SPLIT
#define {NAME}_SPLIT_BATCH_SIZE   {batch_size}
#define {NAME}_SPLIT_ROWS         {rows}
#define {NAME}_SPLIT_COLS         {cols}
#define {NAME}_SPLIT_CHANNELS     {channels}
#define {NAME}_SPLIT_COARSE       {coarse}
#define {NAME}_SPLIT_OUTPUTS      {outputs}
#include "{name}_auxiliary.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t({name}_input_t)  in[{NAME}_COARSE],
    stream_t({name}_output_t) out[{NAME}_OUTPUTS][{NAME}_COARSE],
    int mode
);

#undef name
#endif
"""

split_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t({name}_input_t)  in[{NAME}_COARSE],
    stream_t({name}_output_t) out[{NAME}_OUTPUTS][{NAME}_COARSE],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW

{split}

}}

"""

def gen_split_layer(name,param,src_path,header_path):

    # RELU MODULE INIT
    split_param = {
        'input_t'       : "{name}_input_t".format(name=name),
        'output_t'      : "{name}_output_t".format(name=name)
    }
    split = generate.modules.split.gen_split_module(
        name,
        split_param,
        "in",
        "out",
        indent=4
    )

    # src
    split_layer_src = split_layer_template_src.format(
        name  =name,
        NAME  =name.upper(),
        split  =split  
    )

    # header
    split_layer_header = split_layer_template_header.format(
        name            =name,
        NAME            =name.upper(),
        id              =0, # param['id'],
        batch_size      =param['batch_size'],
        rows            =param['rows'],
        cols            =param['cols'],
        channels        =param['channels'],
        coarse          =param['coarse'],
        outputs         =param['outputs'],
        rows_out        =param['rows_out'],
        cols_out        =param['cols_out'],
        channels_out    =param['channels_out']
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(split_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(split_layer_header)

    # save modules 
    header_path = os.path.dirname(os.path.abspath(header_path))
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/auxiliary.hpp') , os.path.join(header_path,"{name}_auxiliary.hpp".format(name=name)) )

    return
