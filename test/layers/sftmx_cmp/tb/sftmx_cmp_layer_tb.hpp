#ifndef SFTMX_CMP_LAYER_TB_HPP_
#define SFTMX_CMP_LAYER_TB_HPP_

#include "common.hpp"
#include "sftmx_cmp_layer.hpp"
#include "sftmx_cmp_layer_param.hpp"

void sftmx_cmp_layer_top(
  stream_t(data_t) in,
  stream_t(data_t) out[SFTMX_CMP_LAYER_CTRL]
);

#endif
