conv_template = """
{indent}conv<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_FILTERS,
{indent}    {NAME}_GROUPS 
#if {NAME}_KERNEL_SIZE_X > 1 || {NAME}_KERNEL_SIZE_Y > 1
{indent}    ,{NAME}_FINE,
{indent}    {NAME}_KERNEL_SIZE_X,
{indent}    {NAME}_KERNEL_SIZE_Y
#endif
{indent}>({input_stream},{weights_stream},{output_stream});

"""

def gen_conv_module(name,input_stream,weights_stream,output_stream,indent=0):
    return conv_template.format(
        NAME            =name.upper(),
        input_stream    =input_stream,
        weights_stream  =weights_stream,
        output_stream   =output_stream,
        indent          =" "*indent
    )
