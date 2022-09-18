#include "common_tb.hpp"
#include "accum_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    const unsigned int in_size = (int)(ACCUM_BATCH_SIZE*ACCUM_ROWS*ACCUM_COLS*ACCUM_CHANNELS*ACCUM_FILTERS/ACCUM_GROUPS);
    const unsigned int out_size = ACCUM_BATCH_SIZE*ACCUM_ROWS*ACCUM_COLS*ACCUM_FILTERS;

    stream_t(test_accum_t) in;
    stream_t(test_accum_t) out;

    stream_t(test_accum_t) out_valid;

    static test_accum_t test_in[in_size];
    static test_accum_t test_out[out_size];

    // load data_in
    load_data<
        in_size,
        test_accum_t
    >(input_path,test_in);

    // load data_out
    load_data<
        out_size,
        test_accum_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        in_size,
        test_accum_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        out_size,
        test_accum_t
    >(test_out,out_valid);

    // run conv
    accum_top(in,out);

    // check output
    err += checkStreamEqual<test_accum_t>(out,out_valid);

    return err;
}
