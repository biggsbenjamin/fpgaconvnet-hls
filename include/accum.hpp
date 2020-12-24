#include "common.hpp"

template<int _>
void NAME_SUB(name,_accum)(
    stream_t(acc_t) &in,
    stream_t(acc_t) &out
)
{

    #pragma HLS INLINE OFF 
    
    const unsigned int batch_size = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows       = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols       = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels   = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int filters    = NAME_SUB(MODULE_NAME,_FILTERS);
    const unsigned int groups     = NAME_SUB(MODULE_NAME,_GROUPS);
    
    const unsigned int channels_per_group = DIVIDE(channels,groups);
    const unsigned int filters_per_group  = DIVIDE(filters ,groups);

    #pragma HLS STREAM variable=in
    #pragma HLS STREAM variable=out
    DO_PRAGMA(HLS STREAM variable=out depth=filters_per_group+1)
    
    acc_t acc[filters_per_group];
    #pragma HLS dependence variable=acc intra RAW true
    #pragma HLS resource variable=acc core=RAM_2P_BRAM

#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1)
    stream_pixel_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
#endif
#if NAME_SUB(MODULE_NAME,_GROUPS) != 1
        stream_group_loop: for(unsigned int group_index=0;group_index<groups;group_index++) {
#else
            unsigned int group_index=0;
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_PER_GROUP) != 1
            stream_channel_loop: for(unsigned int channel_index=0;channel_index<channels_per_group;channel_index++) {
#else
                unsigned int channel_index=0;
#endif
                while( out.full() ) {}; 
#if NAME_SUB(MODULE_NAME,_FILTERS_PER_GROUP) != 1
                stream_filter_loop: for(unsigned int filter_index=0;filter_index<filters_per_group;filter_index++) {
#else
                    unsigned int filter_index=0;
#endif
                    #pragma HLS PIPELINE II=1 rewind 
                   
                    acc_t prev = ( channel_index == 0 ) ? acc_t(0) : acc[filter_index] ;
                    acc[filter_index] = prev + in.read() ;
                
                    if( channel_index == (channels_per_group-1) ) {
                        out.write( acc[filter_index] ) ;
                    }

#if NAME_SUB(MODULE_NAME,_FILTERS_PER_GROUP) != 1
                }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_PER_GROUP) != 1
            }
#endif
#if NAME_SUB(MODULE_NAME,_GROUPS) != 1
        }
#endif
#if (NAME_SUB(MODULE_NAME,_BATCH_SIZE) > 1) || (NAME_SUB(MODULE_NAME,_ROWS) > 1) || (NAME_SUB(MODULE_NAME,_COLS) > 1) 
    }
#endif

}

