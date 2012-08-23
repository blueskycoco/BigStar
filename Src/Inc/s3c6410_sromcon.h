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
//  Header: s3c6410_sromcon.h
//
//  Defines the SROM Controller CPU register layout and definitions.
//
#ifndef __S3C6410_SROMCON_H
#define __S3C6410_SROMCON_H

#if __cplusplus
    extern "C"
    {
#endif

//------------------------------------------------------------------------------
//  Type: S3C6410_SROMCON_REG
//
//  SROM Controller control registers. This register bank is located by the constant
//  CPU_BASE_REG_XX_SROMCON in the configuration file s3c6410_base_reg_cfg.h.
//

typedef struct
{
	UINT32 SROM_BW;		// 0x00
	UINT32 SROM_BC0;		// 0x04
	UINT32 SROM_BC1;		// 0x08
	UINT32 SROM_BC2;		// 0x0c

	UINT32 SROM_BC3;		// 0x10
	UINT32 SROM_BC4;		// 0x14
	UINT32 SROM_BC5;		// 0x18
	UINT32 PAD;				// 0x1c
} S3C6410_SROMCON_REG, *PS3C6410_SROMCON_REG;

#if __cplusplus
    }
#endif

#endif	// __S3C6410_SROMCON_H