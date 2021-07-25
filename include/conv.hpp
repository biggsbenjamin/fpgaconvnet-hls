#ifndef CONV_HPP_
#define CONV_HPP_

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
    unsigned int KERNEL_SIZE,
    typename Conv_data,
    typename Conv_weight
>
void conv_intr(
    stream_t(Conv_data)    in[KERNEL_SIZE][KERNEL_SIZE],
    const Conv_weight      weights[CHANNELS*FILTERS][KERNEL_SIZE][KERNEL_SIZE],
    stream_t(Conv_data)    window_stream[FINE],
    stream_t(Conv_weight)  weight_stream[FINE]
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
    Conv_data window_cache[kernel_size][kernel_size];
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
    unsigned int KERNEL_SIZE,
   	typename Conv_data,
	  typename Conv_weight,
	  typename Conv_acc
>
void conv_mul(
    stream_t(Conv_data)    window_stream[FINE],
    stream_t(Conv_weight)  weight_stream[FINE],
    stream_t(Conv_acc)     acc_stream[FINE]
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
    Conv_acc acc_cache[fine];
    unsigned char acc_index=0;
    mul_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols*channels*filters*interval;pixel_index++) {
        /* acc_loop: for(unsigned char acc_index=0;acc_index<interval;acc_index++) { */
            #pragma HLS loop_flatten
            #pragma HLS unroll region
            #pragma HLS pipeline II=1
            mul_loop: for(unsigned char fine_index=0;fine_index<fine;fine_index++) {
                #pragma HLS pipeline II=1
                // update accumulation cache
                Conv_acc prev = ( acc_index == 0 ) ? Conv_acc(0) : acc_cache[fine_index] ;
                acc_cache[fine_index] = prev + window_stream[fine_index].read() * weight_stream[fine_index].read();
                // write to output stream
                if( acc_index == (interval-1) ) {
                    acc_stream[fine_index].write( acc_cache[fine_index] ) ;
                }
            }
            // increment accumulation index
            acc_index = (acc_index+1) % interval;
        /* } */
    }
}

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE,
    typename Conv_acc
>
void conv_acc(
    stream_t(Conv_acc) acc_stream[FINE],
    stream_t(Conv_acc) &out
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
        #pragma HLS pipeline II=1 rewind
        #pragma HLS unroll region 
        Conv_acc acc = 0 ;
        acc_fine_loop: for(unsigned char fine_index=0;fine_index<fine;fine_index++) {
            acc += acc_stream[fine_index].read();
        }
        out.write(acc);
    }
}

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int FINE,
    unsigned int KERNEL_SIZE,
   	typename Conv_data,
	  typename Conv_weight,
	  typename Conv_acc
>
void conv(
    stream_t(Conv_data) in[KERNEL_SIZE][KERNEL_SIZE],
    const Conv_weight weights[CHANNELS*FILTERS][KERNEL_SIZE][KERNEL_SIZE],
    stream_t(Conv_acc) &out
)
{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

//#pragma HLS stable variable=weights


#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out

#pragma HLS ARRAY_PARTITION variable=in complete dim=0

    const unsigned int fine = FINE;
    
    stream_t(Conv_data)  window_stream[fine];
    stream_t(Conv_weight) weight_stream[fine];
    stream_t(Conv_acc) acc_stream[fine];

    #pragma HLS STREAM variable=window_stream
    #pragma HLS STREAM variable=weight_stream
    #pragma HLS STREAM variable=acc_stream

    conv_intr<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        FINE,
        KERNEL_SIZE,
  		  Conv_data,
		    Conv_weight
    >(in,weights,window_stream,weight_stream);

    conv_mul<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        FINE,
        KERNEL_SIZE,
  		  Conv_data,
		    Conv_weight,
		    Conv_acc
    >(window_stream,weight_stream,acc_stream);

    conv_acc<
        BATCH_SIZE,
        ROWS,
        COLS,
        CHANNELS,
        FILTERS,
        FINE,
        KERNEL_SIZE,
  		  Conv_acc
    >(acc_stream,out);

}

/**
 *  POINTWISE CONVOLUTION FUNCTION
 */
template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
   	typename Conv_data,
	  typename Conv_weight,
	  typename Conv_acc
>
void conv(
    stream_t(Conv_data) &in,
    const Conv_weight weights[CHANNELS*FILTERS],
    stream_t(Conv_acc) &out
)
{

#pragma HLS INLINE OFF

    const unsigned batch_size   = BATCH_SIZE;
    const unsigned rows         = ROWS;
    const unsigned cols         = COLS;
    const unsigned channels     = CHANNELS;
    const unsigned filters      = FILTERS;

#pragma HLS STREAM variable=in
#pragma HLS STREAM variable=out
    
    Conv_data window_cache;
    
    pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        unsigned int weight_index = 0;
        channel_loop: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
            filter_loop: for(unsigned int filter_index=0;filter_index<filters;filter_index++) {
                #pragma HLS loop_flatten
                #pragma HLS PIPELINE II=1
                /* #pragma HLS dependence variable=windowCache intra RAW true */
                if(filter_index == 0) {
                    DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                    window_cache = in.read();
                }
                Conv_acc acc = window_cache * weights[weight_index];
                weight_index++;
                out.write(acc);
            }
        }
    }
}

#endif
