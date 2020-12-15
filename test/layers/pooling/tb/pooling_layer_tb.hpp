#ifndef POOLING_LAYER_TB_HPP_
#define POOLING_LAYER_TB_HPP_

#include "common.hpp"
#include "pooling_layer.hpp"
#include "pooling_layer_param.hpp"

void pooling_layer_top(
  stream_t(data_t) in[POOLING_LAYER_COARSE],
  stream_t(data_t) out[POOLING_LAYER_COARSE],
  int mode
);

#endif
