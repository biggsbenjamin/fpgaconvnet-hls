import json
import os
import shutil 
import numpy as np
import sys
sys.path.append('..')

from generate.partition_template import *

from generate.layers.convolution    import gen_convolution_layer
from generate.layers.pooling        import gen_pooling_layer
from generate.layers.concat         import gen_concat_layer
from generate.layers.relu           import gen_relu_layer
from generate.layers.split          import gen_split_layer
from generate.layers.inner_product  import gen_inner_product_layer
from generate.layers.squeeze        import gen_squeeze_layer

import tools.graphs as graphs
#import tools.fpgaconvnet_pb2 as fpgaconvnet_pb2
import proto.fpgaconvnet_pb2 as fpgaconvnet_pb2
from google.protobuf.json_format import MessageToDict

# class for weight definition
class generate_weight_def:
    # initialise class
    def __init__(self, name, kernel_size=1, wr=False):
        self.name       = name
        self.type       = "static" if wr else "const static"
        self.kernel_dim = " " if not kernel_size > 1 else "[{NAME}_KERNEL_SIZE][{NAME}_KERNEL_SIZE]".format(NAME=self.name.upper())
    
    def __str__(self):
        return """
{type} weight_t {name}_weights[{NAME}_COARSE_IN][{NAME}_COARSE_OUT][DIVIDE({NAME}_WEIGHTS,{NAME}_COARSE_IN*{NAME}_COARSE_OUT*{NAME}_KERNEL_SIZE*{NAME}_KERNEL_SIZE)]{kernel_dim} = {{ 
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

"""
# class for stream initialisation
class generate_stream_init():
    # initialise class
    def __init__(self):

    def __str__(self):
        return ''

    def __repr__(self):
        return self.__str__()
"""

def gen_network(name,partition,output_path):

    wr_layer   = partition.weights_reloading_layer
    batch_size = partition.batch_size

    input_node  = partition.input_node
    output_node = partition.output_node

    # get all streams
    streams = []
    for layer in partition.layers:
        for stream_in in layer.streams_in:
            streams.append((stream_in.name, stream_in.coarse))
        for stream_out in layer.streams_out:
            streams.append((stream_out.name, stream_out.coarse))
    
    # remove duplicates
    streams = list(set(streams))

    # create stream initialisations
    streams_init = ""
    for stream in streams:
        streams_init +=  """
    stream_t(data_t) {stream_name}[{coarse}];
#pragma HLS STREAM variable={stream_name} 
#pragma HLS ARRAY_PARTITION variable={stream_name} complete dim=0
""".format(stream_name=stream[0],coarse=stream[1])

    # weight information
    weights = ""
    weights_init = ""

    # layers initialisation 
    layers = ""
 
    # generate hardware
    for layer in partition.layers:
        # get parameters of layer
        parameters = MessageToDict(layer.parameters, preserving_proto_field_name=True)
        # init function arguments for this layer
        fn_args=[]
        # init hardware generation args
        args = [
            layer.name,
            parameters,
            os.path.join(output_path,'src',f'{layer.name}.cpp'),
            os.path.join(output_path,'include',f'{layer.name}.hpp')
        ]
        if layer.type == fpgaconvnet_pb2.layer.layer_type.CONVOLUTION:
            # add weights to function arguments
            fn_args.append(f"{layer.name}_weights")
            # generate hardware
            gen_convolution_layer(*args)
            # create weights
            weights += str(generate_weight_def(
                layer.name, 
                kernel_size=int(parameters["kernel_size"]), 
                wr=True if layer.name == wr_layer else False
            ))
            weights_init += str(generate_weight_init(
                layer.name, 
                wr=True if layer.name == wr_layer else False
            ))
        if layer.type == fpgaconvnet_pb2.layer.layer_type.POOLING:
            gen_pooling_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.CONCAT:
            gen_concat_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.RELU:
            gen_relu_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.SPLIT:
            gen_split_layer(*args)
        if layer.type == fpgaconvnet_pb2.layer.layer_type.INNER_PRODUCT:
            # add weights to function arguments
            fn_args.append(f"{layer.name}_weights")
            # generate hardware
            gen_inner_product_layer(*args)
            # create weights
            weights += str(generate_weight_def(
                layer.name, 
                kernel_size=1,
                wr=True if layer.name == wr_layer else False
            ))
            weights_init += str(generate_weight_init(
                layer.name, 
                wr=True if layer.name == wr_layer else False
            ))
        if layer.type == fpgaconvnet_pb2.layer.layer_type.SQUEEZE:
            gen_squeeze_layer(*args)
        # add layer function
        for stream_in in layer.streams_in:
            fn_args.append(stream_in.name)
        for stream_out in layer.streams_out:
            fn_args.append(stream_out.name)
        fn_args.append("mode")
        fn_args = ", ".join(fn_args)
        layers += f"    {layer.name}({fn_args});\n"
           
    # include generation
    include = ""
    for layer in partition.layers:
        include +=f"#include \"{layer.name}.hpp\"\n"
    
    # HEADER
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
        wr_layer    =wr_layer,
        WR_LAYER    =wr_layer.upper(),
        wr_factor   =partition.weights_reloading_factor,
        wr_flag     =int(wr_layer != "None"),
        include     =include
    )
    # SRC
    network_src = network_src_template.format(
        name        =name,
        NAME        =name.upper(),
        wr_layer    =wr_layer,
        weights     =weights,
        weights_init=weights_init,
        streams_init=streams_init,
        layers      =layers
    )
    # TB
    network_tb_src = network_tb_src_template.format(
        name        =name,
        NAME        =name.upper(),
        input_node  =input_node,
        wr_layer    =wr_layer,
        output_node =output_node
    )

    with open(os.path.join(output_path,f'include/{name}_top.hpp'),'w') as f:
        f.write(network_header)
    with open(os.path.join(output_path,f'src/{name}_top.cpp'),'w') as f:
        f.write(network_src)
    with open(os.path.join(output_path,f'tb/{name}_tb.cpp'),'w') as f:
        f.write(network_tb_src)

    # save modules
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/mem_read.hpp')  , os.path.join(output_path,f"include/{name}_mem_read.hpp") )
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/mem_write.hpp') , os.path.join(output_path,f"include/{name}_mem_write.hpp") )
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/mem_read.hpp')  , os.path.join(output_path,f"include/{name}_wr_mem_read.hpp") )
    shutil.copy( os.path.join(os.environ['FPGACONVNET_ROOT'],'include/wr.hpp')        , os.path.join(output_path,f"include/{name}_wr.hpp") )

if __name__=="__main__":

    gen_network(
        'lenet_test',
        'test/networks/lenet_test/lenet_test_partition_info.json',
        'test/networks/lenet_test')
