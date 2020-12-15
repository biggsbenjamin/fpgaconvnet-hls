#include "common.hpp"

/**
 *  RECTIFIED LINEAR UNIT (RELU) FUNCTION
 */
template<int _>
void NAME_SUB(name,_relu)(
    stream_t(data_t)   &in,
    stream_t(data_t)  &out
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
 
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

    for(unsigned long pixel_index=0 ; pixel_index < batch_size*rows*cols*channels ; pixel_index++) {
#pragma HLS PIPELINE II=1 rewind
        data_t tmp = in.read();
        if(tmp[DATA_WIDTH-1])
            out.write(0.0);
        else
            out.write(tmp);
    }
}
