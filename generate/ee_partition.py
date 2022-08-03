import json
import os
import shutil
import numpy as np
import sys

sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

from generate.partition_template import *
from generate.ee_partition_template import *

from generate.layers.convolution    import gen_convolution_layer
from generate.layers.pooling        import gen_pooling_layer
#from generate.layers.concat         import gen_concat_layer
from generate.layers.relu           import gen_relu_layer
from generate.layers.inner_product  import gen_inner_product_layer
from generate.layers.squeeze        import gen_squeeze_layer
# EE layer generators
from generate.layers.split          import gen_split_layer
from generate.layers.buffer         import gen_buffer_layer
from generate.layers.softmax_cmp    import gen_softmax_cmp_layer
#from generate.layers.exit_select    import gen_exit_select # TODO

import fpgaconvnet_optimiser.tools.graphs as graphs
import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2 as fpgaconvnet_pb2
from google.protobuf.json_format import MessageToDict
from tools.onnx_data import get_layer_from_partition, gen_layer_name # REQUIRED EDIT

# class for weight definition
class generate_weight_def:
    # initialise class
    def __init__(self, name, kernel_size_x=1, kernel_size_y=1, wr=False):
        self.name       = name
        self.type       = "static" if wr else "const static"
        self.kernel_dim = "[{NAME}_KERNEL_SIZE_X][{NAME}_KERNEL_SIZE_Y]".format(NAME=self.name.upper())

    def __str__(self):
        return """
{type} {name}_weight_t {name}_weights[{NAME}_COARSE_IN*{NAME}_COARSE_GROUP][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_GROUP*{NAME}_COARSE_OUT*{NAME}_KERNEL_SIZE_X*{NAME}_KERNEL_SIZE_Y)]{kernel_dim} = {{
#include "{name}_weights_0.csv"
}};
""".format(
        NAME=self.name.upper(),
        name=self.name,
        type=self.type,
        kernel_dim=self.kernel_dim
    )

# class for weight initialisation
class generate_weight_init():
    # initialise class
    def __init__(self, name, wr=False):
        self.name = name
        self.bram_type = "RAM" if wr else "ROM"

    def __str__(self):
        return """
#pragma HLS ARRAY_PARTITION variable={name}_weights complete dim=1
#pragma HLS ARRAY_PARTITION variable={name}_weights complete dim=2
#pragma HLS RESOURCE variable={name}_weights core={bram_type}
//#pragma HLS STREAM variable={name}_weights off
#pragma HLS STABLE variable={name}_weights

""".format(name=self.name,bram_type=self.bram_type)

######## Generating biases #########
def generate_bias_def(name, partition_name, wr=False):
    wr = "[{PNAME}_WEIGHTS_RELOADING_FACTOR]".format(PNAME=partition_name.upper()) if wr else ""
    div = "{NAME}_COARSE_OUT".format(NAME=name.upper())
    div += "*{PNAME}_WEIGHTS_RELOADING_FACTOR".format(PNAME=partition_name.upper()) if wr else ""
    return """
const static {name}_biases_t {name}_biases{wr}[{NAME}_COARSE_OUT][DIVIDE({NAME}_FILTERS,{div})] = {{
#include "{name}_biases.csv"
}};
""".format(NAME=name.upper(), name=name, wr=wr, div=div)

def generate_bias_init(name, wr=False):
    bi = """
#pragma HLS ARRAY_PARTITION variable={name}_biases complete dim=1""".format(name=name)
    if wr:
        bi += """
#pragma HLS ARRAY_PARTITION variable={name}_biases complete dim=2""".format(name=name)
    bi +="""
#pragma HLS RESOURCE variable={name}_biases core=ROM
#pragma HLS STABLE variable={name}_biases
""".format(name=name)
    return bi

## class for stream initialisation
#class generate_stream_init():
#    # initialise class
#    def __init__(self):
#
#    def __str__(self):
#        return ''
#
#    def __repr__(self):
#        return self.__str__()

def gen_network(name,partition,output_path, ee_profiling=False):

    def _fix_identifier(name):
        if name[0].isdigit():
            return "n" + name
        else:
            return name

    #partition.weights_reloading_layer =
    #_fix_identifier(partition.weights_reloading_layer)
    #for layer in partition.layers:
    #    for stream_in in layer.streams_in:
    #        stream_in.name = #_fix_identifier(stream_in.name)
    #    for stream_out in layer.streams_out:
    #        stream_out.name = #_fix_identifier(stream_out.name)

    #   layer.name = #_fix_identifier(layer.name)


    wr_layer = partition.weights_reloading_layer

    wr_layer_identifier = "" #NOTE might break
    if wr_layer != "None":
        layer_object = get_layer_from_partition(partition, wr_layer)
        wr_layer_identifier = gen_layer_name(layer_object) #_fix_identifier(wr_layer).replace("/", "_")


    batch_size = partition.batch_size

    input_node  = partition.input_node
    output_node = partition.output_node

    # get all streams
    streams = {}
    ctrl_streams = {}
    split_streams = {}
    for layer in partition.layers:
        for stream_in in layer.streams_in:
            if stream_in.name == 'in' and ee_profiling: #separate input stream
                #TODO make sure pragmas are added
                i_stream='stream_t(data_t) in[{coarse}]'.format(coarse=stream_in.coarse)
                continue
            if stream_in.ctrl:
                ctrl_streams[stream_in.name] = [stream_in.coarse, f"{layer.name}_input_t"]
            elif stream_in.split:
                continue #will be included in stream outs
            else:
                streams[stream_in.name] = [stream_in.coarse, f"{layer.name}_input_t"]
        for stream_out in layer.streams_out:
            if stream_out.name == 'out' and ee_profiling: #separate output stream
                o_stream='stream_t(data_t) out[{coarse}]'.format(coarse=stream_out.coarse)
                continue
            if layer.type == fpgaconvnet_pb2.layer.layer_type.SPLIT:
                split_streams[stream_out.name] = [layer.parameters.ports_out,stream_out.coarse,
                        f"{layer.name}_output_t"]
                #[layer.parameters.ports_out, stream_out.coarse]
            elif stream_out.ctrl:
                ctrl_streams[stream_out.name] = [stream_out.coarse, f"{layer.name}_output_t"]
            else:
                streams[stream_out.name] = [stream_out.coarse, f"{layer.name}_output_t"]
    if ee_profiling:
        io_stream=i_stream+',\n    '+o_stream

    print('streams',streams)
    print('ctrl',ctrl_streams)
    print('split',split_streams)

    # create stream initialisations
    streams_init = ""
    for stream in streams:
        streams_init +=  """
    stream_t({stream_type}) {stream_name}[{coarse}];
#pragma HLS STREAM variable={stream_name}
#pragma HLS ARRAY_PARTITION variable={stream_name} complete dim=0
""".format(stream_name=stream,coarse=streams[stream][0],stream_type=streams[stream][1])

    for key in split_streams.keys():
        streams_init +=  """
    stream_t(data_t) {stream_name}[{ports}][{coarse}];
#pragma HLS STREAM variable={stream_name}
#pragma HLS ARRAY_PARTITION variable={stream_name} complete dim=0
""".format( stream_name=key,
            ports=split_streams[key][0],
            coarse=split_streams[key][1])

    for ctrl in ctrl_streams:
        streams_init +=  """
    stream_t(data_t) {stream_name}[{ctrl_out}];
#pragma HLS STREAM variable={stream_name}
#pragma HLS ARRAY_PARTITION variable={stream_name} complete dim=0
""".format(stream_name=ctrl[0],ctrl_out=ctrl[1])

    # weight information
    weights = ""
    weights_init = ""
    # bias information
    biases = ""
    biases_init = ""

    # layers initialisation
    layers = ""

    # generate hardware
    for layer_idx,layer in enumerate(partition.layers):
        print("Layer index:", layer_idx)
        # get parameters of layer
        parameters = MessageToDict(layer.parameters, preserving_proto_field_name=True)
        # init function arguments for this layer
        fn_args=[]
        layer_name = gen_layer_name(layer)
        # init hardware generation args
        layer_name = gen_layer_name(layer)

        args = [
            layer_name,
            parameters,
            os.path.join(output_path,'src',f'{layer_name}.cpp'),
            os.path.join(output_path,'include',f'{layer_name}.hpp'),
        ]
        if layer.type == fpgaconvnet_pb2.layer.layer_type.CONVOLUTION:
            # add weights to function arguments
            fn_args.append(f"{layer_name}_weights")
            if layer.parameters.has_bias:
                # add bias to arguments
                if layer_name == wr_layer:
                    fn_args.append(f"{layer_name}_biases[weights_reloading_index]")
                else:
                    fn_args.append(f"{layer_name}_biases")
            # generate hardware
            if layer.name == wr_layer:
                print("WR ADDED")
                args.append(partition.weights_reloading_factor)
            gen_convolution_layer(*args)
            # create weights
            weights += str(generate_weight_def(
                layer_name,
                kernel_size_x=int(parameters["kernel_size"][0]),
                kernel_size_y=int(parameters["kernel_size"][1]),
                wr=True if layer_name == wr_layer_identifier else False
            ))
            weights_init += str(generate_weight_init(
                layer_name,
                wr=True if layer_name == wr_layer_identifier else False
            ))
            # create biases
            if layer.parameters.has_bias:
                biases += generate_bias_def(layer.name, name,
                        wr=True if layer_name == wr_layer else False)
                biases_init += generate_bias_init(layer_name,
                        wr=True if layer_name == wr_layer else False)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.POOLING:
            gen_pooling_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.CONCAT:
            gen_concat_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.RELU:
            gen_relu_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.INNER_PRODUCT:
            # add weights to function arguments
            fn_args.append(f"{layer_name}_weights")
            if layer.parameters.has_bias:
                # add bias to arguments
                if layer_name == wr_layer:
                    fn_args.append(f"{layer_name}_biases[weights_reloading_index]")
                else:
                    fn_args.append(f"{layer_name}_biases")
            # generate hardware
            if layer.name == wr_layer:
                args.append(partition.weights_reloading_factor)
            gen_inner_product_layer(*args)
            # create weights
            weights += str(generate_weight_def(
                layer_name,
                kernel_size_x=1,
                kernel_size_y=1,
                wr=True if layer_name == wr_layer_identifier else False
            ))
            weights_init += str(generate_weight_init(
                layer_name,
                wr=True if layer_name == wr_layer_identifier else False
            ))
            # create biases
            if layer.parameters.has_bias:
                biases += generate_bias_def(layer.name, name,
                        wr=True if layer_name == wr_layer else False)
                biases_init += generate_bias_init(layer_name,
                        wr=True if layer_name == wr_layer else False)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.SQUEEZE:
            gen_squeeze_layer(*args)
        #EE layers
        if layer.type == fpgaconvnet_pb2.layer.layer_type.SPLIT:
            gen_split_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.BUFFER:
            gen_buffer_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.GREATER:
            gen_softmax_cmp_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.IF:
        #    gen_exit_select_layer(*args)
            print("need to add exit select")

        # add layer function
        for stream_in in layer.streams_in:
            fn_name = stream_in.name
            if stream_in.split: #assumption that stream_ins will have same name
                #come from a split layer so determine index
                #print("ERROR",split_streams[stream_in.name][0] )
                #print("ERROR",type(split_streams[stream_in.name][0]) )

                split_streams[stream_in.name][0] -= 1
                fn_name += "[{index}]".format(index=split_streams[stream_in.name][0])
            fn_args.append(fn_name)
        for stream_out in layer.streams_out:
            fn_args.append(stream_out.name)
            if stream_out.split:
                break # only want one argument for split_layer output
        fn_args.append("mode")
        fn_args = ", ".join(fn_args)
        layers +=  f"   printf(\"LAYER: {layer_name} \\n\");\n"
        layers += f"    {layer_name}({fn_args});\n"
        #layers += f"    printf(\"{layer_name} done\\n\");\n"

    # include generation
    include = ""
    for layer in partition.layers:
        layer_name = gen_layer_name(layer)
        include +=f"#include \"{layer_name}.hpp\"\n"

    # HEADER
    if ee_profiling:
        print("EE hw gen")
        if partition.layers[-1].type == fpgaconvnet_pb2.layer.layer_type.GREATER:
            streams_out_param = partition.layers[-1].parameters.ctrl_out_size
        else:
            streams_out_param =partition.layers[-1].parameters.coarse_out # TODO: change
        network_header = ee_network_header_template.format(
        name        =name,
        NAME        =name.upper(),
        batch_size  =batch_size,
        rows_in     =partition.layers[0].parameters.rows_in,
        cols_in     =partition.layers[0].parameters.cols_in,
        channels_in =partition.layers[0].parameters.channels_in,
        rows_out    =partition.layers[-1].parameters.rows_out,
        cols_out    =partition.layers[-1].parameters.cols_out,
        channels_out=partition.layers[-1].parameters.channels_out,
        ports       =partition.ports,
        streams_in  =partition.layers[0].parameters.coarse_in, # TODO: change
        streams_out = streams_out_param,
        wr_layer    =wr_layer,
        WR_LAYER    =wr_layer.upper(),
        wr_factor   =partition.weights_reloading_factor,
        wr_flag     =int(wr_layer != "None"),
        include     =include,

        io_stream = io_stream
    )
    else:
        network_header = network_header_template.format(
        name        =name,
        NAME        =name.upper(),
        batch_size  =batch_size,
        rows_in     =partition.layers[0].parameters.rows_in,
        cols_in     =partition.layers[0].parameters.cols_in,
        channels_in =partition.layers[0].parameters.channels_in,
        rows_out    =partition.layers[-1].parameters.rows_out,
        cols_out    =partition.layers[-1].parameters.cols_out,
        channels_out=partition.layers[-1].parameters.channels_out,
        ports       =partition.ports,
        streams_in  =partition.layers[0].parameters.coarse_in, # TODO: change
        streams_out =partition.layers[-1].parameters.coarse_out, # TODO: change
        input_layer =partition.layers[0].name,
        output_layer=partition.layers[-1].name,
        wr_layer    =wr_layer_identifier,
        WR_LAYER    =wr_layer_identifier.upper(),
        wr_factor   =partition.weights_reloading_factor,
        wr_flag     =int(wr_layer != "None"),
        include     =include
    )
    # SRC
    if ee_profiling:
        network_src = ee_network_src_template.format(
        name        =name,
        NAME        =name.upper(),
        wr_layer    =wr_layer,
        weights     =weights,
        weights_init=weights_init,
        streams_init=streams_init,
        layers      =layers,

        io_stream = io_stream
    )
    else:
        network_src = network_src_template.format(
        name        =name,
        NAME        =name.upper(),
        wr_layer    =wr_layer_identifier,
        weights     =weights,
        weights_init=weights_init,
        biases      =biases,
        biases_init =biases_init,
        streams_init=streams_init,
        layers      =layers
    )
    # TB
    print(os.path.abspath(os.path.join(output_path,'data/')))
    if ee_profiling:
        network_tb_src = ee_network_tb_src_template.format(
        name        =name,
        NAME        =name.upper(),
        input_node  =input_node,
        wr_layer    =wr_layer,
        output_node =output_node,
        data_path   =os.path.abspath(os.path.join(output_path,'data'))
    )
    else:
        network_tb_src = network_tb_src_template.format(
        name = name,
        NAME = name.upper(),
        input_data_path = os.path.join(os.getcwd(), output_path, f"data/{input_node}_0.dat"),
        weights_reloading_path = os.path.join(os.getcwd(), output_path, f"data/{wr_layer}_weights_0.dat"),
        output_data_path = os.path.join(os.getcwd(), output_path, f"data/{output_node}_0.dat")
    )
    with open(os.path.join(output_path,f'include/{name}_top.hpp'),'w') as f:
        f.write(network_header)
    with open(os.path.join(output_path,f'src/{name}_top.cpp'),'w') as f:
        f.write(network_src)
    with open(os.path.join(output_path,f'tb/{name}_tb.cpp'),'w') as f:
        f.write(network_tb_src)

if __name__=="__main__":

    gen_network(
        'lenet_test',
        'test/networks/lenet_test/lenet_test_partition_info.json',
        'test/networks/lenet_test')
