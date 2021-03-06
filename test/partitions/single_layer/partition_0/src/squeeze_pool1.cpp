#include "squeeze_pool1.hpp"

void squeeze_pool1(
    stream_t(squeeze_pool1_input_t)  in[SQUEEZE_POOL1_COARSE_IN],
    stream_t(squeeze_pool1_output_t) out[SQUEEZE_POOL1_COARSE_OUT],
    int mode
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth=2
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW


    squeeze_pool1_squeeze<0>(in,out);



}

