#include "split_layer_tb.hpp"
#include "split_layer.hpp"

void split_layer_top(
  stream_t(split_layer_data_t) in[SPLIT_LAYER_COARSE],
  stream_t(split_layer_data_t) out[SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE],
  int mode
)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out
//#pragma HLS STREAM variable=in
//#pragma HLS STREAM variable=out

    split_layer(in,out,mode);

}
