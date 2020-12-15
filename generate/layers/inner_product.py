# import modules
import os
import shutil 

import generate.modules.sliding_window
import generate.modules.fork
import generate.modules.conv
import generate.modules.accum
import generate.modules.glue

inner_product_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#define name        {name}
#define {NAME}_ID   {id}

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_FILTERS      {filters}
#define {NAME}_COARSE_IN    {coarse_in}
#define {NAME}_COARSE_OUT   {coarse_out}
#define {NAME}_KERNEL_SIZE  1

// coefficients
#define {NAME}_WEIGHTS {NAME}_ROWS*{NAME}_COLS*{NAME}_CHANNELS*{NAME}_FILTERS

// dimensions out
#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}


// FORK
#define MODULE_NAME {NAME}_FORK
#define {NAME}_FORK_BATCH_SIZE  {batch_size}
#define {NAME}_FORK_ROWS        1
#define {NAME}_FORK_COLS        1
#define {NAME}_FORK_CHANNELS    {channels_per_module}
#define {NAME}_FORK_COARSE      {coarse_out}
#define {NAME}_FORK_KERNEL_SIZE 1
#include "{name}_fork.hpp"
#undef MODULE_NAME

// CONV
#define MODULE_NAME {NAME}_CONV
#define {NAME}_CONV_BATCH_SIZE  {batch_size}
#define {NAME}_CONV_ROWS        1
#define {NAME}_CONV_COLS        1
#define {NAME}_CONV_CHANNELS    {channels_per_module}
#define {NAME}_CONV_FILTERS     {filters_per_module}
#define {NAME}_CONV_KERNEL_SIZE 1
#define {NAME}_CONV_FINE        1
#define {NAME}_CONV_INTERVAL    1
#include "{name}_conv.hpp"
#undef MODULE_NAME

// ACCUM
#define MODULE_NAME {NAME}_ACCUM
#define {NAME}_ACCUM_BATCH_SIZE         {batch_size}
#define {NAME}_ACCUM_ROWS               1
#define {NAME}_ACCUM_COLS               1
#define {NAME}_ACCUM_GROUPS             1
#define {NAME}_ACCUM_CHANNELS           {channels_per_module}
#define {NAME}_ACCUM_FILTERS            {filters_per_module}
#define {NAME}_ACCUM_CHANNELS_PER_GROUP {channels_per_module}
#define {NAME}_ACCUM_FILTERS_PER_GROUP  {filters_per_module}
#include "{name}_accum.hpp"
#undef MODULE_NAME

// GLUE
#define MODULE_NAME {NAME}_GLUE
#define {NAME}_GLUE_BATCH_SIZE  {batch_size}
#define {NAME}_GLUE_ROWS        1
#define {NAME}_GLUE_COLS        1
#define {NAME}_GLUE_FILTERS     {channels_out}
#define {NAME}_GLUE_COARSE_IN   {coarse_in}
#define {NAME}_GLUE_COARSE_OUT  {coarse_out}
#define {NAME}_GLUE_ACC         {glue_acc}
#include "{name}_glue.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void {name}(
    const weight_t weights[{NAME}_COARSE_IN][{NAME}_COARSE_OUT][CHANNELS_3D({NAME}_ROWS*{NAME}_COLS*{NAME}_CHANNELS,{NAME}_COARSE_IN)*CHANNELS_3D({NAME}_FILTERS,{NAME}_COARSE_OUT)],
    stream_t(data_t) in[{NAME}_COARSE_IN],
    stream_t(data_t) out[{NAME}_COARSE_OUT],
    int mode
);

#endif
#undef name
"""

inner_product_layer_template_src = """#include "{name}.hpp"

void {name}(
    const weight_t weights[{NAME}_COARSE_IN][{NAME}_COARSE_OUT][CHANNELS_3D({NAME}_ROWS*{NAME}_COLS*{NAME}_CHANNELS,{NAME}_COARSE_IN)*CHANNELS_3D({NAME}_FILTERS,{NAME}_COARSE_OUT)],
    stream_t(data_t) in[{NAME}_COARSE_IN],
    stream_t(data_t) out[{NAME}_COARSE_OUT],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW

    {streams}

    {name}_coarse_in_loop: for(int i=0;i<{NAME}_COARSE_IN;i++) {{
        #pragma HLS UNROLL 

        {fork}

        {name}_coarse_out_loop: for(int j=0;j<{NAME}_COARSE_OUT;j++) {{
            #pragma HLS UNROLL

            {conv}
            {accum}

        }}
    }}

    {glue}

}}

"""

def gen_inner_product_layer(name,param,src_path,header_path):

    # get sliding window type
    single_channel  = True if param['channels_in'] == 1 else False

    inputs = {
        'fork'          : "in[i]",
        'conv'          : "fork_out[i][j]",
        'accum'         : "conv_out[i][j]",
        'glue'          : "accum_out"
    }

    outputs = {
        'fork'          : "fork_out[i]",
        'conv'          : "conv_out[i][j]",
        'accum'         : "accum_out[i][j]",
        'glue'          : "out"
    }

    """
    if param['coarse_in'] == 1:
        outputs['accum'] = "out[j]"
        inputs.pop('glue',None)
        outputs.pop('glue',None)
    """

    if single_channel:
        """
        outputs['conv'] = "out[j]"
        inputs.pop('accum',None)
        outputs.pop('accum',None)
        inputs.pop('glue',None)
        outputs.pop('glue',None)
        """
        inputs['glue'] = "conv_out"
        inputs.pop('accum',None)
        outputs.pop('accum',None)    
 
    if (param['channels_in'])/(param['coarse_in']) == 1:
        inputs['glue'] = "conv_out"
        inputs.pop('accum',None)
        outputs.pop('accum',None)    
        
    streams = ""

    # FORK MODULE INIT
    if 'fork' in inputs:
        streams += """
    stream_t(data_t) fork_out[{NAME}_COARSE_IN][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=fork_out
    #pragma HLS ARRAY_PARTITION variable=fork_out complete dim=0
        """.format(NAME=name.upper(),name=name)
        fork_param = {
            'input_t'       : "{name}_input_t".format(name=name),
            'output_t'      : "{name}_fork_t".format(name=name)
        }
        fork = generate.modules.fork.gen_fork_module(
            name,
            fork_param,
            inputs['fork'],
            outputs['fork'],
            indent=8,
            single_stream=True
        )
    else:
        fork = ''

    # CONV MODULE INIT
    if 'conv' in inputs:
        streams += """
    stream_t(acc_t) conv_out[{NAME}_COARSE_IN][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=conv_out
    #pragma HLS ARRAY_PARTITION variable=conv_out complete dim=0
        """.format(NAME=name.upper(),name=name)
        conv_param = {
            'input_t'       : "{name}_fork_t".format(name=name),
            'weight_t'      : "{name}_weight_t".format(name=name),
            'output_t'      : "{name}_conv_t".format(name=name)
        }
        conv = generate.modules.conv.gen_conv_module(
            name,
            conv_param,
            inputs['conv'],
            "weights[i][j]",
            outputs['conv'],
            indent=12,
            single_stream=True
        )
    else:
        conv = ''

    # ACCUM MODULE INIT
    if 'accum' in inputs:
        streams += """
    stream_t(acc_t) accum_out[{NAME}_COARSE_IN][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=accum_out
    #pragma HLS ARRAY_PARTITION variable=accum_out complete dim=0
        """.format(NAME=name.upper(),name=name)
        accum_param = {
            'input_t'       : "{name}_conv_t".format(name=name),
            'output_t'      : "{name}_accum_t".format(name=name)
        }
        accum = generate.modules.accum.gen_accum_module(
            name,
            accum_param,
            inputs['accum'],
            outputs['accum'],
            indent=12,
            grouped=False
        )
    else:
        accum = ''

    # GLUE MODULE INIT
    if 'glue' in inputs:
        glue_param = {
            'input_t'       : "{name}_accum_t".format(name=name),
            'output_t'      : "{name}_output_t".format(name=name)
        }
        glue = generate.modules.glue.gen_glue_module(
            name,
            glue_param,
            inputs['glue'],
            outputs['glue'],
            indent=4
        )
    else:
        glue = ''

    # src
    inner_product_layer_src = inner_product_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        buffer_depth    =max(param['buffer_depth'],2),
        streams         =streams,
        fork            =fork,
        conv            =conv,
        accum           =accum,
        glue            =glue
    )

    # glue accumulation line
    glue_acc = "+".join([ "in[{i}][out_index].read()".format(i=i) for i in range(param['coarse_in']) ])

    # header
    inner_product_layer_header = inner_product_layer_template_header.format(
        name            =name,
        NAME            =name.upper(),
        id              =0, # param['id'],
        batch_size      =param['batch_size'],
        rows            =param['rows_in'],
        cols            =param['cols_in'],
        channels        =param['channels_in'],
        rows_out        =param['rows_out'],
        cols_out        =param['cols_out'],
        channels_out    =param['channels_out'],
        filters         =param['filters'],
        channels_per_module =int(param['channels_in']*param['rows_in']*param['cols_in']/param['coarse_in']), 
        filters_per_module  =int(param['filters']/param['coarse_out']), 
        coarse_in           =param['coarse_in'],
        coarse_out          =param['coarse_out'],
        glue_acc            =glue_acc
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(inner_product_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(inner_product_layer_header)

    # save modules 
    header_path = os.path.dirname(os.path.abspath(header_path))
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/fork.hpp')    , os.path.join(header_path,"{name}_fork.hpp".format(name=name)) )
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/conv.hpp')    , os.path.join(header_path,"{name}_conv.hpp".format(name=name)) )
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/accum.hpp')   , os.path.join(header_path,"{name}_accum.hpp".format(name=name)))
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/glue.hpp')    , os.path.join(header_path,"{name}_glue.hpp".format(name=name)) )

    return
