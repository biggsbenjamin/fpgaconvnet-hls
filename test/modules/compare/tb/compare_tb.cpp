#include "common_tb.hpp"
#include "compare_tb.hpp"

int main()
{

    int err = 0;
    std::string input_path_max  = std::string(DATA_DIR)+"/input_max.dat";
    std::string input_path_thr  = std::string(DATA_DIR)+"/input_thr.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    //stream_t(data_t) in;
    //stream_t(data_t) out;
    //stream_t(data_t) out_valid;
    hls::stream<float> max_in;
    hls::stream<float> thr_in;
    stream_t(data_t) out;
    stream_t(data_t) out_valid;

    // test inputs data
    //static data_t test_in[COMPARE_ROWS*COMPARE_COLS*COMPARE_CHANNELS];
    //static data_t test_out[COMPARE_ROWS*COMPARE_COLS*COMPARE_CHANNELS];
    static float test_max_in[COMPARE_BATCH_SIZE];
    static float test_thr_in[COMPARE_BATCH_SIZE];
    static float test_out[COMPARE_BATCH_SIZE];

    // load data_in
    load_data<
        COMPARE_BATCH_SIZE,
        float
    >(input_path_max,test_max_in);
    load_data<
        COMPARE_BATCH_SIZE,
        float
    >(input_path_thr,test_thr_in);

    // load data_out
    load_data<
        COMPARE_BATCH_SIZE,
        float
    >(output_path,test_out);

    // convert input stream
    to_stream<
        COMPARE_BATCH_SIZE,
        float
    >(test_max_in,max_in);
    to_stream<
        COMPARE_BATCH_SIZE,
        float
    >(test_thr_in,thr_in);

    // convert to out valid stream
    to_stream<
        COMPARE_BATCH_SIZE,
        float
    >(test_out,out_valid);

    // run compare
    compare_top(max_in, thr_in, out);

    printf("\r\n\t COMPARE #1\r\n");
    err += checkStreamEqual <data_t> (out,out_valid,false);
    //err += checkStreamEqual <float> (out,out_valid,true);
    
    return err;
}
