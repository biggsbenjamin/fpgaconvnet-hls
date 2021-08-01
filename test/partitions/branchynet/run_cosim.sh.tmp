#!/bin/bash
$FPGACONVNET_HLS/scripts/run_network.sh -n branchynet \
    -m $FPGACONVNET_OPTIMISER/examples/models/branchynet.onnx \
    -d input_example.png \
    -p branchynet.json \
    -b xilinx.com:zc706:part0:1.4 \
    -f xc7z045ffg900-2 \
    -e
