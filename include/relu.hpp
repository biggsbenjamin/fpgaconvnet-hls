#include "common.hpp"

/**
 *  RECTIFIED LINEAR UNIT (RELU) FUNCTION
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
>
void relu(
    stream_t(data_t)  &in,
    stream_t(data_t)  &out
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
 
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
