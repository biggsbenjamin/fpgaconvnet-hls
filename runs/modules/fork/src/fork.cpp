#include "fork_tb.hpp"
#include "fork.hpp"

typedef ap_fixed<FORK_DATA_WORDLENGTH,FORK_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT> fork_t;
void fork_top(
#if FORK_KERNEL_SIZE == 1
    stream_t(fork_t) &in,
    stream_t(fork_t) out[FORK_COARSE]
#else
    stream_t(fork_t) in[FORK_KERNEL_SIZE][FORK_KERNEL_SIZE],
    stream_t(fork_t) out[FORK_COARSE][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE]
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
        FORK_COARSE,
        fork_t
    >(in,out);
#else
    fork<
        FORK_BATCH_SIZE,
        FORK_ROWS,
        FORK_COLS,
        FORK_CHANNELS,
        FORK_COARSE,
        FORK_KERNEL_SIZE,
        fork_t
    >(in,out);
#endif

}
