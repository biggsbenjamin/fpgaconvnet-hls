#ifndef COMPARE_TB_HPP_
#define COMPARE_TB_HPP_

#include "common.hpp"

#include "compare_param.hpp"

/////////////////////////////////

void compare_top(
    hls::stream<float> &max_in,
    hls::stream<float> &thr_in,
    stream_t(data_t) &out
);

/////////////////////////////////

#endif
