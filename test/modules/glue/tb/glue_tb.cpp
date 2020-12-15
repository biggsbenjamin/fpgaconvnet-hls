#include "common_tb.hpp"
#include "glue_tb.hpp"

int main()
{

    int err = 0;
    
    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

    // in/out streams
    stream_t(acc_t) in[GLUE_COARSE_IN][GLUE_COARSE_OUT];
    stream_t(data_t) out[GLUE_COARSE_OUT];
    stream_t(data_t) out_valid[GLUE_COARSE_OUT];

    // test inputs data
    static acc_t test_in[GLUE_ROWS*GLUE_COLS*CHANNELS_3D(GLUE_FILTERS,GLUE_COARSE_OUT)][GLUE_COARSE_IN][GLUE_COARSE_OUT];
    static data_t test_out[GLUE_ROWS*GLUE_COLS*CHANNELS_3D(GLUE_FILTERS,GLUE_COARSE_OUT)][GLUE_COARSE_OUT];

    // load data_in
    load_data<
        GLUE_ROWS*GLUE_COLS*CHANNELS_3D(GLUE_FILTERS,GLUE_COARSE_OUT),
        GLUE_COARSE_IN,
        GLUE_COARSE_OUT,
        acc_t
    >(input_path,test_in);

    // load data_out
    load_data<
        GLUE_ROWS*GLUE_COLS*CHANNELS_3D(GLUE_FILTERS,GLUE_COARSE_OUT),
        GLUE_COARSE_OUT,
        data_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        GLUE_ROWS*GLUE_COLS*CHANNELS_3D(GLUE_FILTERS,GLUE_COARSE_OUT),
        GLUE_COARSE_IN,
        GLUE_COARSE_OUT,
        acc_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        GLUE_ROWS*GLUE_COLS*CHANNELS_3D(GLUE_FILTERS,GLUE_COARSE_OUT),
        GLUE_COARSE_OUT,
        data_t
    >(test_out,out_valid);

    // run glue
    glue_top(in,out);

    for(int j=0;j<GLUE_COARSE_OUT;j++) {
        printf("GLUE #%d\r\n",j);
        err += checkStreamEqual<data_t>(out[j],out_valid[j]);
    }

    return err;
}
