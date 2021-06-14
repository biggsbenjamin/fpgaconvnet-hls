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
        GLUE_COARSE_OUT,
        ap_fixed<GLUE_ACC_WORDLENGTH,GLUE_ACC_WORDLENGTH_INTEGER,AP_RND,AP_SAT>,
        ap_fixed<GLUE_DATA_WORDLENGTH,GLUE_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT>
    >(in,out);


}
