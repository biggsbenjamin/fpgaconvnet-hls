#include "common.hpp"

/**
 *  CONVOLUTION FUNCTION
 */

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE
>
void conv_intr(
    stream_t(data_t)    in[KERNEL_SIZE][KERNEL_SIZE],
    const weight_t      weights[CHANNELS*FILTERS][KERNEL_SIZE][KERNEL_SIZE],
    stream_t(data_t)    window_stream[FINE],
    stream_t(weight_t)  weight_stream[FINE]
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int filters      = FILTERS;
    const unsigned int kernel_size  = KERNEL_SIZE;
    const unsigned int fine         = FINE;
    const unsigned int interval     = DIVIDE(kernel_size*kernel_size,fine);
 
#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=window_stream 
#pragma HLS STREAM variable=weight_stream 

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0

    // TODO: better generalisation
    const unsigned int weights_partition_factor_k1 = MIN(fine,kernel_size);
    const unsigned int weights_partition_factor_k2 = (fine<=kernel_size) ? 1 : kernel_size;

DO_PRAGMA(HLS ARRAY_PARTITION variable=weights block factor=weights_partition_factor_k1 dim=2) 
DO_PRAGMA(HLS ARRAY_PARTITION variable=weights block factor=weights_partition_factor_k2 dim=3) 

    // INTERLEAVING LOOP
    data_t window_cache[kernel_size][kernel_size];
    #pragma HLS ARRAY_PARTITION variable=window_cache complete dim=0

    intr_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        unsigned int weight_index = 0;
        intr_channel_loop: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
            intr_filter_loop: for(unsigned int filter_index=0;filter_index<filters;filter_index++) {
                
                #pragma HLS loop_flatten
                #pragma HLS dependence variable=window_cache intra RAW true
                DO_PRAGMA( HLS dependence variable=window_cache inter WAW true distance=batch_size*rows*cols*channels )
                DO_PRAGMA( HLS PIPELINE II=interval )

                unsigned char fine_index = 0;

                intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                    intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                        if(filter_index == 0) {
                            DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                            window_cache[k1][k2] = in[k1][k2].read();
                        }
                        
                        window_stream[fine_index].write(window_cache[k1][k2]);
                        weight_stream[fine_index].write(weights[weight_index][k1][k2]);
                        fine_index = ( fine_index + 1 ) % fine;
                    }
                }
                weight_index++;
            }
        }
    }
}

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE
>
void conv_mul(
    stream_t(data_t)    window_stream[FINE],
    stream_t(weight_t)  weight_stream[FINE],
    stream_t(acc_t)     acc_stream[FINE]
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=window_stream 
#pragma HLS STREAM variable=weight_stream 
#pragma HLS STREAM variable=acc_stream 

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int filters      = FILTERS;
    const unsigned int kernel_size  = KERNEL_SIZE;
    const unsigned int fine         = FINE;
    const unsigned int interval     = DIVIDE(kernel_size*kernel_size,fine);
 
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0

    // MULTIPLICATION LOOP
    mul_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols*channels*filters;pixel_index++) {
        acc_t acc_cache[fine];
        acc_loop: for(unsigned char acc_index=0;acc_index<interval;acc_index++) {
            #pragma HLS PIPELINE II=1 rewind
            mul_loop: for(unsigned char fine_index=0;fine_index<fine;fine_index++) {
                // update accumulation cache
                acc_t prev = ( acc_index == 0 ) ? acc_t(0) : acc_cache[fine_index] ;
                acc_cache[fine_index] = prev + window_stream[fine_index].read() * weight_stream[fine_index].read();
                // write to output stream
                if( acc_index == (interval-1) ) {
                    acc_stream[fine_index].write( acc_cache[fine_index] ) ;
                }
            }
        }
    }
}

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE
>
void conv_acc(
    stream_t(acc_t) acc_stream[FINE],
    stream_t(acc_t) &out
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=acc_stream 
#pragma HLS STREAM variable=out

    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    const unsigned int filters      = FILTERS;
    const unsigned int kernel_size  = KERNEL_SIZE;
    const unsigned int fine         = FINE;
 
#pragma HLS ARRAY_PARTITION variable=acc_stream complete dim=0
    
    // ACCUMULATION LOOP
    acc_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols*channels*filters;pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
        acc_t acc = 0 ;
        acc_fine_loop: for(unsigned char fine_index=0;fine_index<fine;fine_index++) {
            acc += acc_stream[fine_index].read();
        }
        out.write(acc);
    }
}

template<int _>
void NAME_SUB(name,_conv)(
    stream_t(data_t) in[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    const weight_t weights[NAME_SUB(MODULE_NAME,_CHANNELS)*NAME_SUB(MODULE_NAME,_FILTERS)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(acc_t) &out
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

//#pragma HLS stable variable=weights

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int filters      = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
    const unsigned int fine         = NAME_SUB(MODULE_NAME,_FINE);
 
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    stream_t(data_t)  window_stream[fine];
    stream_t(weight_t) weight_stream[fine];
    stream_t(acc_t) acc_stream[fine];

    #pragma HLS STREAM variable=window_stream 
    #pragma HLS STREAM variable=weight_stream 
    #pragma HLS STREAM variable=acc_stream    

    conv_intr<
        batch_size,
        rows,
        cols,
        channels,
        filters,
        fine,
        kernel_size
    >(in,weights,window_stream,weight_stream);

    conv_mul<
        batch_size,
        rows,
        cols,
        channels,
        filters,
        fine,
        kernel_size
    >(window_stream,weight_stream,acc_stream);

    conv_acc<
        batch_size,
        rows,
        cols,
        channels,
        filters,
        fine,
        kernel_size
    >(acc_stream,out);

/*


    NAME_SUB(name,_conv_intr)<0>(in,weights,window_stream,weight_stream);
    NAME_SUB(name,_conv_mul)<0>(window_stream,weight_stream,acc_stream);
    NAME_SUB(name,_conv_acc)<0>(acc_stream,out);
*/

}

/**
 *  POINTWISE CONVOLUTION FUNCTION
 */
template<int _>
void NAME_SUB(name,_conv_pw)(
    stream_t(data_t) &in,
    const weight_t weights[NAME_SUB(MODULE_NAME,_CHANNELS)*NAME_SUB(MODULE_NAME,_FILTERS)],
    stream_t(acc_t) &out
)
{

#pragma HLS INLINE OFF

    const unsigned batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned filters      = NAME_SUB(MODULE_NAME,_FILTERS);
 
#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out
    
    data_t window_cache;
    
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)  
    pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
        unsigned int weight_index = 0;
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
        channel_loop: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
#else
        unsigned int channel_index=0;
#endif
#if NAME_SUB(MODULE_NAME,_FILTERS) != 1
            filter_loop: for(unsigned int filter_index=0;filter_index<filters;filter_index++) {
#else
            unsigned int filter_index=0;
#endif
                #pragma HLS PIPELINE II=1 rewind
                #pragma HLS dependence variable=windowCache intra RAW true
                if(filter_index == 0) {
                    DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                    window_cache = in.read();
                }
                acc_t acc = window_cache * weights[weight_index];
                weight_index++;
                out.write(acc);
#if NAME_SUB(MODULE_NAME,_FILTERS) != 1
            }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    }
#endif
}

