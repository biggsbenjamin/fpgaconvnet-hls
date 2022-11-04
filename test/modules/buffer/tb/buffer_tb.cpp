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
    stream_t(buffer_t) in;
    stream_t(ctrl_t) ctrl_in;
    stream_t(buffer_t) out;
    stream_t(buffer_t) out_valid;

    const unsigned int in_size = BUFFER_BATCH_SIZE*BUFFER_ROWS*BUFFER_COLS*BUFFER_CHANNELS;
    const unsigned int fm_size = BUFFER_ROWS*BUFFER_COLS*BUFFER_CHANNELS;
    
    // test inputs data
    static buffer_t test_in[in_size];
    static ctrl_t test_ctrl_in[BUFFER_BATCH_SIZE];
    std::vector<buffer_t> test_out; 
    std::vector<batch_t> batchnum_vec; 

    // load data_in
    load_data_buff<
        BUFFER_BATCH_SIZE,
        fm_size,
        buffer_t
    >(input_path,test_in);
    
    load_data_buff<
        BUFFER_BATCH_SIZE,
        1,
        ctrl_t
    >(ctrl_input_path,test_ctrl_in);

    // load data_out
    load_data_buffout<
        fm_size*2,
        buffer_t
    >(output_path,test_out);

    printf("fm_size:%d \n",fm_size);
    printf("in_size:%d \n",in_size);
    printf("test_out len:%d \n",test_out.size());
    
    //int ctrl_sum=0;
    for (batch_t i=0; i<BUFFER_BATCH_SIZE;i++) {
        if (test_ctrl_in[i].data) {
            if (!BUFFER_DROP_MODE) {
                batchnum_vec.push_back(i);
            } 
        } else {
            if (BUFFER_DROP_MODE) {
                batchnum_vec.push_back(i);
            } 
        }
    }
    // FIXME some other way:
    // adding in this flush sample
    batch_t flushid = 420;
    batchnum_vec.push_back(flushid);
    batchnum_vec.push_back(flushid);


    printf("batchIDs:");
    for (int i=0; i<batchnum_vec.size();i++){
        uint16_t tmp =batchnum_vec[i]; 
        printf("%d,",tmp);
    }
    int out_size = test_out.size(); //batchnum_vec.size()*fm_size;
    printf("\nout_size:%d \n",out_size);

    // convert input stream
    to_stream<
        in_size,
        buffer_t
    >(test_in,in);
    
    to_stream<
        BUFFER_BATCH_SIZE,
        ctrl_t
    >(test_ctrl_in,ctrl_in);

    // convert to out valid stream
    to_stream<
        buffer_t,
        batch_t
    >(test_out, batchnum_vec, out_valid, fm_size);

    // run buffer
    buffer_top(in, ctrl_in, out);

    printf("\r\n\t BUFFER #1\r\n");
    err += checkStreamEqual<buffer_t>(out,out_valid);

    return err;
}
