#ifndef ACCUM_TB_HPP_
#define ACCUM_TB_HPP_

#include "common.hpp"
#include "accum_param.hpp"
typedef acc_t acc;
void accum_top(
	stream_t(acc) &in,
	stream_t(acc) &out
);

#endif
