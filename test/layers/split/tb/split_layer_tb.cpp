#include "common_tb.hpp"
#include "split_layer_tb.hpp"

int main()
{
    int err = 0;

    const int in_size = CHANNELS_3D(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS;
    const int out_size = CHANNELS_3D(SPLIT_LAYER_CHANNELS,SPLIT_LAYER_COARSE)*SPLIT_LAYER_ROWS*SPLIT_LAYER_COLS*SPLIT_LAYER_PORTS_OUT;

    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    stream_t(data_t) in[SPLIT_LAYER_COARSE];
    stream_t(data_t) out[SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE];
    stream_t(data_t) out_correct[SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE];

    // test images
    static data_t test_in[in_size][SPLIT_LAYER_COARSE];
    static data_t test_out[in_size][SPLIT_LAYER_PORTS_OUT][SPLIT_LAYER_COARSE];

    // load input
    load_data<
        in_size,
        SPLIT_LAYER_COARSE
    >(input_path,test_in);

    // load output - using GLUE template
    //load_data<
    //    in_size,
    //    SPLIT_LAYER_PORTS_OUT,
    //    SPLIT_LAYER_COARSE
    //>(output_path,test_out);

    //TODO change optimiser functional model or add to common_tb
    const char *filepath_cstr = output_path.c_str();
    FILE * fp = fopen(filepath_cstr,"r");
    if (fp == NULL) { perror("Failed: "); }
    for(int i=0;i<in_size;i++) {
        for(int a=0;a<SPLIT_LAYER_COARSE;a++) {
            for(int b=0;b<SPLIT_LAYER_PORTS_OUT;b++) {
                float val;
                fscanf(fp,"%f\n", &val);
                test_out[i][b][a] = data_t(val);
	        }
        }
    }
    fclose(fp);

    // convert to streams
    to_stream<
        in_size,
        SPLIT_LAYER_COARSE
    >(test_in,in);

    to_stream<
        in_size,
        SPLIT_LAYER_PORTS_OUT,
        SPLIT_LAYER_COARSE
    >(test_out,out_correct);

    split_layer_top(in,out,0);

    for(int i=0;i<SPLIT_LAYER_PORTS_OUT;i++){
        for(int j=0;j<SPLIT_LAYER_COARSE;j++) {
            printf("TESTING OUTPUT %d: ",i, "-", j);
            err += checkStreamEqual<data_t>(out[i][j],out_correct[i][j],false);
            printf("%s\n",(err==0) ? "passed" : "failed");
        }
    }

    return err;
}
