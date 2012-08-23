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

#include <updateappui.h>
#include <bsp.h>
#include <DrvLib.h>

#include "s3c6410_ldi.h"
#include "s3c6410_display_con.h"



//  Global:  dpCurSettings
//
//  Set debug zones names and initial setting for driver
//
#ifdef DEBUG

DBGPARAM dpCurSettings = {
    L"ULDRUI", {
        L"Undefined",   L"Undefined",   L"Undefined",   L"Undefined",
        L"Undefined",   L"Undefined",   L"Undefined",   L"Undefined",
        L"Undefined",   L"Undefined",   L"Undefined",   L"Undefined",
        L"Undefined",   L"Undefined",   L"Undefined",   L"Undefined"
    },
    0x0000
};

#endif

static volatile S3C6410_GPIO_REG *pGPIOReg;
static volatile S3C6410_DISPLAY_REG *pDispReg;
static volatile S3C6410_SPI_REG *pSPIReg;
static volatile S3C6410_MSMIF_REG *pMSMIFReg;

//------------------------------------------------------------------------------

BOOL InitDisplay(
    DWORD width, DWORD height, DWORD bpp, DWORD orientation)
{
    tDevInfo RGBDevInfo;
    bool rc = true;

    pGPIOReg = (S3C6410_GPIO_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_GPIO, sizeof(S3C6410_GPIO_REG), FALSE);
    pDispReg = (S3C6410_DISPLAY_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_DISPLAY, sizeof(S3C6410_DISPLAY_REG), FALSE);
    pSPIReg = (S3C6410_SPI_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_SPI0, sizeof(S3C6410_SPI_REG), FALSE);
    pMSMIFReg = (S3C6410_MSMIF_REG *)DrvLib_MapIoSpace(S3C6410_BASE_REG_PA_MSMIF_SFR, sizeof(S3C6410_MSMIF_REG), FALSE);

    RETAILMSG(1,(TEXT("++InitializeDisplay()\r\n")));

    // Initialize Virtual Address
    LDI_initialize_register_address((void *)pSPIReg, (void *)pDispReg, (void *)pGPIOReg);
    Disp_initialize_register_address((void *)pDispReg, (void *)pMSMIFReg, (void *)pGPIOReg);

    // Initialize LCD Module

#if        (SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
    LDI_set_LCD_module_type(LDI_LTS222QV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_LTV350)
    LDI_set_LCD_module_type(LDI_LTV350QV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_LTE480)
    LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_D1)
    LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_QV)
    LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_PQV)
    LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_ML)
    LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_EMUL48_MP)
    LDI_set_LCD_module_type(LDI_LTE480WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_LTP700)
    LDI_set_LCD_module_type(LDI_LTP700WV_RGB);
#elif    (SMDK6410_LCD_MODULE == LCD_MODULE_LTM030DK)
    LDI_set_LCD_module_type(LDI_LTM030DK_RGB);
#elif	(SMDK6410_LCD_MODULE == LCD_MODULE_ZQ65RGB)
	LDI_set_LCD_module_type(LDI_ZQ65_RGB);
#endif

    // Get RGB Interface Information from LDI Library
    LDI_fill_output_device_information(&RGBDevInfo);

    // Setup Output Device Information
    Disp_set_output_device_information(&RGBDevInfo);

    // Initialize Display Controller
    Disp_initialize_output_interface(DISP_VIDOUT_RGBIF);

#if        (LCD_BPP == 16)
    Disp_set_window_mode(DISP_WIN1_DMA, DISP_16BPP_565, LCD_WIDTH, LCD_HEIGHT, 0, 0);
#elif    (LCD_BPP == 32)    // XRGB format (RGB888)
    Disp_set_window_mode(DISP_WIN1_DMA, DISP_24BPP_888, LCD_WIDTH, LCD_HEIGHT, 0, 0);
#endif

    Disp_set_framebuffer(DISP_WIN1, IMAGE_FRAMEBUFFER_PA_START);
    Disp_window_onfoff(DISP_WIN1, DISP_WINDOW_ON);

#if    (SMDK6410_LCD_MODULE == LCD_MODULE_LTS222)
    // This type of LCD need MSM I/F Bypass Mode to be Disabled
    pMSMIFReg->MIFPCON &= ~(0x1<<3);    // SEL_BYPASS -> Normal Mode
#endif

#if (SMDK6410_LCD_MODULE == LCD_MODULE_LTM030DK)
    // This type of LCD need to initialize Backlight using IIC


    LDI_LTM030DK_port_initialize();
//    I2c_eboot_Init();
//    LCD_SetALC();
    LDI_LTM030DK_RGB_initialize();

#else

    // Initialize LCD Module
    LDI_initialize_LCD_module();
#endif
    // Video Output Enable
    Disp_envid_onoff(DISP_ENVID_ON);

    // TODO: Backlight Power On
    //Set PWM GPIO to control Back-light  Regulator  Shotdown Pin (GPF[15])
    pGPIOReg->GPFDAT |= (1<<15);
    pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<30)) | (1<<30);    // Backlight Pwm control
	pGPIOReg->GPFDAT |= (1<<14);
	pGPIOReg->GPFCON = (pGPIOReg->GPFCON & ~(3<<28)) | (1<<28);    // Backlight Pwm control

    RETAILMSG(1,(TEXT("1 InitDisplay is called\r\n")));

    memset((void*)IMAGE_FRAMEBUFFER_UA_START, 0x001f, LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8);

    return TRUE;
}
//------------------------------------------------------------------------------

BOOL DeinitDisplay()
{

    RETAILMSG(1,(TEXT("DeInitDisplay is called\r\n")));
    memset((void*)IMAGE_FRAMEBUFFER_UA_START, 0x0, LCD_WIDTH*LCD_HEIGHT*LCD_BPP/8);

    if (pGPIOReg!=NULL)
            DrvLib_UnmapIoSpace((PVOID)pGPIOReg);
    if (pDispReg!=NULL)
            DrvLib_UnmapIoSpace((PVOID)pDispReg);
    if (pSPIReg!=NULL)
            DrvLib_UnmapIoSpace((PVOID)pSPIReg);
    if (pMSMIFReg!=NULL)
            DrvLib_UnmapIoSpace((PVOID)pMSMIFReg);

    return TRUE;
}

//------------------------------------------------------------------------------

BOOL BltRect(RECT rc, DWORD stride, VOID *pBuffer)
{


    volatile USHORT *pFrame = (USHORT *)IMAGE_FRAMEBUFFER_UA_START;
    // TBD
    DWORD screenWidth = LCD_WIDTH;
    USHORT *pData = (USHORT*)pBuffer;
    LONG xs, ys, xt, yt;

    RETAILMSG(1,(TEXT("BlrRect is called\r\n")));
    // Convert stride from bytes to words
    stride /= sizeof(USHORT);
    for (yt = rc.top, ys = 0; yt < rc.bottom; yt++, ys++) {
        for (xt = rc.left, xs = 0; xt < rc.right; xt++, xs++) {
            pFrame[yt * screenWidth + xt] = pData[ys * stride + xs];
        }
    }

    return TRUE;
}

//------------------------------------------------------------------------------

