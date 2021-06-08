pool_template="""
{indent}pool<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_KERNEL_SIZE,
{indent}    {NAME}_t
{indent}>({input_stream},{output_stream});
"""

def gen_pool_module(name,input_stream,output_stream,indent=0):
    return pool_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        indent          =" "*indent
    )
