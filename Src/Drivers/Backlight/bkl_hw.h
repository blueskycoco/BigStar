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
/*

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

*/
#ifndef _BKL_HW_H_
#define _BKL_HW_H_


//  Data structure
typedef struct _BLStruct
{
    DWORD   m_dwBatteryTimeout;
    DWORD   m_dwACTimeout;
    DWORD   m_dwStatus;
    BOOL    m_bBatteryAuto;
    BOOL    m_bACAuto;
    DWORD   m_dwBatteryBrightNess;
    DWORD   m_dwACBrightNess;
    BOOL    m_bBatteryOnOff;
    BOOL    m_bACOnOff;
    BOOL    m_bBatteryTimeout;
    BOOL    m_bACTimeout;
    BOOL    m_bOnOffBatt;
    BOOL    m_bExtOnOff;
    BOOL    m_bAutoMode;
} BLStruct, *PBLStruct;

//  Definitions
#define BL_ON       0x0001
#define BL_OFF      0x0002
#define NUM_EVENTS  2

#define AC_STATUS 1
#define DC_STATUS 0

// functions
BOOL  BacklightInitialize();
void  BL_ReadRegistry(BLStruct *pBLInfo);
void  BL_Deinit();
BOOL  BL_Init();
BOOL  IsACOn();
void  BL_On(BOOL bOn);
void  BL_Delay_On();
void  BL_PowerOn(BOOL bInit);
DWORD BL_MonitorThread(PVOID pParms);
void  BL_ChangeBrightness();
void Change_backlight_from_keypad(BOOL level);
#endif _BKL_HW_H_