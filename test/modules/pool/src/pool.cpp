#include "pool_tb.hpp"
#include "pool.hpp"

void pool_top(
	stream_t(data_t) in[POOL_KERNEL_SIZE_0][POOL_KERNEL_SIZE_1],
	stream_t(data_t) &out
)
{

#pragma HLS DATAFLOW

    pool<
        POOL_BATCH_SIZE,
        POOL_ROWS,
        POOL_COLS,
        POOL_CHANNELS,
        POOL_KERNEL_SIZE_0,
        POOL_KERNEL_SIZE_1
    >(in,out);

}
