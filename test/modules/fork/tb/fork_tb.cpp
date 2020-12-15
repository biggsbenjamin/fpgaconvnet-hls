#include "common_tb.hpp"
#include "fork_tb.hpp"

int main()
{

    int err = 0;

    // file paths
    std::string input_path  = std::string(DATA_DIR)+"/input.dat";
    std::string output_path = std::string(DATA_DIR)+"/output.dat";

#if FORK_KERNEL_SIZE != 1
    // in/out streams
    stream_t(data_t) in[FORK_KERNEL_SIZE][FORK_KERNEL_SIZE];
    stream_t(data_t) out[FORK_COARSE][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE];
    stream_t(data_t) out_valid[FORK_COARSE][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE];

    // test inputs data
    static data_t test_in[FORK_ROWS*FORK_COLS*FORK_CHANNELS][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE];
    static data_t test_out[FORK_ROWS*FORK_COLS*FORK_CHANNELS][FORK_COARSE][FORK_KERNEL_SIZE][FORK_KERNEL_SIZE];
#else
    // in/out streams
    stream_t(data_t) in;
    stream_t(data_t) out[FORK_COARSE];
    stream_t(data_t) out_valid[FORK_COARSE];

    // test inputs data
    data_t test_in[FORK_ROWS*FORK_COLS*FORK_CHANNELS];
    data_t test_out[FORK_ROWS*FORK_COLS*FORK_CHANNELS][FORK_COARSE];
#endif

    // load data_in
    load_data<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
#if FORK_KERNEL_SIZE != 1
        FORK_KERNEL_SIZE,
#endif
        data_t
    >(input_path,test_in);

    // load data_out
    load_data<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
        FORK_COARSE,
#if FORK_KERNEL_SIZE != 1
        FORK_KERNEL_SIZE,
#endif
        data_t
    >(output_path,test_out);

    // convert input stream
    to_stream<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
#if FORK_KERNEL_SIZE != 1
        FORK_KERNEL_SIZE,
#endif
        data_t
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        FORK_ROWS*FORK_COLS*FORK_CHANNELS,
        FORK_COARSE,
#if FORK_KERNEL_SIZE != 1
        FORK_KERNEL_SIZE,
#endif
        data_t
    >(test_out,out_valid);

    // run fork
    fork_top(in,out);

    printf("\r\n\t FORK #1\r\n");

#if FORK_KERNEL_SIZE == 1
    for(int j=0;j<FORK_COARSE;j++) {
	    err += checkStreamEqual<data_t>(out[j],out_valid[j]);
    }
#else
    for(int j=0;j<FORK_COARSE;j++) {
        for(int k1=0;k1<FORK_KERNEL_SIZE;k1++) {
	        for(int k2=0;k2<FORK_KERNEL_SIZE;k2++) {
	            err += checkStreamEqual<data_t>(out[j][k1][k2],out_valid[j][k1][k2]);
	        }
	    }
    }
#endif

    return err;
}
