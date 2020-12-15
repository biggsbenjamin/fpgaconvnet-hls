#include "concat_layer_tb.hpp"
#include "concat_layer.hpp"

void concat_layer_top(
  hls::stream<data_t> in_0[CONCAT_LAYER_COARSE],
  hls::stream<data_t> in_1[CONCAT_LAYER_COARSE],
  hls::stream<data_t> in_2[CONCAT_LAYER_COARSE],
  hls::stream<data_t> in_3[CONCAT_LAYER_COARSE],
  hls::stream<data_t> out[CONCAT_LAYER_COARSE],
  int mode
)
{
#pragma HLS DATAFLOW

//#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out
//#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    concat_layer(in_0, in_1, in_2, in_3,out,mode);

}
