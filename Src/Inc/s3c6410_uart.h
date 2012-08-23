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
//  Header:  s3c6410_uart.h
//
//  Defines the UART controller register layout associated types and constants.
//
#ifndef __S3C6410_UART_H
#define __S3C6410_UART_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//  Type:  S3C6410_UART_REG
//
//  UART control registers. This register bank is located by the constant
//  S3C6410_BASE_REG_XX_UARTn in the configuration file
//  s3c6410_base_reg_cfg.h.
//

typedef struct
{
	UINT32 ULCON;		// 0x000
	UINT32 UCON;		// 0x004
	UINT32 UFCON;		// 0x008
	UINT32 UMCON;		// 0x00c

	UINT32 UTRSTAT;		// 0x010
	UINT32 UERSTAT;		// 0x014
	UINT32 UFSTAT;		// 0x018
	UINT32 UMSTAT;		// 0x01c

	UINT32 UTXH;		// 0x020
	UINT32 URXH;		// 0x024
	UINT32 UBRDIV;		// 0x028
	UINT32 UDIVSLOT;	// 0x02c

	UINT32 UINTP;		// 0x030
	UINT32 UINTSP;		// 0x034
	UINT32 UINTM;		// 0x038
	UINT32 PAD0;		// 0x03c
} S3C6410_UART_REG, *PS3C6410_UART_REG;

//------------------------------------------------------------------------------

#if __cplusplus
    }
#endif

#endif	// __S3C6410_UART_H
