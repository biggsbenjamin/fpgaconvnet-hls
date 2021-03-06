#include "common.hpp"

/**
 *  FORK FUNCTION
 */
template<int _>
void NAME_SUB(name,_fork)(
#if NAME_SUB(MODULE_NAME,_KERNEL_SIZE) != 1
    stream_t(data_t) in[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_COARSE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#else
    stream_t(data_t) &in,
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_COARSE)]
#endif
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int coarse       = NAME_SUB(MODULE_NAME,_COARSE);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);

#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=out

#if NAME_SUB(MODULE_NAME,_KERNEL_SIZE) != 1
#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#endif
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    data_t local_cache[kernel_size][kernel_size];
    #pragma HLS ARRAY_PARTITION variable=local_cache complete dim=0
    #pragma HLS DEPENDENCE variable=local_cache RAW intra true

#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) ||(NAME_SUB(MODULE_NAME,_CHANNELS) > 1) 
    pixel_loop: for (unsigned long pixel_index = 0; pixel_index < batch_size*rows*cols*channels; pixel_index++) {
#endif
#pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_KERNEL_SIZE) != 1
        k1_loop: for (unsigned char k1 = 0; k1 < kernel_size; k1++) {
            k2_loop: for (unsigned char k2 = 0; k2 < kernel_size; k2++) {
                coarse_loop: for(unsigned int coarse_index=0;coarse_index < coarse; coarse_index++) {
                    if(coarse_index == 0) {
                        DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels*kernel_size*kernel_size)
                        local_cache[k1][k2] = in[k1][k2].read();
                    }
                    out[coarse_index][k1][k2].write(local_cache[k1][k2]);
                }
            }
        }
#else
        coarse_loop: for(unsigned int coarse_index=0;coarse_index < coarse; coarse_index++) {
            if(coarse_index == 0) {
                DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                local_cache[0][0] = in.read();
            }
            out[coarse_index].write(local_cache[0][0]);
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) ||(NAME_SUB(MODULE_NAME,_CHANNELS) > 1) 
    }
#endif
}
