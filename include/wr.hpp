#ifndef WR_HPP_
#define WR_HPP_

#include "common.hpp"

/**
 *  WEIGHTS RELOADING FUNCTION
 */
template<int _>
void NAME_SUB(name,_wr)(
    stream_t(weight_t) &in,
#if NAME_SUB(MODULE_NAME,_KERNEL_SIZE) == 1
    weight_t weights[NAME_SUB(MODULE_NAME,_COARSE_IN)][NAME_SUB(MODULE_NAME,_COARSE_OUT)][DIVIDE(NAME_SUB(MODULE_NAME,_WEIGHTS),NAME_SUB(MODULE_NAME,_COARSE_IN)*NAME_SUB(MODULE_NAME,_COARSE_OUT)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE))]
#else
    weight_t weights[NAME_SUB(MODULE_NAME,_COARSE_IN)][NAME_SUB(MODULE_NAME,_COARSE_OUT)][DIVIDE(NAME_SUB(MODULE_NAME,_WEIGHTS),NAME_SUB(MODULE_NAME,_COARSE_IN)*NAME_SUB(MODULE_NAME,_COARSE_OUT)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE))][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE)]
#endif
)
{

#pragma HLS INLINE OFF 
#pragma HLS STREAM variable=in

    const unsigned int coarse_in    = NAME_SUB(MODULE_NAME,_COARSE_IN);
    const unsigned int coarse_out   = NAME_SUB(MODULE_NAME,_COARSE_OUT);
    const unsigned int filters      = DIVIDE(NAME_SUB(MODULE_NAME,_WEIGHTS),
            NAME_SUB(MODULE_NAME,_COARSE_IN)*NAME_SUB(MODULE_NAME,_COARSE_OUT)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE));
    const unsigned int kernel_size  = NAME_SUB(MODULE_NAME,_KERNEL_SIZE);

    coarse_in_loop: for(unsigned int coarse_in_index=0; coarse_in_index < coarse_in; coarse_in_index++) {
        coarse_out_loop: for(unsigned int coarse_out_index=0; coarse_out_index < coarse_out; coarse_out_index++) {
            filter_loop: for(unsigned int filter_index=0; filter_index < filters; filter_index++) {
#if NAME_SUB(MODULE_NAME,_KERNEL_SIZE) == 1
                #pragma HLS PIPELINE II=1
                #pragma HLS dependence variable=weights inter false
                weights[coarse_in_index][coarse_out_index][filter_index] = in.read();
#else
                kernel_1_loop: for(unsigned int k1_index=0; k1_index < kernel_size; k1_index++) {
                    kernel_2_loop: for(unsigned int k2_index=0; k2_index < kernel_size; k2_index++) {
                        #pragma HLS PIPELINE II=1
                        #pragma HLS dependence variable=weights inter false
                        weights[coarse_in_index][coarse_out_index][filter_index][k1_index][k2_index] = in.read();
                    }
                }
#endif                    
            }
        }
    }
}

#endif
