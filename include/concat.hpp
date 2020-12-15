#include "common.hpp"

template<int _>
void NAME_SUB(name,_concat)(
#if NAME_SUB(MODULE_NAME,_CHANNELS_0) != 0
    stream_t(data_t) in_0[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_1) != 0
    stream_t(data_t) in_1[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_2) != 0
    stream_t(data_t) in_2[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_3) != 0
    stream_t(data_t) in_3[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_4) != 0
    stream_t(data_t) in_4[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_5) != 0
    stream_t(data_t) in_5[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_COARSE)]
)
{

#pragma HLS INLINE OFF 

    const unsigned int batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int coarse       = NAME_SUB(MODULE_NAME,_COARSE);
    const unsigned int channels_out = NAME_SUB(MODULE_NAME,_CHANNELS_OUT);

#pragma HLS STREAM variable=out
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

#if NAME_SUB(MODULE_NAME,_CHANNELS_0) != 0
    const unsigned int channels_0 = NAME_SUB(MODULE_NAME,_CHANNELS_0);
    DO_PRAGMA(HLS STREAM variable=in_0 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_0 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_1) != 0
    const unsigned int channels_1 = NAME_SUB(MODULE_NAME,_CHANNELS_1);
    DO_PRAGMA(HLS STREAM variable=in_1 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_1 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_2) != 0
    const unsigned int channels_2 = NAME_SUB(MODULE_NAME,_CHANNELS_2);
    DO_PRAGMA(HLS STREAM variable=in_2 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_2 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_3) != 0
    const unsigned int channels_3 = NAME_SUB(MODULE_NAME,_CHANNELS_3);
    DO_PRAGMA(HLS STREAM variable=in_3 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_3 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_4) != 0
    const unsigned int channels_4 = NAME_SUB(MODULE_NAME,_CHANNELS_4);
    DO_PRAGMA(HLS STREAM variable=in_4 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_4 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_5) != 0
    const unsigned int channels_5 = NAME_SUB(MODULE_NAME,_CHANNELS_5);
    DO_PRAGMA(HLS STREAM variable=in_5 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_5 complete dim=0
#endif

    dim_loop: for(unsigned long pixel_index=0;pixel_index<batch_size*rows*cols;pixel_index++) {
        #pragma HLS dependence variable out intra WAW true
        #pragma HLS pipeline
#if NAME_SUB(MODULE_NAME,_CHANNELS_0) != 0
        //#pragma HLS dependence variable in_0 inter false
        channel_0_loop: for(unsigned int channel_index=0;channel_index<channels_0;channel_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_0_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
                out[coarse_index].write(in_0[coarse_index].read());
            }
        }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_1) != 0
        //#pragma HLS dependence variable in_1 inter false
        channel_1_loop: for(unsigned int channel_index=0;channel_index<channels_1;channel_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_1_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
                out[coarse_index].write(in_1[coarse_index].read());
            }
        }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_2) != 0
        //#pragma HLS dependence variable in_2 inter false
        channel_2_loop: for(unsigned int channel_index=0;channel_index<channels_2;channel_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_2_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
                out[coarse_index].write(in_2[coarse_index].read());
            }
        }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_3) != 0
        //#pragma HLS dependence variable in_3 inter false
        channel_3_loop: for(unsigned int channel_index=0;channel_index<channels_3;channel_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_3_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
                out[coarse_index].write(in_3[coarse_index].read());
            }
        }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_4) != 0
        //#pragma HLS dependence variable in_4 inter false
        channel_4_loop: for(unsigned int channel_index=0;channel_index<channels_4;channel_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_4_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
                out[coarse_index].write(in_4[coarse_index].read());
            }
        }
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_5) != 0
        //#pragma HLS dependence variable in_5 inter false
        channel_5_loop: for(unsigned int channel_index=0;channel_index<channels_5;channel_index++) {
            #pragma HLS PIPELINE II=1 rewind
            coarse_5_loop: for(unsigned int coarse_index=0;coarse_index<coarse;coarse_index++) {
                out[coarse_index].write(in_5[coarse_index].read());
            }
        }
#endif
    }
}
