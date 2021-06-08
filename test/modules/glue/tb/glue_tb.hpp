#ifndef GLUE_TB_HPP_
#define GLUE_TB_HPP_

#include "common.hpp"

#include "glue_param.hpp"

/////////////////////////////////
typedef acc_t glue_acc;
typedef data_t glue_data;
void glue_top(
    stream_t(acc_t) in[GLUE_COARSE_IN][GLUE_COARSE_OUT],
    stream_t(data_t) out[GLUE_COARSE_OUT]
);

/////////////////////////////////

#endif
