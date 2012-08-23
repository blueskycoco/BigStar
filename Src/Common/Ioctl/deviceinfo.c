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
//  File:  deviceinfo.c
//
//  This file implements the IOCTL_HAL_GET_DEVICE_INFO handler.
//
#include <windows.h>
#include <bldver.h>
#include <oal.h>

//
// SmartPhone and PocketPC specific platform version
//
#ifdef SMARTFON
// for AppInstall (SPI_GETPLATFORMVERSION)
#define TOTALPLATFORMS 1
const PLATFORMVERSION HALPlatformVersion[TOTALPLATFORMS] = {{4, 0}};
#elif defined WPC
// for AppInstall (SPI_GETPLATFORMVERSION)
#define TOTALPLATFORMS 2
const PLATFORMVERSION HALPlatformVersion[TOTALPLATFORMS] = {{4, 0}, {5, 0}};
#else
#define TOTALPLATFORMS 1
const PLATFORMVERSION HALPlatformVersion[TOTALPLATFORMS] = {{CE_MAJOR_VER, CE_MINOR_VER}};
#endif


//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlGetDeviceInfo
//
//  Implements the IOCTL_HAL_GET_DEVICE_INFO handler
//
BOOL OALIoCtlHalGetDeviceInfo( 
    UINT32 code, VOID *pInpBuffer, UINT32 inpSize, VOID *pOutBuffer, 
    UINT32 outSize, UINT32 *pOutSize
) {
    BOOL rc = FALSE;
    UINT32 length;


    //RETAILMSG(1, (L"+OALIoCtlHalGetDeviceInfo(...)\r\n"));

    // Validate inputs
    if (pInpBuffer == NULL || inpSize < sizeof(UINT32)) {
        NKSetLastError(ERROR_INVALID_PARAMETER);
        RETAILMSG(1, (
            L"ERROR: OALIoCtlHalGetDeviceInfo: Invalid parameter\r\n"
        ));
        goto cleanUp;
    }

    // Process according to input request
    switch (*(UINT32*)pInpBuffer) {
    case SPI_GETPLATFORMTYPE:
        // Validate output buffer size
#if defined WPC || defined SMARTFON
        // on PocketPC and Smartphone platforms the platform type 
        // string may contain NULLs and is terminated with a 
        // double NULL.
        {
            const WCHAR* pwszTemp;
            UINT32 nSubStrLen;
            
            length = sizeof(WCHAR);
            pwszTemp = g_oalIoCtlPlatformType;
            while( *pwszTemp != 0 )
            {
                nSubStrLen = NKwcslen(pwszTemp) + 1;
                length += nSubStrLen * sizeof(WCHAR);
                pwszTemp += nSubStrLen;
            }
        }
#else
        length = (NKwcslen(g_oalIoCtlPlatformType) + 1) * sizeof(WCHAR);
#endif
        // Return required size
        if (pOutSize != NULL) *pOutSize = length;
        // If there isn't output buffer or it is small return error
        if (pOutBuffer == NULL || outSize < length) {
            NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
            RETAILMSG(1, (
                L"WARN: OALIoCtlHalGetDeviceInfo: Insufficient buffer(SPI_GETPLATFORMTYPE)\r\n"
            ));
            break;
        }
        // Copy requested data to caller's buffer, set output length
        memcpy(pOutBuffer, g_oalIoCtlPlatformType, length);
        rc = TRUE;
        break;

    case SPI_GETOEMINFO:
        // Validate output buffer size
        length = (NKwcslen(g_oalIoCtlPlatformOEM) + 1) * sizeof(WCHAR);
        // Return required size
        if (pOutSize != NULL) *pOutSize = length;
        // If there isn't output buffer or it is small return error
        if (pOutBuffer == NULL || outSize < length) {
            NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
            RETAILMSG(1, (
                L"WARN: OALIoCtlHalGetDeviceInfo: Insufficient buffer(SPI_GETOEMINFO)\r\n"
            ));
            break;
        }
        // Copy requested data to caller's buffer, set output length
        memcpy(pOutBuffer, g_oalIoCtlPlatformOEM, length);
        rc = TRUE;
        break;

    case SPI_GETPLATFORMVERSION:
        length = sizeof HALPlatformVersion;
        // Return required size
        if (pOutSize != NULL) *pOutSize = length;
        // Return platform version
        if (outSize < length)
        {
            NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
            RETAILMSG(1, (
                L"WARN: OALIoCtlHalGetDeviceInfo: Insufficient buffer(SPI_GETPLATFORMVERSION)\r\n"
            ));
            memcpy(pOutBuffer, HALPlatformVersion, outSize);
	        if (pOutSize != NULL) *pOutSize = outSize;
            rc = TRUE;
        }
        else if (!pOutBuffer)
        {
            NKSetLastError(ERROR_INVALID_PARAMETER);
            RETAILMSG(1, (
                L"WARN: OALIoCtlHalGetDeviceInfo: Invalid parameter\r\n"
            ));
        }
        else
        {
            memcpy(pOutBuffer, HALPlatformVersion, length);
            rc = TRUE;
        }
        break;
    case SPI_GETPLATFORMMANUFACTURER:
        // Validate output buffer size
        length = (NKwcslen(g_oalIoCtlPlatformManufacturer) + 1) * sizeof(WCHAR);
        // Return required size
        if (pOutSize != NULL) *pOutSize = length;
        // If there isn't output buffer or it is small return error
        if (pOutBuffer == NULL || outSize < length) {
            NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
            RETAILMSG(1, (
                L"WARN: OALIoCtlHalGetDeviceInfo: Insufficient buffer(SPI_GETPLATFORMMANUFACTURER)\r\n"
            ));
            break;
        }
        // Copy requested data to caller's buffer, set output length
        memcpy(pOutBuffer, g_oalIoCtlPlatformManufacturer, length);
        rc = TRUE;    
    	break;
    case SPI_GETPLATFORMNAME:
        // Validate output buffer size
        length = (NKwcslen(g_oalIoCtlPlatformName) + 1) * sizeof(WCHAR);
        // Return required size
        if (pOutSize != NULL) *pOutSize = length;
        // If there isn't output buffer or it is small return error
        if (pOutBuffer == NULL || outSize < length) {
            NKSetLastError(ERROR_INSUFFICIENT_BUFFER);
            RETAILMSG(1, (
                L"WARN: OALIoCtlHalGetDeviceInfo: Insufficient buffer(SPI_GETPLATFORMNAME)\r\n"
            ));
            break;
        }
        // Copy requested data to caller's buffer, set output length
        memcpy(pOutBuffer, g_oalIoCtlPlatformName, length);
        rc = TRUE;
              
        break;
    default:
        RETAILMSG(1, (
            L"ERROR: OALIoCtlHalGetDeviceInfo: Invalid request\r\n"
        ));
        break;
    }

cleanUp:
    // Indicate status
    //RETAILMSG(1, (L"-OALIoCtlHalGetDeviceInfo(rc = %d)\r\n", rc));
    return rc;
}

//------------------------------------------------------------------------------
