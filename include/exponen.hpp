#ifndef EXPONEN_HPP_
#define EXPONEN_HPP_

#include "common.hpp"
#include "hls_math.h"

/**
 * EXPONENTIAL FUNCTION
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS
>

void exponen(
    stream_t(data_t) &in,
    stream_t(data_t) &out
)
{

#pragma HLS INLINE OFF //prevents functions being inlined
    
    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
 
#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=out //sets up streaming data type

    for(unsigned long pixel_index=0 ; pixel_index < batch_size*rows*cols*channels ; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind //TODO what does rewind do?
        data_t tmp = in.read();
        //exponential function options: cordic, lookup table, something else?
        //for now: convert to float, use built in hls_math exp function
        out.write(data_t{ hls::exp(tmp.to_float()) });
    }
}

#endif

