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
// -----------------------------------------------------------------------------
//
//      THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//      ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//      THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//      PARTICULAR PURPOSE.
//  
// -----------------------------------------------------------------------------
#include <windows.h>
#include <bsp.h>

VOID BSPPowerOff()
{
	//volatile S3C6410_GPIO_REG *pGPIOReg;
	volatile S3C6410_ADC_REG *pADCReg;
	volatile S3C6410_RTC_REG *pRTCReg;
	//volatile S3C6410_SYSCON_REG *pSysConReg;

	OALMSG(TRUE, (TEXT("++BSPPowerOff()\n")));

	//pGPIOReg = (S3C6410_GPIO_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	pADCReg = (S3C6410_ADC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_ADC, FALSE);
	pRTCReg = (S3C6410_RTC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_RTC, FALSE);
	//pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);

	// RTC Control Disable
	pRTCReg->RTCCON = 0x0;			// Subclk 32768 Hz, No Reset, Merged BCD counter, XTAL 2^-15, Control Disable

	// ADC Standby Mode
	pADCReg->ADCCON |= (1<<2);		// ADC StanbyMode

	//
	//CLRPORT32(&pIOPort->GPGDAT, 1 << 4);

	OALMSG(TRUE, (TEXT("--BSPPowerOff()\n")));
}


VOID BSPPowerOn()
{
	//volatile S3C6410_GPIO_REG *pGPIOReg;
	//volatile S3C6410_ADC_REG *pADCReg;
	//volatile S3C6410_SYSCON_REG *pSysConReg;
	volatile S3C6410_NAND_REG *pNANDReg;

	OALMSG(TRUE, (TEXT("++BSPPowerOn()\n")));

	//pGPIOReg = (S3C6410_GPIO_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	//pADCReg = (S3C6410_ADC_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_ADC, FALSE);
	//pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	pNANDReg = (S3C6410_NAND_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_NFCON, FALSE);


	pNANDReg->NFCONF = (NAND_TACLS  <<  12) | (NAND_TWRPH0 <<  8) | (NAND_TWRPH1 <<  4);
	pNANDReg->NFCONT = (0<<17)|(0<<16)|(0<<10)|(0<<9)|(0<<8)|(1<<7)|(1<<6)|(1<<5)|(1<<4)|(0x3<<1)|(1<<0);
	pNANDReg->NFSTAT = (1<<4);
	// NFCon initialize
	//NAND_Init();

	// UART Debug Port Initialize
	//OEMInitDebugSerial();	// Moved to "OEMPowerOff()"

	// Initialize System Timer
	//InitializeSystemTimer();	// Moved to "OEMPowerOff()"

	//InitLDI_LTV350();	-> should be moved to Display Driver

	// USB2.0 and PHY power
	//pIOPort->MISCCR&=~(0x1<<12); //USB port = suspend
	//pCLKPWR->USB_PHYPWR |= (0xf<<0);
	//pCLKPWR->PWRCFG |= (0x1<<4);
	//pIOPort->GSTATUS2 = pIOPort->GSTATUS2;

	//pIOPort->MISCCR &= ~(1<<12); //USB port0 = normal mode
	//pIOPort->MISCCR &= ~(1<<13); //USB port1 = normal mode

	/* LCD Controller Enable               */
	//SETPORT32(&pIOPort->GPGDAT, 1 << 4);	-> should be moved to Display Driver

	OALMSG(TRUE, (TEXT("--BSPPowerOn()\n")));
}

void BSPConfigGPIOforPowerOff(void)
{
	volatile S3C6410_GPIO_REG *pGPIOReg;

	OALMSG(TRUE, (TEXT("++111BSPConfigGPIOforPowerOff()\n")));
	pGPIOReg = (S3C6410_GPIO_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	pGPIOReg->GPKDAT &=~ (0x1<<12);
	//pGPIOReg->GPKDAT |= (0x1<<12);
	pGPIOReg->GPCCONSLP= (pGPIOReg->GPCCONSLP& ~(0x3<<10)) | (1<<10);
	pGPIOReg->GPCCONSLP= (pGPIOReg->GPCCONSLP& ~(0x3<<12)) | (1<<12);
	pGPIOReg->GPCDAT |= (0x1<<7);
	pGPIOReg->GPCCONSLP= (pGPIOReg->GPCCONSLP& ~(0x3<<14)) | (1<<14);

#if	0
	pGPIOReg = (S3C6410_GPIO_REG*)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);

	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_OP1 IO
	// GPF[7:0], GPG[7:0], GPE[15:14], GPE[4:0]


	//GPF
	pIOPort->GPFCON &=~(0xffff<<0);
	pIOPort->GPFCON |= ((0x1<<14)|(0x1<<12)|(0x1<<10)|(0x1<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x2<<0));
#ifdef		EVT1
	pIOPort->EXTINT0 = (READEXTINT0(pIOPort->EXTINT0) & ~((1<<31) | (1<<27) | (1<<23) | (1<<19) | (1<<15) | (1<<11) | (1<<7) | (1<<3) ));
	pIOPort->EXTINT0 = (READEXTINT0(pIOPort->EXTINT0)  | ((1<<31) | (1<<27) | (1<<23) | (1<<19) | (0<<15) | (1<<11) | (1<<7) | (1<<3) )); //rEXTINT0[11] = PD_dis(Because of VBUS_DET)
#else
	pIOPort->GPFUDP &=~(0xffff<<0);
	pIOPort->GPFUDP |= ((0x2<<14)|(0x2<<12)|(0x2<<10)|(0x2<<8)|(0x1<<6)|(0x1<<4)|(0x2<<2)|(0x1<<0));
#endif
	pIOPort->GPFDAT &=~ (0xff<<0);
	pIOPort->GPFDAT |= ((0x0<<7)|(0x0<<6)|(0x0<<5)|(0x0<<4)|(0x0<<3)|(0x0<<2)|(0x0<<1)|(0x0<<0));

	//GPG
	pIOPort->GPGCON &=~(0xffff<<0);
	pIOPort->GPGCON |= ((0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x2<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));

	pIOPort->GPGUDP &=~(0xffff<<0);
#ifdef		EVT1
	pIOPort->EXTINT1 = (READEXTINT1(pIOPort->EXTINT1) | (0<<31) | (0<<27) | (0<<23) | (0<<19) | (1<<15) | (0<<11) | (0<<7)| (0<<0) );
#else
	pIOPort->GPGUDP |= ((0x2<<14)|(0x2<<12)|(0x1<<10)|(0x1<<8)|(0x1<<6)|(0x1<<4)|(0x1<<2)|(0x1<<0));
#endif

	pIOPort->GPGDAT &=~ (0xff<<0);
	pIOPort->GPGDAT |= ((0x1<<7)|(0x0<<6)|(0x1<<5)|(0x1<<4)|(0x0<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));

	//GPA
#ifdef		EVT1
	pIOPort->GPACDH = (READGPACDH(pIOPort->GPACDL,pIOPort->GPACDH) | (1<<16) | (1<<5));	//GPACDH[16] is exclusive use of nRSTOUT and '1' is PRESET
#else
	pIOPort->GPACON &=~(0x1<<21);
	pIOPort->GPACON |= ((0x0<<21));//Set GPA21 as Output(default : nRSTOUT)

	pIOPort->GPADAT &=~ (0x7<<21);//Clear GPA21~23
	pIOPort->GPADAT |= ((0x1<<21));//Set GPA21(DAT) as '1'
#endif


	// DP0 / DN0
	pIOPort->MISCCR |= (1<<12); //Set USB port as suspend mode


	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_OP2 IO
	// GPB[10:0], GPH[12:0], GPE[15:14], GPE[4:0]


	//GPB
	pIOPort->GPBCON &=~(0x3fffff<<0);
#ifdef	EVT1
	pIOPort->GPBCON |= ((0x0<<20)|(0x0<<18)|(0x0<<16)|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x3<<0));
#else
	pIOPort->GPBCON |= ((0x0<<20)|(0x0<<18)|(0x0<<16)|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x1<<6)|(0x1<<4)|(0x0<<2)|(0x1<<0));
#endif

	pIOPort->GPBUDP = (pIOPort->GPBUDP &=~(0x3fffff<<0));
#ifdef		EVT1
	pIOPort->GPBUDP |= ((0x3<<20)|(0x2<<18)|(0x2<<16)|(0x2<<14)|(0x2<<12)|(0x2<<10)|(0x3<<8)|(0x3<<6)|(0x3<<4)|(0x0<<2)|(0x3<<0));
#else
	pIOPort->GPBUDP |= ((0x1<<20)|(0x1<<18)|(0x1<<16)|(0x1<<14)|(0x1<<12)|(0x1<<10)|(0x0<<8)|(0x2<<6)|(0x2<<4)|(0x1<<2)|(0x2<<0));
#endif


	//GPE
	pIOPort->GPECON &=~((0xf<<28)|(0x3ff<<0));
#ifdef		EVT1
	pIOPort->GPECON |= ((0x0<<30)|(0x0<<28)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#else
	pIOPort->GPECON |= ((0x0<<30)|(0x0<<28)|(0x1<<8)|(0x0<<6)|(0x1<<4)|(0x0<<2)|(0x0<<0));
#endif

	pIOPort->GPEUDP &=~((0xf<<28)|(0x3ff<<0));
#ifdef		EVT1
	pIOPort->GPEUDP |= ((0x0<<30)|(0x0<<28)|(0x0<<8)|(0x3<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#else
	pIOPort->GPEUDP |= ((0x2<<30)|(0x2<<28)|(0x2<<8)|(0x1<<6)|(0x2<<4)|(0x1<<2)|(0x1<<0));
#endif

	pIOPort->GPEDAT &=~ (0xffff<<0);
	pIOPort->GPEDAT |= ((0x1<<15)|(0x1<<14)|(0x1<<4)|(0x1<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));
#ifdef		EVT1
#else
	pIOPort->GPEDAT = 0;
	//printf("rgPEcon=%08x, rgPEUDP=%08x, rGPeDAT=%08x\n", rGPECON,rGPEUDP,rGPEDAT);
#endif


	//GPG
	pIOPort->GPGCON &=~(0xffff<<16);
	pIOPort->GPGCON |= ((0x0<<30)|(0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16));

	pIOPort->GPGUDP &=~(0xffff<<16);
#ifdef		EVT1
	pIOPort->GPGUDP |= ((0x2<<30)|(0x0<<28)|(0x3<<26)|(0x3<<24)|(0x0<<22)|(0x3<<20)|(0x3<<18)|(0x3<<16));
#else
	pIOPort->GPGUDP |= ((0x0<<30)|(0x1<<28)|(0x1<<26)|(0x1<<24)|(0x1<<22)|(0x2<<20)|(0x2<<18)|(0x1<<16));
#endif

	pIOPort->GPGDAT &=~ (0xff<<8);
	pIOPort->GPGDAT |= ((0x0<<15)|(0x0<<14)|(0x0<<13)|(0x0<<12)|(0x0<<11)|(0x0<<10)|(0x0<<9)|(0x0<<8));



	//GPH
	pIOPort->GPHCON &=~(0x3fffffff<<0);
#ifdef		EVT1
	pIOPort->GPHCON |= ((0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#else
	pIOPort->GPHCON |= ((0x1<<28)|(0x0<<26)|(0x0<<24)|(0x1<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#endif

	pIOPort->GPHUDP &=~(0x3fffffff<<0);
#ifdef		EVT1
	pIOPort->GPHUDP |= ((0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x3<<20)|(0x0<<18)|(0x3<<16)|(0x3<<6)|(0x0<<4)|(0x3<<2)|(0x0<<0));
#else
	pIOPort->GPHUDP |= ((0x2<<28)|(0x1<<26)|(0x1<<24)|(0x2<<22)|(0x1<<20)|(0x2<<18)|(0x1<<16)|(0x1<<6)|(0x2<<4)|(0x1<<2)|(0x2<<0));
#endif
	pIOPort->GPHDAT &=~ (0x7fff<<0);
	pIOPort->GPHDAT |= ((0x0<<14)|(0x1<<13)|(0x0<<12)|(0x1<<11)|(0x0<<10)|(0x1<<9)|(0x1<<8)|(0x0<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));



	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_SD IO
	// GPE[13:5], GPL[14:0], GPJ[15:13]

	//GPE
	pIOPort->GPECON &=~(0x3ffff<<10);
	pIOPort->GPECON |= ((0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)|(0x0<<14)|(0x0<<12)|(0x0<<10));//Set Input All

	pIOPort->GPEUDP &=~(0x3ffff<<10);
#ifdef		EVT1
	pIOPort->GPEUDP |= ((0x0<<26)|(0x0<<24)|(0x0<<22)|(0x3<<20)|(0x3<<18)|(0x3<<16)|(0x3<<14)|(0x3<<12)|(0x0<<10));
#else
	pIOPort->GPEUDP |= ((0x1<<26)|(0x1<<24)|(0x1<<22)|(0x2<<20)|(0x2<<18)|(0x2<<16)\
	|(0x2<<14)|(0x2<<12)|(0x1<<10));
#endif

	pIOPort->GPEDAT &=~ (0x1ff<<5);
	pIOPort->GPEDAT |= ((0x0<<13)|(0x0<<12)|(0x0<<11)|(0x0<<10)|(0x1<<9)|(0x1<<8)|(0x0<<7)|(0x1<<6)|(0x1<<5));


	//GPL
	pIOPort->GPLCON &=~(0x3fffffff<<0);
	pIOPort->GPLCON |= ((0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
	pIOPort->GPLUDP &=~(0x3fffffff<<0);
#ifdef		EVT1
	pIOPort->GPLUDP |= ((0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x3<<16)|(0x3<<14)|(0x3<<12)|(0x3<<10)|(0x3<<8)|(0x3<<6)|(0x3<<4)|(0x3<<2)|(0x3<<0));
#else
	pIOPort->GPLUDP |= ((0x1<<28)|(0x1<<26)|(0x1<<24)|(0x1<<22)|(0x1<<20)|(0x1<<18)|(0x2<<16)|(0x2<<14)|(0x2<<12)|(0x2<<10)|(0x2<<8)|(0x1<<6)|(0x2<<4)|(0x2<<2)|(0x2<<0));
#endif
	pIOPort->GPLDAT &=~ (0x1fff<<0);
	pIOPort->GPLDAT |= ((0x0<<14)|(0x0<<13)|(0x0<<12)|(0x0<<11)|(0x0<<10)|(0x1<<9)|(0x1<<8)|(0x1<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));

	//GPJ
	pIOPort->GPJCON &=~(0x3f<<26);
	pIOPort->GPJCON |= ((0x0<<30)|(0x0<<28)|(0x0<<26));

	pIOPort->GPJUDP &=~(0x3f<<26);
#ifdef		EVT1
	pIOPort->GPJUDP |= ((0x0<<30)|(0x0<<28)|(0x0<<26));
#else
	pIOPort->GPJUDP |= ((0x2<<30)|(0x2<<28)|(0x2<<26));
#endif

	pIOPort->GPJDAT &=~ (0x7<<13);
	pIOPort->GPJDAT |= ((0x1<<15)|(0x1<<14)|(0x1<<13));

#ifdef		EVT1
	//GPH - This configuration is setting for CFG3(UART) set as [off:off:off:off]
	pIOPort->GPHCON &= ~((0x3<<14)|(0x3<<12)|(0x3<<10)|(0x3<<8));
	pIOPort->GPHUDP &= ~((0x3<<14)|(0x3<<12)|(0x3<<10)|(0x3<<8));
#endif




	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_LCD IO
	// GPC[15:0], GPD[15:0]

	//GPC
	pIOPort->GPCCON &=~(0xffffffff<<0);
	pIOPort->GPCCON |= ((0x0<<30)|(0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)\
	|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));

	pIOPort->GPCUDP &=~(0xffffffff<<0);
#ifdef		EVT1
	pIOPort->GPCUDP |= ((0x2<<30)|(0x2<<28)|(0x2<<26)|(0x2<<24)|(0x2<<22)|(0x2<<20)|(0x2<<18)|(0x2<<16)
	|(0x2<<14)|(0x2<<12)|(0x2<<10)|(0x2<<8)|(0x2<<6)|(0x2<<4)|(0x2<<2)|(0x2<<0));
#else
	pIOPort->GPCUDP |= ((0x0<<30)|(0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)
	|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#endif

	pIOPort->GPCDAT &=~ (0x1fff<<0);
	pIOPort->GPCDAT |= ((0x0<<14)|(0x1<<13)|(0x0<<12)|(0x1<<11)|(0x0<<10)|(0x1<<9)|(0x1<<8)|(0x0<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));

	//GPD
	pIOPort->GPDCON &=~(0xffffffff<<0);
	pIOPort->GPDCON |= ((0x0<<30)|(0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)\
	|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));


	pIOPort->GPDUDP &=~(0xffffffff<<0);
#ifdef		EVT1
	pIOPort->GPDUDP |= ((0x2<<30)|(0x2<<28)|(0x2<<26)|(0x2<<24)|(0x2<<22)|(0x2<<20)|(0x2<<18)|(0x2<<16)\
	|(0x2<<14)|(0x2<<12)|(0x2<<10)|(0x2<<8)|(0x2<<6)|(0x2<<4)|(0x2<<2)|(0x2<<0));
#else
	pIOPort->GPDUDP |= ((0x0<<30)|(0x0<<28)|(0x0<<26)|(0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)\
	|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#endif

	pIOPort->GPDDAT &=~ (0xffff<<0);
	pIOPort->GPDDAT |= ((0x0<<14)|(0x1<<13)|(0x0<<12)|(0x1<<11)|(0x0<<10)|(0x1<<9)|(0x1<<8)|(0x0<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));


	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_CAM IO
	// GPJ[15:0],

	//GPJ
	pIOPort->GPJCON &=~(0x3ffffff<<0);
	pIOPort->GPJCON |= ((0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)\
	|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));

	pIOPort->GPJUDP &=~(0x3ffffff<<0);
#ifdef		EVT1
	pIOPort->GPJUDP |= ((0x2<<24)|(0x2<<22)|(0x2<<20)|(0x2<<18)|(0x2<<16)\
	|(0x2<<14)|(0x2<<12)|(0x2<<10)|(0x2<<8)|(0x2<<6)|(0x2<<4)|(0x2<<2)|(0x2<<0));
#else
	pIOPort->GPJUDP |= ((0x0<<24)|(0x0<<22)|(0x0<<20)|(0x0<<18)|(0x0<<16)\
	|(0x0<<14)|(0x0<<12)|(0x0<<10)|(0x0<<8)|(0x0<<6)|(0x0<<4)|(0x0<<2)|(0x0<<0));
#endif

	pIOPort->GPJDAT &=~ (0x1fff<<0);
	pIOPort->GPJDAT |= ((0x0<<12)|(0x1<<11)|(0x0<<10)|(0x1<<9)|(0x1<<8)\
	|(0x0<<7)|(0x1<<6)|(0x1<<5)|(0x1<<4)|(0x0<<3)|(0x1<<2)|(0x1<<1)|(0x1<<0));

	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_RMOP
	pIOPort->MSLCON = 0x0;
#ifdef		EVT1
	pIOPort->GPMCON &=~((0x3<<2) | (0x3<<0));
	pIOPort->GPMUDP &=~((0x3<<4) |(0x3<<2) | (0x3<<0));
	pIOPort->GPMUDP |= ((0x3<<4) | (0x3<<2) | (0x3<<0));
#endif

	///////////////////////////////////////////////////////////////////////////////////////////
	// VDD_SMOP : sleep wakeup iteration fail or not?
	pIOPort->MSLCON = 0x0;
#ifdef		EVT1
	pIOPort->DATAPDEN &=~((0x1<<0)|(0x1<<1)|(0x1<<2)|(0x1<<3)|(0x1<<4)|(0x1<<5)); // reset value = 0x3f; --> 0x30 = 2uA
	pIOPort->DATAPDEN = (0x3<<4);
#else
	pIOPort->GPKUDP &=~((0x1<<0)|(0x1<<1)|(0x1<<2)|(0x1<<3)|(0x1<<4)|(0x1<<5)); // reset value = 0x3f; --> 0x30 = 2uA
	pIOPort->GPKUDP |= (0x3<<4);
#endif

#endif
	OALMSG(TRUE, (TEXT("--BSPConfigGPIOforPowerOff()\n")));
}

