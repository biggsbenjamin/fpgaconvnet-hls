#include "mem_read_tb.hpp"

#define MODULE_NAME MEM_READ
#define name        test
#include "mem_read.hpp"

void mem_read_top(
    volatile mem_int in_hw[MEM_READ_PORTS_IN][MEM_READ_BATCH_SIZE*MEM_READ_ROWS_IN*MEM_READ_COLS_IN*DIVIDE(MEM_READ_CHANNELS_IN,MEM_READ_STREAMS_IN)],
    stream_t(data_t) in[MEM_READ_STREAMS_IN]
)
{
    const int size_in = MEM_READ_BATCH_SIZE*MEM_READ_ROWS_IN*MEM_READ_COLS_IN*DIVIDE(MEM_READ_CHANNELS_IN,MEM_READ_STREAMS_IN);
    //DO_PRAGMA( HLS INTERFACE m_axi port=in_hw[0] depth=size_in num_read_outstanding=1 max_read_burst_length=256)
    DO_PRAGMA( HLS INTERFACE m_axi port=in_hw depth=size_in num_read_outstanding=1 max_read_burst_length=256)
#pragma HLS ARRAY_PARTITION variable=in_hw complete dim=1
    //DO_PRAGMA( HLS INTERFACE m_axi port=in_hw_1 depth=size_in num_read_outstanding=1 max_read_burst_length=256)
    //DO_PRAGMA( HLS INTERFACE m_axi port=in_hw_2 depth=size_in num_read_outstanding=1 max_read_burst_length=256)
    //DO_PRAGMA( HLS INTERFACE m_axi port=in_hw_3 depth=size_in num_read_outstanding=1 max_read_burst_length=256)
    #pragma HLS INTERFACE axis port=in

    #pragma HLS DATAFLOW
    test_mem_read(in_hw,in);
}

#undef MODULE_NAME
#undef name
