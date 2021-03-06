#ifndef CONV1_HPP_
#define CONV1_HPP_

#define name        conv1
#define CONV1_ID   0

// parameters
#define CONV1_BATCH_SIZE   256
#define CONV1_ROWS         28
#define CONV1_COLS         28
#define CONV1_CHANNELS     1
#define CONV1_FILTERS      20
#define CONV1_GROUPS       1
#define CONV1_COARSE_IN    1
#define CONV1_COARSE_OUT   5
#define CONV1_KERNEL_SIZE  5
#define CONV1_FINE         25
#define CONV1_STRIDE       1
#define CONV1_PAD          0

// coefficients
#define CONV1_WEIGHTS CONV1_CHANNELS*DIVIDE(CONV1_FILTERS,CONV1_GROUPS)*CONV1_KERNEL_SIZE*CONV1_KERNEL_SIZE

// dimensions out
#define CONV1_ROWS_OUT     24 
#define CONV1_COLS_OUT     24 
#define CONV1_CHANNELS_OUT 20 

// SLIDING WINDOW
#define MODULE_NAME CONV1_SLIDING_WINDOW
#define CONV1_SLIDING_WINDOW_BATCH_SIZE    256
#define CONV1_SLIDING_WINDOW_ROWS          28
#define CONV1_SLIDING_WINDOW_COLS          28
#define CONV1_SLIDING_WINDOW_CHANNELS      1
#define CONV1_SLIDING_WINDOW_KERNEL_SIZE   5
#define CONV1_SLIDING_WINDOW_STRIDE        1
#define CONV1_SLIDING_WINDOW_PAD_LEFT      0
#define CONV1_SLIDING_WINDOW_PAD_RIGHT     0
#define CONV1_SLIDING_WINDOW_PAD_TOP       0
#define CONV1_SLIDING_WINDOW_PAD_BOTTOM    0
#include "conv1_sliding_window.hpp"
#undef MODULE_NAME

// FORK
#define MODULE_NAME CONV1_FORK
#define CONV1_FORK_BATCH_SIZE  256
#define CONV1_FORK_ROWS        24
#define CONV1_FORK_COLS        24
#define CONV1_FORK_CHANNELS    1
#define CONV1_FORK_COARSE      5
#define CONV1_FORK_KERNEL_SIZE 5
#include "conv1_fork.hpp"
#undef MODULE_NAME

// CONV
#define MODULE_NAME CONV1_CONV
#define CONV1_CONV_BATCH_SIZE  256
#define CONV1_CONV_ROWS        24
#define CONV1_CONV_COLS        24
#define CONV1_CONV_CHANNELS    1
#define CONV1_CONV_FILTERS     4
#define CONV1_CONV_KERNEL_SIZE 5
#define CONV1_CONV_FINE        25
#define CONV1_CONV_INTERVAL    1
#include "conv1_conv.hpp"
#undef MODULE_NAME

// ACCUM
#define MODULE_NAME CONV1_ACCUM
#define CONV1_ACCUM_BATCH_SIZE         256
#define CONV1_ACCUM_ROWS               24
#define CONV1_ACCUM_COLS               24
#define CONV1_ACCUM_GROUPS             1
#define CONV1_ACCUM_CHANNELS           1
#define CONV1_ACCUM_FILTERS            4
#define CONV1_ACCUM_CHANNELS_PER_GROUP 1
#define CONV1_ACCUM_FILTERS_PER_GROUP  4
#include "conv1_accum.hpp"
#undef MODULE_NAME

// GLUE
#define MODULE_NAME CONV1_GLUE
#define CONV1_GLUE_BATCH_SIZE  256
#define CONV1_GLUE_ROWS        24
#define CONV1_GLUE_COLS        24
#define CONV1_GLUE_FILTERS     20 
#define CONV1_GLUE_COARSE_IN   1
#define CONV1_GLUE_COARSE_OUT  5
#define CONV1_GLUE_ACC         in[0][out_index].read()
#include "conv1_glue.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void conv1(
#if CONV1_KERNEL_SIZE == 1
    const weight_t weights[CONV1_COARSE_IN][CONV1_COARSE_OUT][DIVIDE(CONV1_CHANNELS,CONV1_COARSE_IN)*DIVIDE(CONV1_FILTERS,CONV1_COARSE_OUT*CONV1_GROUPS)],
#else
    const weight_t weights[CONV1_COARSE_IN][CONV1_COARSE_OUT][DIVIDE(CONV1_CHANNELS,CONV1_COARSE_IN)*DIVIDE(CONV1_FILTERS,CONV1_COARSE_OUT*CONV1_GROUPS)][CONV1_KERNEL_SIZE][CONV1_KERNEL_SIZE],
#endif
    stream_t(data_t)  in[CONV1_COARSE_IN],
    stream_t(data_t) out[CONV1_COARSE_OUT],
    int mode
);

#undef name
#endif
