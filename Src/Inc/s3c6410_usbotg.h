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
//  Header: s3c6410_usbotg.h
//
//  Defines the USBOTG controller CPU register layout and definitions.
//
#ifndef __S3C6410_USBOTG_H
#define __S3C6410_USBOTG_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//  Type: S3C6410_USBOTG_REG
//
//  Defines the USB device control register block. 
//


typedef struct {
	UINT32	OPHYPWR;
	UINT32	OPHYCLK;
	UINT32 	ORSTCON;
}OTG_PHY_REG, *PS_OTG_PHY_REG;



//------------------------------------------------------------------------------

#if __cplusplus
}
#endif

#endif

