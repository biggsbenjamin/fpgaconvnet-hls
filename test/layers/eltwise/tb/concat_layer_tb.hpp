#ifndef CONCAT_LAYER_TB_HPP_
#define CONCAT_LAYER_TB_HPP_

#include "common.hpp"
#include "concat_layer.hpp"
#include "concat_layer_param.hpp"

void concat_layer_top(
  hls::stream<data_t> in_0[CONCAT_LAYER_COARSE],
  hls::stream<data_t> in_1[CONCAT_LAYER_COARSE],
  hls::stream<data_t> in_2[CONCAT_LAYER_COARSE],
  hls::stream<data_t> in_3[CONCAT_LAYER_COARSE],
  hls::stream<data_t> out[CONCAT_LAYER_COARSE],
  int mode
);

#endif
