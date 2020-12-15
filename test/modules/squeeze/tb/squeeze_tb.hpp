#ifndef SQUEEZE_TB_HPP_
#define SQUEEZE_TB_HPP_

#include "common.hpp"

#include "squeeze_param.hpp"

/////////////////////////////////

void squeeze_top(
    stream_t(data_t) in[SQUEEZE_COARSE_IN],
    stream_t(data_t) out[SQUEEZE_COARSE_OUT]
);

/////////////////////////////////

#endif
