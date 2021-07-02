buff_template="""
{indent}buffer<
{indent}    {NAME}_BATCH_SIZE,
{indent}    {NAME}_ROWS,
{indent}    {NAME}_COLS,
{indent}    {NAME}_CHANNELS,
{indent}    {NAME}_DROP_MODE
{indent}>({input_stream}, {ctrl_input_stream}, {output_stream});

"""

def gen_buff_module(name,input_stream, ctrl_input_stream,output_stream,indent=0):
    return buff_template.format(
        NAME                =name.upper(),
        input_stream        =input_stream,
        ctrl_input_stream   =ctrl_input_stream,
        output_stream       =output_stream,
        indent              =" "*indent
    )


