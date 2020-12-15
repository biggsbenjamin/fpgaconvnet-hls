#include "fpgaconvnet.hpp"

int fpgaconvnet::reconf_init()
{
	int Status;

	// Initialise Device Configuration Interface
	reconf_config = XDcfg_LookupConfig(XPAR_XDCFG_0_DEVICE_ID);
	if (!reconf_config)
	{
		xil_printf("No config found for %d\r\n", XPAR_XDCFG_0_DEVICE_ID);
		return XST_FAILURE;
	}
	Status =  XDcfg_CfgInitialize(&reconf, reconf_config, reconf_config->BaseAddr);
	if (Status != XST_SUCCESS) {
		xil_printf("DevCfg initialisation  failed %d\r\n", Status);
		return XST_FAILURE;
	}

	XDcfg_SetLockRegister(&reconf, 0x757BDF0D);

	Status = XDcfg_SelfTest(&reconf);
	if (Status != XST_SUCCESS) {
		xil_printf("DevCfg self test  failed %d\r\n", Status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int fpgaconvnet::reconf_partition(int partition_index)
{

	/* Look at ug585 page 212 for details. Also, make sure to use .bin file (not .bit) */

	int status;

    Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
    Xil_Out32(SLCR_LVL_SHFTR_EN, 0xA);
    Xil_Out32(SLCR_LOCK, SLCR_LOCK_VAL);

	// Enable the pcap clock.
	status = Xil_In32(SLCR_PCAP_CLK_CTRL);
	if (!(status & SLCR_PCAP_CLK_CTRL_EN_MASK)) {
		Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
		Xil_Out32(SLCR_PCAP_CLK_CTRL,
				(status | SLCR_PCAP_CLK_CTRL_EN_MASK));
		Xil_Out32(SLCR_UNLOCK, SLCR_LOCK_VAL);
	}

	// 1 - Enable PCAP bridge
	XDcfg_SetControlRegister(&reconf, XDCFG_CTRL_PCAP_MODE_MASK);
	XDcfg_SetControlRegister(&reconf, XDCFG_CTRL_PCAP_PR_MASK  );

	// 2 - Clear interrupts
	XDcfg_IntrClear(&reconf, XDCFG_IXR_PCFG_DONE_MASK);
	XDcfg_IntrClear(&reconf, XDCFG_IXR_D_P_DONE_MASK);
	XDcfg_IntrClear(&reconf, XDCFG_IXR_DMA_DONE_MASK);
	//XDcfg_IntrClear(&reconf, 0xFFFFFFFF);

	// 3 - Initialise PL (optional)
	XDcfg_SetControlRegister(&reconf, XDCFG_CTRL_PCFG_PROG_B_MASK);
	XDcfg_ClearControlRegister(&reconf, XDCFG_CTRL_PCFG_PROG_B_MASK);
	while( (XDcfg_GetStatusRegister(&reconf) & XDCFG_STATUS_PCFG_INIT_MASK) );
	XDcfg_SetControlRegister(&reconf, XDCFG_CTRL_PCFG_PROG_B_MASK);
	XDcfg_IntrClear(&reconf, XDCFG_IXR_PCFG_DONE_MASK);

	// 4 - wait for ready signal
	while( !(XDcfg_GetStatusRegister(&reconf) & XDCFG_STATUS_PCFG_INIT_MASK) );

	// 5 - check there is room in command queue
	if ( (XDcfg_GetStatusRegister(&reconf) & XDCFG_STATUS_DMA_CMD_Q_F_MASK) ) {
		printf("command queue full\n\r");
		return XST_FAILURE;
	}

	// 6 - Disable PCAP loopback
	//printf("stage 6\n\r");
	//XDcfg_ClearMiscControlRegister(&reconf, XDCFG_MCTRL_PCAP_LPBK_MASK);

	// 7 - program PCAP 2x clock divider
	XDcfg_ClearControlRegister(&reconf, XDCFG_CTRL_PCAP_RATE_EN_MASK);

	// 8 - Queue DMA transfer
	status = XDcfg_Transfer(
			&reconf,
			(u8*) partition_info[partition_index].partition_ptr,
			(partition_info[partition_index].size_partition >> 2),
			(u8*) XDCFG_DMA_INVALID_ADDRESS,
			(partition_info[partition_index].size_partition >> 2),
			XDCFG_NON_SECURE_PCAP_WRITE
	);
	if (status != XST_SUCCESS)
	{
		xil_printf("Error : FPGA configuration failed!\n\r");
		return XST_FAILURE;
	}

	// 9 - wait for DMA transfer to finish
	while( !(XDcfg_IntrGetStatus(&reconf) & XDCFG_IXR_DMA_DONE_MASK) );

	// 10 - Check for errors

	if ( XDcfg_IntrGetStatus(&reconf) & XDCFG_IXR_ERROR_FLAGS_MASK )
	{
		xil_printf("Error : FPGA configuration failed!\n\r");
		return XST_FAILURE;
	}

	// 11 - wait for PL config to be done
	while( !(XDcfg_IntrGetStatus(&reconf) & XDCFG_IXR_PCFG_DONE_MASK) );

    Xil_Out32(SLCR_UNLOCK, SLCR_UNLOCK_VAL);
    Xil_Out32(SLCR_LVL_SHFTR_EN, 0xF);
    Xil_Out32(SLCR_LOCK, SLCR_LOCK_VAL);

    return XST_SUCCESS;
}


