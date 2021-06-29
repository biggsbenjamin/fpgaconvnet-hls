#include "accum_tb.hpp"
#include "accum.hpp"

typedef ap_fixed<ACCUM_DATA_WORDLENGTH,ACCUM_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT> accum_t;
void accum_top(
    stream_t(accum_t) &in,
    stream_t(accum_t) &out
)
{

    #pragma HLS DATAFLOW

    // DUT
    accum<
        ACCUM_BATCH_SIZE,  
        ACCUM_ROWS,  
        ACCUM_COLS,  
        ACCUM_CHANNELS,  
        ACCUM_FILTERS,  
        ACCUM_GROUPS,
        accum_t
    >(in,out);

}

