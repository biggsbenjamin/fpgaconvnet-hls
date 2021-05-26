#ifndef EXP_HPP_
#define

#include "common.hpp"

/**
 * EXPONENTIAL FUNCTION
 */

template<
>

void exp(
    stream_t(data_t) &in,
    stream_t(data_t) &out
)
{

#pragma HLS INLINE OFF //TODO what does this do?
    
    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
 
#pragma HLS STREAM variable=in //TODO what does this do?
#pragma HLS STREAM variable=out //TODO what does this do?

    for(unsigned long pixel_index=0 ; pixel_index < batch_size*rows*cols*channels ; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind //TODO what does rewind do?
        data_t tmp = in.read();
        //exponential function options: cordic, lookup table, something else?
    }
}

#endif

