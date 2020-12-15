#include "accum_tb.hpp"

#define MODULE_NAME ACCUM
#define name        test

#include "accum.hpp"

void accum_top(
	stream_t(acc_t) &in,
	stream_t(acc_t) &out
)
{

    #pragma HLS INTERFACE axis port=in
    #pragma HLS INTERFACE axis port=out

    #pragma HLS DATAFLOW
    
    // DUT
    test_accum<0>(in,out);

}

#undef MODULE_NAME
#undef name
