// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xfpgaconvnet_ip.h"

extern XFpgaconvnet_ip_Config XFpgaconvnet_ip_ConfigTable[];

XFpgaconvnet_ip_Config *XFpgaconvnet_ip_LookupConfig(u16 DeviceId) {
	XFpgaconvnet_ip_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XFPGACONVNET_IP_NUM_INSTANCES; Index++) {
		if (XFpgaconvnet_ip_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XFpgaconvnet_ip_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XFpgaconvnet_ip_Initialize(XFpgaconvnet_ip *InstancePtr, u16 DeviceId) {
	XFpgaconvnet_ip_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XFpgaconvnet_ip_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XFpgaconvnet_ip_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

