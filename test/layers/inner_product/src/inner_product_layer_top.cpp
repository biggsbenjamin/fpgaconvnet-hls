//NOT automatically generated
#include "inner_product_layer_tb.hpp"
#include "inner_product_layer.hpp"

/////////////////////
// WEIGHTS AND BIASES
/////////////////////

const static inner_product_layer_weight_t weights[INNER_PRODUCT_LAYER_COARSE_IN][INNER_PRODUCT_LAYER_COARSE_OUT][DIVIDE(INNER_PRODUCT_LAYER_CHANNELS,INNER_PRODUCT_LAYER_COARSE_IN)*DIVIDE(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT*INNER_PRODUCT_LAYER_GROUPS)][INNER_PRODUCT_LAYER_KERNEL_SIZE_X][INNER_PRODUCT_LAYER_KERNEL_SIZE_Y] = {
        #include "weights.csv"
};

#if (INNER_PRODUCT_LAYER_HAS_BIAS == 1)
const static inner_product_layer_biases_t biases[INNER_PRODUCT_LAYER_COARSE_OUT][DIVIDE(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT)] = {
        #include "biases.csv"
};
#endif

/////////////////////
// TOP LEVEL FUNCTION
/////////////////////

void inner_product_layer_top(
  stream_t(inner_product_layer_input_t) in[INNER_PRODUCT_LAYER_COARSE_IN],
  stream_t(inner_product_layer_output_t) out[INNER_PRODUCT_LAYER_COARSE_OUT],
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

#if (INNER_PRODUCT_LAYER_HAS_BIAS == 1)
#pragma HLS ARRAY_PARTITION variable=biases complete dim=1
#pragma HLS ARRAY_PARTITION variable=biases complete dim=2
//ROM resource specification causes HLS error, conflicting with default np_rom
//#pragma HLS RESOURCE variable=biases core=ROM
#pragma HLS STABLE variable=biases
#endif

    inner_product_layer(weights,
#if (INNER_PRODUCT_LAYER_HAS_BIAS == 1)
                        biases,
#endif
            in,out,mode);

}
