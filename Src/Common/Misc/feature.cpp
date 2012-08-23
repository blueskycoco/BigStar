//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  File:  feature.cpp
//
#include <windows.h>
#include <winnt.h>
#include <oal.h>
#include <algorithm>

extern "C" UINT32 OALGetIdCode();

// The Architecture field definitions in the ID Code
enum ARM_ARCHITECTURE {
    ARCH_4 = 1,
    ARCH_4T,
    ARCH_5,
    ARCH_5T,
    ARCH_5TE,
    ARCH_5TEJ,
    ARCH_6 = 15,		// added by JJG. ARMv6 architecture is 0xf.
};

// Layout of the ID Code register
union IDCodeReg {
    UINT32 idCode;

    struct {
        INT32               revision     :  4;
        UINT32              partNumber   : 12;
        ARM_ARCHITECTURE    architecture :  4;
        UINT32              variant      :  4;
        UINT32              implementor  :  8;
    };
};

// Map a feature to a bitfield of architectures that have that feature.
struct FeatureToArchMask {
    UINT32 processorFeature;
    UINT32 archMask;
};

#define ARCH_4_ALL ((1 << ARCH_4) | (1 << ARCH_4T))
#define ARCH_5_ALL ((1 << ARCH_5) | (1 << ARCH_5T) | (1 << ARCH_5TE) | (1 << ARCH_5TEJ))
#define ARCH_6_ALL (1 << ARCH_6)


static const FeatureToArchMask sc_rgFeatureToArchMask[] = {
    { PF_ARM_V4,        (ARCH_4_ALL | ARCH_5_ALL | ARCH_6_ALL) },
    { PF_ARM_V5,        (ARCH_5_ALL | ARCH_6_ALL) },
    { PF_ARM_V6,        ( ARCH_6_ALL) },
    { PF_ARM_THUMB,     (1 << ARCH_4T) | (1 << ARCH_5T) | (1 << ARCH_5TE) | (1 << ARCH_5TEJ) |ARCH_6_ALL },
    { PF_ARM_JAZELLE,   (1 << ARCH_5TEJ) | ARCH_6_ALL },
};

#define dim(x) (sizeof(x)/sizeof((x)[0]))


//------------------------------------------------------------------------------
//
//  Function:  OALIsProcessorFeaturePresent
//
//  Called to determine the processor's supported feature set.
//
extern "C" BOOL OALIsProcessorFeaturePresent(DWORD feature)
{
    BOOL rc = FALSE;

    // Get the ID from the hardware.
    IDCodeReg idCode = { OALGetIdCode() };

    OALMSG(OAL_INFO&&OAL_VERBOSE, 
        (L"OALIsProcessorFeaturePresent: ID Code register: %c 0x%02x 0x%02x 0x%03x 0x%02x\r\n",
        idCode.implementor, idCode.variant, idCode.architecture, 
        idCode.partNumber, idCode.revision));
    
    // Is it possible that this feature can be supported?
    UINT32 idx;
    for (idx = 0; idx < dim(sc_rgFeatureToArchMask); ++idx) {
        if (sc_rgFeatureToArchMask[idx].processorFeature == feature) {
            break;
        }
    }

    if (idx < dim(sc_rgFeatureToArchMask)) {
        // Yes, so now see if this CPU supports this feature.
        UINT32 actualMask = (1 << (0xf&idCode.architecture));
        if (sc_rgFeatureToArchMask[idx].archMask & actualMask) {
            rc = TRUE;
        }
    }
 
    return rc;
}

//------------------------------------------------------------------------------
