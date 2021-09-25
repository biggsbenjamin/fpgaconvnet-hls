#include "common_tb.hpp"
#include "conv_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path      = std::string(DATA_DIR)+"/input.dat";
    std::string output_path     = std::string(DATA_DIR)+"/output.dat";
    std::string weights_path    = std::string(DATA_DIR)+"/weights.dat";

    // weights
#if CONV_KERNEL_SIZE == 1
    weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)];
#else
    weight_t weights[CONV_CHANNELS*DIVIDE(CONV_FILTERS,CONV_GROUP)][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1];
#endif

    stream_t(data_t) in[CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1];
    stream_t(acc_t) out("out");
    stream_t(acc_t) out_valid("out_valid");

    // test inputs data
    static data_t test_in[CONV_ROWS*CONV_COLS*CONV_CHANNELS][CONV_KERNEL_SIZE_0][CONV_KERNEL_SIZE_1];
    static acc_t test_out[CONV_ROWS*CONV_COLS*CONV_CHANNELS*FILTERS_PER_UNIT(CONV_FILTERS,CONV_GROUP)];

    // load weights
    load_data<
        CONV_CHANNELS*FILTERS_PER_UNIT(CONV_FILTERS,CONV_GROUP),
#if CONV_KERNEL_SIZE != 1
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
#endif
        weight_t
    >(weights_path,weights);

    // load data_in
    load_data<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS,
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
        data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS*FILTERS_PER_UNIT(CONV_FILTERS,CONV_GROUP),
        acc_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS,
        CONV_KERNEL_SIZE_0,
        CONV_KERNEL_SIZE_1,
        data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        CONV_ROWS*CONV_COLS*CONV_CHANNELS*FILTERS_PER_UNIT(CONV_FILTERS,CONV_GROUP),
        acc_t
    >(test_out,out_valid);

    // run conv
    conv_top(in,weights,out);

    // check output
    err += checkStreamEqual<acc_t>(out,out_valid);

    return err;

}
