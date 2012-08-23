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
//  File:  hwentropy.c
//
//  This file implements the IOCTL_HAL_GET_HWENTROPY handler.
//
#include <windows.h>
#include <oal.h>
#include <s3c6410.h>


//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalGetHWEntropy
//
//  Implements the IOCTL_HAL_GET_HWENTROPY handler. This function creates a
//  64-bit value which is unique to the hardware.  This value never changes.
//
BOOL OALIoCtlHalGetHWEntropy( 
    UINT32 dwIoControlCode, VOID *lpInBuf, UINT32 nInBufSize, VOID *lpOutBuf, 
    UINT32 nOutBufSize, UINT32* lpBytesReturned)
{

    UINT32 DieID[2];
    BOOL rc = FALSE;

	//volatile S3C6410_GPIO_REG *v_IOPRegs = OALPAtoUA(S3C6410_BASE_REG_PA_GPIO);
	OALMSG(1, (L"+OALIoCtlHalGetHWEntropy\r\n"));
	
    // Check buffer size
    if (lpBytesReturned != NULL) 
    {
        *lpBytesReturned = sizeof(DieID);
    }

    if (lpOutBuf == NULL || nOutBufSize < sizeof(DieID))
    {
        NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
        OALMSG(OAL_WARN, (L"WARN: OALIoCtlHalGetHWEntropy: Buffer too small\r\n"));
    }
    else
    {
        // Get die ids
//        DieID[0] = v_IOPRegs->GSTATUS1;
//		DieID[1] = v_IOPRegs->GSTATUS1;
// TBC JYLEE 070227
        DieID[0] = 0x36410001;
        DieID[1] = 0x36410001;


        // Copy pattern to output buffer
        memcpy(lpOutBuf, DieID, sizeof(DieID));

        // We are done
        rc = TRUE;
    }

    // Indicate status
    OALMSG(OAL_IOCTL&&OAL_FUNC, (L"-OALIoCtlHalGetHWEntropy(rc = %d)\r\n", rc));
    return rc;
}

