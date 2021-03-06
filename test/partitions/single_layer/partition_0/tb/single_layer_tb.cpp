#include "single_layer_top.hpp"
#include "common_tb.hpp"

int main()
{
    int err = 0;
    std::string data_path   = "data/data.yaml";
    std::string weight_path = "data/weights.yaml";

    static mem_int test_in[SINGLE_LAYER_PORTS_IN][SINGLE_LAYER_SIZE_IN]             = {0};

    // load input
    printf("LOADING INPUT DATA \n");
    load_net_data<
        SINGLE_LAYER_PORTS_IN,
        SINGLE_LAYER_BATCH_SIZE,
        SINGLE_LAYER_ROWS_IN,
        SINGLE_LAYER_COLS_IN,
        SINGLE_LAYER_CHANNELS_IN,
        SINGLE_LAYER_STREAMS_IN
    >(data_path,"in",test_in);

    for( int wr_index=0;wr_index<SINGLE_LAYER_WEIGHTS_RELOADING_FACTOR;wr_index++) {
        
        static mem_int test_out[SINGLE_LAYER_PORTS_OUT][SINGLE_LAYER_SIZE_OUT]          = {0};
        static mem_int test_out_valid[SINGLE_LAYER_PORTS_OUT][SINGLE_LAYER_SIZE_OUT]    = {0};
        
#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
        static mem_int weights[SINGLE_LAYER_PORTS_WR][SINGLE_LAYER_SIZE_WR] = {0};
#endif

        // load weights
        load_net_weights<
            SINGLE_LAYER_PORTS_WR,
            SINGLE_LAYER_SIZE_WR,
            SINGLE_LAYER_WEIGHTS_RELOADING_FACTOR
        >(wr_index,weight_path,"conv1",weights);

        // load valid output
        load_net_data<
            SINGLE_LAYER_PORTS_OUT,
            SINGLE_LAYER_BATCH_SIZE,
            SINGLE_LAYER_ROWS_OUT,
            SINGLE_LAYER_COLS_OUT,
            SINGLE_LAYER_CHANNELS_OUT,
            SINGLE_LAYER_STREAMS_OUT,
            SINGLE_LAYER_WEIGHTS_RELOADING_FACTOR
        >(data_path,"out",test_out_valid,wr_index);

        // run network
        printf("RUNNING NETWORK \n");
#if SINGLE_LAYER_WEIGHTS_RELOADING_FLAG
        if( wr_index > 0 ) {
            fpgaconvnet_ip(1,wr_index,weights,test_in,test_out);
        }
        fpgaconvnet_ip(0,wr_index,weights,test_in,test_out);
#else
        fpgaconvnet_ip(0,wr_index,test_in,test_out);
#endif
        for(int i=0; i<SINGLE_LAYER_PORTS_OUT;i++) {
            printf("PORT %d\n",i);
            check_array_equal<SINGLE_LAYER_SIZE_OUT>(test_out[i],test_out_valid[i]);
        }
           
    }

    printf("%s\n",(err==0) ? "\t--- PASSED ---" : "\t--- FAILED ---");
    return err;
}
