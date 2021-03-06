#include "pool1.hpp"

void pool1(
    stream_t(pool1_input_t)  in[POOL1_COARSE],
    stream_t(pool1_output_t) out[POOL1_COARSE],
    int mode
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=in depth=2
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(pool1_sliding_window_t) sw_out[POOL1_COARSE][POOL1_KERNEL_SIZE][POOL1_KERNEL_SIZE]; //sliding window output

#pragma HLS STREAM variable=sw_out
#pragma HLS ARRAY_PARTITION variable=sw_out complete dim=0

    for(unsigned int coarseIndex=0;coarseIndex<POOL1_COARSE;coarseIndex++)
    {
        //SLIDING WINDOW
#pragma HLS UNROLL

        pool1_sliding_window<0>(in[coarseIndex],sw_out[coarseIndex]);



        pool1_pool<0>(sw_out[coarseIndex],out[coarseIndex]);


    }
}

