#include "common_tb.hpp"
#include "exit_merge_tb.hpp"

int main()
{

    int err = 0;
    std::string input0_path  = std::string(DATA_DIR)+"/input0.dat";
    std::string input1_path  = std::string(DATA_DIR)+"/input1.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(data_t) in[EXIT_MERGE_EXITS];
    stream_t(data_t) out;
    stream_t(data_t) out_valid;

    const unsigned int size_in = EXIT_MERGE_ROWS*EXIT_MERGE_COLS*EXIT_MERGE_CHANNELS;
    const unsigned int size_out = EXIT_MERGE_BATCH_SIZE*EXIT_MERGE_ROWS*EXIT_MERGE_COLS*EXIT_MERGE_CHANNELS;

    // test inputs data
    static data_t test_in0[size_in*EXIT_MERGE_EX0_IN];
    static data_t test_in1[size_in*EXIT_MERGE_EX1_IN];
    static data_t test_out[size_out];

    // load data_in0
    load_data<
        size_in*EXIT_MERGE_EX0_IN,
        data_t
    >(input0_path,test_in0);
    // load data_in1
    load_data<
        size_in*EXIT_MERGE_EX1_IN,
        data_t
    >(input1_path,test_in1);

    // load data_out
    load_data<
        size_out,
        data_t
    >(output_path,test_out);

    // convert input stream0
    to_stream<
        size_in*EXIT_MERGE_EX0_IN,
        data_t
    >(test_in0,in[0]);
    // convert input stream1
    to_stream<
        size_in*EXIT_MERGE_EX1_IN,
        data_t
    >(test_in1,in[1]);

    // convert to out valid stream
    to_stream<
        size_out,
        data_t
    >(test_out,out_valid);

    // run es 
    exit_merge_top(in, out);

    printf("\r\n\t EXIT_MERGE #1\r\n");
    err += checkStreamEqual <data_t> (out,out_valid,false);
    //err += checkStreamEqual <float> (out,out_valid,true);
    
    return err;
}
