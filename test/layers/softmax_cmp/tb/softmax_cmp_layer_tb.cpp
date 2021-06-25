#include "common_tb.hpp"
#include "softmax_cmp_layer_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(data_t) in;
    stream_t(data_t) out[SOFTMAX_CMP_LAYER_CTRL];
    stream_t(data_t) out_correct[SOFTMAX_CMP_LAYER_CTRL];

    // test images
    static data_t test_in[SOFTMAX_CMP_LAYER_CHANNELS*SOFTMAX_CMP_LAYER_ROWS*SOFTMAX_CMP_LAYER_COLS];
    static data_t test_out[SOFTMAX_CMP_LAYER_CHANNELS*SOFTMAX_CMP_LAYER_COARSE*SOFTMAX_CMP_LAYER_ROWS_OUT*SOFTMAX_CMP_LAYER_COLS_OUT][SOFTMAX_CMP_LAYER_CTRL];

    // load input
    load_data<SOFTMAX_CMP_LAYER_CHANNELS*SOFTMAX_CMP_LAYER_ROWS*SOFTMAX_CMP_LAYER_COLS,
        //SOFTMAX_CMP_LAYER_COARSE
        data_t
    >(input_path,test_in);

    // load output
    load_data<SOFTMAX_CMP_LAYER_BATCH_SIZE,
        SOFTMAX_CMP_LAYER_CTRL
    >(output_path,test_out);

    // convert to streams
    to_stream<SOFTMAX_CMP_LAYER_CHANNELS*SOFTMAX_CMP_LAYER_ROWS*SOFTMAX_CMP_LAYER_COLS,
        //SOFTMAX_CMP_LAYER_COARSE
        data_t
    >(test_in,in);

    to_stream<SOFTMAX_CMP_LAYER_BATCH_SIZE,
        SOFTMAX_CMP_LAYER_CTRL
    >(test_out,out_correct);

    softmax_cmp_layer_top(in,out);
    
    for(int i=0;i<SOFTMAX_CMP_LAYER_CTRL;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i],true);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
