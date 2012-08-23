//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of your Windows Mobile
// Software Shared Source Premium Derivatives License Form or other
// applicable Microsoft license agreement for the software. If you did not
// accept the terms of such a license, you are not authorized to use this
// source code.
//
/*---------------------------------------------------------------------------*\
 *
 *
 *  module: iidl.h
 *  date:
 *
 *  purpose: 
 *
\*---------------------------------------------------------------------------*/
#ifndef __IIDL_H__
#define __IIDL_H__

/////////////////////////////////////////////////////////////////////////////

#pragma warning(disable: 4200) // nonstandard extension used : zero-sized array in struct/union

#include <Shlobj.h>

#ifdef    __cplusplus
extern "C" {
#endif

#define IIDL_ALL                      ((UINT)-1)
#define IIDL_NAMESPACEINFO            0x00001002
#define IIDL_FILESYSTEMINFO           0x00001004

typedef struct tagFILESYSTEMINFO {
    UINT nflags;
    LPSHELLFOLDER lpsf;
    UINT dwFileAttributes;
    UINT cbSize;
    FILETIME ft;
    TCHAR szFileName[];
} FILESYSTEMINFO;


typedef FILESYSTEMINFO __unaligned *LPFILESYSTEMINFO;
    
typedef struct tagNAMESPACEINFO {
    UINT nflags;
    LPSHELLFOLDER lpsf;
    UINT dwFileAttributes;
    TCHAR szGUID[40];
} NAMESPACEINFO, *LPNAMESPACEINFO;

typedef struct tagPIDLDATA {
    UINT nflags;
    UINT wObjType;
    TCHAR szItem[MAX_PATH]; 
    UINT dwFileAttributes;
    UINT cbSize;
    FILETIME ft;
    LPSHELLFOLDER lpsf;
} PIDLDATA, *LPPIDLDATA;


/////////////////////////////////////////////////////////////////////////////

LPITEMIDLIST ILCreateNameSpacePidl(LPCTSTR lpszKey);
BOOL ILSetPidlShellFolder(LPITEMIDLIST pidlData, LPSHELLFOLDER lpsf);

/////////////////////////////////////////////////////////////////////////////


__inline UINT ILFlags(LPCITEMIDLIST pidl)
{
    if (!pidl) { return 0;}
    return ((LPFILESYSTEMINFO)((LPBYTE)pidl + sizeof(pidl->mkid.cb)))->nflags;
}

__inline LPITEMIDLIST ILNext(LPCITEMIDLIST pidl)
{
    LPITEMIDLIST pidlnext;

    if (!pidl) { return NULL; }

    if (! pidl->mkid.cb) {
        return NULL;
    }

    if (NULL == (pidlnext = (LPITEMIDLIST) (((LPBYTE) pidl) + pidl->mkid.cb)))
    {
        return NULL;
    }
    
    return (pidlnext->mkid.cb == 0) ? NULL : pidlnext;
}

__inline UINT ILCount(LPCITEMIDLIST pidl)
{
    UINT cb;
    
    for (cb=0; (pidl && pidl->mkid.cb); cb++, pidl=ILNext(pidl));
    return cb;
}

__inline LPITEMIDLIST ILGetLastPidl(LPCITEMIDLIST pidl)
{
    int i, npidls = ILCount(pidl);
                
    for (i=1; i<npidls; i++, pidl=ILNext(pidl));
    return (LPITEMIDLIST)pidl;
}

/////////////////////////////////////////////////////////////////////////////

#ifdef    __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////
    
#endif /* __IIDL_H__ */
