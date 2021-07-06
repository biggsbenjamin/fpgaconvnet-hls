#ifndef BUFFER_LAYER_TB_HPP_
#define BUFFER_LAYER_TB_HPP_

#include "common.hpp"
#include "buffer_layer.hpp"
#include "buffer_layer_param.hpp"

void buffer_layer_top(
  stream_t(data_t) in[BUFFER_LAYER_COARSE],
  stream_t(data_t) &ctrl_in,
  stream_t(data_t) out[BUFFER_LAYER_COARSE]
);

#endif
