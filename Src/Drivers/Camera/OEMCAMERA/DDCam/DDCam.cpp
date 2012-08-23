#include <windows.h>
#include <mmsystem.h>
#include <streams.h>
#include <cs.h>
#include <csmedia.h>
#include <camera.h>
#include <aygshell.h>
#include "ddcam.h"
#include "grabber.h"

#define MAX_LOADSTRING 100
#define WM_GRAPHNOTIFY  WM_APP + 1
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }

#define FOURCC_Y444     MAKEFOURCC('Y', '4', '4', '4')  // TIYUV: 1394 conferencing camera 4:4:4 mode 0
#define FOURCC_UYVY     MAKEFOURCC('U', 'Y', 'V', 'Y')  // MSYUV: 1394 conferencing camera 4:4:4 mode 1 and 3
#define FOURCC_Y411     MAKEFOURCC('Y', '4', '1', '1')  // TIYUV: 1394 conferencing camera 4:1:1 mode 2
#define FOURCC_Y800     MAKEFOURCC('Y', '8', '0', '0')  // TIYUV: 1394 conferencing camera 4:1:1 mode 5
#define FOURCC_YUV422   MAKEFOURCC('U', 'Y', 'V', 'Y')
#define FOURCC_YV16     MAKEFOURCC('Y', 'V', '1', '6')

//const GUID MEDIASUBTYPE_YV16 = {FOURCC_YV16, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71};
typedef struct
{ 	
	const unsigned short *text;
	const GUID *guid;
} MEDIA_SUBTYPE_LIST;
const MEDIA_SUBTYPE_LIST media_list[] = {
	{TEXT("YVU9"),&MEDIASUBTYPE_YVU9},
	{TEXT("Y411"),&MEDIASUBTYPE_Y411},
	{TEXT("Y41P"),&MEDIASUBTYPE_Y41P},
	{TEXT("YUY2"),&MEDIASUBTYPE_YUY2},
	{TEXT("YVYU"),&MEDIASUBTYPE_YVYU},
	{TEXT("UYVY"),&MEDIASUBTYPE_UYVY},
	{TEXT("Y211"),&MEDIASUBTYPE_Y211},
	{TEXT("YV12"),&MEDIASUBTYPE_YV12},
	{TEXT("YV16"),&MEDIASUBTYPE_YV16},
	{TEXT("Overlay"),&MEDIASUBTYPE_Overlay},
	{TEXT("RGB1"),&MEDIASUBTYPE_RGB1},
	{TEXT("RGB4"),&MEDIASUBTYPE_RGB4},
	{TEXT("RGB8"),&MEDIASUBTYPE_RGB8},
	{TEXT("RGB565"),&MEDIASUBTYPE_RGB565},
	{TEXT("RGB555"),&MEDIASUBTYPE_RGB555},
	{TEXT("RGB24"),&MEDIASUBTYPE_RGB24},
	{TEXT("RGB32"),&MEDIASUBTYPE_RGB32},
};

#define MLIST_SIZE (sizeof(media_list)/sizeof(media_list[0]))

#define StillWidth  640  //480
#define StillHeight 512  //640

unsigned long light;
unsigned long gain;


struct _capstuff
{
    TCHAR szCaptureFile[_MAX_PATH];
    WORD wCapFileSize;  
	ICaptureGraphBuilder2 *pBuilder;
	IVideoWindow *pVWS, *pVWP;  
    IMediaEventEx *pME;
    IAMDroppedFrames *pDF;
    IAMVideoCompression *pVC;
	IAMVideoControl *pAMVidControl;
	IAMCameraControl *pCamControl;
	IAMVideoProcAmp  *pVProcAmp;
	IAMStreamConfig  *pConfigP; //Preview config
//  IAMVfwCaptureDialogs *pDlg;  
    IAMStreamConfig *pVSC;      // for video cap
    IBaseFilter *pRenderS;		//Still render
	IBaseFilter *pRenderP;		//Preview render
    IBaseFilter *pCap;
	IBasicVideo *pBV; //basic video for still video render
    IGraphBuilder *pGraph;
	CSampleGrabber *pGrab;
	IDirectDrawVideo *pDdrawVideoP;   //Preview directdraw video
	IDirectDrawVideo *pDdrawVideoS;   //Still directdraw video
    IFileSinkFilter *pSink;    
	
    BOOL fStillGraphBuilt;
    BOOL fPreviewGraphBuilt;
    BOOL fStillCapturing;
    BOOL fPreviewing;
    double FrameRate;
    BOOL fWantPreview;
} gcap;

int exposure = 3;
int picture_number=0;
// Global Variables:
HINSTANCE			hInst;			// The current instance
HWND				hWndApp;			//The window instance
HWND                hWndMenu = NULL;             //Menu handle
HWND				hWndMenuStill = NULL;
HMENU               hMenu;

REFERENCE_TIME rtStart = 10000000, rtStop = 50000000;
const WORD wStartCookie = 1, wStopCookie = 2;  // Arbitrary values.

const TCHAR *picture_path = TEXT("\\Temp") ;
//const TCHAR *picture_path = TEXT("\\Storage Card") ;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass	(HINSTANCE, LPTSTR);
BOOL				InitInstance	(HINSTANCE, int);
LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About			(HWND, UINT, WPARAM, LPARAM);
BOOL ConvertGuidToString(GUID  * guid,unsigned short *str);
HRESULT SetCapMode(IBaseFilter *pCap);
HRESULT OpenCamera(LPCOLESTR lpFile,BOOL bCapture,BOOL bStill,BOOL bPreview);
void WriteBMPToDisk(unsigned char *pStillImageBuffer);
void UpdatePictureNumber();
HRESULT ActivatePreviousInstance(const TCHAR* pszClass,const TCHAR* pszTitle,BOOL* pfActivated);

HRESULT Callback( IMediaSample * pSample, REFERENCE_TIME * StartTime, REFERENCE_TIME * 
				  StopTime,BOOL TypeChanged )
{
	unsigned char *pbuf;
	HRESULT  hr;
    // NOTE: We cannot do anything with this sample until we call GetConnectedMediaType 
    // on the filter to find out what format these samples are.	
    RETAILMSG(1, (TEXT("Callback with sample %lx for time %ld"), pSample, long( *StartTime / 10000 )  ) );
	hr = pSample->GetPointer(&pbuf);
	WriteBMPToDisk(pbuf);
	return S_OK;
}

// Start previewing
//
BOOL StartPreview()
{
	HRESULT hr;
    RETAILMSG(1,(TEXT("StartPreview++++\r\n")));
    // way ahead of you
    if(gcap.fPreviewing)
        return TRUE;

    
    // run the graph
    IMediaControl *pMC = NULL;
    hr = gcap.pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if(SUCCEEDED(hr))
    {
        hr = pMC->Run();
        if(FAILED(hr))
        {
            // stop parts that ran
            pMC->Stop();
        }
        pMC->Release();
    }
    if(FAILED(hr))
    {
        return FALSE;
    }
	gcap.pVWS->put_WindowState(SW_HIDE);		
	gcap.pVWP->put_WindowState(SW_SHOW);
    gcap.fPreviewing = TRUE;
    RETAILMSG(1,(TEXT("StartPreview----\r\n")));
    return TRUE;
}


// stop the preview graph
//
BOOL StopPreview()
{
    // way ahead of you
    RETAILMSG(1,(TEXT("StopPreview++++\r\n")));
    if(!gcap.fPreviewing)
    {
        return FALSE;
    }

    // stop the graph
    IMediaControl *pMC = NULL;
    HRESULT hr = gcap.pGraph->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if(SUCCEEDED(hr))
    {
        hr = pMC->Stop();
        pMC->Release();
    }
    if(FAILED(hr))
    {      
        return FALSE;
    }

    gcap.fPreviewing = FALSE;
    RETAILMSG(1,(TEXT("StopPreview----\r\n")));

    return TRUE;
}



HRESULT CloseCamera()
{
    HRESULT hr;	

    RETAILMSG(1,(TEXT("CloseCamera++++\r\n")));
	StopPreview();	
	SAFE_RELEASE(gcap.pCap);	
    SAFE_RELEASE(gcap.pConfigP);

    if(gcap.pVWS)
    {
       gcap.pVWS->put_Visible(OAFALSE);
       gcap.pVWS->put_Owner(NULL);
	   hr = gcap.pVWS->Release();
    }
	
	if(gcap.pVWP)
    {
       gcap.pVWP->put_Visible(OAFALSE);
       gcap.pVWP->put_Owner(NULL);
	   hr = gcap.pVWP->Release();
    }

    SAFE_RELEASE(gcap.pGraph);
	SAFE_RELEASE(gcap.pBuilder); 
    // Finished with COM
    CoUninitialize();

	//bOpenCamera = FALSE;
    RETAILMSG(1,(TEXT("CloseCamera----\r\n")));
	
	return hr;
}

HRESULT RenderStill()
{
    HRESULT hr;      
	hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_STILL,&MEDIATYPE_Video, gcap.pCap, NULL, NULL);	
	return hr;
}

HRESULT CaptureStillImage()
{
	HRESULT hr;	
	IBaseFilter *pMux = NULL;
	IFileSinkFilter *pSink = NULL;
	IMediaControl *pMC = NULL;
    SHMENUBARINFO mbi;

	if (!hWndMenuStill)
	{
		ZeroMemory(&mbi, sizeof(SHMENUBARINFO));
		mbi.cbSize = sizeof(SHMENUBARINFO);
		mbi.hwndParent = hWndApp;
		mbi.nToolBarId = IDM_MENU_STILL;
		mbi.hInstRes = hInst;
		mbi.dwFlags = SHCMBF_HMENU|SHCMBF_HIDDEN|SHCMBF_HIDESIPBUTTON ;

		if(!SHCreateMenuBar(&mbi))
		{
			//Couldn't create the menu bar.  Fail creation of the window.			
		}
		hWndMenuStill = mbi.hwndMB;
	}
	

	if (gcap.fPreviewing)
	{	        
        hr = SetCapMode(gcap.pCap); //Run still pin		

		ShowWindow(hWndMenu,SW_HIDE);
		ShowWindow(hWndMenuStill,SW_SHOW);
		gcap.pVWP->put_WindowState(SW_HIDE);
		gcap.pVWS->put_WindowState(SW_SHOW);	
		gcap.fPreviewing=false; 
		// create the menu bar
	}
	else
	{
		ShowWindow(hWndMenu,SW_SHOW);
		ShowWindow(hWndMenuStill,SW_HIDE);
		gcap.pVWS->put_WindowState(SW_HIDE);		
		gcap.pVWP->put_WindowState(SW_SHOW);
		gcap.fPreviewing=true;	
	}
	return hr;
}


HRESULT InitCapFilter()
{
    HRESULT hr;
	IPersistPropertyBag *pPropertyBag = NULL;
    // Create Capture Filter
	
    RETAILMSG(1,(TEXT("InitCapFilter++++\r\n")));
    hr = CoCreateInstance(CLSID_VideoCapture, NULL, CLSCTX_INPROC_SERVER,IID_IBaseFilter,
		                   (void **)&gcap.pCap);

    GUID clsid = DEVCLASS_CAMERA_GUID;
    DEVMGR_DEVICE_INFORMATION pdi;

    HANDLE hand = FindFirstDevice(DeviceSearchByGuid,&clsid,&pdi);

	RETAILMSG(1, (TEXT("CamTest: Find device: %x %x\r\n"),hand,pdi.szDeviceName));
	
    hr = gcap.pCap->QueryInterface(IID_IPersistPropertyBag, (void **)&pPropertyBag);
	if (!SUCCEEDED(hr))
		return hr;

    VARIANT varCamName;		
    IPropertyBag *propBag = NULL;
    varCamName.byref =  L"CAM1:" ;	

    //hr = gcap.pCap->QueryInterface(IID_IPropertyBag, (void **)&propBag);
    
	//hr = propBag->Write(L"VCapName", &varCamName);
	
    hr = pPropertyBag->Load(propBag,NULL);	
	
 	pPropertyBag->Release();	

    RETAILMSG(1,(TEXT("InitCapFilter----\r\n")));
	return S_OK;
}

void SetupVideoWindow(IVideoWindow *pVW)
{
	RECT rt;
	long lWidth, lHeight;	
	HRESULT hr;
	
    RETAILMSG(1,(TEXT("SetupVideoWindow++++\r\n")));
	if (!pVW)
	{
		hr = pVW->put_Owner((OAHWND)hWndApp) ;
	    hr = pVW->put_WindowStyle(WS_CHILD); 
	}

    if (GetWindowRect(hWndApp, &rt))
	{				
		pVW->get_Width(&lWidth);
		pVW->get_Height(&lHeight);
		if ((lWidth < rt.right)&&(lHeight<rt.bottom))
		{
			pVW->put_Left(rt.left + (rt.right-rt.left-lWidth)/2);
			pVW->put_Top( (rt.bottom-rt.top-lHeight)/2);		
		}
		else
		{
			pVW->put_Left(rt.left);
			pVW->put_Top(0);
			pVW->put_Width(rt.right);
			pVW->put_Height(rt.bottom-rt.top);
		}		
	}	
    RETAILMSG(1,(TEXT("SetupVideoWindow----w:%d h:%d\r\n"),lWidth, lHeight));
}

HRESULT ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pF1, int iPin1,IBaseFilter *pF2,int iPin2,IPin **ppPinout)
{
	IPin *pPin1, *pPin2;
	IEnumPins    *pEnum;	
	unsigned long fetched;
	HRESULT hr;

    RETAILMSG(1,(TEXT("ConnectFilters++++\r\n")));
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

    RETAILMSG(1,(TEXT("ConnectFilters----\r\n")));
	return hr;
}


HRESULT OpenCamera(LPCOLESTR lpFile,BOOL bCapture,BOOL bStill,BOOL bPreview)
{
    HRESULT hr;
	IBaseFilter *pMux = NULL;
	IPin *pPin;

    RETAILMSG(1,(TEXT("OpenCamera++++\r\n")));
	gcap.pGrab = new CSampleGrabber(NULL,&hr,FALSE);	
	gcap.pGrab->AddRef();
	gcap.pGrab->SetCallback(&Callback);
	CMediaType mt;
	mt.SetType(&MEDIATYPE_Video);
	mt.SetSubtype(&MEDIASUBTYPE_YV12);
	gcap.pGrab->SetAcceptedMediaType(&mt);

	// Create the Filter Graph Manager.
	hr =  CoCreateInstance(CLSID_FilterGraph, NULL,
						CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&gcap.pGraph);
 
	// Create the Capture Graph Builder.
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder, NULL,
							  CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
							  (void **)&gcap.pBuilder);

	gcap.pBuilder->SetFiltergraph(gcap.pGraph);

	hr = CoCreateInstance(CLSID_VideoRenderer, NULL,
							  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
							  (void **)&gcap.pRenderP);

	hr = CoCreateInstance(CLSID_VideoRenderer, NULL,
							  CLSCTX_INPROC_SERVER, IID_IBaseFilter, 
							  (void **)&gcap.pRenderS);

	hr = gcap.pGraph->AddFilter(gcap.pCap,L"Video Capture Source");	

	hr = gcap.pGraph->AddFilter(gcap.pRenderP,L"Video Render1");

	//Add filter to graph builder
	hr = gcap.pGraph->AddFilter(gcap.pRenderS,L"Video Render2");

	hr = gcap.pGraph->AddFilter(gcap.pGrab,L"Grabber");
	
	if (bStill)
	{
		if (lpFile == NULL)
		{	

			ConnectFilters(gcap.pGraph,gcap.pCap,1,gcap.pGrab,0,&pPin); // 1 is still pin
			ConnectFilters(gcap.pGraph,gcap.pGrab,1,gcap.pRenderS,0,NULL); // 1 is grab's output pin

			gcap.pGraph->Render(pPin);
			
			hr = gcap.pRenderS->QueryInterface(IID_IVideoWindow, (void**)&gcap.pVWS);			
			hr = gcap.pRenderS->QueryInterface(IID_IBasicVideo, (void**)&gcap.pBV);
			hr = gcap.pRenderS->QueryInterface(IID_IDirectDrawVideo, (void **)&gcap.pDdrawVideoS);
			gcap.pBV->SetDestinationPosition(0,0,240,300);
			gcap.pVWS->put_Owner((OAHWND)hWndApp);
			SetupVideoWindow(gcap.pVWS);
		}
		else
		{			
			hr = gcap.pBuilder->SetOutputFileName(&MEDIASUBTYPE_Asf,lpFile,&pMux,NULL);
			hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_STILL,&MEDIATYPE_Video, gcap.pCap, NULL, pMux);
		
		}
	}

	//Start capture
	if (bCapture)  //capture to file
	{	
		if (lpFile == NULL)
		{
			hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video, gcap.pCap, NULL, NULL);		
		}
		else
		{			
			hr = gcap.pBuilder->SetOutputFileName(&MEDIASUBTYPE_Asf,lpFile,&pMux,NULL);
			hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,&MEDIATYPE_Video, gcap.pCap, NULL, pMux);
		
		}
		gcap.pBuilder->ControlStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, gcap.pCap, 0, 0,     // Start and stop times.
						wStartCookie, wStopCookie);
	}

	if (bPreview)
	{		
		hr = gcap.pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,&MEDIATYPE_Video, gcap.pCap, NULL, gcap.pRenderP);			

		hr = gcap.pRenderP->QueryInterface(IID_IVideoWindow, (void**)&gcap.pVWP);
		hr = gcap.pRenderP->QueryInterface(IID_IDirectDrawVideo, (void **)&gcap.pDdrawVideoP);
	    SetupVideoWindow(gcap.pVWP);
	}

    // Query for video interfaces, which may not be relevant for audio files
    hr = gcap.pGraph->QueryInterface(IID_IMediaEventEx, (void **)&gcap.pME);        
	hr = gcap.pCap->QueryInterface(IID_IAMCameraControl,(void **)&gcap.pCamControl);
	hr = gcap.pCap->QueryInterface(IID_IAMVideoProcAmp,(void **)&gcap.pVProcAmp);
    // Query the output pin for IAMStreamConfig (not shown).
	hr = gcap.pBuilder->FindInterface(
							&PIN_CATEGORY_PREVIEW, // Preview pin.
							0,    // Any media type.
							gcap.pCap, // Pointer to the capture filter.
							IID_IAMStreamConfig, (void**)&gcap.pConfigP);	
	
     // Have the graph signal event via window callbacks for performance
    hr = gcap.pME->SetNotifyWindow((OAHWND)hWndApp, WM_GRAPHNOTIFY, 0);    
	
    RETAILMSG(1,(TEXT("OpenCamera----\r\n")));
	return hr;
}


HRESULT SetCapMode(IBaseFilter *pCap)
{
    HRESULT hr;
	IPin *pPin = NULL;
	IEnumPins    *pEnum;	
	unsigned long fetched;
	int nPin = 1;	//Still Pin

    RETAILMSG(1,(TEXT("SetCapMode++++\r\n")));
	hr = gcap.pCap->QueryInterface(IID_IAMVideoControl,(void **)&gcap.pAMVidControl);

	hr = gcap.pCap->EnumPins(&pEnum);
	
	while (nPin>0)
	{	
		hr = pEnum->Next(1,&pPin,&fetched); 
		nPin--;
	}
	hr = pEnum->Next(1,&pPin,&fetched);

	if (SUCCEEDED(hr))
	{	  
		//hr = gcap.pAMVidControl->SetMode(pPin, VideoControlFlag_ExternalTriggerEnable);
	
		hr = gcap.pAMVidControl->SetMode(pPin, VideoControlFlag_Trigger);	

		//pPin->Release();
	}
	else
	{
		RETAILMSG(1, (TEXT("CamTest: Fail to Find Pin! %x\r\n"),hr));
	}
    RETAILMSG(1,(TEXT("SetCapMode----\r\n")));
	return hr;
}

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

    RETAILMSG(1,(TEXT("DDCam WinMain\r\n")));
	//Init COM
   // Get COM interfaces
	if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
    {
        RETAILMSG(1, (TEXT("CoInitialize Failed!\r\n")));
        return FALSE;
    }

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_WCETEST);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    // Finished with COM
    CoUninitialize();

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr;
	BOOL fActivated;
	TCHAR	szTitle[MAX_LOADSTRING];			// The title bar text
	TCHAR	szWindowClass[MAX_LOADSTRING];		// The window class name

    RETAILMSG(1,(TEXT("InitInstance++++\r\n")));
	hInst = hInstance;		// Store instance handle in our global variable
	// Initialize global strings
	LoadString(hInstance, IDC_WCETEST, szWindowClass, MAX_LOADSTRING);
	//MyRegisterClass(hInstance, szWindowClass);
	WNDCLASS	wc;

    wc.style			= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc		= (WNDPROC) WndProc;
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
        return(0);
    }

   // Create the main window.    
   //         WS_CLIPCHILDREN, // Setting this to 0 gives a default style we don't want.  Use a benign style bit instead.
  
	hWndApp = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWndApp)
	{	
		return FALSE;
	}

	ShowWindow(hWndApp, nCmdShow);
	UpdateWindow(hWndApp);

	hr = InitCapFilter();  
	if (SUCCEEDED(hr))
	{	
		OpenCamera(NULL,false,  //Capture
		            true,     //true,  //Still    
					true    //Preview
			);
		StartPreview();
		gcap.pVWS->put_WindowState(SW_HIDE);		
	}
	else
	{
		RETAILMSG(1,(TEXT("CamTest: Fail to create Capture filter. \r\n")));  
	}

    RETAILMSG(1,(TEXT("InitInstance----\r\n")));
	return TRUE;
}

BOOL ConvertGuidToString(GUID  * guid,unsigned short *str)
{
    // ConvertStringToGuid
    // this routine converts a string into a GUID and returns TRUE if the
    // conversion was successful.

    // Local variables.
    int  i;
    
	for (i = 0; i<MLIST_SIZE; i++)
    
    {   if (IsEqualGUID(*media_list[i].guid,*guid) )
		{			
			memcpy(str,media_list[i].text,6*sizeof(unsigned short)); //len = 6  :)
			return TRUE;
		}
	}    

    return FALSE;
}


/**************************************************************************************
   OnCreate
 **************************************************************************************/
LRESULT OnCreate( HWND hwnd,  CREATESTRUCT* lParam )
{
    // create the menu bar
    SHMENUBARINFO mbi;
    ZeroMemory(&mbi, sizeof(SHMENUBARINFO));
    mbi.cbSize = sizeof(SHMENUBARINFO);
    mbi.hwndParent = hwnd;
    mbi.nToolBarId = IDM_MENU;
    mbi.hInstRes = hInst;
	mbi.dwFlags = SHCMBF_HMENU;

    RETAILMSG(1,(TEXT("DDCam OnCreate\r\n")));
    if(!SHCreateMenuBar(&mbi))
    {
        // Couldn't create the menu bar.  Fail creation of the window.
        return(-1);
    }

	hWndMenu = mbi.hwndMB; 

	//hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDM_MENU)); //CommandBar_GetMenu(hWndMenu, 0);
	hMenu = GetSystemMenu(hWndMenu,FALSE);
			
	gcap.fPreviewing = FALSE;
	gcap.fStillCapturing  = FALSE;

    return(0); // continue creation of the window
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
	//HRESULT hr;
	unsigned char buf;
	long flag=CameraControl_Flags_Manual;


//	cbInput = sizeof(CameraApp);
//	cbOutput = 0;

    switch(message)
    {
		case WM_CHAR:
			buf = (unsigned char) wParam;
			RETAILMSG(1,(TEXT("Keypress 0x%x \r\n"), buf));

			switch(buf)
			{
				// Button "1"
			case 0x31:
				// Select format 1280*960; 
				RETAILMSG(1,(TEXT("Capture type: SXGA\r\n")));
				break;

			default:
				break;
			}

		break;

        case WM_CREATE:
            lResult = OnCreate(hWnd, (CREATESTRUCT*)lParam);
            break;

        case WM_SIZE:
			RETAILMSG(1,(TEXT("WM_SIZE\r\n")));
			//StopPreview();		
			//CloseCamera();
            //DestroyWindow(hWnd);
            //PostQuitMessage(0);
            break;
			
        case WM_COMMAND:
            switch (wParam)
            {
				case ID_STILLCAPTURE:					   
					RETAILMSG(1,(TEXT("ID_STILLCAPTURE\r\n")));
					CaptureStillImage();					
					//ReadFrameFromFile();
					break;
					
                case ID_CONTROL_EXIT:
					RETAILMSG(1,(TEXT("ID_CONTROL_EXIT\r\n")));
					StopPreview();		
					CloseCamera();
	                DestroyWindow(hWnd);
				    break;

				case ID_CAMERA_START:
					RETAILMSG(1,(TEXT("ID_CAMERA_START\r\n")));
					StartPreview();
					break;
					
				case ID_CAMERA_STOP:
					RETAILMSG(1,(TEXT("ID_CAMERA_STOP\r\n")));
					StopPreview();
					break;

				default:
					RETAILMSG(1,(TEXT("default %x\r\n"),wParam));
                    goto DoDefault;
            }
            break;

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
			RETAILMSG(1,(TEXT("WM_DESTROY\r\n")));
			StopPreview();		
			CloseCamera();
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            break;

        DoDefault:
        default:
            lResult = DefWindowProc(hWnd, message, wParam, lParam);
            break;
    }

    return(lResult);
}


void WriteBMPToDisk(unsigned char *pStillImageBuffer)
{
	DWORD dwSize;
	HANDLE hFile;
	unsigned short *x;
	BOOL rc = TRUE;	

	BITMAPINFOHEADER	bitmapinfoheader;
	BITMAPFILEHEADER	hdr;
	DWORD               color_table[3];
	long bitmapSize = 0;
	
	int still_size = StillWidth*StillHeight*2;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= still_size + sizeof( hdr ) + sizeof(BITMAPINFOHEADER)+ sizeof(DWORD)*3;
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr )) + sizeof(BITMAPINFOHEADER) + sizeof(DWORD)*3;

	bitmapinfoheader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapinfoheader.biWidth = StillWidth;
	bitmapinfoheader.biHeight = StillHeight;
	bitmapinfoheader.biPlanes = 1;
	bitmapinfoheader.biBitCount = 16;
	bitmapinfoheader.biCompression = BI_BITFIELDS; //BI_RGB;
	bitmapinfoheader.biSizeImage = 0;
	bitmapinfoheader.biClrUsed = 0;

	color_table[0] = 0xF800;
    color_table[1] = 0x07E0;
    color_table[2] = 0x001F;

	//UpdatePictureNumber();
	x = (unsigned short*)malloc(256);	
	bitmapSize = still_size;
	
    //pStillImageBuffer = new unsigned char[bitmapSize];
	if (pStillImageBuffer)
	{			  	 
	   wsprintf((unsigned short *)x, TEXT("%s\\image%d.bmp"), picture_path, picture_number++);
	   hFile = CreateFile(x, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	   if (hFile != NULL)
	   {
		   WriteFile(hFile, &hdr, sizeof(BITMAPFILEHEADER), &dwSize, NULL);
		   WriteFile(hFile, &bitmapinfoheader, sizeof(BITMAPINFOHEADER), &dwSize, NULL);
		   WriteFile(hFile, color_table, sizeof(DWORD)*3, &dwSize, NULL);
		   WriteFile(hFile, pStillImageBuffer, bitmapSize, &dwSize, NULL);
		   CloseHandle(hFile);
		   //free(pStillImageBuffer);				 
	   }
	   else
	   {
		   RETAILMSG(1,(TEXT("CamTest:Cannot create file \r\n")));
	   }

	}
	else
	{
	    RETAILMSG(1,(TEXT("CamTest:Null buffer \r\n")));
	}

	free((void *)x);
	
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


/****************************************************************************

   ActivatePreviousInstance

  ****************************************************************************/
HRESULT ActivatePreviousInstance(
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
            // Something bad happened, fail.
            hr = E_FAIL;
            goto Exit;
        }

        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            HWND hwnd;

            CloseHandle(hMutex);
            hMutex = NULL;

            // There is already an instance of this app
            // running.  Try to bring it to the foreground.

            hwnd = FindWindow(pszClass, pszTitle);
            if(NULL == hwnd)
            {
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
            // We were the first one to create the mutex
            // so that makes us the main instance.  'leak'
            // the mutex in this function so it gets cleaned
            // up by the OS when this instance exits.
            break;
        }
    }

    if(cTries <= 0)
    {
        // Someone else owns the mutex but we cannot find
        // their main window to activate.
        hr = E_FAIL;
        goto Exit;
    }

Exit:
    return(hr);
}


