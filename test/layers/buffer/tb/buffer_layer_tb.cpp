#include "common_tb.hpp"
#include "buffer_layer_tb.hpp"

int main()
{
    int err = 0;

    const int in_size = CHANNELS_3D(BUFFER_LAYER_CHANNELS,BUFFER_LAYER_COARSE)*BUFFER_LAYER_ROWS*BUFFER_LAYER_COLS*BUFFER_LAYER_BATCH_SIZE;
    const int shape_size = BUFFER_LAYER_ROWS*BUFFER_LAYER_COLS*BUFFER_LAYER_CHANNELS;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string ctrl_input_path  = std::string(DATA_DIR)+"/ctrl_in.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(data_t) in[BUFFER_LAYER_COARSE];
    stream_t(data_t) ctrl_in;
    stream_t(data_t) out[BUFFER_LAYER_COARSE];
    stream_t(data_t) out_correct[BUFFER_LAYER_COARSE];

    // test images
    static data_t test_in[in_size][BUFFER_LAYER_COARSE];
    static data_t test_ctrl_in[BUFFER_LAYER_BATCH_SIZE]; //[BUFFER_LAYER_COARSE];
    std::vector<data_t> test_out[BUFFER_LAYER_COARSE]; 

    // load input
    load_data<
        in_size,
        BUFFER_LAYER_COARSE
    >(input_path,test_in);
    
    load_data<
        BUFFER_LAYER_BATCH_SIZE//,
        //BUFFER_LAYER_COARSE
    >(ctrl_input_path,test_ctrl_in);

    // load output - unknown output size
    load_data<
        BUFFER_LAYER_COARSE,
        data_t
    >(output_path,test_out);
    
    //ctrl signals are duplicated so only need to check one value
    int ctrl_sum=0;
    for (unsigned int i=0; i<BUFFER_LAYER_BATCH_SIZE;i++) {
        //ctrl_sum += test_ctrl_in[i][0].to_int();
        ctrl_sum += test_ctrl_in[i].to_int();
    }

    int out_size;
    if (!BUFFER_LAYER_DROP_MODE) {
        out_size = ctrl_sum*shape_size;
    } else {
        out_size = (BUFFER_LAYER_BATCH_SIZE - ctrl_sum)*shape_size;
    }
    
    // convert to streams
    to_stream<
        in_size,
        BUFFER_LAYER_COARSE
    >(test_in,in);
    
    to_stream<
        BUFFER_LAYER_BATCH_SIZE//,
        //BUFFER_LAYER_COARSE
    >(test_ctrl_in, ctrl_in);
    
    //variable stream
    assert(test_out[0].size()*BUFFER_LAYER_COARSE == out_size);
    to_stream<
        BUFFER_LAYER_COARSE
    >(test_out,out_correct, test_out[0].size());
    
    buffer_layer_top(in, ctrl_in, out);
    
    for(int i=0;i<BUFFER_LAYER_COARSE;i++){
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<data_t>(out[i],out_correct[i],false);
        printf("%s\n",(err==0) ? "passed" : "failed");
    }
    
        std::cout<< "in: " <<in[0].empty() <<std::endl;
//        assert(in[0].empty() );
        std::cout<< "in: " <<in[1].empty() <<std::endl;
//        assert(in[1].empty() );
//#ifdef SW
/*    for (int i;i<BUFFER_LAYER_COARSE;i++){
        std::cout<< "in: " <<in[i].empty() <<std::endl;
        assert(in[i].empty() );
        std::cout<< "ci: " <<ctrl_in[i].empty() <<std::endl;
        assert(ctrl_in[i].empty());
        std::cout<< "ot: " <<out[i].empty() <<std::endl;
        assert(out[i].empty());
        std::cout<< "oc: " <<out_correct[i].empty() <<std::endl;
        assert(out_correct[i].empty() );
    }
*/
//#endif
    return err;
}
