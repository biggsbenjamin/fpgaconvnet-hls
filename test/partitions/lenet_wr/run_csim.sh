#!/bin/bash

# Initialise variables
NETWORK=lenet_wr
MODEL_PATH=$FPGACONVNET_OPTIMISER/examples/models/lenet.onnx
WEIGHTS_PATH=""
PARTITION_INFO_PATH=lenet.json
IMAGE_PATH=input_example.png
ZYNQ_PART=xc7z020clg484-1
ZYNQ_BOARD=xilinx.com:zc702:part0:1.4
TEST_TYPE=sim

# move into network folder
#cd $NETWORK

# create outputs folder
mkdir -p outputs

# get the number of partitions
NUM_PARTITIONS=$( jq '.partition | length' $PARTITION_INFO_PATH )

# iterate over partitions
#for i in $( seq 1 ${NUM_PARTITIONS} ); do

    # get current partition index
    #PARTITION_INDEX=$(( $i - 1 ))
    PARTITION_INDEX=0

    # create folders
    mkdir -p partition_${PARTITION_INDEX}
    mkdir -p partition_${PARTITION_INDEX}/tb
    mkdir -p partition_${PARTITION_INDEX}/src
    mkdir -p partition_${PARTITION_INDEX}/include
    mkdir -p partition_${PARTITION_INDEX}/data

    # get weights reloading factor
    WEIGHTS_RELOADING_FACTOR=$( jq .partition[$PARTITION_INDEX].weights_reloading_factor $PARTITION_INFO_PATH )
    WEIGHTS_RELOADING_LAYER=$( jq .partition[$PARTITION_INDEX].weights_reloading_layer $PARTITION_INFO_PATH )
    WEIGHTS_RELOADING_FLAG=1
    if [ "$WEIGHTS_RELOADING_LAYER" == "None" ]; then
        WEIGHTS_RELOADING_FLAG=0
    fi

    # port information
    PORT_WIDTH=64 # TODO: get from partition information file
    #PORTS_IN=$( jq .[$PARTITION_INDEX].partition_info.ports_in $PARTITION_INFO_PATH )
    #PORTS_OUT=$( jq .[$PARTITION_INDEX].partition_info.ports_out $PARTITION_INFO_PATH )

    # partition frequency
    FREQ=100 # TODO: get from partition information file

    # create hardware
    python $FPGACONVNET_HLS/scripts/generate_hardware.py -n $NETWORK -m $MODEL_PATH -p $PARTITION_INFO_PATH -i $PARTITION_INDEX

    # format weights
    python $FPGACONVNET_HLS/scripts/format_weights.py -m $MODEL_PATH -p $PARTITION_INFO_PATH -i $PARTITION_INDEX

    # format featuremaps
    python $FPGACONVNET_HLS/scripts/format_featuremaps.py -m $MODEL_PATH -p $PARTITION_INFO_PATH -d $IMAGE_PATH -i $PARTITION_INDEX

    #run cosim
    cd partition_${PARTITION_INDEX}
    vivado_hls -f $FPGACONVNET_HLS/scripts/run_hls.tcl "_  -type sim -name ${NETWORK} -fpga ${ZYNQ_PART} -network_flag -reset -fast"
    cd ..

#done
