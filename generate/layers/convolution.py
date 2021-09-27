# import modules
import os
import shutil

import generate.modules.sliding_window
import generate.modules.fork
import generate.modules.conv
import generate.modules.accum
import generate.modules.glue

convolution_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#define name        {name}
#define NAME        {NAME}
#define {NAME}_ID   {id}

// parameters
#define {NAME}_BATCH_SIZE    {batch_size}
#define {NAME}_ROWS          {rows}
#define {NAME}_COLS          {cols}
#define {NAME}_CHANNELS      {channels}
#define {NAME}_FILTERS       {filters}
#define {NAME}_GROUPS        {groups}
#define {NAME}_COARSE_IN     {coarse_in}
#define {NAME}_COARSE_OUT    {coarse_out}
#define {NAME}_COARSE_GROUP  {coarse_group}
#define {NAME}_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_KERNEL_SIZE_Y {kernel_size_y}
#define {NAME}_FINE          {fine}
#define {NAME}_STRIDE_X      {stride_x}
#define {NAME}_STRIDE_Y      {stride_y}

// coefficients
#define {NAME}_WEIGHTS {NAME}_FILTERS*DIVIDE({NAME}_CHANNELS,{NAME}_GROUPS)*{NAME}_KERNEL_SIZE_X*{NAME}_KERNEL_SIZE_Y

// dimensions out
#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

// SLIDING WINDOW
#define {NAME}_SLIDING_WINDOW_BATCH_SIZE    {batch_size}
#define {NAME}_SLIDING_WINDOW_ROWS          {rows}
#define {NAME}_SLIDING_WINDOW_COLS          {cols}
#define {NAME}_SLIDING_WINDOW_CHANNELS      {channels_per_module}
#define {NAME}_SLIDING_WINDOW_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_SLIDING_WINDOW_KERNEL_SIZE_Y {kernel_size_y}
#define {NAME}_SLIDING_WINDOW_STRIDE_X      {stride_x}
#define {NAME}_SLIDING_WINDOW_STRIDE_Y      {stride_y}
#define {NAME}_SLIDING_WINDOW_PAD_LEFT      {pad_left}
#define {NAME}_SLIDING_WINDOW_PAD_RIGHT     {pad_right}
#define {NAME}_SLIDING_WINDOW_PAD_TOP       {pad_top}
#define {NAME}_SLIDING_WINDOW_PAD_BOTTOM    {pad_bottom}

// FORK
#define {NAME}_FORK_BATCH_SIZE    {batch_size}
#define {NAME}_FORK_ROWS          {rows_out}
#define {NAME}_FORK_COLS          {cols_out}
#define {NAME}_FORK_CHANNELS      {channels_per_module}
#define {NAME}_FORK_COARSE        {coarse_out}
#define {NAME}_FORK_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_FORK_KERNEL_SIZE_Y {kernel_size_y}

// CONV
#define {NAME}_CONV_BATCH_SIZE    {batch_size}
#define {NAME}_CONV_ROWS          {rows_out}
#define {NAME}_CONV_COLS          {cols_out}
#define {NAME}_CONV_GROUPS        {groups_per_module}
#define {NAME}_CONV_CHANNELS      {channels_per_module}
#define {NAME}_CONV_FILTERS       {filters_per_module}
#define {NAME}_CONV_KERNEL_SIZE_X {kernel_size_x}
#define {NAME}_CONV_KERNEL_SIZE_Y {kernel_size_y}
#define {NAME}_CONV_FINE          {fine}
#define {NAME}_CONV_INTERVAL      {interval}

// ACCUM
#define {NAME}_ACCUM_BATCH_SIZE         {batch_size}
#define {NAME}_ACCUM_ROWS               {rows_out}
#define {NAME}_ACCUM_COLS               {cols_out}
#define {NAME}_ACCUM_GROUPS             {groups_per_module}
#define {NAME}_ACCUM_CHANNELS           {channels_per_module}
#define {NAME}_ACCUM_FILTERS            {filters_per_module}

// GLUE
#define {NAME}_GLUE_BATCH_SIZE   {batch_size}
#define {NAME}_GLUE_ROWS         {rows_out}
#define {NAME}_GLUE_COLS         {cols_out}
#define {NAME}_GLUE_FILTERS      {channels_out}
#define {NAME}_GLUE_COARSE_IN    {coarse_in}
#define {NAME}_GLUE_COARSE_OUT   {coarse_out}
#define {NAME}_GLUE_COARSE_GROUP {coarse_group}

#include "sliding_window.hpp"
#include "fork.hpp"
#include "conv.hpp"
#include "accum.hpp"
#include "glue.hpp"

/**
 * FUNCTION DEFINITION
 */

void {name}(
    const weight_t weights[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_GROUP*{NAME}_COARSE_OUT*{NAME}_KERNEL_SIZE_X*{NAME}_KERNEL_SIZE_Y)][{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y],
    stream_t(data_t)  in[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP],
    stream_t(data_t) out[{NAME}_COARSE_OUT*{NAME}_COARSE_GROUP],
    int mode
);

#undef name
#undef NAME
#endif
"""

convolution_layer_template_src = """#include "{name}.hpp"

void {name}(
    const weight_t weights[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_GROUP*{NAME}_COARSE_OUT*{NAME}_KERNEL_SIZE_X*{NAME}_KERNEL_SIZE_Y)][{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y],
    stream_t(data_t) in[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP],
    stream_t(data_t) out[{NAME}_COARSE_OUT*{NAME}_COARSE_GROUP],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW
//#pragma HLS stable variable=weights

    {streams}

    for(unsigned int i=0;i<{NAME}_COARSE_IN*{NAME}_COARSE_GROUP;i++) {{
        #pragma HLS unroll

        {sliding_window}
        {fork}

        for(unsigned int j=0;j<{NAME}_COARSE_OUT;j++) {{
            #pragma HLS unroll

            {conv}
            {accum}

        }}
    }}

    {glue}

}}

"""

def gen_convolution_layer(name,param,src_path,header_path):

    # get sliding window type
    single_channel  = True if param['channels_in'] == 1 else False
    single_stream   = True if param['kernel_size'][0] == 1 and param['kernel_size'][1] == 1 else False
    depthwise       = True if ((param['groups'] == param['filters']) and (param['groups'] == param['channels_in'])) else False
    grouped         = True if param['groups'] > 1 else False

    inputs = {
        'sliding_window': "in[i]",
        'fork'          : "sw_out[i]",
        'conv'          : "fork_out[i][j]",
        'accum'         : "conv_out[i][j]",
        'glue'          : "accum_out"
    }

    outputs = {
        'sliding_window': "sw_out[i]",
        'fork'          : "fork_out[i]",
        'conv'          : "conv_out[i][j]",
        'accum'         : "accum_out[i][j]",
        'glue'          : "out"
    }

    if single_stream:
        inputs['fork'] = "in[i]"
        inputs.pop('sliding_window',None)
        outputs.pop('sliding_window',None)

    """
    if param['coarse_in'] == 1:
        outputs['accum'] = "out[j]"
        inputs.pop('glue',None)
        outputs.pop('glue',None)
    """

    if single_channel:
        inputs['glue'] = "conv_out"
        #outputs['conv'] = "out[j]"
        inputs.pop('accum',None)
        outputs.pop('accum',None)
        #inputs.pop('glue',None)
        #outputs.pop('glue',None)

    """
    if depthwise:
        outputs['conv'] = "out[i]"
        inputs.pop('accum',None)
        outputs.pop('accum',None)
        inputs.pop('glue',None)
        outputs.pop('glue',None)

    if (param['channels_in'])/(param['coarse_in']*param['groups']) == 1 and not single_channel:
        inputs['glue'] = "conv_out"
        inputs.pop('accum',None)
        outputs.pop('accum',None)

    print(inputs, outputs)
    """

    streams = ""

    # SLIDING WINDOW MODULE INIT
    if 'sliding_window' in inputs:
        streams += """
    stream_t(data_t) sw_out[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y];
    #pragma HLS STREAM variable=sw_out
    #pragma HLS ARRAY_PARTITION variable=sw_out complete dim=0
        """.format(NAME=name.upper(),name=name)
        sliding_window = generate.modules.sliding_window.gen_sliding_window_module(
            name+"_sliding_window",
            inputs['sliding_window'],
            outputs['sliding_window'],
            indent=8
        )
    else:
        sliding_window = ''

    # FORK MODULE INIT
    if 'fork' in inputs:
        streams += """
    stream_t(data_t) fork_out[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT]{single_stream}[{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y];
    #pragma HLS STREAM variable=fork_out
    #pragma HLS ARRAY_PARTITION variable=fork_out complete dim=0
        """.format(NAME=name.upper(),name=name,
                single_stream=";//" if single_stream else "")
        fork = generate.modules.fork.gen_fork_module(
            name+"_fork",
            inputs['fork'],
            outputs['fork'],
            indent=8
        )
    else:
        fork = ''

    # CONV MODULE INIT
    if 'conv' in inputs:
        streams += """
    stream_t(acc_t) conv_out[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=conv_out
    #pragma HLS ARRAY_PARTITION variable=conv_out complete dim=0
        """.format(NAME=name.upper(),name=name)
        conv = generate.modules.conv.gen_conv_module(
            name+"_conv",
            inputs['conv'],
            "weights[i][j]",
            outputs['conv'],
            indent=12
        )
    else:
        conv = ''

    # ACCUM MODULE INIT
    if 'accum' in inputs:
        streams += """
    stream_t(acc_t) accum_out[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT];
    #pragma HLS STREAM variable=accum_out
    #pragma HLS ARRAY_PARTITION variable=accum_out complete dim=0
        """.format(NAME=name.upper(),name=name)
        accum = generate.modules.accum.gen_accum_module(
            name+"_accum",
            inputs['accum'],
            outputs['accum'],
            indent=12
        )
    else:
        accum = ''

    # GLUE MODULE INIT
    if 'glue' in inputs:
        glue = generate.modules.glue.gen_glue_module(
            name+"_glue",
            inputs['glue'],
            outputs['glue'],
            indent=4
        )
    else:
        glue = ''

    # src
    convolution_layer_src = convolution_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        buffer_depth    =max(param['buffer_depth'],2),
        streams         =streams,
        sliding_window  =sliding_window,
        fork            =fork,
        conv            =conv,
        accum           =accum,
        glue            =glue
    )

    # header
    convolution_layer_header = convolution_layer_template_header.format(
        name                            =name,
        NAME                            =name.upper(),
        id                              =0, # param['id'],
        batch_size                      =param['batch_size'],
        rows                            =param['rows_in'],
        cols                            =param['cols_in'],
        channels                        =param['channels_in'],
        channels_per_module             =param['channels_in']//(param['coarse_in']*param['coarse_group']),
        filters                         =param['filters'],
        filters_per_module              =param['filters']//(param['coarse_out']*param['coarse_group']),
        groups                          =param['groups'],
        groups_per_module               =param['groups']//param['coarse_group'],
        coarse_in                       =param['coarse_in'],
        coarse_out                      =param['coarse_out'],
        coarse_group                    =param['coarse_group'],
        fine                            =param['fine'],
        interval                        =(param['kernel_size'][0]*param['kernel_size'][1])//param['fine'],
        kernel_size_x                   =param['kernel_size'][0],
        kernel_size_y                   =param['kernel_size'][1],
        stride_x                        =param['stride'][0],
        stride_y                        =param['stride'][1],
        pad_left                        =param['pad_left'],
        pad_right                       =param['pad_right'],
        pad_top                         =param['pad_top'],
        pad_bottom                      =param['pad_bottom'],
        rows_out                        =param['rows_out'],
        cols_out                        =param['cols_out'],
        channels_out                    =param['channels_out']
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(convolution_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(convolution_layer_header)

    return
