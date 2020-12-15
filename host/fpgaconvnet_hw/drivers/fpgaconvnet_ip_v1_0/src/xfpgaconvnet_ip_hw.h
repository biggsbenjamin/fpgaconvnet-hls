// ==============================================================
// Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
// Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// ctrl
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - Channel 0 (ap_done)
//        bit 1  - Channel 1 (ap_ready)
//        others - reserved
// 0x10 : Data signal of mode
//        bit 31~0 - mode[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of weights_reloading_index
//        bit 31~0 - weights_reloading_index[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of fpgaconvnet_wr_0
//        bit 31~0 - fpgaconvnet_wr_0[31:0] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of fpgaconvnet_in_0
//        bit 31~0 - fpgaconvnet_in_0[31:0] (Read/Write)
// 0x2c : reserved
// 0x30 : Data signal of fpgaconvnet_out_0
//        bit 31~0 - fpgaconvnet_out_0[31:0] (Read/Write)
// 0x34 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XFPGACONVNET_IP_CTRL_ADDR_AP_CTRL                      0x00
#define XFPGACONVNET_IP_CTRL_ADDR_GIE                          0x04
#define XFPGACONVNET_IP_CTRL_ADDR_IER                          0x08
#define XFPGACONVNET_IP_CTRL_ADDR_ISR                          0x0c
#define XFPGACONVNET_IP_CTRL_ADDR_MODE_DATA                    0x10
#define XFPGACONVNET_IP_CTRL_BITS_MODE_DATA                    32
#define XFPGACONVNET_IP_CTRL_ADDR_WEIGHTS_RELOADING_INDEX_DATA 0x18
#define XFPGACONVNET_IP_CTRL_BITS_WEIGHTS_RELOADING_INDEX_DATA 32
#define XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_WR_0_DATA        0x20
#define XFPGACONVNET_IP_CTRL_BITS_FPGACONVNET_WR_0_DATA        32
#define XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_IN_0_DATA        0x28
#define XFPGACONVNET_IP_CTRL_BITS_FPGACONVNET_IN_0_DATA        32
#define XFPGACONVNET_IP_CTRL_ADDR_FPGACONVNET_OUT_0_DATA       0x30
#define XFPGACONVNET_IP_CTRL_BITS_FPGACONVNET_OUT_0_DATA       32

