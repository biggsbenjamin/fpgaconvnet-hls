#ifndef BUFFER_LAYER_TB_HPP_
#define BUFFER_LAYER_TB_HPP_

#include "common.hpp"
#include "buffer_layer.hpp"
#include "buffer_layer_param.hpp"

void buffer_layer_top(
  stream_t(buffer_layer_data_t) in[BUFFER_LAYER_COARSE],
  stream_t(buffer_layer_ctrl_t) &ctrl_in,
  stream_t(buffer_layer_data_t) out[BUFFER_LAYER_COARSE],
  int mode
);

#endif
