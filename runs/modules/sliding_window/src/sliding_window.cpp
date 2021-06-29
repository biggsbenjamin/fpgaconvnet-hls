#include "sliding_window_tb.hpp"
#include "sliding_window.hpp"

typedef ap_fixed<SLIDING_WINDOW_DATA_WORDLENGTH,SLIDING_WINDOW_DATA_WORDLENGTH_INTEGER,AP_RND,AP_SAT> sliding_window_t;
void sliding_window_top(
    stream_t(sliding_window_t) &in,
    stream_t(sliding_window_t) out[SLIDING_WINDOW_KERNEL_SIZE][SLIDING_WINDOW_KERNEL_SIZE]
)
{

    #pragma HLS DATAFLOW

    sliding_window<
        SLIDING_WINDOW_BATCH_SIZE,
        SLIDING_WINDOW_ROWS,
        SLIDING_WINDOW_COLS,
        SLIDING_WINDOW_CHANNELS,
        SLIDING_WINDOW_PAD_TOP,
        SLIDING_WINDOW_PAD_RIGHT,
        SLIDING_WINDOW_PAD_BOTTOM,
        SLIDING_WINDOW_PAD_LEFT,
        SLIDING_WINDOW_STRIDE,
        SLIDING_WINDOW_STRIDE,
        SLIDING_WINDOW_KERNEL_SIZE,
        sliding_window_t
    >(in,out);

}
