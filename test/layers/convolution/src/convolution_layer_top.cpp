#include "convolution_layer_tb.hpp"
#include "convolution_layer.hpp"

/////////////////////
// WEIGHTS AND BIASES
/////////////////////

const static convolution_layer_weight_t weights[CONVOLUTION_LAYER_COARSE_IN][CONVOLUTION_LAYER_COARSE_OUT][DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN)*DIVIDE(CONVOLUTION_LAYER_FILTERS,CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_GROUPS)][CONVOLUTION_LAYER_KERNEL_SIZE_X][CONVOLUTION_LAYER_KERNEL_SIZE_Y] = {
        #include "weights.csv"
};

#if (CONVOLUTION_LAYER_HAS_BIAS == 1)
const static convolution_layer_biases_t biases[CONVOLUTION_LAYER_COARSE_OUT][DIVIDE(CONVOLUTION_LAYER_FILTERS,CONVOLUTION_LAYER_COARSE_OUT)] = {
        #include "biases.csv"
};
#endif

/////////////////////
// TOP LEVEL FUNCTION
/////////////////////

void convolution_layer_top(
    stream_t(convolution_layer_input_t) in[CONVOLUTION_LAYER_COARSE_IN],
    stream_t(convolution_layer_output_t) out[CONVOLUTION_LAYER_COARSE_OUT],
    int mode
)
{
#pragma HLS DATAFLOW
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS INTERFACE s_axilite port=return bundle=ctrl
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
//ROM resource specification causes HLS error, conflicting with default np_rom
//#pragma HLS RESOURCE variable=weights core=ROM
#pragma HLS STABLE variable=weights

#if (CONVOLUTION_LAYER_HAS_BIAS == 1)
#pragma HLS ARRAY_PARTITION variable=biases complete dim=1
#pragma HLS ARRAY_PARTITION variable=biases complete dim=2
//ROM resource specification causes HLS error, conflicting with default np_rom
//#pragma HLS RESOURCE variable=biases core=ROM
#pragma HLS STABLE variable=biases
#endif

    convolution_layer(  weights,
#if (CONVOLUTION_LAYER_HAS_BIAS == 1)
                        biases,
#endif
                        in,out,mode);

}
