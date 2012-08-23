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
#include "..\gxdma.h"
#include "gxinfo.h"

#define AVG16(a, b)   ( ((((a) ^ (b)) & 0xf7deU) >> 1) + ((a) & (b)) )

// DRAM and frame buffer constants
RawFrameBufferInfo g_rfbi;
PVOID g_pvDRAMBase;
DWORD g_DPI;

// function declarations
void InitGxDMA();

void InitGxDMA()
{
    GXDeviceInfo gxdi;
    HDC hdc = GetDC(NULL);
    HKEY hKey;

    // first get the current DPI settings
    g_DPI = NORMAL_DPI_VALUE;
    if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, DRIVER_DPI_KEY, 0, 0, &hKey ) )
    {
        DWORD dwType, dwData, cbData;
        cbData = sizeof( dwData );
        if( ERROR_SUCCESS == RegQueryValueEx( hKey, DRIVER_DPI_VALUE, NULL, &dwType, (LPBYTE)&dwData, &cbData ) &&
            dwType == REG_DWORD )
        {
            switch( dwData )
            {
            default:
                ASSERT(FALSE); // unexpected DPI settings

            case HIGH_DPI_VALUE:
            case HIGH_DPI_VALUE_TPC:
                g_DPI = dwData;
                break;
            }
        }
        RegCloseKey( hKey );
    }
    
    RETAILMSG(1,(TEXT("InitGxDMA \r\n")));
    // first get g_pvDRAMBase
    gxdi.idVersion = kidVersion100;
    ExtEscape(hdc, GETGXINFO, 0, NULL, sizeof(GXDeviceInfo), (char *) &gxdi);
    g_pvDRAMBase = gxdi.pvFrameBuffer;

    // now get the real frame buffer
    ExtEscape(hdc, GETRAWFRAMEBUFFER, 0, NULL, sizeof(RawFrameBufferInfo), (char *) &g_rfbi);

    // memset the DRAM buffer to black
    //memset(g_pvDRAMBase, 0, gxdi.cbStride * gxdi.cyHeight);
}

void DmaDRAMtoVRAM(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight)
{
    if( g_DPI == HIGH_DPI_VALUE )
    {
        // here we want to do pixel doubling into the frame buffer
        DWORD i,j;
        WORD *pwTempOrgPtr, *pwOrgPtr;
        DWORD *pdwTempDesPtr, *pdwDesPtr;
        WORD wDRAMStride;
        DWORD dwRawStride, dwPixelDbl;
        
        pwOrgPtr = (WORD*) g_pvDRAMBase;
        pdwDesPtr = (DWORD*) g_rfbi.pFramePointer;
        
        // need to do this so that our pointer math below works
        wDRAMStride = DISP_CY_STRIDE / sizeof(WORD);
        dwRawStride = g_rfbi.cyStride / sizeof(DWORD);
        
        for (j = 0; j < DISP_CY_SCREEN; j++)
        {
            pwTempOrgPtr = pwOrgPtr;
            pdwTempDesPtr = pdwDesPtr;
            
            // copy the actual pixel data - double by shoving the current pixel
            // into a DWORD (both upper and lower words) and writing it to 2 scanlines
            for (i= 0; i < DISP_CX_SCREEN; i++)
            {
                dwPixelDbl = (DWORD) (*pwTempOrgPtr << 16) | (DWORD) *pwTempOrgPtr;
                *pdwTempDesPtr = dwPixelDbl;
                *(pdwTempDesPtr+dwRawStride) = dwPixelDbl;
                
                // step to the next doubled-pixel
                pwTempOrgPtr++;
                pdwTempDesPtr++;
            }
            // increment org to the next line, des two lines
            pwOrgPtr += wDRAMStride;
            pdwDesPtr += dwRawStride*2;         
        }
    }
    else if( (g_DPI == HIGH_DPI_VALUE_TPC)&&(SMARTFON_SCREEN != QVGA_SUPPORT) )
    {
        // here we want to shift the data
        DWORD i,j;
        BYTE  *pwOrgPtr, *pwDesPtr;
        DWORD ShiftY, ShiftX;
        
        pwOrgPtr  = (BYTE *) g_pvDRAMBase;
        pwDesPtr  = (BYTE *) g_rfbi.pFramePointer;
        
        RETAILMSG(0, (TEXT("DRAM to VRAM pwOrgPtr %x pwDesPtr %x  \r\n"),pwOrgPtr,pwDesPtr ));
        
        // need to do this so that our pointer math below works
        ShiftY = ((g_rfbi.cyPixels - DISP_CY_SCREEN_TPC) /2 );
        ShiftX = ((g_rfbi.cxPixels - DISP_CX_SCREEN_TPC) /2 ); 
        
        RETAILMSG(0, (TEXT("ShiftY %x ShiftX %x  \r\n"),ShiftY, ShiftX ));
        
        if ( g_rfbi.cyPixels < DISP_CY_SCREEN_TPC || g_rfbi.cxPixels < DISP_CX_SCREEN_TPC || g_rfbi.cxStride < DISP_CX_STRIDE_TPC)
            return;
        // Skip the space at the top of image       
        memset( pwDesPtr, 0x00, (ShiftY*g_rfbi.cyStride) + g_rfbi.cxStride*ShiftX );
        pwDesPtr += (ShiftY*g_rfbi.cyStride) + g_rfbi.cxStride*ShiftX;
        
        
        // Simply shift the image by the required number of pixels
        for (j = 0; j < DISP_CY_SCREEN_TPC; j++)
        {   
            // copy the actual pixel data 
            for (i= 0; i < DISP_CX_SCREEN_TPC; i++)
            {
                *(WORD *)pwDesPtr = *(WORD *)pwOrgPtr;
                
                // step to the next pixel
                pwDesPtr +=DISP_CX_STRIDE_TPC;
                pwOrgPtr +=g_rfbi.cxStride;
            }
            // increment org to the next line
            memset( pwDesPtr, 0x00, g_rfbi.cxStride*ShiftX*2 );
            pwDesPtr += g_rfbi.cxStride*ShiftX*2;           
        }
        // Clear the bottom of the screen 
        memset( pwDesPtr, 0x00, (ShiftY*g_rfbi.cyStride) + g_rfbi.cxStride*ShiftX );    
    }
}

void DmaVRAMtoDRAM(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight)
{
    // we can perform initialization here, because we're always called first
    // and we're only called once
    InitGxDMA();
    
    if( g_DPI == HIGH_DPI_VALUE )
    {
        // here we want to do pixel quartering into the DRAM buffer
        int i,j;
        WORD *pwTempOrgPtr,*pwTempDesPtr,*pwOrgPtr,*pwDesPtr;
        WORD wRawStride, wDRAMStride;
        WORD tl, tr, bl, br;
        
        pwOrgPtr = (WORD*) g_rfbi.pFramePointer;
        pwDesPtr = (WORD*) g_pvDRAMBase;
        
        // need to do this so that our pointer math below works
        wRawStride = g_rfbi.cyStride / sizeof(WORD);
        wDRAMStride = DISP_CY_STRIDE / sizeof(WORD);
        
        for (j = 0; j < DISP_CY_SCREEN; j++)
        {
            pwTempOrgPtr = pwOrgPtr;
            pwTempDesPtr = pwDesPtr;
            for (i= 0; i < DISP_CX_SCREEN; i++)
            {
                // pwTempOrgPtr should always be the top left pixel of a group of 4
                // get all 4 and shove their average in pwTempDesPtr
                tl = *pwTempOrgPtr;
                tr = *(pwTempOrgPtr+1);
                bl = *(pwTempOrgPtr+wRawStride);
                br = *(pwTempOrgPtr+wRawStride+1);
                *pwTempDesPtr++ = AVG16(AVG16(tl, tr), AVG16(bl, br));
                pwTempOrgPtr += 2;
            }
            pwOrgPtr += wRawStride*2;
            pwDesPtr += wDRAMStride;            
        }           
    }
    else if( (g_DPI == HIGH_DPI_VALUE_TPC)&&(SMARTFON_SCREEN != QVGA_SUPPORT) )
    {
        // Simply shift the image by the required number of pixels
        // here we want to shift the data
        DWORD i,j;
        BYTE  *pwOrgPtr, *pwDesPtr;
        DWORD ShiftY, ShiftX;
        
        pwOrgPtr  = (BYTE*) g_pvDRAMBase;
        pwDesPtr  = (BYTE*) g_rfbi.pFramePointer;
        
        RETAILMSG(0, (TEXT("VRAM to DRAM pwOrgPtr %x pwDesPtr %x  \r\n"),pwOrgPtr,pwDesPtr ));
        
        // need to do this so that our pointer math below works
        ShiftY = ((g_rfbi.cyPixels - DISP_CY_SCREEN_TPC) /2 );
        ShiftX = ((g_rfbi.cxPixels - DISP_CX_SCREEN_TPC) /2 ); 
        
        RETAILMSG(0, (TEXT("ShiftY %x ShiftX %x  \r\n"),ShiftY, ShiftX ));
        
        if ( g_rfbi.cyPixels < DISP_CY_SCREEN_TPC || g_rfbi.cxPixels < DISP_CX_SCREEN_TPC || g_rfbi.cxStride < DISP_CX_STRIDE_TPC)
            return;
        // Skip the space at the top of image       
        pwDesPtr += (ShiftY*g_rfbi.cyStride) + g_rfbi.cxStride*ShiftX;
        
        // Simply shift the image by the required number of pixels
        for (j = 0; j < DISP_CY_SCREEN_TPC; j++)
        {   
            // copy the actual pixel data 
            for (i= 0; i < DISP_CX_SCREEN_TPC; i++)
            {
                *(WORD *)pwOrgPtr = *(WORD *)pwDesPtr;
                
                // step to the next pixel
                pwDesPtr +=DISP_CX_STRIDE_TPC;
                pwOrgPtr +=g_rfbi.cxStride;
            }
            // increment org to the next line
            pwDesPtr += g_rfbi.cxStride*ShiftX*2;           
        }   
    }
}

BOOL IsDMAReady(BOOL fForceWait)
{
    // our DMA functions are synchronous, so DMA is always ready
    return TRUE;
}

