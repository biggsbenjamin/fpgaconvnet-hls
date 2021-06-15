#ifndef BUFF_HPP_
#define BUFF_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    bool         DROP_MODE, //true = drop on 1, false = drop on 0
>
void buff(
    stream_t(data_t) &in, //might need a float version too
    stream_t(data_t) &ctrl_drop, 
    stream_t(data_t) &out
)
{
    #pragma HLS INLINE OFF

    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const bool drop_mode          = DROP_MODE;

    //TODO add template value for buffer size
    const unsigned int buff_size  = rows*cols*channels;

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out

    data_t buff[buff_size];
    data_t drop_tmp;
    
    batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) {
        samp_in_loop: for(unsigned long pxi_index=0;pxi_index<buff_size;pxi_index++) {
            buff[pxi_index] = in.read(); //fill up the buffer
        }
        drop_tmp = ctrl_drop.read(); //wait for the ctrl signal
        if ((drop_tmp == 1.0 && !drop_mode) || (drop_tmp == 0.0 && drop_mode)) {
            samp_out_loop: for(unsigned long pxo_index=0;pxo_index<buff_size;pxo_index++) {
                buff[pxo_index] = out.write(); //write out data OR
            }
        } //move on to next sample in batch 
    }
}

#endif
