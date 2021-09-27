#include "convolution_layer_tb.hpp"
#include "convolution_layer.hpp"


void convolution_layer_top(
    stream_t(data_t) in[CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP],
    stream_t(data_t) out[CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP],
    int mode
)
{
#pragma HLS DATAFLOW

    const static weight_t weights[CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP][CONVOLUTION_LAYER_COARSE_OUT][DIVIDE(CONVOLUTION_LAYER_WEIGHTS,CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP*CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_KERNEL_SIZE_X*CONVOLUTION_LAYER_KERNEL_SIZE_Y)][CONVOLUTION_LAYER_KERNEL_SIZE_X][CONVOLUTION_LAYER_KERNEL_SIZE_Y] = {
        #include "weights.csv"
    };

//#pragma HLS interface bram port=weights

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
#pragma HLS RESOURCE variable=weights core=ROM
#pragma HLS STABLE variable=weights

    convolution_layer(weights,in,out,mode);

}
