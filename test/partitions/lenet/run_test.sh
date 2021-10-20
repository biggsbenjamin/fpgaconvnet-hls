#!/bin/bash
$FPGACONVNET_HLS/scripts/run_network.sh -n lenet \
    -m $FPGACONVNET_OPTIMISER/examples/models/lenet.onnx \
    -d input_example.png \
    -p lenet.json \
    -b xilinx.com:zc706:part0:1.4 \
    -f xc7z045ffg900-2 \
    -s
