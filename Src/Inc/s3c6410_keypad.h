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
//  Header: s3c6410_keypad.h
//
//  Defines the KEYPAD Interface controller CPU register layout and definitions.
//
#ifndef __S3C6410_KEYPAD_H
#define __S3C6410_KEYPAD_H

#if __cplusplus
extern "C" 
{
#endif


//------------------------------------------------------------------------------
//  Type: S3C6410_KEYPAD_REG
//
//  Defines KEYPAD interface control register layout. This register bank is located by
//  the constant CPU_BASE_REG_XX_KEYPAD in the configuration file 
//  cpu_base_reg_cfg.h.
//
// KEYPAD Base Address = 0x7E00A000

typedef struct 
{
    UINT32 KEYIFCON; 	// 0x00
    UINT32 KEYIFSTSCLR;	// 0x04
    UINT32 KEYIFCOL;	// 0x08
    UINT32 KEYIFROW;	// 0x0C
    UINT32 KEYIFFC;		// 0x10
    UINT32 KEYIFCLRINT; // 0x14
    
} S3C6410_KEYPAD_REG, *PS3C6410_KEYPAD_REG;        


#if __cplusplus
    }
#endif

#endif 
