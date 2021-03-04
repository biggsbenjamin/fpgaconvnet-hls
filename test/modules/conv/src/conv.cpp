#include "conv_tb.hpp"
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
#pragma HLS DATAFLOW

#if CONV_KERNEL_SIZE == 1
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS
    >(in[0][0],weights,out);
#else
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        CONV_FINE,
        CONV_KERNEL_SIZE    
    >(in,weights,out);
#endif

}
