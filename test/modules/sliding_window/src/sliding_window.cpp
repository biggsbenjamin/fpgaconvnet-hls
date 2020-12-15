#include "sliding_window_tb.hpp"

#define MODULE_NAME SLIDING_WINDOW
#define name        test

#include "sliding_window.hpp"

void sliding_window_top(
    stream_t(data_t) &in,
    stream_t(data_t) out[SLIDING_WINDOW_KERNEL_SIZE][SLIDING_WINDOW_KERNEL_SIZE]
)
{

    #pragma HLS INTERFACE axis port=in
    #pragma HLS INTERFACE axis port=out

    #pragma HLS ARRAY_PARTITION variable=out complete dim=0

    #pragma HLS DATAFLOW

    test_sliding_window<0>(in,out);

}

#undef MODULE_NAME
#undef name
