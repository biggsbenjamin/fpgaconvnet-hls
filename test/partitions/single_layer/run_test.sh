#!/bin/bash
$FPGACONVNET_HLS/scripts/run_network.sh -n single_layer \
    -m $FPGACONVNET_OPTIMISER/examples/models/single_layer.onnx \
    -d input_example.png \
    -p single_layer.json \
    -b xilinx.com:zc706:part0:1.4 \
    -f xc7z045ffg900-2 \
    -s
