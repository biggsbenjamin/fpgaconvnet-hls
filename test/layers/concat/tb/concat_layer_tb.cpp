#include "common_tb.hpp"
#include "concat_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string data_path = std::string(DATA_DIR)+"/data.yaml";


    stream_t(data_t) in_0[CONCAT_LAYER_COARSE];
    data_t test_in_0[CONCAT_LAYER_CHANNELS_0*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load input
    load_data<
        CONCAT_LAYER_CHANNELS_0*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_in_0",test_in_0);

    // convert to streams
    to_stream<
        CONCAT_LAYER_CHANNELS_0*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_in_0,in_0);

    stream_t(data_t) in_1[CONCAT_LAYER_COARSE];
    data_t test_in_1[CONCAT_LAYER_CHANNELS_1*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load input
    load_data<
        CONCAT_LAYER_CHANNELS_1*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_in_1",test_in_1);

    // convert to streams
    to_stream<
        CONCAT_LAYER_CHANNELS_1*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_in_1,in_1);

    stream_t(data_t) in_2[CONCAT_LAYER_COARSE];
    data_t test_in_2[CONCAT_LAYER_CHANNELS_2*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load input
    load_data<
        CONCAT_LAYER_CHANNELS_2*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_in_2",test_in_2);

    // convert to streams
    to_stream<
        CONCAT_LAYER_CHANNELS_2*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_in_2,in_2);

    stream_t(data_t) in_3[CONCAT_LAYER_COARSE];
    data_t test_in_3[CONCAT_LAYER_CHANNELS_3*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load input
    load_data<
        CONCAT_LAYER_CHANNELS_3*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_in_3",test_in_3);

    // convert to streams
    to_stream<
        CONCAT_LAYER_CHANNELS_3*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_in_3,in_3);


    stream_t(data_t) out[CONCAT_LAYER_COARSE];
    stream_t(data_t) out_correct[CONCAT_LAYER_COARSE];

    // test images
    data_t test_out[CONCAT_LAYER_CHANNELS_OUT*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS][CONCAT_LAYER_COARSE];

    // load output
    load_data<
        CONCAT_LAYER_CHANNELS_OUT*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(data_path,"data_out",test_out);

    to_stream<
        CONCAT_LAYER_CHANNELS_OUT*CONCAT_LAYER_ROWS*CONCAT_LAYER_COLS,
        CONCAT_LAYER_COARSE
    >(test_out,out_correct);

    concat_layer_top(in_0, in_1, in_2, in_3,out,0);

    for(int i=0;i<CONCAT_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }

    return err;
}
