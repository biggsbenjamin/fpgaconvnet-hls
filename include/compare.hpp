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
    typename compare_t
>
void compare(
    //stream_t(data_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &max_in,
    hls::stream<float> &thr_in,
    float thr_val[1],
    stream_t(compare_t) &ctrl_out //TODO change this to some ctrl signal format
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = BATCH_SIZE;
    //const unsigned int rows         = ROWS;
    //const unsigned int cols         = COLS;
    //const unsigned int channels     = CHANNELS;
    const float threshold = thr_val[0];
 
#pragma HLS STREAM variable=max_in depth=batch_size
#pragma HLS STREAM variable=thr_in depth=batch_size
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
    
    batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) {
#pragma HLS PIPELINE II=1 rewind 
        thr_mult = thr_in.read(); //this one will arrive later
        cmp_max = max_in.read();
        thr_res = thr_mult * threshold;
        //std::cout<<"threshold: "<<thr_res<<std::endl;
        //std::cout<<"max val: "<<cmp_max<<std::endl;
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
