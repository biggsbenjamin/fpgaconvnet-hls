#include "common_tb.hpp"
#include "pool_tb.hpp"

int main()
{

    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(data_t) in[POOL_KERNEL_SIZE_0][POOL_KERNEL_SIZE_1];
    stream_t(data_t) out;
    stream_t(data_t) out_valid;

    // test inputs data
    static data_t test_in[POOL_ROWS*POOL_COLS*POOL_CHANNELS][POOL_KERNEL_SIZE_0][POOL_KERNEL_SIZE_1];
    static data_t test_out[POOL_ROWS*POOL_COLS*POOL_CHANNELS];

    // load data_in
    load_data<
        POOL_ROWS*POOL_COLS*POOL_CHANNELS,
        POOL_KERNEL_SIZE_0,
        POOL_KERNEL_SIZE_1,
        data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        POOL_ROWS*POOL_COLS*POOL_CHANNELS,
        data_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        POOL_ROWS*POOL_COLS*POOL_CHANNELS,
        POOL_KERNEL_SIZE_0,
        POOL_KERNEL_SIZE_1,
        data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        POOL_ROWS*POOL_COLS*POOL_CHANNELS,
        data_t
    >(test_out,out_valid);

    // run pool
    pool_top(in,out);

    // check the stream is correct
    checkStreamEqual<data_t>(out,out_valid);

    return err;
}
