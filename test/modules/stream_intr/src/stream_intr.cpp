#include "stream_intr_tb.hpp"

#define MODULE_NAME STREAM_INTR
#define name        test
#include "stream_intr.hpp"

void stream_intr_top(
    stream_t(data_t) in[STREAM_INTR_COARSE][STREAM_INTR_KERNEL_SIZE][STREAM_INTR_KERNEL_SIZE],
#if STREAM_INTR_FINE == 1
    stream_t(data_t)  out[STREAM_INTR_COARSE]
#elif STREAM_INTR_FINE == STREAM_INTR_KERNEL_SIZE
    stream_t(data_t)  out[STREAM_INTR_COARSE][STREAM_INTR_KERNEL_SIZE]
#elif STREAM_INTR_FINE == STREAM_INTR_KERNEL_SIZE*STREAM_INTR_KERNEL_SIZE
    stream_t(data_t)  out[STREAM_INTR_COARSE][STREAM_INTR_KERNEL_SIZE][STREAM_INTR_KERNEL_SIZE]
#endif
)
{

    #pragma HLS DATAFLOW
    test_stream_intr(in,out);

}

#undef MODULE_NAME
#undef name
