eltwise_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

//#include "eltwise.hpp"
#include "common.hpp"

#define {NAME}_ID {id}

#define {name}_input_t          data_t
#define {name}_output_t         data_t

#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
#define {NAME}_CHANNELS     {channels}
#define {NAME}_INPUTS       {inputs}
#define {NAME}_COARSE       {coarse}

#define {NAME}_COARSE_IN    {coarse}
#define {NAME}_COARSE_OUT   {coarse}

#define {NAME}_ROWS_OUT     {rows_out} 
#define {NAME}_COLS_OUT     {cols_out} 
#define {NAME}_CHANNELS_OUT {channels_out} 

/**
 * FUNCTION DEFINITION
 */

void {name}(
{fn_inputs}
    stream_t({name}_output_t) out[{NAME}_COARSE],
    int mode
);

#endif
"""

eltwise_layer_template_src = """#include "{name}.hpp"

void {name}(
{fn_inputs}
    stream_t({name}_output_t) out[{NAME}_COARSE],
    int mode
)
{{

#pragma HLS INLINE OFF

#pragma HLS STREAM variable=out
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

{port_info}

    dim_loop: for(int pixelIndex=0;pixelIndex<{NAME}_BATCH_SIZE*{NAME}_ROWS*{NAME}_COLS;pixelIndex++)
    {{
#pragma HLS dependence variable out intra WAW true
#pragma HLS dependence variable out inter false
//#pragma HLS dependence variable in inter false
#pragma HLS PIPELINE
#pragma HLS DATAFLOW

        channel_loop: for(int channel_index=0;channel_index<CHANNELS_3D({NAME}_CHANNELS,{NAME}_COARSE);channel_index++)
        {{
            #pragma HLS PIPELINE II=1
            {name}_output_t tmp[{NAME}_COARSE] = {{0}};
            coarse_loop: for(int c=0;c<{NAME}_COARSE;c++)
            {{

{inputs}

                out[c].write(tmp[c]);
            }}
        }}

    }}

}}

"""

def gen_eltwise_layer(name,param,src_path,header_path):

    # function inputs
    fn_inputs = ""
    for i in range(param['inputs']):
        fn_inputs += "    stream_t({name}_input_t)  in_{i}[{NAME}_COARSE],\n".format(
            name=name,
            NAME=name.upper(),
            i   =i    
        )

    # loop content
    inputs = ""
    for i in range(param['inputs']):
        inputs += """        #pragma HLS dependence variable in_{i} inter false
                tmp[c] += in_{i}[c].read();
""".format(
                name=name,
                NAME=name.upper(),
                i   =i    
            )

    # src
    eltwise_layer_src = eltwise_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        fn_inputs       =fn_inputs,
        port_info       ="\n\n".join(["#pragma HLS INTERFACE axis port=in_{i}\n#pragma HLS STREAM variable=in_{i}\n#pragma HLS ARRAY_PARTITION variable=in_{i} complete dim=0".format(i=i) for i in range(param['inputs'])]),
        inputs          =inputs
    )

    # header
    eltwise_layer_header = eltwise_layer_template_header.format(
        name            =name,
        NAME            =name.upper(),
        fn_inputs       =fn_inputs,
        id              =0, # param['id'],
        batch_size      =param['batch_size'],
        rows            =param['rows'],
        cols            =param['cols'],
        channels        =param['channels'],
        inputs          =param['inputs'],
        coarse          =param['coarse'],
        rows_out        =param['rows_out'],
        cols_out        =param['cols_out'],
        channels_out    =param['channels_out']
    )   

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(eltwise_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(eltwise_layer_header)

    return
