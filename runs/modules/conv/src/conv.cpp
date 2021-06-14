#include "conv_tb.hpp"
#include "conv.hpp"

#if CONV_KERNEL_SIZE == 1
const static weight_t weights[CONV_CHANNELS*CONV_FILTERS] = {
#include "weights.csv"
};
#else
const static weight_t weights[CONV_CHANNELS*CONV_FILTERS][CONV_KERNEL_SIZE][CONV_KERNEL_SIZE] = {
#include "weights.csv"
};
#endif

void conv_top(
    stream_t(data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
    stream_t(acc_t) &out
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
        ap_fixed<CONV_DATA_WORDLENGTH,CONV_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT>,
        ap_fixed<CONV_WEIGHT_WORDLENGTH,CONV_WEIGHT_WORDLENGTH_INTEGER,AP_RND,AP_SAT>,
        ap_fixed<CONV_ACC_WORDLENGTH,CONV_ACC_WORDLENGTH_INTEGER,AP_RND,AP_SAT>
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
        ap_fixed<CONV_DATA_WORDLENGTH,CONV_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT>,
        ap_fixed<CONV_WEIGHT_WORDLENGTH,CONV_WEIGHT_WORDLENGTH_INTEGER,AP_RND,AP_SAT>,
        ap_fixed<CONV_ACC_WORDLENGTH,CONV_ACC_WORDLENGTH_INTEGER,AP_RND,AP_SAT>   
    >(in,weights,out);
#endif

}
