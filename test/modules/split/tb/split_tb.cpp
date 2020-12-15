#include "common_tb.hpp"
#include "split_tb.hpp"

int main()
{

    int err = 0;
    std::string data_path = std::string(DATA_DIR)+"/data.yaml";

    // in/out streams
    stream_t(data_t) in[SPLIT_COARSE];
    stream_t(data_t) out[SPLIT_OUTPUTS][SPLIT_COARSE];
    stream_t(data_t) out_valid[SPLIT_OUTPUTS][SPLIT_COARSE];

    // test inputs data
    static data_t test_in[SPLIT_ROWS*SPLIT_COLS*CHANNELS_3D(SPLIT_CHANNELS,SPLIT_COARSE)][SPLIT_COARSE];
    static data_t test_out[SPLIT_ROWS*SPLIT_COLS*CHANNELS_3D(SPLIT_CHANNELS,SPLIT_COARSE)][SPLIT_OUTPUTS][SPLIT_COARSE];

    // load data_in
    load_data<
        SPLIT_ROWS*SPLIT_COLS*CHANNELS_3D(SPLIT_CHANNELS,SPLIT_COARSE),
        SPLIT_COARSE
    >(data_path,"data_in",test_in);

    // load data_out
    load_data<
        SPLIT_ROWS*SPLIT_COLS*CHANNELS_3D(SPLIT_CHANNELS,SPLIT_COARSE),
        SPLIT_OUTPUTS,
        SPLIT_COARSE
    >(data_path,"data_out",test_out);

    // convert input stream
    to_stream<
        SPLIT_ROWS*SPLIT_COLS*CHANNELS_3D(SPLIT_CHANNELS,SPLIT_COARSE),
        SPLIT_COARSE
    >(test_in,in);

    // convert to out valid stream
    to_stream<
        SPLIT_ROWS*SPLIT_COLS*CHANNELS_3D(SPLIT_CHANNELS,SPLIT_COARSE),
        SPLIT_OUTPUTS,
        SPLIT_COARSE
    >(test_out,out_valid);

    // run split
    split_top(in,out);

    printf("\r\n\t SPLIT #1\r\n");
    for(int i=0;i<SPLIT_OUTPUTS;i++)
    {
        for(int j=0;j<SPLIT_COARSE;j++)
	{
            err += checkStreamEqual<data_t>(out[i][j],out_valid[i][j],false);
        }
    }

    return err;
}
