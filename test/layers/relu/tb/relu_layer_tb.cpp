#include "common_tb.hpp"
#include "relu_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(data_t) in[RELU_LAYER_COARSE];
    stream_t(data_t) out[RELU_LAYER_COARSE];
    stream_t(data_t) out_correct[RELU_LAYER_COARSE];

    // test images
    static data_t test_in[DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS*RELU_LAYER_COLS][RELU_LAYER_COARSE];
    static data_t test_out[DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS_OUT*RELU_LAYER_COLS_OUT][RELU_LAYER_COARSE];

    // load input
    load_data<
        DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS*RELU_LAYER_COLS,
        RELU_LAYER_COARSE
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS_OUT*RELU_LAYER_COLS_OUT,
        RELU_LAYER_COARSE
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS*RELU_LAYER_COLS,
        RELU_LAYER_COARSE
    >(test_in,in);

    to_stream<
        DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS_OUT*RELU_LAYER_COLS_OUT,
        RELU_LAYER_COARSE
    >(test_out,out_correct);

    relu_layer_top(in,out,0);

    for(int i=0;i<RELU_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
