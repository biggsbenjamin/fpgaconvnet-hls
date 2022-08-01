#ifndef SOFTMAX_SUM_HPP_ 
#define SOFTMAX_SUM_HPP_ 

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS
>
void softmax_sum(
    //stream_t(acc_t) &in, //30.16, int.bin_point - larger range
    //stream_t(acc_t) &out
    hls::stream<float> &in,
    hls::stream<float> &out
)
{

    #pragma HLS INLINE OFF 
    
    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;

    const unsigned int out_d = rows*cols*channels;
    //TODO coarse grain folding possibly    

    #pragma HLS STREAM variable=in depth=out_d
    #pragma HLS STREAM variable=out
    
    //acc_t sum;
    float sum;

    batch_loop: for (unsigned long batch_index=0;batch_index<batch_size;batch_index++ ) {
        stream_pixel_loop: for(unsigned long pixel_index=0;pixel_index<out_d;pixel_index++) {
#pragma HLS PIPELINE II=1 rewind
            if (pixel_index == 0){
                //restart sum
                sum = in.read();
                //std::cout<<"at 0 sum: "<<sum<<std::endl;

            } else {
                sum = sum + in.read();
                //std::cout<<"all other sum: "<<sum<<std::endl;
            }
        }
        out.write(sum);
    }
}

#endif
