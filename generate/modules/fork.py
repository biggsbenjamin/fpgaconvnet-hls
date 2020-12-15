fork_template="""
{indent}{name}_fork<0>({input},{output});

"""

def gen_fork_module(name,param,input,output,indent=0,single_stream=False):
    return fork_template.format(
        name            =name,
        input_t         =param['input_t'],
        output_t        =param['output_t'],
        input           =input,
        output          =output,
        indent          =" "*indent
        #single_stream   ="" if not single_stream else "//"
    )
