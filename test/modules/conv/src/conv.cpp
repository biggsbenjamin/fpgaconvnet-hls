#include "conv_tb.hpp"

#define MODULE_NAME CONV 
#define name        test

#include "conv.hpp"

void conv_top(
    stream_t(data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
#if CONV_KERNEL_SIZE == 1
    weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)],
#else
    weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)][CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
#endif
    stream_t(acc_t) &out
)
{

#pragma HLS RESOURCE variable=weights core=ROM_2P_BRAM

#if CONV_KERNEL_SIZE != 1
    const int factor_a = MIN(CONV_FINE,CONV_KERNEL_SIZE);
    const int factor_b = (CONV_FINE<=CONV_KERNEL_SIZE) ? 1 : CONV_KERNEL_SIZE;
DO_PRAGMA(HLS ARRAY_PARTITION variable=weights block factor=factor_a dim=2)
DO_PRAGMA(HLS ARRAY_PARTITION variable=weights block factor=factor_b dim=3)
#endif

#pragma HLS DATAFLOW

#if CONV_KERNEL_SIZE == 1
    test_conv_pw<0>(in[0][0],weights,out);
#else
    test_conv<0>(in,weights,out);
#endif

}

#undef MODULE_NAME
#undef test
