#ifndef ELTWISE_LAYER_TB_HPP_
#define ELTWISE_LAYER_TB_HPP_

#include "common.hpp"
#include "eltwise_layer.hpp"
#include "eltwise_layer_param.hpp"

void eltwise_layer_top(
  hls::stream<data_t> in_0[ELTWISE_LAYER_COARSE],
  hls::stream<data_t> in_1[ELTWISE_LAYER_COARSE],
  hls::stream<data_t> out[ELTWISE_LAYER_COARSE],
  int mode
);

#endif
