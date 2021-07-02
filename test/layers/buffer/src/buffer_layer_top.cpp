#include "buffer_layer_tb.hpp"
#include "buffer_layer.hpp"

void buffer_layer_top(
  stream_t(data_t) in[BUFFER_LAYER_COARSE],
  stream_t(data_t) ctrl_in[BUFFER_LAYER_COARSE],
  stream_t(data_t) out[BUFFER_LAYER_COARSE]
)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=ctrl_in
#pragma HLS STREAM variable=out

    buffer_layer(in, ctrl_in, out);

}
