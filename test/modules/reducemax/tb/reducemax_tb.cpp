#include "common_tb.hpp"
#include "reducemax_tb.hpp"

int main()
{

    int err = 0;
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    //stream_t(data_t) in;
    //stream_t(data_t) out;
    //stream_t(data_t) out_valid;
    hls::stream<float> in;
    hls::stream<float> out;
    hls::stream<float> out_valid;

    // test inputs data
    //static data_t test_in[REDUCEMAX_ROWS*REDUCEMAX_COLS*REDUCEMAX_CHANNELS];
    //static data_t test_out[REDUCEMAX_ROWS*REDUCEMAX_COLS*REDUCEMAX_CHANNELS];
    static float test_in[REDUCEMAX_BATCH_SIZE*REDUCEMAX_ROWS*REDUCEMAX_COLS*REDUCEMAX_CHANNELS];
    static float test_out[REDUCEMAX_BATCH_SIZE];

    // load data_in
    load_data<
        REDUCEMAX_BATCH_SIZE*REDUCEMAX_ROWS*REDUCEMAX_COLS*REDUCEMAX_CHANNELS,
        //data_t
        float
    >(input_path,test_in);

    // load data_out
    load_data<
        REDUCEMAX_BATCH_SIZE,
        //data_t
        float
    >(output_path,test_out);

    // convert input stream
    to_stream<
        REDUCEMAX_BATCH_SIZE*REDUCEMAX_ROWS*REDUCEMAX_COLS*REDUCEMAX_CHANNELS,
        //data_t
        float
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        REDUCEMAX_BATCH_SIZE,
        //data_t
        float
    >(test_out,out_valid);

    // run reducemax
    reducemax_top(in,out);

    printf("\r\n\t REDUCEMAX #1\r\n");
    //err += checkStreamEqual <data_t> (out,out_valid,false);
    err += checkStreamEqual <float> (out,out_valid,true);
    
    return err;
}
