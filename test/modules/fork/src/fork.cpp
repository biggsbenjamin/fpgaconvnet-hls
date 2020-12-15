#include "fork_tb.hpp"

#define MODULE_NAME FORK
#define name        test
#include "fork.hpp"

void fork_top(
#if FORK_KERNEL_SIZE == 1
    stream_t(data_t) &in,
    stream_t(data_t) out[FORK_COARSE]
#else
    stream_t(data_t) in[FORK_KERNEL_SIZE][FORK_KERNEL_SIZE],
    stream_t(data_t) out[FORK_COARSE][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE]
#endif
)
{

    #pragma HLS DATAFLOW
    test_fork<0>(in,out);

}

#undef MODULE_NAME
#undef name
