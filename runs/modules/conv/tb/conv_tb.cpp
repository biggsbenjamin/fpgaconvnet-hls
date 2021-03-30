#include "common_tb.hpp"
#include "conv_tb.hpp"

int main() {
    // variable initialisation
    stream_t(data_t) in[CONV_KERNEL_SIZE][CONV_KERNEL_SIZE];
    stream_t(acc_t) out;

    // dimensions
    const int size_in  = CONV_BATCH_SIZE*CONV_ROWS*CONV_COLS*CONV_CHANNELS;
    const int size_out = CONV_BATCH_SIZE*CONV_ROWS*CONV_COLS*CONV_CHANNELS*CONV_FILTERS;

    // gen input
    k1_loop: for(int k1=0;k1<CONV_KERNEL_SIZE;k1++) {
        k2_loop: for(int k2=0;k1<CONV_KERNEL_SIZE;k2++) {
            gen_stream<data_t,size_in>(in[k1][k2]);
        }
    }

    // run component
    conv_top(in,out);

}
