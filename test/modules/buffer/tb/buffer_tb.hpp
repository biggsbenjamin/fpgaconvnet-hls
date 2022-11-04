#ifndef BUFFER_TB_HPP_
#define BUFFER_TB_HPP_

#include "common.hpp"

#include "buffer_param.hpp"

typedef b_data_t buffer_t;
//typedef b_data_t ctrl_t;
typedef b_bool_t ctrl_t;

/////////////////////////////////

void buffer_top(
	stream_t(buffer_t) &in,
	stream_t(ctrl_t) &ctrl_in,
	stream_t(buffer_t) &out
);

/////////////////////////////////

#endif
