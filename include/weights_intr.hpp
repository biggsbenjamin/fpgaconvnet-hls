#include "common.hpp"

/**
 *  CONVOLUTION FUNCTION
 */

template<int _>
void NAME_SUB(name,_weights_intr)(
    const weight_t weights[NAME_SUB(MODULE_NAME,_COARSE_IN)][NAME_SUB(MODULE_NAME,_COARSE_OUT)][NAME_SUB(MODULE_NAME,_CHANNELS)*NAME_SUB(MODULE_NAME,_FILTERS)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#if NAME_SUB(MODULE_NAME,_FINE) == 1
    stream_t(weight_t) &weight_stream
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(weight_t) weight_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(weight_t) weight_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#endif
)
{

#pragma HLS INLINE OFF

    const unsigned int coarse_in    = NAME_SUB(MODULE_NAME,_COARSE_IN);
    const unsigned int coarse_out   = NAME_SUB(MODULE_NAME,_COARSE_OUT);
    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int filters      = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
    const unsigned int fine         = NAME_SUB(MODULE_NAME,_FINE);
 
#pragma HLS STREAM variable=weight_stream 
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0

#pragma HLS ARRAY_PARTITION variable=weights complete dim=1
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2

#if NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=weights complete dim=4
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=weights complete dim=4
#pragma HLS ARRAY_PARTITION variable=weights complete dim=5
#endif

    // INTERLEAVING LOOP
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)  
    pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
        unsigned int weight_index = 0;
#if NAME_SUB(MODULE_NAME,_CHANNELS)*NAME_SUB(MODULE_NAME,_FILTERS) != 1
        weight_loop: for(unsigned int weight_index=0;weight_index<channels*filters;weight_index++) {
#else
        unsigned int weight_index=0;
#endif
#if NAME_SUB(MODULE_NAME,_FINE) == 1
            k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                    #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                    coarse_in_loop: for(unsigned int coarse_in_index=0;coarse_in_index<coarse_in;coarse_in_index++) {
#else
                        unsigned int coarse_in_index=0;                                           
#endif                                        
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
                        coarse_out_loop: for(unsigned int coarse_out_index=0;coarse_out_index<coarse_out;coarse_out_index++) {
#else
                            unsigned int coarse_out_index=0;                                           
#endif
                            weight_stream[coarse_in_index][coarse_out_index].write(weights[coarse_in_index][coarse_out_index][weight_index][k1][k2]);
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
                        }
#endif                                    
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                    }
#endif                        
                }
            }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
            k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                #pragma HLS PIPELINE II=1 rewind
                k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                    coarse_in_loop: for(unsigned int coarse_in_index=0;coarse_in_index<coarse_in;coarse_in_index++) {
#else
                        unsigned int coarse_in_index=0;                                           
#endif                                        
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
                        coarse_out_loop: for(unsigned int coarse_out_index=0;coarse_out_index<coarse_out;coarse_out_index++) {
#else
                            unsigned int coarse_out_index=0;                                           
#endif
                            weight_stream[coarse_in_index][coarse_out_index][k1].write(weights[coarse_in_index][coarse_out_index][weight_index][k1][k2]);
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
                        }
#endif                                    
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                    }
#endif                        
                }
            }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
            #pragma HLS PIPELINE II=1 rewind
            k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                    coarse_in_loop: for(unsigned int coarse_in_index=0;coarse_in_index<coarse_in;coarse_in_index++) {
#else
                        unsigned int coarse_in_index=0;                                           
#endif                                        
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
                        coarse_out_loop: for(unsigned int coarse_out_index=0;coarse_out_index<coarse_out;coarse_out_index++) {
#else
                            unsigned int coarse_out_index=0;                                           
#endif
                            weight_stream[coarse_in_index][coarse_out_index][k1][k2].write(weights[coarse_in_index][coarse_out_index][weight_index][k1][k2]);
#if NAME_SUB(MODULE_NAME,_COARSE_OUT) > 1
                        }
#endif                                    
#if NAME_SUB(MODULE_NAME,_COARSE_IN) > 1
                    }
#endif                        
                }
            }
#if NAME_SUB(MODULE_NAME,_CHANNELS)*NAME_SUB(MODULE_NAME,_FILTERS) != 1
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)  
    }
#endif
}

