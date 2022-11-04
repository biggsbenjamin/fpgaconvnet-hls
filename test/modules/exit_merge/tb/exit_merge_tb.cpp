#include "common_tb.hpp"
#include "exit_merge_tb.hpp"

int main()
{

    int err = 0;
    std::string input0_path  = std::string(DATA_DIR)+"/input0.dat";
    std::string input1_path  = std::string(DATA_DIR)+"/input1.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    const unsigned int fm_size = EXIT_MERGE_ROWS*EXIT_MERGE_COLS*EXIT_MERGE_CHANNELS;
    const unsigned int size_out = EXIT_MERGE_BATCH_SIZE*EXIT_MERGE_ROWS*EXIT_MERGE_COLS*EXIT_MERGE_CHANNELS;

    // test inputs data
    static em_t test_in0[fm_size*EXIT_MERGE_EX0_IN];
    static em_t test_in1[fm_size*EXIT_MERGE_EX1_IN];
    static em_t test_out[size_out];

    // in/out streams
    stream_t(em_t) in[EXIT_MERGE_EXITS];
    stream_t(em_t) out;
    stream_t(em_t) out_valid;

    std::cout<< "ex0 in:"<<EXIT_MERGE_EX0_IN
        <<" ex1 in:"<<EXIT_MERGE_EX1_IN
        <<" bs:"<<EXIT_MERGE_BATCH_SIZE
        <<std::endl;

    // load data_in0
    load_data_em<
        EXIT_MERGE_EX0_IN,
        fm_size,
        0,
        em_t
    >(input0_path,test_in0);
    // load data_in1
    load_data_em<
        EXIT_MERGE_EX1_IN,
        fm_size,
        EXIT_MERGE_EX0_IN, // offsetting by first exit batch size, CSIM concats?
        em_t
    >(input1_path,test_in1);

    // load data_out - FIXME values not strictly of any use
    load_data_buff<
        EXIT_MERGE_BATCH_SIZE,
        fm_size,
        em_t
    >(output_path,test_out);

    // convert input stream0
    to_stream<//_ex<
        fm_size*EXIT_MERGE_EX0_IN,
        //(fm_size-1),
        em_t
    >(test_in0,in[0]);
    // convert input stream1
    to_stream<//_ex<
        fm_size*EXIT_MERGE_EX1_IN,
        //(fm_size-1),
        em_t
    >(test_in1,in[1]);

    // convert to out valid stream - FIXME values not strictly of any use
    to_stream<
        size_out,
        em_t
    >(test_out,out_valid);

    // run es 
    exit_merge_top(in, out);

    printf("\r\n\t EXIT_MERGE #1\r\n");
    // FIXME only checks that batch grouping of data is preserved, not that data is correct
    err += checkStreamEqual_EM<em_t> (fm_size,out,out_valid,true);
    //err += checkStreamEqual <float> (out,out_valid,true);
    
    return err;
}
