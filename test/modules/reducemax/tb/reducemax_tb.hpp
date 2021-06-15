#ifndef REDUCEMAX_TB_HPP_
#define REDUCEMAX_TB_HPP_

#include "common.hpp"

#include "reducemax_param.hpp"

/////////////////////////////////

void reducemax_top(
	//stream_t(data_t) &in,
	//stream_t(data_t) &out
    hls::stream<float> &in,
    hls::stream<float> &out
);

/////////////////////////////////

#endif
