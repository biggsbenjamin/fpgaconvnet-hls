#include "relu_layer_tb.hpp"
#include "relu_layer.hpp"

void relu_layer_top(
  stream_t(data_t) in[RELU_LAYER_COARSE],
  stream_t(data_t) out[RELU_LAYER_COARSE],
  int mode
)
{

#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    relu_layer(in,out,mode);

}
