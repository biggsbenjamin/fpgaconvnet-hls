#include "split_tb.hpp"

#define MODULE_NAME SPLIT
#define name        test

#include "auxiliary.hpp"

void split_top(
    stream_t(data_t) in[SPLIT_COARSE],
    stream_t(data_t) out[SPLIT_OUTPUTS][SPLIT_COARSE]
)
{
    #pragma HLS INTERFACE axis port=in
    #pragma HLS INTERFACE axis port=out

    #pragma HLS DATAFLOW

    // SPLIT
    test_split(in,out);

}

#undef MODULE_NAME
#undef name
