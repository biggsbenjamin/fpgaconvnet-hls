#ifndef SLIDING_WINDOW_TB_HPP_
#define SLIDING_WINDOW_TB_HPP_

#include "common.hpp"
#include "sliding_window_param.hpp"

void sliding_window_top(
    stream_t(data_t) &in,
    stream_t(data_t) out[SLIDING_WINDOW_KERNEL_SIZE_X][SLIDING_WINDOW_KERNEL_SIZE_Y]
);

#endif
