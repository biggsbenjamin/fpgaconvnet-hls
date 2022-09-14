#include "common_tb.hpp"
#include "split_layer_tb.hpp"

int main()
{
    int err = 0;

    //const int fm_size = CHANNELS_3D(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS;
    const int fm_size = SPLIT_LAYER_CHANNELS_PM*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS;
    //const int out_size = CHANNELS_3D(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS*SPLIT_LAYER_PORTS_OUT;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(split_layer_data_t) in[SPLIT_LAYER_COARSE];
    stream_t(split_layer_data_t) out[SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE];
    stream_t(split_layer_data_t) out_correct[SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE];

    // test images
    static split_layer_data_t test_in[SPLIT_LAYER_BATCH_SIZE*fm_size][SPLIT_LAYER_COARSE];
    static split_layer_data_t test_out[SPLIT_LAYER_BATCH_SIZE*fm_size][SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE];

    // load input
    load_data_l<
        SPLIT_LAYER_BATCH_SIZE,
        fm_size,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(input_path,test_in);

    // load output - using GLUE template
    //load_data<
    //    fm_size,
    //    SPLIT_LAYER_PORTS_OUT,
    //    SPLIT_LAYER_COARSE
    //>(output_path,test_out);

    //TODO change optimiser functional model or add to common_tb
    //const char *filepath_cstr = output_path.c_str();
    //FILE * fp = fopen(filepath_cstr,"r");
    //if (fp == NULL) { perror("Failed: "); }
    //for (int b_idx=0; b_idx<SPLIT_LAYER_BATCH_SIZE; b_idx++) {
    for(int i=0;i<fm_size*SPLIT_LAYER_BATCH_SIZE;i++) {
        for(int crs_idx=0;crs_idx<SPLIT_LAYER_COARSE;crs_idx++) {
            for(int prt_idx=0;prt_idx<SPLIT_LAYER_PORTS_OUT;prt_idx++) {
                //float val;
                //fscanf(fp,"%f\n", &val);
                
                test_out[i][prt_idx][crs_idx] = test_in[i][crs_idx];
            }
        }
    }
    //}
    //fclose(fp);

    // convert to streams
    to_stream_l<
        SPLIT_LAYER_BATCH_SIZE,
        fm_size,
        SPLIT_LAYER_COARSE,
        split_layer_data_t
    >(test_in,in);

    to_stream_l<
        SPLIT_LAYER_BATCH_SIZE,
        fm_size,
        SPLIT_LAYER_COARSE,
        SPLIT_LAYER_PORTS_OUT,
        split_layer_data_t
    >(test_out,out_correct);

    split_layer_top(in,out,0);

    for(int i=0;i<SPLIT_LAYER_PORTS_OUT;i++){
        for(int j=0;j<SPLIT_LAYER_COARSE;j++) {
            printf("TESTING OUTPUT [%d] [%d] : \n", i,j);
            err += checkStreamEqual<split_layer_data_t>(out[i][j],out_correct[i][j],false);
            printf("%s\n",(err==0) ? "passed" : "failed");
        }
    }

    return err;
}
