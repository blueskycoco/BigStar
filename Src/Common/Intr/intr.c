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
//  File: interrupt.c
//
//  This file implement major part of interrupt module for S3C3210X SoC.
//
#include <windows.h>
#include <ceddk.h>
#include <Nkintr.h>
#include <oal.h>
#include <bsp_cfg.h>
#include <s3c6410.h>
#include <intr.h>

#define VIC_MASK

static const UINT32 g_VirIrq2PhyIrq[IRQ_MAX_S3C6410] =
{
	PHYIRQ_EINT0,		// 0	// 0
	PHYIRQ_EINT0,		// 1	// 0
	PHYIRQ_EINT0,		// 2	// 0
	PHYIRQ_EINT0,		// 3	// 0
	PHYIRQ_EINT1,		// 4	// 1
	PHYIRQ_EINT1,		// 5	// 1
	PHYIRQ_EINT1,		// 6	// 1
	PHYIRQ_EINT1,		// 7	// 1
	PHYIRQ_EINT1,		// 8	// 1
	PHYIRQ_EINT1,		// 9	// 1

	PHYIRQ_EINT1,		// 10 // 1
	PHYIRQ_EINT1,		// 11 // 1
	PHYIRQ_RTC_TIC,		// 12 // 2
	PHYIRQ_CAMIF_C,		// 13 // 3
	PHYIRQ_CAMIF_P,		// 14 // 4
	PHYIRQ_I2C1,			// 15 // 5
	PHYIRQ_I2S_V40,		// 16 // 6
	PHYIRQ_GPS,			// 17 // 7
	PHYIRQ_3D,			// 18 // 8
	PHYIRQ_POST	,		// 19 // 9

	PHYIRQ_ROTATOR,		// 20 // 10
	PHYIRQ_2D,			// 21 // 11
	PHYIRQ_TVENC,		// 22 // 12
	PHYIRQ_TVSCALER,	// 23 // 13
	PHYIRQ_BATF,		// 24 // 14
	PHYIRQ_JPEG,			// 25 // 15
	PHYIRQ_MFC,			// 26 // 16
	PHYIRQ_SDMA0,		// 27 // 17
	PHYIRQ_SDMA0,		// 28 // 17
	PHYIRQ_SDMA0,		// 29 // 17

	PHYIRQ_SDMA0,		// 30 // 17
	PHYIRQ_SDMA0,		// 31 // 17
	PHYIRQ_SDMA0,		// 32 // 17
	PHYIRQ_SDMA0,		// 33 // 17
	PHYIRQ_SDMA0,		// 34 // 17
	PHYIRQ_SDMA1,		// 35 // 18
	PHYIRQ_SDMA1,		// 36 // 18
	PHYIRQ_SDMA1,		// 37 // 18
	PHYIRQ_SDMA1,		// 38 // 18
	PHYIRQ_SDMA1,		// 39 // 18

	PHYIRQ_SDMA1,		// 40 // 18
	PHYIRQ_SDMA1,		// 41 // 18
	PHYIRQ_SDMA1,		// 42 // 18
	PHYIRQ_ARM_DMAERR,	// 43 // 19
	PHYIRQ_ARM_DMA,	// 44 // 20
	PHYIRQ_ARM_DMAS,	// 45 // 21
	PHYIRQ_KEYPAD,		// 46 // 22
	PHYIRQ_TIMER0,		// 47 // 23
	PHYIRQ_TIMER1,		// 48 // 24
	PHYIRQ_TIMER2,		// 49 // 25

	PHYIRQ_WDT,			// 50 // 26
	PHYIRQ_TIMER3,		// 51 // 27
	PHYIRQ_TIMER4,		// 52 // 28
	PHYIRQ_LCD0_FIFO,	// 53 // 29
	PHYIRQ_LCD1_FRAME,	// 54 // 30
	PHYIRQ_LCD2_SYSIF,	// 55 // 31
	PHYIRQ_EINT2,		// 56 // 32
	PHYIRQ_EINT2,		// 57 // 32
	PHYIRQ_EINT2,		// 58 // 32
	PHYIRQ_EINT2,		// 59 // 32

	PHYIRQ_EINT2,		// 60 // 32
	PHYIRQ_EINT2,		// 61 // 32
	PHYIRQ_EINT2,		// 62 // 32
	PHYIRQ_EINT2,		// 63 // 32
	PHYIRQ_EINT3,		// 64 // 33
	PHYIRQ_EINT3,		// 65 // 33
	PHYIRQ_EINT3,		// 66 // 33
	PHYIRQ_EINT3,		// 67 // 33
	PHYIRQ_EINT3,		// 68 // 33
	PHYIRQ_EINT3,		// 69 // 33

	PHYIRQ_EINT3,		// 70 // 33
	PHYIRQ_EINT3,		// 71 // 33
	PHYIRQ_PCM0	,		// 72 // 34
	PHYIRQ_PCM1	,		// 73 // 35
	PHYIRQ_AC97,		// 74 // 36
	PHYIRQ_UART0,		// 75 // 37
	PHYIRQ_UART1,		// 76 // 38
	PHYIRQ_UART2,		// 77 // 39
	PHYIRQ_UART3,		// 78 // 40
	PHYIRQ_DMA0,		// 79 // 41

	PHYIRQ_DMA0,		// 80 // 41
	PHYIRQ_DMA0,		// 81 // 41
	PHYIRQ_DMA0,		// 82 // 41
	PHYIRQ_DMA0,		// 83 // 41
	PHYIRQ_DMA0,		// 84 // 41
	PHYIRQ_DMA0,		// 85 // 41
	PHYIRQ_DMA0,		// 86 // 41
	PHYIRQ_DMA1,		// 87 // 42
	PHYIRQ_DMA1,		// 88 // 42
	PHYIRQ_DMA1,		// 89 // 42

	PHYIRQ_DMA1,		// 90 // 42
	PHYIRQ_DMA1,		// 91 // 42
	PHYIRQ_DMA1,		// 92 // 42
	PHYIRQ_DMA1,		// 93 // 42
	PHYIRQ_DMA1,		// 94 // 42
	PHYIRQ_ONENAND0,	// 95 // 43
	PHYIRQ_ONENAND1,	// 96 // 44
	PHYIRQ_NFC,			// 97 // 45
	PHYIRQ_CFC,			// 98 // 46
	PHYIRQ_UHOST,		// 99 // 47

	PHYIRQ_SPI0,			// 100 // 48
	PHYIRQ_SPI1,			// 101 // 49
	PHYIRQ_I2C,			// 102 // 50
	PHYIRQ_HSITX,		// 103 // 51
	PHYIRQ_HSIRX,		// 104 // 52
	PHYIRQ_RESERVED,	// 105 // 53
	PHYIRQ_MSM,			// 106 // 54
	PHYIRQ_HOSTIF,		// 107 // 55
	PHYIRQ_HSMMC0,		// 108 // 56
	PHYIRQ_HSMMC1,		// 109 // 57

	PHYIRQ_OTG,			// 110 // 58
	PHYIRQ_IRDA,		// 111 // 59
	PHYIRQ_RTC_ALARM,	// 112 // 60
	PHYIRQ_SEC,			// 113 // 61
	PHYIRQ_PENDN,		// 114 // 62
	PHYIRQ_ADC			// 115 // 63
};

static const UINT32 g_PhyIrq2VirIrq[PHYIRQ_MAX_S3C6410] =
{
	// VIC0
	IRQ_EINT0,			// 0 (IRQ_EINT0~IRQ_EINT3)
	IRQ_EINT4,			// 1 (IRQ_EINT4~IRQ_EINT11)
	IRQ_RTC_TIC,		// 2
	IRQ_CAMIF_C,		// 3
	IRQ_CAMIF_P,		// 4
	IRQ_I2C1,			// 5
	IRQ_I2S_V40,			// 6
	IRQ_GPS,			// 7
	IRQ_3D,				// 8
	IRQ_POST,			// 9
	IRQ_ROTATOR,		// 10
	IRQ_2D,				// 11
	IRQ_TVENC,			// 12
	IRQ_TVSCALER,		// 13
	IRQ_BATF,			// 14
	IRQ_JPEG,			// 15
	IRQ_MFC,			// 16
	IRQ_SDMA0_CH0,			// 17
	IRQ_SDMA1_CH0,			// 18
	IRQ_ARM_DMAERR,	// 19
	IRQ_ARM_DMA,		// 20
	IRQ_ARM_DMAS,		// 21
	IRQ_KEYPAD,			// 22
	IRQ_TIMER0,			// 23
	IRQ_TIMER1,			// 24
	IRQ_TIMER2,			// 25
	IRQ_WDT,			// 26
	IRQ_TIMER3,			// 27
	IRQ_TIMER4,			// 28
	IRQ_LCD0_FIFO,		// 29
	IRQ_LCD1_FRAME,		// 30
	IRQ_LCD2_SYSIF,		// 31

	// VIC1
	IRQ_EINT12,			// 32 (IRQ_EINT12~IRQ_EINT19)
	IRQ_EINT20,			// 33 (IRQ_EINT20~IRQ_EINT27)
	IRQ_PCM0,			// 34
	IRQ_PCM1,			// 35
	IRQ_AC97,			// 36
	IRQ_UART0,			// 37
	IRQ_UART1,			// 38
	IRQ_UART2,			// 39
	IRQ_UART3,			// 40
	IRQ_DMA0_CH0,		// 41 (IRQ_DMA0_CH0~IRQ_DMA0_CH7)
	IRQ_DMA1_CH0,		// 42 (IRQ_DMA1_CH0~IRQ_DMA1_CH7)
	IRQ_ONENAND0,		// 43
	IRQ_ONENAND1,		// 44
	IRQ_NFC,			// 45
	IRQ_CFC,			// 46
	IRQ_UHOST,			// 47
	IRQ_SPI0,			// 48
	IRQ_SPI1,			// 49
	IRQ_I2C,				// 50
	IRQ_HSITX,			// 51
	IRQ_HSIRX,			// 52
	IRQ_RESERVED,		// 53
	IRQ_MSM	,			// 54
	IRQ_HOSTIF,			// 55
	IRQ_HSMMC0,			// 56
	IRQ_HSMMC1,			// 57
	IRQ_OTG	,			// 58
	IRQ_IRDA,			// 59
	IRQ_RTC_ALARM,		// 60
	IRQ_SEC,			// 61
	IRQ_PENDN,			// 62
	IRQ_ADC				// 63
};

//------------------------------------------------------------------------------
//
//  Globals:  g_pIntrRegs/g_pPortRegs
//
//  The global variables are storing virual address for interrupt and port
//  registers for use in interrupt handling to avoid possible time consumig
//  call to OALPAtoVA function.
//
static volatile S3C6410_VIC_REG *g_pVIC0Reg;
static volatile S3C6410_VIC_REG *g_pVIC1Reg;
static volatile S3C6410_GPIO_REG *g_pGPIOReg;
static volatile S3C6410_DMAC_REG *g_pDMAC0Reg;
static volatile S3C6410_DMAC_REG *g_pDMAC1Reg;
static volatile S3C6410_DMAC_REG *g_pSDMAC0Reg;
static volatile S3C6410_DMAC_REG *g_pSDMAC1Reg;
//  Function pointer to profiling timer ISR routine.
//
PFN_PROFILER_ISR g_pProfilerISR = NULL;

static void PrepareEINTIntr(void);
static void PrepareDMACIntr(void);
static void InitializeVIC(void);
void VICTableInit(void);					// Reference by OEMPowerOff() in "Off.c"

//------------------------------------------------------------------------------
//
//  Function:  OALIntrInit
//
//  This function initialize interrupt mapping, hardware and call platform
//  specific initialization.
//
BOOL OALIntrInit()
{
	BOOL rc = FALSE;

	OALMSG( OAL_FUNC&&OAL_INTR, (L"+OALInterruptInit\r\n") );

	// First get uncached virtual addresses
	g_pVIC0Reg = (S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC0, FALSE);
	g_pVIC1Reg = (S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC1, FALSE);
	g_pGPIOReg = (S3C6410_GPIO_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	g_pDMAC0Reg = (S3C6410_DMAC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_DMA0, FALSE);
	g_pDMAC1Reg = (S3C6410_DMAC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_DMA1, FALSE);
	g_pSDMAC0Reg = (S3C6410_DMAC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_SDMA0, FALSE);
	g_pSDMAC1Reg = (S3C6410_DMAC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_SDMA1, FALSE);

	// Initialize interrupt mapping
	OALIntrMapInit();

	PrepareEINTIntr();

	PrepareDMACIntr();

	// Initialize VIC
	InitializeVIC();

#ifdef OAL_BSP_CALLBACKS
	// Give BSP change to initialize subordinate controller
	rc = BSPIntrInit();
#else
	rc = TRUE;
#endif

	OALMSG(OAL_INTR&&OAL_FUNC, (L"-OALInterruptInit(rc = %d)\r\n", rc));
	return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  OALIntrRequestIrqs
//
//  This function returns IRQs for CPU/SoC devices based on their
//  physical address.
//
BOOL OALIntrRequestIrqs(DEVICE_LOCATION *pDevLoc, UINT32 *pCount, UINT32 *pIrqs)
{
	BOOL rc = FALSE;

	OALMSG(OAL_INTR&&OAL_FUNC, (L"+OALIntrRequestIrqs(0x%08x->%d/%d/0x%08x/%d, 0x%08x, 0x%08x)\r\n",
				pDevLoc, pDevLoc->IfcType, pDevLoc->BusNumber, pDevLoc->LogicalLoc,
				pDevLoc->Pin, pCount, pIrqs));

	// This shouldn't happen
	if (*pCount < 1) goto cleanUp;

#ifdef OAL_BSP_CALLBACKS
	rc = BSPIntrRequestIrqs(pDevLoc, pCount, pIrqs);
#endif

cleanUp:

	OALMSG(OAL_INTR&&OAL_FUNC, (L"-OALIntrRequestIrqs(rc = %d)\r\n", rc));
	return rc;
}


//------------------------------------------------------------------------------
//
//  Function:  OALIntrEnableIrqs
//
BOOL OALIntrEnableIrqs(UINT32 count, const UINT32 *pIrqs)
{
	BOOL bRet = TRUE;
	UINT32 VirtualIRQ;
	UINT32 PhysicalIRQ;
	UINT32 i;

	OALMSG(OAL_INTR&&OAL_FUNC, (L"+OALIntrEnableIrqs(%d, 0x%08x)\r\n", count, pIrqs));

	for (i = 0; i < count; i++)
	{
#ifndef OAL_BSP_CALLBACKS
		VirtualIRQ = pIrqs[i];
#else
		// Give BSP chance to enable irq on subordinate interrupt controller
		VirtualIRQ = BSPIntrEnableIrq(pIrqs[i]);
#endif

		if (VirtualIRQ == OAL_INTR_IRQ_UNDEFINED) continue;

		// Translate to Physical IRQ
		PhysicalIRQ = g_VirIrq2PhyIrq[VirtualIRQ];

		if (PhysicalIRQ < VIC1_BIT_OFFSET)
		{
			g_pVIC0Reg->VICINTENABLE = (0x1<<PhysicalIRQ);
		}
		else if (PhysicalIRQ < PHYIRQ_MAX_S3C6410)
		{
			g_pVIC1Reg->VICINTENABLE = (0x1<<(PhysicalIRQ-VIC1_BIT_OFFSET));
		}
		else
		{
			bRet = FALSE;
		}
	}

	OALMSG(OAL_INTR&&OAL_FUNC, (L"-OALIntrEnableIrqs(rc = %d)\r\n", bRet));

	return bRet;
}

//------------------------------------------------------------------------------
//
//  Function:  OALIntrDisableIrqs
//
VOID OALIntrDisableIrqs(UINT32 count, const UINT32 *pIrqs)
{
	UINT32 VirtualIRQ;
	UINT32 PhysicalIRQ;
	UINT32 i;

	OALMSG(OAL_INTR&&OAL_FUNC, (L"+OALIntrDisableIrqs(%d, 0x%08x)\r\n", count, pIrqs));

	for (i = 0; i < count; i++)
	{
#ifndef OAL_BSP_CALLBACKS
		VirtualIRQ = pIrqs[i];
#else
		// Give BSP chance to disable irq on subordinate interrupt controller
		VirtualIRQ = BSPIntrDisableIrq(pIrqs[i]);
#endif
		if (VirtualIRQ == OAL_INTR_IRQ_UNDEFINED) continue;

		// Translate to Physical IRQ
		PhysicalIRQ = g_VirIrq2PhyIrq[VirtualIRQ];

		if (PhysicalIRQ == PHYIRQ_EINT0)
		{
			// IRQ_EINT0 ~ IRQ_EINT3
			g_pGPIOReg->EINT0MASK |= (1<<VirtualIRQ);	// Mask Sub Interrupt
			g_pGPIOReg->EINT0PEND |= (1<<VirtualIRQ);	// Clear Sub Pending

			// Do not Mask PHYIRQ_EINT0 Interrupt !!!
		}
		else if (PhysicalIRQ == PHYIRQ_EINT1)
		{
			// IRQ_EINT4 ~ IRQ_EINT11
			g_pGPIOReg->EINT0MASK |= (1<<VirtualIRQ);	// Mask Sub Interrupt
			g_pGPIOReg->EINT0PEND |= (1<<VirtualIRQ);	// Clear Sub Pending

			// Do not Mask PHYIRQ_EINT1 Interrupt !!!
		}
		else if (PhysicalIRQ == PHYIRQ_EINT2)
		{
			// IRQ_EINT12 ~ IRQ_EINT19
			g_pGPIOReg->EINT0MASK |= (1<<(VirtualIRQ-30));	// Mask Sub Interrupt
			g_pGPIOReg->EINT0PEND |= (1<<(VirtualIRQ-30));	// Clear Sub Pending

			// Do not Mask PHYIRQ_EINT2 Interrupt !!!
		}
		else if (PhysicalIRQ == PHYIRQ_EINT3)
		{
			// IRQ_EINT20 ~ IRQ_EINT27
			g_pGPIOReg->EINT0MASK |= (1<<(VirtualIRQ-30));	// Mask Sub Interrupt
			g_pGPIOReg->EINT0PEND |= (1<<(VirtualIRQ-30));	// Clear Sub Pending

			// Do not Mask PHYIRQ_EINT3 Interrupt !!!
		}
		else if (PhysicalIRQ == PHYIRQ_DMA0)
		{
			// Do not Mask PHYIRQ_DMA0 Interrupt !!!
		}
		else if (PhysicalIRQ == PHYIRQ_DMA1)
		{
			// Do not Mask PHYIRQ_DMA1 Interrupt !!!
		}
		else if (PhysicalIRQ < VIC1_BIT_OFFSET)
		{
			g_pVIC0Reg->VICINTENCLEAR = (0x1<<PhysicalIRQ);
		}
		else if (PhysicalIRQ < PHYIRQ_MAX_S3C6410)
		{
			g_pVIC1Reg->VICINTENCLEAR = (0x1<<(PhysicalIRQ-VIC1_BIT_OFFSET));
		}
	}

	OALMSG(OAL_INTR&&OAL_FUNC, (L"-OALIntrDisableIrqs\r\n"));
}

//------------------------------------------------------------------------------
//
//  Function:  OALIntrDoneIrqs
//
VOID OALIntrDoneIrqs(UINT32 count, const UINT32 *pIrqs)
{
	UINT32 VirtualIRQ;
	UINT32 PhysicalIRQ;
	UINT32 i;

	OALMSG(OAL_INTR&&OAL_VERBOSE, (L"+OALIntrDoneIrqs(%d, 0x%08x)\r\n", count, pIrqs));

	for (i = 0; i < count; i++)
	{
#ifndef OAL_BSP_CALLBACKS
		VirtualIRQ = pIrqs[i];
#else
		// Give BSP chance to finish irq on subordinate interrupt controller
		VirtualIRQ = BSPIntrDoneIrq(pIrqs[i]);
#endif

		// Translate to Physical IRQ
		PhysicalIRQ = g_VirIrq2PhyIrq[VirtualIRQ];

		if (PhysicalIRQ < VIC1_BIT_OFFSET)
		{
			g_pVIC0Reg->VICINTENABLE = (0x1<<PhysicalIRQ);
		}
		else if (PhysicalIRQ < PHYIRQ_MAX_S3C6410)
		{
			g_pVIC1Reg->VICINTENABLE = (0x1<<(PhysicalIRQ-VIC1_BIT_OFFSET));
		}
	}

	OALMSG(OAL_INTR&&OAL_VERBOSE, (L"-OALIntrDoneIrqs\r\n"));
}

//------------------------------------------------------------------------------
//
//  Function:  OEMInterruptHandler
//
ULONG OEMInterruptHandler(ULONG ra)
{
	UINT32 SysIntr = SYSINTR_NOP;
	UINT32 PhysicalIRQ;
	UINT32 VirtualIRQ;
	UINT32 uiPending;
	UINT32 uiMask;
	int nNumber;
#ifdef	VIC_MASK
	UINT32 IntEnVIC0, IntEnVIC1;
#endif

	// Get Pending Interrupt Number
	PhysicalIRQ = g_pVIC1Reg->VICADDRESS;	// Ready Dummy from VIC1
	PhysicalIRQ = g_pVIC0Reg->VICADDRESS;

#ifdef	VIC_MASK
	// To Avoid low proirity interrupt lost
	IntEnVIC0 = g_pVIC0Reg->VICINTENABLE;
	IntEnVIC1 = g_pVIC1Reg->VICINTENABLE;
	g_pVIC0Reg->VICINTENCLEAR = 0xffffffff;
	g_pVIC1Reg->VICINTENCLEAR = 0xffffffff;
#endif

	// Translate Physical IRQ to Virtual IRQ (Except Flatten IRQ)
	VirtualIRQ = g_PhyIrq2VirIrq[PhysicalIRQ];

	if (PhysicalIRQ == PHYIRQ_TIMER4)
	{
		//-------------
		// System Timer
		//-------------

		if (g_pVIC0Reg->VICRAWINTR & (1<<PHYIRQ_TIMER4))
		{
			// Handling System Timer Interrupt
			SysIntr = OALTimerIntrHandler();
		}
#ifndef	REMOVE_BEFORE_RELEASE
		else
		{
			OALMSG(TRUE, (L"[OAL:ERR] PHYIRQ_TIMER4 Interrupt But There is No Pending !!!!\r\n"));
			OALMSG(TRUE, (L"[OAL:ERR] Maybe OALTimerIntrHandler() in OEMIdle() Make this\r\n"));
		}
#endif

	}
	else if (PhysicalIRQ == PHYIRQ_TIMER2)
	{
		//-------------
		// Profiling Timer
		//-------------

		// Masking Interrupt
#ifdef	VIC_MASK
		IntEnVIC0 &= ~(1<<PHYIRQ_TIMER2);
#else
		g_pVIC0Reg->VICINTENCLEAR = (1<<PHYIRQ_TIMER2);
#endif

		// TODO: Clear Source Pending
		//g_pPwmRegs->TCNTCLRINT2;

		// Handling Profiling Timer Interrupt
		if (g_pProfilerISR)
		{
			SysIntr = g_pProfilerISR(ra);
		}
	}
	else
	{
#ifdef OAL_ILTIMING
		if (g_oalILT.active)
		{
			g_oalILT.isrTime1 = OALTimerCountsSinceSysTick();
			g_oalILT.savedPC = 0;
			g_oalILT.interrupts++;
		}
#endif

		if (PhysicalIRQ == PHYIRQ_EINT0)
		{
			// Do not mask PHYIRQ_EINT0 interrupt for other sub interrupt
			// Mask EINTMASK and EINTPEND which occured this time
			// So each IST of EINT should unmask their EINT for Next Interrupt

			// Check Sub Source and Mask
			uiPending = g_pGPIOReg->EINT0PEND;
			uiMask = g_pGPIOReg->EINT0MASK;

			for (nNumber=0; nNumber<4; nNumber++)	// EINT0~EINT3
			{
				if ((uiPending & (1<<nNumber)) && !(uiMask & (1<<nNumber)))
				{
					g_pGPIOReg->EINT0MASK |= (1<<nNumber);	// Mask Sub Interrupt
					g_pGPIOReg->EINT0PEND |= (1<<nNumber);		// Clear Sub Pending
					VirtualIRQ = (IRQ_EINT0+nNumber);				// Set Virtual IRQ
					break;
				}
			}
		}
		else if (PhysicalIRQ == PHYIRQ_EINT1)
		{
			// Do not mask PHYIRQ_EINT1 interrupt for other sub interrupt
			// Mask EINTMASK and EINTPEND which occured this time
			// So each IST of EINT should unmask their EINT for Next Interrupt

			// Check Sub Source
			uiPending = g_pGPIOReg->EINT0PEND;
			uiMask = g_pGPIOReg->EINT0MASK;

			for (nNumber=4; nNumber<12; nNumber++)	// EINT4~EINT11
			{
				if ((uiPending & (1<<nNumber)) && !(uiMask & (1<<nNumber)))
				{
					g_pGPIOReg->EINT0MASK |= (1<<nNumber);	// Mask Sub Interrupt
					g_pGPIOReg->EINT0PEND |= (1<<nNumber);		// Clear Sub Pending
					VirtualIRQ = (IRQ_EINT4+(nNumber-4));			// Set Virtual IRQ
					break;
				}
			}
		}
		else if (PhysicalIRQ == PHYIRQ_EINT2)
		{
			// Do not mask PHYIRQ_EINT2 interrupt for other sub interrupt
			// Mask EINTMASK and EINTPEND which occured this time
			// So each IST of EINT should unmask their EINT for Next Interrupt

			// Check Sub Source
			uiPending = g_pGPIOReg->EINT0PEND;
			uiMask = g_pGPIOReg->EINT0MASK;

			for (nNumber=12; nNumber<20; nNumber++)	// EINT12~EINT19
			{
				if ((uiPending & (1<<nNumber)) && !(uiMask & (1<<nNumber)))
				{
					g_pGPIOReg->EINT0MASK |= (1<<nNumber);	// Mask Sub Interrupt
					g_pGPIOReg->EINT0PEND |= (1<<nNumber);		// Clear Sub Pending
					VirtualIRQ = (IRQ_EINT12+(nNumber-12));		// Set Virtual IRQ
					break;
				}
			}
		}
		else if (PhysicalIRQ == PHYIRQ_EINT3)
		{
			// Do not mask PHYIRQ_EINT3 interrupt for other sub interrupt
			// Mask EINTMASK and EINTPEND which occured this time
			// So each IST of EINT should unmask their EINT for Next Interrupt

			// Check Sub Source
			uiPending = g_pGPIOReg->EINT0PEND;
			uiMask = g_pGPIOReg->EINT0MASK;

			for (nNumber=20; nNumber<28; nNumber++)	// EINT20~EINT27
			{
				if ((uiPending & (1<<nNumber)) && !(uiMask & (1<<nNumber)))
				{
					g_pGPIOReg->EINT0MASK |= (1<<nNumber);	// Mask Sub Interrupt
					g_pGPIOReg->EINT0PEND |= (1<<nNumber);		// Clear Sub Pending
					VirtualIRQ = (IRQ_EINT20+(nNumber-20));		// Set Virtual IRQ
					break;
				}
			}
		}
		else if (PhysicalIRQ == PHYIRQ_DMA0)
		{
			DWORD dwIntStatus;

#ifdef	VIC_MASK
			IntEnVIC1 &= ~(1<<(PHYIRQ_DMA0-VIC1_BIT_OFFSET));
#else
			g_pVIC1Reg->VICINTENCLEAR = (1<<(PHYIRQ_DMA0-VIC1_BIT_OFFSET));
#endif

			dwIntStatus = g_pDMAC0Reg->DMACIntStatus;

			if (dwIntStatus & 0x01) VirtualIRQ = IRQ_DMA0_CH0;		// channel 0
			else if (dwIntStatus & 0x02) VirtualIRQ = IRQ_DMA0_CH1;	// channel 1
			else if (dwIntStatus & 0x04) VirtualIRQ = IRQ_DMA0_CH2;	// channel 2
			else if (dwIntStatus & 0x08) VirtualIRQ = IRQ_DMA0_CH3;	// channel 3
			else if (dwIntStatus & 0x10) VirtualIRQ = IRQ_DMA0_CH4;	// channel 4
			else if (dwIntStatus & 0x20) VirtualIRQ = IRQ_DMA0_CH5;	// channel 5
			else if (dwIntStatus & 0x40) VirtualIRQ = IRQ_DMA0_CH6;	// channel 6
			else if (dwIntStatus & 0x80) VirtualIRQ = IRQ_DMA0_CH7;	// channel 7
		}
		else if (PhysicalIRQ == PHYIRQ_DMA1)
		{
			DWORD dwIntStatus;

#ifdef	VIC_MASK
			IntEnVIC1 &= ~(1<<(PHYIRQ_DMA1-VIC1_BIT_OFFSET));
#else
			g_pVIC1Reg->VICINTENCLEAR = (1<<(PHYIRQ_DMA1-VIC1_BIT_OFFSET));
#endif

			dwIntStatus = g_pDMAC1Reg->DMACIntStatus;

			if (dwIntStatus & 0x01) VirtualIRQ = IRQ_DMA1_CH0;		// channel 0
			else if (dwIntStatus & 0x02) VirtualIRQ = IRQ_DMA1_CH1;	// channel 1
			else if (dwIntStatus & 0x04) VirtualIRQ = IRQ_DMA1_CH2;	// channel 2
			else if (dwIntStatus & 0x08) VirtualIRQ = IRQ_DMA1_CH3;	// channel 3
			else if (dwIntStatus & 0x10) VirtualIRQ = IRQ_DMA1_CH4;	// channel 4
			else if (dwIntStatus & 0x20) VirtualIRQ = IRQ_DMA1_CH5;	// channel 5
			else if (dwIntStatus & 0x40) VirtualIRQ = IRQ_DMA1_CH6;	// channel 6
			else if (dwIntStatus & 0x80) VirtualIRQ = IRQ_DMA1_CH7;	// channel 7
		}
		else if (PhysicalIRQ == PHYIRQ_SDMA0)
		{
			DWORD dwIntStatus;

#ifdef	VIC_MASK
			IntEnVIC0 &= ~(1<<PhysicalIRQ);
#else
			g_pVIC0Reg->VICINTENCLEAR = (1<<PhysicalIRQ);
#endif

			dwIntStatus = g_pSDMAC0Reg->DMACIntStatus;

			if (dwIntStatus & 0x01) VirtualIRQ = IRQ_SDMA0_CH0;		// channel 0
			else if (dwIntStatus & 0x02) VirtualIRQ = IRQ_SDMA0_CH1;	// channel 1
			else if (dwIntStatus & 0x04) VirtualIRQ = IRQ_SDMA0_CH2;	// channel 2
			else if (dwIntStatus & 0x08) VirtualIRQ = IRQ_SDMA0_CH3;	// channel 3
			else if (dwIntStatus & 0x10) VirtualIRQ = IRQ_SDMA0_CH4;	// channel 4
			else if (dwIntStatus & 0x20) VirtualIRQ = IRQ_SDMA0_CH5;	// channel 5
			else if (dwIntStatus & 0x40) VirtualIRQ = IRQ_SDMA0_CH6;	// channel 6
			else if (dwIntStatus & 0x80) VirtualIRQ = IRQ_SDMA0_CH7;	// channel 7
		}
		else if (PhysicalIRQ == PHYIRQ_SDMA1)
		{
			DWORD dwIntStatus;

#ifdef	VIC_MASK
			IntEnVIC0 &= ~(1<<PhysicalIRQ);
#else
			g_pVIC0Reg->VICINTENCLEAR = (1<<PhysicalIRQ);
#endif

			dwIntStatus = g_pSDMAC1Reg->DMACIntStatus;
                    
			if (dwIntStatus & 0x01) VirtualIRQ = IRQ_SDMA1_CH0;		// channel 0
			else if (dwIntStatus & 0x02) VirtualIRQ = IRQ_SDMA1_CH1;	// channel 1
			else if (dwIntStatus & 0x04) VirtualIRQ = IRQ_SDMA1_CH2;	// channel 2
			else if (dwIntStatus & 0x08) VirtualIRQ = IRQ_SDMA1_CH3;	// channel 3
			else if (dwIntStatus & 0x10) VirtualIRQ = IRQ_SDMA1_CH4;	// channel 4
			else if (dwIntStatus & 0x20) VirtualIRQ = IRQ_SDMA1_CH5;	// channel 5
			else if (dwIntStatus & 0x40) VirtualIRQ = IRQ_SDMA1_CH6;	// channel 6
			else if (dwIntStatus & 0x80) VirtualIRQ = IRQ_SDMA1_CH7;	// channel 7
		}        
		else if (PhysicalIRQ < VIC1_BIT_OFFSET)
		{
#ifdef	VIC_MASK
			IntEnVIC0 &= ~(1<<PhysicalIRQ);
#else
			g_pVIC0Reg->VICINTENCLEAR = (1<<PhysicalIRQ);
#endif
		}
		else if (PhysicalIRQ < PHYIRQ_MAX_S3C6410)
		{
#ifdef	VIC_MASK
			IntEnVIC1 &= ~(1<<(PhysicalIRQ-VIC1_BIT_OFFSET));
#else
			g_pVIC1Reg->VICINTENCLEAR = (1<<(PhysicalIRQ-VIC1_BIT_OFFSET));
#endif
		}

#if	0	// Multi IRQ is not supported
		// First find if IRQ is claimed by chain
		SysIntr = NKCallIntChain((BYTE)VirtualIRQ);

		if (SysIntr == SYSINTR_CHAIN || !NKIsSysIntrValid(SysIntr))
		{
			// IRQ wasn't claimed, use static mapping
			SysIntr = OALIntrTranslateIrq(VirtualIRQ);
		}
#else
	        // We don't assume IRQ sharing, use static mapping
		SysIntr = OALIntrTranslateIrq(VirtualIRQ);
#endif
	}

	// Clear Vector Address Register
	g_pVIC0Reg->VICADDRESS = 0x0;
	g_pVIC1Reg->VICADDRESS = 0x0;

#ifdef	VIC_MASK
	// To Avoid low proirity interrupt lost
	g_pVIC0Reg->VICINTENABLE = IntEnVIC0;
	g_pVIC1Reg->VICINTENABLE = IntEnVIC1;
#endif

	return SysIntr;
}

static void PrepareEINTIntr(void)
{
#if	0	// TODO:
	// Mask and clear external interrupts
	OUTREG32(&g_pPortRegs->EINTMASK, 0xFFFFFFFF);
	OUTREG32(&g_pPortRegs->EINTPEND, 0xFFFFFFFF);

	// Mask and clear internal sub interrupts
	OUTREG32(&g_pIntrRegs->SUBSRCPND, 0xFFFFFFFF);
	OUTREG32(&g_pIntrRegs->INTSUBMSK, 0xFFFFFFFF);

	// Mask and clear internal interrupts
	OUTREG32(&g_pIntrRegs->INTMSK, 0xFFFFFFFF);
	OUTREG32(&g_pIntrRegs->SRCPND, 0xFFFFFFFF);
#endif
}

static void PrepareDMACIntr(void)
{
	// Disable Interrupt of All Channel
	// Mask TC and Error Interrupt
	// Clear Interrupt Pending

	g_pDMAC0Reg->DMACC0Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC0Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC1Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC1Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC2Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC2Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC3Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC3Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC4Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC4Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC5Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC5Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC6Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC6Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC0Reg->DMACC7Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC0Reg->DMACC7Configuration &= (3<<14);	// TCINT & ErrINT Mask

	g_pDMAC0Reg->DMACIntTCClear = 0xff;			// TC Int Pending Clear
	g_pDMAC0Reg->DMACIntErrClear = 0xff;			// Err Int Pending Clear

	g_pDMAC1Reg->DMACC0Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC0Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC1Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC1Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC2Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC2Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC3Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC3Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC4Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC4Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC5Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC5Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC6Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC6Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pDMAC1Reg->DMACC7Control0 &= ~(1<<31);		// TCINT Disable
	g_pDMAC1Reg->DMACC7Configuration &= (3<<14);	// TCINT & ErrINT Mask

	g_pDMAC1Reg->DMACIntTCClear = 0xff;			// TC Int Pending Clear
	g_pDMAC1Reg->DMACIntErrClear = 0xff;			// Err Int Pending Clear

    //SDMA
	g_pSDMAC0Reg->DMACC0Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC0Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC1Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC1Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC2Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC2Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC3Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC3Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC4Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC4Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC5Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC5Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC6Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC6Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC0Reg->DMACC7Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC0Reg->DMACC7Configuration &= (3<<14);	// TCINT & ErrINT Mask

	g_pSDMAC0Reg->DMACIntTCClear = 0xff;			// TC Int Pending Clear
	g_pSDMAC0Reg->DMACIntErrClear = 0xff;			// Err Int Pending Clear

	g_pSDMAC1Reg->DMACC0Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC0Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC1Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC1Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC2Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC2Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC3Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC3Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC4Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC4Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC5Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC5Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC6Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC6Configuration &= (3<<14);	// TCINT & ErrINT Mask
	g_pSDMAC1Reg->DMACC7Control0 &= ~(1<<31);		// TCINT Disable
	g_pSDMAC1Reg->DMACC7Configuration &= (3<<14);	// TCINT & ErrINT Mask

	g_pSDMAC1Reg->DMACIntTCClear = 0xff;			// TC Int Pending Clear
	g_pSDMAC1Reg->DMACIntErrClear = 0xff;			// Err Int Pending Clear    
}

static void InitializeVIC(void)
{
	// Disable All Interrupts
	g_pVIC0Reg->VICINTENCLEAR = 0xFFFFFFFF;
	g_pVIC1Reg->VICINTENCLEAR = 0xFFFFFFFF;
	g_pVIC0Reg->VICSOFTINTCLEAR = 0xFFFFFFFF;
	g_pVIC1Reg->VICSOFTINTCLEAR = 0xFFFFFFFF;

	// Clear Current Active Vector Address
	g_pVIC0Reg->VICADDRESS = 0x0;
	g_pVIC1Reg->VICADDRESS = 0x0;

	// Initialize Vector Address Table
	VICTableInit();

	// Disable Vectored Interrupt Mode on CP15
	System_DisableVIC();

	// Enable IRQ Interrupt on CP15
	System_EnableIRQ();

	// Enable FIQ Interrupt on CP15
	System_EnableFIQ();
}

void VICTableInit(void)
{
	// This Function is reference by OEMPowerOff() in "Off.c"
	// Make Sure that Caller and This function is in Same Address Space

	// Fill Vector Address of VIC0
	// Actually, Filled with Physical IRQ Numbers.
	// Because We do not use vectored interrupt feature

	g_pVIC0Reg->VICVECTADDR0 = PHYIRQ_EINT0;
	g_pVIC0Reg->VICVECTADDR1 = PHYIRQ_EINT1;
	g_pVIC0Reg->VICVECTADDR2 = PHYIRQ_RTC_TIC;
	g_pVIC0Reg->VICVECTADDR3 = PHYIRQ_CAMIF_C;
	g_pVIC0Reg->VICVECTADDR4 = PHYIRQ_CAMIF_P;
	g_pVIC0Reg->VICVECTADDR5 = PHYIRQ_I2C1;
	g_pVIC0Reg->VICVECTADDR6 = PHYIRQ_I2S_V40;
	g_pVIC0Reg->VICVECTADDR7 = PHYIRQ_GPS;
	g_pVIC0Reg->VICVECTADDR8 = PHYIRQ_3D;
	g_pVIC0Reg->VICVECTADDR9 = PHYIRQ_POST;
	g_pVIC0Reg->VICVECTADDR10 = PHYIRQ_ROTATOR;
	g_pVIC0Reg->VICVECTADDR11 = PHYIRQ_2D;
	g_pVIC0Reg->VICVECTADDR12 = PHYIRQ_TVENC;
	g_pVIC0Reg->VICVECTADDR13 = PHYIRQ_TVSCALER;
	g_pVIC0Reg->VICVECTADDR14 = PHYIRQ_BATF;
	g_pVIC0Reg->VICVECTADDR15 = PHYIRQ_JPEG;
	g_pVIC0Reg->VICVECTADDR16 = PHYIRQ_MFC;
	g_pVIC0Reg->VICVECTADDR17 = PHYIRQ_SDMA0;
	g_pVIC0Reg->VICVECTADDR18 = PHYIRQ_SDMA1;
	g_pVIC0Reg->VICVECTADDR19 = PHYIRQ_ARM_DMAERR;
	g_pVIC0Reg->VICVECTADDR20 = PHYIRQ_ARM_DMA;
	g_pVIC0Reg->VICVECTADDR21 = PHYIRQ_ARM_DMAS;
	g_pVIC0Reg->VICVECTADDR22 = PHYIRQ_KEYPAD;
	g_pVIC0Reg->VICVECTADDR23 = PHYIRQ_TIMER0;
	g_pVIC0Reg->VICVECTADDR24 = PHYIRQ_TIMER1;
	g_pVIC0Reg->VICVECTADDR25 = PHYIRQ_TIMER2;
	g_pVIC0Reg->VICVECTADDR26 = PHYIRQ_WDT;
	g_pVIC0Reg->VICVECTADDR27 = PHYIRQ_TIMER3;
	g_pVIC0Reg->VICVECTADDR28 = PHYIRQ_TIMER4;
	g_pVIC0Reg->VICVECTADDR29 = PHYIRQ_LCD0_FIFO;
	g_pVIC0Reg->VICVECTADDR30 = PHYIRQ_LCD1_FRAME;
	g_pVIC0Reg->VICVECTADDR31 = PHYIRQ_LCD2_SYSIF;

	// Fill Vector Address of VIC1
	g_pVIC1Reg->VICVECTADDR0 = PHYIRQ_EINT2;
	g_pVIC1Reg->VICVECTADDR1 = PHYIRQ_EINT3;
	g_pVIC1Reg->VICVECTADDR2 = PHYIRQ_PCM0;
	g_pVIC1Reg->VICVECTADDR3 = PHYIRQ_PCM1;
	g_pVIC1Reg->VICVECTADDR4 = PHYIRQ_AC97;
	g_pVIC1Reg->VICVECTADDR5 = PHYIRQ_UART0;
	g_pVIC1Reg->VICVECTADDR6 = PHYIRQ_UART1;
	g_pVIC1Reg->VICVECTADDR7 = PHYIRQ_UART2;
	g_pVIC1Reg->VICVECTADDR8 = PHYIRQ_UART3;
	g_pVIC1Reg->VICVECTADDR9 = PHYIRQ_DMA0;
	g_pVIC1Reg->VICVECTADDR10 = PHYIRQ_DMA1;
	g_pVIC1Reg->VICVECTADDR11 = PHYIRQ_ONENAND0;
	g_pVIC1Reg->VICVECTADDR12 = PHYIRQ_ONENAND1;
	g_pVIC1Reg->VICVECTADDR13 = PHYIRQ_NFC;
	g_pVIC1Reg->VICVECTADDR14 = PHYIRQ_CFC;
	g_pVIC1Reg->VICVECTADDR15 = PHYIRQ_UHOST;
	g_pVIC1Reg->VICVECTADDR16 = PHYIRQ_SPI0;
	g_pVIC1Reg->VICVECTADDR17 = PHYIRQ_SPI1;
	g_pVIC1Reg->VICVECTADDR18 = PHYIRQ_I2C;
	g_pVIC1Reg->VICVECTADDR19 = PHYIRQ_HSITX;
	g_pVIC1Reg->VICVECTADDR20 = PHYIRQ_HSIRX;
	g_pVIC1Reg->VICVECTADDR21 = PHYIRQ_RESERVED;
	g_pVIC1Reg->VICVECTADDR22 = PHYIRQ_MSM;
	g_pVIC1Reg->VICVECTADDR23 = PHYIRQ_HOSTIF;
	g_pVIC1Reg->VICVECTADDR24 = PHYIRQ_HSMMC0;
	g_pVIC1Reg->VICVECTADDR25 = PHYIRQ_HSMMC1;
	g_pVIC1Reg->VICVECTADDR26 = PHYIRQ_OTG;
	g_pVIC1Reg->VICVECTADDR27 = PHYIRQ_IRDA;
	g_pVIC1Reg->VICVECTADDR28 = PHYIRQ_RTC_ALARM;
	g_pVIC1Reg->VICVECTADDR29 = PHYIRQ_SEC;
	g_pVIC1Reg->VICVECTADDR30 = PHYIRQ_PENDN;
	g_pVIC1Reg->VICVECTADDR31 = PHYIRQ_ADC;
}

//------------------------------------------------------------------------------
