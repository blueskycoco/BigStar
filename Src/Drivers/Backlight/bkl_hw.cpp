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

#include <bsp.h>

#include <pm.h>
#include "pmplatform.h"
#include "bkl_hw.h"
#include <drvlib.h>


//  Globals
const TCHAR szevtBacklightChange[] = TEXT("BackLightChangeEvent");
const TCHAR szevtPowerChange[] = TEXT("PowerChangeEvent");

const TCHAR szregRootKey[] = TEXT("ControlPanel\\Backlight");
const TCHAR szregBatteryTimeout[] = TEXT("BatteryTimeout");
const TCHAR szregACTimeout[] = TEXT("ACTimeout");
const TCHAR szregBatteryAuto[] = TEXT("BacklightOnTap");
const TCHAR szregACAuto[] = TEXT("ACBacklightOnTap");
const TCHAR szregBatteryBrightNess[] = TEXT("BrightNess");
const TCHAR szregACBrightNess[] = TEXT("ACBrightNess");
const TCHAR szregBatteryOnOff[] = TEXT("OnOff");
const TCHAR szregBatteryTimeoutUnchecked[] = TEXT("BatteryTimeout");
const TCHAR szregACTimeoutUnchecked[] = TEXT("ACTimeout");
const TCHAR szregExtOnOff[] = TEXT("ExtOnOff");
const TCHAR szregAutoMode[] = TEXT("AutoMode");

HANDLE   g_evtSignal[NUM_EVENTS];


//  Global structure
BLStruct g_BLInfo;

volatile static S3C6410_GPIO_REG *v_pGPIORegs;
volatile static S3C6410_PWM_REG *v_pPWMRegs;
volatile static BSP_ARGS *v_pBSPArgs;

//
// Perform all one-time initialization of the backlight
//
BOOL
BacklightInitialize()
{
    BOOL    bRet = TRUE;

	v_pGPIORegs = (volatile S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (v_pGPIORegs == NULL)
	{
		RETAILMSG(1, (TEXT("#####BackLight: v_pGPIORegs DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;
	}

	v_pPWMRegs = (volatile S3C6410_PWM_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_PWM, sizeof(S3C6410_PWM_REG), FALSE);
	if (v_pPWMRegs == NULL)
	{
		RETAILMSG(1, (TEXT("#####BackLight: v_pPWMRegs DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;
	}

	v_pBSPArgs = (volatile BSP_ARGS *)DrvLib_MapIoSpace(IMAGE_SHARE_ARGS_PA_START, sizeof(BSP_ARGS), FALSE);
	if (v_pBSPArgs == NULL)
	{
		RETAILMSG(1, (TEXT("#####BackLight: v_pBSPArgs DrvLib_MapIoSpace() Failed \n\r")));
		return FALSE;
	}

    if (bRet)
    {
    	RETAILMSG(1, (TEXT("BacklightHWInitialize\r\n")));
		v_pGPIORegs->GPFPUD = v_pGPIORegs->GPFPUD&(~(3<<30))|(0<<30); // Pull-down enable

 		BL_PowerOn(TRUE);
	}
	else
	{
		RETAILMSG(1,(_T("Backlight Hw Initialize Fail\r\n")));
	}

    return bRet;
}


//  Utility function to read from registry for the parameters
void BL_ReadRegistry(BLStruct *pBLInfo)
{
    HKEY    hKey;
    LONG    lResult;
    DWORD   dwType;
    DWORD   dwVal;
    DWORD   dwLen;

    lResult = RegOpenKeyEx(HKEY_CURRENT_USER, szregRootKey, 0, KEY_ALL_ACCESS, &hKey);
    if(ERROR_SUCCESS == lResult) {
        dwType = REG_DWORD;
        dwLen = sizeof(DWORD);

        lResult = RegQueryValueEx(hKey, szregBatteryTimeout, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_dwBatteryTimeout = dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregACTimeout, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_dwACTimeout = dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregBatteryAuto, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bBatteryAuto = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregACAuto, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bACAuto = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregBatteryBrightNess, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_dwBatteryBrightNess = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregACBrightNess, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_dwACBrightNess = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregBatteryOnOff, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bBatteryOnOff = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregBatteryTimeoutUnchecked, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bBatteryTimeout = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregACTimeoutUnchecked, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bACTimeout = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregExtOnOff, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bExtOnOff = (BOOL) dwVal;
        }

        lResult = RegQueryValueEx(hKey, szregAutoMode, NULL, &dwType, (LPBYTE)&dwVal, &dwLen);
        if(ERROR_SUCCESS == lResult) {
            pBLInfo->m_bAutoMode = (BOOL) dwVal;
        }

        RegCloseKey(hKey);
    }
    else {
        RETAILMSG(1, (TEXT("BKL : HKEY_CURRENT_USER\\%s key doesn't exist!\r\n"), szregRootKey));
    }
}


// uninitialize the backlight
void BL_Deinit()
{
    int i;

	RETAILMSG(1, (TEXT("BKL : BL_Deinit!\r\n")));

	if(v_pGPIORegs != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)v_pGPIORegs);
		v_pGPIORegs = NULL;
	}
	if(v_pPWMRegs != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)v_pPWMRegs);
		v_pPWMRegs = NULL;
	}
	if(v_pBSPArgs != NULL)
	{
		DrvLib_UnmapIoSpace((PVOID)v_pBSPArgs);
		v_pBSPArgs = NULL;
	}

    //  Clean up
    for(i=0; i<NUM_EVENTS; i++) {
        if(g_evtSignal[i]) {
            CloseHandle(g_evtSignal[i]);
        }
    }
}


//
// initialize the backlight
//
BOOL BL_Init()
{
    //  Set up all the events we need.
    g_evtSignal[0] = CreateEvent(NULL, FALSE, FALSE, szevtBacklightChange);
    g_evtSignal[1] = CreateEvent(NULL, FALSE, FALSE, szevtPowerChange);


    if(!g_evtSignal[0] || !g_evtSignal[1] ) {
        BL_Deinit();
        return FALSE;
    }
    return TRUE;
}


//
//  find out if AC power is plugged in
//
BOOL IsACOn()
{
    if (v_pBSPArgs->bPowerStatus & AC_STATUS)
    {
		RETAILMSG(1,(TEXT("BACKLIGHT::AC STATUS \r\n")));
		return TRUE;
	}
    else
    {
    	RETAILMSG(1,(TEXT("BACKLIGHT::DC STATUS \r\n")));
		return FALSE;
	}
}



void BL_InitBrightness()
{
#if (SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB)
			v_pGPIORegs->GPFDAT = v_pGPIORegs->GPFDAT&(~(1<<14))|(1<<14); //Back Light enable

			v_pGPIORegs->GPFCON = v_pGPIORegs->GPFCON&(~(3<<28))|(2<<28); //GPF15=TOUT1

			v_pPWMRegs->TCFG0 = v_pPWMRegs->TCFG0 & (~(0xff<<0)|(PWM0_1_PRESCALER<<0));
			v_pPWMRegs->TCFG1 = v_pPWMRegs->TCFG1&(~(0xf<<0))|(PWM1_DIVIDER<<0); 	//Timer1 Devider set

			v_pPWMRegs->TCON = v_pPWMRegs->TCON&(~(1<<3))|(1<<3);		// enable Timer1 auto reload
			v_pPWMRegs->TCON = v_pPWMRegs->TCON&(~(1<<1))|(1<<1); 	//Timer1 manual update clear (TCNTB1 & TCMPB1)
#endif

}
void BL_Delay_On()
{
	v_pGPIORegs->GPFDAT |= (1<<15);
	v_pGPIORegs->GPFCON = (v_pGPIORegs->GPFCON & ~(3<<30)) | (1<<30); // set GPF[15] as Output

}
//
// turn on/off the backlight
//
void BL_On(BOOL bOn)
{
    if(bOn) {
		if (g_BLInfo.m_dwStatus != BL_ON)
		{
			g_BLInfo.m_dwStatus = BL_ON;
			v_pGPIORegs->GPFDAT = v_pGPIORegs->GPFDAT&(~(1<<14))|(1<<14); //Back Light power on
			v_pGPIORegs->GPFCON = v_pGPIORegs->GPFCON&(~(3<<28))|(1<<28);  // GPF15 = OUTPUT  //backlight power
			BL_InitBrightness();
			RETAILMSG(1,(TEXT("BACKLIGHT::ON\r\n")));
		}
    }
    else {
		if (g_BLInfo.m_dwStatus != BL_OFF)
		{
			g_BLInfo.m_dwStatus = BL_OFF;
			v_pGPIORegs->GPFDAT = v_pGPIORegs->GPFDAT&(~(1<<14))|(0<<14); //Back Light power off
			v_pGPIORegs->GPFCON = v_pGPIORegs->GPFCON&(~(3<<28))|(1<<28);  // GPF15 = OUTPUT  //backlight power
			RETAILMSG(1,(TEXT("BACKLIGHT::OFF\r\n")));
		}
    }
}


void BL_ChangeBrightness()
{
#if (SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB)
			RETAILMSG(1,(TEXT("g_BLInfo.m_dwACBrightNess = %d \r\n"), g_BLInfo.m_dwACBrightNess));
			RETAILMSG(1,(TEXT("g_BLInfo.m_dwBatteryBrightNess = %d \r\n"), g_BLInfo.m_dwBatteryBrightNess));
			if ( IsACOn() == TRUE )
			{
				v_pPWMRegs->TCNTB0 = 5000;						// Timer1 counter buffer value set (TCNTB:500 1 period=1kHz, TCNTB:5000 1period=100Hz, TCNTB:2500, 1period=200Hz))
				v_pPWMRegs->TCMPB0 = (2500*g_BLInfo.m_dwACBrightNess)/100;			// Timer1 compare buffer value set to input value
				RETAILMSG(1,(TEXT("Changebrightness: ACBrightness=%d TCMPB1=%d\r\n"), g_BLInfo.m_dwACBrightNess, v_pPWMRegs->TCMPB0));
			}
			else
			{
				v_pPWMRegs->TCNTB0 = 5000;						// Timer1 counter buffer value set (TCNTB:500 1 period=1kHz, TCNTB:5000 1period=100Hz, TCNTB:2500, 1period=200Hz))
				v_pPWMRegs->TCMPB0 = (2500*g_BLInfo.m_dwBatteryBrightNess)/100;	// Timer1 compare buffer value set to input value
				RETAILMSG(1,(TEXT("Changebrightness: BatteryBrightness=%d TCMPB1=%d\r\n"),  g_BLInfo.m_dwBatteryBrightNess, v_pPWMRegs->TCMPB0));
			}

			v_pPWMRegs->TCON = v_pPWMRegs->TCON&(~(1<<0))|(1<<0); 	//Timer1 start
			v_pPWMRegs->TCON = v_pPWMRegs->TCON&(~(1<<1))|(0<<1); 	//Timer1 manual update set (TCNTB2 & TCMPB2)
#else
			RETAILMSG(1,(TEXT("This LCD Module does not support Backlight Brightness Control\r\n")));
#endif
}
void Change_backlight_from_keypad(BOOL level)
{
	if(level)
		{
			if ( IsACOn() == TRUE )
    				g_BLInfo.m_dwACBrightNess += 25;
			else
				g_BLInfo.m_dwBatteryBrightNess += 25;
			if(g_BLInfo.m_dwACBrightNess>=200)
				g_BLInfo.m_dwACBrightNess=195;
			if(g_BLInfo.m_dwBatteryBrightNess>=200)
				g_BLInfo.m_dwBatteryBrightNess=195;
		}
	else
		{
			if ( IsACOn() == TRUE )
    				g_BLInfo.m_dwACBrightNess -= 25;
			else
				g_BLInfo.m_dwBatteryBrightNess -= 25;

			if(g_BLInfo.m_dwACBrightNess<=25)
				g_BLInfo.m_dwACBrightNess=25;
			if(g_BLInfo.m_dwBatteryBrightNess<=25)
				g_BLInfo.m_dwBatteryBrightNess=25;

		}
	BL_ChangeBrightness();
}
//
// restore power to the backlight
//
void BL_PowerOn(BOOL bInit)
{
    //
    //  Add power-on GPIO register setting
    //

    BL_On(TRUE);
}


// The backlight handling is done by a thread, which monitors those
// three event and performs some actions based on the parameters specified
// in HKLM/ControlPanel/Backlight
//
// backlight service thread
//
DWORD BL_MonitorThread(PVOID pParms)
{
    DWORD   dwResult;
    DWORD   dwTimeout;

    //  Initialization stuff is here
    //
    //  Initialize the events
    //  Initialize the BLInfo data structure
    //  Those are default values. Modify them if necessary
    g_BLInfo.m_bACAuto = TRUE;
    g_BLInfo.m_bBatteryAuto = TRUE;
    g_BLInfo.m_dwBatteryBrightNess = 50;
    g_BLInfo.m_dwACBrightNess = 64;

    //  Now read from the registry to see what they say
    BL_ReadRegistry(&g_BLInfo);

	BL_InitBrightness();
    BL_ChangeBrightness();

   //RETAILMSG(1,(TEXT("MonitorThread: ACBrightness=%d\r\n"), g_BLInfo.m_dwACBrightNess));
   //RETAILMSG(1,(TEXT("MonitorThread: BatteryBrightness=%d\r\n"),  g_BLInfo.m_dwBatteryBrightNess));

    //  Initialize BL
    if(!BL_Init()) {
        RETAILMSG(1, (TEXT("BL_Init() Failed! Exit from BL_MonitorThread!\r\n")));
        return 0;
    }
    while(1) {
		__try {
			dwTimeout = INFINITE;


//			RETAILMSG(1, (TEXT("BackLight Thread: dwTimeout = %d \r\n"), dwTimeout));

			//  Now let's wait for either there is an update on registry, or
			//  there is user action on the device, or there is activity on
			//  AC power supply.
			dwResult = WaitForMultipleObjects(NUM_EVENTS, g_evtSignal, FALSE, dwTimeout);

//			RETAILMSG(1, (TEXT("BackLight Thread: dwResult = %d \r\n"), dwResult));

			//  If we are signaled by registry event
			if(WAIT_OBJECT_0 == dwResult) {		// BackLight Change
				//  All we need to do is to read from registry and update the tick count
				BL_ReadRegistry(&g_BLInfo);

				RETAILMSG(1, (TEXT("+ BackLight Thread: backlight changed!\r\n")));
				//  Always turn on the Backlight after a change to registry
				//BL_On(TRUE);
				BL_ChangeBrightness();
//				RETAILMSG(1, (TEXT("- BackLight Thread: backlight changed!\r\n")));
			}

			else if(dwResult == WAIT_OBJECT_0+1) {
				BL_ReadRegistry(&g_BLInfo);
				RETAILMSG(1, (TEXT("BackLight Thread: Power changed!\r\n")));
				BL_ChangeBrightness();
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER){
			// do nothing
			RETAILMSG(1, (TEXT("an exception is raised in BL_MonitorThread... \r\n")));
		}
	}
}



