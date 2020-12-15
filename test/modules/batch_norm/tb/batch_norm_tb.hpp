#ifndef BATCH_NORM_TB_HPP_
#define BATCH_NORM_TB_HPP_

#include "common.hpp"

#include "batch_norm_param.hpp"

/////////////////////////////////

void batch_norm_top(
    stream_t(data_t) &in,
    const data_t scale[BATCH_NORM_CHANNELS],
    const data_t shift[BATCH_NORM_CHANNELS],
    stream_t(data_t) &out
);

/////////////////////////////////

#endif
