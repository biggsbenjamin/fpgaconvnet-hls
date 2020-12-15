squeeze_template="""
{indent}{name}_squeeze<0>({input},{output});

"""

def gen_squeeze_module(name,param,input,output,indent=0):
    return squeeze_template.format(
        name            =name,
        input_t         =param['input_t'],
        output_t        =param['output_t'],
        input           =input,
        output          =output,
        indent          =" "*indent
        #single_stream   ="" if not single_stream else "//"
    )
