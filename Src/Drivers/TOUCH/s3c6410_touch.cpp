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
Copyright (c) 2005. Samsung Electronics, co. ltd  All rights reserved.

Module Name:

Abstract:

    Platform dependent TOUCH initialization functions

rev:
    2004.4.27    : S3C2440 port 
    2005.05.23    : Magneto porting revision
    2006.06.29    : S3C2443 port
    2007.02.21    : S3C6410 port

Notes:
--*/

#include <bsp.h>
#include <tchddsi.h>
#include "s3c6410_adc_touch_macro.h"
#include "TSC2003.h"

#ifndef DEBUG
DBGPARAM dpCurSettings = {
    TEXT("Touch"), { 
    TEXT("Samples"),TEXT("Calibrate"),TEXT("Stats"),TEXT("Thread"),
    TEXT("TipState"),TEXT("Init"),TEXT(""),TEXT(""),
    TEXT(""),TEXT("Misc"),TEXT("Delays"),TEXT("Timing"),
    TEXT("Alloc"),TEXT("Function"),TEXT("Warning"),TEXT("Error") },
    0x8000              // error
};
#endif





//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------
DWORD gIntrTouch = SYSINTR_NOP;
DWORD gIntrTouchChanged = SYSINTR_NOP;

//------------------------------------------------------------------------------
// Local Variables
//------------------------------------------------------------------------------
static volatile S3C6410_GPIO_REG * g_pGPIOReg = NULL;
static volatile S3C6410_ADC_REG * g_pADCReg = NULL;
static volatile S3C6410_VIC_REG * g_pVIC0Reg = NULL;
static volatile S3C6410_VIC_REG * g_pVIC1Reg = NULL;
static volatile S3C6410_PWM_REG * g_pPWMReg = NULL;

static BOOL g_bTSP_Initialized = FALSE;
static BOOL g_bTSP_DownFlag = FALSE;
static int g_TSP_CurRate = 0;
static unsigned int g_SampleTick_High;
static unsigned int g_SampleTick_Low;
static CRITICAL_SECTION g_csTouchADC;    // Critical Section for ADC Done
static BOOL TouchIrq = TRUE;

//------------------------------------------------------------------------------
// External Variables
//------------------------------------------------------------------------------
extern "C" const int MIN_CAL_COUNT = 1;

static VOID TSP_VirtualFree(VOID);
static VOID TSP_SampleStop(VOID);

static BOOL
TSP_VirtualAlloc(VOID)
{
    BOOL bRet = TRUE;
	  PHYSICAL_ADDRESS    ioPhysicalBase = {0,0};

    DEBUGMSG(TSP_ZONE_FUNCTION,(_T("[TSP] ++TSP_VirtualAlloc()\r\n")));

    ioPhysicalBase.LowPart = S3C6410_BASE_REG_PA_GPIO;
    g_pGPIOReg = (S3C6410_GPIO_REG *)MmMapIoSpace(ioPhysicalBase, sizeof(S3C6410_GPIO_REG), FALSE);
    if (g_pGPIOReg == NULL)
    {
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] TSP_VirtualAlloc() : g_pGPIOReg Allocation Fail\r\n")));
        bRet = FALSE;
        goto CleanUp;
    }
    
        
    ioPhysicalBase.LowPart = S3C6410_BASE_REG_PA_VIC0;
    g_pVIC0Reg = (S3C6410_VIC_REG *)MmMapIoSpace(ioPhysicalBase, sizeof(S3C6410_VIC_REG), FALSE);
    if (g_pVIC0Reg == NULL)
    {
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] TSP_VirtualAlloc() : g_pVIC0Reg Allocation Fail\r\n")));
        bRet = FALSE;
        goto CleanUp;
    }

    ioPhysicalBase.LowPart = S3C6410_BASE_REG_PA_VIC1;
    g_pVIC1Reg = (S3C6410_VIC_REG *)MmMapIoSpace(ioPhysicalBase, sizeof(S3C6410_VIC_REG), FALSE);
    if (g_pVIC1Reg == NULL)
    {
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] TSP_VirtualAlloc() : g_pVIC1Reg Allocation Fail\r\n")));
        bRet = FALSE;
        goto CleanUp;
    }

    ioPhysicalBase.LowPart = S3C6410_BASE_REG_PA_PWM;
    g_pPWMReg = (S3C6410_PWM_REG *)MmMapIoSpace(ioPhysicalBase, sizeof(S3C6410_PWM_REG), FALSE);
    if (g_pPWMReg == NULL)
    {
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] TSP_VirtualAlloc() : g_pPWMReg Allocation Fail\r\n")));
        bRet = FALSE;
        goto CleanUp;
    }

CleanUp:

    if (bRet == FALSE)
    {
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] TSP_VirtualAlloc() : Failed\r\n")));

        TSP_VirtualFree();
    }

    TSPMSG((_T("[TSP] --TSP_VirtualAlloc() = %d\r\n"), bRet));

    return bRet;
}

static VOID
TSP_VirtualFree(VOID)
{
    TSPMSG((_T("[TSP] ++TSP_VirtualFree()\r\n")));

    if (g_pGPIOReg)
    {
        MmUnmapIoSpace((PVOID)g_pGPIOReg, sizeof(S3C6410_SYSCON_REG));
        g_pGPIOReg = NULL;
    }

    if (g_pVIC0Reg)
    {
        MmUnmapIoSpace((PVOID)g_pVIC0Reg, sizeof(S3C6410_VIC_REG));
        g_pVIC0Reg = NULL;
    }

    if (g_pVIC1Reg)
    {
        MmUnmapIoSpace((PVOID)g_pVIC1Reg, sizeof(S3C6410_VIC_REG));
        g_pVIC1Reg = NULL;
    }

    if (g_pPWMReg)
    {
        MmUnmapIoSpace((PVOID)g_pPWMReg, sizeof(S3C6410_PWM_REG));
        g_pPWMReg = NULL;
    }

    TSPMSG((_T("[TSP] --TSP_VirtualFree()\r\n")));
}


static VOID
TSP_PowerOn(VOID)
{
    TSPMSG((_T("[TSP] ++TSP_PowerOn()\r\n")));
	RETAILMSG(1,(TEXT("[TSP] ++TSP_PowerOn()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n")));
    //TSC2003 init
   // TSC_Init();
     
     
    //timer 
    g_SampleTick_Low = TSP_TIMER_CNT_LOW;
    g_SampleTick_High = TSP_TIMER_CNT_HIGH;

    // Set Divider MUX for Timer3
    SET_TIMER3_DIVIDER_MUX(g_pPWMReg, TSP_TIMER_DIVIDER);    

    g_pPWMReg->TCNTB3  = g_SampleTick_Low;

    // timer3 interrupt disable
    g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) & ~TIMER3_INTERRUPT_ENABLE;

    // timer3 interrupt status clear
    g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER3_PENDING_CLEAR;

	
    TSPMSG((_T("[TSP] --TSP_PowerOn()\r\n")));
}

static VOID
TSP_PowerOff(VOID)
{
    TSPMSG((_T("[TSP] ++TSP_PowerOff()\r\n")));

    TSP_SampleStop();

    // To prevent touch locked after wake up,
    // Wait for ADC Done
    // Do not turn off ADC before its A/D conversion finished
    EnterCriticalSection(&g_csTouchADC);
    // ADC Done in TSP_GETXy()..
    LeaveCriticalSection(&g_csTouchADC);

    TSPMSG((_T("[TSP] --TSP_PowerOff()\r\n")));
}

static VOID
TSP_SampleStart(VOID)
{
    // timer3 interrupt status clear, Do not use OR/AND operation on TINTC_CSTAT directly
    g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER3_PENDING_CLEAR;

    // timer3 interrupt enable, Do not use OR/AND operation on TINTC_CSTAT directly
    g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER3_INTERRUPT_ENABLE;

    STOP_TIMER3(g_pPWMReg);

    UPDATE_TCNTB3(g_pPWMReg);
    NOUPDATE_TCNTB3(g_pPWMReg);

    SET_TIMER3_AUTORELOAD(g_pPWMReg);
    START_TIMER3(g_pPWMReg);
}

static VOID
TSP_SampleStop(VOID)
{
    STOP_TIMER3(g_pPWMReg);

    // timer3 interrupt disable, Do not use OR/AND operation on TINTC_CSTAT directly
    g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) & ~TIMER3_INTERRUPT_ENABLE;

    // timer3 interrupt status clear, Do not use OR/AND operation on TINTC_CSTAT directly
    g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER3_PENDING_CLEAR;
}

static BOOL
TSP_CalibrationPointGet(TPDC_CALIBRATION_POINT *pTCP)
{
    int cDisplayWidth, cDisplayHeight;
    int CalibrationRadiusX, CalibrationRadiusY;

    TSPMSG((_T("[TSP] ++TSP_CalibrationPointGet()\r\n")));

    cDisplayWidth = pTCP->cDisplayWidth;
    cDisplayHeight = pTCP->cDisplayHeight;
 
    
    CalibrationRadiusX = cDisplayWidth  / 20;
    CalibrationRadiusY = cDisplayHeight / 20;

    switch (pTCP->PointNumber)
    {
    case    0:
        pTCP->CalibrationX = cDisplayWidth  / 2;
        pTCP->CalibrationY = cDisplayHeight / 2;
        break;

    case    1:
        pTCP->CalibrationX = CalibrationRadiusX * 2;
        pTCP->CalibrationY = CalibrationRadiusY * 2;
        break;

    case    2:
        pTCP->CalibrationX = CalibrationRadiusX * 2;
        pTCP->CalibrationY = cDisplayHeight - CalibrationRadiusY * 2;
        break;

    case    3:
        pTCP->CalibrationX = cDisplayWidth  - CalibrationRadiusX * 2;
        pTCP->CalibrationY = cDisplayHeight - CalibrationRadiusY * 2;
        break;

    case    4:
        pTCP->CalibrationX = cDisplayWidth - CalibrationRadiusX * 2;
        pTCP->CalibrationY = CalibrationRadiusY * 2;
        break;

    default:
        pTCP->CalibrationX = cDisplayWidth  / 2;
        pTCP->CalibrationY = cDisplayHeight / 2;
        SetLastError(ERROR_INVALID_PARAMETER);
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] TSP_CalibrationPointGet() : ERROR_INVALID_PARAMETER\r\n")));
        return FALSE;
    }

    TSPMSG((_T("[TSP] --TSP_CalibrationPointGet()\r\n")));

    return TRUE;
}



//---------------------------------------------------------------------------


BOOL
DdsiTouchPanelEnable(VOID)
{
    UINT32 Irq[3];

    TSPMSG((_T("[TSP] ++DdsiTouchPanelEnable()\r\n")));

    if (!g_bTSP_Initialized)    // Map Virtual address and Interrupt at First time Only
    {
        if (!TSP_VirtualAlloc())
        {
            RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] DdsiTouchPanelEnable() : TSP_VirtualAlloc() Failed\r\n")));
            return FALSE;
        }

        // Initialize Critical Section
        InitializeCriticalSection(&g_csTouchADC);

        // Obtain SysIntr values from the OAL for the touch and touch timer interrupts.
        Irq[0] = -1;
        Irq[1] = OAL_INTR_FORCE_STATIC;
        Irq[2] = IRQ_EINT18;
        if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &Irq, sizeof(Irq), &gIntrTouch, sizeof(UINT32), NULL))
        {
            RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] DdsiTouchPanelEnable() : IOCTL_HAL_REQUEST_SYSINTR Failed\r\n")));
            gIntrTouch = SYSINTR_UNDEFINED;
            return FALSE;
        }

        Irq[0] = -1;
        Irq[1] = OAL_INTR_FORCE_STATIC;
        Irq[2] = IRQ_TIMER3;
        if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &Irq, sizeof(Irq), &gIntrTouchChanged, sizeof(UINT32), NULL))
        {
            RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] DdsiTouchPanelEnable() : IOCTL_HAL_REQUEST_SYSINTR Failed\r\n")));
            gIntrTouchChanged = SYSINTR_UNDEFINED;
            return FALSE ;
        }

        TSPINF((_T("[TSP:INF] DdsiTouchPanelEnable() : gIntrTouch = %d\r\n"), gIntrTouch));
        TSPINF((_T("[TSP:INF] DdsiTouchPanelEnable() : gIntrTouchChanged = %d\r\n"), gIntrTouchChanged));

        g_bTSP_Initialized = TRUE;
    }

    TSP_PowerOn();
	TSC_Init();
    TSPMSG((_T("[TSP] --DdsiTouchPanelEnable()\r\n")));

    return TRUE;
}

VOID
DdsiTouchPanelDisable(VOID)
{
    TSPMSG((_T("[TSP] ++DdsiTouchPanelDisable()\r\n")));

    if (g_bTSP_Initialized)
    {
        TSP_PowerOff();
        //TSP_VirtualFree();    // Do not release Virtual Address... Touch will be use all the time...
        //g_bTSP_Initialized = FALSE;
    }

    TSPMSG((_T("[TSP] --DdsiTouchPanelDisable()\r\n")));
}

BOOL
DdsiTouchPanelGetDeviceCaps(INT iIndex, LPVOID lpOutput)
{
    if ( lpOutput == NULL )
    {
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] DdsiTouchPanelGetDeviceCaps() : ERROR_INVALID_PARAMETER\r\n")));
        SetLastError(ERROR_INVALID_PARAMETER);
        ASSERT(0);
        return FALSE;
    }

    switch(iIndex)
    {
    case TPDC_SAMPLE_RATE_ID:
        {
            TPDC_SAMPLE_RATE *pTSR = (TPDC_SAMPLE_RATE*)lpOutput;

            TSPMSG((_T("[TSP] DdsiTouchPanelGetDeviceCaps() : TPDC_SAMPLE_RATE_ID\r\n")));

            pTSR->SamplesPerSecondLow = TSP_SAMPLE_RATE_LOW;
            pTSR->SamplesPerSecondHigh = TSP_SAMPLE_RATE_HIGH;
            pTSR->CurrentSampleRateSetting = g_TSP_CurRate;
        }
        break;
    case TPDC_CALIBRATION_POINT_COUNT_ID:
        {
            TPDC_CALIBRATION_POINT_COUNT *pTCPC = (TPDC_CALIBRATION_POINT_COUNT*)lpOutput;

            TSPMSG((_T("[TSP] DdsiTouchPanelGetDeviceCaps() : TPDC_CALIBRATION_POINT_COUNT_ID\r\n")));

            pTCPC->flags = 0;
            pTCPC->cCalibrationPoints = 5;      // calibrate touch point using 5points
        }
        break;
    case TPDC_CALIBRATION_POINT_ID:
        return(TSP_CalibrationPointGet((TPDC_CALIBRATION_POINT*)lpOutput));
    default:
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] DdsiTouchPanelGetDeviceCaps() : ERROR_INVALID_PARAMETER\r\n")));
        SetLastError(ERROR_INVALID_PARAMETER);
        ASSERT(0);
        return FALSE;
    }

    return TRUE;
}

BOOL
DdsiTouchPanelSetMode(INT iIndex, LPVOID  lpInput)
{
    BOOL  bRet = FALSE;

    TSPMSG((_T("[TSP] ++DdsiTouchPanelSetMode(%d)\r\n"), iIndex));

    switch ( iIndex )
    {
    case TPSM_SAMPLERATE_LOW_ID:
        g_TSP_CurRate = 0;
        g_pPWMReg->TCNTB3  = g_SampleTick_Low;
        SetLastError( ERROR_SUCCESS );
        bRet = TRUE;
        break;
    case TPSM_SAMPLERATE_HIGH_ID:
        g_TSP_CurRate = 1;
        g_pPWMReg->TCNTB3  = g_SampleTick_High;
        SetLastError( ERROR_SUCCESS );
        bRet = TRUE;
        break;
    default:
        RETAILMSG(TSP_ZONE_ERROR,(_T("[TSP:ERR] DdsiTouchPanelSetMode() : ERROR_INVALID_PARAMETER\r\n")));
        SetLastError( ERROR_INVALID_PARAMETER );
        break;
    }

    TSPMSG((_T("[TSP] --DdsiTouchPanelSetMode() = %d\r\n"), bRet));

    return bRet;
}

static DWORD  g_NextExpectedInterrupt = 1;
VOID
DdsiTouchPanelGetPoint(TOUCH_PANEL_SAMPLE_FLAGS *pTipState, INT *pUncalX, INT *pUncalY)
{
    static int PrevX=0;
    static int PrevY=0;
    int TmpX = 0;
    int TmpY = 0;
	//    DWORD bytes;
    DWORD InterruptType = SYSINTR_NOP; 
    static BOOL fail_flag= TRUE;
    //g_pGPIOReg->EINT0MASK  |= (1<<18) ;  //mask EINT0 interrupt 
    if( TouchIrq ) 
    {    
      if( fail_flag )
      {	
			PrevX = *pUncalX;
			PrevY = *pUncalY;  
			if(PDDSampleTouchScreen(pUncalX,pUncalY))
			{
				*pTipState = TouchSampleValidFlag | TouchSampleDownFlag;
				*pTipState &= ~TouchSampleIgnore; 
			}
			else
			{
				*pTipState = TouchSampleIgnore; 
				*pUncalX = PrevX;
				*pUncalY = PrevY;  
			}
			TouchIrq = FALSE;
			InterruptType   =  gIntrTouch;
			TSP_SampleStart();
			g_NextExpectedInterrupt = 2;
			RETAILMSG(0,(TEXT("GetPoint: pen is down ...\r\n")));
	     	
	    }
	    else 
	    {
			RETAILMSG(0,(TEXT("ENTER Here ...\r\n")));	
			fail_flag = TRUE;
			InterruptType   =  gIntrTouch;
			*pTipState = TouchSampleIgnore;
			*pUncalX = PrevX;
			*pUncalY = PrevY; 
			goto L1;
	    }
    }
    else
    {   
    	  InterruptType = gIntrTouchChanged;
    	  
    	  PrevX = *pUncalX;
    	  PrevY = *pUncalY; 
    	  if( PDDSampleTouchScreen(pUncalX, pUncalY))
    	  {
			*pTipState = TouchSampleValidFlag | TouchSampleDownFlag;
			*pTipState &= ~TouchSampleIgnore;
    	  }
        else
    	 {
			*pTipState = TouchSampleIgnore; 
			*pUncalX = PrevX;
			*pUncalY = PrevY;  
    	 }
    	 g_NextExpectedInterrupt = 2;
      	 //	 RETAILMSG(1,(TEXT("GetPoint: timer ...\r\n")));	      	
    }
    
    if( (g_NextExpectedInterrupt ==2 ) && (GetTouchStatus()))
    {    	
		RETAILMSG(0,(TEXT("GetPoint: pen is up ...\r\n")));	
		TouchIrq = TRUE;
		g_NextExpectedInterrupt = 1;
		*pTipState = TouchSampleValidFlag;
		TSP_SampleStop();   //stop timer
		fail_flag = FALSE;
		EnableTSC_Interrupt();   //Enable TSC2003 Interrup            
    }       
 L1:   
	InterruptDone(InterruptType);        // Not handled in MDD
	g_pGPIOReg->EINT0MASK &= ~(1<<18);	//unmask EINT0 interrupt
    
}

LONG
DdsiTouchPanelAttach(VOID)
{
    return (0);
}

LONG
DdsiTouchPanelDetach(VOID)
{
    return (0);
}

VOID
DdsiTouchPanelPowerHandler(BOOL bOff)
{
    TSPMSG((_T("[TSP] ++DdsiTouchPanelPowerHandler(%d)\r\n"), bOff));

    if (bOff)
    {
        TSP_PowerOff();
    }
    else
    {
        TSP_PowerOn();

        // Detect proper touch state after Wake Up
        SetInterruptEvent(gIntrTouchChanged);
    }

    TSPMSG((_T("[TSP] --DdsiTouchPanelPowerHandler()\r\n")));
}

