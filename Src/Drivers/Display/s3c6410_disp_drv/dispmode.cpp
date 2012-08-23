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
// Copyright (c) Samsung Electronics. Co. LTD. All rights reserved.

/*++

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:

    dispmode.cpp

Abstract:

    This module implement the main class that derived from DDGPE of display driver to support DirectDraw
    In this part, there are codes related with Display Mode

Functions:

    DrvGetMask, SetMode, GetModeInfoEx, ...

Notes:

--*/

#include "precomp.h"

#if (LCD_BPP == 16)
ULONG gBitMasks[] = {0xF800, 0x07E0, 0x001F, 0x0000};
#elif (LCD_BPP == 32)
ULONG gBitMasks[] = {0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000};
#else
#error LCD_BPP_UNDEFINED
#endif

ULONG *
APIENTRY
DrvGetMasks(
    DHPDEV dhpdev
    )
{
    return gBitMasks;
}


SCODE
S3C6410Disp::SetMode (INT modeId, HPALETTE *palette)
{
    SCODE scRet = S_OK;

    RETAILMSG(DISP_ZONE_ENTER, (_T("[DISPDRV] ++%s(%d)\n\r"), _T(__FUNCTION__), modeId));

    if (modeId == 0)
    {
        m_dwPhysicalModeID = m_pMode->modeId;

        // Create Palette
        if (palette)
        {
            *palette = EngCreatePalette(PAL_BITFIELDS, 0, NULL, gBitMasks[0], gBitMasks[1], gBitMasks[2]);
        }
    }
    else
    {
        RETAILMSG(DISP_ZONE_ERROR, (_T("[DISPDRV:ERR] %s() : modeId = %d, Driver Support Only Mode 0\n\r"), _T(__FUNCTION__), modeId));
        scRet = E_INVALIDARG;
    }

    RETAILMSG(DISP_ZONE_ENTER,(_T("[DISPDRV] --%s()\n\r"), _T(__FUNCTION__)));

    return scRet;
}

SCODE
S3C6410Disp::GetModeInfo(GPEMode *mode, int modeNo)
{
    if (modeNo != 0)
    {
        RETAILMSG(DISP_ZONE_ERROR, (_T("[DISPDRV:ERR] GetModeInfo() : modeNo = %d, Driver Support Only Mode 0\n\r"), modeNo));
        return E_INVALIDARG;
    }

    *mode = *m_pMode;

    return S_OK;
}

SCODE
S3C6410Disp::GetModeInfoEx(GPEModeEx *pModeEx, int modeNo)
{
    if (modeNo != 0)
    {
        RETAILMSG(DISP_ZONE_ERROR, (_T("[DISPDRV:ERR] GetModeInfoEx() : modeNo = %d, Driver Support Only Mode 0\n\r"), modeNo));
        return    E_INVALIDARG;
    }

    *pModeEx = *m_pModeEx;

    return S_OK;
}

int
S3C6410Disp::NumModes()
{
    return MAX_SUPPORT_MODE;
}

void
S3C6410Disp::InitializeDisplayMode()
{
    //Setup ModeInfoEx, ModeInfo
    m_pModeEx = &m_ModeInfoEx;
    m_pMode = &m_ModeInfoEx.modeInfo;
    memset(m_pModeEx, 0, sizeof(GPEModeEx));

#if        (LCD_BPP == 16)
    m_pModeEx->ePixelFormat = ddgpePixelFormat_565;
#elif    (LCD_BPP == 32)
    m_pModeEx->ePixelFormat = ddgpePixelFormat_8888;
#endif

    // Fill GPEMode modeInfo
    m_pMode->modeId = 0;
    m_pMode->width = m_nScreenWidth;
    m_pMode->height = m_nScreenHeight;
    m_pMode->format = EDDGPEPixelFormatToEGPEFormat[m_pModeEx->ePixelFormat];
    m_pMode->Bpp = EGPEFormatToBpp[m_pMode->format];
    m_pMode->frequency = 60;        // Usually LCD Panel require 60Hz

    // Fill DDGPEStandardHeader
    m_pModeEx->dwSize = sizeof(GPEModeEx);
    m_pModeEx->dwVersion = GPEMODEEX_CURRENTVERSION;

    // Fill ModeInfoEX
    m_pModeEx->dwPixelFourCC = 0;                                // Should be Zero
    m_pModeEx->dwPixelFormatData = 0;                            // Don't care
    m_pModeEx->lPitch = m_dwDeviceScreenWidth*(m_pMode->Bpp/8);
    m_pModeEx->dwFlags = 0;                                        // Should be Zero
    m_pModeEx->dwRBitMask = gBitMasks[0];
    m_pModeEx->dwGBitMask = gBitMasks[1];
    m_pModeEx->dwBBitMask = gBitMasks[2];
    m_pModeEx->dwAlphaBitMask = gBitMasks[3];
}
