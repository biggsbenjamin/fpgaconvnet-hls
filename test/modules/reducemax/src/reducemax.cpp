#include "reducemax_tb.hpp"
#include "reducemax.hpp"

void reducemax_top(
    //stream_t(data_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &in,
    hls::stream<float> &out
)
{

#pragma HLS DATAFLOW

    // DUT
    reducemax<
        REDUCEMAX_BATCH_SIZE,
        REDUCEMAX_ROWS,
        REDUCEMAX_COLS,
        REDUCEMAX_CHANNELS
    >(in,out);

}
