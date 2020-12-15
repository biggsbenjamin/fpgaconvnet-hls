// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xfpgaconvnet_ip.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XFpgaconvnet_ip_CfgInitialize(XFpgaconvnet_ip *InstancePtr, XFpgaconvnet_ip_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Ctrl_BaseAddress = ConfigPtr->Ctrl_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XFpgaconvnet_ip_Start(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL) & 0x80;
    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XFpgaconvnet_ip_IsDone(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XFpgaconvnet_ip_IsIdle(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XFpgaconvnet_ip_IsReady(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XFpgaconvnet_ip_EnableAutoRestart(XFpgaconvnet_ip *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL, 0x80);
}

void XFpgaconvnet_ip_DisableAutoRestart(XFpgaconvnet_ip *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL, 0);
}

void XFpgaconvnet_ip_Set_mode(XFpgaconvnet_ip *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_MODE_DATA, Data);
}

u32 XFpgaconvnet_ip_Get_mode(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_MODE_DATA);
    return Data;
}

void XFpgaconvnet_ip_Set_weights_reloading_index(XFpgaconvnet_ip *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_WEIGHTS_RELOADING_INDEX_DATA, Data);
}

u32 XFpgaconvnet_ip_Get_weights_reloading_index(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_WEIGHTS_RELOADING_INDEX_DATA);
    return Data;
}

void XFpgaconvnet_ip_Set_fpgaconvnet_wr_0(XFpgaconvnet_ip *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_WR_0_DATA, Data);
}

u32 XFpgaconvnet_ip_Get_fpgaconvnet_wr_0(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_WR_0_DATA);
    return Data;
}

void XFpgaconvnet_ip_Set_fpgaconvnet_in_0(XFpgaconvnet_ip *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_IN_0_DATA, Data);
}

u32 XFpgaconvnet_ip_Get_fpgaconvnet_in_0(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_IN_0_DATA);
    return Data;
}

void XFpgaconvnet_ip_Set_fpgaconvnet_out_0(XFpgaconvnet_ip *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_OUT_0_DATA, Data);
}

u32 XFpgaconvnet_ip_Get_fpgaconvnet_out_0(XFpgaconvnet_ip *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_OUT_0_DATA);
    return Data;
}

void XFpgaconvnet_ip_InterruptGlobalEnable(XFpgaconvnet_ip *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_GIE, 1);
}

void XFpgaconvnet_ip_InterruptGlobalDisable(XFpgaconvnet_ip *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_GIE, 0);
}

void XFpgaconvnet_ip_InterruptEnable(XFpgaconvnet_ip *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_IER);
    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_IER, Register | Mask);
}

void XFpgaconvnet_ip_InterruptDisable(XFpgaconvnet_ip *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_IER);
    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_IER, Register & (~Mask));
}

void XFpgaconvnet_ip_InterruptClear(XFpgaconvnet_ip *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XFpgaconvnet_ip_WriteReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_ISR, Mask);
}

u32 XFpgaconvnet_ip_InterruptGetEnabled(XFpgaconvnet_ip *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_IER);
}

u32 XFpgaconvnet_ip_InterruptGetStatus(XFpgaconvnet_ip *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XFpgaconvnet_ip_ReadReg(InstancePtr->Ctrl_BaseAddress, XFPGACONVNET_IP_CTRL_ADDR_ISR);
}

