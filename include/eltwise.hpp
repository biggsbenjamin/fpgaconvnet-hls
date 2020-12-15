#include "common.hpp"

template<
    typename input_t,
    typename output_t
>
void NAME_SUB(name,_eltwise)(
#if NAME_SUB(MODULE_NAME,_CHANNELS_0) != 0
    stream_t(input_t) in_0[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_1) != 0
    stream_t(input_t) in_1[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_2) != 0
    stream_t(input_t) in_2[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_3) != 0
    stream_t(input_t) in_3[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_4) != 0
    stream_t(input_t) in_4[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_5) != 0
    stream_t(input_t) in_5[NAME_SUB(MODULE_NAME,_COARSE)],
#endif
    stream_t(output_t) out[NAME_SUB(MODULE_NAME,_COARSE)]
)
{{

#pragma HLS inline off 
#pragma HLS DATAFLOW

#pragma HLS INTERFACE ap_ctrl_chain port=return

#pragma HLS STREAM variable=out
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    const unsigned batch_size   = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned rows         = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned cols         = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned coarse       = NAME_SUB(MODULE_NAME,_COARSE);
    const unsigned channels_out = NAME_SUB(MODULE_NAME,_CHANNELS_OUT);

#if NAME_SUB(MODULE_NAME,_CHANNELS_0) != 0
    const unsigned channels_0 = NAME_SUB(MODULE_NAME,_CHANNELS_0);
    DO_PRAGMA(HLS STREAM variable=in_0 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_0 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_1) != 0
    const unsigned channels_1 = NAME_SUB(MODULE_NAME,_CHANNELS_1);
    DO_PRAGMA(HLS STREAM variable=in_1 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_1 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_2) != 0
    const unsigned channels_2 = NAME_SUB(MODULE_NAME,_CHANNELS_2);
    DO_PRAGMA(HLS STREAM variable=in_2 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_2 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_3) != 0
    const unsigned channels_3 = NAME_SUB(MODULE_NAME,_CHANNELS_3);
    DO_PRAGMA(HLS STREAM variable=in_3 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_3 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_4) != 0
    const unsigned channels_4 = NAME_SUB(MODULE_NAME,_CHANNELS_4);
    DO_PRAGMA(HLS STREAM variable=in_4 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_4 complete dim=0
#endif
#if NAME_SUB(MODULE_NAME,_CHANNELS_5) != 0
    const unsigned channels_5 = NAME_SUB(MODULE_NAME,_CHANNELS_5);
    DO_PRAGMA(HLS STREAM variable=in_5 depth=channels_out+1)
    #pragma HLS ARRAY_PARTITION variable=in_5 complete dim=0
#endif

    dim_loop: for(int pixelIndex=0;pixelIndex<{NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS;pixelIndex++)
    {
#pragma HLS dependence variable out intra WAW true
#pragma HLS dependence variable out inter false
//#pragma HLS dependence variable in inter false
#pragma HLS PIPELINE
#pragma HLS DATAFLOW

        channel_loop: for(int channel_index=0;channel_index<CHANNELS_3D({NAME}_CHANNELS,{NAME}_COARSE);channel_index++)
        {{
            #pragma HLS PIPELINE II=1
            {name}_output_t tmp[coarse] = {0};
            coarse_loop: for(int coarse_index=0;c<{NAME}_COARSE;c++)
            {

{inputs}

                out[c].write(tmp[c]);
            }
        }

    }

}}


