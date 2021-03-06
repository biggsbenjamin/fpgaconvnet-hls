#include "common.hpp"

/**
  * POOL 
  */
template<int _>
void NAME_SUB(name,_pool)(
    stream_t(data_t)  in[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(data_t) &out
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
 
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out
#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    data_t cache;
    #pragma HLS DEPENDENCE variable=cache RAW intra true

    pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols*channels;pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
        pool_loop_1: for (unsigned char k1 = 0; k1 < kernel_size; k1++) {
            pool_loop_2: for (unsigned char k2 = 0; k2 < kernel_size; k2++) {
                if (k1 == 0 && k2 == 0) {
                    cache = in[k1][k2].read();
                }
                else {
                    data_t tmp = in[k1][k2].read();
                    cache = (cache > tmp ) ? cache : tmp ;
                }
            }
        }
        out.write(cache);
    }
}
