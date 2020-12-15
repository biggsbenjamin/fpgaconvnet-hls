#include "common_tb.hpp"
#include "relu_tb.hpp"

int main()
{

    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(data_t) in;
    stream_t(data_t) out;
    stream_t(data_t) out_valid;

    // test inputs data
    static data_t test_in[RELU_ROWS*RELU_COLS*RELU_CHANNELS];
    static data_t test_out[RELU_ROWS*RELU_COLS*RELU_CHANNELS];

    // load data_in
    load_data<
        RELU_ROWS*RELU_COLS*RELU_CHANNELS,
        data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        RELU_ROWS*RELU_COLS*RELU_CHANNELS,
        data_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        RELU_ROWS*RELU_COLS*RELU_CHANNELS,
        data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        RELU_ROWS*RELU_COLS*RELU_CHANNELS,
        data_t
    >(test_out,out_valid);

    // run relu
    relu_top(in,out);

    printf("\r\n\t RELU #1\r\n");
    err += checkStreamEqual<data_t>(out,out_valid,false);
    
    return err;
}
