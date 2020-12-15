#ifndef SRC_FPGACONVNET_HPP_
#define SRC_FPGACONVNET_HPP_

#include <stdlib.h>
#include <stdio.h>
#include "xparameters.h"
#include "xfpgaconvnet_ip.h" // TODO
//#include "xiicps.h"
#include "math.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "xdevcfg.h"
#include "xil_io.h"
#include "xil_types.h"
#include "ff.h"
#include "xscutimer.h"
#include "xil_cache.h"


#define FPGACONVNET_FILENAME_LENGTH_MAX 20
#define FPGACONVNET_WR_FACTOR_MAX 		20
#define FPGACONVNET_FILE_BUFFER_SIZE	1000

#define FPGACONVNET_PL_LOAD_WEIGHTS 	1
#define FPGACONVNET_PL_RUN_PARTITIONS 	0

#define FPGACONVNET_DEBUG 0
#define FPGACONVNET_PCAP_BASE_ADDR 0x200000
#define FPGACONVNET_DATA_BASE_ADDR 0xF00000

/* HW */
#define HW_ID		    	XPAR_FPGACONVNET_IP_DEVICE_ID // TODO
#define HW_BASE_ADDR 	    XPAR_FPGACONVNET_IP_S_AXI_CTRL_BASEADDR // TODO

#define UPDATE_NAME(a)      XFpgaconvnet_ip_##a
#define CONFIG_INITIALISE   UPDATE_NAME(CfgInitialize)
#define INTR_GLOBAL_EN      UPDATE_NAME(InterruptGlobalEnable)
#define HW_START            UPDATE_NAME(Start)
#define CHECK_READY         UPDATE_NAME(IsReady)
#define CHECK_IDLE          UPDATE_NAME(IsIdle)
#define CHECK_DONE          UPDATE_NAME(IsDone)

/* RECONFIG */
// SLCR registers (find details => ug586, page 218)
#define SLCR_LOCK					0xF8000004 // SLCR Write Protection Lock
#define SLCR_UNLOCK					0xF8000008 // SLCR Write Protection Unlock
#define SLCR_LVL_SHFTR_EN 			0xF8000900 // SLCR Level Shifters Enable
#define SLCR_PCAP_CLK_CTRL 			XPAR_PS7_SLCR_0_S_AXI_BASEADDR + 0x168 // SLCR PCAP clock control register address
#define SLCR_PCAP_CLK_CTRL_EN_MASK 	0x1
#define SLCR_LOCK_VAL				0x767B767B
#define SLCR_UNLOCK_VAL				0xDF0DDF0D
#define SLCR_LVL_PS_PL				0x0A
#define SLCR_LVL_PL_PS				0x0F
#define SLCR_INT_PCAP_LPBK			0x10

#define RECONF_ERROR_FLAGS 		(XDCFG_IXR_AXI_WERR_MASK | \
								XDCFG_IXR_AXI_RTO_MASK |  \
								XDCFG_IXR_AXI_RERR_MASK | \
								XDCFG_IXR_RX_FIFO_OV_MASK | \
								XDCFG_IXR_DMA_CMD_ERR_MASK |\
								XDCFG_IXR_DMA_Q_OV_MASK |   \
								XDCFG_IXR_P2D_LEN_ERR_MASK |\
								XDCFG_IXR_PCFG_HMAC_ERR_MASK)

//	FPGA Software Reset
#define	FPGA_RST_CTRL	XPAR_PS7_SLCR_0_S_AXI_BASEADDR+0x240

#define TIMER_LOAD_VALUE 0xFFFFFFFF
#define CLOCK_FREQ (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ/2)	//	in Hz (NOTE: frequency of scu timer is half of the processor clock)

typedef u64 port_t;

typedef struct {
    // partition info
	int partition_id;
	int batch_size;
	int wr_factor;

	// dimensions in
	int ports_in = 1;
    int size_in;
    port_t * data_in_ptr;
    int input_dep;

    // dimensions out
    int ports_out = 1;
    int size_out;
    port_t * data_out_ptr;
    int output_dep;

    // weights reloading information
    int ports_wr = 1;
    int size_wr;
    port_t * data_wr_ptr[FPGACONVNET_WR_FACTOR_MAX];

    // partition info
    int size_partition;
    u32 * partition_ptr;

    // file information
    char partition_filename[FPGACONVNET_FILENAME_LENGTH_MAX];
    char input_filename[FPGACONVNET_FILENAME_LENGTH_MAX];
    char output_filename[FPGACONVNET_FILENAME_LENGTH_MAX];
    char weights_filename[FPGACONVNET_WR_FACTOR_MAX][FPGACONVNET_FILENAME_LENGTH_MAX];

    // flags
    bool INPUT_ADDR_ASSIGNED		= false;
    bool OUTPUT_ADDR_ASSIGNED		= false;
    bool WEIGHTS_ADDR_ASSIGNED		= false;
    bool PARTITION_ADDR_ASSIGNED	= false;
    bool PARTITION_DONE		= false;
    bool LOAD_PARTITION 	= true;
    bool GEN_INPUT      	= false;
    bool LOAD_INPUT			= true;
    bool LOAD_WEIGHTS		= true;
    bool LOAD_FIRST_WEIGHTS	= true;
    bool UNLOAD_WEIGHTS 	= true;
    bool SAVE_OUTPUT		= true;

    // profiling
    float t_load_input 		= 0.0;
    float t_load_partition	= 0.0;
    float t_read_weights_sd	= 0.0;
    float t_load_weights_pl = 0.0;
    float t_run_partition	= 0.0;
    float t_save_output		= 0.0;

} partition_info_t;

class fpgaconvnet {
    // hw variables
    XFpgaconvnet_ip         hw;
    XFpgaconvnet_ip_Config *hw_config = new XFpgaconvnet_ip_Config;
    // reconf variables
    XDcfg         reconf;
    XDcfg_Config *reconf_config = new XDcfg_Config;
    // sd card variables
    FATFS fatfs;
    // timer variables
    XScuTimer         timer;
    XScuTimer_Config *timer_config = new XScuTimer_Config;
    // fpgaconvnet variables
    int n_partitions;

    public:

        // public variables
        partition_info_t* partition_info;

        // hw functions
        int   hw_init();
        void  hw_check_data(u32 size, u32* out_ptr,  u32* gold_ptr);
        int   hw_check_idle();
        int   hw_check_done();
        int   hw_check_ready();
        void  hw_start();
        void  hw_dump_indata();
        void  hw_dump_outdata();
        void  hw_make_file_name(char* dest, char* str, u8 i);
        u32   hw_array_max(u32* array, int length);

        // power monitoring functions
        // TODO

        // reconfig functions
        int   reconf_init();
        int   reconf_partition(int);

        // sd card functions
        int sd_init();
        FSIZE_t sd_file_size(char *filename);
        int sd_read(char *filename, void* dest_addr, u32 byte_len);
        int sd_read_config_num_partitions(char *filename);
        int sd_read_config(char *filename);
        int sd_write(char *filename, port_t* start_addr, u32 byte_len);
        int sd_compare(char *FileName, u8* DDRfile);

        //int SDvsDDR_files(char *FileName, u8* DDRfile);

        // timer functions
        int   timer_init();
        void  timer_start();
        float timer_stop();

        // fpgaconvnet functions
        int init(char *);
        int config_init(char *, bool, bool);
        int unassign_address_space(void);
        int free_featuremap_space(void);
        int config_print(void);
        int profiling_print(void);
        int find_partition_index(int);
        int run_partition(int);
        int run_network(void);

};

#endif

