sliding_window_template = """
{indent}{name}_sliding_window<0>({input},{output});

"""

def gen_sliding_window_module(name,param,input,output,indent=0):
    return sliding_window_template.format(
        name                =name,
        input_t             =param['input_t'],
        output_t            =param['output_t'],
        input               =input,
        output              =output,
        indent              =" "*indent
    )
