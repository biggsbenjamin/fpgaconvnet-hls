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
from google.protobuf import json_format

sys.path.append(os.environ.get("FPGACONVNET_ROOT"))

import proto.fpgaconvnet_pb2
import tools.onnx_helper as onnx_helper
import tools.third_party.prototxt
from tools.onnx_data import ONNXData

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Weights Formatting Script")
    parser.add_argument('-n','--name',required=True,
        help='Name of network')
    parser.add_argument('-p','--partition_path',metavar='PATH',required=True,
        help='Path to partition info (.pb.bin)')
    parser.add_argument('-m','--onnx_path',metavar='PATH',required=True,
        help='Path to onnx model (.onnx)')

    # parse arguments
    args = parser.parse_args()
    
    # load partition information
    partitions = proto.fpgaconvnet_pb2.partitions()
    with open(args.partition_path,'r') as f:
        json_format.Parse(f.read(), partitions)
    
    # iterate over partitions
    for i, partition in enumerate(partitions.partition): 
        
        # onnx data manipulation
        onnx_data = ONNXData(partition, args.onnx_path)
    
        ## save weight coefficients ##
        onnx_data.save_weights_partition(
            f'partition_{i}/data',
            to_yaml=True,
            to_csv=True,
            to_bin=True )

        ## save batch_norm coefficients ##
        """
        self.caffe_data.save_batch_norm(
            self.partition_info[self.partition_index]['layer_info'],
            'partition_{partition_index}/data'.format(partition_index=self.partition_index))
        self.caffe_data.save_batch_norm_csv(
            'partition_{partition_index}/data'.format(partition_index=self.partition_index),
            self.partition_info[self.partition_index]['layer_info'])
        """

