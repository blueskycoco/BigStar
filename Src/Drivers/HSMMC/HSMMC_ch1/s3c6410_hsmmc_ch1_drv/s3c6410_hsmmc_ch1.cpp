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

#include <windows.h>
#include <nkintr.h>
#include <ceddk.h>
#include <s3c6410.h>
#include <DrvLib.h>
#include <cebuscfg.h>
#include "s3c6410_hsmmc_ch1.h"

#define _SRCCLK_48MHZ_	// from USB PHY (Keep sync with "sdhcslot.cpp")
#define _DBG_ 0         // a MACRO for debug message enable

BOOL CSDHController::Init(LPCTSTR pszActiveKey) {
	RETAILMSG(1,(TEXT("[HSMMC1] Initializing the HSMMC Host Controller\n")));

	// 08.03.14 by KYS
	// HSMMC Ch1 initialization
	if (!InitCh()) return FALSE;
	return CSDHCBase::Init(pszActiveKey);
}

VOID CSDHController::PowerUp() {
	RETAILMSG(1,(TEXT("[HSMMC1] Power Up the HSMMC Host Controller\n")));

	// 08.03.14 by KYS
	// HSMMC Ch1 initialization for "WakeUp"
	if (!InitCh()) return;
	CSDHCBase::PowerUp();
}

extern "C" PCSDHCBase CreateHSMMCHCObject() {
	return new CSDHController;
}

VOID CSDHController::DestroyHSMMCHCObject(PCSDHCBase pSDHC) {
	DEBUGCHK(pSDHC);
	delete pSDHC;
}

// 08.03.14 by KYS
// The function that initilize SYSCON for a clock gating.
BOOL CSDHController::InitClkPwr() {
	volatile S3C6410_SYSCON_REG *pCLKPWR = NULL;

	pCLKPWR = (volatile S3C6410_SYSCON_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_SYSCON, sizeof(S3C6410_SYSCON_REG), FALSE);
	if (pCLKPWR == NULL) {
		RETAILMSG(1, (TEXT("[HSMMC1] Clock & Power Management Special Register is *NOT* mapped.\n")));
		return FALSE;
	}

#ifdef _SRCCLK_48MHZ_
	RETAILMSG(_DBG_, (TEXT("[HSMMC1] Setting registers for the USB48MHz (EXTCLK for SDCLK) : SYSCon.\n")));
	// SCLK_HSMMC#_48 : CLK48M_PHY(OTH PHY 48MHz Clock Source from SYSCON block)
	// To use the USB clock, must be set the "USB_SIG_MASK" bit in the syscon register.
	pCLKPWR->OTHERS    |= (0x1<<16);  // set USB_SIG_MASK
	pCLKPWR->HCLK_GATE |= (0x1<<18);	// Gating HCLK for HSMMC1
	pCLKPWR->SCLK_GATE |= (0x1<<28);	// Gating special clock for HSMMC1 (SCLK_MMC1_48)
#else
	RETAILMSG(_DBG_, (TEXT("[HSMMC1] Setting registers for the EPLL (for SDCLK) : SYSCon.\n")));
	// SCLK_HSMMC#  : EPLLout, MPLLout, PLL_source_clk or CLK27 clock
	// (from SYSCON block, can be selected by MMC#_SEL[1:0] fields of the CLK_SRC register in SYSCON block)
	// Set the clock source to EPLL out for CLKMMC1
	pCLKPWR->CLK_SRC   = (pCLKPWR->CLK_SRC & ~(0x3<<20) & ~(0x1<<2)) |  // Control MUX(MMC1:MOUT EPLL)
		(0x1<<2); // Control MUX(EPLL:FOUT EPLL)
	pCLKPWR->HCLK_GATE |= (0x1<<18);  // Gating HCLK for HSMMC1
	pCLKPWR->SCLK_GATE  = (pCLKPWR->SCLK_GATE & ~(0x3F<<24)) | (0x1<<25);  // Gating special clock for HSMMC1 (SCLK_MMC1)
#endif

	DrvLib_UnmapIoSpace((PVOID)pCLKPWR);
	return TRUE;
}

// 08.03.14 by KYS
// The function that initilize GPIO for DAT, CD and WP lines.
BOOL CSDHController::InitGPIO() {
	volatile S3C6410_GPIO_REG *pIOPreg = NULL;

	pIOPreg = (volatile S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
	if (pIOPreg == NULL) {
		RETAILMSG(1, (TEXT("[HSMMC1] GPIO registers is *NOT* mapped.\n")));
		return FALSE;
	}
	RETAILMSG(_DBG_, (TEXT("[HSMMC1] Setting registers for the GPIO.\n")));
#ifdef _SMDK6410_CH1_8BIT_
	pIOPreg->GPHCON0 = (pIOPreg->GPHCON0 & ~(0xFFFFFFFF)) | (0x22222222);  // 4'b0010 for the MMC 1
	pIOPreg->GPHCON1 = (pIOPreg->GPHCON1 &~(0xFF)) | (0x22);               // 4'b0010 for the MMC 1
	pIOPreg->GPHPUD &= ~(0xFFFFF); // Pull-up/down disabled
#else // DAT 4-Bit
	pIOPreg->GPHCON0 = (pIOPreg->GPHCON0 & ~(0xFFFFFF)) | (0x222222);  // 4'b0010 for the MMC 1
	pIOPreg->GPHPUD &= ~(0xFFF); // Pull-up/down disabled
#endif
#ifdef _SMDK6410_CH1_WP_
	pIOPreg->GPFCON &= ~(0x3<<26);  // WP_SD1
	pIOPreg->GPFPUD &= ~(0x3<<26);  // Pull-up/down disabled
#endif
	pIOPreg->GPGCON  = (pIOPreg->GPGCON & ~(0xF<<24)) | (0x3<<24); // MMC CDn1
	pIOPreg->GPGPUD &= ~(0x3<<12); // Pull-up/down disabled

	DrvLib_UnmapIoSpace((PVOID)pIOPreg);
	return TRUE;
}

// 08.03.14 by KYS
// The function that initilize the register for HSMMC Control.
BOOL CSDHController::InitHSMMC() {
	volatile S3C6410_HSMMC_REG *pHSMMC = NULL;

	pHSMMC = (volatile S3C6410_HSMMC_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_HSMMC1, sizeof(S3C6410_HSMMC_REG), FALSE);
	if (pHSMMC == NULL) {
		RETAILMSG(1, (TEXT("[HSMMC1] HSMMC Special Register is *NOT* mapped.\n")));
		return FALSE;
	}

#ifdef _SRCCLK_48MHZ_
	RETAILMSG(_DBG_, (TEXT("[HSMMC1] Setting registers for the USB48MHz (EXTCLK) : HSMMCCon.\n")));
	// Set the clock source to USB_PHY for CLKMMC1
	pHSMMC->CONTROL2 = (pHSMMC->CONTROL2 & ~(0xffffffff)) |
		(0x3<<9) |  // Debounce Filter Count 0x3=64 iSDCLK
		(0x1<<8) |  // SDCLK Hold Enable
		(0x3<<4);   // Base Clock Source = External Clock
#else
	RETAILMSG(_DBG_, (TEXT("[HSMMC1] Setting registers for the EPLL : HSMMCCon.\n")));
	// Set the clock source to EPLL out for CLKMMC1
	pHSMMC->CONTROL2 = (pHSMMC->CONTROL2 & ~(0xffffffff)) |
		(0x3<<9) |  // Debounce Filter Count 0x3=64 iSDCLK
		(0x1<<8) |  // SDCLK Hold Enable
		(0x2<<4);   // Base Clock Source = EPLL out
#endif

	DrvLib_UnmapIoSpace((PVOID)pHSMMC);
	return TRUE;
}

BOOL CSDHController::InitCh() {
	if (!InitClkPwr()) return FALSE;
	if (!InitGPIO()) return FALSE;
	if (!InitHSMMC()) return FALSE;
	return TRUE;
}

