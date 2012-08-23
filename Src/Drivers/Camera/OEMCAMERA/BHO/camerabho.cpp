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

#include "stdafx.h"
#include "bho_i.c"
#include "bhoobj.h"
#include "Aygshell.h"
SHCAMERACAPTURE shcc;
int picture_number=0;
void UpdatePictureNumber();


// insert the camera icon
// the cameraicon.dll will replace the frame window and file explore window's proc
// for most of the messages, the cameraicon.dll will call the original window proc
HRESULT InsertCameraIconToBrowser(IUnknown *pUnkSite);

// test if the camera icon is selected
HRESULT PressCameraIcon(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam, BOOL *pfPressed);

// return the picture/video file to caller and quick the picture app.
// this is for picture picker only.
HRESULT ReturnFile2Caller(LPCTSTR pFilePath);

// check the pictures app mode
HRESULT GetPicturesMode(BOOL *pfPicturesAPP);

// Subclass dialog procs
BOOL CALLBACK BhoDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Original dialog procs
WNDPROC g_lpfnOriginalProc;

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY(CLSID_IBHOObj, CIBHOObj)
END_OBJECT_MAP()

HINSTANCE g_hInstRes = NULL;
bool IsInCamera =FALSE;

extern "C" BOOL WINAPI DllMain
(
    HANDLE  hInstance,
    DWORD   dwReason,
    void *  /*lpReserved*/
)
{
	RETAILMSG(1, (TEXT("DllMain hInstance=0x%x,dwReason=0x%x\r\n") ,hInstance,dwReason) );
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, (HINSTANCE)hInstance);
        g_hInstRes = (HINSTANCE)hInstance;
        DisableThreadLibraryCalls((HINSTANCE)hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        g_hInstRes = NULL;
        _Module.Term();
    }
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DllCanUnloadNow
/////////////////////////////////////////////////////////////////////////////
STDAPI DllCanUnloadNow
(
)
{
	RETAILMSG(1, (TEXT("DllCanUnloadNow\r\n") ) );
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// DllGetClassObject
/////////////////////////////////////////////////////////////////////////////
STDAPI DllGetClassObject
(
    REFCLSID    rclsid,
    REFIID      riid,
    void **     ppv
)
{
	RETAILMSG(1, (TEXT("DllGetClassObject \r\n") ) );
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer
/////////////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer
(
)
{
	RETAILMSG(1, (TEXT("DllRegisterServer\r\n")) );
    return _Module.RegisterServer(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer
/////////////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer
(
)
{
	RETAILMSG(1, (TEXT("DllUnregisterServer\r\n") ) );
    _Module.UnregisterServer();
    return S_OK;
}


STDMETHODIMP CIBHOObj::SetSite
(
    IUnknown * pUnkSite
)
{
    HWND    hwndDlg;
    HWND    hwndFrame;
    HRESULT hr = NOERROR;

	RETAILMSG(1, (TEXT("SetSite m_pWindow=0x%x,pUnkSite=0x%x\r\n") ,m_pWindow,pUnkSite) );

    if (m_pWindow != NULL)
    {
        m_pWindow->Release();
        m_pWindow = NULL;
    }

    if (pUnkSite != NULL)
    {
        m_pWindow = (IOleWindow *)pUnkSite;
        m_pWindow->AddRef();
    }

    if (m_pWindow != NULL)
    {
        hr = m_pWindow->GetWindow(&hwndDlg);
        if (SUCCEEDED(hr))
        {
            // Only the frame window (the parent of parent of the ListView)
            // handles WM_SETFOCUSITEMINFO message,
            // so we must hook the window procedure of the frame window.
            hwndFrame = GetParent(GetParent(hwndDlg));
            if (hwndFrame)
            {
                g_lpfnOriginalProc = (WNDPROC)SetWindowLong(hwndFrame, GWL_WNDPROC, (DWORD)BhoDlgProc);
			//SHSetAppKeyWndAssoc(ID_Capture_Key,hwndFrame);
	// Set the SHCAMERACAPTURE structure.
	    ZeroMemory(&shcc, sizeof(shcc));
	    shcc.cbSize             = sizeof(shcc);
	    //shcc.hwndOwner          = hwndDlg;
	    //shcc.pszInitialDir      = TEXT("\\My Documents\\我的图片\\");
	    //shcc.pszDefaultFileName = TEXT("test.jpg");
	    shcc.pszTitle           = TEXT("Camera Demo");
	    shcc.VideoTypes         = CAMERACAPTURE_VIDEOTYPE_MESSAGING;
	    //shcc.nResolutionWidth   = 1280;
	    //shcc.nResolutionHeight  = 1024;
	    shcc.nVideoTimeLimit    = 0;
	    shcc.Mode               = CAMERACAPTURE_MODE_STILL;

	StringCchPrintf(shcc.szFile,MAX_PATH,TEXT("%s%s"),shcc.pszInitialDir,shcc.pszDefaultFileName);
            }
        }
    }
    
    return InsertCameraIconToBrowser(pUnkSite);
}

/////////////////////////////////////////////////////////////////////////////
// Function Name: OEMCameraAPP
// 
// Purpose: this is function demonstrate how the OEM camera interact with pictures app
//
// Arguments:
//    IN     fMode: 1: the pictures app is in Pictures app mode.
//              0: the pictures app is in picture picker mode.
//    IN/OUT pFileName: caller pass in the buffer to contain the captured file name.
//    IN     dwBufLength: the lenght of the pFileName.
// Return Values:
//
// Side effects:  
//    caller should call DeleteInstance to release the class.
// Description:  
//    If there is no CDShow existing, this call would construct a CDShow object,
//    initializing it. Finally, it return the pointer to caller.
//    pass NULL parameter will let the wrapper use the default camera. 
// NOTE: current the pDevInfo parameter is not used. It reserved for future.
/////////////////////////////////////////////////////////////////////////////
HRESULT OEMCameraAPP(HWND hwndDlg,BOOL fMode, TCHAR *pFileName, DWORD dwBufLength)
{
    HRESULT hr = S_OK;
    if (fMode)
    { // this is pictures app mode
	{
	    HRESULT         hResult;
	//size_t cch;
	//TCHAR pFormat[3];
	//LPTSTR pszFilename;
//	TCHAR	pTemp[100];

	//UpdatePictureNumber();
	
	//StringCchLength(shcc.szFile,MAX_PATH,&cch);
	//StringCchCopy(pFormat,4,shcc.szFile+cch-3);
	//StringCchPrintf(pTemp,MAX_PATH,TEXT("test%d.%s"),picture_number,pFormat);
	//shcc.pszDefaultFileName = pTemp;

	    // Display the Camera Capture dialog.
	    while(hResult = SHCameraCapture(&shcc),hResult == S_OK)
	    	{
	    		//UpdatePictureNumber();
	    	
			//StringCchLength(shcc.szFile,MAX_PATH,&cch);
			//StringCchCopy(pFormat,4,shcc.szFile+cch-3);
			//StringCchPrintf(pTemp,100,TEXT("test%d.%s"),picture_number,pFormat);
			//shcc.pszDefaultFileName = pTemp;
			RETAILMSG(1, (TEXT("S_OK:Mode=%d nResolutionWidth=%d VideoTypes=0x%x\r\n"),shcc.Mode,shcc.nResolutionWidth,shcc.VideoTypes) );
			//StringCchCopy(pszFilename, MAX_PATH, shcc.szFile);
	    	}

		RETAILMSG(1, (TEXT("S_OK:Mode=%d nResolutionWidth=%d\r\n"),shcc.Mode,shcc.nResolutionWidth) );

	    // The next statements will execute only after the user takes
	    // a picture or video, or closes the Camera Capture dialog.
	    if (S_OK != hResult)
	    {
		RETAILMSG(1, (TEXT("SHCameraCapture false!!!!!\r\n") ) );
	    }

	    return hResult;
	}

    }
    else
    { // this is picture picker mode
        if (NULL == pFileName)
        {
            hr = E_OUTOFMEMORY;
            return hr;
        }
        if (dwBufLength > MAX_PATH)
        { // the caller can't accept the file name longer that MAX_PATH
            hr = E_FAIL;
            return hr;
        }
        MessageBox (NULL, TEXT("We are in the OEM camera app, it is picture picker mode"),
                    TEXT ("OEMCAMERA"),MB_OK);
        hr = StringCchCopy (pFileName, dwBufLength, TEXT("My Documents\\My Pictures\\flower.jpg"));
    }
    return hr;
}
/////////////////////////////////////////////////////////////////////////////
// BhoDlgProc
//
// Subclassed dialog proc
//
// Process BHO messages and unsubclass the window when it is about to be
// destroyed.
//
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK BhoDlgProc
(
    HWND hwndDlg, 
    UINT message, 
    WPARAM wParam, 
    LPARAM lParam
)
{
    BOOL fPressCameraIcon = FALSE;
	//RETAILMSG(1, (TEXT("BhoDlgProc hwndDlg=0x%x, message=0x%x, wParam=0x%x, lParam=0x%x\r\n"),hwndDlg, message, wParam, lParam ) );

    if (SUCCEEDED (PressCameraIcon(hwndDlg, message, wParam, lParam, &fPressCameraIcon))
        && fPressCameraIcon)
    {
        // get the picture app mode
        HRESULT hr = S_OK;
        BOOL fPictureMode = 0;
        TCHAR szCaptureFile[MAX_PATH] = {0};
        hr = GetPicturesMode (&fPictureMode);
        if (SUCCEEDED(hr))
        {
            // launch the OEM camera app.
            hr = OEMCameraAPP (hwndDlg,fPictureMode, szCaptureFile, MAX_PATH);

		{
			HKEY  hKey = 0;
			DWORD dwType  = 0;
			DWORD dwSize  = sizeof ( DWORD );
			DWORD dwValue = -1;
			if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_CURRENT_USER, L"\\Software\\Microsoft\\Pictures\\Camera\\User", 0, 0, &hKey ))
			{
			false;
			}

			if( ERROR_SUCCESS == RegQueryValueEx( hKey, L"EnterPIMG", 0, &dwType, (BYTE *)&dwValue, &dwSize ) )
			{
			if(dwValue !=1)
				if (memcmp(GetCommandLine(),TEXT("-camerakey"),sizeof(GetCommandLine())) == 0 && (SUCCEEDED(hr)))
					ShowWindow(hwndDlg, SW_MINIMIZE);
			//RETAILMSG(1, (TEXT("BirdCameraAPI=%d"),BirdCameraAPI ) );
			}
		}
	/*if (memcmp(GetCommandLine(),TEXT("-camerakey"),sizeof(GetCommandLine())) == 0)
		ExitProcess(0);*/
            if (SUCCEEDED(hr))
            {
            		//SHSetAppKeyWndAssoc(ID_Capture_Key,hwndDlg);
                if ((0 == fPictureMode)
                    && (0 != szCaptureFile[0]))
                { // In picture picker mode, we need to return the captured file to caller
                  // and quit the picture app.
                    hr = ReturnFile2Caller(szCaptureFile);
                }
            }
        }
        if (FAILED(hr))
        {
        	if(hr == E_INVALIDARG)
			if(MessageBox(hwndDlg,TEXT("参数无效!"),TEXT("警告"),MB_OK) == IDOK)
					PostMessage (hwndDlg, WM_CLOSE, 0, 0);
        	if(hr == E_OUTOFMEMORY)
			if(MessageBox(hwndDlg,TEXT("内存严重不足!"),TEXT("警告"),MB_OK) == IDOK)
					PostMessage (hwndDlg, WM_CLOSE, 0, 0);
            return FALSE;
        }
        return TRUE;
    }

    switch(message)
    {
        case WM_DESTROY:
            // unsubclass the window
            SetWindowLong(hwndDlg, GWL_WNDPROC, (DWORD)g_lpfnOriginalProc);
            break;
	case	WM_KEYDOWN:

		//ShowWindow(hWndApp,SW_HIDE);	
		switch(wParam)
			{
				case	ID_Capture_Key:
					PostMessage(hwndDlg, WM_COMMAND, IDM_FP_CAMERA, lParam);
					break;
			}
		break;

	/*case	WM_WINDOWPOSCHANGED:
		{
		LPWINDOWPOS	lpwp = (LPWINDOWPOS) lParam;
		RETAILMSG(1,(TEXT("lpwp->hwnd=0x%x,lpwp->hwndInsertAfter=0x%x,lpwp->x=0x%x,lpwp->y=0x%x,lpwp->cx=0x%x,lpwp->cy=0x%x,lpwp->flags=0x%x\r\n"), lpwp->hwnd,lpwp->hwndInsertAfter,lpwp->x,lpwp->y,lpwp->cx,lpwp->cy,lpwp->flags));
		if ((lpwp->flags & 0x200) && memcmp(GetCommandLine(),TEXT("-camerakey"),sizeof(GetCommandLine())) == 0)
			PostMessage(hwndDlg, WM_COMMAND, IDM_FP_CAMERA, lParam);
		}
		break;*/
		
	case	0xc009:
		if(wParam == 3 && (memcmp(GetCommandLine(),TEXT("-camerakey"),sizeof(GetCommandLine())) == 0))
			PostMessage(hwndDlg, WM_COMMAND, IDM_FP_CAMERA, lParam);
		break;
	/*case	WM_SETFOCUS:
		RETAILMSG(1, (TEXT("enter  VK_ACTION hwndDlg=0x%x, message=0x%x, wParam=0x%x, lParam=0x%x IsInCamera=%d\r\n"),hwndDlg, message, wParam, lParam,IsInCamera ) );
	        BOOL fPictureMode = 0;
		 GetPicturesMode (&fPictureMode);
			 
		if (memcmp(GetCommandLine(),TEXT("-camerakey"),sizeof(GetCommandLine())) == 0 )
			PostMessage(hwndDlg, WM_COMMAND, IDM_FP_CAMERA, lParam);
		break;*/
    }
    
    return CallWindowProc(g_lpfnOriginalProc, hwndDlg, message, wParam, lParam);
}


// Look for cam.cfg
// If it doesn't exist, create it, and set picture number to 1.
// If it exists, read the value stored inside, increment the number, and write it back.
void UpdatePictureNumber()
{
	DWORD dwSize;
	HANDLE hFile;
	char *buffer;

	buffer = (char *)malloc(1024);

	hFile = CreateFile(TEXT("\\temp\\cam.cfg"), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	dwSize = 0;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// File did not exist, so we are going to create it, and initialize the counter.
		picture_number = 1;
		hFile = CreateFile(TEXT("\\temp\\cam.cfg"), GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		buffer[0] = picture_number & 0x00FF;
		buffer[1] = (picture_number & 0xFF00) >> 8;

		WriteFile(hFile, buffer, 2, &dwSize, NULL);
		CloseHandle(hFile);
	} else
	{
		dwSize = 0;
		ReadFile(hFile, buffer, 2, &dwSize, NULL);

		picture_number = buffer[1];
		picture_number <<= 8;
		picture_number |= buffer[0];
		picture_number++;

		CloseHandle(hFile);

		hFile = CreateFile(TEXT("\\temp\\cam.cfg"), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		buffer[0] = picture_number & 0x00FF;
		buffer[1] = (picture_number & 0xFF00) >> 8;
		dwSize = 0;
		WriteFile(hFile, buffer, 2, &dwSize, NULL);
		CloseHandle(hFile);
	}

	free(buffer);
}
