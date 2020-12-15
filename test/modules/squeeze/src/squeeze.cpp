#include "squeeze_tb.hpp"

#define MODULE_NAME SQUEEZE
#define name        test
#include "squeeze.hpp"

void squeeze_top(
    stream_t(data_t) in[SQUEEZE_COARSE_IN],
    stream_t(data_t) out[SQUEEZE_COARSE_OUT]
)
{

    #pragma HLS DATAFLOW
    test_squeeze<0>(in,out);

}

#undef MODULE_NAME
#undef name
