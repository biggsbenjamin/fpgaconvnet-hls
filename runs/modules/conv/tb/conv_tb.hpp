#ifndef CONV_TB_HPP_
#define CONV_TB_HPP_

#include "common.hpp"
#include "conv_param.hpp"

void conv_top(
    stream_t(data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
    stream_t(acc_t) &out
);

#endif
