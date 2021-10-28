#include "common_tb.hpp"
#include "bias_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path      = std::string(DATA_DIR)+"/input.dat";
    std::string output_path     = std::string(DATA_DIR)+"/output.dat";
    std::string weights_path    = std::string(DATA_DIR)+"/weights.dat";

    // weights
    //bias_weight_t weights[BIAS_CHANNELS*DIVIDE(BIAS_FILTERS,BIAS_GROUPS)];
    bias_weight_t weights[BIAS_FILTERS];

    stream_t(bias_data_t) in[BIAS_KERNEL_COARSE_OUT];
    stream_t(bias_acc_t) out("out");
    stream_t(bias_acc_t) out_valid("out_valid");

    // test inputs data
    static bias_data_t test_in[BIAS_ROWS*BIAS_COLS*BIAS_CHANNELS][BIAS_COARSE_OUT];
    static bias_acc_t test_out[BIAS_ROWS*BIAS_COLS*BIAS_FILTERS][BIAS_COARSE_OUT]; //TODO check

    // load weights
    load_data<
        BIAS_CHANNELS*DIVIDE(BIAS_FILTERS,BIAS_GROUPS),
        BIAS_KERNEL_SIZE_0,
        BIAS_KERNEL_SIZE_1,
        bias_weight_t
    >(weights_path,weights);

    // load data_in
    load_data<
        BIAS_ROWS*BIAS_COLS*BIAS_CHANNELS,
        BIAS_KERNEL_SIZE_0,
        BIAS_KERNEL_SIZE_1,
        bias_data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        BIAS_ROWS*BIAS_COLS*BIAS_CHANNELS*DIVIDE(BIAS_FILTERS,BIAS_GROUPS),
        bias_acc_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        BIAS_ROWS*BIAS_COLS*BIAS_CHANNELS,
        BIAS_KERNEL_SIZE_0,
        BIAS_KERNEL_SIZE_1,
        bias_data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        BIAS_ROWS*BIAS_COLS*BIAS_CHANNELS*DIVIDE(BIAS_FILTERS,BIAS_GROUPS),
        bias_acc_t
    >(test_out,out_valid);

    // run bias
    bias_top(in,weights,out);

    // check output
    err += checkStreamEqual<bias_acc_t>(out,out_valid);

    return err;

}
