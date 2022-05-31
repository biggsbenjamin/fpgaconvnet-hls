#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    bool         DROP_MODE, //true = drop on 1, false = drop on 0
    typename     buffer_t
>
void buffer(
    stream_t(buffer_t) &in, //might need a float version too
    stream_t(buffer_t) &ctrl_drop, 
    stream_t(buffer_t) &out
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

    buffer_t buff[buff_size];
    #pragma HLS resource variable=buff core=RAM_2P_BRAM
    buffer_t drop_tmp;
    
    batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) {
        samp_in_loop: for(unsigned long pxi_index=0;pxi_index<buff_size;pxi_index++) {
#pragma HLS PIPELINE II=1 rewind 
            while (in.empty()) {}
            //buff[pxi_index] = in.read(); //fill up the buffer
            in.read_nb(buff[pxi_index]);
        }
        while (ctrl_drop.empty()) {}
        //drop_tmp = ctrl_drop.read(); //wait for the ctrl signal
        ctrl_drop.read_nb(drop_tmp);
        if ((drop_tmp == 1.0 && !drop_mode) || (drop_tmp == 0.0 && drop_mode)) {
            samp_out_loop: for(unsigned long pxo_index=0;pxo_index<buff_size;pxo_index++) {
#pragma HLS PIPELINE II=1 rewind 
                out.write(buff[pxo_index]); //write out data OR
            }
        } //move on to next sample in batch 
    }
}

#endif
