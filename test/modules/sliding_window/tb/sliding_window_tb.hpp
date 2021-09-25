#ifndef SLIDING_WINDOW_TB_HPP_
#define SLIDING_WINDOW_TB_HPP_

#include "common.hpp"
#include "sliding_window_param.hpp"

#ifndef SLIDING_WINDOW_ROWS_OUT
#define SLIDING_WINDOW_ROWS_OUT GET_SW_ROWS_OUT( \
            SLIDING_WINDOW_ROWS, \
            SLIDING_WINDOW_KERNEL_SIZE_1, \
            SLIDING_WINDOW_STRIDE_1, \
            SLIDING_WINDOW_PAD, \
            SLIDING_WINDOW_PAD)
#endif

#ifndef SLIDING_WINDOW_COLS_OUT
#define SLIDING_WINDOW_COLS_OUT GET_SW_COLS_OUT( \
            SLIDING_WINDOW_COLS, \
            SLIDING_WINDOW_KERNEL_SIZE_0, \
            SLIDING_WINDOW_STRIDE_0, \
            SLIDING_WINDOW_PAD, \
            SLIDING_WINDOW_PAD)
#endif

//////////////////////////////////////////

void sliding_window_top(
    stream_t(data_t) &in,
    stream_t(data_t) out[SLIDING_WINDOW_KERNEL_SIZE_0][SLIDING_WINDOW_KERNEL_SIZE_1]
);

//////////////////////////////////////////

#endif
