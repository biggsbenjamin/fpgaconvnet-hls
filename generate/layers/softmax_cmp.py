# import modules
import os
import shutil

import generate.modules.exponen
import generate.modules.fork
import generate.modules.softmax_sum
import generate.modules.reducemax
import generate.modules.compare

softmax_cmp_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "exponen.hpp"
#include "fork.hpp"
#include "softmax_sum.hpp"
#include "reducemax.hpp"
#include "compare.hpp"

#define name        {name}
#define {NAME}_ID   {id}

#define {name}_input_t          data_t
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

#define {NAME}_COND_TYPE    {cond_type}
#define {NAME}_CTRL_EDGES   {ctrl_edges}
#define {NAME}_CTRL         {ctrl}
#define {NAME}_THRESHOLD    {threshold}

// EXPONEN
#define {NAME}_EXPONEN_BATCH_SIZE   {batch_size}
#define {NAME}_EXPONEN_ROWS         {rows}
#define {NAME}_EXPONEN_COLS         {cols}
#define {NAME}_EXPONEN_CHANNELS     {channels}

// FORK_I
#define {NAME}_FORK_I_BATCH_SIZE   {batch_size}
#define {NAME}_FORK_I_ROWS         {rows_out}
#define {NAME}_FORK_I_COLS         {cols_out}
#define {NAME}_FORK_I_CHANNELS     {channels_per_module}
#define {NAME}_FORK_I_COARSE       2


// SOFTMAX_SUM
#define {NAME}_SOFTMAX_SUM_BATCH_SIZE   {batch_size}
#define {NAME}_SOFTMAX_SUM_ROWS         {rows}
#define {NAME}_SOFTMAX_SUM_COLS         {cols}
#define {NAME}_SOFTMAX_SUM_CHANNELS     {channels_per_module}

// REDUCEMAX
#define {NAME}_REDUCEMAX_BATCH_SIZE   {batch_size}
#define {NAME}_REDUCEMAX_ROWS         {rows}
#define {NAME}_REDUCEMAX_COLS         {cols}
#define {NAME}_REDUCEMAX_CHANNELS     {channels_per_module}

// COMPARE
#define {NAME}_COMPARE_BATCH_SIZE   {batch_size}
#define {NAME}_COMPARE_ROWS         {rows}
#define {NAME}_COMPARE_COLS         {cols}
#define {NAME}_COMPARE_CHANNELS     {channels_per_module}

// FORK_O
#define {NAME}_FORK_O_BATCH_SIZE   {batch_size}
#define {NAME}_FORK_O_ROWS         1
#define {NAME}_FORK_O_COLS         1
#define {NAME}_FORK_O_CHANNELS     1
#define {NAME}_FORK_O_COARSE       {NAME}_CTRL
#define {NAME}_FORK_O_KERNEL_SIZE  1

/*
 * FUNCTION DEFINITION
 */

void {name}(
    stream_t(data_t)    &in,
    stream_t(data_t)    out[{NAME}_CTRL]
);

#undef name
#endif
"""

softmax_cmp_layer_template_src = """#include "{name}.hpp"

void {name}(
    stream_t(data_t)    &in,
    stream_t(data_t)    out[{NAME}_CTRL]
)
{{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=in depth={buffer_depth}
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    hls::stream<float> exponen_out; //exponential output
    hls::stream<float> fork_i_out[2]; //internal fork output
    hls::stream<float> reducemax_out;
    hls::stream<float> softmax_sum_out;
    stream_t(data_t) cmp_out;

#pragma HLS STREAM variable=exponen_out
#pragma HLS STREAM variable=fork_i_out
#pragma HLS ARRAY_PARTITION variable=fork_i_out complete dim=0
#pragma HLS STREAM variable=reducemax_out
#pragma HLS STREAM variable=softmax_sum_out
#pragma HLS STREAM variable=cmp_out

    float cmp_thr[1];
    cmp_thr[0] = {threshold};


//TODO check if the ctrl out fork needs to be unrolled in a loop

{exponen}
{fork_i}
{reducemax}
{softmax_sum}
{compare}
{fork_o}

}}

"""

def gen_softmax_cmp_layer(name, param, src_path, header_path):
    # EXPONEN MODULE INIT
    exponen = generate.modules.exponen.gen_exponen_module(
        name+"_exponen",
        "in",
        "exponen_out",
        indent=8
    )
    # FORK_I MODULE INIT
    fork_i = generate.modules.fork.gen_fork_module(
        name+"_fork_i",
        "exponen_out",
        "fork_i_out", #TODO does this need an array?
        indent=8,
        fp_fork=True
    )
    # REDUCEMAX MODULE INIT
    reducemax = generate.modules.reducemax.gen_reducemax_module(
        name+"_reducemax",
        "fork_i_out[0]",
        "reducemax_out",
        indent=8
    )
    # SOFTMAX_SUM MODULE INIT
    softmax_sum = generate.modules.softmax_sum.gen_softmax_sum_module(
        name+"_softmax_sum",
        "fork_i_out[1]",
        "softmax_sum_out",
        indent=8
    )
    # COMPARE MODULE INIT
    compare = generate.modules.compare.gen_compare_module(
        name+"_compare",
        "reducemax_out",
        "softmax_sum_out",
        "cmp_thr",
        "cmp_out",
        indent=8
    )
    # FORK_O MODULE INIT
    fork_o = generate.modules.fork.gen_fork_module(
        name+"_fork_o",
        "cmp_out",
        "out", #TODO check if this needs array
        indent=8,
    )

    #src
    softmax_cmp_layer_src = softmax_cmp_layer_template_src.format(
        name            = name,
        NAME            = name.upper(),
        buffer_depth    = max(param['buffer_depth'],2),
        exponen         = exponen,
        fork_i          = fork_i,
        reducemax       = reducemax,
        softmax_sum     = softmax_sum,
        compare         = compare,
        fork_o          = fork_o,
        threshold       = param['threshold']
    )

    #header
    softmax_cmp_layer_header = softmax_cmp_layer_template_header.format(
        name                =name,
        NAME                =name.upper(),
        id                  =0, # param['id'],
        batch_size          =param['batch_size'],
        rows                =param['rows_in'],
        cols                =param['cols_in'],
        channels            =param['channels_in'],
        channels_per_module =int(param['channels_in']/(param['coarse'])), #FIXME
        coarse              =param['coarse'],
        rows_out            =param['rows_out'],
        cols_out            =param['cols_out'],
        channels_out        =param['channels_out'],
        cond_type           =param['cond_type'],
        ctrl_edges          =param['ctrl_edges'],
        ctrl                =param['ctrl'],
        threshold           =param['threshold']
    )

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(softmax_cmp_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(softmax_cmp_layer_header)

    return
