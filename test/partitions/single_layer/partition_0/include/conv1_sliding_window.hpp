#include "common.hpp"

template<int _>
void NAME_SUB(name,_sliding_window_line_shift)(
    stream_t(data_t) &in,
    stream_t(data_t) frame_buffer[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int pad_top      = NAME_SUB(MODULE_NAME,_PAD_TOP);
    const unsigned int pad_right    = NAME_SUB(MODULE_NAME,_PAD_RIGHT);
    const unsigned int pad_bottom   = NAME_SUB(MODULE_NAME,_PAD_BOTTOM);
    const unsigned int pad_left     = NAME_SUB(MODULE_NAME,_PAD_LEFT);
    const unsigned int row_stride   = NAME_SUB(MODULE_NAME,_STRIDE);
    const unsigned int col_stride   = NAME_SUB(MODULE_NAME,_STRIDE);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);

    //DO_PRAGMA( HLS STREAM variable=in depth=pad_bottom*channels*cols+pad_left*channels+1 )
    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=frame_buffer
    #pragma HLS ARRAY_PARTITION variable=frame_buffer complete dim=0
 
    stream_t(data_t) line_buffer[kernel_size-1];
    DO_PRAGMA( HLS STREAM variable=line_buffer depth=cols*channels+pad_left*channels+pad_right*channels+1 )
    #pragma HLS ARRAY_PARTITION variable=line_buffer complete dim=0
    #pragma HLS resource variable=line_buffer core=FIFO_BRAM

    stream_t(data_t) window_buffer[kernel_size][kernel_size-1]; // pixel window cache
    DO_PRAGMA( HLS STREAM variable=window_buffer depth=channels+1 )
    #pragma HLS ARRAY_PARTITION variable=window_buffer complete dim=0
    #pragma HLS resource variable=frame_buffer core=FIFO_BRAM

    data_t frame_cache[kernel_size][kernel_size];
    #pragma HLS ARRAY_PARTITION variable=frame_cache complete dim=0

#if NAME_SUB(MODULE_NAME,_BATCH_SIZE) != 1
    in_loop_batch: for(unsigned int batch_index=0;batch_index<batch_size;batch_index++) {
#endif
        in_loop_rows: for(unsigned int row_index=0;row_index<rows+pad_bottom+pad_top;row_index++) {
            in_loop_cols: for(unsigned int col_index=0;col_index<cols+pad_left+pad_right;col_index++) {
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
                in_loop_channels: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
#endif
                    #pragma HLS PIPELINE II=1 rewind
                    #pragma HLS DEPENDENCE variable=line_buffer     WAR intra true
                    #pragma HLS DEPENDENCE variable=window_buffer   WAR intra true
                    #pragma HLS DEPENDENCE variable=frame_cache     WAR intra true
                    data_t pixel;

                    // read in pixel
                    if( row_index < pad_bottom ) {
                        pixel = 0;
                    } 
                    else if ( row_index > rows+pad_bottom-1 ) {
                        pixel = 0;
                    } 
                    else if ( col_index < pad_left ) {
                        pixel = 0;
                    }
                    else if (col_index > cols+pad_left-1 ) {
                        pixel = 0;
                    }
                    else {
                        pixel = in.read();
                    }

                    // init first part of window cache
                    if ( (row_index < kernel_size-1) ) {
                        // fill window cache
                        if( col_index < kernel_size-1 ) {
                            window_buffer[row_index][col_index].write(pixel);
                            frame_cache[row_index][col_index] = pixel;
                            if ( row_index > 0 ) {
                                line_buffer[row_index-1].write(pixel);
                            }
                        }
                        else {
                            line_buffer[row_index].write(pixel);
                        }
                    }

                    // fill top line of window buffer and line buffer
                    else if ( (row_index == (kernel_size-1)) && (col_index < kernel_size-1) ) {
                        window_buffer[row_index][col_index].write(pixel);
                        frame_cache[row_index][col_index] = pixel;
                        line_buffer[kernel_size-2].write(pixel);
                    }

                    // main loop
                    else {
                        
                        // read window buffer into window cache
                        for(unsigned char k1=0;k1<kernel_size;k1++) {
                            for(unsigned char k2=0;k2<kernel_size-1;k2++) {
                                frame_cache[k1][k2] = window_buffer[k1][k2].read();
                            }
                        }

                        // read out line buffer to window cache
                        for(unsigned char k1=0;k1<kernel_size-1;k1++) {
                            frame_cache[k1][kernel_size-1] = line_buffer[k1].read();
                        }

                        // read the top corner into window cache
                        frame_cache[kernel_size-1][kernel_size-1] = pixel;

                        // update window cache
                        if ( !( (row_index == rows+pad_top+pad_bottom-1) && (col_index == cols+pad_left+pad_right-1) ) ) {
                            for(unsigned char k1=0;k1<kernel_size;k1++) {
                                for(unsigned char k2=0;k2<kernel_size-1;k2++) {
                                    window_buffer[k1][k2].write(frame_cache[k1][k2+1]);
                                }
                            }
                        }
                        // update the line buffer
                        if ( !(row_index == rows+pad_top+pad_bottom-1) ) {
                            for(unsigned char k1=0;k1<kernel_size-1;k1++) {
                                line_buffer[k1].write(frame_cache[k1+1][kernel_size-1]);
                            }
                        }
                        // send window cache to frame buffer
                        for(unsigned char k1=0;k1<kernel_size;k1++) {
                            for(unsigned char k2=0;k2<kernel_size;k2++) {
                                frame_buffer[k1][k2].write(frame_cache[k1][k2]);
                            }
                        }
                    }
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
                }
#endif
            }
        }
#if NAME_SUB(MODULE_NAME,_BATCH_SIZE) != 1
    }
#endif
}

template<int _>
void NAME_SUB(name,_sliding_window_out)(
    stream_t(data_t) frame_buffer[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
)
{

#pragma HLS INLINE OFF

    #pragma HLS STREAM variable=frame_buffer
    #pragma HLS ARRAY_PARTITION variable=frame_buffer complete dim=0
 
    #pragma HLS STREAM variable=out
    #pragma HLS ARRAY_PARTITION variable=out complete dim=0
   
    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int pad_top      = NAME_SUB(MODULE_NAME,_PAD_TOP);
    const unsigned int pad_right    = NAME_SUB(MODULE_NAME,_PAD_RIGHT);
    const unsigned int pad_bottom   = NAME_SUB(MODULE_NAME,_PAD_BOTTOM);
    const unsigned int pad_left     = NAME_SUB(MODULE_NAME,_PAD_LEFT);
    const unsigned int row_stride   = NAME_SUB(MODULE_NAME,_STRIDE);
    const unsigned int col_stride   = NAME_SUB(MODULE_NAME,_STRIDE);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);

    // writing frames out
#if NAME_SUB(MODULE_NAME,_BATCH_SIZE) != 1
    out_loop_batch: for(unsigned int batch_index=0;batch_index<batch_size;batch_index++) {
#endif
        out_loop_rows: for(unsigned int row_index=0;row_index<rows+pad_bottom+pad_top;row_index++) {
            out_loop_cols: for(unsigned int col_index=0;col_index<cols+pad_left+pad_right;col_index++) {
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
                out_loop_channels: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
#endif
                    #pragma HLS PIPELINE II=1 rewind
                    if ( !( (row_index < (kernel_size-1)) || ( (row_index == (kernel_size-1)) && (col_index < kernel_size-1) ) ) ) {
                        for(unsigned char k1=0;k1<kernel_size;k1++) {
                            for(unsigned char k2=0;k2<kernel_size;k2++) {
                                data_t tmp = frame_buffer[k1][k2].read();
                                if ( (row_index > (kernel_size-2)) && ((row_index-kernel_size+1)%row_stride == 0) && (col_index > (kernel_size-2)) && ((col_index-kernel_size+1)%col_stride == 0) ) {
                                    out[k1][k2].write(tmp);
                                }
                            }
                        }
                    }
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
                }
#endif
            }
        }
#if NAME_SUB(MODULE_NAME,_BATCH_SIZE) != 1
    }
#endif
}

template<int _>
void NAME_SUB(name,_sliding_window)(
    stream_t(data_t) &in,
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out
    #pragma HLS ARRAY_PARTITION variable=out complete dim=0

    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);

    stream_t(data_t) frame_buffer[kernel_size][kernel_size];
    #pragma HLS STREAM variable=frame_buffer
    #pragma HLS ARRAY_PARTITION variable=frame_buffer complete dim=0

    NAME_SUB(name,_sliding_window_line_shift)<0>(in,frame_buffer );
    NAME_SUB(name,_sliding_window_out)<0>(frame_buffer,out );
 
    /*
    HLSLIB_DATAFLOW_INIT();
    HLSLIB_DATAFLOW_FUNCTION( NAME_SUB(name,_sliding_window_line_shift),in,frame_buffer );
    HLSLIB_DATAFLOW_FUNCTION( NAME_SUB(name,_sliding_window_out),frame_buffer,out );
    HLSLIB_DATAFLOW_FINALIZE();
    */
}
