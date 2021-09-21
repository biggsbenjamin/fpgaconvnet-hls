#ifndef CONV_TB_HPP_
#define CONV_TB_HPP_

#include "common.hpp"
#include "conv_param.hpp"

typedef     ap_fixed<CONV_DATA_WORDLENGTH,CONV_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT> conv_data_t;
typedef     ap_fixed<CONV_WEIGHT_WORDLENGTH,CONV_WEIGHT_WORDLENGTH_INTEGER,AP_RND,AP_SAT> conv_weight_t;
typedef     ap_fixed<CONV_ACC_WORDLENGTH,CONV_ACC_WORDLENGTH_INTEGER,AP_RND,AP_SAT> conv_acc_t;
void conv_top(
    stream_t(conv_data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
    stream_t(conv_acc_t) &out
);

#endif
