# LeNet Example

![LeNet Topology](topology.png)

This is an example topology for the LeNet convolutional neural network, generated using [fpgaconvnet-hls](https://github.com/AlexMontgomerie/fpgaconvnet-optimiser). To generate the hardware for this network, run the following from the `examples`:

```
../../scripts/run_network.sh -n lenet \
  -m lenet.onnx \
  -d input_example.png \
  -p lenet.json \
  -b xilinx.com:zc706:part0:1.4 \
  -f xc7z045ffg900-2 \
  -g
```


