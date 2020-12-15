# import modules
import os
import shutil 
import generate.modules.batch_norm

batch_norm_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#define name        {name}
#define {NAME}_ID {id}

#define {name}_input_t          data_t
#define {name}_scale_t          data_t
#define {name}_shift_t          data_t
#define {name}_output_t         data_t

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

#define MODULE_NAME {NAME}_BATCH_NORM
#define {NAME}_BATCH_NORM_BATCH_SIZE   {batch_size}
#define {NAME}_BATCH_NORM_ROWS         {rows}
#define {NAME}_BATCH_NORM_COLS         {cols}
#define {NAME}_BATCH_NORM_CHANNELS     {channels_per_module}
#include "{name}_batch_norm.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t({name}_input_t)  in[{NAME}_COARSE],
    const {name}_scale_t scale[{NAME}_COARSE][CHANNELS_3D({NAME}_CHANNELS,{NAME}_COARSE)],
    const {name}_shift_t shift[{NAME}_COARSE][CHANNELS_3D({NAME}_CHANNELS,{NAME}_COARSE)],
    stream_t({name}_output_t) out[{NAME}_COARSE],
    int mode
);

#undef name
#endif
"""

batch_norm_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t({name}_input_t)  in[{NAME}_COARSE],
    const {name}_scale_t scale[{NAME}_COARSE][CHANNELS_3D({NAME}_CHANNELS,{NAME}_COARSE)],
    const {name}_shift_t shift[{NAME}_COARSE][CHANNELS_3D({NAME}_CHANNELS,{NAME}_COARSE)],
    stream_t({name}_output_t) out[{NAME}_COARSE],
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
#pragma HLS UNROLL

{batch_norm}
    }}
}}

"""

def gen_batch_norm_layer(name,param,src_path,header_path):

    # BATCH NORM MODULE INIT
    batch_norm_param = {
        'input_t'       : "{name}_input_t".format(name=name),
        'scale_t'       : "{name}_scale_t".format(name=name),
        'shift_t'       : "{name}_shift_t".format(name=name),
        'output_t'      : "{name}_output_t".format(name=name)
    }
    batch_norm = generate.modules.batch_norm.gen_batch_norm_module(
        name,
        batch_norm_param,
        "in[coarseIndex]",
        "scale[coarseIndex]",
        "shift[coarseIndex]",
        "out[coarseIndex]",
        indent=8
    )

    # src
    batch_norm_layer_src = batch_norm_layer_template_src.format(
        name  =name,
        NAME  =name.upper(),
        buffer_depth=max(param['buffer_depth'],2),
        batch_norm  =batch_norm
    )

    # header
    batch_norm_layer_header = batch_norm_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows'],
        cols                =param['cols'],
        channels            =param['channels'],
        channels_per_module =int(param['channels']/param['coarse']),
        coarse              =param['coarse'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out']
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(batch_norm_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(batch_norm_layer_header)

    # save modules 
    header_path = os.path.dirname(os.path.abspath(header_path))
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/batch_norm.hpp') , os.path.join(header_path,"{name}_batch_norm.hpp".format(name=name)) )

    return
