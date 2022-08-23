import sys
#import tempfile
import parser
import argparse
import json
import numpy as np
#import random
import os
#import onnx
#from PIL import Image
from google.protobuf import json_format
import fpgaconvnet_optimiser.proto.fpgaconvnet_pb2
#import fpgaconvnet_optimiser.tools.onnx_helper as onnx_helper
from onnx_data import gen_layer_name, get_layer_from_partition

sys.path.append(os.environ.get("FPGACONVNET_OPTIMISER"))
sys.path.append(os.environ.get("FPGACONVNET_HLS"))

########################################################
################## Host Code Template ##################
########################################################

hc_template = """/* THIS CODE IS AUTO GENERATED
* This application configures UART 16550 to baud rate 9600.
* PS7 UART (Zynq) is not initialized by this application, since
* bootrom/bsp configures it to baud rate 115200
*
* ------------------------------------------------
* | UART TYPE   BAUD RATE                        |
* ------------------------------------------------
*   uartns550   9600
*   uartlite    Configurable only in HW design
*   ps7_uart    115200 (configured by bootrom/bsp)
*/

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_cache.h"
//sd header
#include "ff.h"
//dma header
#include "xaxidma.h"
//timer header
#include "xtime_l.h"

{includes}
//ip config pointers and handlers
XAxiDma axiDMA;
XAxiDma_Config *axiDMA_cfg;

{config_pntrs}
// sd file system from example
FATFS fatfs;
static char input_file[20] = "i0.bin"; //NOTE 20 seems to be max allowed
static char output_file[20] = "o0.bin";
static char *sd_in;
static char *sd_out;

//fcn input and outpt data sample sizes
#define INPUT_SIZE {input_size}
#define OUTPUT_SIZE {output_size}
#define BATCH_SIZE {batch_size}

#define SIZE_IN INPUT_SIZE*BATCH_SIZE
#define SIZE_OUT OUTPUT_SIZE*BATCH_SIZE

uint16_t in_stream[SIZE_IN]; //use ap_fixed type

//DMA addresses (from yt example)
#define MEM_BASE_ADDR 0x01000000
#define RX_BUFF_BASE (MEM_BASE_ADDR + 0x00100000)

// *************** initialisation ****************

void initPeriph() {{
    //initialising dma
    xil_printf("Initialising dma\\n\\r");
    axiDMA_cfg = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);
    if (axiDMA_cfg) {{
            int status = XAxiDma_CfgInitialize(&axiDMA,axiDMA_cfg);
            if (status != XST_SUCCESS) {{
                    xil_printf("Error initalising dma\\n");
            }}
    }}
    //disable interrupts (from example)
    XAxiDma_IntrDisable(&axiDMA, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrDisable(&axiDMA, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

    //initialising sd
    FRESULT rc;

    rc = f_mount(&fatfs,"",1);
    if (rc) {{
            xil_printf(" ERROR : f_mount returned %d\\r\\n", rc);
            return XST_FAILURE;
    }}

    {layer_inits}
    return XST_SUCCESS;
}}

// *************** sd card functions ****************

int sd_read( void* dest_addr) {{
    FIL fil;
    FRESULT rc;
    uint length = 0;
    uint bytes_read = 0;
    sd_in = (char *)input_file;
    u32 byte_len = SIZE_IN * sizeof(uint16_t);
    //open file to read
    rc = f_open(&fil, sd_in, FA_READ);
    if (rc) {{
            xil_printf(" ERROR : %s f_open returned %d\\r\\n", sd_in, rc);
            return XST_FAILURE;
    }}
    // get length of file
    length=f_size(&fil);
    //pointer to beginning of file
    rc = f_lseek(&fil, 0);
    if (rc) {{
            xil_printf(" ERROR : %s f_lseek returned %d\\r\\n", sd_in, rc);
            return XST_FAILURE;
    }}
    //file obj, address to read into, file size, number of bytes actually read
    rc = f_read(&fil, dest_addr, length, &bytes_read);
    if (rc) {{
            xil_printf(" ERROR : %s f_read returned %d\\r\\n", sd_in, rc);
            return XST_FAILURE;
    }}

    //check if bytes read equals to expected number of bytes
    if((bytes_read != byte_len)&&(byte_len != 0)) {{
            xil_printf(" ERROR: File: '%s', Expected size: %d; Actual size: %d\\r\\n", sd_in, byte_len, bytes_read);
            return XST_FAILURE;
    }}
    //close the file
    rc = f_close(&fil);
    if (rc) {{
            xil_printf(" ERROR : %s f_close returned %d\\r\\n", sd_in, rc);
            return XST_FAILURE;
    }}
    //if (FPGACONVNET_DEBUG) {{
    //	xil_printf("(DEBUG) SD card: File '%s' read. Length: %d; Addr.: 0x%08x\\r\\n", sd_in, length, dest_addr);
    //}}
    return XST_SUCCESS;
}}

int sd_write(u32* start_addr) {{
    FIL fil;
    FRESULT rc;
    uint length;
    sd_out = (char *)output_file;
    u32 byte_len = SIZE_OUT * sizeof(uint16_t);
    rc = f_open(&fil, sd_out, FA_CREATE_ALWAYS | FA_WRITE);
    if (rc) {{
        xil_printf(" ERROR : %s f_open returned %d\\r\\n", sd_out, rc);
        return XST_FAILURE;
    }}
    //length=file_size(&fil);
    rc = f_lseek(&fil, 0);
    if (rc) {{
        xil_printf(" ERROR : %s f_lseek returned %d\\r\\n", sd_out, rc);
        return XST_FAILURE;
    }}
    rc = f_write(&fil, start_addr, byte_len, &length);
    if (rc) {{
        xil_printf(" ERROR : %s f_write returned %d\\r\\n", sd_out, rc);
        return XST_FAILURE;
    }}
    if(length != byte_len) {{
        xil_printf("Expected written bytes: %d. Written bytes: %d\\r\\n", byte_len, length);
        return XST_FAILURE;
    }}
    rc = f_close(&fil);
    if (rc) {{
        xil_printf(" ERROR : %s f_close returned %d\\r\\n", sd_out, rc);
        return XST_FAILURE;
    }}
    //if (FPGACONVNET_DEBUG) {{
    //    xil_printf("(DEBUG) SD card: File '%s' written. Length: %d\\r\\n", sd_out, length);
    //}}
    return XST_SUCCESS;
}}

// *************************** MAIN ******************************

int main() {{
    init_platform();
    print("Hello World\\n\\r");

    // flush and disable caches
    Xil_DCacheFlush();
    Xil_DCacheDisable();

    //initialising layers, dma blocks, sd card
    initPeriph();

    //pointer to dma rx addresses
    u32 *m_dma_buff_rx = (u32*) RX_BUFF_BASE;

    // read from sd and write to input array
    sd_read(in_stream);
    //non sd data
    /*uint16_t data;
	for (int idx=0;idx<SIZE_IN;idx++) {{
		data = idx % 16;
		in_stream[idx] = data;
	}}*/

    // ****************** starting core *****************
    xil_printf("Starting Core\\n\\r");

    //start fcn blocks
{layer_starts}
    //flush buffer caches?
    Xil_DCacheFlushRange((u32)in_stream, SIZE_IN*sizeof(uint16_t));
    Xil_DCacheFlushRange((u32)m_dma_buff_rx, SIZE_OUT*sizeof(uint16_t));

    //start timer
    //XScuTimer_Start(&timer);
    XTime tStart, tEnd;
    double elapsedTime, diff;

    //setting up simple dma transfers
    //xil_printf("Receive command for DMA\\n\\r");
    XAxiDma_SimpleTransfer(&axiDMA, (u32) m_dma_buff_rx, SIZE_OUT * sizeof(uint16_t), XAXIDMA_DEVICE_TO_DMA);
    //xil_printf("Send command for DMA\\n\\r");

    XAxiDma_SimpleTransfer(&axiDMA, (u32) in_stream, SIZE_IN * sizeof(uint16_t), XAXIDMA_DMA_TO_DEVICE);

    //start timer
    XTime_GetTime(&tStart);
    //check when dma has finished - total time from ddr -> core -> ddr
    while (XAxiDma_Busy(&axiDMA, XAXIDMA_DEVICE_TO_DMA) || XAxiDma_Busy(&axiDMA, XAXIDMA_DMA_TO_DEVICE));
    //stop timer
    XTime_GetTime(&tEnd);

    diff = 2.0 * (tEnd - tStart);
    //elapsed time calculated in us
    elapsedTime = 1000000*(tEnd-tStart)/(COUNTS_PER_SECOND);

    xil_printf("Output took %llu clock cycles.\\n\\r", diff);
    xil_printf("DMA transactions finished. Timer: %.2f us\\n\\r",elapsedTime);

    //invalidate cache to avoid reading rubbish (shouldnt be needed since disabled)
    //Xil_DCacheInvalidateRange((u32) m_dma_buff_rx, SIZE_OUT * sizeof(uint16_t));

    //write finished data to .bin file
    sd_write(m_dma_buff_rx);

    xil_printf("done write\\n\\r");

    cleanup_platform();
    return 0;
}}
"""

# class for host code layer templates
class LayerHostCode:
    def __init__(self, raw_name, lyr_num, net_name):
        self.raw_name=raw_name
        self.lyr_num=lyr_num
        self.NET=net_name.upper()
        self.NAME_TOP="X{}_TOP".format(self.raw_name.upper())
        self.NAME="X{}".format(self.raw_name.upper())
        self.name="{}".format(self.NAME_TOP.lower())
        self.Name="{}{}".format(self.NAME_TOP[0:2],self.name[2:])

        #DEBUG
        #print(self.get_include())
        #print(self.get_code_handlers())
        #print(self.get_layer_inits())
        #print(self.get_layer_starts())

    def get_include(self):
        # return string of include for layer
        inc_str = "#include \"{}.h\"\n".format(self.name)
        return inc_str

    def get_code_handlers(self):
        # gen handler
        hndlr = "{} lyr{};\n".format(self.Name,self.lyr_num)
        # gen cfg pntr
        cfg = "{}_Config *lyr{}_cfg;\n".format(self.Name,self.lyr_num)
        return hndlr+cfg

    def get_layer_inits(self):
        # return code for initialisation of layer
        init ="""
    xil_printf("Initialising fpgaconvnet layer{num}\\n\\r");
    lyr{num}_cfg = {Name}_LookupConfig(XPAR_{NET}_NETWORK_{NAME}_DEVICE_ID);
    if (lyr{num}_cfg) {{
        int status = {Name}_CfgInitialize(&lyr{num}, lyr{num}_cfg);
        if (status != XST_SUCCESS) {{
            xil_printf("Error initalising layer :{num}:\\n\\r");
        }}
    }}
""".format(num=self.lyr_num,Name=self.Name,
        NET=self.NET,NAME=self.NAME[1:])
        return init

    def get_layer_starts(self):
        # return string for layer start command
        strt="    {}_Start(&lyr{});\n".format(self.Name,self.lyr_num)
        return strt

########################################################
################ Split HW Gen Functions ################
########################################################

def gen_host_code(args, prt):
    # Returns some sort of automated generation of host code
    # (required manual integration with sdk)
    print("PLEASE NOTE: HOST CODE REQUIRES MANUAL INTEGRATION WITH VIVADO SDK")
    net = args.network_name

    layer_names = get_layers(args, prt)
    # add the top layer
    layer_names.insert(0, net)
    hc_layers = []
    lyr_inc=""
    lyr_cfg=""
    lyr_inits=""
    lyr_strts=""
    top = LayerHostCode(net, 0, net)
    for idx,l in enumerate(layer_names):
        num=idx
        Lyr = LayerHostCode(l,num,net)
        lyr_inc     +=  Lyr.get_include()
        lyr_cfg     +=  Lyr.get_code_handlers()
        lyr_inits   +=  Lyr.get_layer_inits()
        lyr_strts   +=  Lyr.get_layer_starts()

    # TODO get these sizes from partition info
    input_size=784
    output_size=10
    batch_size = prt.batch_size

    hc = hc_template.format(includes        =lyr_inc,
                            config_pntrs    =lyr_cfg,
                            input_size      =input_size,
                            output_size     =output_size,
                            batch_size      =batch_size,
                            layer_inits     =lyr_inits,
                            layer_starts    =lyr_strts)

    # TODO save the host code to partition
    pth = args.json_path # might as well put it here
    pth = os.path.abspath(pth)
    pth = os.path.dirname(pth)
    op_pth = os.path.join(pth, f"{net}_host_code.c")
    #write...
    with open(op_pth,'w') as f:
        f.write(hc)

# Returns the layer names as a list (to tcl)
def get_layers(args, prt):
    lyrs=[]
    for lyr in prt.layers:
        lyr_name = gen_layer_name(lyr)
        if lyr.name != lyr_name:
            raise NameError("ERROR: layer name difference")
        print(lyr.name) #for Tcl
        lyrs.append(lyr.name)
    return lyrs #for non-Tcl use

# Returns the output node(s as a list) followed by coarse factor
def get_conn(args, prt):
    # search through the partition
    lyr = args.layer_name
    lyr_obj = get_layer_from_partition(prt, lyr)

    lyr_out = lyr_obj.node_out # might be a list...
    crs_out = lyr_obj.parameters.coarse_out # should be consistent btwn lyrs
    result=""
    for l in lyr_out:
        result+=str(l)+" "
    result += str(crs_out)
    # result form: lyr0 lyr1 ... lyrN crs_out
    print(result)

# Returns the input layer of the partition
def get_input(args,prt):
    for l in prt.layers:
        if l.streams_in[0].name == "in":
            print(l.name)
            return
    raise NameError("ERROR: no inputs or something?")

# Returns the output layer of the partition
def get_output(args,prt):
    #FIXME might break for weirdly connected layers
    for l in prt.layers:
        if l.streams_out[0].name == "out":
            print(l.name)
            return
    raise NameError("ERROR: no outputs or something?")

if __name__ == "__main__":
    #NOTE arguments returned are delineated by spaces, newline
    parser = argparse.ArgumentParser(
            description="Script for getting layer names (and connections)")
    # regular top level arguments
    parser.add_argument('-p', '--json_path', metavar='PATH', required=True,
            help='Path to network json file (.json)')
    parser.add_argument('-pi', '--partition_index', type=int, required=False,
            default=0, help='Index of the target partition in the json')

    # first go at sub parser - separate one for each command
    subparsers = parser.add_subparsers(dest='helper_func', required=True)

    #get_layers
    get_layers_p = subparsers.add_parser("get_layers",
            help="Get the layer name for each layer in the partition")
    #get_conn
    get_conn_p = subparsers.add_parser("get_conn",
            help="Get the connections for a layer")
    get_conn_p.add_argument("-n","--layer_name", type=str, required=True)
    #get_input
    get_input_p = subparsers.add_parser("get_input",
            help="Get the input layer name")
    #get_output
    get_output_p = subparsers.add_parser("get_output",
            help="Get the output layer name")

    gen_hc_p = subparsers.add_parser("gen_host_code",
            help="Generate the host code for the network")
    gen_hc_p.add_argument("-n","--network_name",type=str,required=True)

    # parse arguments
    args = parser.parse_args()

    # load partition information
    partitions = fpgaconvnet_optimiser.proto.fpgaconvnet_pb2.partitions()
    with open(args.json_path,'r') as f:
        json_format.Parse(f.read(), partitions)

    # select helper function
    if args.helper_func == 'get_layers':
        get_layers(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'get_conn':
        get_conn(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'get_input':
        get_input(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'get_output':
        get_output(args, partitions.partition[args.partition_index])
    elif args.helper_func == 'gen_host_code':
        gen_host_code(args, partitions.partition[args.partition_index])
    else:
        raise NotImplementedError(
                f"ERROR: {args.helper_func} does not exist")
