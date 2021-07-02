# import modules
import os
import shutil

import generate.modules.buff

buffer_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "buffer.hpp"

#define name        {name}
#define {NAME}_ID   {id}

#define {name}_input_t          data_t
#define {name}_output_t         data_t

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_COARSE       {coarse}
#define {NAME}_DROP_MODE    {drop_mode}

#define {NAME}_COARSE_IN    {NAME}_COARSE
#define {NAME}_COARSE_OUT   {NAME}_COARSE

// BUFFER
#define {NAME}_BUFFER_BATCH_SIZE    {batch_size}
#define {NAME}_BUFFER_ROWS          {rows_out}
#define {NAME}_BUFFER_COLS          {cols_out}
#define {NAME}_BUFFER_CHANNELS      {channels_per_module}
#define {NAME}_BUFFER_DROP_MODE     {drop_mode}


/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t({name}_input_t)  in[{NAME}_COARSE],
    stream_t({name}_input_t)  ctrl_in[{NAME}_COARSE],
    stream_t({name}_output_t) out[{NAME}_COARSE]
);

#undef name
#endif
"""

buffer_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t({name}_input_t)  in[{NAME}_COARSE],
    stream_t({name}_input_t)  ctrl_in[{NAME}_COARSE],
    stream_t({name}_output_t) out[{NAME}_COARSE]
)
{{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=ctrl_in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0


    for(unsigned int coarseIndex=0;coarseIndex<{NAME}_COARSE;coarseIndex++)
    {{
#pragma HLS UNROLL
{buff}
    }}
}}

"""

def gen_buffer_layer(name,param,src_path,header_path):

    # BUFFER MODULE INIT
    buff = generate.modules.buff.gen_buff_module(
        name+"_buffer",
        "in[coarseIndex]",
        "ctrl_in[coarseIndex]",
        "out[coarseIndex]",
        indent=8
    )

    # src
    buffer_layer_src = buffer_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        buffer_depth=max(param['buffer_depth'],2),
        buff            =buff
    )

    if param['drop_mode']:
        dm = 'true'
    else:
        dm = 'false'
    # header
    buffer_layer_header = buffer_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows'],
        cols                =param['cols'],
        channels            =param['channels'],
        channels_per_module =int(param['channels']/(param['coarse'])),
        coarse              =param['coarse'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        drop_mode           =dm
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(buffer_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(buffer_layer_header)

    return

