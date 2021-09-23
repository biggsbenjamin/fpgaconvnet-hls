#ifndef WR_HPP_
#define WR_HPP_

#include "common.hpp"

/**
 *  WEIGHTS RELOADING FUNCTION
 */
template<int _>
void NAME_SUB(name,_wr)(
    stream_t(weight_t) &in,
    weight_t weights[NAME_SUB(MODULE_NAME,_COARSE_IN)*NAME_SUB(MODULE_NAME,_COARSE_GROUP)][NAME_SUB(MODULE_NAME,_COARSE_OUT)][DIVIDE(NAME_SUB(MODULE_NAME,_WEIGHTS),NAME_SUB(MODULE_NAME,_COARSE_IN)*NAME_SUB(MODULE_NAME,_COARSE_GROUP)*NAME_SUB(MODULE_NAME,_COARSE_OUT)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE_X)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE_Y))][NAME_SUB(MODULE_NAME,_KERNEL_SIZE_X)][NAME_SUB(MODULE_NAME,_KERNEL_SIZE_Y)]
)
{

#pragma HLS INLINE OFF 
#pragma HLS STREAM variable=in

    const unsigned int coarse_in     = NAME_SUB(MODULE_NAME,_COARSE_IN);
    const unsigned int coarse_out    = NAME_SUB(MODULE_NAME,_COARSE_OUT);
    const unsigned int coarse_group  = NAME_SUB(MODULE_NAME,_COARSE_GROUP);
    const unsigned int filters       = DIVIDE(NAME_SUB(MODULE_NAME,_WEIGHTS),
            NAME_SUB(MODULE_NAME,_COARSE_IN)*NAME_SUB(MODULE_NAME,_COARSE_GROUP)*NAME_SUB(MODULE_NAME,_COARSE_OUT)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE_X)*NAME_SUB(MODULE_NAME,_KERNEL_SIZE_Y));
    const unsigned int kernel_size_x = NAME_SUB(MODULE_NAME,_KERNEL_SIZE_X);
    const unsigned int kernel_size_y = NAME_SUB(MODULE_NAME,_KERNEL_SIZE_Y);

    coarse_in_loop: for(unsigned int coarse_in_index=0; coarse_in_index < coarse_in*coarse_group; coarse_in_index++) {
        coarse_out_loop: for(unsigned int coarse_out_index=0; coarse_out_index < coarse_out; coarse_out_index++) {
            filter_loop: for(unsigned int filter_index=0; filter_index < filters; filter_index++) {
                kernel_1_loop: for(unsigned int k1_index=0; k1_index < kernel_size_x; k1_index++) {
                    kernel_2_loop: for(unsigned int k2_index=0; k2_index < kernel_size_y; k2_index++) {
                        #pragma HLS PIPELINE II=1
                        #pragma HLS dependence variable=weights inter false
                        weights[coarse_in_index][coarse_out_index][filter_index][k1_index][k2_index] = in.read();
                    }
                }             
            }
        }
    }
}

#endif
