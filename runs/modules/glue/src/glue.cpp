#include "glue_tb.hpp"
#include "glue.hpp"

typedef ap_fixed<GLUE_ACC_WORDLENGTH,GLUE_ACC_WORDLENGTH_INTEGER,AP_RND,AP_SAT> glue_acc_t;
typedef ap_fixed<GLUE_DATA_WORDLENGTH,GLUE_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT> glue_data_t;

void glue_top(
    stream_t(glue_acc_t) in[GLUE_COARSE_IN][GLUE_COARSE_OUT],
    stream_t(glue_data_t) out[GLUE_COARSE_OUT]
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
        glue_acc_t,
        glue_data_t
    >(in,out);


}
