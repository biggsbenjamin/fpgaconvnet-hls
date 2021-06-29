#include "common_tb.hpp"
#include "split_layer_tb.hpp"

int main()
{
    int err = 0;

    const int in_size = CHANNELS_3D(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS;
    const int out_size = CHANNELS_3D(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS*SPLIT_LAYER_PORTS_OUT;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(data_t) in[SPLIT_LAYER_COARSE];
    stream_t(data_t) out[SPLIT_LAYER_COARSE][SPLIT_LAYER_PORTS_OUT];
    stream_t(data_t) out_correct[SPLIT_LAYER_COARSE][SPLIT_LAYER_PORTS_OUT];

    // test images
    static data_t test_in[in_size][SPLIT_LAYER_COARSE];
    static data_t test_out[in_size][SPLIT_LAYER_COARSE][SPLIT_LAYER_PORTS_OUT];

    // load input
    load_data<
        in_size,
        SPLIT_LAYER_COARSE
    >(input_path,test_in);

    // load output - using GLUE template
    load_data<
        in_size,
        SPLIT_LAYER_COARSE,
        SPLIT_LAYER_PORTS_OUT
    >(output_path,test_out);

    // convert to streams
    to_stream<
        in_size,
        SPLIT_LAYER_COARSE
    >(test_in,in);

    to_stream<
        in_size,
        SPLIT_LAYER_COARSE,
        SPLIT_LAYER_PORTS_OUT
    >(test_out,out_correct);

    split_layer_top(in,out);

    for(int i=0;i<SPLIT_LAYER_COARSE;i++){
        for(int j=0;j<SPLIT_LAYER_PORTS_OUT;j++) {
            printf("TESTING OUTPUT %d: ",i, "-", j);
            err += checkStreamEqual<data_t>(out[i][j],out_correct[i][j],false);
            printf("%s\n",(err==0) ? "passed" : "failed");
        }
    }

    return err;
}
