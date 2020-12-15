#include "common.hpp"

/**
 * SPLIT FUNCTION
 */

template<int _>
void NAME_SUB(name,_split)(
    stream_t(data_t)  in[NAME_SUB(MODULE_NAME,_COARSE)],
    stream_t(data_t) out[NAME_SUB(MODULE_NAME,_OUTPUTS)][NAME_SUB(MODULE_NAME,_COARSE)]
)
{

#pragma HLS INLINE OFF 
#pragma HLS INTERFACE ap_ctrl_chain port=return

    const unsigned int batch_size = NAME_SUB(MODULE_NAME,_BATCH_SIZE);
    const unsigned int rows       = NAME_SUB(MODULE_NAME,_ROWS);
    const unsigned int cols       = NAME_SUB(MODULE_NAME,_COLS);
    const unsigned int channels   = NAME_SUB(MODULE_NAME,_CHANNELS);
    const unsigned int coarse     = NAME_SUB(MODULE_NAME,_COARSE);
    const unsigned int outputs    = NAME_SUB(MODULE_NAME,_OUTPUTS);
    const unsigned int channels_per_stream = (const unsigned int) (channels/coarse);

DO_PRAGMA( HLS STREAM variable=in depth=outputs+1 )
DO_PRAGMA( HLS STREAM variable=out depth=channels_per_stream+1 )

#pragma HLS ARRAY_PARTITION variable=in complete dim=0
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

    data_t local_cache[coarse];
    #pragma HLS ARRAY_PARTITION variable=local_cache complete dim=0
    #pragma HLS dependence variable=local_cache intra true RAW

    pixel_loop: for (unsigned long pixel_index=0; pixel_index < batch_size*rows*cols*channels_per_stream; pixel_index++) {
        #pragma HLS PIPELINE II=1 rewind
#if NAME_SUB(MODULE_NAME,_COARSE) != 1
        coarse_loop: for(unsigned int coarse_index=0;coarse_index < coarse; coarse_index++) {
            output_loop: for(unsigned int output_index=0;output_index < outputs; output_index++) {
                if(output_index == 0) {
                    DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels_per_stream)
		    local_cache[coarse_index] = (data_t) in[coarse_index].read();
	        }
	        out[output_index][coarse_index].write(local_cache[coarse_index]);
            }
	}
#else
        output_loop: for(unsigned int output_index=0;output_index < outputs; output_index++) {
            if(output_index == 0) {
                DO_PRAGMA(HLS occurrence cycle=batch_size*rows*cols*channels_per_stream)
                local_cache[0] = (data_t) in[0].read();
            }
            out[output_index][0].write(local_cache[0]);
        }
#endif                     
    }
}

