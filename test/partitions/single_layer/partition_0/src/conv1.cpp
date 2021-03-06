#include "conv1.hpp"

void conv1(
#if CONV1_KERNEL_SIZE == 1
    const weight_t weights[CONV1_COARSE_IN][CONV1_COARSE_OUT][DIVIDE(CONV1_CHANNELS,CONV1_COARSE_IN)*DIVIDE(CONV1_FILTERS,CONV1_COARSE_OUT*CONV1_GROUPS)],
#else
    const weight_t weights[CONV1_COARSE_IN][CONV1_COARSE_OUT][DIVIDE(CONV1_CHANNELS,CONV1_COARSE_IN)*DIVIDE(CONV1_FILTERS,CONV1_COARSE_OUT*CONV1_GROUPS)][CONV1_KERNEL_SIZE][CONV1_KERNEL_SIZE],
#endif
    stream_t(data_t) in[CONV1_COARSE_IN],
    stream_t(data_t) out[CONV1_COARSE_OUT],
    int mode
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=in depth=2
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#pragma HLS DATAFLOW
//#pragma HLS stable variable=weights

    
    stream_t(data_t) sw_out[CONV1_COARSE_IN][CONV1_KERNEL_SIZE][CONV1_KERNEL_SIZE];
    #pragma HLS STREAM variable=sw_out 
    #pragma HLS ARRAY_PARTITION variable=sw_out complete dim=0
        
    stream_t(data_t) fork_out[CONV1_COARSE_IN][CONV1_COARSE_OUT][CONV1_KERNEL_SIZE][CONV1_KERNEL_SIZE];
    #pragma HLS STREAM variable=fork_out
    #pragma HLS ARRAY_PARTITION variable=fork_out complete dim=0
        
    stream_t(acc_t) conv_out[CONV1_COARSE_IN][CONV1_COARSE_OUT];
    #pragma HLS STREAM variable=conv_out
    #pragma HLS ARRAY_PARTITION variable=conv_out complete dim=0
        

    for(unsigned int i=0;i<CONV1_COARSE_IN;i++) {
        #pragma HLS unroll

        
        conv1_sliding_window<0>(in[i],sw_out[i]);


        
        conv1_fork<0>(sw_out[i],fork_out[i]);



        for(unsigned int j=0;j<CONV1_COARSE_OUT;j++) {
            #pragma HLS unroll
            
            
            conv1_conv<0>(fork_out[i][j],weights[i][j],conv_out[i][j]);


            

        }
    }

    
    conv1_glue<0>(conv_out,out);



}

