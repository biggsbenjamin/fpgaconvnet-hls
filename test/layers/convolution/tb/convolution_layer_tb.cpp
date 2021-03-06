#include "common_tb.hpp"
#include "convolution_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    const int rows_out = CONVOLUTION_LAYER_ROWS_OUT;
    const int cols_out = CONVOLUTION_LAYER_COLS_OUT;

    stream_t(data_t) in[CONVOLUTION_LAYER_COARSE_IN];
    stream_t(data_t) out[CONVOLUTION_LAYER_COARSE_OUT];
    stream_t(data_t) out_correct[CONVOLUTION_LAYER_COARSE_OUT];

    // test images
    static data_t test_in[DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN)*CONVOLUTION_LAYER_ROWS*CONVOLUTION_LAYER_COLS][CONVOLUTION_LAYER_COARSE_IN];
    static data_t test_out[DIVIDE(CONVOLUTION_LAYER_CHANNELS_OUT,CONVOLUTION_LAYER_COARSE_OUT)*rows_out*cols_out][CONVOLUTION_LAYER_COARSE_OUT];

    // load input
    load_data<
        DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN)*CONVOLUTION_LAYER_ROWS*CONVOLUTION_LAYER_COLS,
        CONVOLUTION_LAYER_COARSE_IN
    >(input_path,test_in);

    // load output
    load_data<
        DIVIDE(CONVOLUTION_LAYER_CHANNELS_OUT,CONVOLUTION_LAYER_COARSE_OUT)*rows_out*cols_out,
        CONVOLUTION_LAYER_COARSE_OUT
    >(output_path,test_out);

    // convert to streams
    to_stream<
        DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN)*CONVOLUTION_LAYER_ROWS*CONVOLUTION_LAYER_COLS,
        CONVOLUTION_LAYER_COARSE_IN
    >(test_in,in);

    to_stream<
        DIVIDE(CONVOLUTION_LAYER_CHANNELS_OUT,CONVOLUTION_LAYER_COARSE_OUT)*rows_out*cols_out,
        CONVOLUTION_LAYER_COARSE_OUT
    >(test_out,out_correct);

    convolution_layer_top(in,out,0);

    for(int i=0;i<CONVOLUTION_LAYER_COARSE_OUT;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i],true);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
