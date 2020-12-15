#include "batch_norm_tb.hpp"

#define MODULE_NAME BATCH_NORM
#define name        test


#include "batch_norm.hpp"

void batch_norm_top(
    stream_t(data_t) &in,
    const data_t scale[BATCH_NORM_CHANNELS],
    const data_t shift[BATCH_NORM_CHANNELS],
    stream_t(data_t) &out
)
{

#pragma HLS RESOURCE variable=scale core=ROM_2P_BRAM
#pragma HLS RESOURCE variable=shift core=ROM_2P_BRAM

#pragma HLS DATAFLOW

    // DUT
    test_batch_norm<0>(in,scale,shift,out);

}

#undef MODULE_NAME
#undef test

