# FpgaConvNet 2
This is CNN-to-FPGA mapping framework designed to find the optimal implementation of a CNN architecture on an FPGA for power, latency and throughput driven designs.

## Setup

_This project has only been verified on Ubuntu 18.04_

The following programmes are needed:

1. Vivado 2019.1
2. Anaconda
3. yaml-cpp (version 0.5.3)

Also, you will need to add the environmental variable `FPGACONVNET_HLS` that points to the install directory of this repository. 


### Vivado Setup

To install Vivado 2019.1, first [download](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/vivado-design-tools/2019-1.html) from the Xilinx website.

Once installed, you will need to add a license server to your .bashrc file. You will also need to add Vivado to your path. To do so, add the following to your .basrc:

```
export PATH=/tools/Xilinx/Vivado/2019.1/bin:$PATH
export PATH=/tools/Xilinx/SDK/2019.1/bin:$PATH
```


You will need to setup JTAG drivers to program a device. To do so, execute the following script:

```
/tools/Xilinx/Vivado/2019.1/data/xicom/cable_drivers/lin64/install_script/install_drivers/install_drivers
```

For more information, visit [here](https://www.xilinx.com/support/answers/59128.html).

### Anaconda Setup

Miniconda is the recommended python environment manager to install. Download and setup instructions can be found [here](https://docs.conda.io/en/latest/miniconda.html).

After install conda, you will need to setup an fpgaconvnet environment. Run the following commands to do so:

```
conda create -n fpgaconvnet python=3.6
conda activate fpgaconvnet
```

Next you will need to install ONNX and it's dependencies. Currently this project is working with the latest version (1.8) only, so will need to create from source.

```
conda install -c conda-forge protobuf numpy
git clone https://github.com/onnx/onnx.git
cd onnx
git submodule update --init --recursive
python setup.py install
cd ..
```

And next you will have to install the rest of the packages needed.

```
conda install -c anaconda caffe
python -m pip install -r requirements.txt
```

### yaml-cpp Setup

> **_NOTE:_**  This dependency is only for testing complete networks, and is being phased out.

The version 0.5.3 of yaml-cpp can be downloaded from [here](https://github.com/jbeder/yaml-cpp/archive/release-0.5.3.zip). To install, execute the following in the yaml-cpp folder:

```
mkdir build
cd build
cmake ..
make
make install
```

you may have issues with cmake as sometimes the vivado version of cmake overrides your installed version of cmake, so you may need to specify the cmake path.


