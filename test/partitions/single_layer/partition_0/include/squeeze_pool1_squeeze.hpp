#include "common.hpp"

template<int _>
void NAME_SUB(name,_squeeze)(
    stream_t(data_t) in[NAME_SUB(MODULE_NAME,_COARSE_IN)],
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_COARSE_OUT)]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int coarse_in    = NAME_SUB(MODULE_NAME,_COARSE_IN);
    const unsigned int coarse_out   = NAME_SUB(MODULE_NAME,_COARSE_OUT);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(data_t)  channel_cache[channels];
#pragma HLS STREAM variable=channel_cache
#pragma HLS ARRAY_PARTITION variable=channel_cache complete dim=0
    
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)
    dim_in_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols;pixel_index++) {
#endif
        unsigned int cache_in_index = 0;
#if NAME_SUB(MODULE_NAME,_CHANNELS) / NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
        channel_in_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse_in);channel_index++) { 
#else
            unsigned int channel_index = 0; 
#endif
            #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
            in_loop: for (unsigned int in_index = 0; in_index < coarse_in; in_index++) {
                channel_cache[cache_in_index].write(in[in_index].read());
                cache_in_index++;
            }
#else
            channel_cache[channel_index].write(in[0].read());
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS) / NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    }
#endif

#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    dim_out_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols;pixel_index++) {
#endif
        unsigned int cache_out_index = 0;
#if NAME_SUB(MODULE_NAME,_CHANNELS) / NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
        channel_out_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse_out);channel_index++) { 
#else
            unsigned int channel_index = 0; 
#endif
            #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
            out_loop: for (unsigned int out_index = 0; out_index < coarse_out; out_index++) {
                out[out_index].write(channel_cache[cache_out_index].read());
                cache_out_index++;
            }
#else
            out[0].write(channel_cache[channel_index].read());
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS) / NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    }
#endif
}
