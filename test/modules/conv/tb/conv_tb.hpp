#ifndef CONV_TB_HPP_
#define CONV_TB_HPP_

#include "common.hpp"

#include "conv_param.hpp"

/////////////////////////////////

void conv_top(
    stream_t(data_t) in[CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1],
#if CONV_KERNEL_SIZE == 1
    weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)],
#else
    weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1],
#endif
    stream_t(acc_t) &out
);

/////////////////////////////////

#endif
