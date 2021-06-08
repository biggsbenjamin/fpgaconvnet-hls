#include "conv_tb.hpp"
#include "conv.hpp"

void conv_top(
    stream_t(Conv_data) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
#if CONV_KERNEL_SIZE == 1
    Conv_weight weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)],
#else
    Conv_weight weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)][CONV_KERNEL_SIZE][CONV_KERNEL_SIZE],
#endif
    stream_t(Conv_acc) &out
)
{

#pragma HLS RESOURCE variable=weights core=ROM_2P_BRAM
#pragma HLS DATAFLOW

#if CONV_KERNEL_SIZE == 1
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
  		  Conv_data,
		    Conv_weight,
		    Conv_acc
    >(in[0][0],weights,out);
#else
    conv<
        CONV_BATCH_SIZE,
        CONV_ROWS,
        CONV_COLS,
        CONV_CHANNELS,
        CONV_FILTERS,
        CONV_FINE,
        CONV_KERNEL_SIZE,
		    Conv_data,
		    Conv_weight,
		    Conv_acc   
    >(in,weights,out);
#endif

}
