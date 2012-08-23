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
#include <DrvLib.h>
#include <pmpolicy.h>
#include "smdk6410_button.h"

#ifdef	REMOVE_BEFORE_RELEASE
#define PWRBTN_MSG(x) RETAILMSG(TRUE, x)
#define PWRBTN_INF(x) RETAILMSG(TRUE, x)
#define PWRBTN_ERR(x)	RETAILMSG(TRUE, x)
#else
//#define PWRBTN_MSG(x)	RETAILMSG(TRUE, x)
#define PWRBTN_MSG(x)
#define PWRBTN_INF(x)	RETAILMSG(TRUE, x)
//#define PWRBTN_INF(x)
#define PWRBTN_ERR(x)	RETAILMSG(TRUE, x)
//#define PWRBTN_ERR(x)
#endif

//#define SLEEP_AGING_TEST
HANDLE APPEvent = NULL;
#define APP_NOTIFY_EVENT (TEXT("POWER APP KEY"))

static volatile S3C6410_GPIO_REG *g_pGPIOReg = NULL;
static DWORD g_dwSysIntrPowerBtn = SYSINTR_UNDEFINED;
static DWORD g_dwSysIntrResetBtn = SYSINTR_UNDEFINED;
static HANDLE g_hEventPowerBtn = NULL;
static HANDLE g_hEventResetBtn = NULL;
static HANDLE g_hThreadPowerBtn = NULL;
static HANDLE g_hThreadResetBtn = NULL;
static BOOL g_bExitThread = FALSE;
static BOOL bFirstStartUp=TRUE;
INT WINAPI PowerButtonThread(void)
{
	DWORD nBtnCount = 0;
	WCHAR  state[1024] = {0};
	LPWSTR pState = &state[0];
	DWORD dwBufChars = (sizeof(state) / sizeof(state[0]));
	DWORD  dwStateFlags = 0;
	DWORD dwErr;
	PROCESS_INFORMATION AppProcess;

	PWRBTN_INF((_T("[PWR:INF] ++PowerButtonThread()\r\n")));

	while(!g_bExitThread)
	{
		nBtnCount=0;
		WaitForSingleObject(g_hEventPowerBtn, INFINITE);

		if(g_bExitThread)
		{
			break;
		}

		Button_pwrbtn_disable_interrupt();			// Mask EINT
		Button_pwrbtn_clear_interrupt_pending();		// Clear Interrupt Pending

		dwErr = GetSystemPowerState(pState, dwBufChars, &dwStateFlags);
		if (ERROR_SUCCESS != dwErr)
		{
			RETAILMSG(1, (TEXT("PMGET!GetSystemPowerState:ERROR:%d\n"), dwErr));
		}
		else
		{
			RETAILMSG(1, (TEXT("PMGET! System Power state is '%s', flags 0x%08x\n"), state, dwStateFlags));
		}
		InterruptDone(g_dwSysIntrPowerBtn);
		

#ifndef    SLEEP_AGING_TEST
		while(Button_pwrbtn_is_pushed())
		{
			// Wait for Button Released...
			Sleep(10);
			nBtnCount++;
			if(nBtnCount>=100)
				break;
		}
#endif
		if(nBtnCount>=100)
			{
				//if(bFirstStartUp)
					{
						CreateProcess(L"STD.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, &AppProcess);
						bFirstStartUp=FALSE;
						Sleep(700);
					}
				SetEvent(APPEvent);
				//InterruptDone(g_dwSysIntrPowerBtn);
				Button_pwrbtn_clear_interrupt_pending();
				Button_pwrbtn_enable_interrupt();		
				continue;
			}else if(nBtnCount<=5)
				{
				RETAILMSG(1,(TEXT("Not power button,continue %d "),nBtnCount));
				Button_pwrbtn_clear_interrupt_pending();
				Button_pwrbtn_enable_interrupt();		
				continue;
				}
		// if(wcscmp(state,TEXT("on")) != 0)
		//	{
		//		InterruptDone(g_dwSysIntrPowerBtn);
		//		Button_pwrbtn_enable_interrupt();		
		//		continue;
		//	}
			
		PWRBTN_INF((_T("[PWR] Power Button Event [%d]\r\n"), nBtnCount));

		//SetSystemPowerState(NULL, POWER_STATE_SUSPEND, POWER_FORCE);
		//PowerPolicyNotify(PPN_POWERBUTTONPRESSED,0);

		if ( wcscmp(state,TEXT("screenoff")) == 0 )
		{
			RETAILMSG(1,(TEXT("setsystempowerstate to ON from screen off\r\n")));
			SetSystemPowerState( NULL, POWER_STATE_ON, POWER_FORCE );
		}
		else if ( wcscmp(state,TEXT("backlightoff")) == 0 ||wcscmp(state,TEXT("resuming")) == 0 )
		{

			#ifdef WPC
				RETAILMSG(1,(TEXT("setsystempowerstate to ON from backlight off\r\n")));
				SetSystemPowerState( NULL, POWER_STATE_ON, POWER_FORCE );
			#endif
			#ifdef SMARTFON
			  	 RETAILMSG(1,(TEXT("setsystempowerstate to IDLE from backlight off\r\n")));
				SetSystemPowerState( TEXT("useridle"), POWER_STATE_IDLE, POWER_FORCE );
			#endif
		}
		else
		{
			#ifdef WPC
				//SetSystemPowerState( NULL, POWER_STATE_SUSPEND, POWER_FORCE );
				PowerPolicyNotify(PPN_POWERBUTTONPRESSED,0);
			#endif
			#ifdef SMARTFON
				RETAILMSG(1,(TEXT("setsystempowerstate to IDLE from on\r\n")));
				SetSystemPowerState( TEXT("useridle"), POWER_STATE_IDLE, POWER_FORCE );
			#endif
		}
		

		Button_pwrbtn_enable_interrupt();			// UnMask EINT
#ifdef    SLEEP_AGING_TEST
        //Sleep(2000);
	
        //SetEvent(g_hEventPowerBtn);
#endif
	}

	PWRBTN_INF((_T("[PWR:INF] --PowerButtonThread()\r\n")));

	return 0;
}

INT WINAPI ResetButtonThread(void)
{
	PWRBTN_INF((_T("[PWR:INF] ++ResetButtonThread()\r\n")));

	while(!g_bExitThread)
	{
		WaitForSingleObject(g_hEventResetBtn, INFINITE);

		if(g_bExitThread)
		{
			break;
		}

		Button_rstbtn_disable_interrupt();			// Mask EINT
		Button_rstbtn_clear_interrupt_pending();		// Clear Interrupt Pending

		InterruptDone(g_dwSysIntrResetBtn);

		PWRBTN_INF((_T("[PWR] Reset Button Event\r\n")));

		SetSystemPowerState(NULL, POWER_STATE_RESET, POWER_FORCE);
		//KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL);

		PWRBTN_ERR((_T("[PWR:ERR] Soft Reset Failed\r\n")));

		Button_rstbtn_enable_interrupt();				// UnMask EINT
	}

	PWRBTN_INF((_T("[PWR:INF] --ResetButtonThread()\r\n")));

	return 0;
}

static BOOL
PWR_AllocResources(void)
{
	DWORD dwIRQ;

	PWRBTN_MSG((_T("[PWR] ++PWR_AllocResources()\r\n")));

	//------------------
	// GPIO Controller SFR
	//------------------
	g_pGPIOReg = (S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (g_pGPIOReg == NULL)
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : pGPIOReg DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;
	}

	//--------------------
	// Power Button Interrupt
	//--------------------
	dwIRQ = IRQ_EINT11;
	g_dwSysIntrPowerBtn = SYSINTR_UNDEFINED;
	g_hEventPowerBtn = NULL;

	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &dwIRQ, sizeof(DWORD), &g_dwSysIntrPowerBtn, sizeof(DWORD), NULL))
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : IOCTL_HAL_REQUEST_SYSINTR Power Button Failed \n\r")));
		g_dwSysIntrPowerBtn = SYSINTR_UNDEFINED;
		return FALSE;
	}

	g_hEventPowerBtn = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == g_hEventPowerBtn)
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : CreateEvent() Power Button Failed \n\r")));
		return FALSE;
	}

	if (!(InterruptInitialize(g_dwSysIntrPowerBtn, g_hEventPowerBtn, 0, 0)))
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : InterruptInitialize() Power Button Failed \n\r")));
		return FALSE;
	}

	//--------------------
	// Reset Button Interrupt
	//--------------------
	/*dwIRQ = IRQ_EINT9;
	g_dwSysIntrResetBtn = SYSINTR_UNDEFINED;
	g_hEventResetBtn = NULL;

	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &dwIRQ, sizeof(DWORD), &g_dwSysIntrResetBtn, sizeof(DWORD), NULL))
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : IOCTL_HAL_REQUEST_SYSINTR Reset Button Failed \n\r")));
		g_dwSysIntrResetBtn = SYSINTR_UNDEFINED;
		return FALSE;
	}

	g_hEventResetBtn = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(NULL == g_hEventResetBtn)
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : CreateEvent() Reset Button Failed \n\r")));
		return FALSE;
	}

	if (!(InterruptInitialize(g_dwSysIntrResetBtn, g_hEventResetBtn, 0, 0)))
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_AllocResources() : InterruptInitialize() Reset Button Failed \n\r")));
		return FALSE;
	}
*/
	PWRBTN_MSG((_T("[PWR] --PWR_AllocResources()\r\n")));

	return TRUE;
}

static void
PWR_ReleaseResources(void)
{
	PWRBTN_MSG((_T("[PWR] ++PWR_ReleaseResources()\r\n")));

	if (g_pGPIOReg != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)g_pGPIOReg);
		g_pGPIOReg = NULL;
	}

	if (g_dwSysIntrPowerBtn != SYSINTR_UNDEFINED)
	{
		InterruptDisable(g_dwSysIntrPowerBtn);
	}

	if (g_hEventPowerBtn != NULL)
	{
		CloseHandle(g_hEventPowerBtn);
	}

	if (g_dwSysIntrPowerBtn != SYSINTR_UNDEFINED)
	{
		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &g_dwSysIntrPowerBtn, sizeof(DWORD), NULL, 0, NULL);
	}

	if (g_dwSysIntrResetBtn != SYSINTR_UNDEFINED)
	{
		InterruptDisable(g_dwSysIntrResetBtn);
	}

	if (g_hEventResetBtn != NULL)
	{
		CloseHandle(g_hEventResetBtn);
	}

	if (g_dwSysIntrResetBtn != SYSINTR_UNDEFINED)
	{
		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &g_dwSysIntrResetBtn, sizeof(DWORD), NULL, 0, NULL);
	}

	g_pGPIOReg = NULL;

	g_dwSysIntrPowerBtn = SYSINTR_UNDEFINED;
	g_dwSysIntrResetBtn = SYSINTR_UNDEFINED;

	g_hEventPowerBtn = NULL;
	g_hEventResetBtn = NULL;

	PWRBTN_MSG((_T("[PWR] --PWR_ReleaseResources()\r\n")));
}

BOOL
DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//DEBUGREGISTER(hinstDll);
		PWRBTN_MSG((_T("[PWR] DllEntry() : Process Attach\r\n")));
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		PWRBTN_MSG((_T("[PWR] DllEntry() : Process Detach\r\n")));
	}

	return TRUE;
}

BOOL
PWR_PowerUp(DWORD pContext)
{
	PWRBTN_INF((_T("[PWR] PWR_PowerUp(0x%08x)\r\n"), pContext));

	// Do not Disable Interrupt for Power Bottun
	// Also Do not Clear Pending.
	// After Wake Up. Button Driver Should Get Interrupt
	// and Set System Power State to Resume...
	// Interrupt Disable
	Button_pwrbtn_disable_interrupt();
	//Button_rstbtn_disable_interrupt();

	// Port Initialize as External Interrupt
	Button_port_initialize();

	// Interrupt Siganl Method and Filtering
	Button_pwrbtn_set_interrupt_method(EINT_SIGNAL_FALL_EDGE);
	Button_pwrbtn_set_filter_method(EINT_FILTER_DELAY, 0);
	//Button_rstbtn_set_interrupt_method(EINT_SIGNAL_FALL_EDGE);
	//Button_rstbtn_set_filter_method(EINT_FILTER_DELAY, 0);

	// Clear Interrupt Pending
	Button_pwrbtn_clear_interrupt_pending();
	//Button_rstbtn_clear_interrupt_pending();

	// Enable Interrupt
	Button_pwrbtn_enable_interrupt();
	//Button_rstbtn_enable_interrupt();

	return TRUE;
}

BOOL
PWR_PowerDown(DWORD pContext)
{
	PWRBTN_INF((_T("[PWR] PWR_PowerDown(0x%08x)\r\n"), pContext));

	// Interrupt Disable and Clear Pending
	Button_pwrbtn_disable_interrupt();
	Button_pwrbtn_clear_interrupt_pending();
	Button_rstbtn_disable_interrupt();
	Button_rstbtn_clear_interrupt_pending();

	return TRUE;
}

BOOL PWR_Deinit(DWORD pContext)
{
	PWRBTN_MSG((_T("[PWR] ++PWR_Deinit(0x%08x)\r\n"), pContext));

	g_bExitThread = TRUE;

	if (g_hThreadPowerBtn)		// Make Sure if thread is exist
	{
		Button_pwrbtn_disable_interrupt();
		Button_pwrbtn_clear_interrupt_pending();

		// Signal Thread to Finish
		SetEvent(g_hEventPowerBtn);
		// Wait for Thread to Finish
		WaitForSingleObject(g_hThreadPowerBtn, INFINITE);
		CloseHandle(g_hThreadPowerBtn);
		g_hThreadPowerBtn = NULL;
	}

	if (g_hThreadResetBtn)		// Make Sure if thread is exist
	{
		Button_rstbtn_disable_interrupt();
		Button_rstbtn_clear_interrupt_pending();

		// Signal Thread to Finish
		SetEvent(g_hEventResetBtn);
		// Wait for Thread to Finish
		WaitForSingleObject(g_hThreadResetBtn, INFINITE);
		CloseHandle(g_hThreadResetBtn);
		g_hThreadResetBtn = NULL;
	}

	PWR_ReleaseResources();

	PWRBTN_MSG((_T("[PWR] --PWR_Deinit()\r\n")));

	return TRUE;
}

DWORD
PWR_Init(DWORD dwContext)
{
	PWRBTN_MSG((_T("[PWR] ++PWR_Init(0x%08x)\r\n"), dwContext));

	if (PWR_AllocResources() == FALSE)
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_Init() : PWR_AllocResources() Failed \n\r")));

		goto CleanUp;
	}

	Button_initialize_register_address((void *)g_pGPIOReg);

	// Interrupt Disable and Clear Pending
	Button_pwrbtn_disable_interrupt();
	//Button_rstbtn_disable_interrupt();

	// Initialize Port as External Interrupt
	Button_port_initialize();

	// Create Power Button Thread
	g_hThreadPowerBtn = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) PowerButtonThread, NULL, 0, NULL);
	if (g_hThreadPowerBtn == NULL )
	{
		PWRBTN_ERR((_T("[PWR:ERR] PWR_Init() : CreateThread() Power Button Failed \n\r")));
		goto CleanUp;
	}

	// Create Reset Button Thread
//	g_hThreadResetBtn = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ResetButtonThread, NULL, 0, NULL);
//	if (g_hThreadResetBtn == NULL )
//	{
//		PWRBTN_ERR((_T("[PWR:ERR] PWR_Init() : CreateThread() Reset Button Failed \n\r")));
//		goto CleanUp;
//	}

	// Interrupt Siganl Method and Filtering
	Button_pwrbtn_set_interrupt_method(EINT_SIGNAL_FALL_EDGE);
	Button_pwrbtn_set_filter_method(EINT_FILTER_DELAY, 0);
	//Button_rstbtn_set_interrupt_method(EINT_SIGNAL_FALL_EDGE);
	//Button_rstbtn_set_filter_method(EINT_FILTER_DELAY, 0);

	// Clear Interrupt Pending
	Button_pwrbtn_clear_interrupt_pending();
	//Button_rstbtn_clear_interrupt_pending();

	// Enable Interrupt
	Button_pwrbtn_enable_interrupt();
	//Button_rstbtn_enable_interrupt();

	PWRBTN_MSG((_T("[PWR] --PWR_Init()\r\n")));
	APPEvent = CreateEvent(NULL, FALSE, FALSE, APP_NOTIFY_EVENT);

	return TRUE;

CleanUp:

	PWRBTN_ERR((_T("[PWR:ERR] --PWR_Init() : Failed\r\n")));

	PWR_Deinit(0);

	return FALSE;
}

DWORD
PWR_Open(DWORD pContext, DWORD dwAccess, DWORD dwShareMode)
{
	PWRBTN_MSG((_T("[PWR] PWR_Open(0x%08x, 0x%08x, 0x%08x)\r\n"), pContext, dwAccess, dwShareMode));

	return (0x12345678);
}

BOOL
PWR_Close(DWORD pContext)
{
	PWRBTN_MSG((_T("[PWR] PWR_Close(0x%08x)\r\n"), pContext));

	return TRUE;
}

DWORD
PWR_Read (DWORD pContext,  LPVOID pBuf, DWORD Len)
{
	PWRBTN_MSG((_T("[PWR] PWR_Read(0x%08x, 0x%08x, 0x%08x)\r\n"), pContext, pBuf, Len));

	return (0);	// End of File
}

DWORD
PWR_Write(DWORD pContext, LPCVOID pBuf, DWORD Len)
{
	PWRBTN_MSG((_T("[PWR] PWR_Write(0x%08x, 0x%08x, 0x%08x)\r\n"), pContext, pBuf, Len));

	return (0);	// Number of Byte
}

DWORD
PWR_Seek (DWORD pContext, long pos, DWORD type)
{
	PWRBTN_MSG((_T("[PWR] PWR_Seek(0x%08x, 0x%08x, 0x%08x)\r\n"), pContext, pos, type));

	return (DWORD)-1;	// Failure
}

BOOL
PWR_IOControl(DWORD pContext, DWORD dwCode, PBYTE pBufIn, DWORD dwLenIn, PBYTE pBufOut, DWORD dwLenOut, PDWORD pdwActualOut)
{
	PWRBTN_MSG((_T("[PWR] PWR_IOControl(0x%08x, 0x%08x)\r\n"), pContext, dwCode));

	return FALSE;	// Failure
}

