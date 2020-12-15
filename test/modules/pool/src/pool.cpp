#include "pool_tb.hpp"

#define MODULE_NAME POOL
#define name        test

#include "pool.hpp"

void pool_top(
	stream_t(data_t) in[POOL_KERNEL_SIZE][POOL_KERNEL_SIZE],
	stream_t(data_t) &out
)
{

#pragma HLS DATAFLOW

    test_pool<0>(in,out);

}

#undef MODULE_NAME
#undef name
