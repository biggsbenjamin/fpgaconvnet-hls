#include "common_tb.hpp"
#include "mem_read_tb.hpp"

int main()
{

    int err = 0;
    std::string data_path = std::string(DATA_DIR)+"/data.yaml";

    const int size_in = MEM_READ_BATCH_SIZE*MEM_READ_ROWS_IN*MEM_READ_COLS_IN*DIVIDE(MEM_READ_CHANNELS_IN,MEM_READ_STREAMS_IN);

    // in/out streams
    stream_t(data_t) out[MEM_READ_STREAMS_IN];
    stream_t(data_t) out_valid[MEM_READ_STREAMS_IN];

    // test inputs data
    static mem_int test_in[MEM_READ_PORTS_IN][size_in] = {0};
    static data_t test_out[size_in][MEM_READ_STREAMS_IN] = {0};

    // load data_in
    load_net_data<
        MEM_READ_PORTS_IN,
        MEM_READ_BATCH_SIZE,
        MEM_READ_ROWS_IN,
        MEM_READ_COLS_IN,
        MEM_READ_CHANNELS_IN,
        MEM_READ_STREAMS_IN
    >(data_path,"data_in",test_in);

    // load data_out
    load_data<
        size_in,
        MEM_READ_STREAMS_IN
    >(data_path,"data_out",test_out);

    // convert to out valid stream
    to_stream<
        size_in,
        MEM_READ_STREAMS_IN
    >(test_out,out_valid);

    // run mem read
    mem_read_top(test_in,out);

    for(int i=0;i<MEM_READ_STREAMS_IN;i++) {
        err += checkStreamEqual<data_t>(out[i],out_valid[i],true);
    }

    return err;
}
