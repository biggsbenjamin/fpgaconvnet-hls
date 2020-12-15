#ifndef SPLIT_TB_HPP_
#define SPLIT_TB_HPP_

#include "common.hpp"

#include "split_param.hpp"

/////////////////////////////////

void split_top(
	stream_t(data_t) in[SPLIT_COARSE],
	stream_t(data_t) out[SPLIT_OUTPUTS][SPLIT_COARSE]
);

/////////////////////////////////

#endif
