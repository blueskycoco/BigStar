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
/*

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

*/

#include <windows.h>
#include <pm.h>
#include "pmplatform.h"
#include "bkl_hw.h"

#define FILE_DEVICE_BAK 0x2330
#define IOCTL_BACKLIGHT_SET			CTL_CODE( FILE_DEVICE_BAK, 1, METHOD_NEITHER,FILE_ANY_ACCESS)


//  Global structure
extern BLStruct g_BLInfo;
extern HANDLE   g_evtSignal[NUM_EVENTS];
HANDLE   g_lcdSignal;

DWORD
BKL_Init(DWORD dwContext)
{
	HANDLE  hThread;
	DWORD   dwThreadID;

	RETAILMSG(1, (TEXT("BKL_Init: dwContext = 0x%x\r\n"), dwContext));

	// Perform all one-time initialization of the backlight
	if (!BacklightInitialize())
	{
		RETAILMSG(1, (TEXT("BKL_Init:couldn't initialize backlight hardware \r\n")));
		return 0;
	}

	// Create the backlight service thread
	hThread = CreateThread(NULL, 0, BL_MonitorThread, NULL, 0, &dwThreadID);
	if (hThread == NULL)
	{
		RETAILMSG(1, (TEXT("BKL_Init: failed to create BL_MonitorThread\r\n")));
		return 0;
	}
	g_lcdSignal = CreateEvent(NULL, FALSE, FALSE, TEXT("LcdPowerUp"));

	return dwThreadID;
}



BOOL
BKL_Deinit(DWORD dwContext)
{
	BL_Deinit();
    return TRUE;
}



DWORD
BKL_Open(DWORD dwData, DWORD dwAccess, DWORD dwShareMode)
{
	return dwData;
}



BOOL
BKL_Close(DWORD Handle)
{
    return TRUE;
}



void
BKL_PowerDown(void)
{
    BL_On(FALSE);
}



void
BKL_PowerUp(void)
{
	//mf BL_PowerOn(TRUE);
}



DWORD
BKL_Read(DWORD Handle, LPVOID pBuffer, DWORD dwNumBytes)
{
	return 0;
}



DWORD
BKL_Write(DWORD Handle, LPCVOID pBuffer, DWORD dwNumBytes)
{
	return 0;
}



DWORD
BKL_Seek(DWORD Handle, long lDistance, DWORD dwMoveMethod)
{
	return (DWORD) -1;
}
void DelayOpenBacklight()
{
	//Sleep(1);
	BL_On(TRUE);
	BL_ChangeBrightness();
	Sleep(800);
	BL_Delay_On();
	RETAILMSG(1, (TEXT("<<<Delay Open Backlight>>>\r\n")));
}



BOOL
BKL_IOControl(
			  DWORD Handle,
			  DWORD dwCode,
			  PBYTE pBufIn,
			  DWORD dwLenIn,
			  PBYTE pBufOut,
			  DWORD dwLenOut,
			  PDWORD pdwActualOut
			  )
{
    BOOL RetVal = TRUE;
    DWORD dwErr = ERROR_SUCCESS;

    switch (dwCode)
	{
		RETAILMSG(1, (TEXT("BKL_IOControl: Handle = 0x%x, dwCode = 0x%x\r\n"), Handle, dwCode));
		RETAILMSG(1, (TEXT("               pBufIn = 0x%x, dwLenIn = 0x%x\r\n"), pBufIn, dwLenIn));
		RETAILMSG(1, (TEXT("               pOutBuf = 0x%x, dwLenOut = 0x%x\r\n"), pBufOut, dwLenOut));
		RETAILMSG(1, (TEXT("               pdwActualOut = 0x%x\r\n"), pdwActualOut));
        //
        // Power Management
        //
		case IOCTL_POWER_CAPABILITIES:
        {
            PPOWER_CAPABILITIES ppc;
			RETAILMSG(1, (TEXT("BKL: IOCTL_POWER_CAPABILITIES\r\n")));

			if ( !pdwActualOut || !pBufOut || (dwLenOut < sizeof(POWER_CAPABILITIES)) ) {
                RetVal = FALSE;
                dwErr = ERROR_INVALID_PARAMETER;
                break;
            }

            ppc = (PPOWER_CAPABILITIES)pBufOut;

            memset(ppc, 0, sizeof(POWER_CAPABILITIES));

            // support D0, D4
            ppc->DeviceDx = 0x11;

            // Report our power consumption in uAmps rather than mWatts.
            ppc->Flags = POWER_CAP_PREFIX_MICRO | POWER_CAP_UNIT_AMPS;

			// 25 m = 25000 uA
            // TODO: find out a more accurate value
			ppc->Power[D0] = 25000;

            *pdwActualOut = sizeof(POWER_CAPABILITIES);
        } break;

		case IOCTL_POWER_SET:
        {
            CEDEVICE_POWER_STATE NewDx;

            if ( !pdwActualOut || !pBufOut || (dwLenOut < sizeof(CEDEVICE_POWER_STATE)) ) {
                RetVal = FALSE;
                dwErr = ERROR_INVALID_PARAMETER;
                break;
            }

            NewDx = *(PCEDEVICE_POWER_STATE)pBufOut;

            if ( VALID_DX(NewDx) ) {
                switch ( NewDx ) {
                case D0:
                    if (g_BLInfo.m_bBatteryOnOff && g_BLInfo.m_bBatteryAuto)
                    {
                        //  Power changed, we need to notify the monitor thread to resync
                        //  the timer
                        //WaitForSingleObject(g_lcdSignal,INFINITE);
                        //BL_On(TRUE);
                        //BL_ChangeBrightness();
						//ResetEvent(g_lcdSignal);
						CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,(LPTHREAD_START_ROUTINE)DelayOpenBacklight, NULL, 0, NULL);
						RETAILMSG(1, (TEXT("<<<set backlight on>>>\r\n")));
                    }
                    break;

                default:
                    BL_On(FALSE);
                    break;
                }

                RETAILMSG(1, (TEXT("BKL: IOCTL_POWER_SET: D%u \r\n"), NewDx));

                *pdwActualOut = sizeof(CEDEVICE_POWER_STATE);
            } else {
                RetVal = FALSE;
                dwErr = ERROR_INVALID_PARAMETER;
            }

        } break;

        case IOCTL_POWER_GET:
            if ( !pdwActualOut || !pBufOut || (dwLenOut < sizeof(CEDEVICE_POWER_STATE)) ) {
                RetVal = FALSE;
                dwErr = ERROR_INVALID_PARAMETER;
                break;
            }

			CEDEVICE_POWER_STATE Dx;


			if (g_BLInfo.m_dwStatus == BL_ON){
				Dx = D0;
			}
			else {
				Dx = D4;
			}

			*(PCEDEVICE_POWER_STATE)pBufOut = Dx;
            RETAILMSG(1, (TEXT("BKL: IOCTL_POWER_GET: D%u \r\n"), Dx));

            *pdwActualOut = sizeof(CEDEVICE_POWER_STATE);
	        break;
        case IOCTL_BACKLIGHT_SET: 
            if ( !pdwActualOut || !pBufOut || (dwLenOut < sizeof(unsigned char)) ) {
                RetVal = FALSE;
                dwErr = ERROR_INVALID_PARAMETER;
                break;
            }
		Change_backlight_from_keypad((BOOL)*pBufOut);
               *pdwActualOut = sizeof(unsigned char);
            break;

		default:
            RetVal = FALSE;
            RETAILMSG(1, (TEXT(" Unsupported ioctl 0x%X\r\n"), dwCode));
            break;
	}

	return(RetVal);
}



BOOL
WINAPI
DllMain(
    HANDLE  hinstDll,
    DWORD   dwReason,
    LPVOID  lpReserved
    )
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			// RETAILMSG(1, (TEXT("BAK_DllMain: DLL_PROCESS_ATTACH\r\n")));
			break;

		case DLL_PROCESS_DETACH:
			// RETAILMSG(1, (TEXT("BAK_DllMain: DLL_PROCESS_DETACH\r\n")));
			break;
    }
    return TRUE;
}

