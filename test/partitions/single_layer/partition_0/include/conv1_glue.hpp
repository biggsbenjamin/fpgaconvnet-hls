#include "common.hpp"

/**
 *  GLUE FUNCTION
 */
template<int _>
void NAME_SUB(name,_glue)(
    stream_t(acc_t) in[NAME_SUB(MODULE_NAME,_COARSE_IN)][NAME_SUB(MODULE_NAME,_COARSE_OUT)],
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_COARSE_OUT)]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int coarse_in    = NAME_SUB(MODULE_NAME,_COARSE_IN);
    const unsigned int coarse_out   = NAME_SUB(MODULE_NAME,_COARSE_OUT);
    const unsigned int filters_per_coarse   = (const unsigned int) ( NAME_SUB(MODULE_NAME,_FILTERS)/NAME_SUB(MODULE_NAME,_COARSE_OUT) );

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in  complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    acc_t acc[coarse_out];
#pragma HLS ARRAY_PARTITION variable=acc complete dim=0

#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
#if NAME_SUB(MODULE_NAME,_FILTERS) != NAME_SUB(MODULE_NAME,_COARSE_OUT)
        filter_loop: for(unsigned int filter_index=0;filter_index<filters_per_coarse;filter_index++) {
#endif
            #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
            coarse_out_loop: for(unsigned int out_index=0; out_index<coarse_out; out_index++) {
#else
                unsigned int out_index=0;
#endif
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                coarse_in_loop: for(unsigned int in_index=0; in_index<coarse_in; in_index++) {
                    
                                    
                    acc_t prev = ( in_index == 0 ) ? acc_t(0) : acc[out_index] ;
                    
                    acc[out_index] = prev + in[in_index][out_index].read() ;
                
                    if( in_index == (coarse_in-1) ) {
                        out[out_index].write( acc[out_index] ) ;
                    }

                    /*
                    if( in_index == 0 ) {
                        acc[out_index] = in[in_index][out_index].read();
                    } else if ( in_index == (coarse_in-1) ) {
		        out[out_index].write( data_t( acc[out_index] + in[in_index][out_index].read() ) );
                    } else {
                        acc[out_index] += in[in_index][out_index].read();
                    }
                    */
                }
#else
		out[out_index].write( data_t( in[0][out_index].read() ) );
#endif
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
            }
#endif
#if NAME_SUB(MODULE_NAME,_FILTERS) != NAME_SUB(MODULE_NAME,_COARSE_OUT)
	}
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    }
#endif
}
