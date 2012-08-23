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
// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6181A7BC_1D18_4430_A34B_0C450963D31D__INCLUDED_)
#define AFX_STDAFX_H__6181A7BC_1D18_4430_A34B_0C450963D31D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef STRICT
#define STRICT
#endif

#if _WIN32_WCE == 201
#error ATL is not supported for Palm-Size PC
#endif


#define _WIN32_WINNT 0x0400
#define _ATL_FREE_THREADED
#if defined(_WIN32_WCE)    
#undef _WIN32_WINNT        
#endif

#include <windows.h>

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>
#include "resource.h"

EXTERN_C const CLSID CLSID_IBHOObj;
EXTERN_C const IID IID_IBHOTest;

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6181A7BC_1D18_4430_A34B_0C450963D31D__INCLUDED)
