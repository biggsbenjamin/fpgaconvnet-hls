#include "common_tb.hpp"
#include "buffer_layer_tb.hpp"

int main()
{
    int err = 0;

    //const int in_size = CHANNELS_3D(BUFFER_LAYER_CHANNELS,BUFFER_LAYER_COARSE)*BUFFER_LAYER_ROWS*BUFFER_LAYER_COLS*BUFFER_LAYER_BATCH_SIZE;
    const int fm_size = BUFFER_LAYER_ROWS*BUFFER_LAYER_COLS*CHANNELS_3D(BUFFER_LAYER_CHANNELS,BUFFER_LAYER_COARSE);

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string ctrl_input_path  = std::string(DATA_DIR)+"/ctrl_in.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(buffer_layer_data_t) in[BUFFER_LAYER_COARSE];
    stream_t(buffer_layer_ctrl_t) ctrl_in;
    stream_t(buffer_layer_data_t) out[BUFFER_LAYER_COARSE];
    stream_t(buffer_layer_data_t) out_correct[BUFFER_LAYER_COARSE];

    // test images
    static buffer_layer_data_t test_in[BUFFER_LAYER_BATCH_SIZE*fm_size][BUFFER_LAYER_COARSE];
    static buffer_layer_ctrl_t test_ctrl_in[BUFFER_LAYER_BATCH_SIZE]; //[BUFFER_LAYER_COARSE];
    std::vector<buffer_layer_data_t> test_out[BUFFER_LAYER_COARSE]; 
    std::vector<batch_t> batchnum_vec; 

    std::cout<<"fm_size: "<< fm_size << std::endl;
    std::cout<<"in_size: "<< fm_size*BUFFER_LAYER_BATCH_SIZE << std::endl;
    std::cout<<"full fm size: "<< fm_size*BUFFER_LAYER_COARSE << std::endl;

    // load input
    load_data_l<
        BUFFER_LAYER_BATCH_SIZE,
        fm_size,
        BUFFER_LAYER_COARSE,
        buffer_layer_data_t
    >(input_path,test_in);
    
    // NOTE coarse ctrl signal done internally
    load_data_buff<
        BUFFER_LAYER_BATCH_SIZE,
        1,
        buffer_layer_ctrl_t
    >(ctrl_input_path,test_ctrl_in);

    // load output - unknown output size
    load_data_buffout<
        fm_size*2,
        BUFFER_LAYER_COARSE,
        buffer_layer_data_t
    >(output_path,test_out);
    
    //ctrl signals are duplicated so only need to check one value
    //int ctrl_sum=0;
    for (unsigned int i=0; i<BUFFER_LAYER_BATCH_SIZE;i++) {
        //ctrl_sum += test_ctrl_in[i][0].to_int();
        //ctrl_sum += test_ctrl_in[i].to_int();
        if (test_ctrl_in[i].data) {
            if (!BUFFER_LAYER_DROP_MODE) {
                batchnum_vec.push_back(i);
            } 
        } else {
            if (BUFFER_LAYER_DROP_MODE) {
                batchnum_vec.push_back(i);
            } 
        }

    }
    // FIXME some other way:
    // adding in this flush sample
    batch_t flushid = 420;
    batchnum_vec.push_back(flushid);
    batchnum_vec.push_back(flushid);

    //int out_size;
    //if (!BUFFER_LAYER_DROP_MODE) {
    //    out_size = ctrl_sum*fm_size;
    //} else {
    //    out_size = (BUFFER_LAYER_BATCH_SIZE - ctrl_sum)*fm_size;
    //}
    printf("batchIDs:");
    for (int i=0; i<batchnum_vec.size();i++){
        uint16_t tmp =batchnum_vec[i]; 
        printf("%d,",tmp);
    }
    int out_size = batchnum_vec.size()*fm_size;
    printf("\nout_size:%d \n",out_size);
    
    // convert to streams
    to_stream_l<
        BUFFER_LAYER_BATCH_SIZE,
        fm_size,
        BUFFER_LAYER_COARSE,
        buffer_layer_data_t
    >(test_in,in);
    
    // NOTE from buffer module
    to_stream<
        BUFFER_LAYER_BATCH_SIZE,
        buffer_layer_ctrl_t
    >(test_ctrl_in,ctrl_in);
    
    //variable stream
    std::cout<< "test out[0] size: " <<test_out[0].size() <<std::endl;
    std::cout<< "test out[1] size: " <<test_out[1].size() <<std::endl;
    std::cout<< "out size: " <<out_size <<std::endl;

    //assert(batchnum_vec.size()*test_out[0].size()*BUFFER_LAYER_COARSE == out_size);

    to_stream<
        BUFFER_LAYER_COARSE,
        buffer_layer_data_t,
        batch_t
    >(test_out,batchnum_vec,out_correct, fm_size); //test_out[0].size());
    
    buffer_layer_top(in, ctrl_in, out, 0);
    
    for(int i=0;i<BUFFER_LAYER_COARSE;i++){
        printf("TESTING OUTPUT %d: ",i);
        err += checkStreamEqual<buffer_layer_data_t>(out[i],out_correct[i],false);
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
