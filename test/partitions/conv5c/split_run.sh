#!/bin/bash

# Initialise variables
NETWORK=conv5c
MODEL_PATH=$FPGACONVNET_OPTIMISER/examples/models/conv5c.onnx
WEIGHTS_PATH=""
PARTITION_INFO_PATH=conv5c.json
IMAGE_PATH=input_example.png
ZYNQ_PART=xc7z045ffg900-2
ZYNQ_BOARD=xilinx.com:zc706:part0:1.4
TEST_TYPE=sim

# move into network folder
#cd $NETWORK (already in network folder)

FREQ=125
PORT_WIDTH=32 #NOTE dma port width different to axis port width
WEIGHTS_RELOADING_FLAG=0 #NOTE not currently implemented
SPLIT_NETWORK_FLAG=1 #extra arg for tcl scripts
printf "Network name: ${NETWORK}\n"
printf "Target operating frequency: ${FREQ}MHz\n"

# create outputs folder
#mkdir -p outputs

# get the number of partitions
NUM_PARTITIONS=$( jq '.partition | length' $PARTITION_INFO_PATH )
PARTITION_INDEX=0

for i in $( seq 1 ${NUM_PARTITIONS} ); do

    # get current partition index
    PARTITION_INDEX=$(( $i - 1 ))

    # create folders
    mkdir -p partition_${PARTITION_INDEX}
    #mkdir -p partition_${PARTITION_INDEX}/${NETWORK}_top/tb #FIXME add testing back in
    mkdir -p partition_${PARTITION_INDEX}/${NETWORK}_top/src
    mkdir -p partition_${PARTITION_INDEX}/${NETWORK}_top/include
    #mkdir -p partition_${PARTITION_INDEX}/${NETWORK}_top/data
    mkdir -p partition_${PARTITION_INDEX}/data

    #get number of layers
    NUM_LYRS=$( jq --argjson pi $PARTITION_INDEX '.partition[$pi].layers | length' $PARTITION_INFO_PATH )
    #echo "number of layers $NUM_LYRS "
    # create folders for each layer
    for j in $( seq 1 ${NUM_LYRS} ); do
        # get current layer index
        LYR_INDEX=$(( $j - 1 ))

        LYR_NAME=$( jq  -r --argjson pi $PARTITION_INDEX --argjson li $LYR_INDEX '.partition[$pi].layers[$li] | .name' $PARTITION_INFO_PATH)
        #get name of each layer and create folder for compilation
        #echo "layer name: $LYR_NAME "

        # create folders
        mkdir -p partition_${PARTITION_INDEX}/${LYR_NAME}
        mkdir -p partition_${PARTITION_INDEX}/${LYR_NAME}/src
        mkdir -p partition_${PARTITION_INDEX}/${LYR_NAME}/include
        #mkdir -p partition_${PARTITION_INDEX}/${LYR_NAME}/tb #FIXME add testing back in
    done

    # create hardware, #-sp is split layer compilation
    python $FPGACONVNET_HLS/scripts/generate_hardware.py -n $NETWORK -m $MODEL_PATH -p $PARTITION_INFO_PATH -i $PARTITION_INDEX -sp
    # format weights
    python $FPGACONVNET_HLS/scripts/format_weights.py -m $MODEL_PATH -p $PARTITION_INFO_PATH -i $PARTITION_INDEX
    # format featuremaps
    python $FPGACONVNET_HLS/scripts/format_featuremaps.py -m $MODEL_PATH -p $PARTITION_INFO_PATH -d $IMAGE_PATH -i $PARTITION_INDEX

    cd partition_${PARTITION_INDEX}

    ##FIXME can't get include flags to work so copying files
    mkdir -p tmp_inc

    #TODO currently gen hw py script requires paths to exits, get rid of this so that only loop is hls gen
    #synthesise implement and export ip for each layer - checking not performed
    for i in $( seq 1 ${NUM_LYRS} ); do #does all layers
    #for i in $(seq 1 1 ); do #does numbered layer
        # get current layer index
        LYR_INDEX=$(( $i - 1 ))
        LYR_NAME=$( jq  -r --argjson pi $PARTITION_INDEX --argjson li $LYR_INDEX '.partition[$pi].layers[$li] | .name' ../$PARTITION_INFO_PATH)

        #run vivado hls
        cd ${LYR_NAME}
        #vivado_hls -f $FPGACONVNET_HLS/scripts/run_hls.tcl "_ -name ${LYR_NAME} -fpga ${ZYNQ_PART} -split_net_flag -fast -reset -type impl"

        ##FIXME can't get include flags to work so copying files
        cp include/* ../tmp_inc
        cd ..
    done

    # top level axiu (stream) to stream with less signals
    cd ${NETWORK}_top
    #vivado_hls -f $FPGACONVNET_HLS/scripts/run_hls.tcl "_ -name ${NETWORK} -fpga ${ZYNQ_PART} -split_net_flag -fast -reset -type impl"
    cd ..

    ################################# set up vivado hardware ################################
    printf "\n\
#################################\n\
# Running tcl scripts in Vivado #\n\
#################################\n"

    #  ( argv 0 : network name          )
    #  ( argv 1 : fpga part name        )
    #  ( argv 2 : fpga board part       )
    #  ( argv 3 : board frequency       )
    #  ( argv 4 : port width            )
    #  ( argv 5 : weights reloading on  )
    #SPLIT NETWORK
    #  ( argv 6 : split network on      )

    vivado -mode batch -notrace -source $FPGACONVNET_HLS/scripts/gen_hw.tcl \
        -tclargs    $NETWORK \
                    $ZYNQ_PART \
                    $ZYNQ_BOARD \
                    $FREQ \
                    $PORT_WIDTH \
                    $WEIGHTS_RELOADING_FLAG \
                    $SPLIT_NETWORK_FLAG

done
