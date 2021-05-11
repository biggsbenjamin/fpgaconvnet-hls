// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XFPGACONVNET_IP_H
#define XFPGACONVNET_IP_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xfpgaconvnet_ip_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
typedef struct {
    u16 DeviceId;
    u32 Ctrl_BaseAddress;
} XFpgaconvnet_ip_Config;
#endif

typedef struct {
    u32 Ctrl_BaseAddress;
    u32 IsReady;
} XFpgaconvnet_ip;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XFpgaconvnet_ip_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XFpgaconvnet_ip_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XFpgaconvnet_ip_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XFpgaconvnet_ip_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XFpgaconvnet_ip_Initialize(XFpgaconvnet_ip *InstancePtr, u16 DeviceId);
XFpgaconvnet_ip_Config* XFpgaconvnet_ip_LookupConfig(u16 DeviceId);
int XFpgaconvnet_ip_CfgInitialize(XFpgaconvnet_ip *InstancePtr, XFpgaconvnet_ip_Config *ConfigPtr);
#else
int XFpgaconvnet_ip_Initialize(XFpgaconvnet_ip *InstancePtr, const char* InstanceName);
int XFpgaconvnet_ip_Release(XFpgaconvnet_ip *InstancePtr);
#endif

void XFpgaconvnet_ip_Start(XFpgaconvnet_ip *InstancePtr);
u32 XFpgaconvnet_ip_IsDone(XFpgaconvnet_ip *InstancePtr);
u32 XFpgaconvnet_ip_IsIdle(XFpgaconvnet_ip *InstancePtr);
u32 XFpgaconvnet_ip_IsReady(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_EnableAutoRestart(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_DisableAutoRestart(XFpgaconvnet_ip *InstancePtr);

void XFpgaconvnet_ip_Set_mode(XFpgaconvnet_ip *InstancePtr, u32 Data);
u32 XFpgaconvnet_ip_Get_mode(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_Set_weights_reloading_index(XFpgaconvnet_ip *InstancePtr, u32 Data);
u32 XFpgaconvnet_ip_Get_weights_reloading_index(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_Set_fpgaconvnet_wr_0(XFpgaconvnet_ip *InstancePtr, u32 Data);
u32 XFpgaconvnet_ip_Get_fpgaconvnet_wr_0(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_Set_fpgaconvnet_in_0(XFpgaconvnet_ip *InstancePtr, u32 Data);
u32 XFpgaconvnet_ip_Get_fpgaconvnet_in_0(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_Set_fpgaconvnet_out_0(XFpgaconvnet_ip *InstancePtr, u32 Data);
u32 XFpgaconvnet_ip_Get_fpgaconvnet_out_0(XFpgaconvnet_ip *InstancePtr);

void XFpgaconvnet_ip_InterruptGlobalEnable(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_InterruptGlobalDisable(XFpgaconvnet_ip *InstancePtr);
void XFpgaconvnet_ip_InterruptEnable(XFpgaconvnet_ip *InstancePtr, u32 Mask);
void XFpgaconvnet_ip_InterruptDisable(XFpgaconvnet_ip *InstancePtr, u32 Mask);
void XFpgaconvnet_ip_InterruptClear(XFpgaconvnet_ip *InstancePtr, u32 Mask);
u32 XFpgaconvnet_ip_InterruptGetEnabled(XFpgaconvnet_ip *InstancePtr);
u32 XFpgaconvnet_ip_InterruptGetStatus(XFpgaconvnet_ip *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
