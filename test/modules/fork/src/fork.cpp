#include "fork_tb.hpp"

#include "fork.hpp"

void fork_top(
#if FORK_KERNEL_SIZE == 1
    stream_t(data_t) &in,
    stream_t(data_t) out[FORK_COARSE]
#else
    stream_t(data_t) in[FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1],
    stream_t(data_t) out[FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1]
#endif
)
{

    #pragma HLS DATAFLOW
#if FORK_KERNEL_SIZE == 1
    fork<
        FORK_BATCH_SIZE,
        FORK_ROWS,
        FORK_COLS,
        FORK_CHANNELS,
        FORK_COARSE
    >(in,out);
#else
    fork<
        FORK_BATCH_SIZE,
        FORK_ROWS,
        FORK_COLS,
        FORK_CHANNELS,
        FORK_COARSE,
        FORK_KERNEL_SIZE_0,
        FORK_KERNEL_SIZE_1
    >(in,out);
#endif

}
