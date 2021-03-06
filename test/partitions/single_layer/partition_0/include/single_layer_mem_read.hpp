#include "common.hpp"

template<typename type_t>
void NAME_SUB(name,_mem_read)(
    volatile mem_int in_hw[NAME_SUB(MODULE_NAME,_PORTS_IN)][NAME_SUB(MODULE_NAME,_BATCH_SIZE)*NAME_SUB(MODULE_NAME,_ROWS_IN)*NAME_SUB(MODULE_NAME,_COLS_IN)*DIVIDE(NAME_SUB(MODULE_NAME,_CHANNELS_IN),NAME_SUB(MODULE_NAME,_STREAMS_IN))],
    stream_t(type_t) in[NAME_SUB(MODULE_NAME,_STREAMS_IN)]
)
{

#pragma HLS INLINE OFF 

    const unsigned batch_size           = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned rows_in              = NAME_SUB(MODULE_NAME,_ROWS_IN);
    const unsigned cols_in              = NAME_SUB(MODULE_NAME,_COLS_IN);
    const unsigned channels_in          = NAME_SUB(MODULE_NAME,_CHANNELS_IN);
    const unsigned ports_in             = NAME_SUB(MODULE_NAME,_PORTS_IN);
    const unsigned streams_in           = NAME_SUB(MODULE_NAME,_STREAMS_IN);
    const unsigned channels_per_stream  = DIVIDE(channels_in,streams_in);
    const unsigned dma_channels         = DIVIDE(DMA_WIDTH,DATA_WIDTH);

#pragma HLS STREAM variable=in depth=256 
#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    read_loop: for (unsigned long size_index=0; size_index < batch_size*rows_in*cols_in*channels_per_stream; size_index++) {
        #pragma HLS PIPELINE II=1
                   
        mem_int port_cache[ports_in];
#pragma HLS ARRAY_PARTITION variable=port_cache complete dim=0

        port_read_loop: for (unsigned port_index=0; port_index < ports_in; port_index++) {
            //if ( streams_in > port_index*dma_channels ) {
                port_cache[port_index] = in_hw[port_index][size_index];
                // mem_int tmp = in_hw[port_index][size_index];
            //}
        }
#if NAME_SUB(MODULE_NAME,_STREAMS_IN) != 1
        stream_loop: for (unsigned int stream_index=0; stream_index < streams_in; stream_index++) {
#else
            unsigned int stream_index = 0;
#endif

            //
            unsigned int port_index = (int)(stream_index/dma_channels);    
       
            //     
            type_t stream_cache = 0;
            stream_cache.range() = ( ( ( port_cache[port_index] ) >> ( ( stream_index%dma_channels ) * DATA_WIDTH ) ) & BIT_MASK );
            // write to the stream
            in[stream_index].write(stream_cache);

#if NAME_SUB(MODULE_NAME,_STREAMS_IN) != 1
        }
#endif
    }
}

