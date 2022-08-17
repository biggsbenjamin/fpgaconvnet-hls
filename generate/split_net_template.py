network_header_template = """//auto generated file
#ifndef {NAME}_TOP_HPP_
#define {NAME}_TOP_HPP_

#include "common.hpp"
//FIXME only reason layers are included is because of in/out types, CHANGE THIS
{include}
#include "mem_read.hpp"
#include "mem_write.hpp"
//#include "wr.hpp"

#define {NAME}_BATCH_SIZE   {batch_size}

#define {NAME}_ROWS_IN      {rows_in}
#define {NAME}_COLS_IN      {cols_in}
#define {NAME}_CHANNELS_IN  {channels_in}

#define {NAME}_ROWS_OUT     {rows_out}
#define {NAME}_COLS_OUT     {cols_out}
#define {NAME}_CHANNELS_OUT {channels_out}

#define {NAME}_STREAMS_IN   {streams_in}
#define {NAME}_STREAMS_OUT  {streams_out}
//#define {NAME}_STREAMS_WR   1

#define {NAME}_PORTS        {ports}
#define {NAME}_PORTS_IN     {ports}  //{NAME}_PORTS
#define {NAME}_PORTS_OUT    {ports}  //{NAME}_PORTS
//#define {NAME}_PORTS_WR     1 //{NAME}_PORTS

//#define {NAME}_WEIGHTS_RELOADING_FACTOR {wr_factor}
//#define {NAME}_WEIGHTS_RELOADING_LAYER  {wr_layer}
//#define {NAME}_WEIGHTS_RELOADING_FLAG   {wr_flag}

#define {NAME}_SIZE_IN  {NAME}_BATCH_SIZE*{NAME}_ROWS_IN*{NAME}_COLS_IN*DIVIDE({NAME}_CHANNELS_IN,{NAME}_STREAMS_IN)
#define {NAME}_SIZE_OUT {NAME}_BATCH_SIZE*{NAME}_ROWS_OUT*{NAME}_COLS_OUT*DIVIDE({NAME}_CHANNELS_OUT,{NAME}_STREAMS_OUT) //*{NAME}_WEIGHTS_RELOADING_FACTOR

typedef {input_layer}_input_t   {name}_input_t;
typedef {output_layer}_output_t {name}_output_t;

//no weights reloading
//no process but adding in dma translation

//FIXME replace in/out streams with 16 + 8bit struct for bs
void {name}_top(
    axis_hls_t &dma_in,
    axis_hls_t &dma_out,
    {in_net_stream},
    {out_net_stream}
);

#endif
"""

network_src_template = """//auto generated file
#include "{name}_top.hpp"

//NOTE weights and biases contained within layers
//no wr process

void axis_convert_in(
    axis_hls_t &dma_in,
    {in_net_stream}
)
{{

#pragma HLS DATAFLOW
//in out array partitioning not required

    const unsigned size_in  = {NAME}_SIZE_IN ;
    const unsigned size_out = {NAME}_SIZE_OUT;

    axis_t cache;
    //FIXME auto match the input data stream or just use axiu
    {in_type} data;

    for (unsigned int in_idx=0;in_idx<size_in;in_idx++) {{
#pragma HLS PIPELINE II=1
        //read in axis packet
        cache = dma_in.read();

        // cast data to input type
        data = cache.data;

        //write out simple hls stream of ap_fixed type
        in[0].write(data); //16 bits maps to 16 bits
    }}

}}

void axis_convert_out(
    {out_net_stream},
    axis_hls_t &dma_out
)
{{

#pragma HLS DATAFLOW
//TODO add inline off pragma

//in out array partitioning not required

    const unsigned size_in  = {NAME}_SIZE_IN ;
    const unsigned size_out = {NAME}_SIZE_OUT;

    //FIXME auto match the output data stream or just use axiu
    {out_type} cache;
    axis_t out_pkt;

    //NOTE looping for one extra packet because of tlast staying high
    for (unsigned int out_idx=0;out_idx<size_out;out_idx++) {{
#pragma HLS PIPELINE II=1
        //read in stream data from last fcn layer
        cache = out[0].read();

        //construct packets
        //if (out_idx == 0){{
        //    out_pkt.user = 1;
        //}} else {{
        //    out_pkt.user = 0;
        //}}

        //out_pkt.id = 0;
        //out_pkt.dest = 0;
        //out_pkt.strb = 0;
        //out_pkt.keep = 0;

        //writing data to out packet
        out_pkt.data = cache;

        if (out_idx+1 == size_out) {{
            //null packet to make tlast go low
            out_pkt.last = 1;
        }} else {{
            out_pkt.last = 0;
        }}

        dma_out.write(out_pkt);

    }}

}}

void {name}_top(
    axis_hls_t &dma_in,
    axis_hls_t &dma_out,
    {in_net_stream},
    {out_net_stream}
)
{{
#pragma HLS INTERFACE s_axilite port=return bundle=ctrl

    const unsigned size_in  = {NAME}_SIZE_IN ;
    const unsigned size_out = {NAME}_SIZE_OUT;

//denotes axi streams to DMA controller (requires tlast)
#pragma HLS INTERFACE axis port=dma_in
#pragma HLS INTERFACE axis port=dma_out
//denotes internal stream interfaces
#pragma HLS INTERFACE axis port=in
#pragma HLS INTERFACE axis port=out

    axis_convert_in(dma_in, in);
    axis_convert_out(out, dma_out);

}}
"""
