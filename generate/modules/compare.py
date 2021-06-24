compare_template="""
{indent}compare<
{indent}    {NAME}_BATCH_SIZE
{indent}>({input_max},{input_thr},{threshold},{output_stream});

"""

def gen_compare_module(name,input_max,input_thr,threshold,output_stream,indent=0):
    return compare_template.format(
        NAME            =name.upper(),
        input_max       =input_max,
        input_thr       =input_thr,
        threshold       =threshold,
        output_stream   =output_stream,
        indent          =" "*indent
    )


