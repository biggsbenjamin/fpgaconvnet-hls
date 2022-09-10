#include "common_tb.hpp"
#include "pooling_layer_tb.hpp"

int main()
{
    int err = 0;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(pooling_layer_data_t) in[POOLING_LAYER_COARSE];
    stream_t(pooling_layer_data_t) out[POOLING_LAYER_COARSE];
    stream_t(pooling_layer_data_t) out_correct[POOLING_LAYER_COARSE];

    const int size_in = CHANNELS_3D(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS*POOLING_LAYER_COLS;
    const int size_out = CHANNELS_3D(POOLING_LAYER_CHANNELS,POOLING_LAYER_COARSE)*POOLING_LAYER_ROWS_OUT*POOLING_LAYER_COLS_OUT;

    // test images
    static pooling_layer_data_t test_in[POOLING_LAYER_BATCH_SIZE*size_in][POOLING_LAYER_COARSE];
    static pooling_layer_data_t test_out[POOLING_LAYER_BATCH_SIZE*size_out][POOLING_LAYER_COARSE];

    std::cout << "data in size:" << size_in << std::endl;
    std::cout << "data out size:" << size_out << std::endl;
    // load input
    load_data_l<
        POOLING_LAYER_BATCH_SIZE,
        size_in,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(input_path,test_in);

    // load output
    load_data_l<
        POOLING_LAYER_BATCH_SIZE,
        size_out,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(output_path,test_out);

    // convert to streams
    to_stream_l<
        POOLING_LAYER_BATCH_SIZE,
        size_in,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(test_in,in);

    to_stream_l<
        POOLING_LAYER_BATCH_SIZE,
        size_out,
        POOLING_LAYER_COARSE,
        pooling_layer_data_t
    >(test_out,out_correct);

    pooling_layer_top(in,out,0);

    for(int i=0;i<POOLING_LAYER_COARSE_IN;i++) {
        //check output streams are empty
        bool in_empty = in[i].empty();
        std::cout<<"in stream "<<i<<" empty returned:"<<in_empty<<std::endl;
    }

    for(int i=0;i<POOLING_LAYER_COARSE;i++)
    {
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<pooling_layer_data_t>(out[i],out_correct[i],false);
        printf("%s\n",(err==0) ? "passed" : "failed");
        
        //check output streams are empty
        bool out_empty = out[i].empty();
        std::cout<<"out stream "<<i<<" empty returned:"<<out_empty<<std::endl;
    }

    return err;
}
