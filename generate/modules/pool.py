pool_template="""
{indent}{name}_pool<0>({input},{output});

"""

def gen_pool_module(name,param,input,output,indent=0):
    return pool_template.format(
        name        =name,
        input_t     =param['input_t'],
        output_t    =param['output_t'],
        input       =input,
        output      =output,
        indent      =" "*indent
    )
