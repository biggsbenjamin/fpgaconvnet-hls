#ifndef CMP_HPP_
#define CMP_HPP_

#include "common.hpp"

/**
  * EXIT COMPARE
  */
template<
    unsigned int BATCH_SIZE,
    //unsigned int ROWS,
    //unsigned int COLS,
    //unsigned int CHANNELS,
    float THR_VAL,
>
void compare(
    //stream_t(data_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &max_in,
    hls::stream<float> &thr_in,
    stream_t(data_t) &ctrl_out //TODO change this to some ctrl signal format
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = BATCH_SIZE;
    //const unsigned int rows         = ROWS;
    //const unsigned int cols         = COLS;
    //const unsigned int channels     = CHANNELS;
    const float thr_val             = THR_VAL;
 
#pragma HLS STREAM variable=max_in
#pragma HLS STREAM variable=thr_in
#pragma HLS STREAM variable=ctrl_out

    //TODO:
    //comparison against threshold value
    //maybe multiply threshold by sftmx sum
    //output ctrl signals to relevant buffer modu + exit select mod
    //  needs to be done synchronously with result?
    //synchronise with batch id pipeline
    //TODO - consider later exits will be ctrl dependent, unknown number of iterations
    //  might make sense to have two versions (in this file)

    float thr_mult, thr_res, cmp_max;
    
    batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) 
                {
        thr_mult = thr_in.read(); //this one will arrive later
        cmp_max = max_in.read();
        thr_res = thr_mult * thr_value;
        if (cmp_max > thr_res) {
            //exit should occur
            ctrl_out.write(1.0);
        } else {
            //no exit for this batch item
            ctrl_out.write(0.0);
        }
    }
}

#endif
