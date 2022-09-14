#include "buffer_tb.hpp"
#include "buffer.hpp"

void buffer_top(
	stream_t(buffer_t) &in,
	stream_t(ctrl_t) &ctrl_in,
	stream_t(buffer_t) &out
)
{

#pragma HLS DATAFLOW

    buffer<
        BUFFER_BATCH_SIZE,
        BUFFER_ROWS,
        BUFFER_COLS,
        BUFFER_CHANNELS,
        BUFFER_DROP_MODE
    >(in,ctrl_in,out);

}
