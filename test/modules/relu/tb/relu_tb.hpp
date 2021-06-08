#ifndef RELU_TB_HPP_
#define RELU_TB_HPP_

#include "common.hpp"

#include "relu_param.hpp"

/////////////////////////////////
typedef data_t relu_t;
void relu_top(
	stream_t(relu_t) &in,
	stream_t(relu_t) &out
);

/////////////////////////////////

#endif
