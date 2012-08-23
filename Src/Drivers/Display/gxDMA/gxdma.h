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

#ifndef _GXDMA_H_
#define _GXDMA_H_

// required gxdma functions
void DmaDRAMtoVRAM(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight);
void DmaVRAMtoDRAM(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight);
BOOL IsDMAReady(BOOL fForceWait);

#define SHARED_MEM_MIN_SIZE 2*1024*1024    //2 MegaBytes
#define GAPI_DRAM_VIRTUAL_SIZE  0x00040000

#define QVGA_SUPPORT 1
#define QVGS_NONSUPPORT 0

#define SMARTFON_SCREEN QVGA_SUPPORT

const DWORD DISP_CX_SCREEN  = 320;
const DWORD DISP_CY_SCREEN  = 240;
const DWORD DISP_CX_STRIDE  = sizeof(WORD);
const DWORD DISP_CY_STRIDE  = 320*2; //240*2=480;


const DWORD DISP_CX_SCREEN_TPC  = 176;
const DWORD DISP_CY_SCREEN_TPC  = 220;
const DWORD DISP_CX_STRIDE_TPC  = sizeof(WORD);
const DWORD DISP_CY_STRIDE_TPC  = 352;


#define DRIVER_DPI_KEY TEXT("Drivers\\Display\\GPE")
#define DRIVER_DPI_VALUE TEXT("LogicalPixelsX")

#define HIGH_DPI_VALUE      192
#define HIGH_DPI_VALUE_TPC  131
#define NORMAL_DPI_VALUE    96

#ifndef GETRAWFRAMEBUFFER
    #define GETRAWFRAMEBUFFER   0x00020001
    typedef struct _RawFrameBufferInfo
    {
	    WORD wFormat;
	    WORD wBPP;
	    VOID *pFramePointer;
	    int	cxStride;
	    int	cyStride;
        int cxPixels;
        int cyPixels;
    } RawFrameBufferInfo;

    #define FORMAT_565 1
    #define FORMAT_555 2
    #define FORMAT_OTHER 3
#endif

#endif

