#!/bin/bash
../../../scripts/run_network.sh -n single_layer \
    -m single_layer.onnx \
    -d input_example.png \
    -p single_layer.prototxt \
    -b xilinx.com:zc706:part0:1.4 \
    -f xc7z045ffg900-2 \
    -g
