concat_template="""
{indent}concat<0>({input},{output});

"""

def gen_concat_module(name,param,input,output,indent=0):
    return concat_template.format(
        #name            =name,
        batch_size      =param['batch_size'],
        rows            =param['rows'],
        cols            =param['cols'],
        channels        =param['channels'],
        inputs          =param['inputs'],
        id              =param['id'],
        input_t         =param['input_t'],
        output_t        =param['output_t'],
        input           =input,
        output          =output,
        indent          =" "*indent
    )
