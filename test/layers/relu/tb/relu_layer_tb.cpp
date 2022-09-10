#include "common_tb.hpp"
#include "relu_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(relu_layer_data_t) in[RELU_LAYER_COARSE];
    stream_t(relu_layer_data_t) out[RELU_LAYER_COARSE];
    stream_t(relu_layer_data_t) out_correct[RELU_LAYER_COARSE];

    const unsigned int in_size = DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS*RELU_LAYER_COLS;
    const unsigned int out_size = DIVIDE(RELU_LAYER_CHANNELS,RELU_LAYER_COARSE)*RELU_LAYER_ROWS_OUT*RELU_LAYER_COLS_OUT;

    // test images
    static relu_layer_data_t test_in[RELU_LAYER_BATCH_SIZE*in_size][RELU_LAYER_COARSE];
    static relu_layer_data_t test_out[RELU_LAYER_BATCH_SIZE*out_size][RELU_LAYER_COARSE];

    // load input
    load_data_l<
        RELU_LAYER_BATCH_SIZE,
        in_size,
        RELU_LAYER_COARSE,
        relu_layer_data_t
    >(input_path,test_in);

    // load output
    load_data_l<
        RELU_LAYER_BATCH_SIZE,
        out_size,
        RELU_LAYER_COARSE,
        relu_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream_l<
        RELU_LAYER_BATCH_SIZE,
        in_size,
        RELU_LAYER_COARSE,
        relu_layer_data_t
    >(test_in,in);

    to_stream_l<
        RELU_LAYER_BATCH_SIZE,
        out_size,
        RELU_LAYER_COARSE,
        relu_layer_data_t
    >(test_out,out_correct);

    relu_layer_top(in,out,0);

    for(int i=0;i<RELU_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<relu_layer_data_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
