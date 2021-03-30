#include "glue_tb.hpp"
#include "glue.hpp"

void glue_top(
    stream_t(acc_t) in[GLUE_COARSE_IN][GLUE_COARSE_OUT],
    stream_t(data_t) out[GLUE_COARSE_OUT]
)
{

    #pragma HLS DATAFLOW
    glue<
        GLUE_BATCH_SIZE,
        GLUE_ROWS,
        GLUE_COLS,
        GLUE_FILTERS,
        GLUE_COARSE_IN,
        GLUE_COARSE_OUT
    >(in,out);


}
