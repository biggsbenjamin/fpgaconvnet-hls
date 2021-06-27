#include "buffer_tb.hpp"
#include "buffer.hpp"

void buffer_top(
	stream_t(data_t) &in,
	stream_t(data_t) &ctrl_in,
	stream_t(data_t) &out
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
