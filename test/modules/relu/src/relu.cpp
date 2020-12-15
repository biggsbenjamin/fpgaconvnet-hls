#include "relu_tb.hpp"

#define MODULE_NAME RELU
#define name        test

#include "relu.hpp"

void relu_top(
    stream_t(data_t) &in,
    stream_t(data_t) &out
)
{

#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

#pragma HLS DATAFLOW

    // DUT
    test_relu<0>(in,out);

}

#undef MODULE_NAME
#undef name
