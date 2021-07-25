#ifndef SLIDING_WINDOW_TB_HPP_
#define SLIDING_WINDOW_TB_HPP_

#include "common.hpp"
#include "sliding_window_param.hpp"

#ifndef SLIDING_WINDOW_ROWS_OUT
#define SLIDING_WINDOW_ROWS_OUT GET_SW_ROWS_OUT( \
            SLIDING_WINDOW_ROWS, \
            SLIDING_WINDOW_KERNEL_SIZE, \
            SLIDING_WINDOW_STRIDE, \
            SLIDING_WINDOW_PAD, \
            SLIDING_WINDOW_PAD)
#endif

#ifndef SLIDING_WINDOW_COLS_OUT
#define SLIDING_WINDOW_COLS_OUT GET_SW_COLS_OUT( \
            SLIDING_WINDOW_COLS, \
            SLIDING_WINDOW_KERNEL_SIZE, \
            SLIDING_WINDOW_STRIDE, \
            SLIDING_WINDOW_PAD, \
            SLIDING_WINDOW_PAD)
#endif

//////////////////////////////////////////
typedef ap_fixed<SLIDING_WINDOW_WORDLENGTH,SLIDING_WINDOW_WORDLENGTH/2, AP_RND,AP_SAT> sliding_window_t;
void sliding_window_top(
    stream_t(sliding_window_t) &in,
    stream_t(sliding_window_t) out[SLIDING_WINDOW_KERNEL_SIZE][SLIDING_WINDOW_KERNEL_SIZE]
);

//////////////////////////////////////////

#endif
