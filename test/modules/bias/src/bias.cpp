#include "bias_tb.hpp"
#include "bias.hpp"

void bias_top(
    stream_t(bias_data_t) &in,
    bias_biases_t biases[BIAS_FILTERS],
    stream_t(bias_data_t) &out
)
{

#pragma HLS DATAFLOW
//#pragma HLS RESOURCE variable=biases core=ROM
    
    bias<
        BIAS_BATCH_SIZE,
        BIAS_ROWS,
        BIAS_COLS,
        //BIAS_CHANNELS,
        BIAS_FILTERS,
        bias_data_t,
        bias_biases_t
    >(in,biases,out);
}
