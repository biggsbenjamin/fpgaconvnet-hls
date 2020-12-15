#include "common.hpp"

/**
 *  CONVOLUTION FUNCTION
 */

template<int _>
void NAME_SUB(name,_stream_intr)(
    stream_t(data_t) in[NAME_SUB(MODULE_NAME,_COARSE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#if NAME_SUB(MODULE_NAME,_FINE) == 1
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_COARSE)],
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_COARSE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_COARSE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#endif
)
{

#pragma HLS INLINE OFF

    const unsigned int coarse       = NAME_SUB(MODULE_NAME,_COARSE);
    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
    const unsigned int fine         = NAME_SUB(MODULE_NAME,_FINE);
 
#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=window_stream 
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0

    // INTERLEAVING LOOP
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)  
    intr_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS) > 1
        intr_channel_loop: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
#else
        unsigned int channel_index=0;
#endif
#if NAME_SUB(MODULE_NAME,_FINE) == 1
            intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                    #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_COARSE) > 1
                    coarse_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
#else
                        unsigned int coarse_index=0;
#endif
                        window_stream[coarse_index].write(in[coarse_index][k1][k2].read());
#if NAME_SUB(MODULE_NAME,_COARSE) > 1
                    }
#endif
                }
            }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
            intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                #pragma HLS PIPELINE II=1 rewind
                intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
#if NAME_SUB(MODULE_NAME,_COARSE) > 1
                    coarse_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
#else
                        unsigned int coarse_index=0;
#endif
                        window_stream[coarse_index][k2].write(in[coarse_index][k1][k2].read());
#if NAME_SUB(MODULE_NAME,_COARSE) > 1
                    }
#endif
                }
            }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
            #pragma HLS PIPELINE II=1 rewind
            intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
#if NAME_SUB(MODULE_NAME,_COARSE) > 1
                    coarse_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
#else
                        unsigned int coarse_index=0;
#endif
                        window_stream[coarse_index][k1][k2].write(in[coarse_index][k1][k2].read());
#if NAME_SUB(MODULE_NAME,_COARSE) > 1
                    }
#endif
                }
            }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS) > 1
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)  
    }
#endif
}

