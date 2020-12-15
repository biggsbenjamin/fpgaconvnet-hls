batch_norm_template="""
{indent}{name}_batch_norm<0>({input},{scale},{shift},{output});

"""

def gen_batch_norm_module(name,param,input,scale,shift,output,indent=0):
    return batch_norm_template.format(
        name            =name,
        input_t         =param['input_t'],
        scale_t         =param['scale_t'],
        shift_t         =param['shift_t'],
        output_t        =param['output_t'],
        input           =input,
        scale           =scale,
        shift           =shift,
        output          =output,
        indent          =" "*indent
    )
