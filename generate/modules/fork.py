fork_template="""
{indent}fork<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_COARSE
#if {NAME}_KERNEL_SIZE > 1
{indent}    ,{NAME}_KERNEL_SIZE
#endif
{indent}>({input_stream},{output_stream});

"""

def gen_fork_module(name,input_stream,output_stream,indent=0):
    return fork_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        output_stream   =output_stream,
        indent          =" "*indent
        #single_stream   ="" if not single_stream else "//"
    )
