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

#ifndef _S3C6410_POWER_CONTROL_H_
#define _S3C6410_POWER_CONTROL_H_

typedef enum
{
	BLKPWR_DOMAIN_IROM = 0,	// Internal ROM
	BLKPWR_DOMAIN_ETM,		// ETM
	BLKPWR_DOMAIN_S,			// SDMA, Security System
	BLKPWR_DOMAIN_F,			// Display Controller, Post Processor, Rotator
	BLKPWR_DOMAIN_P,			// FIMG-2D, TV Encoder, TV Scaler
	BLKPWR_DOMAIN_I,			// Camera interface, Jpeg
	BLKPWR_DOMAIN_V,			// MFC
	BLKPWR_DOMAIN_G,			// 3D
} BLKPWR_DOMAIN;

typedef enum
{
	RST_HW_RESET = 0,		// External reset by XnRESET
	RST_WARM_RESET,		// Warm reset by XnWRESET
	RST_WDT_RESET,		// Watch dog timer reset
	RST_SLEEP_WAKEUP,		// Reset by Wake up from Sleep
	RST_ESLEEP_WAKEUP,	// Reset by Wake up from eSleep
	RST_SW_RESET,			// Software reset
	RST_UNKNOWN			// Undefined status
} RESET_STATUS;


BOOL PwrCon_initialize_register_address(void *pSysConReg);
BOOL PwrCon_set_block_power_on(BLKPWR_DOMAIN eDomain);
BOOL PwrCon_set_block_power_off(BLKPWR_DOMAIN eDomain);
void PwrCon_set_USB_phy(BOOL bOn);
RESET_STATUS PwrCon_get_reset_status(void);

#endif	// _S3C6410_POWER_CONTROL_H_

