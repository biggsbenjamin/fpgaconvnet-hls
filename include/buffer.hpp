#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include "common.hpp"

/* adding this between in, ctr_in and the conditional layer 
 * so there can be buffering */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    typename     intr_t
>
void buffer_buff(
    stream_t(intr_t) &in, //might need a float version too
    //stream_t(b_bool_t) &ctrl_drop, 
    stream_t(intr_t) &out
)
{
    #pragma HLS INLINE OFF

    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;

    const unsigned int buff_size  = rows*cols*channels;

    intr_t cache;

    full_loop: 
    for(unsigned long b_index=0;b_index<batch_size*buff_size;b_index++) {
        #pragma HLS PIPELINE II=1
        cache = in.read();
        out.write( cache );
    }
}

/* part of the buffer module that does the dropping */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    bool         DROP_MODE, //true = drop on 1, false = drop on 0
    typename     buffer_t
>
void buffer_conditional(
    stream_t(buffer_t) &in, //might need a float version too
    stream_t(b_bool_t) &ctrl_drop, 
    stream_t(buffer_t) &out
)
{
    #pragma HLS INLINE OFF

    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const bool drop_mode          = DROP_MODE;

    const unsigned int buff_size  = rows*cols*channels;

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out
    #pragma HLS STREAM variable=ctrl_drop

    b_bool_t drop_tmp;
    buffer_t data_tmp;

    batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) {
        samp_loop: for(unsigned long pxi_index=0;pxi_index<buff_size;pxi_index++) {
        #pragma HLS PIPELINE II=1 

            if(pxi_index == 0) {
                //wait for ctrl signal per batch
                drop_tmp = ctrl_drop.read();
            }
            //read in intermediate fm every time
            data_tmp = in.read();

            if ((drop_tmp.data == 1 && !drop_mode) || (drop_tmp.data == 0 && drop_mode)) {
                assert(drop_tmp.batchid == data_tmp.batchid);
                //either drop or pass thru data read
                out.write(data_tmp);
            }
            //else do nothing with data 
        }
    }

}

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
    stream_t(b_bool_t) &ctrl_drop, 
    stream_t(buffer_t) &out
)
{
    #pragma HLS DATAFLOW
    #pragma HLS INLINE OFF

    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const bool drop_mode          = DROP_MODE;

    //TODO add template value for buffer size
    const unsigned int buff_size  = rows*cols*channels;
    // FIXME this is huge buffering and not realistic
    const unsigned int in_size  = batch_size*buff_size;

    stream_t(b_bool_t) cd_buff;
    stream_t(buffer_t) in_buff;

    //should prevent stalling first exit FIXME limited to one sample bc other stream
    #pragma HLS STREAM variable=cd_buff depth=batch_size
    //should buffer at least one fm, should prevent deadlock at least 
    #pragma HLS STREAM variable=in_buff depth=in_size

    buffer_buff<
        BATCH_SIZE,
        1, //rows
        1, //cols
        1, //channels
        b_bool_t
    >(ctrl_drop, cd_buff);

    buffer_buff<
        BATCH_SIZE,
        ROWS, //rows
        COLS, //cols
        CHANNELS, //channels
        buffer_t
    >(in, in_buff);

    buffer_conditional<
        BATCH_SIZE,
        ROWS, //rows
        COLS, //cols
        CHANNELS, //channels
        DROP_MODE,
        buffer_t
    >(in_buff, cd_buff, out);


    ////TODO add template value for buffer size
    //const unsigned int buff_size  = rows*cols*channels;
    //// FIXME this is huge buffering and not realistic
    //const unsigned int in_size  = batch_size*buff_size;

    ////should buffer at least one fm, should prevent deadlock at least 
    //#pragma HLS STREAM variable=in depth=in_size //buff_size
    //#pragma HLS STREAM variable=out
    ////should prevent stalling first exit FIXME limited to one sample bc other stream
    //#pragma HLS STREAM variable=ctrl_drop depth=batch_size

    //b_bool_t drop_tmp;
    //buffer_t data_tmp;

    //batch_loop: for(unsigned long b_index=0;b_index<batch_size;b_index++) {
    //    samp_loop: for(unsigned long pxi_index=0;pxi_index<buff_size;pxi_index++) {
    //    #pragma HLS PIPELINE II=1 

    //        if(pxi_index == 0) {
    //            //wait for ctrl signal per batch
    //            drop_tmp = ctrl_drop.read();
    //        }
    //        //read in intermediate fm every time
    //        data_tmp = in.read();

    //        if ((drop_tmp.data == 1 && !drop_mode) || (drop_tmp.data == 0 && drop_mode)) {
    //            assert(drop_tmp.batchid == data_tmp.batchid);
    //            //either drop or pass thru data read
    //            out.write(data_tmp);
    //        }
    //        //else do nothing with data 
    //    }
    //}

}
#endif
