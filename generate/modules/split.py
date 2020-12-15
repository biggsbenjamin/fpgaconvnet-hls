split_template="""
{indent}{name}_split<0>({input},{output});

"""

def gen_split_module(name,param,input,output,indent=0):
    return split_template.format(
        name            =name,
        input_t         =param['input_t'],
        output_t        =param['output_t'],
        input           =input,
        output          =output,
        indent          =" "*indent
    )
