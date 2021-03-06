#ifndef SQUEEZE_POOL1_HPP_
#define SQUEEZE_POOL1_HPP_

#define name        squeeze_pool1
#define SQUEEZE_POOL1_ID   0

#define squeeze_pool1_input_t          data_t
#define squeeze_pool1_output_t         data_t

#define SQUEEZE_POOL1_BATCH_SIZE   256
#define SQUEEZE_POOL1_ROWS         12
#define SQUEEZE_POOL1_COLS         12
#define SQUEEZE_POOL1_CHANNELS     20

#define SQUEEZE_POOL1_COARSE_IN    5
#define SQUEEZE_POOL1_COARSE_OUT   4

#define SQUEEZE_POOL1_ROWS_OUT     12 
#define SQUEEZE_POOL1_COLS_OUT     12 
#define SQUEEZE_POOL1_CHANNELS_OUT 20 

#define MODULE_NAME SQUEEZE_POOL1_SQUEEZE
#define SQUEEZE_POOL1_SQUEEZE_BATCH_SIZE   256
#define SQUEEZE_POOL1_SQUEEZE_ROWS         12
#define SQUEEZE_POOL1_SQUEEZE_COLS         12
#define SQUEEZE_POOL1_SQUEEZE_CHANNELS     20
#define SQUEEZE_POOL1_SQUEEZE_COARSE_IN    5
#define SQUEEZE_POOL1_SQUEEZE_COARSE_OUT   4
#include "squeeze_pool1_squeeze.hpp"
#undef MODULE_NAME

/**
 * FUNCTION DEFINITION
 */

void squeeze_pool1(
    stream_t(squeeze_pool1_input_t)  in[SQUEEZE_POOL1_COARSE_IN],
    stream_t(squeeze_pool1_output_t) out[SQUEEZE_POOL1_COARSE_OUT],
    int mode
);

#undef name
#endif
