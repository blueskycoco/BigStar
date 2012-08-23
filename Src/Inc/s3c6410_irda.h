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
//  Header: s3c6410_irda.h
//
//  Defines the IrDA controller CPU register layout and definitions.
//
#ifndef __S3C6410_IRDA_H
#define __S3C6410_IRDA_H

#if __cplusplus
extern "C" 
{
#endif


//------------------------------------------------------------------------------
//  Type: S3C6410_IRDA_REG
//
//  Defines IRDA bus control register layout. This register bank is located by
//  the constant CPU_BASE_REG_XX_IRDA in the configuration file 
//  cpu_base_reg_cfg.h.
//
// IRDA Base Address = 0x7F007000

typedef struct 
{
    UINT32    IrDA_CNT;		//0x00
    UINT32    IrDA_MDR;
    UINT32    IrDA_CNF;
    UINT32    IrDA_IER;
    UINT32    IrDA_IIR;		// 0x10
    UINT32    IrDA_LSR;
    UINT32    IrDA_FCR;
    UINT32    IrDA_PLR;
    UINT32    IrDA_RBR;		// 0x20
    UINT32    IrDA_TXNO;
    UINT32    IrDA_RXNO;
    UINT32    IrDA_TXFLL;
	UINT32    IrDA_TXFLH;	// 0x30
    UINT32    IrDA_RXFLL;
	UINT32    IrDA_RXFLH;
    
} S3C6410_IICBUS_REG, *PS3C6410_IICBUS_REG;        


#if __cplusplus
    }
#endif

#endif 
