#ifndef EXIT_SELECT_HPP_
#define EXIT_SELECT_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS
>
void exit_select(
    stream_t(data_t) &in_early, 
    stream_t(uint32_t) &early_batch_id, 
    stream_t(data_t) &in_late, 
    stream_t(uint32_t) &late_batch_id, 
    stream_t(data_t) &out,
    stream_t(uint32_t) &out_batch_id 
)
{
    #pragma HLS INLINE OFF

    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const unsigned int size       = ROWS*COLS*CHANNELS;

    #pragma HLS STREAM variable=in_early
    #pragma HLS STREAM variable=early_batch_id
    #pragma HLS STREAM variable=in_late
    #pragma HLS STREAM variable=late_batch_id
    #pragma HLS STREAM variable=out
    #pragma HLS STREAM variable=out_batch_id

    uint32_t batch_id;
    
    batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) {
        //assumption is that when a batch_id arrives, the corresponding stream will be available
        //otherwise, need to read in the ctrl signal
       
        while (in_early.empty() && in_late.empty()) {}; //stall to prevent batch size incrementing

        if (!in_early.empty() && !early_batch_id.empty()){
            samp_loop_early: for(unsigned long pxo_index=0;pxo_index<size;pxo_index++) {
                out.write(early_batch_id.read());
                out.write(in_early.read()); 
            }
        
        } else if (!in_late.empty() && !late_batch_id.empty()) {
            samp_loop_late: for(unsigned long pxo_index=0;pxo_index<size;pxo_index++) {
                out.write(late_batch_id.read());
                out.write(in_late.read()); 
            }
        }
    }
}

#endif

