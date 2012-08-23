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
//  File: ioctl.c
//
//  This file implements the OEM's IO Control (IOCTL) functions and declares
//  global variables used by the IOCTL component.
//
#include <windows.h>
#include <oal.h>
#include <bsp.h>
#include <Pkfuncs.h>
#include <Storemgr.h>
#include <pmplatform.h>

//------------------------------------------------------------------------------
//
//  Global: g_oalIoctlPlatformType/OEM
//
//  Platform Type/OEM
//
#ifdef SMARTFON
LPCWSTR g_oalIoCtlPlatformType = L"SmartPhone\0";
#elif defined WPC
LPCWSTR g_oalIoCtlPlatformType = L"PocketPC\0SSDK\0";
#else
LPCWSTR g_oalIoCtlPlatformType = L"SMDK6410 Board";
#endif
LPCWSTR g_oalIoCtlPlatformOEM  = L"Samsung Electronics";

LPCWSTR g_oalIoCtlPlatformName  = L"SMDK6410 Board";
LPCWSTR g_oalIoCtlPlatformManufacturer  = L"Samsung Electronics";
//------------------------------------------------------------------------------
//
//  Global: g_oalIoctlProcessorVendor/Name/Core
//
//  Processor information
//
LPCWSTR g_oalIoCtlProcessorVendor = L"Smasung Electronics";
LPCWSTR g_oalIoCtlProcessorName   = L"S3C6410";
LPCWSTR g_oalIoCtlProcessorCore   = L"ARM1176";

const UINT8 g_oalIoCtlVendorId[6] = { 0x00, 0x50, 0xBF, 0x43, 0x39, 0xBF };

//------------------------------------------------------------------------------
//
//  Global: g_oalIoctlInstructionSet
//
//  Processor instruction set identifier
//

UINT32 g_oalIoCtlInstructionSet = 0;
extern UINT32 g_oalIoCtlClockSpeed;


BOOL OALIoCtlHalGetHiveCleanFlag(UINT32 code, VOID *lpInBuf, UINT32 nInBufSize, VOID *lpOutBuf, UINT32 nOutBufSize, UINT32 *lpBytesReturned )
{
	volatile S3C6410_GPIO_REG *pOalPortRegs;

	BOOL HiveCleanFlag = TRUE;
	DWORD backup_GPNCON;

	pOalPortRegs = OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	backup_GPNCON = pOalPortRegs->GPNCON;
	pOalPortRegs->GPNCON = (backup_GPNCON & ~(0x3<<18)) | (0x0 << 18);
	if(INREG32(&pOalPortRegs->GPNDAT) & (1<<9))
	{
		HiveCleanFlag = FALSE;
	}

	pOalPortRegs->GPNCON = backup_GPNCON;

	RETAILMSG(1, (TEXT("<<<< OALIoCtlHalGetHiveCleanFlag .>>>>\n")));

	if (!lpInBuf || (nInBufSize != sizeof(DWORD)) || !lpOutBuf || (nOutBufSize != sizeof(BOOL)))
	{
		NKSetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	} else
	{
		DWORD *pdwFlags = (DWORD*)lpInBuf;
		BOOL  *pfClean  = (BOOL*)lpOutBuf;
		if (*pdwFlags == HIVECLEANFLAG_SYSTEM) {
			if(HiveCleanFlag){
			RETAILMSG(1, (TEXT("OEM: Cleaning system hive\r\n")));
			*pfClean = TRUE;
			}
			else{
			RETAILMSG(1, (TEXT("OEM: No Cleaning system hive\r\n")));
			*pfClean = FALSE;
			}
		} else if (*pdwFlags == HIVECLEANFLAG_USERS) {
			if(HiveCleanFlag){
			RETAILMSG(1, (TEXT("OEM: Cleaning user profiles\r\n")));
			*pfClean = TRUE;
			}
			else{
			RETAILMSG(1, (TEXT("OEM: NO Cleaning user profiles\r\n")));
			*pfClean = FALSE;
			}
		}
	}

	return TRUE;
}

BOOL OALIoCtlHalQueryFormatPartition(UINT32 code, VOID *lpInBuf, UINT32 nInBufSize, VOID *lpOutBuf, UINT32 nOutBufSize, UINT32 *lpBytesReturned )
{
	volatile S3C6410_GPIO_REG *pOalPortRegs;
	BOOL g_fFormatRootFS = TRUE;
	BOOL g_fFormatBootableFS = TRUE;
	STORAGECONTEXT* pContext = (STORAGECONTEXT*)lpInBuf;
	DWORD backup_GPNCON;

	pOalPortRegs = OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	backup_GPNCON = pOalPortRegs->GPNCON;
	pOalPortRegs->GPNCON = (backup_GPNCON & ~(0x3<<18)) | (0x0 << 18);
	if(INREG32(&pOalPortRegs->GPNDAT) & (1<<9))
	{
		g_fFormatRootFS = FALSE;
		g_fFormatBootableFS = FALSE;
	}

	pOalPortRegs->GPNCON = backup_GPNCON;

	RETAILMSG(1, (TEXT("<<<< OALIoCtlHalQueryFormatPartition .>>>>\n")));

	// validate parameters
	if (sizeof(STORAGECONTEXT) != nInBufSize || !lpInBuf ||
		sizeof(BOOL) != nOutBufSize || !lpOutBuf) {
		NKSetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	// by default, do not format any partitions
	*(BOOL*)lpOutBuf = FALSE;
//
// format the root file system? (MountAsRoot=dword:1)
//
	if (g_fFormatRootFS && (AFS_FLAG_ROOTFS & pContext->dwFlags)) {
		*(BOOL*)lpOutBuf = TRUE;
		RETAILMSG(1, (TEXT("format the root file system\n")));
	}
//
// format the bootable file system? (MountAsBootable=dword:1)
//
	if (g_fFormatBootableFS && (AFS_FLAG_BOOTABLE & pContext->dwFlags)) {
		*(BOOL*)lpOutBuf = TRUE;
		RETAILMSG(1, (TEXT("format the bootable file system\n")));
	}
	return TRUE;
}

BOOL OALIoCtlHalQueryDisplaySettings (UINT32 dwIoControlCode, VOID *lpInBuf, UINT32 nInBufSize, VOID *lpOutBuf, UINT32 nOutBufSize, UINT32* lpBytesReturned)
{
    DWORD dwErr = 0;

	RETAILMSG(0, (TEXT("In OALIoCtlHalQueryDisplaySettings^^^^^\r\n")));

    if (lpBytesReturned) {
        *lpBytesReturned = 0;
    }

    if (!lpOutBuf) {
        dwErr = ERROR_INVALID_PARAMETER;
    } else if (sizeof(DWORD)*3 > nOutBufSize) {
        dwErr = ERROR_INSUFFICIENT_BUFFER;
    } else {
        // Check the boot arg structure for the default display settings.
        __try {

            // TBD, jylee
            ((PDWORD)lpOutBuf)[0] = (DWORD)LCD_WIDTH;
            ((PDWORD)lpOutBuf)[1] = (DWORD)LCD_HEIGHT;
            ((PDWORD)lpOutBuf)[2] = (DWORD)LCD_BPP;

            if (lpBytesReturned) {
                *lpBytesReturned = sizeof (DWORD) * 3;
            }

        } __except (EXCEPTION_EXECUTE_HANDLER) {
            dwErr = ERROR_INVALID_PARAMETER;
        }
    }

    if (dwErr) {
        NKSetLastError (dwErr);
    }

    return !dwErr;
}


//------------------------------------------------------------------------------
//
//  Function: OALIoCtlHalReboot
//  This function make a Warm Boot of the target device
//
static BOOL OALIoCtlHalReboot(
		UINT32 code, VOID *pInpBuffer, UINT32 inpSize,
		VOID *pOutBuffer, UINT32 outSize, UINT32 *pOutSize)
{
#ifdef ULDR
	volatile BSP_ARGS *pBSPArgs = ((volatile BSP_ARGS *) IMAGE_SHARE_ARGS_UA_START);
#endif

	OALMSG(OAL_IOCTL&&OAL_FUNC, (TEXT("[OAL] ++OALIoCtlHalReboot()\r\n")));

#ifdef ULDR
    	pBSPArgs->fUldrReboot = TRUE;
#endif
	OEMSWReset();

	OALMSG(OAL_IOCTL&&OAL_FUNC, (TEXT("[OAL] --OALIoCtlHalReboot()\r\n")));

	return TRUE;
}

BOOL OALIoCtlHalProfile(UINT32 dwIoControlCode, VOID *lpInBuf, UINT32 nInBufSize, VOID *lpOutBuf, UINT32 nOutBufSize, UINT32* lpBytesReturned)
{
	RETAILMSG(0, (TEXT("<<<< OALIoCtlHalProfile .>>>>\n")));
	return(TRUE);
}

//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalSetSystemLevel
//
static BOOL OALIoCtlHalSetSystemLevel(
        UINT32 dwIoControlCode, VOID *lpInBuf, UINT32 nInBufSize,
        VOID *lpOutBuf, UINT32 nOutBufSize, UINT32* lpBytesReturned)
{
#ifdef DVS_EN
    DWORD dwErr = 0;
    DWORD dwTargetLevel;

    OALMSG(OAL_IOCTL&&OAL_FUNC, (TEXT("++OALIoCtlHalSetSystemLevel()\r\n")));

    if (lpBytesReturned)
    {
        *lpBytesReturned = 0;
    }

    if (lpInBuf == NULL)
    {
        dwErr = ERROR_INVALID_PARAMETER;
    }
    else if (sizeof(DWORD) > nInBufSize)
    {
        dwErr = ERROR_INSUFFICIENT_BUFFER;
    }
    else
    {
        __try
        {
            dwTargetLevel = *((PDWORD)lpInBuf);

            if (dwTargetLevel >= SYS_LEVEL_MAX)
            {
                dwErr = ERROR_INVALID_PARAMETER;
            }
            else
            {
                ChangeDVSLevel((SYSTEM_ACTIVE_LEVEL)dwTargetLevel);
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            dwErr = ERROR_INVALID_PARAMETER;
        }
    }

    if (dwErr)
    {
        OALMSG(OAL_ERROR, (TEXT("[OAL:ERR] OALIoCtlHalSetSystemLevel() Failed dwErr = 0x%08x\r\n"), dwErr));
        NKSetLastError(dwErr);
    }

    OALMSG(OAL_IOCTL&&OAL_FUNC, (TEXT("++OALIoCtlHalSetSystemLevel()\r\n")));

    return !dwErr;
#else
    OALMSG(TRUE, (TEXT("[OAL:ERR] OALIoCtlHalSetSystemLevel() : BSP_USEDVS is disabled !!!\r\n")));

    return TRUE;
#endif
}

//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalProfileDVS
//
//  For Profiling DVS transition and CPU idle/active rate
//  NOTE!!! Enable #define DVS_LEVEL_PROFILE in DVS.c
//
static BOOL OALIoCtlHalProfileDVS(
        UINT32 dwIoControlCode, VOID *lpInBuf, UINT32 nInBufSize,
        VOID *lpOutBuf, UINT32 nOutBufSize, UINT32* lpBytesReturned)
{
#ifdef DVS_EN
    DWORD dwErr = 0;
    DWORD dwProfileOnOff;

    OALMSG(OAL_IOCTL&&OAL_FUNC, (TEXT("++OALIoCtlHalProfileDVS()\r\n")));

    if (lpBytesReturned)
    {
        *lpBytesReturned = 0;
    }

    if (lpInBuf == NULL)
    {
        dwErr = ERROR_INVALID_PARAMETER;
    }
    else if (sizeof(DWORD) > nInBufSize)
    {
        dwErr = ERROR_INSUFFICIENT_BUFFER;
    }
    else
    {
        __try
        {
            dwProfileOnOff = *((PDWORD)lpInBuf);

            if (dwProfileOnOff == 0)
            {
                ProfileDVSOnOff(FALSE);
            }
            else
            {
                ProfileDVSOnOff(TRUE);
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            dwErr = ERROR_INVALID_PARAMETER;
        }
    }

    if (dwErr)
    {
        OALMSG(OAL_ERROR, (TEXT("[OAL:ERR] OALIoCtlHalProfileDVS() Failed dwErr = 0x%08x\r\n"), dwErr));
        NKSetLastError(dwErr);
    }

    OALMSG(OAL_IOCTL&&OAL_FUNC, (TEXT("++OALIoCtlHalProfileDVS()\r\n")));

    return !dwErr;
#else
    OALMSG(TRUE, (TEXT("[OAL:ERR] OALIoCtlHalProfileDVS() : BSP_USEDVS is disabled !!!\r\n")));

    return TRUE;
#endif
}


//------------------------------------------------------------------------------
//
//  Global: g_oalIoCtlTable[]
//
//  IOCTL handler table. This table includes the IOCTL code/handler pairs
//  defined in the IOCTL configuration file. This global array is exported
//  via oal_ioctl.h and is used by the OAL IOCTL component.
//
const OAL_IOCTL_HANDLER g_oalIoCtlTable[] = {
#include "ioctl_tab.h"
};

//------------------------------------------------------------------------------

