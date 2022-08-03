#include "exponen_tb.hpp"
#include "exponen.hpp"

void exponen_top(
    stream_t(data_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &out
    //stream_t(bdata_t) &out
)
{

#pragma HLS DATAFLOW

    // DUT
    exponen<
        EXPONEN_BATCH_SIZE,
        EXPONEN_ROWS,
        EXPONEN_COLS,
        EXPONEN_CHANNELS
    >(in,out);

}
