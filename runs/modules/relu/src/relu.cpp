#include "relu_tb.hpp"
#include "relu.hpp"

void relu_top(
    stream_t(data_t) &in,
    stream_t(data_t) &out
)
{

    #pragma HLS DATAFLOW

    relu<
        RELU_BATCH_SIZE,
        RELU_ROWS,
        RELU_COLS,
        RELU_CHANNELS,
        ap_fixed<RELU_DATA_WORDLENGTH,RELU_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT>
    >(in,out);

}
