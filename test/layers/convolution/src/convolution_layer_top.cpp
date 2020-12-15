#include "convolution_layer_tb.hpp"
#include "convolution_layer.hpp"

#if CONVOLUTION_LAYER_KERNEL_SIZE == 1
const static weight_t weights[CONVOLUTION_LAYER_COARSE_IN][CONVOLUTION_LAYER_COARSE_OUT][DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN)*DIVIDE(CONVOLUTION_LAYER_FILTERS,CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_GROUPS)] = {
#else
const static weight_t weights[CONVOLUTION_LAYER_COARSE_IN][CONVOLUTION_LAYER_COARSE_OUT][DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN)*DIVIDE(CONVOLUTION_LAYER_FILTERS,CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_GROUPS)][CONVOLUTION_LAYER_KERNEL_SIZE][CONVOLUTION_LAYER_KERNEL_SIZE] = {
#endif
#include "weights.csv"
};

void convolution_layer_top(
    stream_t(data_t) in[CONVOLUTION_LAYER_COARSE_IN],
    stream_t(data_t) out[CONVOLUTION_LAYER_COARSE_OUT],
    int mode
)
{
#pragma HLS DATAFLOW

#pragma HLS interface bram port=weights

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
#pragma HLS RESOURCE variable=weights core=RAM_1P_BRAM

    convolution_layer(weights,in,out,mode);

}
