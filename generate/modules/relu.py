relu_template="""
{indent}{name}_relu<0>({input},{output});

"""

def gen_relu_module(name,param,input,output,indent=0):
    return relu_template.format(
        name            =name,
        input_t         =param['input_t'],
        output_t        =param['output_t'],
        input           =input,
        output          =output,
        indent          =" "*indent
    )
