#ifndef SPLIT_LAYER_TB_HPP_
#define SPLIT_LAYER_TB_HPP_

#include "common.hpp"
#include "split_layer.hpp"
#include "split_layer_param.hpp"

void split_layer_top(
  stream_t(data_t) in[SPLIT_LAYER_COARSE],
  stream_t(data_t) out[SPLIT_LAYER_COARSE][SPLIT_LAYER_PORTS_OUT]
);

#endif
