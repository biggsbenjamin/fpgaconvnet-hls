#ifndef CONV_TB_HPP_
#define CONV_TB_HPP_

#include "common.hpp"

#include "conv_param.hpp"

/////////////////////////////////
typedef data_t Conv_data;
typedef weight_t Conv_weight;
typedef acc_t Conv_acc;

void conv_top(
    stream_t(Conv_data) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
#if CONV_KERNEL_SIZE == 1
    Conv_weight weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)],
#else
    Conv_weight weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)][CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
#endif
    stream_t(Conv_acc) &out
);

/////////////////////////////////

#endif
