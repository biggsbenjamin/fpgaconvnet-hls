#ifndef BUFFER_TB_HPP_
#define BUFFER_TB_HPP_

#include "common.hpp"

#include "buffer_param.hpp"

/////////////////////////////////

void buffer_top(
	stream_t(data_t) &in,
	stream_t(data_t) &ctrl_in,
	stream_t(data_t) &out
);

/////////////////////////////////

#endif
