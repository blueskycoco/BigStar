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
//#include "classfact.h"
//#include "srvlife.h"

#include "Aygshell.h"
#include "OEMCameraCapture.h"

HRESULT ConformInputValue(PSHCAMERACAPTURE pshcc);
DWORD WINAPI BirdCAMAPIThread(void);
HRESULT	ReadUserRegistryData(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,DWORD* lpData);
HRESULT	ReadUserRegistryStr(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,BYTE* lpStr);
HRESULT SetUserRegistryDate(PSHCAMERACAPTURE pshcc);
void UpdatePictureNumber();



HINSTANCE hInstance = NULL;
BOOL	DllIsLoad=FALSE;
//class CameraAPI;
CCameraAPI 	*pBirdCameraAPI;
HANDLE hCancleCapture=0;
HANDLE hCaptureDone=0;
HANDLE hOUTOFMEMORY=0;
HANDLE hINVALIDARG=0;
HANDLE events[6];
PSHCAMERACAPTURE pshcc_Last;
bool FirstLoaded=TRUE;
HINSTANCE hLoadedlib=NULL;
int picture_number=0;




extern "C" BOOL WINAPI DllMain
(
    HANDLE  g_hInstRes,
    DWORD   dwReason,
    void *  /*lpReserved*/
)
{
	RETAILMSG(1, (TEXT("DllMain hInstance=0x%x,dwReason=0x%x,hInstance=0x%x,GetCurrentThread=0x%x\r\n") ,g_hInstRes,dwReason,hInstance,GetCurrentThread()) );
    if (dwReason == DLL_PROCESS_ATTACH)
    {
    	HANDLE hThread = NULL;
		RETAILMSG(1, (TEXT("DLL_PROCESS_ATTACH hInstance=0x%x,FirstLoaded=0x%x,hInstance=0x%x\r\n") ,g_hInstRes,FirstLoaded,hInstance) );
		if(!FirstLoaded)
		{
			if(MessageBox(NULL,TEXT("其他应用正在使用相机"),TEXT("警告"),MB_OK) == IDOK)
				return FALSE;
		}
		FirstLoaded=FALSE;
        hInstance = (HINSTANCE)g_hInstRes;
		pBirdCameraAPI=new	CCameraAPI;
        DisableThreadLibraryCalls((HINSTANCE)g_hInstRes);
		hLoadedlib=LoadLibrary(L"OEMCameraCapture.dll");
		// Launch the Camera ISR
		if (hThread == NULL)
	    {
			hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)BirdCAMAPIThread,0,0,NULL);
			CloseHandle(hThread);
	    }
		hCancleCapture = CreateEvent(NULL, FALSE, FALSE, NULL);
		hCaptureDone = CreateEvent(NULL, FALSE, FALSE, NULL);
		hOUTOFMEMORY = CreateEvent(NULL, FALSE, FALSE, NULL);
		hINVALIDARG = CreateEvent(NULL, FALSE, FALSE, NULL);
		events[4]  = CreateEvent(NULL, FALSE, FALSE, NULL);//初始化时等待初始化窗口完毕
		events[5]  = CreateEvent(NULL, FALSE, FALSE, NULL);//初始化时等待初始化窗口完毕

		events[0] =hCaptureDone ;	//对应于返回S_OK
		events[1] =hCancleCapture ;	//对应于返回S_FALSE
		events[2] =hOUTOFMEMORY ;	//对应于返回E_OUTOFMEMORY
		events[3] =hINVALIDARG ;	//对应于返回E_INVALIDARG 
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
		delete	pBirdCameraAPI;
		RETAILMSG(1, (TEXT("DLL_PROCESS_DETACH hInstance=0x%x,dwReason=0x%x,hInstance=0x%x\r\n") ,g_hInstRes,dwReason,hInstance) );
        hInstance = NULL;
		DllIsLoad=FALSE;
    }
    return TRUE;
}


EXTERN_C
HRESULT 
OEMCameraCapture(PSHCAMERACAPTURE pshcc)
{
	HRESULT         hResult;
	DWORD event_id = 0;
	RETAILMSG(1, (TEXT("OEMCameraCap ture pshcc=0x%x,DllIsLoad=%d\r\n"),pshcc,DllIsLoad ) );
	RETAILMSG(1, (TEXT("OEMCameraCapture shcc.nResolutionWidth=0x%x,shcc.VideoTypes=0x%x,pshcc->Mode=0x%x\r\n"),pshcc->nResolutionWidth,pshcc->VideoTypes,pshcc->Mode ) );

	//InitSHCAMERACAPTURE(pshcc);
		//RETAILMSG(1, (TEXT("OEMCameraCapture hWndApp =0x%x,gcap.pCap=0x%x!!!!!\r\n"),hWndApp, gcap.pCap) );
	
		//hResult=0;//pBirdCameraAPI->InitCamera(hInstance);
	if(ConformInputValue(pshcc) == E_INVALIDARG)
		return	E_INVALIDARG;


	if(pBirdCameraAPI->pshcc == NULL)//表示第一次进去
	{
		pBirdCameraAPI->pshcc=(PSHCAMERACAPTURE)malloc(sizeof(SHCAMERACAPTURE));
		memcpy(pBirdCameraAPI->pshcc, pshcc, sizeof(SHCAMERACAPTURE));
	}
	else
	{
		memcpy(pBirdCameraAPI->pshcc, pshcc, sizeof(SHCAMERACAPTURE));
		//ShowWindow(pBirdCameraAPI->hWndApp,SW_SHOW);
		//pBirdCameraAPI->OpenCamera(pshcc);
		SetWindowPos(pBirdCameraAPI->hWndApp,HWND_TOPMOST,0,0,240,320,SWP_SHOWWINDOW);
		//pBirdCameraAPI->StartPreview();
	}

	event_id = WaitForMultipleObjects(4,(CONST HANDLE*)&events, FALSE,INFINITE);
	SetWindowPos(pBirdCameraAPI->hWndApp,HWND_NOTOPMOST,0,0,240,320,SWP_HIDEWINDOW |SWP_NOACTIVATE);
	if(event_id == WAIT_OBJECT_0)
		{
		UpdatePictureNumber();
		hResult = S_OK;   //补抓视频完成 
		}
	else if(event_id == WAIT_OBJECT_0+1)
		hResult = S_FALSE;    //取消预览
	else if(event_id == WAIT_OBJECT_0+2)
		hResult = E_OUTOFMEMORY;    //E_OUTOFMEMORY 
		
	memcpy(pshcc,pBirdCameraAPI->pshcc,sizeof(SHCAMERACAPTURE));//反输出shcc
	SetUserRegistryDate(pshcc);
	RETAILMSG(1, (TEXT("event_id=%d hResult=%d mode=%d\r\n"),event_id, hResult,pshcc->Mode) );
	return	hResult;
}


DWORD WINAPI BirdCAMAPIThread(void)
{
	HRESULT         hResult;
	RETAILMSG(1, (TEXT("BirdCAMAPIThread:GetCurrentThreadId=0x%x\r\n"), GetCurrentThreadId()) );
	hResult=pBirdCameraAPI->InitCamera(hInstance,events);
	RETAILMSG(1, (TEXT("BirdCAMAPIThread:hResult=0x%x\r\n"), hResult) );
	if(hResult == FALSE)
	{
		SetEvent(hCancleCapture);
		FreeLibrary(hLoadedlib);
	}
	return hResult;
}

HRESULT ConformInputValue(PSHCAMERACAPTURE pshcc)
{

	HRESULT	hr=S_OK;
	size_t cch;
	TCHAR pFormat[3];
	RETAILMSG(1, (TEXT("Enter ConformInputValue hr=0x%x\r\n"),hr ) );
	if(pshcc->pszInitialDir == NULL)
		{
		ReadUserRegistryStr(HKEY_CURRENT_USER,L"\\Software\\Microsoft\\Pictures\\Camera\\User",L"DefaultDir",(BYTE*)pshcc->szFile);
		//StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("\\My Documents\\我的图片\\"));
		}
	else
		StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("%s"),pshcc->pszInitialDir);		
	if(pshcc->pszDefaultFileName == NULL)
		{
		TCHAR FilePrefix[MAX_PATH];
		ReadUserRegistryStr(HKEY_CURRENT_USER,L"\\Software\\Microsoft\\Pictures\\Camera\\User",L"FilePrefix",(BYTE*)FilePrefix);
		if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
			StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("%s%s%d.jpg"),pshcc->szFile,FilePrefix,picture_number);
		else
			{
			if(pBirdCameraAPI->m_nFileFormat==1)
				StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("%s%s%d.mp4"),pshcc->szFile,FilePrefix,picture_number);
			else
				StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("%s%s%d.3gp"),pshcc->szFile,FilePrefix,picture_number);
			}
		}
	else
		StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("%s%s"),pshcc->szFile,pshcc->pszDefaultFileName);

	StringCchLength(pshcc->szFile,MAX_PATH,&cch);
	StringCchCopy(pFormat,4,pshcc->szFile+cch-3);//读取后缀名
	if(pshcc->Mode!=CAMERACAPTURE_MODE_STILL && pshcc->Mode!=CAMERACAPTURE_MODE_VIDEOONLY && pshcc->Mode!=CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		hr=E_INVALIDARG;
	
	RETAILMSG(1, (TEXT("ConformInputValue:Width=%d Height=%d"),pshcc->nResolutionWidth,pshcc->nResolutionHeight ) );
	if(pshcc->nResolutionWidth == 0 && pshcc->nResolutionHeight == 0)
		{
		if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
			{
			DWORD Resolution;
			TCHAR SubReg[MAX_PATH];
			ReadUserRegistryData(HKEY_CURRENT_USER, L"\\Software\\Microsoft\\Pictures\\Camera\\User", L"Resolution", &Resolution);
			StringCchPrintf(SubReg,MAX_PATH,TEXT("\\Software\\Microsoft\\Pictures\\Camera\\OEM\\PictureResolution\\%d"),Resolution);
			ReadUserRegistryData(HKEY_LOCAL_MACHINE, SubReg, L"Width", &pshcc->nResolutionWidth);
			ReadUserRegistryData(HKEY_LOCAL_MACHINE, SubReg, L"Height", &pshcc->nResolutionHeight);
			RETAILMSG(1, (TEXT("ConformInputValue:Width=%d Height=%d"),pshcc->nResolutionWidth,pshcc->nResolutionHeight ) );
			}else
			{
			DWORD VideoProfile;
			TCHAR SubReg[MAX_PATH];
			ReadUserRegistryData(HKEY_CURRENT_USER, L"\\Software\\Microsoft\\Pictures\\Camera\\User", L"VideoProfile", &VideoProfile);
			StringCchPrintf(SubReg,MAX_PATH,TEXT("\\Software\\Microsoft\\Pictures\\Camera\\OEM\\VideoProfile\\%d"),VideoProfile);
			ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg, L"Width", &pshcc->nResolutionWidth);
			ReadUserRegistryData(HKEY_LOCAL_MACHINE, SubReg, L"Height", &pshcc->nResolutionHeight);
			RETAILMSG(1, (TEXT("ConformInputValue:Width=%d Height=%d"),pshcc->nResolutionWidth,pshcc->nResolutionHeight ) );
			}
		}
	
	if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
		{
		if(_tcsicmp(pFormat, _T("jpg")) != 0) 
			hr=E_INVALIDARG;
		RETAILMSG(1, (TEXT("conform resolution\r\n") ) );
		switch(pshcc->nResolutionWidth)
			{
			case	1280:
				if(pshcc->nResolutionHeight !=1024)
					hr=E_INVALIDARG;
				break;
			case	640:
				if(pshcc->nResolutionHeight !=512 && pshcc->nResolutionHeight!=480)
					hr=E_INVALIDARG;
				break;
			case	320:
				if(pshcc->nResolutionHeight !=240)
					hr=E_INVALIDARG;
				break;
			default:
					hr=E_INVALIDARG;
			}
		}
	if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOONLY || pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
		if((_tcsicmp(pFormat, _T("3gp")) != 0) && (_tcsicmp(pFormat, _T("mp4")) != 0))
			hr=E_INVALIDARG;
		switch(pshcc->nResolutionWidth)
			{
			case	320:
				if(pshcc->nResolutionHeight !=240)
					hr=E_INVALIDARG;
				break;
			case	176:
				if(pshcc->nResolutionHeight !=144)
					hr=E_INVALIDARG;
				break;
			default:
					hr=E_INVALIDARG;
			}
		}
	RETAILMSG(1, (TEXT("OEMCameraCapture hr=0x%x\r\n"),hr ) );
	return	hr;
}


HRESULT	ReadUserRegistryData(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,DWORD* lpData)
{
	    HKEY  phkResult = 0;
	    DWORD dwType  = 0;
	    DWORD dwSize  = sizeof ( DWORD );
		if( ERROR_SUCCESS != RegOpenKeyEx( hKey, lpSubKey, 0, 0, &phkResult ))
		    {
		       return false;
		    }

		    if( ERROR_SUCCESS != RegQueryValueEx( phkResult, lpValueName, 0, &dwType, (BYTE *)lpData, &dwSize ) )
		    {
			return false;
		    }
	    RegCloseKey( hKey );
	return	TRUE;
}

HRESULT	ReadUserRegistryStr(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,BYTE* lpStr)
{
	    HKEY  phkResult = 0;
	    DWORD dwType  = 0;
	    DWORD dwSize  = MAX_PATH;
		if( ERROR_SUCCESS != RegOpenKeyEx( hKey, lpSubKey, 0, 0, &phkResult ))
		    {
		       return false;
		    }

		    if( ERROR_SUCCESS != RegQueryValueEx( phkResult, lpValueName, 0, &dwType, (BYTE *)lpStr, &dwSize ) )
		    {
			return false;
		    }
	    RegCloseKey( hKey );
	return	TRUE;
}


HRESULT SetUserRegistryDate(PSHCAMERACAPTURE pshcc)
{
	    HKEY  phkResult = 0;
	    DWORD dwType  = 0;
	    DWORD dwSize  = sizeof ( DWORD );
		DWORD	OptionNum;
	if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
		{
		    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"\\Software\\Microsoft\\Pictures\\Camera\\OEM\\PictureResolution", 0, 0, &phkResult ))
		       return false;
		    if( ERROR_SUCCESS != RegQueryValueEx( phkResult, L"OptionNum", 0, &dwType, (BYTE *)&OptionNum, &dwSize ) )
			return false;
		    for(unsigned int i=1;i<=OptionNum;i++)
		    	{
			TCHAR SubReg[MAX_PATH];
			DWORD	Width,Height;
			StringCchPrintf(SubReg,MAX_PATH,TEXT("\\Software\\Microsoft\\Pictures\\Camera\\OEM\\PictureResolution\\%d"),i);
			if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, SubReg, 0, 0, &phkResult ))
			       return false;
			 if( ERROR_SUCCESS != RegQueryValueEx( phkResult, L"Width", 0, &dwType, (BYTE *)&Width, &dwSize ) )
				return false;
			 if( ERROR_SUCCESS != RegQueryValueEx( phkResult, L"Height", 0, &dwType, (BYTE *)&Height, &dwSize ) )
				return false;
			 if(pshcc->nResolutionWidth == Width && pshcc->nResolutionHeight == Height)
			 	{
				HKEY hkCamera = NULL; //zqs
				if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
				{
					RegSetValueEx(hkCamera, TEXT("Resolution"), 0, REG_DWORD, (BYTE*)&i, sizeof(unsigned int));
					RegCloseKey(hkCamera);
				}
				RegCloseKey( phkResult );
			 	return TRUE;
			 	}
			}

		}else
		{
		    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"\\Software\\Microsoft\\Pictures\\Camera\\OEM\\VideoProfile", 0, 0, &phkResult ))
		       return false;
		    if( ERROR_SUCCESS != RegQueryValueEx( phkResult, L"OptionNum", 0, &dwType, (BYTE *)&OptionNum, &dwSize ) )
			return false;
		    for(unsigned int i=1;i<=OptionNum;i++)
		    	{
			TCHAR SubReg[MAX_PATH];
			DWORD	Width,Height;
			StringCchPrintf(SubReg,MAX_PATH,TEXT("\\Software\\Microsoft\\Pictures\\Camera\\OEM\\VideoProfile\\%d"),i);
			if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, SubReg, 0, 0, &phkResult ))
			       return false;
			 if( ERROR_SUCCESS != RegQueryValueEx( phkResult, L"Width", 0, &dwType, (BYTE *)&Width, &dwSize ) )
				return false;
			 if( ERROR_SUCCESS != RegQueryValueEx( phkResult, L"Height", 0, &dwType, (BYTE *)&Height, &dwSize ) )
				return false;
			 if(pshcc->nResolutionWidth == Width && pshcc->nResolutionHeight == Height)
			 	{
				HKEY hkCamera = NULL; //zqs
				if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
				{
					RegSetValueEx(hkCamera, TEXT("VideoProfile"), 0, REG_DWORD, (BYTE*)&i, sizeof(unsigned int));
					RegCloseKey(hkCamera);
				}
				RegCloseKey( phkResult );
			 	return TRUE;
			 	}
			}

		}
	    RegCloseKey( phkResult );
	return	FALSE;
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


