import sys
#import tempfile
import parser
import argparse
import json
import numpy as np
#import random
import os
#import onnx
#from PIL import Image
from google.protobuf import json_format
import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2
#import fpgaconvnet_optimiser.tools.onnx_helper as onnx_helper
from onnx_data import gen_layer_name, get_layer_from_partition

sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

# Returns the layer names as a list (to tcl)
def get_layers(args, prt):
    for lyr in prt.layers:
        lyr_name = gen_layer_name(lyr)
        if lyr.name != lyr_name:
            raise NameError("ERROR: layer name difference")
        print(lyr.name)

# Returns the output node(s as a list) followed by coarse factor
def get_conn(args, prt):
    # search through the partition
    lyr = args.layer_name
    lyr_obj = get_layer_from_partition(prt, lyr)

    lyr_out = lyr_obj.node_out # might be a list...
    crs_out = lyr_obj.parameters.coarse_out # should be consistent btwn lyrs
    result=""
    for l in lyr_out:
        result+=str(l)+" "
    result += str(crs_out)
    # result form: lyr0 lyr1 ... lyrN crs_out
    print(result)

# Returns the input layer of the partition
def get_input(args,prt):
    for l in prt.layers:
        if l.streams_in[0].name == "in":
            print(l.name)
            return
    raise NameError("ERROR: no inputs or something?")

# Returns the output layer of the partition
def get_output(args,prt):
    #FIXME might break for weirdly connected layers
    for l in prt.layers:
        if l.streams_out[0].name == "out":
            print(l.name)
            return
    raise NameError("ERROR: no outputs or something?")

def gen_host_code(args):
    # Returns some sort of automated generation of host code
    # (required manual integration with sdk)

    raise NotImplementedError

if __name__ == "__main__":
    #NOTE arguments returned are delineated by spaces, newline

    parser = argparse.ArgumentParser(
            description="Script for getting layer names (and connections)")
    # regular top level arguments
    parser.add_argument('-p', '--json_path', metavar='PATH', required=True,
            help='Path to network json file (.json)')
    parser.add_argument('-pi', '--partition_index', type=int, required=False,
            default=0, help='Index of the target partition in the json')

    # first go at sub parser - separate one for each command
    subparsers = parser.add_subparsers(dest='helper_func', required=True)

    #get_layers
    get_layers_p = subparsers.add_parser("get_layers",
            help="Get the layer name for each layer in the partition")
    #get_conn
    get_conn_p = subparsers.add_parser("get_conn",
            help="Get the connections for a layer")
    get_conn_p.add_argument("-n","--layer_name", type=str, required=True)
    #get_input
    get_input_p = subparsers.add_parser("get_input",
            help="Get the input layer name")
    #get_output
    get_output_p = subparsers.add_parser("get_output",
            help="Get the output layer name")

    gen_hc_p = subparsers.add_parser("gen_host_code",
            help="Generate the host code for the network")

    # parse arguments
    args = parser.parse_args()

    # load partition information
    partitions = fpgaconvnet_optimiser.proto.fpgaconvnet_pb2.partitions()
    with open(args.json_path,'r') as f:
        json_format.Parse(f.read(), partitions)

    # select helper function
    if args.helper_func == 'get_layers':
        get_layers(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'get_conn':
        get_conn(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'get_input':
        get_input(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'get_output':
        get_output(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'gen_host_code':
        gen_host_code(args)
    else:
        raise NotImplementedError(
                f"ERROR: {args.helper_func} does not exist")
