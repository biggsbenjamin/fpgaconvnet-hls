#include "glue_tb.hpp"
#include "glue.hpp"

void glue_top(
    stream_t(glue_acc_t) in[GLUE_COARSE_IN*GLUE_COARSE_GROUP][GLUE_COARSE_OUT],
    stream_t(glue_data_t) out[GLUE_COARSE_OUT*GLUE_COARSE_GROUP]
)
{

    #pragma HLS DATAFLOW
    glue<
        GLUE_BATCH_SIZE,
        GLUE_ROWS,
        GLUE_COLS,
        GLUE_FILTERS,
        GLUE_COARSE_IN,
        GLUE_COARSE_OUT,
        GLUE_COARSE_GROUP,
        glue_acc_t,
        glue_data_t
    >(in,out);


}
