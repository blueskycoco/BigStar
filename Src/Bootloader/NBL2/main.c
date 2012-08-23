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

Module Name: main.c

Abstract: OEM IPL routines for the Samsung SMDK6410 hardware platform.

Functions:

Notes:

--*/
#include <windows.h>
#include <bsp.h>
#include <pehdr.h>
#include <romldr.h>
#include <loader.h>
#include <fmd.h>

#include "s3c6410_ldi.h"
#include "s3c6410_display_con.h"
//#include "samsung.c"

ROMHDR * volatile const pTOC = (ROMHDR *)-1;


// Base address of the image flash part.
//
// NOTE: This is the base address of the NAND flash controller...
//
#define EBOOT_VIRTUAL_BASEADDR	IMAGE_EBOOT_ADDR_VA
#define IPL_VIRTUAL_BASEADDR		IMAGE_IPL_ADDR_VA


void Launch(DWORD dwLaunchAddr);
void OEMInitDebugSerial(void);
void OEMWriteDebugString(unsigned short *str);
void delay()
{
	volatile long i,j,k;
	for(i=0;i<20000;i++)
		for(j=0;j<500;j++)
			k=1;
}

static void DisableVIC(void)
{
    volatile S3C6410_VIC_REG *pVIC0Reg = NULL, *pVIC1Reg = NULL;
    const DWORD dwAllF = 0xffffffff;
    DWORD dwRCnt = 10;  // Retry count

    pVIC0Reg = (volatile S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC0, FALSE);
    if (pVIC0Reg == NULL)
    {
        OEMWriteDebugString(L"[NBL2:ERR] DisableVIC: Failed to OALPAtoVA for the VIC0!!\r\n");
        goto CleanUp;
    }

    pVIC1Reg = (volatile S3C6410_VIC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_VIC1, FALSE);
    if (pVIC1Reg == NULL)
    {
        OEMWriteDebugString(L"[NBL2:ERR] DisableVIC: Failed to OALPAtoVA for the VIC1!!\r\n");
        goto CleanUp;
    }

    do
    {
        // Clear all bits in the VICINTENABLE Registers.
        pVIC0Reg->VICINTENCLEAR = dwAllF;
        pVIC1Reg->VICINTENCLEAR = dwAllF;

        // Check
        if (((pVIC0Reg->VICINTENABLE) | (pVIC1Reg->VICINTENABLE)) == 0)
            break;
    } while (dwRCnt--);

    if (dwRCnt == 0)
        goto CleanUp;

    OEMWriteDebugString(L"INFO: DisableVIC: Succeeded.\r\n");
    return;

    CleanUp:
    OEMWriteDebugString(L"[NBL2:ERR] DisableVIC: Failed!!\r\n");
    return;
}


DWORD SetKMode (DWORD fMode)
{
    return(1);
}

void OEMLaunchImage(UINT32 ulLaunchAddr)
{
    UINT32 ulPhysicalJump = 0;

    // Disable all of the interrupt enable before the launch to prevent an interrupt occurrence while the MMU is disabled.
    //
    DisableVIC();

    // The IPL is running with the MMU on - before we jump to the loaded image, we need to convert
    // the launch address to a physical address and turn off the MMU.
    //

    // Convert jump address to a physical address.
    ulPhysicalJump = OALVAtoPA((void *)ulLaunchAddr);

	RETAILMSG(1, (L"LaunchAddr=0x%x PhysicalJump=0x%x\r\n", ulLaunchAddr, ulPhysicalJump));
    // Jump...
    Launch((DWORD)ulPhysicalJump);
}

// --- These are used by iplcommon (end)   ---

BOOL ReadBlock(DWORD dwBlock, LPBYTE pbBlock, PFlashInfo pflashInfo)
{
	WORD iSector;
	int i;
    for (iSector = 0; iSector < pflashInfo->wSectorsPerBlock; iSector++) {
        if (!FMD_ReadSector(dwBlock * pflashInfo->wSectorsPerBlock + iSector, pbBlock, NULL, 1))
            return FALSE;
	/*else{
		for(i=0;i<2048;i++)
				RETAILMSG(1, (L"%x ",*(pbBlock+i)));
			RETAILMSG(1, (L"\r\n"));

		}*/
        if (pbBlock)
            pbBlock += pflashInfo->wDataBytesPerSector;
    }

    return TRUE;
}

static BOOL
_ReadImage(UINT32 nStartVbn, UINT32 nNumOfBlks, UINT32 nRsvNumOfBlks, UINT32 nSrcAddr)
{
    DWORD       dwBlock = nStartVbn;
    UINT8      *pSrcAddr = (UINT8*)nSrcAddr;
    UINT32      nTotNumOfBlks;
    FlashInfo   flashInfo;
    UINT32      nBlkStat;
    INT32       nRet = TRUE;

    nTotNumOfBlks = nNumOfBlks + nRsvNumOfBlks;
    RETAILMSG(1, (L"_ReadImage nStartVbn %x, nNumOfBlks %x , nRsvNumOfBlks %x, nSrcAddr %x.\r\n",nStartVbn,nNumOfBlks,nRsvNumOfBlks,nSrcAddr));

    if (!FMD_GetInfo(&flashInfo))
    {
        OEMWriteDebugString(L"FMD_GetInfo Error\r\n");
    }

    while(nNumOfBlks--)
    {
		if (dwBlock >= (nStartVbn+nTotNumOfBlks))
		{
			nRet = FALSE;
			break;
		}

		nBlkStat = FMD_GetBlockStatus(dwBlock);
		if ((nBlkStat & BLOCK_STATUS_BAD) && !(nBlkStat & BLOCK_STATUS_RESERVED))
		{
			OEMWriteDebugString(L"Block is bad\r\n");
			dwBlock++;
			nNumOfBlks++;
			continue;
		}

		nRet = ReadBlock(dwBlock, pSrcAddr, &flashInfo);

		dwBlock++;
		pSrcAddr += (flashInfo.dwBytesPerBlock);
    }

    return nRet;
}
void ShadowLogo()
{
	UINT32			nVol = 0;
    UINT32          nStartVbn;
    UINT32          nNumOfBlks;
    UINT32          nRsvNumOfBlks;
	UINT32 ulLaunchAddr = 0x8e800000;

    OEMWriteDebugString(L"++ Shadow Logo ++\r\n");

	nStartVbn  = LOGO_BLOCK;
	nNumOfBlks = LOGO_BLOCK_SIZE;
	nRsvNumOfBlks = LOGO_BLOCK_RESERVED;

    if(!_ReadImage(nStartVbn, nNumOfBlks, nRsvNumOfBlks, ulLaunchAddr)) {
	    OEMWriteDebugString(L"In Shadow Logo,can't read image.\r\n");
	    while(1);
    }

    OEMWriteDebugString(L"-- Shadow Logo --\r\n");
	//OEMLaunchImage(ulLaunchAddr);
}

void ShadowEboot()
{
	UINT32			nVol = 0;
    UINT32          nStartVbn;
    UINT32          nNumOfBlks;
    UINT32          nRsvNumOfBlks;
	UINT32 ulLaunchAddr = EBOOT_VIRTUAL_BASEADDR;

    OEMWriteDebugString(L"++ Shadow Eboot ++\r\n");

	nStartVbn  = EBOOT_BLOCK;
	nNumOfBlks = EBOOT_BLOCK_SIZE;
	nRsvNumOfBlks = EBOOT_BLOCK_RESERVED;

    if(!_ReadImage(nStartVbn, nNumOfBlks, nRsvNumOfBlks, ulLaunchAddr)) {
	    OEMWriteDebugString(L"In ShadowEboot,can't read image.\r\n");
	    while(1);
    }

    OEMWriteDebugString(L"-- Shadow Eboot --\r\n");
	OEMLaunchImage(ulLaunchAddr);
}

void ShadowIPL()
{
	UINT32			nVol = 0;
    UINT32          nStartVbn;
    UINT32          nNumOfBlks;
    UINT32          nRsvNumOfBlks;
	UINT32 ulLaunchAddr = IPL_VIRTUAL_BASEADDR;

    OEMWriteDebugString(L"++ Shadow IPL ++\r\n");

	nStartVbn  = IPL_BLOCK;
	nNumOfBlks = IPL_BLOCK_SIZE;
	nRsvNumOfBlks = IPL_BLOCK_RESERVED;

    if(!_ReadImage(nStartVbn, nNumOfBlks, nRsvNumOfBlks, ulLaunchAddr)) {
	    OEMWriteDebugString(L"In ShadowIPL,can't read image.\r\n");
	    while(1);
    }

    OEMWriteDebugString(L"-- Shadow IPL --\r\n");
	OEMLaunchImage(ulLaunchAddr);
}

static void InitializeDisplay(void)
{
	tDevInfo RGBDevInfo;

	volatile S3C6410_GPIO_REG *pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
	volatile S3C6410_DISPLAY_REG *pDispReg = (S3C6410_DISPLAY_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_DISPLAY, FALSE);
	volatile S3C6410_SPI_REG *pSPIReg = (S3C6410_SPI_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SPI0, FALSE);
	volatile S3C6410_MSMIF_REG *pMSMIFReg = (S3C6410_MSMIF_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_MSMIF_SFR, FALSE);

	volatile S3C6410_SYSCON_REG *pSysConReg = (S3C6410_SYSCON_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_SYSCON, FALSE);
	RETAILMSG(1,(L"[IPL] ++InitializeDisplay()\r\n"));
	// Initialize Display Power Gating
	if(!(pSysConReg->BLK_PWR_STAT & (1<<4))) {
		pSysConReg->NORMAL_CFG |= (1<<14);
		while(!(pSysConReg->BLK_PWR_STAT & (1<<4)));
		}
	/*
	pGPIOReg->GPFDAT &= ~(1<<14);
	pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<28)) | (1<<28); // LCD EN
	pGPIOReg->GPFDAT |= (1<<15);
	pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<28)) | (1<<28); // LCD EN
	*/
	
	//delay();
	//delay();

	// Initialize Virtual Address
	LDI_initialize_register_address((void *)pSPIReg, (void *)pDispReg, (void *)pGPIOReg);
	Disp_initialize_register_address((void *)pDispReg, (void *)pMSMIFReg, (void *)pGPIOReg);

	// Set LCD Module Type
#if		(SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
	LDI_set_LCD_module_type(LDI_LTS222QV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_LTV350)
	LDI_set_LCD_module_type(LDI_LTV350QV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_LTE480)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_D1)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_QV)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_PQV)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_ML)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_MP)
	LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_LTP700)
	LDI_set_LCD_module_type(LDI_LTP700WV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_QC430)
	LDI_set_LCD_module_type(LDI_QC430QV_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB)
	LDI_set_LCD_module_type(LDI_ZQ65_RGB);
#else
	RETAILMSG(1,(L"[IPL:ERR] InitializeDisplay() : Unknown Module Type [%d]\r\n", SMDK6410_LCD_MODULE));
#endif

	// Get RGB Interface Information from LDI Library
	LDI_fill_output_device_information(&RGBDevInfo);
	// Setup Output Device Information
	Disp_set_output_device_information(&RGBDevInfo);

	// Initialize Display Controller
	Disp_initialize_output_interface(DISP_VIDOUT_RGBIF);

#if		(LCD_BPP == 16)
	Disp_set_window_mode(DISP_WIN1_DMA, DISP_16BPP_565, LCD_WIDTH, LCD_HEIGHT, 0, 0);
#elif	(LCD_BPP == 32)	// XRGB format (RGB888)
	Disp_set_window_mode(DISP_WIN1_DMA, DISP_24BPP_888, LCD_WIDTH, LCD_HEIGHT, 0, 0);
#else
	RETAILMSG(1,(L"[IPL:ERR] InitializeDisplay() : Unknown Color Depth %d bpp\r\n", LCD_BPP));
#endif

	Disp_set_framebuffer(DISP_WIN1, IMAGE_FRAMEBUFFER_PA_START);
	Disp_window_onfoff(DISP_WIN1, DISP_WINDOW_ON);

#if	(SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
	// This type of LCD need MSM I/F Bypass Mode to be Disabled
	pMSMIFReg->MIFPCON &= ~(0x1<<3);	// SEL_BYPASS -> Normal Mode
#endif

	// Initialize LCD Module
	//mf LDI_initialize_LCD_module();
	pGPIOReg->GPLCON0 = (pGPIOReg->GPLCON0 & ~(0xF<<28) ) | (1<<28);
	pGPIOReg->GPLDAT |= (1<<7);    //LCD POWER
	RETAILMSG(1,(L"Before delay"));
	delay();
	RETAILMSG(1,(L"after delay"));
	pGPIOReg->GPICON = (pGPIOReg->GPICON & ~((3<<16)|(3<<18)|(3<<0)|(3<<2))) | ((1<<16)|(1<<18)|(1<<0)|(1<<2));
	pGPIOReg->GPIDAT |= (1<<8)|(1<<1)|(1<<9);
	pGPIOReg->GPIDAT &= ~(1<<0);

	pGPIOReg->GPFDAT |= (1<<15);
	pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<30)) | (1<<30); // LCD EN

	
	//-----------------FOR LCD TEST
	
	pGPIOReg->GPKCON1 = (pGPIOReg->GPKCON1 & ~(0xf<<8))|(0x1<<8);
	pGPIOReg->GPKPUD &=~(0x3<< 20);
	pGPIOReg->GPKDAT &= ~(1<<10);	//FN

	// Video Output Enable
	Disp_envid_onoff(DISP_ENVID_ON);

	// Fill Framebuffer
#if	(SMDK6410_LCD_MODULE == LCD_MODULE_LTV350/* ||SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB*/)
	memcpy((void *)IMAGE_FRAMEBUFFER_UA_START, (void *)/*InitialImage_rgb16_320x240*/ScreenBitmap, 320*240*2);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_EMULQV)
	memcpy((void *)IMAGE_FRAMEBUFFER_UA_START, (void *)InitialImage_rgb16_320x240, 320*240*2);
#elif	(LCD_BPP == 16)
	{
		/*int i;
		unsigned short *pFB,*plogo;
		pFB = (unsigned short *)IMAGE_FRAMEBUFFER_UA_START;
        	plogo = (unsigned short *)ScreenBitmap;

		for (i=0; i<LCD_WIDTH*LCD_HEIGHT/3; i++)
			*pFB++ = 0x001f;//*plogo++;//0x001F;		// Blue
		for (i=0; i<LCD_WIDTH*LCD_HEIGHT/3; i++)
			*pFB++ = 0x07e0;//*plogo++;//0x001F;		// Blue
		for (i=0; i<LCD_WIDTH*LCD_HEIGHT/3; i++)
			*pFB++ = 0xf800;//*plogo++;//0x001F;		// Blue
*/
ShadowLogo();
	}
#elif	(LCD_BPP == 32)
	{
		int i;
		unsigned int *pFB;
		pFB = (unsigned int *)IMAGE_FRAMEBUFFER_UA_START;

		for (i=0; i<LCD_WIDTH*LCD_HEIGHT; i++)
			*pFB++ = 0x000000FF;		// Blue
	}
#endif
	RETAILMSG(1,(L"[IPL] --wanggong waiting(1)\r\n"));

	// TODO:
	// Backlight Power On
	
	RETAILMSG(1,(L"[I11PL] --wanggong waiting(2)\r\n"));

	//Set PWM GPIO to control Back-light  Regulator  Shotdown Pin (GPF[15])
	//pGPIOReg->GPFDAT |= (1<<15);
	//pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<30)) | (1<<30);	// set GPF[15] as Output
      delay();
     pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<28)) | (1<<28);
       pGPIOReg->GPFDAT |= (1<<14);
	   RETAILMSG(1,(L"[IPL] --wanggong waiting(3)\r\n"));

	RETAILMSG(1,(L"[IPL] --InitializeDisplay()\r\n"));
}

static BOOLEAN SetupCopySection(ROMHDR *const pTOC)
{
    UINT32 ulLoop;
    COPYentry *pCopyEntry;

    if (pTOC == (ROMHDR *const) -1)
    {
        return(FALSE);
    }

    // This is where the data sections become valid... don't read globals until after this
    //
    for (ulLoop = 0; ulLoop < pTOC->ulCopyEntries; ulLoop++)
    {
        pCopyEntry = (COPYentry *)(pTOC->ulCopyOffset + ulLoop*sizeof(COPYentry));
        if (pCopyEntry->ulCopyLen)
        {
            memcpy((LPVOID)pCopyEntry->ulDest, (LPVOID)pCopyEntry->ulSource, pCopyEntry->ulCopyLen);
        }
        if (pCopyEntry->ulCopyLen != pCopyEntry->ulDestLen)
        {
            memset((LPVOID)(pCopyEntry->ulDest+pCopyEntry->ulCopyLen), 0, pCopyEntry->ulDestLen-pCopyEntry->ulCopyLen);
        }
    }

    return(TRUE);

}

#define FROM_BCD(n)    ((((n) >> 4) * 10) + ((n) & 0xf))

static BOOL OEMGetRealTime(LPSYSTEMTIME lpst)
{
    volatile S3C6410_RTC_REG *s6410RTC = (S3C6410_RTC_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_RTC, FALSE);

    do
    {
        lpst->wYear         = FROM_BCD(s6410RTC->BCDYEAR) + 2000 ;
        lpst->wMonth        = FROM_BCD(s6410RTC->BCDMON   & 0x1f);
        lpst->wDay          = FROM_BCD(s6410RTC->BCDDATE  & 0x3f);

        lpst->wDayOfWeek    = (s6410RTC->BCDDAY - 1);

        lpst->wHour         = FROM_BCD(s6410RTC->BCDHOUR  & 0x3f);
        lpst->wMinute       = FROM_BCD(s6410RTC->BCDMIN   & 0x7f);
        lpst->wSecond       = FROM_BCD(s6410RTC->BCDSEC   & 0x7f);
        lpst->wMilliseconds = 0;
    }
    while (!(lpst->wSecond));

    return(TRUE);
}

DWORD OEMEthGetSecs(void)
{
    SYSTEMTIME sTime;

    OEMGetRealTime(&sTime);
    return((60UL * (60UL * (24UL * (31UL * sTime.wMonth + sTime.wDay) + sTime.wHour) + sTime.wMinute)) + sTime.wSecond);
}

void main(void)
{
	UINT32 nVol = 0;
    BOOL   bLoadIPL = TRUE;

    // Set up the copy section data.
    //
    if (!SetupCopySection(pTOC))
    {
        while(1);
    }

	OEMInitDebugSerial();
    OEMWriteDebugString(L"++ NBL2 : main ++\r\n");


    if ( !FMD_Init(NULL, NULL, NULL) )
    {
	    OEMWriteDebugString(L"++ NBL2 : main : FMD_Init failed++\r\n");
    }
    else
		OEMWriteDebugString(L"++ NBL2 : main : FMD_Init ok++\r\n");

	stDeviceInfo = GetNandInfo();
	InitializeDisplay();

#if 1  // use button
	{
		volatile S3C6410_GPIO_REG *pGPIOReg = (S3C6410_GPIO_REG *)OALPAtoVA(S3C6410_BASE_REG_PA_GPIO, FALSE);
		UINT32 backup_GPNCON;
		UINT32 nGPNDAT;
		UINT32 backup_GPNPUD;

		backup_GPNCON = pGPIOReg->GPNCON;
		backup_GPNPUD = pGPIOReg->GPNPUD;
		pGPIOReg->GPNCON = (backup_GPNCON & ~(0x3<<0)) | (0x0<<0);  // EINT10(GPN20) is input
		pGPIOReg->GPNPUD = (backup_GPNPUD & ~(0x3<<0)) | (0x0<<0);  // EINT10(GPN20) is input
		nGPNDAT = pGPIOReg->GPNDAT;
		pGPIOReg->GPNCON = backup_GPNCON;
		pGPIOReg->GPNPUD = backup_GPNPUD;

		OEMWriteDebugString(L"++ NBL2 : main : Check boot mode++\r\n");

		if ((nGPNDAT & (0x1<<0)) == 0) bLoadIPL = FALSE;
	}
#else // use rtc timer.  if 0, nbl2 load ipl directly.
	{
		UINT32 nDelay;
	    UINT8  KeySelect;
	    UINT32 dwStartTime, dwCurrTime;

		RETAILMSG(1, (L"Press [SPACE] to enter EBOOT within 2 sec.\r\n"));

		nDelay = 3;
	    dwStartTime = OEMEthGetSecs();
	    dwCurrTime  = dwStartTime;
	    KeySelect   = 0;

	    while((dwCurrTime - dwStartTime) < nDelay)
	    {
	        KeySelect = OEMReadDebugByte();
	        if ((KeySelect == 0x20) || (KeySelect == 0x0d))
	        {
	        	bLoadIPL = FALSE;
	            break;
	        }

	        dwCurrTime = OEMEthGetSecs();
	    }
    }
#endif

	if (bLoadIPL) ShadowIPL();
	else ShadowEboot();
}


