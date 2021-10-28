#include "common.hpp"

/**
 *  BIAS FUNCTION
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int COARSE_OUT,
    typename bias_data_t,
    typename bias_weight_t
>
void bias(
    stream_t(bias_data_t) in[COARSE_OUT],
    const bias_weight_t bias[FILTERS],
    stream_t(bias_data_t) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int filters      = FILTERS;
    const unsigned int coarse_out   = COARSE_OUT;
    //const unsigned int coarse_group = COARSE_GROUP;
    const unsigned int filters_per_coarse   = DIVIDE(filters,coarse_out);//*coarse_group);

#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=out

    pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        filter_loop: for(unsigned int filter_index=0;filter_index<filters_per_coarse;filter_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_out_loop: for(unsigned int out_index=0; out_index<coarse_out; out_index++) {
                in[out_index].read() + bias 
	            out.write( in.read() + bias[filter_index] );
            }
	    }
    }
