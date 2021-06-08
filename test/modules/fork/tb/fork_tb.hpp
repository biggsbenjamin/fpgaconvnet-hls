#ifndef FORK_TB_HPP_
#define FORK_TB_HPP_

#include "common.hpp"

#include "fork_param.hpp"
typedef data_t fork_t;
/////////////////////////////////

void fork_top(
#if FORK_KERNEL_SIZE == 1
    stream_t(fork_t) &in,
    stream_t(fork_t) out[FORK_COARSE]
#else
    stream_t(fork_t) in[FORK_KERNEL_SIZE][FORK_KERNEL_SIZE],
    stream_t(fork_t) out[FORK_COARSE][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE]
#endif
);

/////////////////////////////////

#endif
