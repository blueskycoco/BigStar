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
//  Module: timer.c
//
//  Interface to OAL timer services.
//
#include <windows.h>
#include <nkintr.h>
#include <ceddk.h>
#include <oal.h>
#include <s3c6410.h>
#include <bsp_cfg.h>
#include <pmplatform.h>

#define MIN_IDLE_COUNT              20
#ifdef ENABLE_WATCH_DOG
extern void OALInitWatchDogTimer(void);
#endif

static INT32 g_PreSinceSysTick;
//------------------------------------------------------------------------------
// Local Variables
static volatile S3C6410_PWM_REG *g_pPWMReg = NULL;
static volatile S3C6410_SYSCON_REG *g_pSysConReg = NULL;
static volatile S3C6410_VIC_REG *g_pVIC0Reg = NULL;
static volatile S3C6410_VIC_REG *g_pVIC1Reg = NULL;
static volatile S3C6410_GPIO_REG *g_pGPIOReg = NULL;

//------------------------------------------------------------------------------
//
//  Function: OALTimerInit
//
//  This function is typically called from the OEMInit to initialize
//  Windows CE system timer. The tickMSec parameter determine timer
//  period in milliseconds. On most platform timer period will be
//  1 ms, but it can be usefull to use higher value for some
//  specific (low-power) devices.
//
//  Implementation for S3C6410 is using timer 4 as system timer.
//
BOOL OALTimerInit(UINT32 msecPerSysTick, UINT32 countsPerMSec, UINT32 countsMargin)
{
	BOOL rc = FALSE;
	UINT32 countsPerSysTick;
	UINT32 SysIntr, Irq;

	OALMSG(OAL_TIMER&&OAL_FUNC, (
		L"+OALTimerInit( %d, %d, %d )\r\n",
		msecPerSysTick, countsPerMSec, countsMargin	));
	OALMSG(1, (
		L"+OALTimerInit( %d, %d, %d )\r\n",
		msecPerSysTick, countsPerMSec, countsMargin	));

	// Initialize High Resolution Timer function pointers
	pQueryPerformanceFrequency = OALTimerQueryPerformanceFrequency;
	pQueryPerformanceCounter = OALTimerQueryPerformanceCounter;

	countsPerSysTick = countsPerMSec * msecPerSysTick;

	// Validate Input parameters
	if ((msecPerSysTick < 1)
		||(msecPerSysTick > 1000)
		||(countsPerSysTick < 1)
		||(countsPerSysTick > 0xFFFF) )
	{
		OALMSG(OAL_ERROR, (L"ERROR: OALTimerInit: System tick period out of range..."));
		goto cleanUp;
	}

	// Initialize Timer State Global variable (OAL_TIMER_STATE)
	g_oalTimer.msecPerSysTick = msecPerSysTick;
	g_oalTimer.countsPerMSec = countsPerMSec;
	g_oalTimer.countsMargin = countsMargin;
	g_oalTimer.countsPerSysTick = countsPerSysTick;
	g_oalTimer.curCounts = 0;
	g_oalTimer.maxPeriodMSec = 0xFFFF/g_oalTimer.countsPerMSec;

	g_oalTimer.actualMSecPerSysTick = g_oalTimer.msecPerSysTick;
	g_oalTimer.actualCountsPerSysTick = g_oalTimer.countsPerSysTick;

	// Set Kernel Exported Globals to Initial values
	idleconv = countsPerMSec;
	curridlehigh = 0;
	curridlelow = 0;

	// Create SYSINTR for timer
	Irq = IRQ_TIMER4;
	SysIntr = OALIntrRequestSysIntr(1, &Irq, OAL_INTR_FORCE_STATIC);

	g_pPWMReg = (S3C6410_PWM_REG*)OALPAtoUA(S3C6410_BASE_REG_PA_PWM);
	g_pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	g_pVIC0Reg = (S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC0, FALSE);
	g_pVIC1Reg = (S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC1, FALSE);
	g_pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	//---------------------
	// Initialize System Timer
	//---------------------

	// Set Prescaler 1 (Timer2,3,4)
	g_pPWMReg->TCFG0 = (g_pPWMReg->TCFG0 & ~(0xff<<8)) | ((SYS_TIMER_PRESCALER-1)<<8);

	// Set Divider MUX for Timer4
	switch(SYS_TIMER_DIVIDER)
	{
	case 1:
		g_pPWMReg->TCFG1 = (g_pPWMReg->TCFG1 & ~(0xf<<16)) | (0<<16);
		break;
	case 2:
		g_pPWMReg->TCFG1 = (g_pPWMReg->TCFG1 & ~(0xf<<16)) | (1<<16);
		break;
	case 4:
		g_pPWMReg->TCFG1 = (g_pPWMReg->TCFG1 & ~(0xf<<16)) | (2<<16);
		break;
	case 8:
		g_pPWMReg->TCFG1 = (g_pPWMReg->TCFG1 & ~(0xf<<16)) | (3<<16);
		break;
	case 16:
		g_pPWMReg->TCFG1 = (g_pPWMReg->TCFG1 & ~(0xf<<16)) | (4<<16);
		break;
	default:
		g_pPWMReg->TCFG1 = (g_pPWMReg->TCFG1 & ~(0xf<<16)) | (0<<16);
		break;
	}


	// Set Timer4 Count Buffer Register
	g_pPWMReg->TCNTB4 = g_oalTimer.countsPerSysTick - 1;

	// Timer4 Clear Interrupt Pending
	//g_pPWMReg->TINT_CSTAT |= (1<<9);	// Do not use OR/AND operation on TINTC_CSTAT
	g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER4_PENDING_CLEAR;

	// Timer4 Interrupt Enable
	//g_pPWMReg->TINT_CSTAT |= (1<<4);	// Do not use OR/AND operation on TINTC_CSTAT
	g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER4_INTERRUPT_ENABLE;

	// Start Timer4 in Auto Reload mode (Fixed Tick!!!)
	g_pPWMReg->TCON &= ~(0x7<<20);

	g_pPWMReg->TCON |= (1<<21);			// Update TCNTB4
	g_pPWMReg->TCON &= ~(1<<21);

	g_pPWMReg->TCON |= (1<<22)|(1<<20);	// Auto-reload Mode, Timer4 Start

	// Enable System Tick Interrupt
	if (!OEMInterruptEnable(SysIntr, NULL, 0))
	{
		OALMSG(OAL_ERROR, (L"ERROR: OALTimerInit: Interrupt enable for system timer failed"));
		goto cleanUp;
	}

#ifdef DVS_EN
	InitializeDVS();
#endif

	//
	// Define ENABLE_WATCH_DOG to enable watchdog timer support.
	// NOTE: When watchdog is enabled, the device will reset itself if watchdog timer is not refreshed within ~4.5 second.
	//       Therefore it should not be enabled when kernel debugger is connected, as the watchdog timer will not be refreshed.
	//
#ifdef ENABLE_WATCH_DOG
	OALInitWatchDogTimer ();
#endif

    // Initialize LED7(GPN12) used for "heart beat"
    g_pGPIOReg->GPNCON = (g_pGPIOReg->GPNCON & (~(0x3<<24))) | (0x1<<24);   // GPN12: Output
    g_pGPIOReg->GPNPUD = (g_pGPIOReg->GPNPUD & (~(0x3<<24)));               // GPN12: pull-up/down disabled

	// Done
	rc = TRUE;

cleanUp:

	OALMSG(OAL_TIMER && OAL_FUNC, (L"-OALTimerInit(rc = %d)\r\n", rc));

	return rc;
}

//------------------------------------------------------------------------------
//
//  Function: OALTimerIntrHandler
//
//  This function implement timer interrupt handler. It is called from common
//  ARM interrupt handler.
//
UINT32 OALTimerIntrHandler()
{
	static DWORD HeartBeatCnt, HeartBeatStat;  //LED4 is used for heart beat

	UINT32 SysIntr = SYSINTR_NOP;

    g_PreSinceSysTick = g_oalTimer.countsPerSysTick - (g_pPWMReg->TCNTO4);

	// Timer4 Clear Interrupt Pending
	//g_pPWMReg->TINT_CSTAT |= (1<<9);	// Do not use OR/AND operation on TINTC_CSTAT
	g_pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(g_pPWMReg->TINT_CSTAT) | TIMER4_PENDING_CLEAR;

	// Update high resolution counter
	g_oalTimer.curCounts += g_oalTimer.countsPerSysTick;

	// Update the millisecond counter
	CurMSec += g_oalTimer.msecPerSysTick;

	// Reschedule?
	if ((int)(CurMSec - dwReschedTime) >= 0) SysIntr = SYSINTR_RESCHED;

#ifdef OAL_ILTIMING
	if (g_oalILT.active)
	{
		if (--g_oalILT.counter == 0)
		{
			SysIntr = SYSINTR_TIMING;
			g_oalILT.counter = g_oalILT.counterSet;
			g_oalILT.isrTime2 = OALTimerCountsSinceSysTick();
		}
	}
#endif

		// Heart Beat LED
		if (++HeartBeatCnt >= 1000)
		{
			HeartBeatCnt = 0;
			HeartBeatStat ^= 1;

			if (HeartBeatStat)
			{
				g_pGPIOReg->GPNDAT &= ~(1<<12);
			}
			else
			{
				g_pGPIOReg->GPNDAT |=  (1<<12);
			}
		}


#ifdef DVS_EN
	UpdateDVS();
#endif

	return SysIntr;
}


//------------------------------------------------------------------------------
//
//  Function: OALTimerCountsSinceSysTick
//
//  This function return count of hi res ticks since system tick.
//
//  Timer 4 counts down, so we should substract actual value from
//  system tick period.
//
INT32 OALTimerCountsSinceSysTick()
{
    INT32 TimerCountsSinceSysTick = (g_oalTimer.countsPerSysTick - (g_pPWMReg->TCNTO4)) - g_PreSinceSysTick;
    if (TimerCountsSinceSysTick < 0)
        TimerCountsSinceSysTick += g_oalTimer.countsPerSysTick;
    return TimerCountsSinceSysTick;
}

//------------------------------------------------------------------------------
//
//  Function: OALTimerUpdate
//
//  This function is called to change length of actual system timer period.
//  If end of actual period is closer than margin period isn't changed (so
//  original period elapse). Function returns time which already expires
//  in new period length units. If end of new period is closer to actual time
//  than margin period end is shifted by margin (but next period should fix
//  this shift - this is reason why OALTimerRecharge doesn't read back
//  compare register and it uses saved value instead).
//
UINT32 OALTimerUpdate(UINT32 period, UINT32 margin)
{
#if	0	// Fixed Tick do not Update Timer
	UINT32 tcon, ret;

	ret = OALTimerCountsSinceSysTick();

	OUTREG32(&g_pPWMRegs->TCNTB4, period);
	tcon = INREG32(&g_pPWMRegs->TCON) & ~(0x0F << 20);
	OUTREG32(&g_pPWMRegs->TCON, tcon | (0x2 << 20) );
	OUTREG32(&g_pPWMRegs->TCON, tcon | (0x5 << 20) );

	return (ret);
#else
	return 0;
#endif
}

//------------------------------------------------------------------------------
//
//  Function:   OEMIdle
//
//  This Idle function implements a busy idle. It is intend to be used only
//  in development (when CPU doesn't support idle mode it is better to stub
//  OEMIdle function instead use this busy loop). The busy wait is cleared by
//  an interrupt from interrupt handler setting the g_oalLastSysIntr.
//
void OEMIdle(DWORD idleParam)
{
    UINT32 baseMSec;
    INT32 usedCounts, idleCounts;
    ULARGE_INTEGER idle;

    // Get current system timer counter
    baseMSec = CurMSec;

    // Find how many hi-res ticks was already used
    usedCounts = OALTimerCountsSinceSysTick();

    if (usedCounts + MIN_IDLE_COUNT >= (INT32)g_oalTimer.countsPerSysTick)
    {
        // Abandon Idle
        return;
    }


#ifdef DVS_EN
    PerformDVS();
#endif

    // Move SoC/CPU to idle mode
    OALCPUIdle();

    // Get real idle value. If result is negative we didn't idle at all.
    idleCounts = OALTimerCountsSinceSysTick() - usedCounts;

    // Get real idle value. If result is negative, idle period is laid across the Time quantum
    idleCounts = (idleCounts >= 0) ? (idleCounts) : (idleCounts + g_oalTimer.countsPerSysTick);

    // Update idle counters
    idle.LowPart = curridlelow;
    idle.HighPart = curridlehigh;
    idle.QuadPart += idleCounts;
    curridlelow  = idle.LowPart;
    curridlehigh = idle.HighPart;

#ifdef DVS_EN
    SetCurrentIdleTime((DWORD)(idle.QuadPart/idleconv));
#endif
}

//------------------------------------------------------------------------------
//
//  Function:   OALCPUIdle
//
//  This Idle function implements a busy idle. It is intend to be used only
//  in development (when CPU doesn't support idle mode it is better to stub
//  OEMIdle function instead use this busy loop). The busy wait is cleared by
//  an interrupt from interrupt handler setting the g_oalLastSysIntr.
//
//
VOID OALCPUIdle()
{
	g_pGPIOReg->GPNDAT |= (1<<15);

	g_pSysConReg->PWR_CFG = (g_pSysConReg->PWR_CFG & ~(0x3<<5)) | (0x1<<5);	// Enter IDLE mode

	System_WaitForInterrupt();	// Enter ARM core to WaitForInterrupt

	g_pSysConReg->PWR_CFG &= ~(0x1<<5);	// Clear IDLE mode

	g_pGPIOReg->GPNDAT &= ~(1<<15);
}
