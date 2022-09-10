#include "common_tb.hpp"
#include "convolution_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    const int rows_out = CONVOLUTION_LAYER_ROWS_OUT;
    const int cols_out = CONVOLUTION_LAYER_COLS_OUT;
    const int batch_size = CONVOLUTION_LAYER_BATCH_SIZE; 
    const unsigned int input_size = DIVIDE(CONVOLUTION_LAYER_CHANNELS,CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP)*CONVOLUTION_LAYER_ROWS*CONVOLUTION_LAYER_COLS;
    const unsigned int output_size = DIVIDE(CONVOLUTION_LAYER_CHANNELS_OUT,CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP)*rows_out*cols_out;

    stream_t(convolution_layer_input_t) in[CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP];
    stream_t(convolution_layer_output_t) out[CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP];
    stream_t(convolution_layer_output_t) out_correct[CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP];


    // test images
    static convolution_layer_input_t test_in[input_size*CONVOLUTION_LAYER_BATCH_SIZE][CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP];
    static convolution_layer_output_t test_out[output_size*CONVOLUTION_LAYER_BATCH_SIZE][CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP];

    printf("input size:%d\n",input_size);
    printf("output size:%d\n",output_size);
    printf("batch:%d\n",batch_size);
    // load input
    load_data_l<
        CONVOLUTION_LAYER_BATCH_SIZE,
        input_size,
        CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP,
        convolution_layer_input_t
    >(input_path,test_in);

    // load output
    load_data_l<
        CONVOLUTION_LAYER_BATCH_SIZE,
        output_size,
        CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP,
        convolution_layer_output_t
    >(output_path,test_out);

    // convert to streams
    to_stream_l<
        CONVOLUTION_LAYER_BATCH_SIZE,
        input_size,
        CONVOLUTION_LAYER_COARSE_IN*CONVOLUTION_LAYER_COARSE_GROUP,
        convolution_layer_input_t
    >(test_in,in);

    to_stream_l<
        CONVOLUTION_LAYER_BATCH_SIZE,
        output_size,
        CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP,
        convolution_layer_output_t
    >(test_out,out_correct);

    convolution_layer_top(in,out,0);

    for(int i=0;i<CONVOLUTION_LAYER_COARSE_OUT*CONVOLUTION_LAYER_COARSE_GROUP;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<convolution_layer_output_t>(out[i], out_correct[i], false);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
