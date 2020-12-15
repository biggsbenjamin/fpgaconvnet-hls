conv_template = """
{indent}{name}_conv{pointwise}<0>({input},{weights},{output});

"""

def gen_conv_module(name,param,input,weights,output,indent=0,single_stream=False):
    return conv_template.format(
        name            =name,
        input_t         =param['input_t'],
        weight_t        =param['weight_t'],
        output_t        =param['output_t'],
        input           =input,
        weights         =weights,
        output          =output,
        indent          =" "*indent,
        #single_stream   ="" if not single_stream else "//",
        pointwise       ="_pw" if single_stream else ""
    )
