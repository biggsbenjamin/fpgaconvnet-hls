#include "conv_tb.hpp"
#include "conv.hpp"

#if CONV_KERNEL_SIZE == 1
const static conv_weight_t weights[CONV_CHANNELS*CONV_FILTERS] = {
#include "weights.csv"
};
#else
const static conv_weight_t weights[CONV_CHANNELS*CONV_FILTERS][CONV_KERNEL_SIZE][CONV_KERNEL_SIZE] = {
#include "weights.csv"
};
#endif
void conv_top(
    stream_t(conv_data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
    stream_t(conv_acc_t) &out
)
{

#pragma HLS DATAFLOW
#pragma HLS RESOURCE variable=weights core=ROM_2P_BRAM

#if CONV_KERNEL_SIZE == 1
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        conv_data_t,
        conv_weight_t,
        conv_acc_t
    >(in[0][0],weights,out);
#else
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        CONV_FINE,
        CONV_KERNEL_SIZE,
        conv_data_t,
        conv_weight_t,
        conv_acc_t
    >(in,weights,out);
#endif

}
