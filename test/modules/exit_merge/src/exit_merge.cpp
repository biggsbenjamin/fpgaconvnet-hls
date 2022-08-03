#include "exit_merge_tb.hpp"
#include "exit_merge.hpp"

void exit_merge_top(
    stream_t(data_t) in[EXIT_MERGE_EXITS],
    stream_t(data_t) &out
)
{

#pragma HLS DATAFLOW

    // DUT
    exit_merge<
        EXIT_MERGE_BATCH_SIZE,
        EXIT_MERGE_ROWS,
        EXIT_MERGE_COLS,
        EXIT_MERGE_CHANNELS,
        EXIT_MERGE_EXITS                                 
        >(in, out);

}
