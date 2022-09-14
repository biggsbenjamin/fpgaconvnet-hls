#ifndef EXIT_MERGE_HPP_
#define EXIT_MERGE_HPP_

#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int EXITS,
    typename em_t
>
void exit_merge(
    stream_t(em_t) in[EXITS], 
    stream_t(em_t) &out
)
{
#pragma HLS INLINE OFF

    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const unsigned int exits      = EXITS;
    const unsigned int fm_size    = ROWS*COLS*CHANNELS;

#pragma HLS STREAM variable=in depth=fm_size
#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS STREAM variable=out

    batch_loop: 
    for(unsigned long b_index=0;b_index<batch_size;b_index++) {
        exit_loop: 
        for(unsigned int ex_idx=0;ex_idx<exits;ex_idx++){
        #pragma HLS UNROLL
             if (!in[ex_idx].empty()){
                samp_loop: 
                for(unsigned long pxo_index=0;pxo_index<fm_size;pxo_index++) {
                #pragma HLS PIPELINE II=1 rewind
                    out.write(in[ex_idx].read()); 
                } 
            }   
        }

        /*if (!in_early.empty()){
            samp_loop_early: for(unsigned long pxo_index=0;pxo_index<fm_size;pxo_index++) {
                //out.write(early_batch_id.read());
                out.write(in_early.read()); 
            }
        
        } else if (!in_late.empty()) {
            samp_loop_late: for(unsigned long pxo_index=0;pxo_index<fm_size;pxo_index++) {
                //out.write(late_batch_id.read());
                out.write(in_late.read()); 
            }
        }*/
    }
}

#endif

