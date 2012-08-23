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
//  Header: oal_intr.h
//
//  This header define OAL interrupt module. Module code implements all
//  functionality related to interrupt management.
//
//  Depending on CPU/SoC model the module can be implemented with platform
//  callbacks. This model is usefull on hardware where external secondary
//  interrupt controller is used.
//
//  Export for kernel/public interface:
//      * Interrupt handlers/SYS_INTR_XXXX
//      * OEMInterruptEnable
//      * OEMInterruptDisable
//      * OEMInterruptDone
//      * OEMIoControl/IOCTL_HAL_REQUEST_SYSINTR
//      * OEMIoControl/IOCTL_HAL_RELEASE_SYSINTR
//      * OEMIoControl/IOCTL_HAL_REQUEST_IRQ
//
//  Export for other OAL modules/protected interface:
//      * OALIntrInit (initialization)
//      * OALIntrStaticTranslate (initialization)
//      * OALIntrRequestSysIntr (KITL)
//      * Interrupt handler (timer)
//      * OAL_INTR_IRQ_MAXIMUM (power)
//      * OALPAtoVA (memory)
//
//  Internal module functions:
//      * OALIntrMapInit
//      * OALIntrReleaseSysIntr
//      * OALIntrTranslateIrq
//      * OALIntrTranslateSysIntr
//
//  Platform callback module functions:
//      * BSPIntrInit
//      * BSPIntrRequestIrqs
//      * BSPIntrEnableIrq
//      * BSPIntrDisableIrq
//      * BSPIntrDoneIrq
//      * BSPActiveIrq
//
//
#ifndef __OAL_INTR_H
#define __OAL_INTR_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
//
//  Define: IRQ_XXX
//
//  Virtual Interrupt Sources Numbers
//  Platten Physical IRQ Numbers
//
//  Drivers and Application have to use this Virtual IRQ Numbers
//

#define IRQ_EINT0			0	// 0
#define IRQ_EINT1			1	// 0
#define IRQ_EINT2			2	// 0
#define IRQ_EINT3			3	// 0
#define IRQ_EINT4			4	// 1
#define IRQ_EINT5			5	// 1
#define IRQ_EINT6			6	// 1
#define IRQ_EINT7			7	// 1
#define IRQ_EINT8			8	// 1
#define IRQ_EINT9			9	// 1

#define IRQ_EINT10			10	// 1
#define IRQ_EINT11			11	// 1
#define IRQ_RTC_TIC			12	// 2
#define IRQ_CAMIF_C			13	// 3
#define IRQ_CAMIF_P			14	// 4
#define IRQ_I2C1			15	// 5
#define IRQ_I2S_V40			16   // 6
#define IRQ_GPS				17   // 7
#define IRQ_3D				18   // 8
#define IRQ_POST			19	// 9

#define IRQ_ROTATOR			20	// 10
#define IRQ_2D				21	// 11
#define IRQ_TVENC			22	// 12
#define IRQ_TVSCALER		23	// 13
#define IRQ_BATF			24	// 14
#define IRQ_JPEG			25	// 15
#define IRQ_MFC				26	// 16
#define IRQ_SDMA0_CH0			27	// 17
#define IRQ_SDMA0_CH1			28	// 17
#define IRQ_SDMA0_CH2			29	// 17

#define IRQ_SDMA0_CH3			30	// 17
#define IRQ_SDMA0_CH4			31	// 17
#define IRQ_SDMA0_CH5			32	// 17
#define IRQ_SDMA0_CH6			33	// 17
#define IRQ_SDMA0_CH7			34	// 17
#define IRQ_SDMA1_CH0			35	// 18
#define IRQ_SDMA1_CH1			36	// 18
#define IRQ_SDMA1_CH2			37	// 18
#define IRQ_SDMA1_CH3			38	// 18
#define IRQ_SDMA1_CH4			39	// 18

#define IRQ_SDMA1_CH5			40	// 18
#define IRQ_SDMA1_CH6			41	// 18
#define IRQ_SDMA1_CH7			42	// 18
#define IRQ_ARM_DMAERR		43	// 19
#define IRQ_ARM_DMA		    44	// 20
#define IRQ_ARM_DMAS		45	// 21
#define IRQ_KEYPAD			46	// 22
#define IRQ_TIMER0			47	// 23
#define IRQ_TIMER1			48	// 24
#define IRQ_TIMER2			49	// 25

#define IRQ_WDT				50	// 26
#define IRQ_TIMER3			51	// 27
#define IRQ_TIMER4			52	// 28
#define IRQ_LCD0_FIFO		53	// 29
#define IRQ_LCD1_FRAME		54	// 30
#define IRQ_LCD2_SYSIF		55	// 31
#define IRQ_EINT12			56	// 32
#define IRQ_EINT13			57	// 32
#define IRQ_EINT14			58	// 32
#define IRQ_EINT15			59	// 32

#define IRQ_EINT16			60	// 32
#define IRQ_EINT17			61	// 32
#define IRQ_EINT18			62	// 32
#define IRQ_EINT19			63	// 32
#define IRQ_EINT20			64	// 33
#define IRQ_EINT21			65	// 33
#define IRQ_EINT22			66	// 33
#define IRQ_EINT23			67	// 33
#define IRQ_EINT24			68	// 33
#define IRQ_EINT25			69	// 33

#define IRQ_EINT26			70	// 33
#define IRQ_EINT27			71	// 33
#define IRQ_PCM0			72	// 34
#define IRQ_PCM1			73	// 35
#define IRQ_AC97			74	// 36
#define IRQ_UART0			75	// 37
#define IRQ_UART1			76	// 38
#define IRQ_UART2			77	// 39
#define IRQ_UART3			78	// 40
#define IRQ_DMA0_CH0		79	// 41

#define IRQ_DMA0_CH1		80	// 41
#define IRQ_DMA0_CH2		81	// 41
#define IRQ_DMA0_CH3		82	// 41
#define IRQ_DMA0_CH4		83	// 41
#define IRQ_DMA0_CH5		84	// 41
#define IRQ_DMA0_CH6		85	// 41
#define IRQ_DMA0_CH7		86	// 41
#define IRQ_DMA1_CH0		87	// 42
#define IRQ_DMA1_CH1		88	// 42
#define IRQ_DMA1_CH2		89	// 42

#define IRQ_DMA1_CH3		90	// 42
#define IRQ_DMA1_CH4		91	// 42
#define IRQ_DMA1_CH5		92	// 42
#define IRQ_DMA1_CH6		93	// 42
#define IRQ_DMA1_CH7		94	// 42
#define IRQ_ONENAND0		95	// 43
#define IRQ_ONENAND1		96	// 44
#define IRQ_NFC				97	// 45
#define IRQ_CFC				98	// 46
#define IRQ_UHOST			99	// 47

#define IRQ_SPI0			100	// 48
#define IRQ_SPI1			101	// 49
#define IRQ_I2C				102	// 50
#define IRQ_HSITX			103	// 51
#define IRQ_HSIRX			104	// 52
#define IRQ_RESERVED		105	// 53
#define IRQ_MSM				106	// 54
#define IRQ_HOSTIF			107	// 55
#define IRQ_HSMMC0			108	// 56
#define IRQ_HSMMC1			109	// 57

#define IRQ_OTG				110	// 58
#define IRQ_IRDA			111	// 59
#define IRQ_RTC_ALARM		112	// 60
#define IRQ_SEC				113	// 61
#define IRQ_PENDN			114	// 62
#define IRQ_ADC				115	// 63

#define IRQ_MAX_S3C6410	116

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//
//  Define:  IRQ_MAXIMUM
//
//  This value define maximum number of IRQs. Even if there isn't any
//  limitation for this number in OAL library, Windows CE resource manager
//  support only 64 IRQs currently.
//
#define OAL_INTR_IRQ_MAXIMUM		IRQ_MAX_S3C6410

//------------------------------------------------------------------------------
//
//  Define:  IRQ_UNDEFINED
//
//  Invalid IRQ number used to verify SYSINTR/IRQ translations
//
#define OAL_INTR_IRQ_UNDEFINED	(-1)

//------------------------------------------------------------------------------
//
//  Define:  OAL_INTR_STATIC/OAL_INTR_FORCE_STATIC
//
//  This constants are used to specify type of IRQ to SYSINTR mapping sets
//  in OALIntrRequestSysIntr function. When none flag is set backward
//  compatible method is used (first request for given IRQ creates static
//  mapping). Flag OAL_INTR_TRANSLATE allows to use obsolete
//  IOCTL_HAL_TRANSLATE_IRQ semantic (if static mapping exists return mapped
//  SYSINTR otherwise create new mapping).
//
#define OAL_INTR_DYNAMIC			(1 << 0)
#define OAL_INTR_STATIC				(1 << 1)
#define OAL_INTR_FORCE_STATIC		(1 << 2)
#define OAL_INTR_TRANSLATE			(1 << 3)

//------------------------------------------------------------------------------
//
//  Function: OALIoCtlHalRequestSysIntr
//
//  This function is called form OEMIoControl for IOCTL_HAL_REQUEST_SYSINTR.
//
BOOL OALIoCtlHalRequestSysIntr(UINT32, VOID*, UINT32, VOID*, UINT32, UINT32*);

//------------------------------------------------------------------------------
//
//  Function: OALIoCtlHalReleaseSysIntr
//
//  This function is called form OEMIoControl for IOCTL_HAL_RELEASE_SYSINTR.
//
BOOL OALIoCtlHalReleaseSysIntr(UINT32, VOID*, UINT32, VOID*, UINT32, UINT32*);

//------------------------------------------------------------------------------
//
//  Function: OALIoCtlHalRequestIrq
//
//  This function is called form OEMIoControl for IOCTL_HAL_REQUEST_IRQ.
//
BOOL OALIoCtlHalRequestIrq(UINT32, VOID*, UINT32, VOID*, UINT32, UINT32*);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrInit/BSPIntrInit
//
//  This function initialize interrupt hardware. It is usally called at system
//  initialization. If implementation uses platform callback it will call
//  BPSIntrInit.
//
BOOL OALIntrInit();
BOOL BSPIntrInit();

//------------------------------------------------------------------------------
//
//  Function:  OALIntrMapInit
//
//  This function must be called from OALIntrInit to initialize IRQ/SYSINTR
//  mapping structure.
//
VOID OALIntrMapInit();

//------------------------------------------------------------------------------
//
//  Function:  OALIntrRequestSysIntr
//
//  This function allocate new SYSINTR value for given IRQ list. Based
//  on flags parameter it also can set backward static mapping (IRQ -> SYSINTR)
//  see OAL_INTR_xxx above.
//
UINT32 OALIntrRequestSysIntr(UINT32 count, const UINT32 *pIrqs, UINT32 flags);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrReleaseSysIntr
//
//  This function release SYSINTR. It also clears backward static mapping
//  (IRQ -> SYSINTR) if it exists.
//
BOOL OALIntrReleaseSysIntr(UINT32 sysIntr);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrStaticTranslate
//
//  This function set static IRQ <-> SYSINTR mapping for given values. It is
//  typically used in system initialization for legacy drives.
//
VOID OALIntrStaticTranslate(UINT32 sysIntr, UINT32 irq);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrTranslateSysIntr
//
//  This function return list of IRQs for give SYSINTR.
//
BOOL OALIntrTranslateSysIntr(
    UINT32 sysIntr, UINT32 *pCount, const UINT32 **ppIrqs
);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrRequestIrq
//
//  This function return SYSINTR for given IRQ based on static mapping.
//
UINT32 OALIntrTranslateIrq(UINT32 irq);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrRequestIrq/BSPIntrRequestIrq
//
//  This function return list of IRQs for device identified by DEVICE_LOCATION
//  parameter. If implementation uses platform callbacks it should call
//  BSPIntrRequestIrq  in case that it doesn't recognise device.
//  On input *pCount contains maximal number of IRQs allowed in list. On return
//  it returns number of IRQ in list.
//
BOOL OALIntrRequestIrqs(DEVICE_LOCATION *pDevLoc, UINT32 *pCount, UINT32 *pIrqs);
BOOL BSPIntrRequestIrqs(DEVICE_LOCATION *pDevLoc, UINT32 *pCount, UINT32 *pIrqs);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrEnableIrqs/BSPIntrEnableIrqs
//
//  This function enable list of interrupts identified by IRQ. If
//  implementation uses platform callbacks OALIntrEnableIrqs must call
//  BSPIntrEnableIrq for IRQ before it enables this IRQ. The BSPIntrEnableIrq
//  can do board specific action and optionaly modify IRQ.
//
BOOL OALIntrEnableIrqs(UINT32 count, const UINT32 *pIrqs);
UINT32 BSPIntrEnableIrq(UINT32 irq);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrDisableIrqs/BSPIntrDisableIrqs
//
//  This function disable list of interrupts identified by IRQ. If
//  implementation uses platform callbacks OALIntrDisableIrqs must call
//  BSPIntrDisableIrq for each IRQ in list before it disables this IRQ.
//  The BSPIntrEnableIrq can do board specific action and optionaly modify IRQ.
//
VOID OALIntrDisableIrqs(UINT32 count, const UINT32 *pIrqs);
UINT32 BSPIntrDisableIrq(UINT32 irq);

//------------------------------------------------------------------------------
//
//  Function:  OALIntrDoneIrqs/BSPIntrDoneIrqs
//
//  This function finish list of interrupts identified by IRQ. If
//  implementation uses platform callbacks OALIntrDoneIrqs must call
//  BSPIntrDoneIrq for each IRQ in list before it re-enable this IRQ.
//  The BSPIntrDoneIrq can do board specific action and optionaly modify IRQ.
//
VOID OALIntrDoneIrqs(UINT32 count, const UINT32 *pIrqs);
UINT32 BSPIntrDoneIrq(UINT32 irq);

//------------------------------------------------------------------------------
//
//  Function:  BSPIntrActiveIrq
//
//  This function is called from interrupt handler if implementation uses
//  platform callbacks. It allows translate IRQ for chaining controller
//  interrupt.
//
UINT32 BSPIntrActiveIrq(UINT32 irq);

//------------------------------------------------------------------------------

#if __cplusplus
}
#endif

#endif

