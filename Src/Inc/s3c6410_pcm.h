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
//  Header: s3c6410_pcm.h
//
//  Defines the IIS bus control register layout and associated constants 
//  and types.
//
//------------------------------------------------------------------------------

#ifndef __S3C6410_PCM_H
#define __S3C6410_PCM_H

#if __cplusplus
    extern "C" 
    {
#endif


//------------------------------------------------------------------------------
//  Type: name    
//
//  Defines IIS bus control register layout. This register bank is located by
//  the constant CPU_BASE_REG_XX_PCM in the configuration file 
//  cpu_base_reg_cfg.h.
//
// PCM Base Address = 0x46200000

typedef struct 
{
    UINT32 CONTROL;		// 0x00 - PCM Main control register
    UINT32 CLKCTL;		// 0x04 - PCM Clock and Shift control register
    UINT32 TXFIFO;		// 0x08 - PCM TxFIFO write port register
    UINT32 RXFIFO;		// 0x0c - PCM RxFIFO read port register
    UINT32 IRQ_CTL;		// 0x10 - PCM Interrupt Control register
    UINT32 IRQ_STAT;	// 0x14 - PCM Interrupt Status register
    UINT32 FIFO_STAT;	// 0x18 - PCM FIFO Status register
    UINT32 CLRINT;		// 0x20 - PCM Interrupt Clear register

} S3C6410_PCM_REG, *PS3C6410_PCM_REG;


#if __cplusplus
    }
#endif

#endif 
