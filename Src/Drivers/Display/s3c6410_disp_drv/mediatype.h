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

    mediatype.h

Abstract:

    The definition of media type(FOURCC) used in DirectDraw
    
Functions:


Notes:

--*/

#ifndef __MEDIATYPE_H__
#define __MEDIATYPE_H__

//-----------------------
// FOURCC codes supported
//-----------------------

#define MAKE_FOURCC(ch0, ch1, ch2, ch3)    ((DWORD)(BYTE)(ch0)|((DWORD)(BYTE)(ch1)<<8)|((DWORD)(BYTE)(ch2)<<16)|((DWORD)(BYTE)(ch3)<<24))

// Most of all FOURCC Format is already defined in ddgpe.h
// planar formats

#define FOURCC_I420        MAKE_FOURCC('I','4','2','0')

// packed formats

#define FOURCC_YUYV        (FOURCC_YUYV422)            // Windows Mobile have "~422" suffix in "ddgpe.h"
#define FOURCC_YUY2        (FOURCC_YUY2422)            // Windows Mobile have "~422" suffix in "ddgpe.h"
#define FOURCC_UYVY        (FOURCC_UYVY422)            // Windows Mobile have "~422" suffix in "ddgpe.h"
#define FOURCC_YVYU        MAKE_FOURCC('Y','V','Y','U')    // YCrYCb
#define FOURCC_VYUY        MAKE_FOURCC('V','Y','U','Y')    // CrYCbY

//-------------------------------
// EDDGPEPixelFormat Custom Format
//-------------------------------
#define ddgpePixelFormat_I420    (ddgpePixelFormat_CustomFormat + 1)
#define ddgpePixelFormat_YUYV    (ddgpePixelFormat_YUYV422)        // Windows Mobile have "~422" suffix in "ddgpe.h"
#define ddgpePixelFormat_UYVY    (ddgpePixelFormat_UYVY422)        // Windows Mobile have "~422" suffix in "ddgpe.h"
#define ddgpePixelFormat_YUY2    (ddgpePixelFormat_YUY2422)        // Windows Mobile have "~422" suffix in "ddgpe.h"
#define ddgpePixelFormat_YVYU    (ddgpePixelFormat_CustomFormat + 3)
#define ddgpePixelFormat_VYUY    (ddgpePixelFormat_CustomFormat + 4)


//-------------------------------------------
// Surface Detection Macros (defined in ddgpepriv.h)
//-------------------------------------------
#define IsRGB1555(ppixelFormat) \
    ( \
     ((ppixelFormat)->dwRGBAlphaBitMask == 0x8000) && \
     ((ppixelFormat)->dwRBitMask == 0x7c00) && \
     ((ppixelFormat)->dwGBitMask == 0x03e0) && \
     ((ppixelFormat)->dwBBitMask == 0x001f) && \
     (1))

#define IsRGB0555(ppixelFormat) \
    ( \
     ((ppixelFormat)->dwRGBAlphaBitMask == 0x0000) && \
     ((ppixelFormat)->dwRBitMask == 0x7c00) && \
     ((ppixelFormat)->dwGBitMask == 0x03e0) && \
     ((ppixelFormat)->dwBBitMask == 0x001f) && \
     (1))

#define IsRGB565(ppixelFormat) \
    ( \
     ((ppixelFormat)->dwRGBAlphaBitMask == 0x0000) && \
     ((ppixelFormat)->dwRBitMask == 0xf800) && \
     ((ppixelFormat)->dwGBitMask == 0x07e0) && \
     ((ppixelFormat)->dwBBitMask == 0x001f) && \
     (1))

#define IsRGB4444(ppixelFormat) \
    ( \
     ((ppixelFormat)->dwRGBAlphaBitMask == 0xf000) && \
     ((ppixelFormat)->dwRBitMask == 0x0f00) && \
     ((ppixelFormat)->dwGBitMask == 0x00f0) && \
     ((ppixelFormat)->dwBBitMask == 0x000f) && \
     (1))

#define IsRGB0888(ppixelFormat) \
    ( \
     ((ppixelFormat)->dwRGBAlphaBitMask == 0x00000000) && \
     ((ppixelFormat)->dwRBitMask == 0x00ff0000) && \
     ((ppixelFormat)->dwGBitMask == 0x0000ff00) && \
     ((ppixelFormat)->dwBBitMask == 0x000000ff) && \
     (1))

#define IsRGB8888(ppixelFormat) \
    ( \
     ((ppixelFormat)->dwRGBAlphaBitMask == 0xff000000) && \
     ((ppixelFormat)->dwRBitMask == 0x00ff0000) && \
     ((ppixelFormat)->dwGBitMask == 0x0000ff00) && \
     ((ppixelFormat)->dwBBitMask == 0x000000ff) && \
     (1))

#endif    // __MEDIATYPE_H__
