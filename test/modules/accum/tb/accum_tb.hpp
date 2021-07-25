#ifndef ACCUM_TB_HPP_
#define ACCUM_TB_HPP_

#include "common.hpp"
#include "accum_param.hpp"
typedef ap_fixed<ACCUM_WORDLENGTH,ACCUM_WORDLENGTH/2, AP_RND,AP_SAT> acc;
void accum_top(
	stream_t(acc) &in,
	stream_t(acc) &out
);

#endif
