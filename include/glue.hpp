#ifndef GLUE_HPP_
#define GLUE_HPP_

#include "common.hpp"

/**
 *  GLUE FUNCTION
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int FILTERS,
    unsigned int COARSE_IN,
    unsigned int COARSE_OUT,
    typename glue_acc,
    typename glue_data
>
void glue(
    stream_t(glue_acc) in[COARSE_IN][COARSE_OUT],
    stream_t(glue_data) out[COARSE_OUT]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int filters      = FILTERS;
    const unsigned int coarse_in    = COARSE_IN;
    const unsigned int coarse_out   = COARSE_OUT;
    const unsigned int filters_per_coarse   = DIVIDE(filters,coarse_out);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    glue_acc acc[coarse_out];
#pragma HLS ARRAY_PARTITION variable=acc complete dim=0

    pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        filter_loop: for(unsigned int filter_index=0;filter_index<filters_per_coarse;filter_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            #pragma HLS unroll region
            coarse_out_loop: for(unsigned int out_index=0; out_index<coarse_out; out_index++) {
                coarse_in_loop: for(unsigned int in_index=0; in_index<coarse_in; in_index++) {
                    // update accumulation cache
                    glue_acc prev = ( in_index == 0 ) ? glue_acc (0) : acc[out_index] ;
                    acc[out_index] = prev + in[in_index][out_index].read() ;
                    // write to output stream 
                    if( in_index == (coarse_in-1) ) {
                        out[out_index].write(glue_data(acc[out_index]) ) ;
                    }
                }
            }
	}
    }
}

#endif
