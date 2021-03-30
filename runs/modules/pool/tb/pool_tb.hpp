#ifndef POOL_TB_HPP_
#define POOL_TB_HPP_

#include "common.hpp"
#include "pool_param.hpp"

void pool_top(
    stream_t(data_t) in[POOL_KERNEL_SIZE][POOL_KERNEL_SIZE],
    stream_t(data_t) &out
);

#endif
