# import modules
import os
import shutil 

concat_layer_template_header = """#ifndef {NAME}_HPP_
#define {NAME}_HPP_

#include "common.hpp"

#define name        {name}
#define {NAME}_ID   {id}

#define {name}_input_t          data_t
#define {name}_output_t         data_t

// define empty channels
#define {NAME}_CHANNELS_0 0
#define {NAME}_CHANNELS_1 0
#define {NAME}_CHANNELS_2 0
#define {NAME}_CHANNELS_3 0
#define {NAME}_CHANNELS_4 0
#define {NAME}_CHANNELS_5 0

#define MODULE_NAME {NAME}
#define {NAME}_BATCH_SIZE   {batch_size}
#define {NAME}_ROWS         {rows}
#define {NAME}_COLS         {cols}
{channels}
#define {NAME}_COARSE       {coarse}
#define {NAME}_CHANNELS_OUT {channels_out}
#include "{name}_concat.hpp"
#undef MODULE_NAME

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

#undef name
#endif
"""

concat_layer_template_src = """#include "{name}.hpp"

void {name}(
{fn_inputs}
    stream_t({name}_output_t) out[{NAME}_COARSE],
    int mode
)
{{

#pragma HLS INLINE OFF
#pragma HLS DATAFLOW

#pragma HLS STREAM variable=out
#pragma HLS ARRAY_PARTITION variable=out complete dim=0

{port_info}

    {name}_concat<{name}_input_t,{name}_output_t>({inputs},out);



}}

"""

def gen_concat_layer(name,param,src_path,header_path):

    # function inputs
    fn_inputs = ""
    for i in range(param['inputs']):
        fn_inputs += "    stream_t({name}_input_t)  in_{i}[{NAME}_COARSE],\n".format(
            name=name,
            NAME=name.upper(),
            i   =i    
        )

    # src
    concat_layer_src = concat_layer_template_src.format(
        name            =name,
        NAME            =name.upper(),
        fn_inputs       =fn_inputs,
        port_info       ="\n\n".join(["DO_PRAGMA(HLS STREAM variable=in_{i} depth={NAME}_CHANNELS_OUT)\n#pragma HLS ARRAY_PARTITION variable=in_{i} complete dim=0".format(i=i,NAME=name.upper()) for i in range(param['inputs'])]),
        inputs          =",".join(["in_{i}".format(i=i) for i in range(param['inputs'])])
    )

    # header
    concat_layer_header = concat_layer_template_header.format(
        name            =name,
        NAME            =name.upper(),
        fn_inputs       =fn_inputs,
        id              =0, # param['id'],
        batch_size      =param['batch_size'],
        rows            =param['rows'],
        cols            =param['cols'],
        channels        ="\n".join(["#undef {NAME}_CHANNELS_{i}\n#define {NAME}_CHANNELS_{i}    {c}".format(NAME=name.upper(),i=i,c=int(param['channels'][i]/param['coarse'])) for i in range(param['inputs']) ]),
        inputs          =param['inputs'],
        coarse          =param['coarse'],
        rows_out        =param['rows_out'],
        cols_out        =param['cols_out'],
        channels_out    =int(param['channels_out']/param['coarse'])
    )   

    # write source file
    with open(src_path,'w') as src_file:
        src_file.write(concat_layer_src)

    # write header file
    with open(header_path,'w') as header_file:
        header_file.write(concat_layer_header)

    # save modules 
    header_path = os.path.dirname(os.path.abspath(header_path))
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/concat.hpp') , os.path.join(header_path,"{name}_concat.hpp".format(name=name)) )

    return
