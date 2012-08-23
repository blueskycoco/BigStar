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
//  Header:  s3c6410_syscon.h
//
//  Defines the System Controller register layout and associated types
//  and constants.
//
#ifndef __S3C6410_SYSCON_H
#define __S3C6410_SYSCON_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//  Type:  S3C6410_SYSCON_REG
//
//  System Controller registers. This register bank is located by the
//  constant S3C6410_BASE_REG_XX_SYSCON in the configuration file
//  s3c6410_reg_base_cfg.h.
//

typedef struct
{
	UINT32 APLL_LOCK;		// 0x00
	UINT32 MPLL_LOCK;		// 0x04
	UINT32 EPLL_LOCK;		// 0x08
	UINT32 APLL_CON;		// 0x0c

	UINT32 MPLL_CON;		// 0x10
	UINT32 EPLL_CON0;		// 0x14
	UINT32 EPLL_CON1;		// 0x18
	UINT32 CLK_SRC;			// 0x1c

	UINT32 CLK_DIV0;		// 0x20
	UINT32 CLK_DIV1;		// 0x24
	UINT32 CLK_DIV2;		// 0x28
	UINT32 CLK_OUT;			// 0x2c

	UINT32 HCLK_GATE;		// 0x30
	UINT32 PCLK_GATE;		// 0x34
	UINT32 SCLK_GATE;		// 0x38
	UINT32 MEM0_CLK_GATE;	// 0x3c

	UINT32 PAD0[48];		// 0x40~0xff

	UINT32 AHB_CON0;		// 0x100
	UINT32 AHB_CON1;		// 0x104
	UINT32 AHB_CON2;		// 0x108
	UINT32 CLK_SRC2;		// 0x10c

	UINT32 SDMA_SEL;		// 0x110
	#if   (CPU_REVISION == EVT0)
	UINT32 SW_RST;			// 0x114
	#elif (CPU_REVISION == EVT1)
	UINT32 PAD1;			// 0x114
    #endif
	UINT32 SYS_ID;			// 0x118
	UINT32 SYS_OTHERS;      // 0x11c

	UINT32 MEM_SYS_CFG;     // 0x120
	#if   (CPU_REVISION == EVT0)
	UINT32 QOS_OVERRIDE0;	// 0x124
	#elif (CPU_REVISION == EVT1)
	UINT32 PAD2;			// 0x124
    #endif
	UINT32 QOS_OVERRIDE1;	// 0x128
	UINT32 MEM_CFG_STAT;	// 0x12c

	UINT32 PAD3[436];		// 0x130~0x7ff

	UINT32 PAD4;			// 0x800
	UINT32 PWR_CFG; 		// 0x804
	UINT32 EINT_MASK;		// 0x808
	UINT32 PAD5;			// 0x80c

	UINT32 NORMAL_CFG;		// 0x810
	UINT32 STOP_CFG;		// 0x814
	UINT32 SLEEP_CFG;		// 0x818
	UINT32 STOP_MEM_CFG;    // 0x81c

	UINT32 OSC_FREQ;		// 0x820
	UINT32 OSC_STABLE;		// 0x824
	UINT32 PWR_STABLE;		// 0x828
	UINT32 PAD6;			// 0x82c

	UINT32 MTC_STABLE;		// 0x830
	#if   (CPU_REVISION == EVT0)
	UINT32 BUS_CACHEABLE_CON;   // 0x834
	UINT32 PAD7[2];			// 0x838~0x83f
	#elif (CPU_REVISION == EVT1)
    UINT32 PAD8;            // 0x834
    UINT32 MISC_CON;        // 0x838
    UINT32 PAD9;            // 0x83c
    #endif

	UINT32 PAD10[48];		// 0x840~0x8ff

	UINT32 OTHERS;			// 0x900
	UINT32 RST_STAT;		// 0x904
	UINT32 WAKEUP_STAT;		// 0x908
	UINT32 BLK_PWR_STAT;	// 0x90c

	UINT32 PAD11[60];		// 0x910~0x9ff

	UINT32 INFORM0;			// 0xa00
	UINT32 INFORM1;			// 0xa04
	UINT32 INFORM2;			// 0xa08
	UINT32 INFORM3;			// 0xa0c
} S3C6410_SYSCON_REG, *PS3C6410_SYSCON_REG;

//------------------------------------------------------------------------------

#if __cplusplus
    }
#endif

#endif	// __S3C6410_SYSCON_H
