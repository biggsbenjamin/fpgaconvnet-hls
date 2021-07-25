#ifndef GLUE_TB_HPP_
#define GLUE_TB_HPP_

#include "common.hpp"

#include "glue_param.hpp"

/////////////////////////////////
typedef acc_t glue_acc;
typedef ap_fixed<GLUE_WORDLENGTH,GLUE_WORDLENGTH/2, AP_RND,AP_SAT> glue_data;
void glue_top(
    stream_t(glue_acc) in[GLUE_COARSE_IN][GLUE_COARSE_OUT],
    stream_t(glue_data) out[GLUE_COARSE_OUT]
);

/////////////////////////////////

#endif
