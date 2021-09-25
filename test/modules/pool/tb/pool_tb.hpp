#ifndef POOL_TB_HPP_
#define POOL_TB_HPP_

#include "common.hpp"

#include "pool_param.hpp"

/////////////////////////////////

void pool_top(
	stream_t(data_t) in[POOL_KERNEL_SIZE_0][POOL_KERNEL_SIZE_1],
	stream_t(data_t) &out
);

/////////////////////////////////

#endif
