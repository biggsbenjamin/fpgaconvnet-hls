#include "bias_tb.hpp"
#include "bias.hpp"

void bias_top(
    stream_t(bias_data_t) in[BIAS_COARSE_OUT],
    //bias_weight_t weights[BIAS_CHANNELS*DIVIDE(BIAS_FILTERS,BIAS_GROUPS)],
    bias_weight_t weights[BIAS_FILTERS],
    stream_t(bias_acc_t) &ou[BIAS_COARSE_OUT]t
)
{

#pragma HLS DATAFLOW
#pragma HLS RESOURCE variable=weights core=ROM_2P_BRAM

    bias<
        BIAS_BATCH_SIZE,
        BIAS_ROWS,
        BIAS_COLS,
        BIAS_CHANNELS,
        BIAS_FILTERS,
        bias_data_t,
        bias_weight_t,
        bias_acc_t
    >(in[0][0],weights,out);
}
