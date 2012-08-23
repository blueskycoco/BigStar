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
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//

#ifndef _DBGSETTINGS_H
#define _DBGSETTINGS_H

#ifndef DEBUG
#define DEBUG                // always turn on debug output
#endif // DEBUG



#ifdef DEBUG


#undef DEBUGMSG
#define DEBUGMSG(cond,printf_exp)   ((cond)?(NKDbgPrintfW printf_exp),1:0)	//((void)0)


#define DEBUGMASK(bit)       (1 << (bit))

#define MASK_ERROR           DEBUGMASK(0)
#define MASK_WARN            DEBUGMASK(1)
#define MASK_INIT            DEBUGMASK(2)
#define MASK_FUNCTION        DEBUGMASK(3)
#define MASK_IOCTL           DEBUGMASK(4)
#define MASK_DEVICE          DEBUGMASK(5)

#define PIN_REG_PATH         TEXT("Software\\Microsoft\\DirectX\\DirectShow\\Capture")

#ifdef CAMINTERFACE
DBGPARAM dpCurSettings = {
    _T("CAMMDD"), 
    {
        _T("Errors"), _T("Warnings"), _T("Init"), _T("Function"), 
        _T("Ioctl"), _T("Device"), _T("Activity"), _T(""),
        _T(""),_T(""),_T(""),_T(""),
        _T(""),_T(""),_T(""),_T("") 
    },
    MASK_ERROR | MASK_WARN | MASK_INIT 
}; 
#else
extern DBGPARAM dpCurSettings;
#endif

#define ZONE_ERROR           1	//DEBUGZONE(0)
#define ZONE_WARN            1	//DEBUGZONE(1)
#define ZONE_INIT            1	//DEBUGZONE(2)
#define ZONE_FUNCTION        1	//DEBUGZONE(3)
#define ZONE_IOCTL           1	//DEBUGZONE(4)
#define ZONE_DEVICE          1	//DEBUGZONE(5)
#define dbg 0
#endif
#endif
