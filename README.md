# FpgaConvNet 2
This is CNN-to-FPGA mapping framework designed to find the optimal implementation of a CNN architecture on an FPGA for power, latency and throughput driven designs.

## Setup

_This project is currently working on Ubuntu 18.04 only_

The following programmes are needed:

1. Vivado 2019.1
2. Anaconda
3. yaml-cpp (version 0.5.3)

Also, you will need to add the environmental variable `FPGACONVNET_ROOT` that points to the install directory of this repository. 


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
> **_NOTE:_**  The note content.

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

The version 0.5.3 of yaml-cpp can be downloaded from [here](https://github.com/jbeder/yaml-cpp/archive/release-0.5.3.zip). To install, execute the following in the yaml-cpp folder:

```
mkdir build
cd build
cmake ..
make
make install
```

you may have issues with cmake as sometimes the vivado version of cmake overrides your installed version of cmake, so you may need to specify the cmake path.

## Test and Development

All testing can be found in the `test` folder, where there are seperate folders for testing modules, layers and networks.

### Module Testing

To run a module test, use the `run_test.sh` script using the following arguments:

```
run_test.sh [-m (module)] [-n (test number)] [-c,-s,-e,-i]
```

The final flags correspond to the following:
 - `-c` = C simulation
 - `-s` = Synthesis
 - `-e` = Co-simulation
 - `-i` = Implementation

To run c-simulation for the 0th test if the __fork__ module for example, you would run `./run_test.sh -m fork -n 0 -c`.

For more details on what the `run_test.sh` script is calling, look at the `scripts/run_hls.tcl` script, which Vivado HLS calls.

The general file structure for each module test is as follows:

```bash
module/
├── config
│   └── config_n.json     # file containing parameters for test n
├── data
│   └── test_n
│       └── data.yaml     # data generated by gen_data.py script for the HLS testbench
├── module_test_prj/
├── gen_data.py           # generates data.yaml file from config_n.json
├── rpt
│   └── test_n.json       # a report generated which gives usage and performance for module
├── src
│   └── module.cpp        # main DUT file, where the module is called
└── tb
    ├── module_param.hpp  # header file containing config_n.json parameters as C++ macros
    ├── module_tb.cpp     # module testbench
    └── module_tb.hpp     # header file for DUT and testbench
```

For each module, a functional model is needed to generate reference test data. These models can be found in the `models/modules/` folder. The `gen_data.py` script calls this model for the given parameters.

The data is then loaded into the testbench using the `load_data` function from the `include/common_tb.cpp` folder. This function is overloaded for data of different dimensionality. This is then converted to a hls stream using the `to_stream` function.

 > **_NOTE:_**  The `load_data` is where the yaml-cpp dependency comes from

In terms of calling the DUT, currently several macros need to be defined in order to call the function with constant parameters. The general layout of the `src/module.cpp` file is as such:

```cpp
#include "module_tb.hpp"

#define MODULE_NAME MODULE
#define name        test
#include "module.hpp"

void module_top(stream_t(data_t) &in, stream_t(data_t) &out) {
    #pragma HLS DATAFLOW
    test_module(in,out);
}

#undef MODULE_NAME
#undef name
```

The `MODULE_NAME` and `name` must be defined prior to including the module. The `MODULE_NAME` macro is a prefix for all parameters defined in the `module_param.hpp` header file. The `name` macro is used to identify the instance of the module (in this case `test_module`). It is important to undefine these macros after the module is instantiated to avoid conflicting names.

### Layer Testing

__TODO__

### Network Testing

__TODO__
