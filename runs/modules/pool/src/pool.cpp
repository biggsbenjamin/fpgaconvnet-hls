#include "pool_tb.hpp"
#include "pool.hpp"

void pool_top(
    stream_t(data_t) in[POOL_KERNEL_SIZE][POOL_KERNEL_SIZE],
    stream_t(data_t) &out
)
{

    #pragma HLS DATAFLOW
    pool<
        POOL_BATCH_SIZE,
        POOL_ROWS,
        POOL_COLS,
        POOL_CHANNELS,
        POOL_KERNEL_SIZE,
        ap_fixed<POOL_DATA_WORDLENGTH,POOL_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT>
    >(in,out);

}
