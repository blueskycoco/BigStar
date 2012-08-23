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
//  Header: oal_perreg.h
//
//  This header define functions and structures used by optional persistent
//  registry implementation.
//
#ifndef __OAL_PERREG_H
#define __OAL_PERREG_H

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

typedef struct {
    UINT32 base;
    UINT32 start;
    UINT32 size;
} OAL_PERREG_REGION;

//------------------------------------------------------------------------------

BOOL OALPerRegInit(BOOL clean, OAL_PERREG_REGION *pArea);
BOOL OALPerRegWrite(UINT32 flags, UINT8 *pData, UINT32 dataSize);
UINT32 OALPerRegRead(UINT32 flags, UINT8 *pData, UINT32 dataSize);

//------------------------------------------------------------------------------

#if __cplusplus
}
#endif

#endif
