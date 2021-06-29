#include "relu_tb.hpp"
#include "relu.hpp"

typedef ap_fixed<RELU_DATA_WORDLENGTH,RELU_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT> relu_t;
void relu_top(
    stream_t(relu_t) &in,
    stream_t(relu_t) &out
)
{

    #pragma HLS DATAFLOW

    relu<
        RELU_BATCH_SIZE,
        RELU_ROWS,
        RELU_COLS,
        RELU_CHANNELS,
        relu_t
    >(in,out);

}
