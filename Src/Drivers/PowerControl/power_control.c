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

/*++

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:

    Drv.c   Power Controller Driver

Abstract:

   Streams interface driver (MDD)

Functions:

Notes:

--*/

#include <bsp.h>
#include <pmplatform.h>
#include "s3c6410_power_control.h"

#define QUEUE_ENTRIES	(16)
#define MAX_NAMELEN	(64)
#define QUEUE_SIZE		(QUEUE_ENTRIES*(sizeof(POWER_BROADCAST)+MAX_NAMELEN))
#define PWC_UNIQUE_ID   (0x12345678)

#define POWER_MONITOR_THREAD_PRIODITY	(98)

static volatile S3C6410_SYSCON_REG *g_pSysConReg = NULL;
static HANDLE g_hThreadPowerMon = NULL;
static HANDLE g_hMsgQueue = NULL;
static BOOL g_aIPPowerStatus[PWR_IP_MAX] = {FALSE, };
static BOOL g_bExitThread = FALSE;
static CRITICAL_SECTION csPowerCon;

#define __MODULE__  "S3C6410 Power Control Driver"

#define ZONEID_ERROR                0
#define ZONEID_WARNING              1
#define ZONEID_PERF                 2
#define ZONEID_TEMP                 3
#define ZONEID_ENTER                4
#define ZONEID_INIT                 5
#define ZONEID_BLK_PWR_ON           6
#define ZONEID_BLK_PWR_OFF          7
#define ZONEID_DVS_PROFILE          8
#define ZONEID_DVS_CHANGE           9

#define PWC_ZONE_ERROR             DEBUGZONE(ZONEID_ERROR)
#define PWC_ZONE_WARNING           DEBUGZONE(ZONEID_WARNING)
#define PWC_ZONE_PERF              DEBUGZONE(ZONEID_PERF)
#define PWC_ZONE_TEMP              DEBUGZONE(ZONEID_TEMP)
#define PWC_ZONE_ENTER             DEBUGZONE(ZONEID_ENTER)
#define PWC_ZONE_INIT              DEBUGZONE(ZONEID_INIT)
#define PWC_ZONE_BLK_PWR_ON        DEBUGZONE(ZONEID_BLK_PWR_ON)
#define PWC_ZONE_BLK_PWR_OFF       DEBUGZONE(ZONEID_BLK_PWR_OFF)
#define PWC_ZONE_DVS_PROFILE       DEBUGZONE(ZONEID_DVS_PROFILE)
#define PWC_ZONE_DVS_CHANGE        DEBUGZONE(ZONEID_DVS_CHANGE)

#define ZONEMASK_ERROR         (1 << ZONEID_ERROR)
#define ZONEMASK_WARNING       (1 << ZONEID_WARNING)
#define ZONEMASK_PERF          (1 << ZONEID_PERF)
#define ZONEMASK_TEMP          (1 << ZONEID_TEMP)
#define ZONEMASK_ENTER         (1 << ZONEID_ENTER)
#define ZONEMASK_INIT          (1 << ZONEID_INIT)
#define ZONEMASK_BLK_PWR_ON    (1 << ZONEID_BLK_PWR_ON)
#define ZONEMASK_BLK_PWR_OFF   (1 << ZONEID_BLK_PWR_OFF)
#define ZONEMASK_DVS_PROFILE   (1 << ZONEID_DVS_PROFILE)
#define ZONEMASK_DVS_CHANGE    (1 << ZONEID_DVS_CHANGE)

#ifndef PWRCTL_DEBUGZONES
#define PWRCTL_DEBUGZONES          (ZONEMASK_ERROR | ZONEMASK_WARNING | ZONEMASK_BLK_PWR_ON | \
                                    ZONEMASK_BLK_PWR_OFF | ZONEMASK_DVS_CHANGE)
#endif
#ifndef PWRCTL_RETAILZONES
#define PWRCTL_RETAILZONES          (ZONEMASK_ERROR | ZONEMASK_WARNING | ZONEMASK_BLK_PWR_ON | \
                                    ZONEMASK_BLK_PWR_OFF | ZONEMASK_DVS_CHANGE)
#endif
#ifdef  DEBUG
#define PWRCTL_ZONES PWRCTL_DEBUGZONES
#else
#define PWRCTL_ZONES PWRCTL_RETAILZONES
#endif

DBGPARAM dpCurSettings =                                \
{                                                       \
    TEXT(__MODULE__),                                      \
    {                                                   \
        TEXT("Errors"),                 /* 0  */        \
        TEXT("Warnings"),               /* 1  */        \
        TEXT("Performance"),            /* 2  */        \
        TEXT("Temporary tests"),        /* 3  */        \
        TEXT("Enter,Exit"),             /* 4  */        \
        TEXT("Initialize"),             /* 5  */        \
        TEXT("IOCTL : Block Power On"), /* 6  */        \
        TEXT("IOCTL : Block Power Off"),/* 7  */        \
        TEXT("IOCTL : DVS Profile"),    /* 8  */        \
        TEXT("IOCTL : DVS Change"),     /* 9  */        \
    },                                                  \
    (PWRCTL_ZONES)                               \
};

INT WINAPI PowerMonitorThread(void)
{
	HANDLE hPowerNotification = NULL;
	MSGQUEUEOPTIONS msgOptions;
	PPOWER_BROADCAST pB;
	RESET_STATUS eRstStat;
	UCHAR msgBuf[QUEUE_SIZE];
    DWORD iBytesInQueue = 0;
	DWORD dwFlags;

    RETAILMSG(PWC_ZONE_ENTER, (_T("[PWRCON:INF] ++%s\r\n"), _T(__FUNCTION__)));

	eRstStat = PwrCon_get_reset_status();

	//-------------------------------------------------
	// Set Power Monitor Thread Priority
	//-------------------------------------------------
	CeSetThreadPriority(g_hThreadPowerMon, POWER_MONITOR_THREAD_PRIODITY);

	//-------------------------------------------------
	// Create a message queue for Power Manager notifications.
	//-------------------------------------------------
	memset((void *)&msgOptions, 0x0, sizeof(msgOptions));
	msgOptions.dwSize = sizeof(MSGQUEUEOPTIONS);
	msgOptions.dwFlags = 0;
	msgOptions.dwMaxMessages = QUEUE_ENTRIES;
	msgOptions.cbMaxMessage = sizeof(POWER_BROADCAST) + MAX_NAMELEN;
	msgOptions.bReadAccess = TRUE;

	g_hMsgQueue = CreateMsgQueue(NULL, &msgOptions);
	if (g_hMsgQueue == NULL )
	{
        RETAILMSG(PWC_ZONE_ERROR,(_T("[PWRCON:ERR] %s->CreateMsgQueue() Failed : Err %d\r\n"), _T(__FUNCTION__), GetLastError()));
        goto Thread_CleanUp;
	}

	// Request Power notifications
	hPowerNotification = RequestPowerNotifications(g_hMsgQueue, POWER_NOTIFY_ALL);
	if (!hPowerNotification)
	{
        RETAILMSG(PWC_ZONE_ERROR,(_T("[PWRCON:ERR] %s->RequestPowerNotifications() Failed : Err %d\r\n"), _T(__FUNCTION__), GetLastError()));
        goto Thread_CleanUp;
	}

	while(!g_bExitThread)
	{
		memset(msgBuf, 0x0, QUEUE_SIZE);
		pB = (PPOWER_BROADCAST)msgBuf;

		//PWRCON_INF((_T("[PWR:INF] PowerMonitorThread() : Wait for PM Notification\r\n")));

		// Read message from queue.
		if (!ReadMsgQueue(g_hMsgQueue, msgBuf, QUEUE_SIZE, &iBytesInQueue, INFINITE, &dwFlags))
		{
			if (g_bExitThread)
			{
				break;
			}

            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s->ReadMsgQueue() Failed : Err %d\r\n"), _T(__FUNCTION__), GetLastError()));
		}
		else if (iBytesInQueue < sizeof(POWER_BROADCAST))
		{
            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s->Receive Insufficient Message (Size is %d, Expected %d)\r\n"), _T(__FUNCTION__), iBytesInQueue, sizeof(POWER_BROADCAST)));
		}
		else
		{
			switch (pB->Message)
			{
			//-----------------------
			// Notified State Transition
			//-----------------------
			case PBT_TRANSITION:

                RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Notified [PBT_TRANSITION : %s (0x%08x)]\r\n"), pB->SystemPowerState, pB->Flags));
				break;

			//-----------------------
			// Notified Resume State
			//-----------------------
			case PBT_RESUME:

                RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Notified [PBT_RESUME]\r\n")));
				{
					DWORD dwWakeSrc = SYSWAKE_UNKNOWN;
					DWORD dwBytesRet = 0;

					if (KernelIoControl(IOCTL_HAL_GET_WAKE_SOURCE, NULL, 0, &dwWakeSrc, sizeof(dwWakeSrc), &dwBytesRet)
						&& (dwBytesRet == sizeof(dwWakeSrc)))
					{
						switch(dwWakeSrc)
						{
						case SYSWAKE_POWER_BUTTON:	// Power Button
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by Power Button\r\n")));
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						case OEMWAKE_RTC_ALARM:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by RTC Alarm\r\n")));
							//PWRCON_INF((_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							// Do not change Power State to POWER_STATE_ON
							break;
						case OEMWAKE_RTC_TICK:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by RTC Tick\r\n")));
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						case OEMWAKE_KEYPAD:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by Keypad\r\n")));
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						case OEMWAKE_MSM:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by MSM I/F\r\n")));
							//PWRCON_INF((_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							//SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						case OEMWAKE_BATTERY_FAULT:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by Battery Fault\r\n")));
							//PWRCON_INF((_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							//SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						case OEMWAKE_WARM_RESET:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by Warm Reset\r\n")));
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						case OEMWAKE_HSI:
                            RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Wake Up by HSI I/F\r\n")));
							//PWRCON_INF((_T("[PWRCON:INF] SetSystemPowerState(POWER_STATE_ON)\r\n")));
							//SetSystemPowerState(NULL, POWER_STATE_ON, POWER_FORCE);
							break;
						default:
                            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] WakeUp Source = 0x%08x\r\n"), dwWakeSrc));
							break;
						}

					}
					else
					{
						NKDbgPrintfW(L"PWRCON: Error getting wake source\r\n");
                        RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] IOCTL_HAL_GET_WAKE_SOURCE Failed\r\n")));
                        RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] System is Still in [PBT_RESUME] State\r\n")));
					}
				}
				break;

			//-----------------------------------
			// Notified Power Supply changed (AC/DC)
			//-----------------------------------
			case PBT_POWERSTATUSCHANGE:
                RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Notified [PBT_POWERSTATUSCHANGE]\r\n")));
				break;

			//---------------------------------
			// Notified Power Information changed
			//---------------------------------
			case PBT_POWERINFOCHANGE:
			{
				PPOWER_BROADCAST_POWER_INFO ppbpi;

				ppbpi = (PPOWER_BROADCAST_POWER_INFO)pB->SystemPowerState;

                RETAILMSG(PWC_ZONE_TEMP, (_T("[PWRCON:INF] Notified [PBT_POWERINFOCHANGE]\r\n")));
				//PWRCON_INF((_T("[PWRCON:INF] 	ACLine Status : %d\r\n"), ppbpi->bACLineStatus));
				//PWRCON_INF((_T("[PWRCON:INF] 	Battery Flag  : %d\r\n"), ppbpi->bBatteryFlag));
				//PWRCON_INF((_T("[PWRCON:INF] 	Backup Flag   : %d\r\n"), ppbpi->bBackupBatteryFlag));
				//PWRCON_INF((_T("[PWRCON:INF] 	Level         : %d\r\n"), ppbpi->dwNumLevels));
				break;
			}

			default:
                RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] Notified Unknown Message [0x%08x]\r\n"), pB->Message));
				break;
			}
		}
	}

Thread_CleanUp:
	if (hPowerNotification)
	{
		StopPowerNotifications(hPowerNotification);
		hPowerNotification = NULL;
	}

	if (g_hMsgQueue)
	{
		CloseMsgQueue(g_hMsgQueue);
		g_hMsgQueue = NULL;
	}

    RETAILMSG(PWC_ZONE_ENTER, (_T("[PWRCON:INF] --%s\r\n"), _T(__FUNCTION__)));

	return 0;
}


static BOOL
PWC_AllocResources(void)
{
    PHYSICAL_ADDRESS    ioPhysicalBase = {0,0};
    DEBUGMSG(PWC_ZONE_ENTER, (_T("[PWRCON] ++%s\r\n"), _T(__FUNCTION__)));

	//--------------------
	// System Controller SFR
	//--------------------
    ioPhysicalBase.LowPart = S3C6410_BASE_REG_PA_SYSCON;
    g_pSysConReg = (S3C6410_SYSCON_REG *)MmMapIoSpace(ioPhysicalBase, sizeof(S3C6410_SYSCON_REG), FALSE);
	if (g_pSysConReg == NULL)
	{
        RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s->g_pSysConReg MmMapIoSpace() Failed \n\r"), _T(__FUNCTION__)));
		return FALSE;
	}

	//--------------------
	// Critical Section
	//--------------------
	InitializeCriticalSection(&csPowerCon);

    DEBUGMSG(PWC_ZONE_ENTER, (_T("[PWRCON] --%s\r\n"), _T(__FUNCTION__)));

	return TRUE;
}

static void
PWC_ReleaseResources(void)
{
    DEBUGMSG(PWC_ZONE_ENTER, (_T("[PWRCON] ++%s\r\n"), _T(__FUNCTION__)));

	if (g_pSysConReg != NULL)
	{
        MmUnmapIoSpace((PVOID)g_pSysConReg, sizeof(S3C6410_SYSCON_REG));
		g_pSysConReg = NULL;
	}

	DeleteCriticalSection(&csPowerCon);

    DEBUGMSG(PWC_ZONE_ENTER, (_T("[PWRCON] --%s\n"), _T(__FUNCTION__)));
}

BOOL
DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
        DEBUGREGISTER(hinstDll);
        DisableThreadLibraryCalls ((HMODULE)hinstDll);
        DEBUGMSG(PWC_ZONE_INIT,(_T("[PWRCON] %s : Process Attach\r\n"), _T(__FUNCTION__)));
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
        DEBUGMSG(PWC_ZONE_INIT,(_T("[PWRCON] %s : Process Detach\r\n"), _T(__FUNCTION__)));
	}

	return TRUE;
}

BOOL PWC_Deinit(DWORD pContext)
{
    DEBUGMSG(PWC_ZONE_INIT,(_T("[PWRCON] ++%s(0x%08x)\r\n"), _T(__FUNCTION__), pContext));

	g_bExitThread = TRUE;

	if (g_hThreadPowerMon)		// Make Sure if thread is exist
	{
		// Signal Thread to Finish
		if (g_hMsgQueue)
		{
			CloseMsgQueue(g_hMsgQueue);	// Closing the MsgQueue will force ReadMsgQueue to return
			g_hMsgQueue = NULL;
		}

		// Wait for Thread to Finish
		WaitForSingleObject(g_hThreadPowerMon, INFINITE);
		CloseHandle(g_hThreadPowerMon);
		g_hThreadPowerMon = NULL;
	}

	PWC_ReleaseResources();

    DEBUGMSG(PWC_ZONE_INIT,(_T("[PWRCON] --%s\r\n"), _T(__FUNCTION__) ));

	return TRUE;
}

DWORD
PWC_Init(DWORD dwContext)
{
    DEBUGMSG(PWC_ZONE_INIT,(_T("[PWRCON:INF] ++%s(0x%08x)\r\n"), _T(__FUNCTION__), dwContext));

	if (PWC_AllocResources() == FALSE)
	{
        RETAILMSG(PWC_ZONE_ERROR,(_T("[PWRCON:ERR] %s->AllocResources() Failed \n\r"), _T(__FUNCTION__)));
		goto CleanUp;
	}

	PwrCon_initialize_register_address((void *)g_pSysConReg);

	// Create power Monitor Thread
	g_hThreadPowerMon = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) PowerMonitorThread, NULL, 0, NULL);
	if (g_hThreadPowerMon == NULL )
	{
        RETAILMSG(PWC_ZONE_ERROR,(_T("[PWRCON:ERR] %s->CreateThread() Power Monitor Failed \n\r"), _T(__FUNCTION__)));
		goto CleanUp;
	}

    DEBUGMSG(PWC_ZONE_INIT,(_T("[PWRCON:INF] --%s()\r\n"), _T(__FUNCTION__)));

	return TRUE;

CleanUp:

    RETAILMSG(PWC_ZONE_ERROR,(_T("[PWRCON:ERR] --%s : Failed\r\n"), _T(__FUNCTION__)));

	PWC_Deinit(0);

	return FALSE;
}

DWORD
PWC_Open(DWORD pContext, DWORD dwAccess, DWORD dwShareMode)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x, 0x%08x, 0x%08x)\r\n"), _T(__FUNCTION__), pContext, dwAccess, dwShareMode));
    return PWC_UNIQUE_ID;
}

BOOL
PWC_Close(DWORD pContext)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x)\r\n"), _T(__FUNCTION__), pContext));
    return TRUE;
}

DWORD
PWC_Read (DWORD pContext,  LPVOID pBuf, DWORD Len)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x, 0x%08x, 0x%08x)\r\n"), _T(__FUNCTION__), pContext, pBuf, Len));
    return (0);    // End of File
}

DWORD
PWC_Write(DWORD pContext, LPCVOID pBuf, DWORD Len)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x, 0x%08x, 0x%08x)\r\n"), _T(__FUNCTION__), pContext, pBuf, Len));
    return (0);    // Number of Byte
}

DWORD
PWC_Seek (DWORD pContext, long pos, DWORD type)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x, 0x%08x, 0x%08x)\r\n"), _T(__FUNCTION__), pContext, pos, type));
    return (DWORD)-1;    // Failure
}

BOOL
PWC_PowerUp(DWORD pContext)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x)\r\n"), _T(__FUNCTION__), pContext));

    return TRUE;
}

BOOL
PWC_PowerDown(DWORD pContext)
{
    DEBUGMSG(PWC_ZONE_ENTER,(_T("[PWRCON] %s(0x%08x)\r\n"), _T(__FUNCTION__), pContext));

    return TRUE;
}

BOOL
PWC_IOControl(DWORD pContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn, PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	BOOL bRet = TRUE;
	DWORD dwIndex;

    if (pContext != PWC_UNIQUE_ID)
    {
        RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s(0x%08x, 0x%08x) : Invalid Handle\r\n"), _T(__FUNCTION__), pContext, dwCode));
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if ( !( (dwCode == IOCTL_PWRCON_SET_POWER_ON)
        || (dwCode == IOCTL_PWRCON_SET_POWER_OFF)
#ifdef DVS_EN        
        || (dwCode == IOCTL_PWRCON_SET_SYSTEM_LEVEL)
        || (dwCode == IOCTL_PWRCON_QUERY_SYSTEM_LEVEL)        
        || (dwCode == IOCTL_PWRCON_PROFILE_DVS)
#endif
        ))
        
    {
        RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s : Unknown IOCTL [0x%08x]\r\n"), _T(__FUNCTION__), dwCode));
        SetLastError (ERROR_INVALID_ACCESS);
        return FALSE;
    }

	switch(dwCode)
	{

	case IOCTL_PWRCON_SET_POWER_ON:
		if ((dwLenIn < sizeof(DWORD)) || (NULL == pBufIn))
		{
            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s(IOCTL_PWRCON_SET_POWER_ON) : Invalid Parameter\r\n"),_T(__FUNCTION__)));
			SetLastError (ERROR_INVALID_PARAMETER);
			bRet = FALSE;
			break;
		}

		dwIndex = (DWORD)(*pBufIn);

		EnterCriticalSection(&csPowerCon);

		switch(dwIndex)
		{
		case PWR_IP_IROM:
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : IROM\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_IROM);
			break;
		case PWR_IP_ETM:
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : ETM\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_ETM);
			break;
		case PWR_IP_SDMA0:		// Domain S
		case PWR_IP_SDMA1:
		case PWR_IP_SECURITY:
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : S\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_S);
			break;
		case PWR_IP_ROTATOR:	// Domain F
		case PWR_IP_POST:
		case PWR_IP_DISPCON:
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : F\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_F);
			break;
		case PWR_IP_2D:			// Domain P
		case PWR_IP_TVENC:
		case PWR_IP_TVSC:
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : P\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_P);
			break;
		case PWR_IP_JPEG:		// Domain I
		case PWR_IP_CAMIF:
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : I\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_I);
			break;
		case PWR_IP_3D:         // Domain G
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : I\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_G);
			break;
		case PWR_IP_MFC:		// Domain V
			g_aIPPowerStatus[dwIndex] = TRUE;
            RETAILMSG(PWC_ZONE_BLK_PWR_ON,(_T("[PWRCON:INF] %s(SET_POWER_ON, %d) : V\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_on(BLKPWR_DOMAIN_V);
			break;
		default:
            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s(IOCTL_PWRCON_SET_POWER_ON, %d) : Invalid Parameter\r\n"), _T(__FUNCTION__), dwIndex));
			SetLastError (ERROR_INVALID_PARAMETER);
			bRet = FALSE;
		}

		LeaveCriticalSection(&csPowerCon);

		break;

	case IOCTL_PWRCON_SET_POWER_OFF:
		if ((dwLenIn < sizeof(DWORD)) || (NULL == pBufIn))
		{
            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s(IOCTL_PWRCON_SET_POWER_OFF) : Invalid Parameter\r\n"), _T(__FUNCTION__)));
			SetLastError (ERROR_INVALID_PARAMETER);
			bRet = FALSE;
			break;
		}

		dwIndex = (DWORD)(*pBufIn);

		EnterCriticalSection(&csPowerCon);

		switch(dwIndex)
		{
		case PWR_IP_IROM:
			g_aIPPowerStatus[dwIndex] = FALSE;
            RETAILMSG(PWC_ZONE_BLK_PWR_OFF,(_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : IROM\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_off(BLKPWR_DOMAIN_IROM);
			break;
		case PWR_IP_ETM:
			g_aIPPowerStatus[dwIndex] = FALSE;
            RETAILMSG(PWC_ZONE_BLK_PWR_OFF,(_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : ETM\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_off(BLKPWR_DOMAIN_ETM);
			break;
		case PWR_IP_SDMA0:		// Domain S
		case PWR_IP_SDMA1:
		case PWR_IP_SECURITY:
			g_aIPPowerStatus[dwIndex] = FALSE;
			if ( (g_aIPPowerStatus[PWR_IP_SDMA0] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_SDMA1] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_SECURITY] == FALSE) )
			{
                RETAILMSG(PWC_ZONE_BLK_PWR_OFF, (_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : S\r\n"), _T(__FUNCTION__), dwIndex));
				PwrCon_set_block_power_off(BLKPWR_DOMAIN_S);
			}
			break;
		case PWR_IP_ROTATOR:	// Domain F
		case PWR_IP_POST:
		case PWR_IP_DISPCON:
			g_aIPPowerStatus[dwIndex] = FALSE;
			if ( (g_aIPPowerStatus[PWR_IP_ROTATOR] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_POST] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_DISPCON] == FALSE) )
			{
                RETAILMSG(PWC_ZONE_BLK_PWR_OFF, (_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : F\r\n"), _T(__FUNCTION__), dwIndex));
				PwrCon_set_block_power_off(BLKPWR_DOMAIN_F);
			}
			break;
		case PWR_IP_2D:			// Domain P
		case PWR_IP_TVENC:
		case PWR_IP_TVSC:
			g_aIPPowerStatus[dwIndex] = FALSE;
			if ( (g_aIPPowerStatus[PWR_IP_2D] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_TVENC] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_TVSC] == FALSE) )
			{
                RETAILMSG(PWC_ZONE_BLK_PWR_OFF, (_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : P\r\n"), _T(__FUNCTION__), dwIndex));
				PwrCon_set_block_power_off(BLKPWR_DOMAIN_P);
			}
			break;
		case PWR_IP_JPEG:		// Domain I
		case PWR_IP_CAMIF:
			g_aIPPowerStatus[dwIndex] = FALSE;
			if ( (g_aIPPowerStatus[PWR_IP_JPEG] == FALSE)
				&& (g_aIPPowerStatus[PWR_IP_CAMIF] == FALSE) )
			{
                RETAILMSG(PWC_ZONE_BLK_PWR_OFF, (_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : I\r\n"), _T(__FUNCTION__), dwIndex));
				PwrCon_set_block_power_off(BLKPWR_DOMAIN_I);
			}
			break;
		case PWR_IP_3D:		// Domain G
			g_aIPPowerStatus[dwIndex] = FALSE;
            RETAILMSG(PWC_ZONE_BLK_PWR_OFF, (_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : V\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_off(BLKPWR_DOMAIN_G);
			break;
		case PWR_IP_MFC:		// Domain V
			g_aIPPowerStatus[dwIndex] = FALSE;
            RETAILMSG(PWC_ZONE_BLK_PWR_OFF, (_T("[PWRCON:INF] %s(SET_POWER_OFF, %d) : V\r\n"), _T(__FUNCTION__), dwIndex));
			PwrCon_set_block_power_off(BLKPWR_DOMAIN_V);
			break;
		default:
            RETAILMSG(PWC_ZONE_ERROR, (_T("[PWRCON:ERR] %s(SET_POWER_OFF, %d) : Invalid Parameter\r\n"), _T(__FUNCTION__), dwIndex));
			SetLastError (ERROR_INVALID_PARAMETER);
			bRet = FALSE;
		}

		LeaveCriticalSection(&csPowerCon);

		break;
#ifdef DVS_EN
    case IOCTL_PWRCON_SET_SYSTEM_LEVEL:
        RETAILMSG(PWC_ZONE_DVS_CHANGE, (_T("Set System Level\r\n")));
        return KernelIoControl(IOCTL_HAL_SET_SYSTEM_LEVEL, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);
        break;
    case IOCTL_PWRCON_QUERY_SYSTEM_LEVEL:
//        KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &g_dwSysIntrPowerBtn, sizeof(DWORD), NULL, 0, NULL);        
        break;
#define DVS_LEVEL_PROFILE
#ifdef  DVS_LEVEL_PROFILE        
    case IOCTL_PWRCON_PROFILE_DVS:
        RETAILMSG(PWC_ZONE_DVS_PROFILE, (_T("Call HAL Profiler DVS Io Control\r\n")));        
        return KernelIoControl(IOCTL_HAL_PROFILE_DVS, pBufIn, dwLenIn, pBufOut, dwLenOut, pdwActualOut);        
        break;
#endif        
#endif        
	}

	return bRet;
}

// EOF
