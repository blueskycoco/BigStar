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
#include "BirdCameraAPI.h"
//#include "OEMCameraCapture.h"


CCameraAPI::CCameraAPI()
{
	exposure = 3;
	picture_number=0;
	EnterPIMG=0;
	Enable_Dialog=0;
	bInStartMenu=0;
	bKeyDown=0;
	bOutOfMemory=FALSE;
	DuringInCapture=NotInCapture;
	// Global Variables:
	hInst=NULL;			// The current instance
	hWndApp=NULL;			//The window instance
	hWndDialog=NULL;
	hWndMenu = NULL;             //Menu handle
	hWndMenuStill = NULL;
	gcap.m_Flash_State = Flash_On;//打开闪光灯
	gcap.m_Still_Capture_Mode=StillCaptureMode_Normal_capture;//普通拍照
	gcap.m_Theme_Frame=0;
	m_nFileFormat=FILE_FORMAT_MP4;
	gcap.m_WhiteBanlance=WhiteBanlence_Auto;
	pshcc_last=NULL;
	pshcc=NULL;
	bCaptureWithFrame=FALSE;
	gcap.m_Resolution_Still=Resolution_Still_SXGA;//每默认是SXGA
	gcap.m_Resolution_Capture=Resolution_Capture_QVGA;//每默认是QVGA
	dwCameraAPIContext = reinterpret_cast<DWORD>( this );
	bFirstEnter=TRUE;
	bCaptureWithFrame_Last=FALSE;
	bModeChanged=FALSE;
}

CCameraAPI::~CCameraAPI()
{
	dwCameraAPIContext=NULL;
}


void CCameraAPI::SetupVideoWindow(IVideoWindow *pVW)
{
	RECT rt;
	long lWidth, lHeight;	
	HRESULT hr;

	if (pVW)
	{
		CHK( pVW->put_Owner((OAHWND)hWndApp)) ;
		CHK( pVW->put_WindowStyle(WS_CHILD)); 
		//hr = pVW->SetWindowForeground(OATRUE); 
		//hr = pVW->put_MessageDrain(OATRUE); 
		//hr = pVW->put_AutoShow(OATRUE); 		
	}

	if (GetWindowRect(hWndApp, &rt))
	{				

		CHK(pVW->get_Width(&lWidth));
		CHK(pVW->get_Height(&lHeight));
		if ((lWidth < rt.right)&&(lHeight<rt.bottom))
		{
			CHK(pVW->put_Left(rt.left + (rt.right-rt.left-lWidth)/2));
			CHK(pVW->put_Top( (rt.bottom-rt.top-lHeight)/2));		
		}
		else
		{
			CHK(pVW->put_Left(rt.left));
			CHK(pVW->put_Top(0));
			CHK(pVW->put_Width(rt.right));
			CHK(pVW->put_Height(rt.bottom-rt.top));
		}		
	}	
	return;
Cleanup:
	RETAILMSG(1, (TEXT("Return false at SetupVideoWindow") ) );
	return;
}

HRESULT CCameraAPI::InitCamera(HINSTANCE hInstance,HANDLE* events)
{
	MSG msg;
	HACCEL hAccelTable;

	hEvent[0]=events[0];
	hEvent[1]=events[1];
	hEvent[2]=events[2];
	hEvent[3]=events[3];
	hEvent[4]=events[4];
	hEvent[5]=events[5];

	//Init COM
	// Get COM interfaces
    if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
	{
	    RETAILMSG(1, (TEXT("CoInitialize Failed!\r\n")));
	    return FALSE;
	}
		
	RETAILMSG(1, (TEXT("InitCamera hInstance=0x%x!\r\n"),hInstance));

	// Perform application initialization:
	if (!InitInstance (hInstance, SW_SHOW)) 
	{
		RETAILMSG(1, (TEXT("InitInstance return false!!!\r\n")));
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WCETEST);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
	
	  	//RETAILMSG(1, (TEXT("GetMessage msg=0x%x,hwnd=0x%x,message=0x%x!\r\n"),msg,msg.hwnd,msg.message));
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Finished with COM
	CoUninitialize();

	//CoFreeLibrary(hInstance);
	
	return msg.wParam;
}


//
//  FUNCTION: InitInstance(HANDLE, int)
//
//  PURPOSE: Saves instance handle and creates main window
//
//  COMMENTS:
//
//    In this function, we save the instance handle in a global variable and
//    create and display the main program window.
//
HRESULT	CCameraAPI::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr=S_OK;
	BOOL fActivated;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_WCETEST, szWindowClass, MAX_LOADSTRING);
	//MyRegisterClass(hInstance, szWindowClass);
	WNDCLASS	wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW|CS_PARENTDC;
	wc.lpfnWndProc		=CCameraAPI::WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WCETEST));
	wc.hCursor			= 0;
	wc.hbrBackground	= (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= szWindowClass;

	RegisterClass(&wc);

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	if(FAILED(ActivatePreviousInstance(szWindowClass, szTitle, &fActivated)) ||
	        fActivated)
	{
		RETAILMSG(1, (TEXT("ActivatePreviousInstance failed!!!\r\n")));
	    return(0);
	}


	// Create the main window.    
	//         WS_CLIPCHILDREN, // Setting this to 0 gives a default style we don't want.  Use a benign style bit instead.

	hWndApp = CreateWindowEx(WS_EX_TOPMOST,//WS_EX_CAPTIONOKBTN ,//|
	szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWndApp)
	{
		RETAILMSG(1, (TEXT("hWndApp is null!!!\r\n")));
		return FALSE;
	}

	//hWndApp=pshcc->hwndOwner;
	//SetWindowLong(hWndApp, GWL_WNDPROC, (DWORD)WndProc);
	RotateHandle(TRUE, DMDO_0);
	{
		//写入注册表，表示采用Bird应用程序
		unsigned int lpData;
		HKEY hkCamera = NULL; //zqs
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
		{
			lpData=1;
			RegSetValueEx(hkCamera, TEXT("BirdCameraAPI"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			RegCloseKey(hkCamera);
		}
	}	

	ShowWindow(hWndApp, SW_SHOW);
	UpdateWindow(hWndApp);
	RETAILMSG(1, (TEXT("SetEvent!!!\r\n")));
	//SetEvent(hEvent[4]);
	RETAILMSG(1, (TEXT("WaitForSingleObject(hEvent[5],INFINITE)!!!\r\n")));
	if(pshcc)
	{
	
		CHK( InitCapFilter());
		if (SUCCEEDED(hr))
		{	CHK(OpenCamera(pshcc));
			CHK(StartPreview());
			gcap.fCapturing  = FALSE;
			//gcap.pVWS->put_WindowState(SW_HIDE);		
		}
		else
		{
			RETAILMSG(1,(TEXT("CamTest: Fail to create Capture filter. \r\n")));  
			goto Cleanup;
		}
	}
	return TRUE;
Cleanup:
	RETAILMSG(1, (TEXT("Return false at OpenCamera") ) );
	return hr;

}


//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
/**************************************************************************************

   WndProc

 **************************************************************************************/
int VK_ACTION_Start=0;
int VK_ACTION_Stop=0;
LRESULT CALLBACK CCameraAPI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;

	CCameraAPI *pThis;
	pThis = (CCameraAPI*) dwCameraAPIContext;
 
	//RETAILMSG(1,(TEXT("hWnd=0x%x, message=0x%x, wParam=0x%x, lParam=0x%x,Enable_Dialog =0x%x\r\n"), hWnd,message,wParam,lParam,pThis->Enable_Dialog));

	switch(message)
	{
		case	WM_GRAPHNOTIFY:
			pThis->ProcessDShowEvent(hWnd);
			break;

	       /* case WM_ACTIVATE:
	            if (WA_ACTIVE == LOWORD(wParam))
	            {
			// Set the focus to the main dialog if we are activated
			SetFocus(hWnd);
			SetCapture(hWnd);
	            }
	            break;*/ 

		case WM_LBUTTONUP://鼠标放开消息
				{
					int x,y;//获取两个坐标
					RECT rt;
					x=LOWORD(lParam);
					y=HIWORD(lParam);
					RETAILMSG(1,(TEXT("x=%d, y=%d \r\n"), x,y));
					GetWindowRect(hWnd, &rt);
					y=y+rt.top;
					RETAILMSG(1,(TEXT("x=%d, y=%d, rt.top=%d\r\n"), x,y,rt.top));
					if(x>=190&&x<=190+menu_size2&&y>=276&&y<=276+menu_size2)//模式切换，向前
						pThis->Previous_Menu_Proc();
					if(x>=216&&x<=216+menu_size2&&y>=276&&y<=276+menu_size2)//模式切换，向后
						pThis->Next_Menu_Proc();
					if(x>=100&&x<=100+24&&y>=280&&y<=280+22 && pThis->bCaptureWithFrame)//场景选择，向前
						pThis->Previous_Theme_Frame();
					if(x>=126&&x<=126+24&&y>=280&&y<=280+22 && pThis->bCaptureWithFrame)//场景选择，向后
						pThis->Next_Theme_Frame();
					if(x>=180&&x<=180+menu_size1&&y>=296&&y<=296+menu_size1)//功能菜单
						pThis->VisableDialog();
					if(x>=210&&x<=210+menu_size1&&y>=296&&y<=296+menu_size1)//图片管理
					{
						{
							//写入注册表，表示进入PIMG
							unsigned int lpData;
							HKEY hkCamera = NULL; //zqs
							if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
							{
								lpData=1;
								RegSetValueEx(hkCamera, TEXT("EnterPIMG"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
								RegCloseKey(hkCamera);
							}
						}
						ShowWindow(hWnd,SW_HIDE);//进入PIMG
						}
					if(x>=220&&x<=220+menu_size2 && y>=2&&y<=2+menu_size2)
						ShowWindow(hWnd,SW_HIDE);	//关闭窗口	
				}
			break;

		case	WM_KEYDOWN:
 			switch(wParam)
				{
					case	ID_Capture_Key:
						pThis->CaptureKeyUpProc();   //按键处理消息
						break;

					default:
						break;
				}
			break;

		case WM_COMMAND:
		    switch (LOWORD(wParam))
		    {
		    		case	IDOK:
						PostMessage(hWnd, WM_CLOSE, wParam, lParam);
						break;
				default:
		            goto DoDefault;
		    }
		    break;

		case	WM_CAPTURECHANGED:
			if(gcap.fPreviewing)
				SetCapture(hWnd);
			break;

		case 0xc009:

			if(pThis->bFirstEnter==TRUE)
				{
				pThis->bFirstEnter=FALSE;
				break;
				}
			if((LOWORD(wParam) == 3))
				{
				RETAILMSG(1, (TEXT("Enter message VK_ACTION start preview") ) );
				pThis->OpenCamera(pThis->pshcc);
				ShowWindow(hWnd,SW_SHOW);
				pThis->StartPreview();
				}
			if(pThis->Enable_Dialog )
				pThis->Enable_Dialog =0;
			break;

		case VK_ACTION:				
			 if(!gcap.fCapturing)
				{
				if((LOWORD(wParam) == FALSE) )
					{
					//if(!pThis->bModeChanged)
						{
						RETAILMSG(1, (TEXT("Enter message VK_ACTION stop preview") ) );
						ShowWindow(hWnd,SW_HIDE);
						pThis->StopPreview();
						if(!pThis->Enable_Dialog && !pThis->bOutOfMemory)
							SetEvent(pThis->hEvent[1]);  //对应于返回
						}//else
					//pThis->bModeChanged=FALSE;

					}
				}else
				gcap.fCapturing=!gcap.fCapturing;

			break;

		/*case	WM_WINDOWPOSCHANGED:
			{
			LPWINDOWPOS	lpwp = (LPWINDOWPOS) lParam;
			RETAILMSG(1,(TEXT("lpwp->hwnd=0x%x,lpwp->hwndInsertAfter=0x%x,lpwp->x=0x%x,lpwp->y=0x%x,lpwp->cx=0x%x,lpwp->cy=0x%x,lpwp->flags=0x%x\r\n"), lpwp->hwnd,lpwp->hwndInsertAfter,lpwp->x,lpwp->y,lpwp->cx,lpwp->cy,lpwp->flags));
			}
			break;*/

		case WM_PAINT:
			{
			    HDC hdc;
			    PAINTSTRUCT ps;
			    RECT rect;
			    hdc = BeginPaint(hWnd, &ps);
			    GetClientRect(hWnd, &rect);
			    EndPaint (hWnd, &ps);
			}
			break;

		case WM_DESTROY:
			{
				//写入注册表，表示退出Bird应用程序
				unsigned int lpData;
				HKEY hkCamera = NULL; //zqs
				if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
				{
					lpData=0;
					RegSetValueEx(hkCamera, TEXT("BirdCameraAPI"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
					RegCloseKey(hkCamera);
				}
			}
			pThis->RotateHandle(FALSE, pThis->CurrentAngle);
			//StopPreview();		
			pThis->UninitGraph();
			SAFE_RELEASE(gcap.pCap);
			//CoUninitialize();
			//DestroyWindow(hWnd);
			PostQuitMessage(S_FALSE );
			break;

		DoDefault:
		default:
			lResult = DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}
	//RETAILMSG(1,(TEXT("Leaving message windows hWnd=0x%x, message=0x%x, wParam=0x%x, lParam=0x%x\r\n"), hWnd,message,wParam,lParam));
	return(lResult);
}


// Look for cam.cfg
// If it doesn't exist, create it, and set picture number to 1.
// If it exists, read the value stored inside, increment the number, and write it back.
void CCameraAPI::UpdatePictureNumber()
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


/****************************************************************************

   ActivatePreviousInstance

  ****************************************************************************/
HRESULT CCameraAPI::ActivatePreviousInstance(
    const TCHAR* pszClass,
    const TCHAR* pszTitle,
    BOOL* pfActivated
    )
{
	HRESULT hr = S_OK;
	int cTries;
	HANDLE hMutex = NULL;

	*pfActivated = FALSE;
	cTries = 5;
	while(cTries > 0)
	{
	    hMutex = CreateMutex(NULL, FALSE, pszClass); // NOTE: We don't want to own the object.
	    if(NULL == hMutex)
	    {
		RETAILMSG(1, (TEXT("hMutex is null!!!\r\n")));
	        // Something bad happened, fail.
	        hr = E_FAIL;
	        goto Exit;
	    }else
	    	{

		    if(GetLastError() == ERROR_ALREADY_EXISTS)
		    {
		        HWND hwnd;
			RETAILMSG(1, (TEXT("GetLastError() == ERROR_ALREADY_EXISTS!!!\r\n")));

		        CloseHandle(hMutex);
		        hMutex = NULL;

		        // There is already an instance of this app
		        // running.  Try to bring it to the foreground.

		        hwnd = FindWindow(pszClass, pszTitle);
		        if(NULL == hwnd)
		        {
				RETAILMSG(1, (TEXT("NULL == hwnd!pszClass=0x%xGetLastError() =0x%x!!\r\n"),pszClass,GetLastError() ));

		            // It's possible that the other window is in the process of being created...
		            Sleep(500);
		            hwnd = FindWindow(pszClass, pszTitle);
		        }

		        if(NULL != hwnd) 
		        {
		            // Set the previous instance as the foreground window

		            // The "| 0x01" in the code below activates
		            // the correct owned window of the
		            // previous instance's main window.
		            SetForegroundWindow((HWND) (((ULONG) hwnd) | 0x01));

		            // We are done.
		            *pfActivated = TRUE;
		            break;
		        }

		        // It's possible that the instance we found isn't coming up,
		        // but rather is going down.  Try again.
		        cTries--;
		    }
		    else
		    {
			RETAILMSG(1, (TEXT("We were the first one to create the mutex\r\n")));
		        // We were the first one to create the mutex
		        // so that makes us the main instance.  'leak'
		        // the mutex in this function so it gets cleaned
		        // up by the OS when this instance exits.
		        break;
		    }
	    	}
	}

	if(cTries <= 0)
	{
		RETAILMSG(1, (TEXT("Someone else owns the mutex but we cannot find their main window to activate\r\n")));
	    // Someone else owns the mutex but we cannot find
	    // their main window to activate.
	    hr = E_FAIL;
	    goto Exit;
	}

Exit:
	return(hr);
}

HRESULT CCameraAPI::StartCaptureVideoInternal()
{
	HRESULT  hr = S_OK;
	LONGLONG dwStart = 0, dwEnd = 0;
	WORD     wStartCookie = 0, wEndCookie = 0;
	//UpdatePictureNumber();

	// Make sure that the capture graph builder is present
	if( gcap.pBuilder== NULL ) 
	{
	    return false;
	}
	CHK(gcap.pFileSinkFilter->SetFileName(pshcc->szFile, NULL));
	// To start the video capture, call ICaptureGraphBuilder2::ControlStream. 
	dwStart=10000000;
	dwEnd=MAXLONGLONG;
	CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, &dwStart, &dwEnd, 0, 0));
	if(pshcc->Mode == CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
		CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE,&MEDIATYPE_Audio, gcap.pAudioCaptureFilter,&dwStart, &dwEnd, 0, 0 ));
		}
	ValidateMemory(0);
	SystemIdleTimerReset();
	if(VideoTime<30000)//如果是小于30s，则定时VideoTime时长，否则定时30s
		SetTimer(hWndApp, nSystemTimer, VideoTime, TimerProc );
	else
		SetTimer(hWndApp, nSystemTimer, 30000, TimerProc );

	//StartPreview();
	DuringInCapture=InCapture;
	gcap.fCapturing=TRUE;
    return hr;

Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at StartCaptureVideoInternal") ) );
	//if(x)
		//free(x);
	return hr;
}

HRESULT CCameraAPI::StopCaptureVideoInternal(bool	OutOfMemory)
{
	HRESULT   hr = S_OK;
	//LONGLONG  dwStart = 0, dwEnd = 0;
	//WORD      wStartCookie = 1, wEndCookie = 2;
	//int count=0;

	/*IMediaSeeking *pMediaSeeking;

	CHK(gcap.pGraph->QueryInterface( &pMediaSeeking ));*/

/*/ Stop the capture of the graph
	dwStart = 0;
	CHK(pMediaSeeking->GetCurrentPosition( &dwEnd ));
	CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, &dwStart, &dwEnd, wStartCookie, wEndCookie));
	if(pshcc->Mode ==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE,&MEDIATYPE_Audio, gcap.pAudioCaptureFilter,&dwStart, &dwEnd, wStartCookie, wEndCookie ));
/*	
	dwStart=0;
	dwEnd=MAXLONGLONG;
	CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, &dwStart, &dwEnd, wStartCookie, wEndCookie));
	if(pshcc->Mode == CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
		CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE,&MEDIATYPE_Audio, gcap.pAudioCaptureFilter,&dwStart, &dwEnd, wStartCookie, wEndCookie ));
		}*/
	// Wait for the ControlStream event. 
		//RETAILMSG(1,(TEXT("zqs:Wating for the control stream events dwEnd=%d!!!!!!!!1\r\n"),dwEnd));
		CHK(StopPreview());
		ShowWindow(hWndApp,SW_HIDE);
		ReleaseCapture();
		if(OutOfMemory)
			SetEvent(hEvent[2]);//对应于S_OK 
		else
			SetEvent(hEvent[0]);//对应于S_OK 
	DuringInCapture=NotInCapture;
	return hr;

Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at StopCaptureVideoInternal") ) );
	return hr;
}

int count=0;
HRESULT CCameraAPI::ProcessDShowEvent(HWND hWnd)
	{
	    HRESULT hr = S_OK;
	    long    lEventCode, lParam1, lParam2;

	    //IMediaEvent *pMediaEvent;
	    //IGraphBuilder *pFilterGraph;
	    //IMediaControl *pMediaControl;

		if( gcap.pBuilder == NULL )
		{
			RETAILMSG(1,(TEXT("zqs:gcap.pBuilder error!!!!!!!!1\r\n")));
		}

		//CHK(gcap.pBuilder->GetFiltergraph( &pFilterGraph ));
		//CHK(pFilterGraph->QueryInterface( &pMediaEvent ));
		CHK(gcap.pME->GetEvent( &lEventCode, &lParam1, &lParam2, 0 ));
		RETAILMSG(1,(TEXT("zqs:lEventCode=0x%x!!!!!!!!1\r\n"),lEventCode));
		// To close the file, get the IMediaControl interface, stop and 
		// restart the graph
		//CHK(pFilterGraph->QueryInterface( &pMediaControl ));

		if( lEventCode == EC_VIDEOFRAMEREADY ) 
			{
			//SetCapture(hWnd);
			//SetFocus(hWnd) ;	
			RETAILMSG(1, (TEXT("lEventCode == EC_VIDEOFRAMEREADY GetCapture() =x%x  GetFocus()=0x%x GetActiveWindow()=0x%x GetForegroundWindow=0x%x!!!!!!!"),GetCapture(),GetFocus(),GetActiveWindow(),GetForegroundWindow()) );
				//SetCapture(hWnd);
			}
		
		if( lEventCode == EC_STREAM_CONTROL_STOPPED)   // We have received a control stream stop event
		{
			//RETAILMSG(1,(TEXT("zqs:Recording done ...!!!!!!!!1\r\n")));      

			//CHK(StopPreview());
			//Sleep(500);	
			//CHK(gcap.pFileSinkFilter->SetFileName(L"//temp//temp.mp4", NULL));
			//CHK(StartPreview());
			RETAILMSG(1,(TEXT("zqs:File captured ...!!!!!!!!1\r\n")));
			//count++;
			//if(count==2)
				//{
				//count=0;
				StopPreview();
				ShowWindow(hWnd,SW_HIDE);
				SetEvent(hEvent[0]);//对应于S_OK 
				//}
		}/*else*/
		/*if( lEventCode == EC_STREAM_CONTROL_STOPPED)   // We have received a control stream stop event
		{
			RETAILMSG(1,(TEXT("zqs:Recording done ...!!!!!!!!2\r\n")));      

			CHK(StopPreview());
			ReleaseCapture();
			SetEvent(hEvent[0]);//对应于S_OK 

			//Sleep(500);	
			//CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, 0, 0 ,0,0 ));//video
			//if(DuringInCapture==InCapture)
				//{CHK(gcap.pFileSinkFilter->SetFileName(L"//temp//temp.mp4", NULL));}
			//CHK(StartPreview());
			RETAILMSG(1,(TEXT("zqs:File captured ...!!!!!!!!2\r\n")));
		}
	    else*/
	    if( lEventCode == EC_CAP_FILE_COMPLETED )
	    {
	    		/*if(gcap.fCapturing)
	    		{
				CHK(StopPreview());
				CHK(StartPreview());
				gcap.fCapturing=false;
	    		}*/
			RETAILMSG(1,(TEXT("zqs:lEventCode == EC_CAP_FILE_COMPLETED1\r\n")));
			StopPreview();
			ShowWindow(hWnd,SW_HIDE);
			ReleaseCapture();
			SetEvent(hEvent[0]);//对应于S_OK 
			//PostMessage(hWnd, WM_CLOSE, 0, 0);
			//ExitThread(S_OK);
			//SetupVideoWindow(gcap.pVWP);
	    		//gcap.fCapturing=!gcap.fCapturing;
			RETAILMSG(1,(TEXT("zqs:File captured1 ...!!!!!!!!1\r\n")));
	    }else  if(EC_COMPLETE == lEventCode) // If this is the end of the clip, reset to beginning
        {
			//MessageBox(NULL, L"EC_COMPLETE", L"Info", MB_OK);
            PostMessage(hWnd,WM_CLOSE,0,0);
		}

	  gcap.pME->FreeEventParams( lEventCode, lParam1, lParam2 );


	    return S_OK;
Cleanup:
	RETAILMSG(1, (TEXT("Return false at ProcessDShowEvent") ) );
	return hr;
	}


HRESULT CCameraAPI::CaptureKeyUpProc()
{
	HRESULT hr = E_FAIL;
	if(pshcc->Mode == CAMERACAPTURE_MODE_STILL)
		CHK(CaptureStillImage());    //如果是静态的话，则拍照
	if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO ||pshcc->Mode==CAMERACAPTURE_MODE_VIDEOONLY)
		{
		if(DuringInCapture==NotInCapture)
			StartCaptureVideoInternal();   //如果不在录像过程中，开始录像
		else if(DuringInCapture==InCapture)
			StopCaptureVideoInternal(FALSE);  //在录像过程中，停止录像
		}
	return true;
Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at CaptureKeyUpProc") ) );
	return hr;
}
HRESULT CCameraAPI::Previous_Menu_Proc()
{
	HRESULT hr = E_FAIL;
	int CurrentMode;
	bModeChanged=TRUE;

	RETAILMSG(1,(TEXT("zqs:Previous_Menu_Proc before videomode is %d!!!!!!!!1\r\n"),pshcc->Mode));	
	CurrentMode=pshcc->Mode;
	
	if(CurrentMode==CAMERACAPTURE_MODE_STILL && !bCaptureWithFrame)
		{
		pshcc->Mode=CAMERACAPTURE_MODE_VIDEOONLY;
		SetFileFormat(m_nFileFormat);
		SetResolution(gcap.m_Resolution_Capture,pshcc->Mode);
		CHK(OpenCamera(pshcc));
		CHK(StartPreview());
		}
	else if(CurrentMode==CAMERACAPTURE_MODE_VIDEOONLY || CurrentMode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
		//gcap.m_Still_Capture_Mode=StillCaptureMode_Normal_capture;  //设成普通拍照
		pshcc->Mode=CAMERACAPTURE_MODE_STILL;
		SetFileFormat(FILE_FORMAT_JPG);
		bCaptureWithFrame=TRUE;
		CHK(OpenCamera(pshcc));
		CHK(StartPreview());
		}
	else if(CurrentMode==CAMERACAPTURE_MODE_STILL && bCaptureWithFrame)
	//if(CurrentMode==CAMERACAPTURE_MODE_VIDEOONLY)
		{
		pshcc->Mode=CAMERACAPTURE_MODE_STILL;
		SetFileFormat(FILE_FORMAT_JPG);
		SetResolution(gcap.m_Resolution_Still,pshcc->Mode);
		bCaptureWithFrame=FALSE;
		CHK(OpenCamera(pshcc));
		CHK(StartPreview());
		}

		RETAILMSG(1,(TEXT("zqs:Previous_Menu_Proc after videomode is %d!!!!!!!!1\r\n"),pshcc->Mode));
	return hr;
Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at Previous_Menu_Proc") ) );
	return hr;
}
HRESULT CCameraAPI::Next_Menu_Proc()
{
	HRESULT hr = E_FAIL;
	int CurrentMode;

	bModeChanged=TRUE;
	
	RETAILMSG(1,(TEXT("zqs:Next_Menu_Proc before videomode is %d!!!!!!!!1\r\n"),pshcc->Mode));	
	CurrentMode=pshcc->Mode;
	
	if(CurrentMode==CAMERACAPTURE_MODE_STILL && !bCaptureWithFrame)
		{
		//gcap.m_Still_Capture_Mode=StillCaptureMode_Normal_capture;  //设成普通拍照
		pshcc->Mode=CAMERACAPTURE_MODE_STILL;
		bCaptureWithFrame=TRUE;
		SetFileFormat(FILE_FORMAT_JPG);
		CHK(OpenCamera(pshcc));
		CHK(StartPreview());
		}
	else if(CurrentMode==CAMERACAPTURE_MODE_STILL && bCaptureWithFrame)
		{
		pshcc->Mode=CAMERACAPTURE_MODE_VIDEOONLY;
		SetResolution(gcap.m_Resolution_Capture,pshcc->Mode);
		SetFileFormat(m_nFileFormat);
		CHK(OpenCamera(pshcc));
		CHK(StartPreview());
		}
	else
	//if(CurrentMode==CAMERACAPTURE_MODE_VIDEOONLY)
		{
		pshcc->Mode=CAMERACAPTURE_MODE_STILL;//重新开始静态图片捕捉
		SetFileFormat(FILE_FORMAT_JPG);
		SetResolution(gcap.m_Resolution_Still,pshcc->Mode);
		bCaptureWithFrame=FALSE;
		CHK(OpenCamera(pshcc));
		CHK(StartPreview());
		}

	RETAILMSG(1,(TEXT("zqs:Next_Menu_Proc after videomode is %d!!!!!!!!1\r\n"),pshcc->Mode));
	return hr;
Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at Next_Menu_Proc") ) );
	return hr;
}

HRESULT CCameraAPI::VisableDialog()
{
	HRESULT hr = S_OK;
	RETAILMSG(1,(TEXT("zqs:Enter visable dialog !!!!!!!1\r\n")));
	//gcap.fPreviewing=true;//to stop preview
	Enable_Dialog=1;
	CHK(StopPreview());	
	//ReleaseCapture();
	ReleaseCapture();
	//ShowWindow(hWndApp,SW_HIDE);
	//if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
	SetWindowPos(hWndApp,HWND_NOTOPMOST,hWndrect.left,hWndrect.top,hWndrect.right,hWndrect.bottom,SWP_SHOWWINDOW);
	DialogBox(hInst,MAKEINTRESOURCE(IDD_CAMCTRL),hWndApp,DialogProc);
	//else
		//DialogBox(hInst,MAKEINTRESOURCE(IDD_CAMCTRL_VIDEO),hWndApp,DialogProc);
	return	hr;

Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at VisableDialog") ) );
	return hr;
}

BOOL CALLBACK CCameraAPI::DialogProc(
  HWND hwndDlg, 
  UINT uMsg, 
  WPARAM wParam, 
  LPARAM lParam
)
{
    BOOL bHandled = TRUE;
	CCameraAPI *pThis;
	pThis = (CCameraAPI*) dwCameraAPIContext;
	//RETAILMSG(1,(TEXT("DialogProc:hWnd=0x%x, message=0x%x, wParam=0x%x, lParam=0x%x, \r\n"), hwndDlg,uMsg,wParam,lParam));

    switch (uMsg)
    {
        case WM_INITDIALOG:
            // Make sure the dialog box was created
            if (!pThis->InitDialog(hwndDlg))
            {
            	pThis->Enable_Dialog=0;
                PostMessage(hwndDlg, WM_CLOSE, 0, 0);
            }

            break;

        case WM_COMMAND:
			switch(LOWORD(wParam))
				{
					TCHAR szSel[32];
					case	IDC_RUN_CAMERA:
						//ShowWindow(hWndDialog,SW_HIDE);
						pThis->HandleDLGRUNCamera(hwndDlg);
						//SetWindowPos(pThis->hWndApp,HWND_TOPMOST,0,0,240,320,SWP_SHOWWINDOW);
						bHandled=EndDialog(hwndDlg, IDCANCEL);
						//ShowWindow(pThis->hWndApp,SW_SHOW);
						//pThis->Enable_Dialog=0;
						//pThis->StartPreview();
						break;
					case	IDOK:
						pThis->HandleDLGRUNCamera(hwndDlg);
						//SetWindowPos(pThis->hWndApp,HWND_TOPMOST,0,0,240,320,SWP_SHOWWINDOW);
						bHandled=EndDialog(hwndDlg, IDCANCEL);
						//ShowWindow(pThis->hWndApp,SW_SHOW);
						//pThis->Enable_Dialog=0;
						//pThis->StartPreview();
						break;

					case	IDC_VIDEOMODE:
							if(HIWORD(wParam) == CBN_SELENDOK)
								{
								//运行模式
								LONG nIndex = SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_GETCURSEL , 0, 0);
								SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
								if(_tcsicmp(szSel, _T("拍照")) == 0)
									{
									pThis->pshcc->Mode=CAMERACAPTURE_MODE_STILL;
									pThis->bCaptureWithFrame=FALSE;
									pThis->ChangeDlgItem(hwndDlg);
									}
								if(_tcsicmp(szSel, _T("相框拍照")) == 0)
									{
									pThis->pshcc->Mode=CAMERACAPTURE_MODE_STILL;
									pThis->bCaptureWithFrame=TRUE;
									pThis->ChangeDlgItem(hwndDlg);
									}
								else if(_tcsicmp(szSel, _T("摄像")) == 0)
									{
									pThis->pshcc->Mode=CAMERACAPTURE_MODE_VIDEOONLY;
									pThis->ChangeDlgItem(hwndDlg);
									}
								}
							break;

					case	IDC_STILL_CAPTURE_MODE:
							if(HIWORD(wParam) == CBN_SELENDOK)
								{
								//运行模式
								LONG nIndex = SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_GETCURSEL , 0, 0);
								SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
								if(_tcsicmp(szSel, _T("普通拍照")) == 0)
									gcap.m_Still_Capture_Mode=StillCaptureMode_Normal_capture;
								else if(_tcsicmp(szSel, _T("定时5秒拍照")) == 0)
									gcap.m_Still_Capture_Mode=StillCaptureMode_Timer_Capture;
								else if(_tcsicmp(szSel, _T("连拍5张")) == 0)
									gcap.m_Still_Capture_Mode=StillCaptureMode_Sequence_Capture;
								}
							break;

					case	IDC_VIDEOFORMAT:
							if(HIWORD(wParam) == CBN_SELENDOK)
								{
								//运行模式
								LONG nIndex = SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_GETCURSEL , 0, 0);
								SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
								if(_tcsicmp(szSel, _T("mp4")) == 0)
									pThis->m_nFileFormat=FILE_FORMAT_MP4;
								else if(_tcsicmp(szSel, _T("3gp")) == 0)
									pThis->m_nFileFormat=FILE_FORMAT_3GP;
								pThis->SetFileFormat(pThis->m_nFileFormat);
								}
							break;

					case	IDC_WHITEBALANCE:
							if(HIWORD(wParam) == CBN_SELENDOK)
								{
								//运行模式
								LONG nIndex = SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_GETCURSEL , 0, 0);
								SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
								if(_tcsicmp(szSel, _T("自动")) == 0)
									gcap.m_WhiteBanlance=WhiteBanlence_Auto;
								else if(_tcsicmp(szSel, _T("夜间")) == 0)
									gcap.m_WhiteBanlance=WhiteBanlence_Night;
								else if(_tcsicmp(szSel, _T("白天")) == 0)
									gcap.m_WhiteBanlance=WhiteBanlence_Day;
								else if(_tcsicmp(szSel, _T("多云")) == 0)
									gcap.m_WhiteBanlance=WhiteBanlence_Cloudy;
								else if(_tcsicmp(szSel, _T("荧光")) == 0)
									gcap.m_WhiteBanlance=WhiteBanlence_Fluor;
								else if(_tcsicmp(szSel, _T("白炽光")) == 0)
									gcap.m_WhiteBanlance=WhiteBanlence_Lamp;							
							}
							break;

			}
			break;


        case WM_CLOSE:
			pThis->Enable_Dialog=0;
            EndDialog(hwndDlg, IDCANCEL);
            break;

        /*case WM_ACTIVATE:
            if (WA_ACTIVE == LOWORD(wParam))
            {
                // Set the focus to the main dialog if we are activated
                SetFocus(hwndDlg);
            }
            break;*/


        default:
            // Specify that we didn't process this message, let the default
            // dialog box window procedure to process this message
            bHandled = FALSE;
            break;
    }

    return bHandled;
}

BOOL CCameraAPI::InitDialog(HWND hwndDlg)
{
    SHINITDLGINFO shidi;
    BOOL          bSuccess = TRUE;

    // Specify that the dialog box should stretch full screen
    ZeroMemory(&shidi, sizeof(shidi));
    shidi.dwMask  = SHIDIM_FLAGS;
    shidi.hDlg    = hwndDlg;
    shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIZEDLGFULLSCREEN ;

    SHInitDialog(&shidi);
		// create the menu bar
	SHMENUBARINFO mbi;
	ZeroMemory(&mbi, sizeof(SHMENUBARINFO));
	mbi.cbSize = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hwndDlg;
	mbi.nToolBarId = IDM_MENU;
	mbi.hInstRes = hInst;
	mbi.dwFlags = SHCMBF_EMPTYBAR;

	if(!SHCreateMenuBar(&mbi))
	{
	    // Couldn't create the menu bar.  Fail creation of the window.
	    return(-1);
	}

	//运行模式
	SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("拍照"));
	SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("相框拍照"));
	SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("摄像"));

	ChangeDlgItem(hwndDlg);



    return bSuccess;
}
HRESULT CCameraAPI::HandleDLGRUNCamera(HWND hwndDlg)
{
	HRESULT hr = E_FAIL;
	LONGLONG dwEnd = MAXLONGLONG;
	TCHAR szSel[32];
	
	RETAILMSG(1,(TEXT("zqs:HandleDLGRUNCamera before videomode is %d!!!!!!!!1\r\n"),pshcc->Mode));	
	
	if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
	{//处理静态拍照对话框消息
		int FlashOn = SendDlgItemMessage(hwndDlg, IDC_CHECK_FLASH, BM_GETSTATE , 0, 0);
		if(FlashOn & BST_CHECKED)//是否打开闪光灯
			gcap.m_Flash_State = Flash_On;
		else
			gcap.m_Flash_State = Flash_Off;
		//分辨率
		LONG nIndex = SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_GETCURSEL , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
		if(!bCaptureWithFrame)//不是相框拍照时才设置分辨率，是相框拍照时，不设分辨率
		{
			if(_tcsicmp(szSel, _T("1280*1024")) == 0)
				{
				gcap.m_Resolution_Still=Resolution_Still_SXGA;		//设置分辨率
				}
			else if(_tcsicmp(szSel, _T("640*512")) == 0)
				{
				gcap.m_Resolution_Still=Resolution_Still_QSXGA;
				}
			else if(_tcsicmp(szSel, _T("640*480")) == 0)
				{
				gcap.m_Resolution_Still=Resolution_Still_VGA;				
				}
			SetResolution(gcap.m_Resolution_Still,pshcc->Mode);
		}

		//场景选择
		nIndex = SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_GETCURSEL , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
		if(_tcsicmp(szSel, _T("LOVE")) == 0)
			gcap.m_Theme_Frame=0;
		else if(_tcsicmp(szSel, _T("TREE")) == 0)
			gcap.m_Theme_Frame=1;
		else if(_tcsicmp(szSel, _T("ZGB")) == 0)
			gcap.m_Theme_Frame=2;

	SetFileFormat(FILE_FORMAT_JPG);
		
		//CHK(OpenCamera(pshcc));
	}
	else
	{//处理视频对话框消息
	
		int stateA = SendDlgItemMessage(hwndDlg, IDC_CHECK_AUDIO, BM_GETSTATE , 0, 0);
		if(stateA & BST_CHECKED)//是否带音频
			{
			pshcc->Mode=CAMERACAPTURE_MODE_VIDEOWITHAUDIO;
			}
		else
			{
			pshcc->Mode=CAMERACAPTURE_MODE_VIDEOONLY;
			}
		//分辨率
		LONG nIndex = SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_GETCURSEL , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_GETLBTEXT, nIndex, (LPARAM)(LPCSTR)szSel);
		if(_tcsicmp(szSel, _T("320*240")) == 0)
			{
			gcap.m_Resolution_Capture=Resolution_Capture_QVGA;
			}
		else if(_tcsicmp(szSel, _T("176*144")) == 0)
			{
			gcap.m_Resolution_Capture=Resolution_Capture_QCIF;
			}
		SetResolution(gcap.m_Resolution_Capture,pshcc->Mode);
		//CHK(OpenCamera(pshcc));
	}
	return hr;
/*Cleanup:
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at HandleDLGRUNCamera") ) );
	return hr;*/
}


HRESULT CCameraAPI::GetUnconnectedPin(
    IBaseFilter *pFilter,   // Pointer to the filter.
    PIN_DIRECTION PinDir,   // Direction of the pin to find.
    IPin **ppPin)           // Receives a pointer to the pin.
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;
            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // Did not find a matching pin.
    return E_FAIL;
}




HRESULT CCameraAPI::ConnectFilters(
    IGraphBuilder *pGraph, // Filter Graph Manager.
    IPin *pOut,            // Output pin on the upstream filter.
    IBaseFilter *pDest)    // Downstream filter.
{
    if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }
#ifdef debug
        PIN_DIRECTION PinDir;
        pOut->QueryDirection(&PinDir);
        _ASSERTE(PinDir == PINDIR_OUTPUT);
#endif

    // Find an input pin on the downstream filter.
    IPin *pIn = 0;
    HRESULT hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (FAILED(hr))
    {
        return hr;
    }
    // Try to connect them.
    hr = pGraph->Connect(pOut, pIn);
    pIn->Release();
    return hr;
}

HRESULT CCameraAPI::ConnectFilters(
    IGraphBuilder *pGraph, 
    IBaseFilter *pSrc, 
    IBaseFilter *pDest)
{
    if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // Find an output pin on the first filter.
    IPin *pOut = 0;
    HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (FAILED(hr)) 
    {
        return hr;
    }
    hr = ConnectFilters(pGraph, pOut, pDest);
    pOut->Release();
    return hr;
}


HRESULT CCameraAPI::InitGraph(BOOL bCapture,BOOL bStill)
{
	HRESULT       hr = S_OK;
	IPersistPropertyBag    *pPropertyBag;

	 //InitCapFilter();

	// Create the Filter Graph Manager.
	CHK( CoCreateInstance(CLSID_FilterGraph, NULL,
				CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&gcap.pGraph));

	// Create the Capture Graph Builder.
	CHK( CoCreateInstance(CLSID_CaptureGraphBuilder, NULL,
					  CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
					  (void **)&gcap.pBuilder));

	CHK(gcap.pBuilder->SetFiltergraph(gcap.pGraph));

	CHK( CoCreateInstance(CLSID_VideoRenderer, NULL,
					  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
					  (void **)&gcap.pRenderP));

	CHK(gcap.pGraph->AddFilter(gcap.pRenderP,L"Video Render1"));

	CHK( gcap.pGraph->AddFilter(gcap.pCap,L"Video Capture Source"));	
	//hr = gcap.pGraph->AddFilter(gcap.pGrab,L"Grabber");

	if(bCapture)
		{
		
			// Initialize the audio capture filter
			if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
			{
				CHK(CoCreateInstance(CLSID_AudioCapture, NULL,
								  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
								  (void **)&gcap.pAudioCaptureFilter));
				CHK(gcap.pAudioCaptureFilter->QueryInterface( &pPropertyBag ));
				pPropertyBag->Load( NULL, NULL );
				pPropertyBag->Release();
				CHK( gcap.pGraph->AddFilter( gcap.pAudioCaptureFilter, L"Audio Capture Filter" ));

						//Create audio encoder

				CHK( CoCreateInstance(CLSID_ArcAmrnbEncoder, NULL,
								  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
								  (void **)&gcap.pAudioEncoder));
				CHK( gcap.pGraph->AddFilter( gcap.pAudioEncoder, L"Audio Capture Filter" ));
			}
			// 
			// Third step: Create the video encoder DMO, load the WMV9 encoder, and 
			// add it to the graph
			//

			/*/ Create the video encoder
			hr = CoCreateInstance(CLSID_DMOWrapperFilter, NULL,
							  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
							  (void **)&gcap.pVideoEncoder);
			gcap.pVideoEncoder->QueryInterface( &gcap.pWrapperFilter );

			// Load the WMV9 DMO
			gcap.pWrapperFilter->Init( CLSID_CWMV9EncMediaObject, DMOCATEGORY_VIDEO_ENCODER );

			// Everything succeeded, let's add the encoder to the graph
			gcap.pGraph->AddFilter( gcap.pVideoEncoder, L"WMV9 DMO Encoder" );*/
			
			// Create the video encoder arcsoft
			CHK(CoCreateInstance(CLSID_ArcMpeg4Encoder, NULL,
							  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
							  (void **)&gcap.pVideoEncoder));

			// Everything succeeded, let's add the encoder to the graph
			CHK(gcap.pGraph->AddFilter( gcap.pVideoEncoder, L"Arcsoft MPEG4 encoder" ));

			// Load Arcsoft  multiplexer. 
			// To create a MPEG file, change the CLSID_ASFWriter into the GUID
			/*/ of your multiplexer
			hr = CoCreateInstance(CLSID_ArcMP4MuxRender, NULL,
						  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
						  (void **)&gcap.pAsfWriter);
			hr=gcap.pAsfWriter->QueryInterface( IID_IFileSinkFilter, (void**) &gcap.pFileSinkFilter );
			hr=gcap.pFileSinkFilter->SetFileName( L"\\test0.mp4", NULL );*/

		}
	if(bStill)
		{
			CHK(CoCreateInstance(CLSID_IMGSinkFilter, NULL,
							  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
							  (void **)&gcap.pRenderS));
			//Add filter to graph builder
			CHK( gcap.pGraph->AddFilter(gcap.pRenderS,L"Video Render2"));

		}


	return hr;
Cleanup:
	RETAILMSG(1, (TEXT("Return false at InitFilter") ) );
	return hr;
}

HRESULT CCameraAPI::InitPinVideoFormat(BOOL bCapture,BOOL bStill)
{
    HRESULT       hr = S_OK;
//add by zqs//preview pin
	    // Query the output pin for IAMStreamConfig (not shown).
	if(!gcap.pConfigP)
		CHK(gcap.pBuilder->FindInterface(
							&PIN_CATEGORY_PREVIEW, // Preview pin.
							0,    // Any media type.
							gcap.pCap, // Pointer to the capture filter.
							IID_IAMStreamConfig, (void**)&gcap.pConfigP));
	{
		int iCount=0,iSize=0;
		CHK(gcap.pConfigP->GetNumberOfCapabilities(&iCount,&iSize));
		if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
			//for(int iFORMAT =0;iFORMAT < iCount; iFORMAT++)
				{
					VIDEO_STREAM_CONFIG_CAPS scc;
					AM_MEDIA_TYPE *pmtConfig;
					CHK(gcap.pConfigP->GetStreamCaps(Resolution_Capture_QVGA, & pmtConfig,(BYTE*)&scc));//预览都是以VGA预览
					if(SUCCEEDED(hr))
						{
						VIDEOINFOHEADER*  vi = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
						CHK(gcap.pConfigP->SetFormat(pmtConfig));
						DeleteMediaType(pmtConfig);
						}
				}
	}

	//设置参数，包括亮度、对比度、色调、饱和度、白平衡
		{
			IAMVideoProcAmp 	*pProcAmp=0;
			CHK(gcap.pCap->QueryInterface(IID_IAMVideoProcAmp,(void**)&pProcAmp));
			CHK(pProcAmp->Set(VideoProcAmp_WhiteBalance,gcap.m_WhiteBanlance,VideoProcAmp_Flags_Manual));
			pProcAmp->Release();
		}
//capture
if(bCapture)
{
	    // Query the output pin for IAMStreamConfig (not shown).
	 if(!gcap.pConfigC)
		CHK( gcap.pBuilder->FindInterface(
							&PIN_CATEGORY_CAPTURE, // Preview pin.
							0,    // Any media type.
							gcap.pCap, // Pointer to the capture filter.
							IID_IAMStreamConfig, (void**)&gcap.pConfigC));
	 {
		int iCount=0,iSize=0;
		CHK(gcap.pConfigC->GetNumberOfCapabilities(&iCount,&iSize));
		if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
			//for(int iFORMAT =0;iFORMAT < iCount; iFORMAT++)
				{
					VIDEO_STREAM_CONFIG_CAPS scc;
					AM_MEDIA_TYPE *pmtConfig;
					CHK(gcap.pConfigC->GetStreamCaps(gcap.m_Resolution_Capture, & pmtConfig,(BYTE*)&scc));
					if(SUCCEEDED(hr))
						{
						VIDEOINFOHEADER*  vi = (VIDEOINFOHEADER*)pmtConfig->pbFormat;	
						CHK(gcap.pConfigC->SetFormat(pmtConfig));
						DeleteMediaType(pmtConfig);
						}
				}
	}
	 
	{
		IAMCameraControl 	*pCameraControl=0;
		CHK(gcap.pCap->QueryInterface(IID_IAMCameraControl,(void**)&pCameraControl));
		CHK(pCameraControl->Set(CameraControl_Flash,Flash_Off,VideoProcAmp_Flags_Manual));//视频时闪光灯永远都是关的
		pCameraControl->Release();
	}

	if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
	{
		//set media type for audio capture
		AM_MEDIA_TYPE*	mt;
		WAVEFORMATEX*	waveForm;
		if(!gcap.pConfigA)
			CHK(gcap.pBuilder->FindInterface(
									&PIN_CATEGORY_CAPTURE, // Preview pin.
									0,    // Any media type.
									gcap.pAudioCaptureFilter, // Pointer to the capture filter.
									IID_IAMStreamConfig, 
									(void**)&gcap.pConfigA));
		{
		CHK(gcap.pConfigA->GetFormat(&mt));
		waveForm = ((WAVEFORMATEX*)mt->pbFormat); 	
		waveForm->wFormatTag = WAVE_FORMAT_PCM;
		waveForm->nChannels = 1;
		waveForm->nSamplesPerSec = 8000; //Arcsoft amr-nb encoder only support 8000 sample rate.
		waveForm->wBitsPerSample = 16;
		waveForm->nBlockAlign = waveForm->nChannels * waveForm->wBitsPerSample / 8;
		waveForm->nAvgBytesPerSec = waveForm->nSamplesPerSec * waveForm->nBlockAlign;

		CHK(gcap.pConfigA->SetFormat(mt));
		DeleteMediaType(mt);
		}
	}
}
		
// still pin
if(bStill)
{
	if(!gcap.pConfigS)                      //设置分辨率
		CHK( gcap.pBuilder->FindInterface(
							&PIN_CATEGORY_STILL, // still pin.
							0,    // Any media type.
							gcap.pCap, // Pointer to the capture filter.
							IID_IAMStreamConfig, (void**)&gcap.pConfigS));
	{
		int iCount=0,iSize=0;
		CHK(gcap.pConfigS->GetNumberOfCapabilities(&iCount,&iSize));
		if(iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
			for(int iFORMAT =0;iFORMAT < iCount; iFORMAT++)
				{
					VIDEO_STREAM_CONFIG_CAPS scc;
					AM_MEDIA_TYPE *pmtConfig;
					if(bCaptureWithFrame)
						{CHK(gcap.pConfigS->GetStreamCaps(Resolution_Still_VGA, & pmtConfig,(BYTE*)&scc));}//带相框拍照就永远用分辨率640*480
					else							
						{CHK(gcap.pConfigS->GetStreamCaps(gcap.m_Resolution_Still, & pmtConfig,(BYTE*)&scc));}
					if(SUCCEEDED(hr))
						{
						VIDEOINFOHEADER *pVih=(VIDEOINFOHEADER*)pmtConfig->pbFormat;
						VIDEOINFOHEADER*  vi = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
						CHK(gcap.pConfigS->SetFormat(pmtConfig));
						DeleteMediaType(pmtConfig);
						}
				}
	}

	//设置闪光灯
	{
		IAMCameraControl 	*pCameraControl=0;
		CHK(gcap.pCap->QueryInterface(IID_IAMCameraControl,(void**)&pCameraControl));
		CHK(pCameraControl->Set(CameraControl_Flash,gcap.m_Flash_State,VideoProcAmp_Flags_Manual));
		pCameraControl->Release();
	}

}
//end by zqs
	return hr;
Cleanup:
	RETAILMSG(1, (TEXT("Return false at InitPinVideoFormat") ) );
	return hr;
}

HRESULT CCameraAPI::OpenCamera(PSHCAMERACAPTURE pshcc)
{
	HRESULT hr;
	IBaseFilter *pMux = NULL;
	bool	bCapture=FALSE,bStill=FALSE,bPreview=FALSE;
	//IPin *pPin;
	size_t cch;
	TCHAR pFormat[3];

	{
		//写入注册表，表示没有PIMG
		unsigned int lpData;
		HKEY hkCamera = NULL; //zqs
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
		{
			lpData=0;
			RegSetValueEx(hkCamera, TEXT("EnterPIMG"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			RegCloseKey(hkCamera);
		}
	}

	StringCchLength(pshcc->szFile,MAX_PATH,&cch);
	StringCchCopy(pFormat,4,pshcc->szFile+cch-3);
	if(_tcsicmp(pFormat, _T("3gp")) == 0)
		m_nFileFormat=FILE_FORMAT_3GP;
	if(_tcsicmp(pFormat, _T("mp4")) == 0)
		m_nFileFormat=FILE_FORMAT_MP4;
	
	//StringCchPrintf(pshcc->szFile,MAX_PATH,TEXT("%s%s"),pshcc->pszInitialDir,pshcc->pszDefaultFileName);

	if(pshcc_last== NULL)//表示第一次进去
	{
		pshcc_last=(PSHCAMERACAPTURE)malloc(sizeof(SHCAMERACAPTURE));
		memset(pshcc_last, 0xff, sizeof(SHCAMERACAPTURE));
		//第一次必须设置分辨率
		if(pshcc->Mode== CAMERACAPTURE_MODE_VIDEOONLY|| pshcc->Mode== CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
			if(pshcc->nResolutionWidth == 320 && pshcc->nResolutionHeight==240)
				gcap.m_Resolution_Capture=Resolution_Capture_QVGA;
			else if(pshcc->nResolutionWidth == 176 && pshcc->nResolutionHeight==144)
				gcap.m_Resolution_Capture=Resolution_Capture_QCIF;
		}

		if(pshcc->Mode== CAMERACAPTURE_MODE_STILL)
		{
			if(pshcc->nResolutionWidth == 320 && pshcc->nResolutionHeight==240)
				gcap.m_Resolution_Still=Resolution_Still_QVGA;
			else if(pshcc->nResolutionWidth == 1280 && pshcc->nResolutionHeight==1024)
				gcap.m_Resolution_Still=Resolution_Still_SXGA;
			else if(pshcc->nResolutionWidth == 640 && pshcc->nResolutionHeight==512)
				gcap.m_Resolution_Still=Resolution_Still_QSXGA;
			else if(pshcc->nResolutionWidth == 640 && pshcc->nResolutionHeight==480)
				gcap.m_Resolution_Still=Resolution_Still_VGA;
		}
	}

	//if(pshcc->Mode != pshcc_last->Mode)
		{
		bPreview=TRUE;
		if(pshcc->Mode== CAMERACAPTURE_MODE_STILL)
			bStill=TRUE;
		else if(pshcc->Mode== CAMERACAPTURE_MODE_VIDEOONLY|| pshcc->Mode== CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
			bCapture=TRUE;
		}
	//else	
		{
		if((pshcc_last->nResolutionWidth == pshcc->nResolutionWidth) && (pshcc_last->nResolutionHeight == pshcc->nResolutionHeight))//分辨率也相同就返回
			{
			if(pshcc->Mode == pshcc_last->Mode && bCaptureWithFrame == bCaptureWithFrame_Last)
				{
				if(bCapture)
					CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, 0, 0 ,0,0 ));//video
				if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
					CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE,&MEDIATYPE_Audio, gcap.pAudioCaptureFilter,0, 0, 0, 0 ));//audio
				return	TRUE;//如果两次分辨率一样，而且模式一样就返回
				}
			}
		}
	memcpy(pshcc_last, pshcc, sizeof(SHCAMERACAPTURE));
	bCaptureWithFrame_Last=bCaptureWithFrame;
	RotateHandle(FALSE, DMDO_0);
	CHK(UninitGraph());
	CHK(InitGraph(bCapture,bStill));

	CHK(InitPinVideoFormat(bCapture,bStill));

	if (bStill)
	{
		ConnectFilters(gcap.pGraph,gcap.pCap,1,gcap.pRenderS,0,NULL); // 1 is still pin
		//CHK( gcap.pBuilder->RenderStream(&PIN_CATEGORY_STILL,&MEDIATYPE_Video, gcap.pCap, NULL, gcap.pRenderS));
		CHK(gcap.pRenderS->QueryInterface( &gcap.m_pImageSinkFilter));
	}

	//Start capture
	if (bCapture)  //capture to file
	{
		//add by zqs
		DuringInCapture=NotInCapture;
		//
		// Create the ASF multiplexer and add it to the graph
		//
		//hr=gcap.pBuilder->SetOutputFileName( &MEDIASUBTYPE_Asf, L"\\video1.asf", &gcap.pAsfWriter, &gcap.pFileSinkFilter );
		CHK(gcap.pBuilder->SetOutputFileName( &CLSID_ArcMP4MuxRender, pshcc->szFile, &gcap.pAsfWriter, &gcap.pFileSinkFilter ));
		//
		// Connect the video capture filter, the encoder and the multiplexer together
		// 
		//CHK(gcap.pBuilder->RenderStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, gcap.pVideoEncoder, gcap.pAsfWriter ));
		ConnectFilters(gcap.pGraph,gcap.pCap,0,gcap.pVideoEncoder,0,NULL); // 0is capture pin
		ConnectFilters(gcap.pGraph, gcap.pVideoEncoder,gcap.pAsfWriter); // 0 is capture pin
		CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, 0, 0 ,0,0 ));//video

		if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
			ConnectFilters(gcap.pGraph, gcap.pAudioCaptureFilter,gcap.pAudioEncoder); // 0 is capture pin
			ConnectFilters(gcap.pGraph, gcap.pAudioEncoder,gcap.pAsfWriter); // 0 is capture pin
			//CHK(gcap.pBuilder->RenderStream( &PIN_CATEGORY_CAPTURE,&MEDIATYPE_Audio, gcap.pAudioCaptureFilter,gcap.pAudioEncoder, gcap.pAsfWriter ));
			CHK(gcap.pBuilder->ControlStream( &PIN_CATEGORY_CAPTURE,&MEDIATYPE_Audio, gcap.pAudioCaptureFilter,0, 0, 0, 0 ));//audio
		}
	}

	if (bPreview)
	{	
		//ConnectFilters(gcap.pGraph,gcap.pCap,2,gcap.pRenderP,0,NULL); // 2 is preview pin
		CHK( gcap.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video, gcap.pCap, NULL, gcap.pRenderP));			
		CHK(gcap.pRenderP->QueryInterface(IID_IVideoWindow, (void**)&gcap.pVWP));
		//CHK( gcap.pRenderP->QueryInterface(IID_IDirectDrawVideo, (void **)&gcap.pDdrawVideoP));
		SetupVideoWindow(gcap.pVWP);
	}

		// Query for video interfaces, which may not be relevant for audio files
		CHK(gcap.pGraph->QueryInterface(IID_IMediaEventEx, (void **)&gcap.pME));        
		//CHK( gcap.pCap->QueryInterface(IID_IAMCameraControl,(void **)&gcap.pCamControl));
		//CHK(gcap.pCap->QueryInterface(IID_IAMVideoProcAmp,(void **)&gcap.pVProcAmp));

		// Have the graph signal event via window callbacks for performance
		CHK( gcap.pME->SetNotifyWindow((OAHWND)hWndApp, WM_GRAPHNOTIFY, 0));    

		CHK( gcap.pME->CancelDefaultHandling(EC_COMPLETE));


	return hr;

Cleanup:
	RETAILMSG(1, (TEXT("Return false at OpenCamera") ) );
	return hr;
}


HRESULT CCameraAPI::ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pF1, int iPin1,IBaseFilter *pF2,int iPin2,IPin **ppPinout)
{
	IPin *pPin1, *pPin2;
	IEnumPins    *pEnum;	
	unsigned long fetched;
	HRESULT hr;

	hr = pF1->EnumPins(&pEnum);
	while (iPin1>0)
	{	
		hr = pEnum->Next(1,&pPin1,&fetched); //Skip Capture  pin
		iPin1--;
	}
	hr = pEnum->Next(1,&pPin1,&fetched);

	hr = pF2->EnumPins(&pEnum);
	while (iPin2>0)
	{	
		hr = pEnum->Next(1,&pPin2,&fetched); //Skip Capture  pin
		iPin2--;
	}
	hr = pEnum->Next(1,&pPin2,&fetched);

	hr = pGraph->Connect(pPin1,pPin2);
	if (ppPinout)
	{
		*ppPinout = pPin1;
	}
	if (!SUCCEEDED(hr))
		RETAILMSG(1, (TEXT("CamTest: Fail to Connect Pin! %x\r\n"),hr));

	return hr;
}

HRESULT CCameraAPI::InitCapFilter()
{
	HRESULT hr;
	IPersistPropertyBag *pPropertyBag = NULL;
	GUID clsid = DEVCLASS_CAMERA_GUID;
	VARIANT var;	
	CPropertyBag PropBag;
	// Create Capture Filter

	DEVMGR_DEVICE_INFORMATION ddiDevInfo;
	memset(&ddiDevInfo,0,sizeof(ddiDevInfo));
	ddiDevInfo.dwSize = sizeof(ddiDevInfo);

	// find the camera driver
	HANDLE hFirstDevice = FindFirstDevice(DeviceSearchByGuid,
	                                      &clsid,
	                                      &ddiDevInfo);
	if (!hFirstDevice || !ddiDevInfo.hDevice)
	{
	    RETAILMSG(1, (TEXT("CamTest: Find device NULL: %x %x\r\n"),hFirstDevice,ddiDevInfo.szDeviceName));
	    //CHK(hr = E_FAIL);
	}
	    RETAILMSG(1, (TEXT("CamTest: Find device: %x %x\r\n"),hFirstDevice,ddiDevInfo.szDeviceName));

	FindClose(hFirstDevice);
	

	CHK(CoCreateInstance(CLSID_VideoCapture, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter,(void **)&gcap.pCap));

	CHK(gcap.pCap->QueryInterface(IID_IPersistPropertyBag, (void **)&pPropertyBag));



	// set camera driver name property
	VariantInit (&var);
	var.vt = VT_BSTR;
	var.bstrVal = SysAllocString(ddiDevInfo.szLegacyName);
	if (!var.bstrVal)
	{
	    RETAILMSG(1, (TEXT("var.bstrVal is NULL") ) );
	}
	PropBag.Write(L"VCapName",&var);
	CHK(pPropertyBag->Load(&PropBag, NULL)) ;
	SysFreeString(var.bstrVal);

	pPropertyBag->Release();


	return S_OK;

Cleanup:
	RETAILMSG(1, (TEXT("Return false at InitCapFilter") ) );
	return hr;
}


HRESULT CCameraAPI::CaptureStillImage()
{
	IBaseFilter *pMux = NULL;
	//IFileSinkFilter *pSink = NULL;
	IMediaControl *pMC = NULL;
	IUnknown		 *pUnkCaptureFilter;
	IAMVideoControl *pVideoControl;
	IPin			 *pStillPin;
	DWORD Quality=0;
	//IFileSinkFilter *pFileSink;
//	unsigned char* x;	
	HRESULT hr;	

	ReadUserRegistryData(HKEY_CURRENT_USER,L"\\Software\\Microsoft\\Pictures\\Camera\\User",L"QualitySetting",(DWORD*)&Quality);
	if(ValidateMemory(Quality)== FALSE)
		return	FALSE;
	
	if(gcap.fCapturing)
		return FALSE;;
	gcap.fCapturing=TRUE;
	RETAILMSG(1, (TEXT("CaptureStillImage:begin to capture") ) );
	CHK(gcap.m_pImageSinkFilter->QueryInterface( &gcap.pSink));
	CHK(gcap.pSink->SetFileName(pshcc->szFile, NULL ));
	CHK(gcap.m_pImageSinkFilter->SetQuality(Quality));//设置压缩水平
	CHK(gcap.pCap->QueryInterface( &pUnkCaptureFilter ));
	CHK(gcap.pBuilder->FindPin( pUnkCaptureFilter, PINDIR_OUTPUT, &PIN_CATEGORY_STILL, &MEDIATYPE_Video, FALSE, 0, &pStillPin ));
	CHK(gcap.pCap->QueryInterface( &pVideoControl ));
	if(gcap.m_Still_Capture_Mode==StillCaptureMode_Timer_Capture)
		for(int i=0;i<5;i++)
			{
			PlaySound (TEXT("\\windows\\TimerSound.wav"), NULL, SND_SYNC);
			Sleep(1000);
			}
			 
	if(gcap.m_Still_Capture_Mode==StillCaptureMode_Sequence_Capture)
		{
		for(int j=0;j<5;j++)
			{
			RETAILMSG(1, (TEXT("CamTest: enter capure j=%d\r\n"),j));
			for(int i=0;i<5;i++)
				{
				PlaySound (TEXT("\\windows\\TimerSound.wav"), NULL, SND_SYNC);
				Sleep(1000);
				}
			gcap.fCapturing=true;
			CHK(pVideoControl->SetMode( pStillPin, VideoControlFlag_Trigger));
			PlaySound (TEXT("\\windows\\ShutterSound.wav"), NULL, SND_SYNC);
			RETAILMSG(1, (TEXT("CamTest: leave capure j=%d\r\n"),j));
			}
		}else
		{
			gcap.fCapturing=true;
			CHK(pVideoControl->SetMode( pStillPin, VideoControlFlag_Trigger));
			PlaySound (TEXT("\\windows\\ShutterSound.wav"), NULL, SND_SYNC);
			RETAILMSG(1, (TEXT("CaptureStillImage: capture \r\n")));
		}
		 pUnkCaptureFilter->Release();
		 gcap.pSink->Release();
		 pVideoControl->Release();		
	 //StopPreview();

	return true;

Cleanup:
	RETAILMSG(1, (TEXT("Return false at CaptureStillImage GetLastError=%x"),GetLastError()) );
	if(MessageBox(hWndApp,TEXT("初始化失败"),TEXT("警告"),MB_OK) == IDOK)
		PostMessage(hWndApp,WM_CLOSE,0,0);
	RETAILMSG(1, (TEXT("Return false at CaptureStillImage") ) );
	return hr;
}


HRESULT CCameraAPI::UninitGraph()
{
	HRESULT hr;	
	IEnumPins *pinEnum=0;
	IPin	*pin=0;

	CHK(StopPreview());
	if(gcap.pCap)
		{
		gcap.pCap->EnumPins(&pinEnum);
		while(pinEnum->Next(1,&pin,NULL) == S_OK)
			{
			if(gcap.pGraph)
				gcap.pGraph->Disconnect(pin);
			}
		}
	//SAFE_RELEASE(gcap.pCap);	
	

	if(gcap.pVWS)
	{
	   gcap.pVWS->put_Visible(OAFALSE);
	   gcap.pVWS->put_Owner(NULL);
	   //CHK(gcap.pVWS->Release());
	}

	if(gcap.pVWP)
	{
	   gcap.pVWP->put_Visible(OAFALSE);
	   gcap.pVWP->put_Owner(NULL);
	   //CHK( gcap.pVWP->Release());
	}
//add by zqs 成批删除 filter
	SAFE_RELEASE(gcap.pConfigP);
	SAFE_RELEASE(gcap.pVWP);
	SAFE_RELEASE(gcap.pAMVidControl);
	SAFE_RELEASE(gcap.pCamControl);
	SAFE_RELEASE(gcap.pVProcAmp);
	SAFE_RELEASE(gcap.pConfigP);
	SAFE_RELEASE(gcap.pConfigC);
	SAFE_RELEASE(gcap.pConfigA);
	SAFE_RELEASE(gcap.pConfigS);
	SAFE_RELEASE(gcap.pVideoEncoder);
	SAFE_RELEASE(gcap.pWrapperFilter);
	SAFE_RELEASE(gcap.pFileSinkFilter);
	SAFE_RELEASE(gcap.pAudioCaptureFilter);
	SAFE_RELEASE(gcap.pRenderS);
	SAFE_RELEASE(gcap.pRenderP);
	SAFE_RELEASE(gcap.pAsfWriter);
	SAFE_RELEASE(gcap.pAudioEncoder);
	SAFE_RELEASE(gcap.pDdrawVideoP);	
	SAFE_RELEASE(gcap.pSink);	
	SAFE_RELEASE(gcap.m_pImageSinkFilter);	
	SAFE_RELEASE(gcap.pME);	
	
//end by zqs
	SAFE_RELEASE(gcap.pGraph);
	SAFE_RELEASE(gcap.pBuilder); 
	
	return hr;

Cleanup:
	RETAILMSG(1, (TEXT("Return false at CloseCamera") ) );
	return hr;
}


// Start previewing
//
BOOL CCameraAPI::StartPreview()
{
	HRESULT hr;
	

		// way ahead of you
	if(gcap.fPreviewing)
	    return TRUE;

	{
		//如果是带相框拍照，则写入注册表
		unsigned int lpData;
		HKEY hkCamera = NULL; //zqs
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
		{
			if(bCaptureWithFrame)
				lpData=1;
			else
				lpData=0;
			RegSetValueEx(hkCamera, TEXT("EnableCaptureWithFrame"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			lpData=1;
			RegSetValueEx(hkCamera, TEXT("ModelChanged"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			lpData=gcap.m_Theme_Frame;
			RegSetValueEx(hkCamera, TEXT("Theme_Frame"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			if(pshcc->Mode==CAMERACAPTURE_MODE_STILL)
				lpData=1;
			else
				lpData=0;
			RegSetValueEx(hkCamera, TEXT("Still_Mode"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			RegCloseKey(hkCamera);
		}
	}
	if(!gcap.pGraph)
		return true;	
	RotateHandle(FALSE, DMDO_0);
	if(gcap.pVWP)
		{
		RETAILMSG(1, (TEXT("enter StartPreview!!!!!!!")) );
		SetWindowPos(hWndApp,HWND_TOPMOST,0,0,240,320,SWP_SHOWWINDOW);
		//gcap.pVWS->put_WindowState(SW_HIDE);	
		//ShowWindow(hWndApp,SW_SHOW);
		gcap.pVWP->put_WindowState(SW_SHOW);
		//SetWindowPos(hWndApp,HWND_TOPMOST,0,0,240,320,SWP_SHOWWINDOW);
		SetForegroundWindow((HWND) (((ULONG) hWndApp) | 0x01));
		//(hWndApp);
		//SetActiveWindow(hWndApp);
		RETAILMSG(1, (TEXT("before setcapture and set focus GetCapture() =x%x  GetFocus()=0x%x GetForegroundWindow()=0x%x!!!!!!!"),GetCapture(),GetFocus(),GetForegroundWindow()) );
		SetCapture(hWndApp);
		SetFocus(hWndApp) ;	
		//BringWindowToTop(hWndApp);
		GetWindowRect(hWndApp, &hWndrect);  //获得主窗口原始位置
		//SHFullScreen(hWndApp, SHFS_HIDESTARTICON );
		SHSetAppKeyWndAssoc(ID_Capture_Key,hWndApp);
		//SHSetAppKeyWndAssoc(0xc1,hWndApp);
		RETAILMSG(1, (TEXT("befoe MoveWindow left=%d top=%d right=%d bottom=%d"),hWndrect.left, hWndrect.top,hWndrect.right,hWndrect.bottom) );
		//MoveWindow(hWndApp,0,0,240,320,TRUE);
		//RETAILMSG(1, (TEXT("after MoveWindow left=%d top=%d right=%d bottom=%d"),rect.left, rect.top,rect.right,rect.bottom) );
		}
	// run the graph
	IMediaControl *pMC = NULL;

	CHK( gcap.pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC));
	CHK( pMC->Run());
	CHK(pMC->Release());

	nSystemTimer=1;
	SetTimer(hWndApp, nSystemTimer, 30000, TimerProc );

	gcap.fPreviewing = TRUE;


	bInStartMenu=false;
	return TRUE;
Cleanup:
	RETAILMSG(1, (TEXT("Return false at StartPreview") ) );
	CHK(pMC->Stop());
	return hr;
}

// stop the preview graph
//
BOOL CCameraAPI::StopPreview()
{
	HRESULT hr;
		
    // way ahead of you
	if(!gcap.fPreviewing)
	{
	    return FALSE;
	}

	IMediaControl *pMC = NULL;
	CHK( gcap.pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC));
	CHK( pMC->Stop());
	CHK(pMC->Release());

	KillTimer(hWndApp,nSystemTimer);
	//SetWindowPos(hWndApp,HWND_NOTOPMOST,0,0,240,320,SWP_SHOWWINDOW);
	if(gcap.pVWP)
		gcap.pVWP->put_WindowState(SW_HIDE);
	//SHFullScreen(hWndApp,  SHFS_SHOWSTARTICON );
	//GetWindowRect(hWndApp, &hWndrect);
	//RETAILMSG(1, (TEXT("befoe MoveWindow left=%d top=%d right=%d bottom=%d"),hWndrect.left, hWndrect.top,hWndrect.right,hWndrect.bottom) );
	//MoveWindow(hWndApp,0,0,240,320,TRUE);
	//GetWindowRect(hWndApp, &hWndrect);
	//RETAILMSG(1, (TEXT("after MoveWindow left=%d top=%d right=%d bottom=%d"),hWndrect.left, hWndrect.top,hWndrect.right,hWndrect.bottom) );
	//ReleaseCapture();
	//SetFocus(hWndApp);
	RETAILMSG(1, (TEXT("before SHSetAppKeyWndAssoc") ) );
	SHSetAppKeyWndAssoc(ID_Capture_Key,NULL);
	SHSetAppKeyWndAssoc(0xc1,NULL);	// stop the graph
	gcap.fPreviewing = FALSE;

	return TRUE;
	
Cleanup:
	RETAILMSG(1, (TEXT("Return false at StopPreview") ) );
	return hr;
}

BOOL CCameraAPI::ChangeDlgItem(HWND hwndDlg)
{
	//HRESULT	hr;
	if(pshcc->Mode==CAMERACAPTURE_MODE_STILL && !bCaptureWithFrame)
		{
		SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("拍照"));
		
		//保存格式
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("JPG"));
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("JPG"));

		
		//分辨率
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("1280*1024"));
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("640*512"));
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("640*480"));
		if(gcap.m_Resolution_Still==Resolution_Still_SXGA)
			SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("1280*1024"));
		else if(gcap.m_Resolution_Still==Resolution_Still_QSXGA)
			SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("640*512"));
		else if(gcap.m_Resolution_Still==Resolution_Still_VGA)
			SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("640*480"));
		
	
		//拍照方式
		ShowWindow(GetDlgItem(hwndDlg,IDC_STILL_CAPTURE_MODE), SW_SHOW);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STATIC_CAPTURE), SW_SHOW);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STILL_CAPTURE_THEME), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STATIC_THEME), SW_HIDE);
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("普通拍照"));
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("定时5秒拍照"));
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("连拍5张"));
		if(gcap.m_Still_Capture_Mode == StillCaptureMode_Normal_capture)
			SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("普通拍照"));
		else if(gcap.m_Still_Capture_Mode == StillCaptureMode_Timer_Capture)
			SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("定时5秒拍照"));
		else if(gcap.m_Still_Capture_Mode == StillCaptureMode_Sequence_Capture)
			SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("连拍5张"));
		
		//闪光灯
		ShowWindow(GetDlgItem(hwndDlg,IDC_CHECK_FLASH), SW_SHOW);
		ShowWindow(GetDlgItem(hwndDlg,IDC_CHECK_AUDIO), SW_HIDE);
		if(gcap.m_Flash_State==Flash_On)
			SendDlgItemMessage(hwndDlg, IDC_CHECK_FLASH, BM_SETCHECK , BST_CHECKED, 0);
		else
			SendDlgItemMessage(hwndDlg, IDC_CHECK_FLASH, BM_SETCHECK , BST_UNCHECKED, 0);
		}else if(pshcc->Mode==CAMERACAPTURE_MODE_STILL  && bCaptureWithFrame)
	{
		SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("相框拍照"));
		
		//保存格式
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("JPG"));
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("JPG"));

		
		//分辨率
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("640*480"));
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("640*480"));

		//场景选择

		ShowWindow(GetDlgItem(hwndDlg,IDC_STILL_CAPTURE_THEME), SW_SHOW);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STATIC_THEME), SW_SHOW);
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("LOVE"));
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("TREE"));
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("ZGB"));
		if(gcap.m_Theme_Frame == 0)
			SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("LOVE"));
		else if(gcap.m_Theme_Frame == 1)
			SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("TREE"));
		else if(gcap.m_Theme_Frame == 2)
			SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_THEME, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("ZGB"));

		//拍照方式
		gcap.m_Still_Capture_Mode = StillCaptureMode_Normal_capture;
		ShowWindow(GetDlgItem(hwndDlg,IDC_STILL_CAPTURE_MODE), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STATIC_CAPTURE), SW_HIDE);

		//闪光灯
		ShowWindow(GetDlgItem(hwndDlg,IDC_CHECK_FLASH), SW_SHOW);
		ShowWindow(GetDlgItem(hwndDlg,IDC_CHECK_AUDIO), SW_HIDE);
		if(gcap.m_Flash_State==Flash_On)
			SendDlgItemMessage(hwndDlg, IDC_CHECK_FLASH, BM_SETCHECK , BST_CHECKED, 0);
		else
			SendDlgItemMessage(hwndDlg, IDC_CHECK_FLASH, BM_SETCHECK , BST_UNCHECKED, 0);
		}else
		{
		SendDlgItemMessage(hwndDlg, IDC_VIDEOMODE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("摄像"));
		
		//保存格式
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("mp4"));
		SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("3gp"));
		if(m_nFileFormat==FILE_FORMAT_MP4)
			SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("mp4"));
		else if(m_nFileFormat==FILE_FORMAT_3GP)
			SendDlgItemMessage(hwndDlg, IDC_VIDEOFORMAT, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("3gp"));

		
		//分辨率
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("320*240"));
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("176*144"));
		SendDlgItemMessage(hwndDlg, IDC_RESOLUTION, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("320*240"));
	
		//拍照方式
		SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, CB_RESETCONTENT , 0, 0);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STILL_CAPTURE_MODE), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STATIC_CAPTURE), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STILL_CAPTURE_THEME), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg,IDC_STATIC_THEME), SW_HIDE);
		//hr=SendDlgItemMessage(hwndDlg, IDC_STILL_CAPTURE_MODE, WM_SHOWWINDOW, FALSE, 0);
		
		//音频
		ShowWindow(GetDlgItem(hwndDlg,IDC_CHECK_FLASH), SW_HIDE);
		ShowWindow(GetDlgItem(hwndDlg,IDC_CHECK_AUDIO), SW_SHOW);
		if(pshcc->Mode==CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
			SendDlgItemMessage(hwndDlg, IDC_CHECK_AUDIO, BM_SETCHECK , BST_CHECKED, 0);
		else
			SendDlgItemMessage(hwndDlg, IDC_CHECK_AUDIO, BM_SETCHECK , BST_UNCHECKED, 0);
		}

		//白平衡
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_RESETCONTENT , 0, 0);
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("自动"));
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("夜间"));
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("白天"));
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("多云"));
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("荧光"));
		SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_ADDSTRING , 0, (LPARAM)(LPCSTR)_T("白炽光"));
		switch(gcap.m_WhiteBanlance)
			{
			case	WhiteBanlence_Auto:
				SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("自动"));
				break;
			case	WhiteBanlence_Night:
				SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("夜间"));
				break;
			case	WhiteBanlence_Day:
				SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("白天"));
				break;
			case	WhiteBanlence_Cloudy:
				SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("多云"));
				break;
			case	WhiteBanlence_Fluor:
				SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("荧光"));
				break;
			case	WhiteBanlence_Lamp:
				SendDlgItemMessage(hwndDlg, IDC_WHITEBALANCE, CB_SELECTSTRING  , 0, (LPARAM)(LPCSTR)_T("白炽光"));
				break;
		}
		
		return TRUE;
}
bool	CCameraAPI::Next_Theme_Frame()
{
	if(!bCaptureWithFrame)
		return	FALSE;
	if(gcap.m_Theme_Frame<Theme_Frame_MAX-1)
		gcap.m_Theme_Frame++;
	else
		gcap.m_Theme_Frame=0;
	{
		//改变场景并写入注册表
		unsigned int lpData;
		HKEY hkCamera = NULL; //zqs
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
		{
			lpData=1;
			RegSetValueEx(hkCamera, TEXT("ModelChanged"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			lpData=gcap.m_Theme_Frame;
			RegSetValueEx(hkCamera, TEXT("Theme_Frame"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			RegCloseKey(hkCamera);
		}
	}
	return	TRUE;
}

bool	CCameraAPI::Previous_Theme_Frame()
{
	if(!bCaptureWithFrame)
		return	FALSE;
	if(gcap.m_Theme_Frame>0)
		gcap.m_Theme_Frame--;
	else
		gcap.m_Theme_Frame=Theme_Frame_MAX-1;
	{
		//改变场景并写入注册表
		unsigned int lpData;
		HKEY hkCamera = NULL; //zqs
		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
		{
			lpData=1;
			RegSetValueEx(hkCamera, TEXT("ModelChanged"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			lpData=gcap.m_Theme_Frame;
			RegSetValueEx(hkCamera, TEXT("Theme_Frame"), 0, REG_DWORD, (BYTE*)&lpData, sizeof(unsigned int));
			RegCloseKey(hkCamera);
		}
	}
	return TRUE;
}



bool	CCameraAPI::RotateHandle(bool	GetCurrentAngle,int NewAngle)
{
   DEVMODE DevMode;

   int RotationAngles;
   //int NewAngle;

   //
   // Check for rotation support by getting the rotation angles supported.
   //

   memset (&DevMode, 0, sizeof (DevMode));
   DevMode.dmSize   = sizeof (DevMode);
   DevMode.dmFields = DM_DISPLAYQUERYORIENTATION;

   if (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsEx(NULL, &DevMode, NULL, CDS_TEST, NULL))
   {
      RotationAngles = DevMode.dmDisplayOrientation;
      RETAILMSG(1, (L"ChangeDisplaySettingsEx supports these rotation angles %d", RotationAngles));
   }
   else
   {
      RETAILMSG(1, (L"ChangeDisplaySettingsEx failed to get the supported rotation angles."));
      RotationAngles = -1;
   }

   //
   // Get the current rotation angle.
   //
	if(GetCurrentAngle)
	{
		memset(&DevMode, 0, sizeof (DevMode));
		DevMode.dmSize   = sizeof (DevMode);
		DevMode.dmFields = DM_DISPLAYORIENTATION;

		if (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsEx(NULL, &DevMode, NULL, CDS_TEST, NULL))
		{
		  CurrentAngle = DevMode.dmDisplayOrientation;
		  RETAILMSG(1, (L"ChangeDisplaySettingsEx reports the current rotation as %d", CurrentAngle));
		}
		else
		{ 
		  RETAILMSG(1, (L"ChangeDisplaySettingsEx failed to get the current rotation angle."));
		  CurrentAngle = -1;
		}
	}

      memset(&DevMode, 0, sizeof (DevMode));
      DevMode.dmSize               = sizeof (DevMode);
      DevMode.dmFields             = DM_DISPLAYORIENTATION;
      DevMode.dmDisplayOrientation = NewAngle;

      if (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsEx(NULL, &DevMode, NULL, CDS_RESET, NULL))
      {
         RETAILMSG(1, (L"ChangeDisplaySettingsEx changed rotation angle to %d", NewAngle));
      }
      else
      {
         RETAILMSG(1, (L"ChangeDisplaySettingsEx failed to change the rotation angle to %d", NewAngle));
      }
  // }

   return 0;
}

void CALLBACK CCameraAPI::TimerProc(HWND hwnd,  UINT uMsg,  UINT idEvent,   DWORD dwTime )
{
	CCameraAPI *pThis;
	pThis = (CCameraAPI*) dwCameraAPIContext;
	if(pThis->VideoTime<30000 && pThis->DuringInCapture==InCapture)
		pThis->StopCaptureVideoInternal(TRUE);
	else if(pThis->DuringInCapture==InCapture)
		pThis->VideoTime=pThis->VideoTime-30000;
	if(pThis->DuringInCapture==InCapture)
		{
		SetSystemPowerState(NULL,POWER_STATE_ON,POWER_FORCE);
		//SystemIdleTimerReset();
		//SHIdleTimerReset();		
		}
}

void CCameraAPI::SetResolution(int Resolution,CAMERACAPTURE_MODE mode)
{
	if(mode == CAMERACAPTURE_MODE_VIDEOONLY || mode == CAMERACAPTURE_MODE_VIDEOWITHAUDIO)
		{
		if(Resolution==Resolution_Capture_QVGA)
			{
			pshcc->nResolutionWidth=320;
			pshcc->nResolutionHeight=240;
			}
		if(Resolution==Resolution_Capture_QCIF)
			{
			pshcc->nResolutionWidth=176;
			pshcc->nResolutionHeight=144;
			}
		}
	if(mode == CAMERACAPTURE_MODE_STILL)
		{
		if(Resolution==Resolution_Still_QVGA)
			{
			pshcc->nResolutionWidth=320;
			pshcc->nResolutionHeight=240;
			}
		if(Resolution==Resolution_Still_SXGA)
			{
			pshcc->nResolutionWidth=1280;
			pshcc->nResolutionHeight=1024;
			}
		if(Resolution==Resolution_Still_QSXGA)
			{
			pshcc->nResolutionWidth=640;
			pshcc->nResolutionHeight=512;
			}
		if(Resolution==Resolution_Still_VGA)
			{
			pshcc->nResolutionWidth=640;
			pshcc->nResolutionHeight=480;
			}
		}
}



void CCameraAPI::SetFileFormat(int Format)
{
	size_t cch;

	StringCchLength(pshcc->szFile,MAX_PATH,&cch);
	if(Format ==FILE_FORMAT_3GP)
		StringCchCopy(pshcc->szFile+cch-3,4,TEXT("3gp"));		//修改成3gp
	if(Format ==FILE_FORMAT_MP4)
		StringCchCopy(pshcc->szFile+cch-3,4,TEXT("mp4"));		//修改成3gp
	if(Format ==FILE_FORMAT_JPG)
		StringCchCopy(pshcc->szFile+cch-3,4,TEXT("jpg"));		//修改成3gp
}

bool CCameraAPI::ValidateMemory(DWORD Quality)
{
	STORE_INFORMATION StoreInfo;
	DWORD	FileSize;

	GetStoreInformation(&StoreInfo);
	if(pshcc->Mode == CAMERACAPTURE_MODE_STILL)//静态拍照
	{

	
		{
		DWORD	OptionNum;
		ReadUserRegistryData(HKEY_LOCAL_MACHINE,L"\\Software\\Microsoft\\Pictures\\Camera\\OEM\\PictureResolution",L"OptionNum",&OptionNum);
		 for(unsigned int i=1;i<=OptionNum;i++)
		    	{
			TCHAR SubReg[MAX_PATH];
			DWORD	Width,Height;
			StringCchPrintf(SubReg,MAX_PATH,TEXT("\\Software\\Microsoft\\Pictures\\Camera\\OEM\\PictureResolution\\%d"),i);
			ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"Width",&Width);
			ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"Height",&Height);
			 if(pshcc->nResolutionWidth == Width && pshcc->nResolutionHeight == Height)
			 	{
				HKEY hkCamera = NULL; //zqs
				if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
				{
					RegSetValueEx(hkCamera, TEXT("Resolution"), 0, REG_DWORD, (BYTE*)&i, sizeof(unsigned int));
					RegCloseKey(hkCamera);
				}
				switch(Quality)
					{
					case	0:
						ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"LowQualityFileSize",&FileSize);
						break;
					case	1:
						ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"NormalQualityFileSize",&FileSize);
						break;
					case	2:
						ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"HighQualityFileSize",&FileSize);
						break;
					}
			 	break;
			 	}
			}

		}
		if(gcap.m_Still_Capture_Mode==StillCaptureMode_Sequence_Capture)
			{
			if(StoreInfo.dwFreeSize<FileSize*5)//RGB565格式 连拍要乘5
				{
				bOutOfMemory=TRUE;
				PostMessage(hWndApp,WM_CLOSE,0,0);
				SetEvent(hEvent[2]);
				return	FALSE;
				}		
			}
		if(StoreInfo.dwFreeSize<FileSize)//RGB565格式 
			{
			bOutOfMemory=TRUE;
			PostMessage(hWndApp,WM_CLOSE,0,0);
			SetEvent(hEvent[2]);
			return	FALSE;
			}
	}else{
			DWORD	OptionNum;
			ReadUserRegistryData(HKEY_LOCAL_MACHINE,L"\\Software\\Microsoft\\Pictures\\Camera\\OEM\\VideoProfile",L"OptionNum",&OptionNum);
			for(unsigned int i=1;i<=OptionNum;i++)
			{
				TCHAR SubReg[MAX_PATH];
				DWORD	Width,Height;
				StringCchPrintf(SubReg,MAX_PATH,TEXT("\\Software\\Microsoft\\Pictures\\Camera\\OEM\\VideoProfile\\%d"),i);
				ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"Width",&Width);
				ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"Height",&Height);
				if(pshcc->nResolutionWidth == Width && pshcc->nResolutionHeight == Height)
				{
				HKEY hkCamera = NULL; //zqs
				if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("\\Software\\Microsoft\\Pictures\\Camera\\User"), 0, NULL, REG_OPTION_NON_VOLATILE, 0, NULL,&hkCamera, NULL))
					{
					RegSetValueEx(hkCamera, TEXT("VideoProfile"), 0, REG_DWORD, (BYTE*)&i, sizeof(unsigned int));
					RegCloseKey(hkCamera);
					}
				if(pshcc->Mode == CAMERACAPTURE_MODE_VIDEOONLY)//仅视频
					ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"VideoOnlyFileSizePerSecond",&FileSize);
				else if(pshcc->Mode == CAMERACAPTURE_MODE_VIDEOWITHAUDIO)//带音频
					ReadUserRegistryData(HKEY_LOCAL_MACHINE,SubReg,L"VideoAudioFileSizePerSecond",&FileSize);
				VideoTime=StoreInfo.dwFreeSize/FileSize;//以s为单位
				if(VideoTime>=pshcc->nVideoTimeLimit && pshcc->nVideoTimeLimit !=0)
					VideoTime=pshcc->nVideoTimeLimit;
				VideoTime=VideoTime*1000;//化成以ms为单位
				RETAILMSG(1, (L"ValidateMemory: VideoTime=%d.",VideoTime));
				}
			}
		}
	return TRUE;
}
