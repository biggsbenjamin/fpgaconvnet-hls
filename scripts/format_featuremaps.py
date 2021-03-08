import sys
import tempfile
import parser
import argparse
import json
import numpy as np
import random
import os
import onnx
from PIL import Image
from google.protobuf import text_format

sys.path.append(os.environ.get("FPGACONVNET_HLS"))

import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2
import fpgaconvnet_optimiser.tools.onnx_helper as onnx_helper

from tools.onnx_data import ONNXData

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Weights Formatting Script")
    parser.add_argument('-p','--partition_path',metavar='PATH',required=True,
        help='Path to partition info (.pb.bin)')
    parser.add_argument('-m','--onnx_path',metavar='PATH',required=True,
        help='Path to onnx model (.onnx)')
    parser.add_argument('-d','--image_path',metavar='PATH',required=False,
        help='Path to input image')

    # parse arguments
    args = parser.parse_args()
    
    # load partition information
    partitions = fpgaconvnet_optimiser.proto.fpgaconvnet_pb2.partitions()
    with open(args.partition_path,'r') as f:
        text_format.Parse(f.read(), partitions)
   

    # iterate over partitions
    for i, partition in enumerate(partitions.partition): 
        
        # onnx data manipulation
        onnx_data = ONNXData(partition, args.onnx_path)
 
        # load input image
        onnx_data.load_input(args.image_path)
   
        # save feature maps
        onnx_data.save_featuremap_in_out(
            f'partition_{i}/data', 
            to_bin=True, 
            to_csv=False )

