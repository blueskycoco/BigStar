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
//  Header: s3c6410_hsi.h
//
//  Defines the HSI Interface controller CPU register layout and definitions.
//
#ifndef __S3C6410_HSI_H
#define __S3C6410_HSI_H

#if __cplusplus
extern "C" 
{
#endif


//------------------------------------------------------------------------------
//  Type: S3C6410_HSITX_REG, S3C6410_HSIRX_REG
//
//  Defines HSI Tx/Rx interface control register layout. This register bank is located by
//  the constant CPU_BASE_REG_XX_HSITX, CPU_BASE_REG_XX_HSIRX in the configuration file 
//  cpu_base_reg_cfg.h.
//
// HSI Tx Base Address = 0x7E006000
// HSI Rx Base Address = 0x7E007000

typedef struct 
{
	UINT32 STATUS_REG	; // 0x00
	UINT32 CONFIG_REG	; // 0x04
	UINT32 reseved		; // 0x08
	UINT32 INTSRC_REG	; // 0x0C
	UINT32 INTMSK_REG	; // 0x10
	UINT32 SWRST_REG	; // 0x14
	UINT32 CHID_REG		; // 0x18
	UINT32 DATA_REG		; // 0x1C
    
} S3C6410_HSITX_REG, *PS3C6410_HSITX_REG;

typedef struct 
{
	UINT32 STATUS_REG	; // 0x00
	UINT32 CONFIG0_REG	; // 0x04
	UINT32 CONFIG1_REG	; // 0x08
	UINT32 INTSRC_REG	; // 0x0C
	UINT32 INTMSK_REG	; // 0x10
	UINT32 SWRST_REG	; // 0x14
	UINT32 CHID_REG		; // 0x18
	UINT32 DATA_REG		; // 0x1C
    
} S3C6410_HSIRX_REG, *PS3C6410_HSIRX_REG;

#if __cplusplus
    }
#endif

#endif 
