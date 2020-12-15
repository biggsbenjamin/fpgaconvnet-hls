#include "glue_tb.hpp"

#define MODULE_NAME GLUE
#define name        test

#include "glue.hpp"

void glue_top(
    stream_t(acc_t) in[GLUE_COARSE_IN][GLUE_COARSE_OUT],
    stream_t(data_t) out[GLUE_COARSE_OUT]
)
{

    #pragma HLS DATAFLOW

    test_glue<0>(in,out);

}

#undef MODULE_NAME
#undef name
