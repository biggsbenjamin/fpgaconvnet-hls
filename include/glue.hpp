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
    unsigned int COARSE_GROUP
>
void glue(
    stream_t(acc_t) in[COARSE_IN*COARSE_GROUP][COARSE_OUT],
    stream_t(data_t) out[COARSE_OUT*COARSE_GROUP]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int filters      = FILTERS;
    const unsigned int coarse_in    = COARSE_IN;
    const unsigned int coarse_out   = COARSE_OUT;
    const unsigned int coarse_group = COARSE_GROUP;
    const unsigned int filters_per_coarse   = DIVIDE(filters,coarse_out*coarse_group);

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    acc_t acc[coarse_out*coarse_group];
#pragma HLS ARRAY_PARTITION variable=acc complete dim=0

    pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        filter_loop: for(unsigned int filter_index=0;filter_index<filters_per_coarse;filter_index++) {
            #pragma HLS loop_flatten
            #pragma HLS pipeline II=1 rewind
            #pragma HLS unroll region
            coarse_group_loop: for(unsigned int group_index=0; group_index<coarse_group; group_index++) {
                coarse_out_loop: for(unsigned int out_index=0; out_index<coarse_out; out_index++) {
                    coarse_in_loop: for(unsigned int in_index=0; in_index<coarse_in; in_index++) {
                        // update accumulation cache
                        acc_t prev = ( in_index == 0 ) ? acc_t(0) : acc[group_index*coarse_out+out_index] ;
                        acc[group_index*coarse_out+out_index] = prev + in[group_index*coarse_in+in_index][out_index].read() ;
                        
                        // write to output stream 
                        if( in_index == (coarse_in-1) ) {
                            out[group_index*coarse_out+out_index].write( data_t(acc[group_index*coarse_out+out_index]) ) ;
                        }
                    }
                }
            }
	}
    }
}

#endif
