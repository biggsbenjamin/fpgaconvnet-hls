#include "sftmx_cmp_layer_tb.hpp"
#include "sftmx_cmp_layer.hpp"

void sftmx_cmp_layer_top(
  stream_t(data_t) &in, //change to coarse if needed
  stream_t(data_t) out[SFTMX_CMP_LAYER_CTRL]
)
{
#pragma HLS DATAFLOW
#pragma HLS INTERFACE ap_ctrl_chain port=return

//#pragma HLS INTERFACE axis port=in
//#pragma HLS INTERFACE axis port=out
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    sftmx_cmp_layer(in,out);

}

