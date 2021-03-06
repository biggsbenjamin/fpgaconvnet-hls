#include "single_layer_top.hpp"


static weight_t conv1_weights[CONV1_COARSE_IN][CONV1_COARSE_OUT][DIVIDE(CONV1_WEIGHTS,CONV1_COARSE_IN*CONV1_COARSE_OUT*CONV1_KERNEL_SIZE*CONV1_KERNEL_SIZE)][CONV1_KERNEL_SIZE][CONV1_KERNEL_SIZE] = { 
#include "conv1_weights_0.csv" 
};


#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
void reload_weights(
    int weights_reloading_index,
    volatile mem_int wr_hw[SINGLE_LAYER_PORTS_WR][SINGLE_LAYER_SIZE_WR],
#if SINGLE_LAYER_WR_KERNEL_SIZE == 1
    weight_t weights[SINGLE_LAYER_WR_COARSE_IN][SINGLE_LAYER_WR_COARSE_OUT][DIVIDE(SINGLE_LAYER_WR_WEIGHTS,SINGLE_LAYER_WR_COARSE_IN*SINGLE_LAYER_WR_COARSE_OUT*SINGLE_LAYER_WR_KERNEL_SIZE*SINGLE_LAYER_WR_KERNEL_SIZE)]
#else
    weight_t weights[SINGLE_LAYER_WR_COARSE_IN][SINGLE_LAYER_WR_COARSE_OUT][DIVIDE(SINGLE_LAYER_WR_WEIGHTS,SINGLE_LAYER_WR_COARSE_IN*SINGLE_LAYER_WR_COARSE_OUT*SINGLE_LAYER_WR_KERNEL_SIZE*SINGLE_LAYER_WR_KERNEL_SIZE)][SINGLE_LAYER_WR_KERNEL_SIZE][SINGLE_LAYER_WR_KERNEL_SIZE]
#endif
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS stable variable=weights

    // stream init
    stream_t(weight_t) wr[SINGLE_LAYER_STREAMS_WR];
#pragma HLS STREAM variable=wr 
#pragma HLS ARRAY_PARTITION variable=wr complete dim=0

    single_layer_wr_mem_read<weight_t>(wr_hw,wr);
    single_layer_wr<0>(wr[0],weights);
}
#endif

void process(
    int weights_reloading_index,
    volatile mem_int in_hw[SINGLE_LAYER_PORTS_IN][SINGLE_LAYER_SIZE_IN],
    volatile mem_int out_hw[SINGLE_LAYER_PORTS_OUT][SINGLE_LAYER_SIZE_OUT]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW


#pragma HLS ARRAY_PARTITION variable=conv1_weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=conv1_weights complete dim=2
#pragma HLS RESOURCE variable=conv1_weights core=RAM
//#pragma HLS STREAM variable=conv1_weights off
#pragma HLS STABLE variable=conv1_weights
   


    stream_t(data_t) out[4];
#pragma HLS STREAM variable=out 
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(data_t) pool1_squeeze_pool1[5];
#pragma HLS STREAM variable=pool1_squeeze_pool1 
#pragma HLS ARRAY_PARTITION variable=pool1_squeeze_pool1 complete dim=0

    stream_t(data_t) in[1];
#pragma HLS STREAM variable=in 
#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    stream_t(data_t) conv1_pool1[5];
#pragma HLS STREAM variable=conv1_pool1 
#pragma HLS ARRAY_PARTITION variable=conv1_pool1 complete dim=0


    // Read in from memory
    single_layer_mem_read<data_t>(in_hw,in);

    int mode = 0;

    conv1(conv1_weights, in, conv1_pool1, mode);
    pool1(conv1_pool1, pool1_squeeze_pool1, mode);
    squeeze_pool1(pool1_squeeze_pool1, out, mode);


    single_layer_mem_write<data_t>(weights_reloading_index,out,out_hw);

}

void fpgaconvnet_ip(
    int mode,
    int weights_reloading_index,
#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
    volatile mem_int wr_hw[SINGLE_LAYER_PORTS_WR][SINGLE_LAYER_SIZE_WR],
#endif
    volatile mem_int in_hw[SINGLE_LAYER_PORTS_IN][SINGLE_LAYER_SIZE_IN],
    volatile mem_int out_hw[SINGLE_LAYER_PORTS_OUT][SINGLE_LAYER_SIZE_OUT]
)
{
#pragma HLS INTERFACE s_axilite port=return                     bundle=ctrl  
#pragma HLS INTERFACE s_axilite port=mode                       bundle=ctrl  
#pragma HLS INTERFACE s_axilite port=weights_reloading_index    bundle=ctrl  

#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
#pragma HLS ARRAY_PARTITION variable=wr_hw  complete dim=1
#endif
#pragma HLS ARRAY_PARTITION variable=in_hw  complete dim=1
#pragma HLS ARRAY_PARTITION variable=out_hw complete dim=1

#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
    const unsigned size_wr  = SINGLE_LAYER_SIZE_WR ;
#endif
    const unsigned size_in  = SINGLE_LAYER_SIZE_IN ;
    const unsigned size_out = SINGLE_LAYER_SIZE_OUT;

#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
#pragma HLS INTERFACE m_axi port=wr_hw  offset=slave depth=size_wr  num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=256 max_write_burst_length=256 name=fpgaconvnet_wr  bundle=fpgaconvnet_port_wr 
#endif

#pragma HLS INTERFACE m_axi port=in_hw  offset=slave depth=size_in  num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=256 max_write_burst_length=256 name=fpgaconvnet_in  bundle=fpgaconvnet_port_in 

#pragma HLS INTERFACE m_axi port=out_hw offset=slave depth=size_out num_read_outstanding=1 num_write_outstanding=1 max_read_burst_length=256 max_write_burst_length=256 name=fpgaconvnet_out bundle=fpgaconvnet_port_out 


    #pragma HLS DATAFLOW
    if ( mode == 0 ) {
        process(weights_reloading_index,in_hw,out_hw);
    } else if ( mode == 1 ) {
#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
        reload_weights(weights_reloading_index,wr_hw,conv1_weights);
#endif
    }

}
