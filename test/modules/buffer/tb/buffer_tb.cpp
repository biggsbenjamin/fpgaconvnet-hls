#include "common_tb.hpp"
#include "buffer_tb.hpp"

int main()
{

    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string ctrl_input_path  = std::string(DATA_DIR)+"/ctrl_in.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(data_t) in;
    stream_t(data_t) ctrl_in;
    stream_t(data_t) out;
    stream_t(data_t) out_valid;

    const unsigned int in_size = BUFFER_BATCH_SIZE*BUFFER_ROWS*BUFFER_COLS*BUFFER_CHANNELS;
    const unsigned int shape_size = BUFFER_ROWS*BUFFER_COLS*BUFFER_CHANNELS;
    
    // test inputs data
    static data_t test_in[in_size];
    static data_t test_ctrl_in[BUFFER_BATCH_SIZE];
    std::vector<data_t> test_out; 

    // load data_in
    load_data<
        in_size,
        data_t
    >(input_path,test_in);
    
    load_data<
        BUFFER_BATCH_SIZE,
        data_t
    >(ctrl_input_path,test_ctrl_in);

    // load data_out
    load_data<
    data_t
    >(output_path,test_out);
    
    int ctrl_sum=0;
    for (unsigned int i=0; i<BUFFER_BATCH_SIZE;i++) {
        ctrl_sum += test_ctrl_in[i].to_int();
    }

    int out_size;
    if (!BUFFER_DROP_MODE) {
        out_size = ctrl_sum*shape_size;
    } else {
        out_size = (BUFFER_BATCH_SIZE - ctrl_sum)*shape_size;
    }

    // convert input stream
    to_stream<
        in_size,
        data_t
    >(test_in,in);
    
    to_stream<
        BUFFER_BATCH_SIZE,
        data_t
    >(test_ctrl_in,ctrl_in);

    // convert to out valid stream
    to_stream<
        data_t
    >(test_out,out_valid, out_size);

    // run buffer
    buffer_top(in, ctrl_in, out);

    printf("\r\n\t BUFFER #1\r\n");
    checkStreamEqual<data_t>(out,out_valid);

    return err;
}
