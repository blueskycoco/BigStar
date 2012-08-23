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
#include <windows.h>
#include <guts.h>
#include <ehm.h>

#include "iidl.h"

HRESULT WINAPI PathIsGUID(LPCTSTR pszPath)
{
    CLSID clsid;
    return CLSIDFromString((LPOLESTR)pszPath, &clsid);

} /* PathIsGUID()
   */
BOOL ILIsFileSystemPidl(LPCITEMIDLIST pidl)
{
    if (pidl == NULL) {
        return TRUE;
    }
    
    if (pidl && pidl->mkid.cb) {
        if (ILFlags(pidl) == IIDL_FILESYSTEMINFO) {
            return TRUE;
        }
    }
    return FALSE;
    
} /* ILIsFileSystemPidl()
   */


BOOL ILIsNameSpacePidl(LPCITEMIDLIST pidl)
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/
{
    UINT cb;
    
    if (pidl && pidl->mkid.cb) {
        cb = sizeof(NAMESPACEINFO) + sizeof(pidl->mkid.cb);
        if ((pidl->mkid.cb == cb) && (ILFlags(pidl) == IIDL_NAMESPACEINFO)) {
            return TRUE;
        }
    }
    return FALSE;
    
} /* ILIsNameSpacePidl()
   */

BOOL ILIsPidl(LPCITEMIDLIST pidl)
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/
{
    if (ILIsNameSpacePidl(pidl) || ILIsFileSystemPidl(pidl)) {
        return TRUE;
    }
    return FALSE;
    
} /* ILIsPidl()
   */


LPITEMIDLIST ILCreateEmpty(UINT cbSize)
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/
{
    LPITEMIDLIST pidl=NULL;
    
    if (!cbSize || (cbSize == sizeof(pidl->mkid.cb))) {
        return NULL;
    }

    if (NULL != (pidl = (LPITEMIDLIST)LocalAlloc(LMEM_FIXED, cbSize))) {
        memset(pidl, 0, cbSize);
    }
    
    return pidl;
    
} /* ILCreateEmpty()
   */




LPITEMIDLIST ILCreateNameSpacePidl(LPCTSTR lpszKey)
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/
{
    LPITEMIDLIST pidl;
    NAMESPACEINFO nsi;
    USHORT cb = 0;
    TCHAR szKey[MAX_PATH];
    
    if (!lpszKey || (PathIsGUID(lpszKey) != NOERROR)) {
        return NULL;
    }

    StringCchCopy(szKey,ARRAYSIZE(szKey),lpszKey);
    
    cb = sizeof(NAMESPACEINFO) + sizeof(pidl->mkid.cb);
        
    if (NULL != (pidl = ILCreateEmpty(cb + sizeof(pidl->mkid.cb)))) {
        pidl->mkid.cb = cb;

        nsi.nflags = IIDL_NAMESPACEINFO;
        nsi.lpsf  = NULL;
        nsi.dwFileAttributes = 0;
        StringCchCopy(nsi.szGUID,ARRAYSIZE(nsi.szGUID),lpszKey);

        memcpy((((LPBYTE) pidl) + sizeof(pidl->mkid.cb)), &nsi,
               cb - sizeof(pidl->mkid.cb));
    }
    return pidl;
    
} /* ILCreateNameSpacePidl()
   */   

BOOL ILSetPidlShellFolder(LPITEMIDLIST pidlData, LPSHELLFOLDER lpsf)
/*---------------------------------------------------------------------------*\
 * 
\*---------------------------------------------------------------------------*/
{
    LPITEMIDLIST pidl = ILGetLastPidl(pidlData);
    
    if (ILIsPidl(pidl) && (NULL != pidl)) {
        ((LPFILESYSTEMINFO)((LPBYTE)pidl + sizeof(pidl->mkid.cb)))->lpsf = lpsf;
        return TRUE;
    }

    return FALSE;

} /* ILSetPidlShellFolder()
   */

