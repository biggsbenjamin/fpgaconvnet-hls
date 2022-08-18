#include "common_tb.hpp"
#include "inner_product_layer_tb.hpp"

int main()
{
    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    //NOTE should both be 1 anyway
    const int rows_out = INNER_PRODUCT_LAYER_ROWS_OUT;
    const int cols_out = INNER_PRODUCT_LAYER_COLS_OUT;

    stream_t(inner_product_layer_input_t) in[INNER_PRODUCT_LAYER_COARSE_IN];
    stream_t(inner_product_layer_output_t) out[INNER_PRODUCT_LAYER_COARSE_OUT];
    stream_t(inner_product_layer_output_t) out_correct[INNER_PRODUCT_LAYER_COARSE_OUT];

    // test images
    const int data_in_size = CHANNELS_3D(INNER_PRODUCT_LAYER_CHANNELS,INNER_PRODUCT_LAYER_COARSE_IN)*INNER_PRODUCT_LAYER_ROWS*INNER_PRODUCT_LAYER_COLS;
    const int data_out_size = FILTERS_PER_UNIT(INNER_PRODUCT_LAYER_FILTERS,INNER_PRODUCT_LAYER_COARSE_OUT)*rows_out*cols_out;

    static inner_product_layer_input_t test_in[data_in_size][INNER_PRODUCT_LAYER_COARSE_IN];
    static inner_product_layer_output_t test_out[data_out_size][INNER_PRODUCT_LAYER_COARSE_OUT];

    std::cout << "data in size:" << data_in_size << std::endl;
    std::cout << "data out size:" << data_out_size << std::endl;
    const bool debug = true;

    // load input
    load_data<
        data_in_size,
        INNER_PRODUCT_LAYER_COARSE_IN,
        inner_product_layer_input_t
    >(input_path,test_in);

    // load output
    load_data<
        data_out_size,
        INNER_PRODUCT_LAYER_COARSE_OUT,
        inner_product_layer_output_t
    >(output_path,test_out);

    // convert to streams
    to_stream<
        data_in_size,
        INNER_PRODUCT_LAYER_COARSE_IN,
        //debug,
        inner_product_layer_input_t
    >(test_in,in);

    to_stream<
        data_out_size,
        INNER_PRODUCT_LAYER_COARSE_OUT,
        //debug,
        inner_product_layer_output_t
    >(test_out,out_correct);

    inner_product_layer_top(in,out,0);

    for(int i=0;i<INNER_PRODUCT_LAYER_COARSE_IN;i++) {
        //check output streams are empty
        bool in_empty = in[i].empty();
        std::cout<<"in stream "<<i<<" empty returned:"<<in_empty<<std::endl;
    }

    for(int i=0;i<INNER_PRODUCT_LAYER_COARSE_OUT;i++)
    {
        printf("TESTING OUTPUT %d: \n",i);
        err += checkStreamEqual<inner_product_layer_output_t>(out[i],out_correct[i]);
        printf("%s\n",(err==0) ? "passed" : "failed");

        //check output streams are empty
        bool out_empty = out[i].empty();
        std::cout<<"out stream "<<i<<" empty returned:"<<out_empty<<std::endl;

    }

    return err;
}
