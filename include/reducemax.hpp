#ifndef REDMAX_HPP_
#define REDMAX_HPP_

#include "common.hpp"

/**
  * REDUCE MAX 
  */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS
>
void reducemax(
    //stream_t(data_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &in,
    hls::stream<float> &out
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
 
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    //data_t cache;
    float cache,tmp;
    //#pragma HLS DEPENDENCE variable=cache RAW intra true

    batch_loop: for(unsigned long batch_index=0;batch_index<batch_size;batch_index++) {
        pixel_loop: for (unsigned long px_index = 0; px_index < rows*cols*channels; px_index++) {
#pragma HLS PIPELINE II=1 rewind
            if (px_index == 0) {
                cache = in.read();
                //std::cout<<"rdmx,in cache: "<<cache<<std::endl;
            } else {
                tmp = in.read();
                cache = (cache > tmp ) ? cache : tmp ;
                //std::cout<<"rdmx,in cache: "<<cache<<std::endl;
            }
        }
        out.write(cache);
    }
}

#endif

