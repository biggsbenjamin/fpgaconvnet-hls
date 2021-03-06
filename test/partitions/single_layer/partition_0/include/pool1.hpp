#ifndef POOL1_HPP_
#define POOL1_HPP_

#define name        pool1
#define POOL1_ID   0

#define pool1_input_t          data_t
#define pool1_sliding_window_t data_t
#define pool1_output_t         data_t

#define POOL1_BATCH_SIZE   256
#define POOL1_ROWS         24
#define POOL1_COLS         24
#define POOL1_CHANNELS     20
#define POOL1_COARSE       5
#define POOL1_KERNEL_SIZE  2
#define POOL1_STRIDE       2
#define POOL1_PAD          0
#define POOL1_FINE         1

#define POOL1_COARSE_IN    POOL1_COARSE
#define POOL1_COARSE_OUT   POOL1_COARSE

#define POOL1_ROWS_OUT     12 
#define POOL1_COLS_OUT     12 
#define POOL1_CHANNELS_OUT 20 

// SLIDING WINDOW
#define MODULE_NAME POOL1_SLIDING_WINDOW
#define POOL1_SLIDING_WINDOW_BATCH_SIZE   256
#define POOL1_SLIDING_WINDOW_ROWS         24
#define POOL1_SLIDING_WINDOW_COLS         24
#define POOL1_SLIDING_WINDOW_CHANNELS     4
#define POOL1_SLIDING_WINDOW_KERNEL_SIZE  2
#define POOL1_SLIDING_WINDOW_STRIDE       2
#define POOL1_SLIDING_WINDOW_PAD_LEFT     0
#define POOL1_SLIDING_WINDOW_PAD_RIGHT    0
#define POOL1_SLIDING_WINDOW_PAD_TOP      0
#define POOL1_SLIDING_WINDOW_PAD_BOTTOM   0
#include "pool1_sliding_window.hpp"
#undef MODULE_NAME

// POOL
#define MODULE_NAME POOL1_POOL
#define POOL1_POOL_BATCH_SIZE   256
#define POOL1_POOL_ROWS         12
#define POOL1_POOL_COLS         12
#define POOL1_POOL_CHANNELS     4
#define POOL1_POOL_KERNEL_SIZE  2
#define POOL1_POOL_FINE         1
#include "pool1_pool.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void pool1(
    stream_t(pool1_input_t)  in[POOL1_COARSE],
    stream_t(pool1_output_t) out[POOL1_COARSE],
    int mode
);

#undef name
#endif
