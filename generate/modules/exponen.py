exponen_template="""
{indent}exponen<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS
{indent}>({input_stream},{output_stream});

"""

def gen_exponen_module(name,input_stream,output_stream,indent=0):
    return exponen_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        indent          =" "*indent
    )

