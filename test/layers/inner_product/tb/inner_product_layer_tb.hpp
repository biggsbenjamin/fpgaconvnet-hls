#ifndef INNER_PRODUCT_LAYER_TB_HPP_
#define INNER_PRODUCT_LAYER_TB_HPP_

#include "common.hpp"
#include "inner_product_layer.hpp"
#include "inner_product_layer_param.hpp"

void inner_product_layer_top(
  stream_t(data_t) in[INNER_PRODUCT_LAYER_COARSE_IN],
  stream_t(data_t) out[INNER_PRODUCT_LAYER_COARSE_OUT],
  int mode
);

#endif
