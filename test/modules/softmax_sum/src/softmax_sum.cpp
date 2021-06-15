#include "softmax_sum_tb.hpp"
#include "softmax_sum.hpp"

void softmax_sum_top(
    //stream_t(data_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &in,
    hls::stream<float> &out
)
{

#pragma HLS DATAFLOW

    // DUT
    softmax_sum<
        SOFTMAX_SUM_BATCH_SIZE,
        SOFTMAX_SUM_ROWS,
        SOFTMAX_SUM_COLS,
        SOFTMAX_SUM_CHANNELS
    >(in,out);

}
