#include "eltwise_layer_tb.hpp"
#include "eltwise_layer.hpp"

void eltwise_layer_top(
  hls::stream<data_t> in_0[ELTWISE_LAYER_COARSE],
  hls::stream<data_t> in_1[ELTWISE_LAYER_COARSE],
  hls::stream<data_t> out[ELTWISE_LAYER_COARSE],
  int mode
)
{
#pragma HLS DATAFLOW

//#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out
//#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    eltwise_layer(in_0, in_1,out,mode);

}
