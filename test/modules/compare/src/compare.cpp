#include "compare_tb.hpp"
#include "compare.hpp"

void compare_top(
    hls::stream<float> &max_in,
    hls::stream<float> &thr_in,
    stream_t(data_t) &out
)
{

#pragma HLS DATAFLOW

    // DUT
    compare<
        COMPARE_BATCH_SIZE,
        COMPARE_THR_VAL
    >(max_in, thr_in, out);

}
