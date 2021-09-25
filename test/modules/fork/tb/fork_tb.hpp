#ifndef FORK_TB_HPP_
#define FORK_TB_HPP_

#include "common.hpp"

#include "fork_param.hpp"

/////////////////////////////////

void fork_top(
#if FORK_KERNEL_SIZE == 1
    stream_t(data_t) &in,
    stream_t(data_t) out[FORK_COARSE]
#else
    stream_t(data_t) in[FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1],
    stream_t(data_t) out[FORK_COARSE][FORK_KERNEL_SIZE_0][FORK_KERNEL_SIZE_1]
#endif
);

/////////////////////////////////

#endif
