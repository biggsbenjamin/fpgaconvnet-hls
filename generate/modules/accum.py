accum_template = """
{indent}{name}_accum<0>({input},{output});
"""

def gen_accum_module(name,param,input,output,indent=0,grouped=False):
    return accum_template.format(
        name        =name,
        input_t     =param['input_t'],
        output_t    =param['output_t'],
        input       =input,
        output      =output,
        indent      =" "*indent
        #is_grouped  ="" if grouped else "//"
    )
