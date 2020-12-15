#include "common_tb.hpp"
#include "batch_norm_tb.hpp"

int main()
{
    int err = 0;

    std::string data_path = std::string(DATA_DIR)+"/data.yaml";
    
    // weights
    data_t scale[BATCH_NORM_CHANNELS];
    data_t shift[BATCH_NORM_CHANNELS];

    stream_t(data_t) in("in");
    stream_t(data_t) out("out");
    stream_t(data_t) out_valid("out_valid");

    // test inputs data
    static data_t test_in[BATCH_NORM_ROWS*BATCH_NORM_COLS*BATCH_NORM_CHANNELS];
    static data_t test_out[BATCH_NORM_ROWS*BATCH_NORM_COLS*BATCH_NORM_CHANNELS];

    // load scale coefficients 
    load_data<
        BATCH_NORM_CHANNELS,
        data_t
    >(data_path,"scale",scale);

    // load shift coefficients 
    load_data<
        BATCH_NORM_CHANNELS,
        data_t
    >(data_path,"shift",shift);

    // load data_in
    load_data<
        BATCH_NORM_ROWS*BATCH_NORM_COLS*BATCH_NORM_CHANNELS,
        data_t
    >(data_path,"data_in",test_in);

    // load data_out
    load_data<
        BATCH_NORM_ROWS*BATCH_NORM_COLS*BATCH_NORM_CHANNELS,
        data_t
    >(data_path,"data_out",test_out);

    // batch_normert input stream
    to_stream<
        BATCH_NORM_ROWS*BATCH_NORM_COLS*BATCH_NORM_CHANNELS,
        data_t
    >(test_in,in);

    // batch_normert to out valid stream
    to_stream<
        BATCH_NORM_ROWS*BATCH_NORM_COLS*BATCH_NORM_CHANNELS,
        data_t
    >(test_out,out_valid);

    // run batch_norm
    batch_norm_top(in,scale,shift,out);

    // check output
    err = checkStreamEqual<data_t>(out,out_valid);

    return err;

}
