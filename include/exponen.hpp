#ifndef EXPONEN_HPP_
#define EXPONEN_HPP_

#include "common.hpp"

/**
 * EXPONENTIAL FUNCTION
 */

//typedef ap_fixed<16,8> exp_t;

template<
    unsigned int BATCH_SIZE,
    unsigned int ROWS,
    unsigned int COLS,
    unsigned int CHANNELS,
    typename exp_t
>

void exponen(
    stream_t(exp_t) &in,
    //stream_t(data_t) &out
    hls::stream<float> &out
    //stream_t(bdata_t) &out
)
{

#pragma HLS INLINE OFF //prevents functions being inlined
    
    const unsigned int batch_size   = BATCH_SIZE;
    const unsigned int rows         = ROWS;
    const unsigned int cols         = COLS;
    const unsigned int channels     = CHANNELS;
    //const data_t exp_max_in         = 4.84375;
    //const data_t data_max           = 127.99609375;
    //might not need these constants?
    const exp_t exp_max_in         = 4.84375;
    const exp_t data_max           = 127.99609375;
 
    const unsigned int depth_in= rows*cols*channels+15;
#pragma HLS STREAM variable=in depth=depth_in
#pragma HLS STREAM variable=out //sets up streaming data type

    for(unsigned long pixel_index=0 ; pixel_index < batch_size*rows*cols*channels ; pixel_index++) {
#pragma HLS PIPELINE II=1 rewind 
        /*exp_t tmp, res;
        tmp = exp_t{ in.read() };
        
        res = hls::exp(tmp);
        
        if (res < 0) { //sat value
            res = data_max; //approx of the same sat behaviour as data_t
        }
        out.write( data_t{res} );*/

        //data_t tmp = in.read();
        float tmp = in.read().to_float();
        //std::cout<<"exponen tmp: "<<tmp<<std::endl;
        //bdata_t tmp = bdata_t{in.read()};
        //exponential function options: cordic, lookup table, something else?
        
        //out.write( data_t{ hls::exp(exp_t{tmp}) } );
        //out.write(data_t{ hls::exp(tmp.to_float()) });
        //out.write( hls::exp(tmp.to_float()) );
        float tmp_out = hls::exp(tmp);
        out.write( tmp_out);//hls::exp(tmp) );
        //std::cout<<"exponen out: "<<tmp_out<<std::endl;
    }
}

#endif

