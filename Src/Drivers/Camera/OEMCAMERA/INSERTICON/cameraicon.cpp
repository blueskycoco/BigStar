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
#include <guts.h>
#include <ehm.h>

#include "iidl.h"
#include "resource.h"
#define IDM_FP_CAMERA           4011
#define IDM_FP_SELECTFILE       4012
#define WM_GETFRAMEPARAM        (WM_USER + 4350)
#define FPRM_ISPICTURESAPP      4

#define BIT_REVERSE                     0x10000000

#define WM_REFRESH                      (WM_USER + 0x69)

const TCHAR c_szCameraCLSID[]       = TEXT("{030D681B-1DFC-4bd0-A72A-A9B3CCCDA653}");
const TCHAR WNDPROP_CAMERA_DEVICE[] = TEXT("CAMERA_DEVICES");
const TCHAR WNDPROP_CAMERA_COUNTS[] = TEXT("CAMERA_COUNTS");

HWND g_hwndListView      = NULL;
HWND g_hwndExplorer      = NULL;
HWND g_hwndFrame         = NULL;

extern HINSTANCE g_hInstRes;

class CCameraDevice;

IOleWindow    *g_pOleWindow    = NULL;
CCameraDevice *g_pCameraDevice = NULL;

// Original window procedure
WNDPROC g_lpfnOriginalExplorerWndProc;
WNDPROC g_lpfnOriginalFrameWndProc;

// Subclass window procedure
BOOL CALLBACK ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK FrameWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL HandleExplorerRefreshMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class CCameraDevice
{
public:
    DEVMGR_DEVICE_INFORMATION m_DevInfo;
    TCHAR m_pszCLSID[MAX_PATH];
    TCHAR m_pszDisplayName[MAX_PATH];
    void*  m_pidl;
    class CCameraDevice *m_pNextCameraDevice;

    CCameraDevice()
    {
       RETAILMSG(1, (TEXT("CCameraDevice\r\n") ) );
        m_pidl = NULL;
        m_pNextCameraDevice = NULL;
        memset(&m_DevInfo,0,sizeof(m_DevInfo));
        m_DevInfo.dwSize = sizeof(m_DevInfo);
    }

    ~CCameraDevice()
    { // we don't care m_pidl. suppose it be free after call ListView_DeleteAllItems
        m_pNextCameraDevice = NULL;
    }
};

HRESULT DeleteCameraDevices(CCameraDevice** ppCameraDevice)
{
       RETAILMSG(1, (TEXT("DeleteCameraDevices ppCameraDevice=0x%x\r\n"),ppCameraDevice ) );

    ASSERT(ppCameraDevice);
    CCameraDevice* pCameraDeviceHeader = *ppCameraDevice;
    CCameraDevice* pTempCameraDevice = NULL;

    while(pCameraDeviceHeader)
    {
        pTempCameraDevice = pCameraDeviceHeader->m_pNextCameraDevice;
        delete pCameraDeviceHeader;
        pCameraDeviceHeader = pTempCameraDevice;
    }
    *ppCameraDevice = NULL;
    return S_OK;
}

HRESULT InsertCameraIconToBrowser(IUnknown *pUnkSite)
{
    HRESULT hr = E_INVALIDARG;
       RETAILMSG(1, (TEXT("CCameraDevice pUnkSite=0x%x\r\n"),pUnkSite ) );
    SetProp(g_hwndFrame, WNDPROP_CAMERA_DEVICE,(HANDLE)0);
    SetProp(g_hwndFrame, WNDPROP_CAMERA_COUNTS,(HANDLE)0);
    DeleteCameraDevices(&g_pCameraDevice);
    g_pCameraDevice = NULL;

    if (NULL != g_pOleWindow)
    {
        g_pOleWindow->Release();
        g_pOleWindow = NULL;
    }

    g_pOleWindow = (IOleWindow *)pUnkSite;
    if (NULL != g_pOleWindow)
    {
        g_pOleWindow->AddRef();

        hr = g_pOleWindow->GetWindow(&g_hwndListView);
        if (SUCCEEDED(hr))
        {
            g_hwndExplorer = GetParent(g_hwndListView);
            g_hwndFrame    = GetParent(g_hwndExplorer);

            // Hook Explorer window
            g_lpfnOriginalExplorerWndProc = (WNDPROC)SetWindowLong(g_hwndExplorer, GWL_WNDPROC, (DWORD)ExplorerWndProc);
            // Hook Frame window
            g_lpfnOriginalFrameWndProc = (WNDPROC)SetWindowLong(g_hwndFrame, GWL_WNDPROC, (DWORD)FrameWndProc);
        }
    }

    return hr;
}

// test if the camera icon is selected
HRESULT PressCameraIcon(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam, BOOL *pfPressed)
{
    HRESULT hr = S_OK;
	//LPTSTR pCommnedLine=GetCommandLine();
	//bool	Keydown=FALSE;
	//RETAILMSG(1, (TEXT("PressCameraIcon hwndDlg=0x%x, message=0x%x, wParam=0x%x, lParam=0x%x\r\n"),hwndDlg, message, wParam, lParam ) );
    CBR (hwndDlg == g_hwndFrame);
	//if(memcmp(pCommnedLine,TEXT("-camerakey"),sizeof(pCommnedLine)) == 0)
		//Keydown=TRUE;
	//RETAILMSG(1, (TEXT("Keydown=0x%x\r\n"),Keydown) );
    if (((WM_COMMAND == message)
        && (IDM_FP_CAMERA == wParam)) )
    {
        *pfPressed = TRUE;
    }
    else
    {
        *pfPressed = FALSE;
    }
Error:
    return hr;
}

// return the picture/video file to caller and quick the picture app.
// this is for picture picker only.
HRESULT ReturnFile2Caller(LPCTSTR pFilePath)
{
       RETAILMSG(1, (TEXT("ReturnFile2Caller pFilePath=0x%x\r\n"),pFilePath ) );

    PostMessage(g_hwndFrame, WM_COMMAND, IDM_FP_SELECTFILE, (LPARAM)pFilePath);
    return S_OK;
}

// check the pictures app mode
HRESULT GetPicturesMode(BOOL *pfPicturesAPP)
{
       RETAILMSG(1, (TEXT("GetPicturesMode\r\n") ) );

    *pfPicturesAPP = SendMessage(g_hwndFrame, WM_GETFRAMEPARAM,FPRM_ISPICTURESAPP,0);
    return S_OK;
}

BOOL CALLBACK ExplorerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
      // RETAILMSG(1, (TEXT("Explorer hwnd=0x%x, uMsg=0x%x, wParam=0x%x, lParam=0x%x\r\n"),hwnd, uMsg, wParam, lParam) );

    switch (uMsg)
    {
        case WM_REFRESH:
            return HandleExplorerRefreshMessage(hwnd, uMsg, wParam, lParam);
            break;
        case WM_DESTROY:
            // unsubclass the window
            SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)g_lpfnOriginalExplorerWndProc);
            break;
    }

    return CallWindowProc(g_lpfnOriginalExplorerWndProc, hwnd, uMsg, wParam, lParam);
}

BOOL CALLBACK FrameWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
      // RETAILMSG(1, (TEXT("FrameWndProc hwnd=0x%x, uMsg=0x%x, wParam=0x%x, lParam=0x%x\r\n"),hwnd, uMsg, wParam, lParam) );

    switch (uMsg)
    {
        case WM_CLOSE:
            SetProp(g_hwndFrame, WNDPROP_CAMERA_DEVICE,(HANDLE)0);
            SetProp(g_hwndFrame, WNDPROP_CAMERA_COUNTS,(HANDLE)0);
            DeleteCameraDevices(&g_pCameraDevice);
            g_pCameraDevice = NULL;
            break;
        case WM_DESTROY:
            // unsubclass the window
            SetWindowLong(hwnd, GWL_WNDPROC, (DWORD)g_lpfnOriginalFrameWndProc);
            break;
    }

    return CallWindowProc(g_lpfnOriginalFrameWndProc, hwnd, uMsg, wParam, lParam);
}

BOOL HandleExplorerRefreshMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
       //RETAILMSG(1, (TEXT("HandleExplorerRefreshMessage hwnd=0x%x, uMsg=0x%x, wParam=0x%x, lParam=0x%x\r\n"),hwnd, uMsg, wParam, lParam) );

    BOOL bRet       = FALSE;
    HRESULT hr      = S_OK;
    UINT nSortOrder = (UINT)lParam;
    DWORD dwType = 0;
    int iIndex;
    LV_FINDINFO lvfi;
    
    // detect if MS camera app has been disabled and we are at the My Pictures directory.
    SetProp(g_hwndFrame, WNDPROP_CAMERA_COUNTS,(HANDLE)1);
    bRet = CallWindowProc(g_lpfnOriginalExplorerWndProc, hwnd, uMsg, wParam, lParam);

    if ((hwnd == g_hwndExplorer && WM_REFRESH == uMsg)
        && (0 == GetProp(g_hwndFrame, WNDPROP_CAMERA_COUNTS))
        && (0 == GetProp(g_hwndFrame, WNDPROP_CAMERA_DEVICE)))
    {
        // the MS camera app has been disabled and we are at the My Pictures directory.

        // create the camera item
        g_pCameraDevice = new CCameraDevice;
        CPR (g_pCameraDevice);
            
        // create the pidl
        LPITEMIDLIST pidl = ILCreateNameSpacePidl(c_szCameraCLSID);
        CPR (pidl);
            
        // clear the focus
        dwType = GetWindowLong(g_hwndListView, GWL_STYLE);
        switch (dwType& LVS_TYPEMASK)
        {
            case LVS_ICON:
            case LVS_SMALLICON:
                memset(&lvfi, 0, sizeof(LV_FINDINFO));
                lvfi.flags = LVFI_NEARESTXY;
                iIndex = ListView_FindItem(g_hwndListView, -1, &lvfi);
                break;
            default:
                iIndex = ListView_GetTopIndex(g_hwndListView);
                break;
        }

        ListView_SetItemState(g_hwndListView, iIndex, (UINT)((~LVIS_FOCUSED) & (~LVIS_SELECTED)), (UINT)(LVIS_FOCUSED | LVIS_SELECTED));
        PostMessage(g_hwndListView, WM_SETREDRAW, TRUE, 0);

        // insert the camera icon to the list view
        ILSetPidlShellFolder(pidl, NULL);
        
        LPCTSTR IconDisplay = (LPCTSTR)LoadString(g_hInstRes,IDS_CAMERA,NULL,0);
        CPR (IconDisplay);
        
        CHR(StringCchCopy(g_pCameraDevice->m_pszCLSID, ARRAYSIZE(g_pCameraDevice->m_pszCLSID),c_szCameraCLSID));
        CHR(StringCchCopy(g_pCameraDevice->m_pszDisplayName, ARRAYSIZE(g_pCameraDevice->m_pszDisplayName),IconDisplay));
        
        g_pCameraDevice->m_pidl = pidl;

        SetProp(g_hwndFrame, WNDPROP_CAMERA_DEVICE,(HANDLE)g_pCameraDevice);
        SetProp(g_hwndFrame, WNDPROP_CAMERA_COUNTS,(HANDLE)1);

        LV_ITEM lvi;
        lvi.state     = 0;
        lvi.stateMask = LVIS_STATEIMAGEMASK;
        lvi.iSubItem  = 0;
        lvi.mask      = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
        lvi.iImage    = 0;
        lvi.iItem     = ListView_GetItemCount(g_hwndListView);
        lvi.lParam    = (LPARAM)pidl;
        ListView_InsertItem(g_hwndListView, &lvi);

        SendMessage(hwnd, WM_COMMAND, (nSortOrder & ~BIT_REVERSE), !!(nSortOrder & BIT_REVERSE));
        dwType = GetWindowLong(g_hwndListView, GWL_STYLE);
        switch (dwType& LVS_TYPEMASK)
        {
            case LVS_ICON:
            case LVS_SMALLICON:
                memset(&lvfi, 0, sizeof(LV_FINDINFO));
                lvfi.flags = LVFI_NEARESTXY;
                iIndex = ListView_FindItem(g_hwndListView, -1, &lvfi);
                break;
            default:
                iIndex = ListView_GetTopIndex(g_hwndListView);
                break;
        }
        
        ListView_SetItemState(g_hwndListView, iIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
        PostMessage(g_hwndListView, WM_SETREDRAW, TRUE, 0);
    }
Error:
    return bRet;
}
