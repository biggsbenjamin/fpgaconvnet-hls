#include "common.hpp"

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    unsigned int FILTERS,
    unsigned int GROUPS
>
void accum(
    stream_t(acc_t) &in,
    stream_t(acc_t) &out
)
{

    #pragma HLS INLINE OFF 
    
    const unsigned int batch_size = BATCH_SIZE;
    const unsigned int rows       = ROWS;
    const unsigned int cols       = COLS;
    const unsigned int channels   = CHANNELS;
    const unsigned int filters    = FILTERS;
    const unsigned int groups     = GROUPS;
    
    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out
    DO_PRAGMA(HLS STREAM variable=out depth=filters_per_group+1)
    
    acc_t acc[filters_per_group];
    #pragma HLS dependence variable=acc intra RAW true
    #pragma HLS resource variable=acc core=RAM_2P_BRAM

    stream_pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        stream_group_loop: for(unsigned int group_index=0;group_index<groups;group_index++) {
            stream_channel_loop: for(unsigned int channel_index=0;channel_index<channels_per_group;channel_index++) {
                while( out.full() ) {}; 
                stream_filter_loop: for(unsigned int filter_index=0;filter_index<filters_per_group;filter_index++) {
                    #pragma HLS PIPELINE II=1 rewind 
                   
                    acc_t prev = ( channel_index == 0 ) ? acc_t(0) : acc[filter_index] ;
                    acc[filter_index] = prev + in.read() ;
                
                    if( channel_index == (channels_per_group-1) ) {
                        out.write( acc[filter_index] ) ;
                    }

                }
            }
        }
    }
}

