#include "common.hpp"

/**
 *  CONVOLUTION FUNCTION
 */

template<int _>
void NAME_SUB(name,_conv_intr)(
    stream_t(data_t) in[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    const weight_t weights[NAME_SUB(MODULE_NAME,_CHANNELS)*NAME_SUB(MODULE_NAME,_FILTERS)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#if NAME_SUB(MODULE_NAME,_FINE) == 1
    stream_t(data_t)  &window_stream,
    stream_t(weight_t) &weight_stream
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(weight_t) weight_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(weight_t) weight_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#endif
)
{

#pragma HLS INLINE OFF

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int filters      = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
    const unsigned int fine         = NAME_SUB(MODULE_NAME,_FINE);
    const unsigned int interval     = DIVIDE(kernel_size*kernel_size,fine);
 
#pragma HLS STREAM variable=in 
#pragma HLS STREAM variable=window_stream 
#pragma HLS STREAM variable=weight_stream 

#if NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=weights complete dim=2
#pragma HLS ARRAY_PARTITION variable=weights complete dim=3
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#endif

    // INTERLEAVING LOOP
    data_t window_cache[kernel_size][kernel_size];
    #pragma HLS ARRAY_PARTITION variable=window_cache complete dim=0
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)  
    intr_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
        unsigned int weight_index = 0;
#if NAME_SUB(MODULE_NAME,_CHANNELS) != 1
        intr_channel_loop: for(unsigned int channel_index=0;channel_index<channels;channel_index++) {
#else
        unsigned int channel_index=0;
#endif
#if NAME_SUB(MODULE_NAME,_FILTERS) != 1
            intr_filter_loop: for(unsigned int filter_index=0;filter_index<filters;filter_index++) {
#else
            unsigned int filter_index=0;
#endif
                #pragma HLS dependence variable=window_cache   intra RAW true
                DO_PRAGMA( HLS dependence variable=window_cache inter WAW true distance=batch_size*rows*cols*channels )
                DO_PRAGMA( HLS PIPELINE II=interval )
#if NAME_SUB(MODULE_NAME,_FINE) == 1
                intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                    intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                        #pragma HLS PIPELINE II=1 rewind
                        if(filter_index == 0) {
                            DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                            window_cache[k1][k2] = in[k1][k2].read();
                        }
                        window_stream.write(window_cache[k1][k2]);
                        weight_stream.write(weights[weight_index][k1][k2]);
                    }
                }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
                intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                    #pragma HLS PIPELINE II=1 rewind
                    intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                        if(filter_index == 0) {
                            DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                            window_cache[k1][k2] = in[k1][k2].read();
                        }   
                        window_stream[k1].write(window_cache[k1][k2]);
                        weight_stream[k1].write(weights[weight_index][k1][k2]);
                    }
                }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
                #pragma HLS PIPELINE II=1 rewind
                intr_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
                    intr_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                        if(filter_index == 0) {
                            DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels)
                            window_cache[k1][k2] = in[k1][k2].read();
                        }
                        window_stream[k1][k2].write(window_cache[k1][k2]);
                        weight_stream[k1][k2].write(weights[weight_index][k1][k2]);
                    }
                }
#endif
                weight_index++;
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

template<int _>
void NAME_SUB(name,_conv_mul)(
#if NAME_SUB(MODULE_NAME,_FINE) == 1
    stream_t(data_t)  &window_stream,
    stream_t(weight_t) &weight_stream,
    stream_t(acc_t) &acc_stream
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(weight_t) weight_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(acc_t) acc_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(weight_t) weight_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
    stream_t(acc_t) acc_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#endif
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=window_stream 
#pragma HLS STREAM variable=weight_stream 
#pragma HLS STREAM variable=acc_stream 

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int filters      = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
    const unsigned int fine         = NAME_SUB(MODULE_NAME,_FINE);
    const unsigned int interval     = DIVIDE(kernel_size*kernel_size,fine);
 
#if NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0
#endif

    // MULTIPLICATION LOOP
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) || (NAME_SUB(MODULE_NAME,_CHANNELS) > 1) || (NAME_SUB(MODULE_NAME,_FILTERS) > 1)
    mul_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols*channels*filters;pixel_index++) {
#endif
#if NAME_SUB(MODULE_NAME,_FINE) == 1
        acc_t acc_cache = 0;
        mul_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
            mul_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                #pragma HLS PIPELINE II=1 rewind

                acc_t prev = ( (k1==0) && (k2==0) ) ? acc_t(0) : acc_cache ;
                acc_cache = prev + window_stream.read() * weight_stream.read();
                
                if( (k1==(kernel_size-1)) && (k2==(kernel_size-1)) ){
                    acc_stream.write( acc_cache ) ;
                }
            }
        }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
        acc_t acc_cache[kernel_size] = {0};
        mul_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
            #pragma HLS PIPELINE II=1 rewind
            mul_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {

                acc_t prev = ( k2 == 0 ) ? acc_t(0) : acc_cache[k1] ;
                acc_cache[k1] = prev + window_stream[k1].read() * weight_stream[k1].read();
                
                if(k2==(kernel_size-1)){
                    acc_stream[k1].write( acc_cache[k1] ) ;
                }
            }
        }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
        #pragma HLS PIPELINE II=1 rewind
        mul_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
            mul_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                data_t  mul_pixel  = window_stream[k1][k2].read();
 		weight_t mul_weight = weight_stream[k1][k2].read();
 		acc_t mul_val = mul_pixel * mul_weight;
		acc_stream[k1][k2].write(mul_val);
            }
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) || (NAME_SUB(MODULE_NAME,_CHANNELS) > 1) || (NAME_SUB(MODULE_NAME,_FILTERS) > 1)
    }
#endif
}

template<int _>
void NAME_SUB(name,_conv_acc)(
#if NAME_SUB(MODULE_NAME,_FINE) == 1
    stream_t(acc_t) &acc_stream,
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(acc_t) acc_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(acc_t) acc_stream[NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)],
#endif
    stream_t(acc_t) &out
)
{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=acc_stream 
#pragma HLS STREAM variable=out

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels     = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int filters      = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);
    const unsigned int fine         = NAME_SUB(MODULE_NAME,_FINE);
    const unsigned int interval     = DIVIDE(kernel_size*kernel_size,fine);
 
#if NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0
#endif
    // ACCUMULATION LOOP
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) || (NAME_SUB(MODULE_NAME,_CHANNELS) > 1) || (NAME_SUB(MODULE_NAME,_FILTERS) > 1)
    acc_pixel_loop: for(unsigned int pixel_index=0;pixel_index<batch_size*rows*cols*channels*filters;pixel_index++) {
#endif
        #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_FINE) == 1
        acc_t acc = acc_stream.read();
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
        acc_t acc = 0 ;
        acc_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
            acc += acc_stream[k1].read();
        }
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
        acc_t acc = 0 ;
        acc_k2_loop: for(unsigned char k2=0;k2<kernel_size;k2++) {
            acc_k1_loop: for(unsigned char k1=0;k1<kernel_size;k1++) {
                acc += acc_stream[k1][k2].read();
            }
        }
#endif
        out.write(acc);
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) || (NAME_SUB(MODULE_NAME,_CHANNELS) > 1) || (NAME_SUB(MODULE_NAME,_FILTERS) > 1)
    }
#endif
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
#if NAME_SUB(MODULE_NAME,_FINE) == 1
    stream_t(data_t)  window_stream;
    stream_t(weight_t) weight_stream;
    stream_t(acc_t) acc_stream;
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[kernel_size];
    stream_t(weight_t) weight_stream[kernel_size];
    stream_t(acc_t) acc_stream[kernel_size];
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0
#elif NAME_SUB(MODULE_NAME,_FINE) == NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)
    stream_t(data_t)  window_stream[kernel_size][kernel_size];
    stream_t(weight_t) weight_stream[kernel_size][kernel_size];
    stream_t(acc_t) acc_stream[kernel_size][kernel_size];
#pragma HLS ARRAY_PARTITION variable=window_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=weight_stream complete dim=0
#pragma HLS ARRAY_PARTITION variable=acc_stream    complete dim=0
#endif

    #pragma HLS STREAM variable=window_stream 
    #pragma HLS STREAM variable=weight_stream 
    #pragma HLS STREAM variable=acc_stream    

    NAME_SUB(name,_conv_intr)<0>(in,weights,window_stream,weight_stream);
    NAME_SUB(name,_conv_mul)<0>(window_stream,weight_stream,acc_stream);
    NAME_SUB(name,_conv_acc)<0>(acc_stream,out);
    
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

