#include "common_tb.hpp"
#include "relu_tb.hpp"

int main() {
    // variable initialisation
    stream_t(data_t) in;
    stream_t(data_t) out;

    // dimensions
    const int size_in  = RELU_BATCH_SIZE*RELU_ROWS*RELU_COLS*RELU_CHANNELS;
    const int size_out = RELU_BATCH_SIZE*RELU_ROWS*RELU_COLS*RELU_CHANNELS;

    // gen input
    gen_stream<data_t,size_in>(in);

    // run component
    relu_top(in,out);

}
