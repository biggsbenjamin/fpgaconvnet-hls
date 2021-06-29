#ifndef SQUEEZE_TB_HPP_
#define SQUEEZE_TB_HPP_

#include "common.hpp"

#include "squeeze_param.hpp"
typedef data_t squeeze_t;
/////////////////////////////////

void squeeze_top(
    stream_t(squeeze_t) in[SQUEEZE_COARSE_IN],
    stream_t(squeeze_t) out[SQUEEZE_COARSE_OUT]
);

/////////////////////////////////

#endif
