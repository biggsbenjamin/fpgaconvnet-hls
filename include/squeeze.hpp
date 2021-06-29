#ifndef SQUEEZE_HPP_
#define SQUEEZE_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    typename squeeze_t
>
void squeeze(
    stream_t(squeeze_t) in[COARSE_IN],
    stream_t(squeeze_t) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int coarse_in    = COARSE_IN;
    const unsigned int coarse_out   = COARSE_OUT;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    stream_t(squeeze_t)  channel_cache[channels];
#pragma HLS STREAM variable=channel_cache
#pragma HLS ARRAY_PARTITION variable=channel_cache complete dim=0

    dim_in_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols; pixel_index++) {
        unsigned int cache_in_index = 0;
        channel_in_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse_in);channel_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            #pragma HLS unroll region
            in_loop: for (unsigned int in_index = 0; in_index < coarse_in; in_index++) {
                channel_cache[cache_in_index+in_index].write(in[in_index].read());
            }
            cache_in_index += coarse_in;
        }
    }

    dim_out_loop: for (unsigned int pixel_index = 0; pixel_index < batch_size*rows*cols; pixel_index++) {
        unsigned int cache_out_index = 0;
        channel_out_loop: for (unsigned int channel_index = 0; channel_index < DIVIDE(channels,coarse_out);channel_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            #pragma HLS unroll region
            out_loop: for (unsigned int out_index = 0; out_index < coarse_out; out_index++) {
                out[out_index].write(channel_cache[cache_out_index+out_index].read());
            }
            cache_out_index += coarse_out;
        }
    }
}

#endif
