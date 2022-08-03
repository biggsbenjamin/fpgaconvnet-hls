#include "squeeze_layer.hpp"

void squeeze_layer_top(
    stream_t(squeeze_layer_data_t) in[SQUEEZE_LAYER_COARSE_IN],
    stream_t(squeeze_layer_data_t) out[SQUEEZE_LAYER_COARSE_OUT],
    int mode
)
{


#pragma HLS STREAM variable=in depth=2
#pragma HLS STREAM variable=out
#pragma HLS DATAFLOW

#pragma HLS INTERFACE ap_ctrl_none port=return
//#pragma HLS INTERFACE ap_fifo port=in
//#pragma HLS INTERFACE ap_fifo port=out
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

    squeeze_layer(in,out,mode);

}

