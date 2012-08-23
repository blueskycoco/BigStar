//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
//------------------------------------------------------------------------------
//
//  Header:  s3c6410_jpeg.h
//
//  Defines the JPEG Codec register layout and associated types 
//  and constants.
//
#ifndef __S3C6410_JPEG_H
#define __S3C6410_JPEG_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//  Type:  S3C6410_JPEG_REG    
//
//  JPEG Codec registers. This register bank is located by the 
//  constant S3C6410_BASE_REG_XX_JPEG in the configuration file 
//  s3c6410_reg_base_cfg.h.
//
// WDT Base Address = 0x78800000

typedef struct {
	// 0x00
    UINT32 JPGMOD;               // Process mode register
    UINT32 JPGSTS;               // Operation status register
    UINT32 JPGQHNO;              // Quantization table number register and Huffman table number register
    UINT32 JPGDRI;               // MCU, which inserts RST marker

	// 0x10
    UINT32 JPGY;                 // Vertical resolution
    UINT32 JPGX;                 // Horizontal resolution
    UINT32 JPGCNT;               // The amount of the compressed data in bytes
    UINT32 JPGIRQS;              // Interrupt setting register

	// 0x20
    UINT32 JPGIRQ;               // Interrupt status register
	UINT32 RSV_h020[247];        // 0x024 ~ 0x3FF = 988 bytes = 247 x DWORDs (4bytes)

	// 0x400
	UINT32 QTBL0[64];            // 8-bit Quantization of table number 0

	// 0x500
	UINT32 QTBL1[64];            // 8-bit Quantization of table number 1

	// 0x600
	UINT32 QTBL2[64];            // 8-bit Quantization of table number 2

	// 0x700
	UINT32 QTBL3[64];            // 8-bit Quantization of table number 3

	// 0x800
	UINT32 HDCTBL0[16];          // The number of code per code length

	// 0x840
	UINT32 HDCTBLG0[12];         // Group number of the order of occurrence
	UINT32 RSV_h870[4];

	// 0x880
	UINT32 HACTBL0[16];          // The number of code per code length

	// 0x8C0
	UINT32 HACTBLG0[162];        // Group number of the order of occurrence/Group number
	UINT32 RSV_hB48[46];         // 0xB48 ~ 0xBFF = 184 = 46 x DWORDs(4bytes)

	// 0xC00
	UINT32 HDCTBL1[16];          // The number of code per code length

	// 0xC40
	UINT32 HDCTBLG1[12];         // Group number of the order of occurrence
	UINT32 RSV_hC70[4];

	// 0xC80
	UINT32 HACTBL1[16];          // The number of code per code length

	// 0xCC0
	UINT32 HACTBLG1[162];        // Group number of the order of occurrence/Group number
	UINT32 RSV_hF48[46];         // 0xF48 ~ 0xFFF = 184 = 46 x DWORDs(4bytes)

	// 0x1000
	UINT32 IMG_ADDR0;            // Source of destination image address 1
	UINT32 IMG_ADDR1;            // Source of destination image address 2
	UINT32 HUFADDR0;             // Source of destination JPEG file address 1
	UINT32 HUFADDR1;             // Source of destination JPEG file address 2

	// 0x1010
	UINT32 SW_JSTART;            // Start JPEG process
	UINT32 SW_JRSTART;           // Restart JPEG process
	UINT32 S_RESET_CON;          // Soft Reset
	UINT32 JPG_CON;              // JPEG control register

	// 0x1020
	UINT32 COEF1;                // Coefficient values for RGB <-> YCbCr converter
	UINT32 COEF2;                // Coefficient values for RGB <-> YCbCr converter
	UINT32 COEF3;                // Coefficient values for RGB <-> YCbCr converter
	UINT32 MISC;                 // Miscellaneous

	// 0x1030
	UINT32 FRAM_INTV;            // Interval tiem value between frames (Clock period base)

} S3C6410_JPEG_REG, *PS3C6410_JPEG_REG;

//------------------------------------------------------------------------------

#if __cplusplus
    }
#endif

#endif 
