#include "common.hpp"

template<typename type_t>
void NAME_SUB(name,_mem_write)(
    int weights_reloading_index,
    stream_t(type_t) out[NAME_SUB(MODULE_NAME,_STREAMS_OUT)],
    volatile mem_int out_hw[NAME_SUB(MODULE_NAME,_PORTS_OUT)][NAME_SUB(MODULE_NAME,_BATCH_SIZE)*NAME_SUB(MODULE_NAME,_ROWS_OUT)*NAME_SUB(MODULE_NAME,_COLS_OUT)*DIVIDE(NAME_SUB(MODULE_NAME,_CHANNELS_OUT),NAME_SUB(MODULE_NAME,_STREAMS_OUT))*NAME_SUB(MODULE_NAME,_WEIGHTS_RELOADING_FACTOR)]
)
{

#pragma HLS INLINE OFF 
#pragma HLS INTERFACE ap_ctrl_chain port=return

    const unsigned batch_size               = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned rows_out                 = NAME_SUB(MODULE_NAME,_ROWS_OUT);
    const unsigned cols_out                 = NAME_SUB(MODULE_NAME,_COLS_OUT);
    const unsigned channels_out             = NAME_SUB(MODULE_NAME,_CHANNELS_OUT);
    const unsigned ports_out                = NAME_SUB(MODULE_NAME,_PORTS_OUT);
    const unsigned streams_out              = NAME_SUB(MODULE_NAME,_STREAMS_OUT);
    const unsigned weights_reloading_factor = NAME_SUB(MODULE_NAME,_WEIGHTS_RELOADING_FACTOR);
    //const unsigned weights_reloading_index  = NAME_SUB(MODULE_NAME,_WEIGHTS_RELOADING_INDEX );
    //const unsigned channels_per_coarse      = DIVIDE(channels_out,coarse_out*weights_reloading_factor);
    const unsigned channels_per_stream      = DIVIDE(channels_out,streams_out);
    const unsigned dma_channels             = DIVIDE(DMA_WIDTH,DATA_WIDTH);

#pragma HLS STREAM variable=out depth=256
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    pixel_loop: for(unsigned pixel_index=0;pixel_index<batch_size*rows_out*cols_out;pixel_index++) {
#if NAME_SUB(MODULE_NAME,_CHANNELS_OUT) > NAME_SUB(MODULE_NAME,_STREAMS_OUT)
        channel_loop: for(unsigned channel_index=0;channel_index<channels_per_stream;channel_index++) { 
#else
            unsigned channel_index = 0;
#endif
            #pragma HLS PIPELINE II=1
            mem_int port_cache[ports_out] = {0};
            #pragma HLS dependence variable=port_cache intra RAW true
#if NAME_SUB(MODULE_NAME,_STREAMS_OUT) != 1
            streams_loop: for(unsigned stream_index=0; stream_index<streams_out; stream_index++) {
#else
                unsigned stream_index = 0;
#endif
                type_t stream_cache = out[stream_index].read();
                unsigned int port_index = (int) (stream_index/dma_channels);
                port_cache[port_index] |= ( ( stream_cache.range() & BIT_MASK ) << ( ( stream_index%dma_channels ) * DATA_WIDTH ) );
#if NAME_SUB(MODULE_NAME,_STREAMS_OUT) != 1
            }
#endif
            port_write_loop: for (unsigned port_index=0; port_index < ports_out; port_index++) {
                int out_index = pixel_index*channels_per_stream*weights_reloading_factor+weights_reloading_index*channels_per_stream+channel_index;
                out_hw[port_index][out_index] = port_cache[port_index];
            }
#if NAME_SUB(MODULE_NAME,_CHANNELS_OUT) > NAME_SUB(MODULE_NAME,_STREAMS_OUT)
        }
#endif
    }
}

