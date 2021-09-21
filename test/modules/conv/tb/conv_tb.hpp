#ifndef CONV_TB_HPP_
#define CONV_TB_HPP_

#include "common.hpp"
#include "conv_param.hpp"

typedef     ap_fixed<CONV_DATAWORDLENGTH,CONV_DATAWORDLENGTH/2,AP_RND,AP_SAT> conv_data_t;
typedef     ap_fixed<CONV_WEIGHTWORDLENGTH,CONV_WEIGHTWORDLENGTH/2,AP_RND,AP_SAT> conv_weight_t;
typedef     ap_fixed<CONV_ACCWORDLENGTH,CONV_ACCWORDLENGTH/2,AP_RND,AP_SAT> conv_acc_t;
void conv_top(
    stream_t(conv_data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
    stream_t(conv_acc_t) &out
)
;

#endif