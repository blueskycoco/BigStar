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
// NOTE: stubs are being used - this isn't done

#include <windows.h>
#include <bsp.h>
#include <pmplatform.h>

//#define SLEEP_AGING_TEST
#define REMOVE_KBD_WAKEUP_SRC 1
// Vector Address Table Initialize Function in "\common\intr\intr.c"
extern void VICTableInit(void);

static void ControlOTGCLK(BOOL OnOff);

static void Delay(UINT32 count)
{
	volatile int i, j = 0;
	volatile static int loop = S3C6410_ACLK/100000;

	for(;count > 0;count--)
		for(i=0;i < loop; i++) { j++; }
}

#ifdef DVS_EN
// DVS control function in "\common\timer\dvs.c"
extern void ChangeDVSLevel(SYSTEM_ACTIVE_LEVEL eLevel);
#endif


static UINT32 g_aSleepSave_VIC[100];
static UINT32 g_aSleepSave_GPIO[100];
static UINT32 g_aSleepSave_SysCon[100];
static UINT32 g_aSleepSave_DMACon0[100];
static UINT32 g_aSleepSave_DMACon1[100];
static UINT32 g_aSleepSave_SDMACon0[100];

static UINT32 g_LastWakeupStatus = 0;

static void OEMInitializeSystemTimer(UINT32 msecPerSysTick, UINT32 countsPerMSec, UINT32 countsMargin)
{
	UINT32 countsPerSysTick;
	volatile S3C6410_PWM_REG *pPWMReg;

	// Validate Input parameters
	countsPerSysTick = msecPerSysTick * countsPerMSec;

	// Initialize S3C6410 Timer
	pPWMReg = (S3C6410_PWM_REG*)OALPAtoUA(S3C6410_BASE_REG_PA_PWM);

	// Set Prescaler 1 (Timer2,3,4)
	pPWMReg->TCFG0 = (pPWMReg->TCFG0 & ~(0xff<<8)) | ((SYS_TIMER_PRESCALER-1)<<8);

	// Set Divider MUX for Timer4
	switch(SYS_TIMER_DIVIDER)
	{
	case 1:
		pPWMReg->TCFG1 = (pPWMReg->TCFG1 & ~(0xf<<16)) | (0<<16);
		break;
	case 2:
		pPWMReg->TCFG1 = (pPWMReg->TCFG1 & ~(0xf<<16)) | (1<<16);
		break;
	case 4:
		pPWMReg->TCFG1 = (pPWMReg->TCFG1 & ~(0xf<<16)) | (2<<16);
		break;
	case 8:
		pPWMReg->TCFG1 = (pPWMReg->TCFG1 & ~(0xf<<16)) | (3<<16);
		break;
	case 16:
		pPWMReg->TCFG1 = (pPWMReg->TCFG1 & ~(0xf<<16)) | (4<<16);
		break;
	default:
		pPWMReg->TCFG1 = (pPWMReg->TCFG1 & ~(0xf<<16)) | (0<<16);
		break;
	}

	// Set Timer4 Count Buffer Register
	pPWMReg->TCNTB4 = countsPerSysTick;

	// Timer4 Clear Interrupt Pending
	//pPWMReg->TINT_CSTAT |= (1<<9);	// Do not use OR/AND operation on TINTC_CSTAT
	pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(pPWMReg->TINT_CSTAT) | TIMER4_PENDING_CLEAR;

	// Timer4 Interrupt Enable
	//pPWMReg->TINT_CSTAT |= (1<<4);	// Do not use OR/AND operation on TINTC_CSTAT
	pPWMReg->TINT_CSTAT = TINT_CSTAT_INTMASK(pPWMReg->TINT_CSTAT) | TIMER4_INTERRUPT_ENABLE;

	// Start Timer4 in Auto Reload mode (Fixed Tick!!!)
	pPWMReg->TCON &= ~(0x7<<20);

	pPWMReg->TCON |= (1<<21);			// Update TCNTB4
	pPWMReg->TCON &= ~(1<<21);

	pPWMReg->TCON |= (1<<22)|(1<<20);	// Auto-reload Mode, Timer4 Start
}

static void S3C6410_WakeUpSource_Configure(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg;
	volatile S3C6410_GPIO_REG *pGPIOReg;
#ifdef	SLEEP_AGING_TEST
	volatile S3C6410_RTC_REG *pRTCReg;
#endif

	pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
#ifdef	SLEEP_AGING_TEST
	pRTCReg = (S3C6410_RTC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_RTC, FALSE);
#endif

	// Wake Up Source Mask
	pSysConReg->PWR_CFG = (pSysConReg->PWR_CFG & ~(0x1FF80))
							|(1<<16)	// MMC2		(Disabled, Only for Stop)
							|(1<<15)	// MMC1		(Disabled, Only for Stop)
							|(1<<14)	// MMC0		(Disabled, Only for Stop)
							|(1<<13)	// HSI		(Disabled)
							|(1<<12)	// Touch		(Disabled, Only for Stop)
#ifdef	SLEEP_AGING_TEST
							|(0<<11)	// RTC Tick	(Enabled)
#else
							|(1<<11)	// RTC Tick	(Disabled)
#endif
							|(0<<10)	// RTC ALARM	(Enabled)
							|(1<<9)		// MSM		(Disabled)
#ifdef	REMOVE_KBD_WAKEUP_SRC		// Remove Keypad I/F from wake up source. if keyboard driver is not loaded
							|(1<<8)		// Keypad		(Disabled)
#else
							|(0<<8)		// Keypad		(Enabled)
#endif
							|(1<<7);		// Battery Fault	(Disabled)

	//-----------------
	// External Interrupt
	//-----------------
	// Power Button EINT[11] (GPN[11] is Retention Port)
	pGPIOReg->GPNCON = (pGPIOReg->GPNCON & ~(0x3<<22)) | (0x2<<22);	// GPN[11] as EINT[11]

	pSysConReg->EINT_MASK = 0x0FFFFFFF;		// Mask All EINT Wake Up Source at Sleep
	pSysConReg->EINT_MASK &= ~(1<<11);		// Enable EINT[11] as Wake Up Source at Sleep

	//-----------------
	// Keypad I/F
	//-----------------

	//-----------------
	// RTC Tick
	//-----------------
#ifdef	SLEEP_AGING_TEST
	pRTCReg->TICCNT = 0x4000;	// 0.5 sec @ 32.768KHz
	pRTCReg->RTCCON |= (1<<8);	// Tick Timer Enable
#endif

	// TODO: WAKEUP_STAT is set before Sleep ???
	// Clear All Wake Up Status bits
	pSysConReg->WAKEUP_STAT = 0xfff;
}

static void S3C6410_WakeUpSource_Detect(void)
{
	volatile S3C6410_SYSCON_REG *pSysConReg;
	volatile S3C6410_GPIO_REG *pGPIOReg;
#ifdef	SLEEP_AGING_TEST
	volatile S3C6410_RTC_REG *pRTCReg;
#endif

	pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
#ifdef	SLEEP_AGING_TEST
	pRTCReg = (S3C6410_RTC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_RTC, FALSE);
#endif

	g_LastWakeupStatus = pSysConReg->WAKEUP_STAT;

	switch(g_LastWakeupStatus)
	{
	case 0x1:	// External Interrupt
		//pSysConReg->WAKEUP_STAT = 0x1;	// Clear EINT_WAKEUP State Bit
		if (pGPIOReg->EINT0PEND&(1<<11))		// Power Button : EINT[11]
		{
			g_oalWakeSource = SYSWAKE_POWER_BUTTON;	// OEMWAKE_EINT11;
			pGPIOReg->EINT0PEND = (1<<11);	// Clear Pending (Power Button Driver No Need to Handle Wake Up Interrupt)
		}
		else
		{
			// TODO: To Be implemented...
			g_oalWakeSource = SYSWAKE_UNKNOWN;
		}
		break;
	case 0x2:	// RTC Alarm
		g_oalWakeSource = OEMWAKE_RTC_ALARM;
		//pSysConReg->WAKEUP_STAT = 0x2;		// Clear RTC_ALARM_WAKEUP State Bit
		break;
	case 0x4:	// RTC Tick
		g_oalWakeSource = OEMWAKE_RTC_TICK;
		//pSysConReg->WAKEUP_STAT = 0x4;		// Clear RTC_TICK_WAKEUP State Bit
#ifdef	SLEEP_AGING_TEST
			pRTCReg->RTCCON &= ~(1<<8);	// Tick Timer Disable
			pRTCReg->INTP = 0x1;				// Clear RTC Tick Interrupt Pending
#endif
		break;
	case 0x10:	// Keypad
		g_oalWakeSource = OEMWAKE_KEYPAD;
		//pSysConReg->WAKEUP_STAT = 0x10;	// Clear KEY_WAKEUP State Bit
		break;
	case 0x20:	// MSM
		g_oalWakeSource = OEMWAKE_MSM;
		//pSysConReg->WAKEUP_STAT = 0x20;	// Clear MSM_WAKEUP State Bit
		break;
	case 0x40:	// BATFLT
		g_oalWakeSource = OEMWAKE_BATTERY_FAULT;
		//pSysConReg->WAKEUP_STAT = 0x40;	// Clear BATFLT_WAKEUP State Bit
		break;
	case 0x80:	// WRESET
		g_oalWakeSource = OEMWAKE_WARM_RESET;
		//pSysConReg->WAKEUP_STAT = 0x80;	// Clear WRESET_WAKEUP State Bit
		break;
	case 0x100:	// HSI
		g_oalWakeSource = OEMWAKE_HSI;
		//pSysConReg->WAKEUP_STAT = 0x100;	// Clear HSI_WAKEUP State Bit
		break;
	default:		// Unknown or Multiple Wakeup Source ???
		g_oalWakeSource = SYSWAKE_UNKNOWN;
		break;
	}

	// Clear All Wake Up Status bits
	pSysConReg->WAKEUP_STAT = 0xfff;

	// EINT_MASK in SysCon Also Effective in Normal State
	// Unmask all bit for External Interrupt at Normal Mode
	pSysConReg->EINT_MASK = 0x0;
}

static void S3C6410_SaveState_VIC(void *pVIC0, void *pVIC1, UINT32 *pBuffer)
{
	volatile S3C6410_VIC_REG *pVIC0Reg;
	volatile S3C6410_VIC_REG *pVIC1Reg;

	pVIC0Reg = (S3C6410_VIC_REG *)pVIC0;
	pVIC1Reg = (S3C6410_VIC_REG *)pVIC1;

	*pBuffer++ = pVIC0Reg->VICINTSELECT;
	*pBuffer++ = pVIC0Reg->VICINTENABLE;
	*pBuffer++ = pVIC0Reg->VICSOFTINT;
	*pBuffer++ = pVIC1Reg->VICINTSELECT;
	*pBuffer++ = pVIC1Reg->VICINTENABLE;
	*pBuffer++ = pVIC1Reg->VICSOFTINT;

	// Do not Save VICVECTADDRXX
	// Reinitialize Vector Address Table with VICTableInit()
}

static void S3C6410_RestoreState_VIC(void *pVIC0, void *pVIC1, UINT32 *pBuffer)
{
	volatile S3C6410_VIC_REG *pVIC0Reg;
	volatile S3C6410_VIC_REG *pVIC1Reg;

	pVIC0Reg = (S3C6410_VIC_REG *)pVIC0;
	pVIC1Reg = (S3C6410_VIC_REG *)pVIC1;

	pVIC0Reg->VICINTSELECT = *pBuffer++;
	pVIC0Reg->VICINTENABLE = *pBuffer++;
	pVIC0Reg->VICSOFTINT = *pBuffer++;
	pVIC1Reg->VICINTSELECT = *pBuffer++;
	pVIC1Reg->VICINTENABLE = *pBuffer++;
	pVIC1Reg->VICSOFTINT = *pBuffer++;

	// Do not Restore VICVECTADDRXX
	// Reinitialize Vector Address Table with VICTableInit()
	VICTableInit();
}

static void S3C6410_SaveState_GPIO(void *pGPIO, UINT32 *pBuffer)
{
	volatile S3C6410_GPIO_REG *pGPIOReg;

	pGPIOReg = (S3C6410_GPIO_REG *)pGPIO;

	// GPIO A
	*pBuffer++ = pGPIOReg->GPACON;		// 000
	*pBuffer++ = pGPIOReg->GPADAT;		// 004
	*pBuffer++ = pGPIOReg->GPAPUD;		// 008
	//GPACONSLP;						// 00c
	//GPAPUDSLP;						// 010

	// GPIO B
	*pBuffer++ = pGPIOReg->GPBCON;		// 020
	*pBuffer++ = pGPIOReg->GPBDAT;		// 024
	*pBuffer++ = pGPIOReg->GPBPUD;		// 028
	//GPBCONSLP;						// 02c
	//GPBPUDSLP;						// 030

	// GPIO C
	*pBuffer++ = pGPIOReg->GPCCON;		// 040
	*pBuffer++ = pGPIOReg->GPCDAT;		// 044
	*pBuffer++ = pGPIOReg->GPCPUD;		// 048
	//GPCCONSLP;						// 04c
	//GPCPUDSLP;						// 050

	// GPIO D
	*pBuffer++ = pGPIOReg->GPDCON;		// 060
	*pBuffer++ = pGPIOReg->GPDDAT;		// 064
	*pBuffer++ = pGPIOReg->GPDPUD;		// 068
	//GPDCONSLP;						// 06c
	//GPDPUDSLP;						// 070

	// GPIO E
	*pBuffer++ = pGPIOReg->GPECON;		// 080
	*pBuffer++ = pGPIOReg->GPEDAT;		// 084
	*pBuffer++ = pGPIOReg->GPEPUD;		// 088
	//GPECONSLP;						// 08c
	//GPEPUDSLP;						// 090

	// GPIO F
	*pBuffer++ = pGPIOReg->GPFCON;		// 0a0
	*pBuffer++ = pGPIOReg->GPFDAT;		// 0a4
	*pBuffer++ = pGPIOReg->GPFPUD;		// 0a8
	//GPFCONSLP;						// 0ac
	//GPFPUDSLP;						// 0b0

	// GPIO G
	*pBuffer++ = pGPIOReg->GPGCON;		// 0c0
	*pBuffer++ = pGPIOReg->GPGDAT;		// 0c4
	*pBuffer++ = pGPIOReg->GPGPUD;		// 0c8
	//GPGCONSLP;						// 0cc
	//GPGPUDSLP;						// 0d0

	// GPIO H
	*pBuffer++ = pGPIOReg->GPHCON0;     // 0e0
	*pBuffer++ = pGPIOReg->GPHCON1;	    // 0e4
	*pBuffer++ = pGPIOReg->GPHDAT;		// 0e8
	*pBuffer++ = pGPIOReg->GPHPUD;		// 0ec
	//GPHCONSLP;						// 0f0
	//GPHPUDSLP;						// 0f4

	// GPIO I
	*pBuffer++ = pGPIOReg->GPICON;		// 100
	*pBuffer++ = pGPIOReg->GPIDAT;		// 104
	*pBuffer++ = pGPIOReg->GPIPUD;		// 108
	//GPICONSLP;						// 10c
	//GPIPUDSLP;						// 110

	// GPIO J
	*pBuffer++ = pGPIOReg->GPJCON;		// 120
	*pBuffer++ = pGPIOReg->GPJDAT;		// 124
	*pBuffer++ = pGPIOReg->GPJPUD;		// 128
	//GPJCONSLP;						// 12c
	//GPJPUDSLP;						// 130

	// GPIO K
	*pBuffer++ = pGPIOReg->GPKCON0;	    // 800
	*pBuffer++ = pGPIOReg->GPKCON1;	    // 804
	*pBuffer++ = pGPIOReg->GPKDAT;		// 808
	*pBuffer++ = pGPIOReg->GPKPUD;		// 80c

	// GPIO L
	*pBuffer++ = pGPIOReg->GPLCON0;	    // 810
	*pBuffer++ = pGPIOReg->GPLCON1;	    // 814
	*pBuffer++ = pGPIOReg->GPLDAT;		// 818
	*pBuffer++ = pGPIOReg->GPLPUD;		// 81c

	// GPIO M
	*pBuffer++ = pGPIOReg->GPMCON;		// 820
	*pBuffer++ = pGPIOReg->GPMDAT;		// 824
	*pBuffer++ = pGPIOReg->GPMPUD;		// 828

	// GPIO N
	*pBuffer++ = pGPIOReg->GPNCON;		// 830
	*pBuffer++ = pGPIOReg->GPNDAT;		// 834
	*pBuffer++ = pGPIOReg->GPNPUD;		// 838

	// GPIO O
	*pBuffer++ = pGPIOReg->GPOCON;		// 140
	*pBuffer++ = pGPIOReg->GPODAT;		// 144
	*pBuffer++ = pGPIOReg->GPOPUD;		// 148
	//GPOCONSLP;						// 14c
	//GPOPUDSLP;						// 150

	// GPIO P
	*pBuffer++ = pGPIOReg->GPPCON;		// 160
	*pBuffer++ = pGPIOReg->GPPDAT;		// 164
	*pBuffer++ = pGPIOReg->GPPPUD;		// 168
	//GPPCONSLP;						// 16c
	//GPPPUDSLP;						// 170

	// GPIO Q
	*pBuffer++ = pGPIOReg->GPQCON;		// 180
	*pBuffer++ = pGPIOReg->GPQDAT;		// 184
	*pBuffer++ = pGPIOReg->GPQPUD;		// 188
	//GPQCONSLP;						// 18c
	//GPQPUDSLP;						// 190

	// Special Port Control
	*pBuffer++ = pGPIOReg->SPCON;		// 1a0
	//SPCONSLP;							// 880	// Configure for Sleep Mode

	// Memory Port Control
	*pBuffer++ = pGPIOReg->MEM0CONSTOP;	// 1b0
	*pBuffer++ = pGPIOReg->MEM1CONSTOP;	// 1b4

	*pBuffer++ = pGPIOReg->MEM0CONSLP0;	// 1c0
	*pBuffer++ = pGPIOReg->MEM0CONSLP1;	// 1c4
	*pBuffer++ = pGPIOReg->MEM1CONSLP;	// 1c8

	*pBuffer++ = pGPIOReg->MEM0DRVCON;	// 1d0
	*pBuffer++ = pGPIOReg->MEM1DRVCON;	// 1d4

	// External Interrupt
	*pBuffer++ = pGPIOReg->EINT12CON;	// 200
	*pBuffer++ = pGPIOReg->EINT34CON;	// 204
	*pBuffer++ = pGPIOReg->EINT56CON;	// 208
	*pBuffer++ = pGPIOReg->EINT78CON;	// 20c
	*pBuffer++ = pGPIOReg->EINT9CON;	// 210

	*pBuffer++ = pGPIOReg->EINT12FLTCON;	// 220
	*pBuffer++ = pGPIOReg->EINT34FLTCON;	// 224
	*pBuffer++ = pGPIOReg->EINT56FLTCON;	// 228
	*pBuffer++ = pGPIOReg->EINT78FLTCON;	// 22c
	*pBuffer++ = pGPIOReg->EINT9FLTCON;		// 230

	*pBuffer++ = pGPIOReg->EINT12MASK;		// 240
	*pBuffer++ = pGPIOReg->EINT34MASK;		// 244
	*pBuffer++ = pGPIOReg->EINT56MASK;		// 248
	*pBuffer++ = pGPIOReg->EINT78MASK;		// 24c
	*pBuffer++ = pGPIOReg->EINT9MASK;		// 250

	//EINT12PEND;							// 260	// Do not Save Pending Bit
	//EINT34PEND;							// 264
	//EINT56PEND;							// 268
	//EINT78PEND;							// 26c
	//EINT9PEND;							// 270

	*pBuffer++ = pGPIOReg->PRIORITY;		// 280
	//SERVICE;								// 284	// Do not Save Read Only Register (But Check before enter sleep...)
	//SERVICEPEND;							// 288	// Do not Save Pending Bit

	// External Interrupt Group 0
	*pBuffer++ = pGPIOReg->EINT0CON0;		// 900
	*pBuffer++ = pGPIOReg->EINT0CON1;		// 904

	*pBuffer++ = pGPIOReg->EINT0FLTCON0;	// 910
	*pBuffer++ = pGPIOReg->EINT0FLTCON1;	// 914
	*pBuffer++ = pGPIOReg->EINT0FLTCON2;	// 918
	*pBuffer++ = pGPIOReg->EINT0FLTCON3;	// 91c

	*pBuffer++ = pGPIOReg->EINT0MASK;		// 920
	//EINT0PEND;							// 924	// Do not Save Pending Bit

	// Sleep Mode Pad Configure Register
	//SLPEN;								// 930	// Configure for Sleep Mode
}

static void S3C6410_RestoreState_GPIO(void *pGPIO, UINT32 *pBuffer)
{
	volatile S3C6410_GPIO_REG *pGPIOReg;

	pGPIOReg = (S3C6410_GPIO_REG *)pGPIO;

	// GPIO A
	pGPIOReg->GPACON = *pBuffer++;		// 000
	pGPIOReg->GPADAT = *pBuffer++;		// 004
	pGPIOReg->GPAPUD = *pBuffer++;		// 008
	//GPACONSLP;						// 00c
	//GPAPUDSLP;						// 010

	// GPIO B
	pGPIOReg->GPBCON = *pBuffer++;		// 020
	pGPIOReg->GPBDAT = *pBuffer++;		// 024
	pGPIOReg->GPBPUD = *pBuffer++;		// 028
	//GPBCONSLP;						// 02c
	//GPBPUDSLP;						// 030

	// GPIO C
	pGPIOReg->GPCCON = *pBuffer++;		// 040
	pGPIOReg->GPCDAT = *pBuffer++;		// 044
	pGPIOReg->GPCPUD = *pBuffer++;		// 048
	//GPCCONSLP;						// 04c
	//GPCPUDSLP;						// 050

	// GPIO D
	pGPIOReg->GPDCON = *pBuffer++;		// 060
	pGPIOReg->GPDDAT = *pBuffer++;		// 064
	pGPIOReg->GPDPUD = *pBuffer++;		// 068
	//GPDCONSLP;						// 06c
	//GPDPUDSLP;						// 070

	// GPIO E
	pGPIOReg->GPECON = *pBuffer++;		// 080
	pGPIOReg->GPEDAT = *pBuffer++;		// 084
	pGPIOReg->GPEPUD = *pBuffer++;		// 088
	//GPECONSLP;						// 08c
	//GPEPUDSLP;						// 090

	// GPIO F
	pGPIOReg->GPFCON = *pBuffer++;		// 0a0
	pGPIOReg->GPFDAT = *pBuffer++;		// 0a4
	pGPIOReg->GPFPUD = *pBuffer++;		// 0a8
	//GPFCONSLP;						// 0ac
	//GPFPUDSLP;						// 0b0

	// GPIO G
	pGPIOReg->GPGCON = *pBuffer++;		// 0c0
	pGPIOReg->GPGDAT = *pBuffer++;		// 0c4
	pGPIOReg->GPGPUD = *pBuffer++;		// 0c8
	//GPGCONSLP;						// 0cc
	//GPGPUDSLP;						// 0d0

	// GPIO H
	pGPIOReg->GPHCON0 = *pBuffer++;	    // 0e0
	pGPIOReg->GPHCON1 = *pBuffer++;	    // 0e4
	pGPIOReg->GPHDAT = *pBuffer++;		// 0e8
	pGPIOReg->GPHPUD = *pBuffer++;		// 0ec
	//GPHCONSLP;						// 0f0
	//GPHPUDSLP;						// 0f4

	// GPIO I
	pGPIOReg->GPICON = *pBuffer++;		// 100
	pGPIOReg->GPIDAT = *pBuffer++;		// 104
	pGPIOReg->GPIPUD = *pBuffer++;		// 108
	//GPICONSLP;						// 10c
	//GPIPUDSLP;						// 110

	// GPIO J
	pGPIOReg->GPJCON = *pBuffer++;		// 120
	pGPIOReg->GPJDAT = *pBuffer++;		// 124
	pGPIOReg->GPJPUD = *pBuffer++;		// 128
	//GPJCONSLP;						// 12c
	//GPJPUDSLP;						// 130

	// GPIO K
	pGPIOReg->GPKCON0 = *pBuffer++;	    // 800
	pGPIOReg->GPKCON1 = *pBuffer++;	    // 804
	pGPIOReg->GPKDAT = *pBuffer++;		// 808
	pGPIOReg->GPKPUD = *pBuffer++;		// 80c

	// GPIO L
	pGPIOReg->GPLCON0 = *pBuffer++;	    // 810
	pGPIOReg->GPLCON1 = *pBuffer++;	    // 814
	pGPIOReg->GPLDAT = *pBuffer++;		// 818
	pGPIOReg->GPLPUD = *pBuffer++;		// 81c

	// GPIO M
	pGPIOReg->GPMCON = *pBuffer++;		// 820
	pGPIOReg->GPMDAT = *pBuffer++;		// 824
	pGPIOReg->GPMPUD = *pBuffer++;		// 828

	// GPIO N
	pGPIOReg->GPNCON = *pBuffer++;		// 830
	pGPIOReg->GPNDAT = *pBuffer++;		// 834
	pGPIOReg->GPNPUD = *pBuffer++;		// 838

	// GPIO O
	pGPIOReg->GPOCON = *pBuffer++;		// 140
	pGPIOReg->GPODAT = *pBuffer++;		// 144
	pGPIOReg->GPOPUD = *pBuffer++;		// 148
	//GPOCONSLP;						// 14c
	//GPOPUDSLP;						// 150

	// GPIO P
	pGPIOReg->GPPCON = *pBuffer++;		// 160
	pGPIOReg->GPPDAT = *pBuffer++;		// 164
	pGPIOReg->GPPPUD = *pBuffer++;		// 168
	//GPPCONSLP;						// 16c
	//GPPPUDSLP;						// 170

	// GPIO Q
	pGPIOReg->GPQCON = *pBuffer++;		// 180
	pGPIOReg->GPQDAT = *pBuffer++;		// 184
	pGPIOReg->GPQPUD = *pBuffer++;		// 188
	//GPQCONSLP;						// 18c
	//GPQPUDSLP;						// 190

	// Special Port Control
	pGPIOReg->SPCON = *pBuffer++;		// 1a0
	//SPCONSLP;							// 880	// Configure for Sleep Mode

	// Memory Port Control
	pGPIOReg->MEM0CONSTOP = *pBuffer++;	// 1b0
	pGPIOReg->MEM1CONSTOP = *pBuffer++;	// 1b4

	pGPIOReg->MEM0CONSLP0 = *pBuffer++;	// 1c0
	pGPIOReg->MEM0CONSLP1 = *pBuffer++;	// 1c4
	pGPIOReg->MEM1CONSLP = *pBuffer++;	// 1c8

	pGPIOReg->MEM0DRVCON = *pBuffer++;	// 1d0
	pGPIOReg->MEM1DRVCON = *pBuffer++;	// 1d4

	// External Interrupt
	pGPIOReg->EINT12CON = *pBuffer++;		// 200
	pGPIOReg->EINT34CON = *pBuffer++;		// 204
	pGPIOReg->EINT56CON = *pBuffer++;		// 208
	pGPIOReg->EINT78CON = *pBuffer++;		// 20c
	pGPIOReg->EINT9CON = *pBuffer++;		// 210

	pGPIOReg->EINT12FLTCON = *pBuffer++;	// 220
	pGPIOReg->EINT34FLTCON = *pBuffer++;	// 224
	pGPIOReg->EINT56FLTCON = *pBuffer++;	// 228
	pGPIOReg->EINT78FLTCON = *pBuffer++;	// 22c
	pGPIOReg->EINT9FLTCON = *pBuffer++;		// 230

	pGPIOReg->EINT12MASK = *pBuffer++;		// 240
	pGPIOReg->EINT34MASK = *pBuffer++;		// 244
	pGPIOReg->EINT56MASK = *pBuffer++;		// 248
	pGPIOReg->EINT78MASK = *pBuffer++;		// 24c
	pGPIOReg->EINT9MASK = *pBuffer++;		// 250

	//EINT12PEND;							// 260	// Do not Save Pending Bit
	//EINT34PEND;							// 264
	//EINT56PEND;							// 268
	//EINT78PEND;							// 26c
	//EINT9PEND;							// 270

	pGPIOReg->PRIORITY = *pBuffer++;		// 280
	//SERVICE;								// 284	// Do not Save Read Only Register (But Check before enter sleep...)
	//SERVICEPEND;							// 288	// Do not Save Pending Bit

	// External Interrupt Group 0
	pGPIOReg->EINT0CON0 = *pBuffer++;		// 900
	pGPIOReg->EINT0CON1 = *pBuffer++;		// 904

	pGPIOReg->EINT0FLTCON0 = *pBuffer++;	// 910
	pGPIOReg->EINT0FLTCON1 = *pBuffer++;	// 914
	pGPIOReg->EINT0FLTCON2 = *pBuffer++;	// 918
	pGPIOReg->EINT0FLTCON3 = *pBuffer++;	// 91c

	pGPIOReg->EINT0MASK = *pBuffer++;		// 920
	//EINT0PEND;							// 924	// Do not Save Pending Bit

	// Sleep Mode Pad Configure Register
	//SLPEN;								// 930	// Configure for Sleep Mode

}

static void S3C6410_SaveState_SysCon(void *pSysCon, UINT32 *pBuffer)
{
	volatile S3C6410_SYSCON_REG *pSysConReg;

	pSysConReg = (S3C6410_SYSCON_REG *)pSysCon;

	//APLL_LOCK;		// Reconfiguration
	//MPLL_LOCK;		// Reconfiguration
	//EPLL_LOCK;		// Reconfiguration
	//APLL_CON;			// Reconfiguration
	//MPLL_CON;			// Reconfiguration
	//EPLL_CON0;		// Reconfiguration
	//EPLL_CON1;		// Reconfiguration

	*pBuffer++ = pSysConReg->CLK_SRC;
	*pBuffer++ = pSysConReg->CLK_DIV0;
	*pBuffer++ = pSysConReg->CLK_DIV1;
	*pBuffer++ = pSysConReg->CLK_DIV2;
	*pBuffer++ = pSysConReg->CLK_OUT;

	*pBuffer++ = pSysConReg->HCLK_GATE;
	*pBuffer++ = pSysConReg->PCLK_GATE;
	*pBuffer++ = pSysConReg->SCLK_GATE;

	*pBuffer++ = pSysConReg->AHB_CON0;
	*pBuffer++ = pSysConReg->AHB_CON1;
	*pBuffer++ = pSysConReg->AHB_CON2;
	*pBuffer++ = pSysConReg->SDMA_SEL;

	#if   (CPU_REVISION == EVT0)
	//SW_RST;			// Reset Trigger
	#endif
	//SYS_ID;			// Read Only

	*pBuffer++ = pSysConReg->MEM_SYS_CFG;
	#if   (CPU_REVISION == EVT0)
	*pBuffer++ = pSysConReg->QOS_OVERRIDE0;
	#endif
	*pBuffer++ = pSysConReg->QOS_OVERRIDE1;
	//MEM_CFG_STAT;	// Read Only

	//PWR_CFG;			// Retension
	//EINT_MASK;		// Retension
	*pBuffer++ = pSysConReg->NORMAL_CFG;	// Retension, But H/W Problem
	//STOP_CFG;			// Retension
	//SLEEP_CFG;		// Retension

	//OSC_FREQ;			// Retension
	//OSC_STABLE;		// Retension
	//PWR_STABLE;		// Retension
	//FPC_STABLE;		// Retension
	//MTC_STABLE;		// Retension

	//OTHERS;			// Retension
	//RST_STAT;			// Retension, Read Only
	//WAKEUP_STAT;		// Retension
	//BLK_PWR_STAT;		// Retension, Read Only

	//INFORM0;			// Retension
	//INFORM1;			// Retension
	//INFORM2;			// Retension
	//INFORM3;			// Retension
}

static void S3C6410_RestoreState_SysCon(void *pSysCon, UINT32 *pBuffer)
{
	volatile S3C6410_SYSCON_REG *pSysConReg;

	pSysConReg = (S3C6410_SYSCON_REG *)pSysCon;
	//APLL_LOCK;		// Reconfiguration
	//MPLL_LOCK;		// Reconfiguration
	//EPLL_LOCK;		// Reconfiguration
	//APLL_CON;			// Reconfiguration
	//MPLL_CON;			// Reconfiguration
	//EPLL_CON0;		// Reconfiguration
	//EPLL_CON1;		// Reconfiguration

	pSysConReg->CLK_SRC = *pBuffer++;
	pSysConReg->CLK_DIV0 = *pBuffer++;
	pSysConReg->CLK_DIV1 = *pBuffer++;
	pSysConReg->CLK_DIV2 = *pBuffer++;
	pSysConReg->CLK_OUT = *pBuffer++;

	pSysConReg->HCLK_GATE = *pBuffer++;
	pSysConReg->PCLK_GATE = *pBuffer++;
	pSysConReg->SCLK_GATE = *pBuffer++;

	pSysConReg->AHB_CON0 = *pBuffer++;
	pSysConReg->AHB_CON1 = *pBuffer++;
	pSysConReg->AHB_CON2 = *pBuffer++;
	pSysConReg->SDMA_SEL = *pBuffer++;

	#if   (CPU_REVISION == EVT0)
	//SW_RST;			// Reset Trigger
	#endif
	//SYS_ID;			// Read Only

	pSysConReg->MEM_SYS_CFG = *pBuffer++;
	#if   (CPU_REVISION == EVT0)
	pSysConReg->QOS_OVERRIDE0 = *pBuffer++;
	#endif
	pSysConReg->QOS_OVERRIDE1 = *pBuffer++;
	//MEM_CFG_STAT;	// Read Only

	//PWR_CFG;			// Retension
	//EINT_MASK;		// Retension
	pSysConReg->NORMAL_CFG = *pBuffer++;	// Retension, But H/W Problem
	//STOP_CFG;			// Retension
	//SLEEP_CFG;		// Retension

	//OSC_FREQ;			// Retension
	//OSC_STABLE;		// Retension
	//PWR_STABLE;		// Retension
	//FPC_STABLE;		// Retension
	//MTC_STABLE;		// Retension

	//OTHERS;			// Retension
	//RST_STAT;			// Retension, Read Only
	//WAKEUP_STAT;		// Retension
	//BLK_PWR_STAT;		// Retension, Read Only

	//INFORM0;			// Retension
	//INFORM1;			// Retension
	//INFORM2;			// Retension
	//INFORM3;			// Retension
}

static void S3C6410_SaveState_DMACon(void *pDMAC, UINT32 *pBuffer)
{
	volatile S3C6410_DMAC_REG *pDMACReg;

	pDMACReg = (S3C6410_DMAC_REG *)pDMAC;

	//DMACIntStatus;		// Read-Only
	//DMACIntTCStatus;		// Read-Only
	//DMACIntTCClear;		// Clear Register
	//DMACIntErrStatus;		// Read-Only
	//DMACIntErrClear;		// Clear Register
	//DMACRawIntTCStatus;	// Read-Only
	//DMACRawIntErrStatus;	// Read-Only
	//DMACEnbldChns;		// Read-Only

	//DMACSoftBReq;			// No use
	//DMACSoftSReq;			// No use
	//DMACSoftLBReq;		// No use
	//DMACSoftLSReq;		// No use

	*pBuffer++ = pDMACReg->DMACConfiguration;
	*pBuffer++ = pDMACReg->DMACSync;

	*pBuffer++ = pDMACReg->DMACC0SrcAddr;
	*pBuffer++ = pDMACReg->DMACC0DestAddr;
	*pBuffer++ = pDMACReg->DMACC0LLI;
	*pBuffer++ = pDMACReg->DMACC0Control0;
	*pBuffer++ = pDMACReg->DMACC0Control1;
	*pBuffer++ = pDMACReg->DMACC0Configuration;

	*pBuffer++ = pDMACReg->DMACC1SrcAddr;
	*pBuffer++ = pDMACReg->DMACC1DestAddr;
	*pBuffer++ = pDMACReg->DMACC1LLI;
	*pBuffer++ = pDMACReg->DMACC1Control0;
	*pBuffer++ = pDMACReg->DMACC1Control1;
	*pBuffer++ = pDMACReg->DMACC1Configuration;

	*pBuffer++ = pDMACReg->DMACC2SrcAddr;
	*pBuffer++ = pDMACReg->DMACC2DestAddr;
	*pBuffer++ = pDMACReg->DMACC2LLI;
	*pBuffer++ = pDMACReg->DMACC2Control0;
	*pBuffer++ = pDMACReg->DMACC2Control1;
	*pBuffer++ = pDMACReg->DMACC2Configuration;

	*pBuffer++ = pDMACReg->DMACC3SrcAddr;
	*pBuffer++ = pDMACReg->DMACC3DestAddr;
	*pBuffer++ = pDMACReg->DMACC3LLI;
	*pBuffer++ = pDMACReg->DMACC3Control0;
	*pBuffer++ = pDMACReg->DMACC3Control1;
	*pBuffer++ = pDMACReg->DMACC3Configuration;

	*pBuffer++ = pDMACReg->DMACC4SrcAddr;
	*pBuffer++ = pDMACReg->DMACC4DestAddr;
	*pBuffer++ = pDMACReg->DMACC4LLI;
	*pBuffer++ = pDMACReg->DMACC4Control0;
	*pBuffer++ = pDMACReg->DMACC4Control1;
	*pBuffer++ = pDMACReg->DMACC4Configuration;

	*pBuffer++ = pDMACReg->DMACC5SrcAddr;
	*pBuffer++ = pDMACReg->DMACC5DestAddr;
	*pBuffer++ = pDMACReg->DMACC5LLI;
	*pBuffer++ = pDMACReg->DMACC5Control0;
	*pBuffer++ = pDMACReg->DMACC5Control1;
	*pBuffer++ = pDMACReg->DMACC5Configuration;

	*pBuffer++ = pDMACReg->DMACC6SrcAddr;
	*pBuffer++ = pDMACReg->DMACC6DestAddr;
	*pBuffer++ = pDMACReg->DMACC6LLI;
	*pBuffer++ = pDMACReg->DMACC6Control0;
	*pBuffer++ = pDMACReg->DMACC6Control1;
	*pBuffer++ = pDMACReg->DMACC6Configuration;

	*pBuffer++ = pDMACReg->DMACC7SrcAddr;
	*pBuffer++ = pDMACReg->DMACC7DestAddr;
	*pBuffer++ = pDMACReg->DMACC7LLI;
	*pBuffer++ = pDMACReg->DMACC7Control0;
	*pBuffer++ = pDMACReg->DMACC7Control1;
	*pBuffer++ = pDMACReg->DMACC7Configuration;
}

static void S3C6410_RestoreState_DMACon(void *pDMAC, UINT32 *pBuffer)
{
	volatile S3C6410_DMAC_REG *pDMACReg;

	pDMACReg = (S3C6410_DMAC_REG *)pDMAC;

	//DMACIntStatus;		// Read-Only
	//DMACIntTCStatus;		// Read-Only
	//DMACIntTCClear;		// Clear Register
	//DMACIntErrStatus;		// Read-Only
	//DMACIntErrClear;		// Clear Register
	//DMACRawIntTCStatus;	// Read-Only
	//DMACRawIntErrStatus;	// Read-Only
	//DMACEnbldChns;		// Read-Only

	//DMACSoftBReq;			// No use
	//DMACSoftSReq;			// No use
	//DMACSoftLBReq;		// No use
	//DMACSoftLSReq;		// No use

	pDMACReg->DMACConfiguration = *pBuffer++;
	pDMACReg->DMACSync = *pBuffer++;

	pDMACReg->DMACC0SrcAddr = *pBuffer++;
	pDMACReg->DMACC0DestAddr = *pBuffer++;
	pDMACReg->DMACC0LLI = *pBuffer++;
	pDMACReg->DMACC0Control0 = *pBuffer++;
	pDMACReg->DMACC0Control1 = *pBuffer++;
	pDMACReg->DMACC0Configuration = *pBuffer++;

	pDMACReg->DMACC1SrcAddr = *pBuffer++;
	pDMACReg->DMACC1DestAddr = *pBuffer++;
	pDMACReg->DMACC1LLI = *pBuffer++;
	pDMACReg->DMACC1Control0 = *pBuffer++;
	pDMACReg->DMACC1Control1 = *pBuffer++;
	pDMACReg->DMACC1Configuration = *pBuffer++;

	pDMACReg->DMACC2SrcAddr = *pBuffer++;
	pDMACReg->DMACC2DestAddr = *pBuffer++;
	pDMACReg->DMACC2LLI = *pBuffer++;
	pDMACReg->DMACC2Control0 = *pBuffer++;
	pDMACReg->DMACC2Control1 = *pBuffer++;
	pDMACReg->DMACC2Configuration = *pBuffer++;

	pDMACReg->DMACC3SrcAddr = *pBuffer++;
	pDMACReg->DMACC3DestAddr = *pBuffer++;
	pDMACReg->DMACC3LLI = *pBuffer++;
	pDMACReg->DMACC3Control0 = *pBuffer++;
	pDMACReg->DMACC3Control1 = *pBuffer++;
	pDMACReg->DMACC3Configuration = *pBuffer++;

	pDMACReg->DMACC4SrcAddr = *pBuffer++;
	pDMACReg->DMACC4DestAddr = *pBuffer++;
	pDMACReg->DMACC4LLI = *pBuffer++;
	pDMACReg->DMACC4Control0 = *pBuffer++;
	pDMACReg->DMACC4Control1 = *pBuffer++;
	pDMACReg->DMACC4Configuration = *pBuffer++;

	pDMACReg->DMACC5SrcAddr = *pBuffer++;
	pDMACReg->DMACC5DestAddr = *pBuffer++;
	pDMACReg->DMACC5LLI = *pBuffer++;
	pDMACReg->DMACC5Control0 = *pBuffer++;
	pDMACReg->DMACC5Control1 = *pBuffer++;
	pDMACReg->DMACC5Configuration = *pBuffer++;

	pDMACReg->DMACC6SrcAddr = *pBuffer++;
	pDMACReg->DMACC6DestAddr = *pBuffer++;
	pDMACReg->DMACC6LLI = *pBuffer++;
	pDMACReg->DMACC6Control0 = *pBuffer++;
	pDMACReg->DMACC6Control1 = *pBuffer++;
	pDMACReg->DMACC6Configuration = *pBuffer++;

	pDMACReg->DMACC7SrcAddr = *pBuffer++;
	pDMACReg->DMACC7DestAddr = *pBuffer++;
	pDMACReg->DMACC7LLI = *pBuffer++;
	pDMACReg->DMACC7Control0 = *pBuffer++;
	pDMACReg->DMACC7Control1 = *pBuffer++;
	pDMACReg->DMACC7Configuration = *pBuffer++;
}

//--------------------------------------------------------------------
//48MHz clock source for usb host1.1, IrDA, hsmmc, spi is shared with otg phy clock.
//So, initialization and reset of otg phy shoud be done on initial booting time.
//--------------------------------------------------------------------
static void ControlOTGCLK(BOOL OnOff)
{
	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	volatile OTG_PHY_REG *pOtgPhyReg = (OTG_PHY_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_USBOTG_PHY, FALSE);

    if (OnOff)
    {
        pSysConReg->HCLK_GATE |= (1<<20);
        pSysConReg->OTHERS |= (1<<16);
        pOtgPhyReg->OPHYPWR = 0x0;  // OTG block, & Analog bock in PHY2.0 power up, normal operation
        pOtgPhyReg->OPHYCLK = 0x00; // Externel clock/oscillator, 48MHz reference clock for PLL
        pOtgPhyReg->ORSTCON = 0x1;
        Delay(100);
        pOtgPhyReg->ORSTCON = 0x0;
        Delay(100);
        pSysConReg->HCLK_GATE &= ~(1<<20);
    }
    else
    {
        pOtgPhyReg->OPHYPWR = ((0x1<<4)|(0x1<<3)|(0x1<<0));  // OTG block, & Analog bock in PHY2.0 power down, force_suspend
    }

}

//------------------------------------------------------------------------------
//
// Function:     OEMPowerOff
//
// Description:  Called when the system is to transition to it's lowest  power mode (off)
//
//
void OEMPowerOff()
{
	volatile S3C6410_SYSCON_REG *pSysConReg;
	volatile S3C6410_GPIO_REG *pGPIOReg;
	volatile S3C6410_VIC_REG *pVIC0Reg;
	volatile S3C6410_VIC_REG *pVIC1Reg;
	volatile S3C6410_DMAC_REG *pDMAC0Reg;
	volatile S3C6410_DMAC_REG *pDMAC1Reg;
	volatile S3C6410_DMAC_REG *pSDMAC0Reg;
	volatile OTG_PHY_REG *pOtgPhyReg;

 	int nIndex = 0;

	OALMSG(TRUE, (L"[OEM] ++OEMPowerOff()\r\n"));

#if	0	// KITL can not support Sleep
	// Make sure that KITL is powered off
	pArgs = (OAL_KITL_ARGS*)OALArgsQuery(OAL_ARGS_QUERY_KITL);
	if ((pArgs->flags & OAL_KITL_FLAGS_ENABLED) != 0)
	{
		OALKitlPowerOff();
		OALMSG(1, (L"OEMPowerOff: KITL Disabled\r\n"));
	}
#endif

	//-----------------------------
	// Disable DVS and Set to Full Speed
	//-----------------------------
#ifdef DVS_EN
	ChangeDVSLevel(SYS_L0);
#endif

	//-----------------------------
	// Prepare Specific Actions for Sleep
	//-----------------------------
	BSPPowerOff();

	//------------------------------
	// Prepare CPU Entering Sleep Mode
	//------------------------------

	//----------------
	// Map SFR Address
	//----------------
	pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	pVIC0Reg = (S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC0, FALSE);
	pVIC1Reg = (S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC1, FALSE);
	pDMAC0Reg = (S3C6410_DMAC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_DMA0, FALSE);
	pDMAC1Reg = (S3C6410_DMAC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_DMA1, FALSE);
	pSDMAC0Reg = (S3C6410_DMAC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SDMA0, FALSE);
	pOtgPhyReg = (OTG_PHY_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_USBOTG_PHY, FALSE);

	//------------------
	// Save VIC Registers
	//------------------
	S3C6410_SaveState_VIC((void *)pVIC0Reg, (void *)pVIC1Reg, g_aSleepSave_VIC);

	// Disable All Interrupt
	pVIC0Reg->VICINTENCLEAR = 0xFFFFFFFF;
	pVIC1Reg->VICINTENCLEAR = 0xFFFFFFFF;
	pVIC0Reg->VICSOFTINTCLEAR = 0xFFFFFFFF;
	pVIC1Reg->VICSOFTINTCLEAR = 0xFFFFFFFF;

	//--------------------
	// Save DMAC Registers
	//--------------------
	S3C6410_SaveState_DMACon((void *)pDMAC0Reg, g_aSleepSave_DMACon0);
	S3C6410_SaveState_DMACon((void *)pDMAC1Reg, g_aSleepSave_DMACon1);
	S3C6410_SaveState_DMACon((void *)pSDMAC0Reg, g_aSleepSave_SDMACon0);

	//------------------
	// Save GPIO Register
	//------------------
	S3C6410_SaveState_GPIO((void *)pGPIOReg, g_aSleepSave_GPIO);

	//--------------------
	// Save SysCon Register
	//--------------------
	S3C6410_SaveState_SysCon((void *)pSysConReg, g_aSleepSave_SysCon);

	//-------------------------------------------------------
	// Unmask Clock Gating and Block Power turn On (SW workaround)
	//-------------------------------------------------------

	// HCLK_IROM, HCLK_MEM1, HCLK_MEM0, HCLK_MFC Should be Always On for power Mode (Something coupled with BUS operation)
	//pSysConReg->HCLK_GATE |= ((1<<25)|(1<<22)|(1<<21)|(1<<0));
	pSysConReg->HCLK_GATE = 0xFFFFFFFF;
	pSysConReg->PCLK_GATE = 0xFFFFFFFF;
	pSysConReg->SCLK_GATE = 0xFFFFFFFF;
	// Turn On All Block Block Power
	pSysConReg->NORMAL_CFG = 0xFFFFFF00;

	// Wait for Block Power Stable
	while((pSysConReg->BLK_PWR_STAT & 0x7E) != 0x7E);

	//----------------------------
	// Wake Up Source Configuration
	//----------------------------
 	S3C6410_WakeUpSource_Configure();

	//-------------------------------
	// Extra work for Entering Sleep Mode
	//-------------------------------

	// USB Power Control
	pSysConReg->OTHERS &= ~(1<<16);	// USB Signal Mask Clear
	 pGPIOReg->SPCON |= (1<<3);			// USB Tranceiver PAD to Suspend
	 ControlOTGCLK(FALSE);  			// OTG/Analog block power up

	// TODO: SPCONSLP ???
	//pGPIOReg->SPCONSLP;	// Use Default Valie

	//-------------------------------
	// GPIO Configuration for Sleep State
	//-------------------------------
	// TODO: Configure GPIO at Sleep
	BSPConfigGPIOforPowerOff();

	// Sleep Mode Pad Configuration
	// This code was moved into OALCPUPowerOff
    //pGPIOReg->SLPEN = 0x2;	// Controlled by SLPEN Bit (You Should Clear SLPEN Bit in Wake Up Process...)

	//-----------------------
	// CPU Entering Sleep Mode
	//-----------------------

	OALCPUPowerOff();	// Now in Sleep

	//----------------------------
	// CPU Wake Up from Sleep Mode
	//----------------------------

	//----------------------------
	// Wake Up Source Determine
	//----------------------------
 	S3C6410_WakeUpSource_Detect();

	// Restore SysCon Register
	S3C6410_RestoreState_SysCon((void *)pSysConReg, g_aSleepSave_SysCon);

	// Restore GPIO Register
	S3C6410_RestoreState_GPIO((void *)pGPIOReg, g_aSleepSave_GPIO);

	// Sleep Mode Pad Configuration
	pGPIOReg->SLPEN = 0x2;	// Clear SLPEN Bit for Pad back to Normal Mode

	//-----------------------
	// Restore DMAC Registers
	//-----------------------
	S3C6410_RestoreState_DMACon((void *)pDMAC0Reg, g_aSleepSave_DMACon0);
	S3C6410_RestoreState_DMACon((void *)pDMAC1Reg, g_aSleepSave_DMACon1);
	S3C6410_RestoreState_DMACon((void *)pSDMAC0Reg, g_aSleepSave_SDMACon0);

	// Restore VIC Registers
	S3C6410_RestoreState_VIC((void *)pVIC0Reg, (void *)pVIC1Reg, g_aSleepSave_VIC);
	//pVIC0Reg->VICADDRESS = 0x0;
	//pVIC1Reg->VICADDRESS = 0x0;

	// UART Debug Port Initialize
	OEMInitDebugSerial();

	// Disable Vectored Interrupt Mode on CP15
	System_DisableVIC();

	// Enable Branch Prediction on CP15
	System_EnableBP();

	// Enable IRQ Interrupt on CP15
	System_EnableIRQ();

	// Enable FIQ Interrupt on CP15
	System_EnableFIQ();

	if (g_oalWakeSource == SYSWAKE_UNKNOWN)
	{
		OALMSG(TRUE, (L"[OEM:ERR] OEMPowerOff() : SYSWAKE_UNKNOWN , WAKEUP_STAT = 0x%08x", g_LastWakeupStatus));
	}else
		OALMSG(TRUE, (L"[OEM:ERR] OEMPowerOff() : WakeUp Reason  = 0x%08x", g_oalWakeSource));

	// Initialize System Timer
	OEMInitializeSystemTimer(RESCHED_PERIOD, OEM_COUNT_1MS, 0);

#if	0	// KITL can not support Sleep
	// Reinitialize KITL
	if ((pArgs->flags & OAL_KITL_FLAGS_ENABLED) != 0)
	{
		OALKitlPowerOn();
	}
#endif

	// USB Power Control
	ControlOTGCLK(TRUE);  			// OTG/Analog block power up
	pGPIOReg->SPCON &= ~(1<<3);		// USB Tranceiver PAD to Normal

	//--------------------------------------
	// Post Processing Specific Actions for Wake Up
	//--------------------------------------
	BSPPowerOn();

	OALMSG(TRUE, (L"[OEM] --OEMPowerOff()\r\n"));
}

