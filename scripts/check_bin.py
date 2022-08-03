import sys
#import tempfile
import parser
import argparse
#import json
import numpy as np
#import random
import os
#import onnx
#from PIL import Image
#from google.protobuf import json_format

sys.path.append(os.environ.get("FPGACONVNET_HLS"))

#import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2
#import fpgaconvnet_optimiser.tools.onnx_helper as onnx_helper

from tools.onnx_data import ONNXData, validate_output

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Script for comparing sample binary files")
    parser.add_argument('-gp', '--gold_path',metavar='PATH',required=True,
            help='Path to golden data generated from onnx (.bin)')
    parser.add_argument('-ap', '--actual_path',metavar='PATH',required=True,
            help='Path to actual board-generated data (.bin)')
    parser.add_argument('-dw', '--data_width',required=False,type=int,default=16,
            help='Number of bits')
    parser.add_argument('-e', '--error_tolerance',required=False,type=float,default=0.2,
            help='Allowable error between gold and actual data')

    #TODO add custom fixed point precision split, defaulting to 8,8

    args = parser.parse_args()

    validate_output(args)
