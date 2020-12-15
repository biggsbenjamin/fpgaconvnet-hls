glue_template="""
{indent}{name}_glue<0>({input},{output});

"""

def gen_glue_module(name,param,input,output,indent=0):
    return glue_template.format(
        name        =name,
        input_t     =param['input_t'],
        output_t    =param['output_t'],
        input       =input,
        output      =output,
        indent      =" "*indent
    )
