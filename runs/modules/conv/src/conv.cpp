#include "conv_tb.hpp"
#include "conv.hpp"

const static weight_t weights[DIVIDE(CONV_CHANNELS*CONV_FILTERS,CONV_GROUPS)][CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y] = {
#include "weights.csv"
};

void conv_top(
    stream_t(data_t) in[CONV_KERNEL_SIZE_X][CONV_KERNEL_SIZE_Y],
    stream_t(acc_t) &out
)
{

#pragma HLS DATAFLOW
#pragma HLS RESOURCE variable=weights core=ROM

#if CONV_KERNEL_SIZE_X == 1 && CONV_KERNEL_SIZE_Y == 1
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        CONV_GROUPS
    >(in[0][0],weights,out);
#else
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        CONV_GROUPS,
        CONV_FINE,
        CONV_KERNEL_SIZE_X,
        CONV_KERNEL_SIZE_Y
    >(in,weights,out);
#endif

}
