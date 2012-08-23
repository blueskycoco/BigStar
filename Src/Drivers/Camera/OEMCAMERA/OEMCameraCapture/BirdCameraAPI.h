#if !defined(AFX_WCETEST_H__BF4C1C7A_BE2E_4AC0_9D93_F2727903D595__INCLUDED_)
#define AFX_WCETEST_H__BF4C1C7A_BE2E_4AC0_9D93_F2727903D595__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include <windows.h>
#include <mmsystem.h>
#include <streams.h>
#include <cs.h>
#include <csmedia.h>
#include <camera.h>
#include <aygshell.h>
#include "Dmodshow.h"
#include "Dmoreg.h"
#include "wmcodecids.h"
#include "Pm.h"

#include <initguid.h>
#include "amc_guid.h"
#include "CPropertyBag.h"

#define MAX_LOADSTRING 100
#define WM_GRAPHNOTIFY  WM_APP + 1
#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#define Theme_Frame_MAX	3

#define FOURCC_Y444     MAKEFOURCC('Y', '4', '4', '4')  // TIYUV: 1394 conferencing camera 4:4:4 mode 0
#define FOURCC_UYVY     MAKEFOURCC('U', 'Y', 'V', 'Y')  // MSYUV: 1394 conferencing camera 4:4:4 mode 1 and 3
#define FOURCC_Y411     MAKEFOURCC('Y', '4', '1', '1')  // TIYUV: 1394 conferencing camera 4:1:1 mode 2
#define FOURCC_Y800     MAKEFOURCC('Y', '8', '0', '0')  // TIYUV: 1394 conferencing camera 4:1:1 mode 5
#define FOURCC_YUV422   MAKEFOURCC('U', 'Y', 'V', 'Y')
#define FOURCC_YV16     MAKEFOURCC('Y', 'V', '1', '6')
#define menu_size1	24 //16*16
#define menu_size2	16 //20*20

#define CHK( x ) do{ if( FAILED( hr = ( x ))) { goto Cleanup; }} while( FALSE );
#define ERR( x ) do{ hr = x; goto Cleanup; } while( FALSE );

#define StillWidth  480
#define StillHeight 640

extern HRESULT	ReadUserRegistryData(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,DWORD* lpData);


/*enum CAMERA_MODE{
  CAMERA_MODE_STILL = 0,
  CAMERA_MODE_VIDEOONLY,
  CAMERA_MODE_VIDEOWITHAUDIO,*/
 //#define CAMERACAPTURE_MODE_STILLWithFrame 3//添加相框
//};


  //CAMERACAPTURE_MODE_STILL = 0,
  //CAMERACAPTURE_MODE_VIDEOONLY,
  //CAMERACAPTURE_MODE_VIDEOWITHAUDIO,
//#define   CAMERACAPTURE_MODE_STILLWithFrame 3

enum 	DuringCapture
{
	NotInCapture =0,
	InCapture,
};

enum 	Flash_state
{
	Flash_Off =0,
	Flash_On,
};

typedef enum enFILE
{
	FILE_FORMAT_MP4 = 1,
	FILE_FORMAT_3GP,
	FILE_FORMAT_3G2,
	FILE_FORMAT_JPG,
};

enum 	Resolution_Capture  //must be the same order as the camera driver 
{
	Resolution_Capture_QVGA =0,
	Resolution_Capture_QCIF,
};

enum 	WhiteBanlence  //must be the same order as the camera driver 
{
	WhiteBanlence_Auto =298,
	WhiteBanlence_Night=273,
	WhiteBanlence_Day=282,
	WhiteBanlence_Cloudy=291,
	WhiteBanlence_Fluor=300,
	WhiteBanlence_Lamp=309,
};

enum 	Resolution_Still_Capture  //must be the same order as the camera driver 
{
	Resolution_Still_QVGA =0,
	Resolution_Still_SXGA,
	Resolution_Still_QSXGA,
	Resolution_Still_VGA,
};

enum 	StillCaptureMode
{
	StillCaptureMode_Normal_capture =0,
	StillCaptureMode_Timer_Capture,
	StillCaptureMode_Sequence_Capture,
};

//const GUID MEDIASUBTYPE_YV16 = {FOURCC_YV16, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71};
typedef struct
{ 	
	const unsigned short *text;
	const GUID *guid;
} MEDIA_SUBTYPE_LIST;


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
	IAMStreamConfig  *pConfigC,*pConfigA; //Capture config
	IAMStreamConfig  *pConfigS; //Still  config
	//  IAMVfwCaptureDialogs *pDlg;  
	IBaseFilter            *pVideoEncoder;
	//IBaseFilter            *pASFMultiplexer;
	IDMOWrapperFilter      *pWrapperFilter;
	IFileSinkFilter        *pFileSinkFilter;
	//IMediaEvent            *pMediaEvent;

	IBaseFilter            *pAudioCaptureFilter;


	IBaseFilter *pRenderS;		//Still render
	IBaseFilter *pRenderP;		//Preview render
	IBaseFilter *pCap,*pAsfWriter,*pAudioEncoder;
	IBasicVideo *pBV; //basic video for still video render
	IGraphBuilder *pGraph;
	//CSampleGrabber *pGrab;
	IDirectDrawVideo *pDdrawVideoP;   //Preview directdraw video
	IDirectDrawVideo *pDdrawVideoS;   //Still directdraw video
	IFileSinkFilter *pSink; 	
	IImageSinkFilter	    *m_pImageSinkFilter;

	BOOL fStillGraphBuilt;
	BOOL fPreviewGraphBuilt;
	BOOL fCapturing;
	BOOL fPreviewing;
	double FrameRate;
	BOOL fWantPreview;
	int VideoMode;
	Resolution_Still_Capture	m_Resolution_Still;
	Resolution_Capture		m_Resolution_Capture;
	Flash_state	m_Flash_State;
	StillCaptureMode	m_Still_Capture_Mode;
	unsigned int		m_Theme_Frame;
	int m_WhiteBanlance;
} gcap;

DWORD	dwCameraAPIContext;
const TCHAR *picture_path = TEXT("\\my documents\\我的图片") ;
//const TCHAR *picture_path = TEXT("\\Storage Card") ;

// Forward declarations of functions included in this code module:

#endif // !defined(AFX_WCETEST_H__BF4C1C7A_BE2E_4AC0_9D93_F2727903D595__INCLUDED_)


class CCameraAPI
{
public:
    CCameraAPI( );

    ~CCameraAPI( );
	
HRESULT InitCamera(HINSTANCE hInstance,HANDLE* events);
HRESULT OpenCamera(PSHCAMERACAPTURE pshcc);
BOOL StartPreview();
BOOL StopPreview();
HWND	hWndApp;			//The window instance
PSHCAMERACAPTURE pshcc;
int m_nFileFormat;


	
private:

HRESULT InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK	WndProc			(HWND, UINT, WPARAM, LPARAM);
void UpdatePictureNumber();
void SetupVideoWindow(IVideoWindow *pVW);
HRESULT ActivatePreviousInstance(const TCHAR* pszClass,const TCHAR* pszTitle,BOOL* pfActivated);
HRESULT StartCaptureVideoInternal();
HRESULT StopCaptureVideoInternal(bool);
HRESULT InitPinVideoFormat(BOOL bCapture,BOOL bStill);
HRESULT InitGraph(BOOL bCapture,BOOL bStill);
void CleanUp();
HRESULT ProcessDShowEvent(HWND hWnd);
HRESULT CaptureKeyUpProc();
HRESULT Next_Menu_Proc();
HRESULT Previous_Menu_Proc();
HRESULT VisableDialog();
BOOL InitDialog(HWND hwndDlg);
static BOOL CALLBACK DialogProc(HWND hwndDlg,UINT uMsg, WPARAM wParam,LPARAM lParam);
HRESULT HandleDLGRUNCamera(HWND hwndDlg);
HRESULT InitCapFilter();
HRESULT UninitGraph();
HRESULT CaptureStillImage();
HRESULT ConnectFilters(IGraphBuilder *pGraph,IBaseFilter *pF1, int iPin1,IBaseFilter *pF2,int iPin2,IPin **ppPinout);
HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
HRESULT ConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
bool	Next_Theme_Frame();
bool	Previous_Theme_Frame();
bool	RotateHandle(bool	GetCurrentAngle,int NewAngle);
static void CALLBACK TimerProc(HWND hwnd,  UINT uMsg,  UINT idEvent,   DWORD dwTime );
HRESULT GetFirstCameraDriver( WCHAR *pwzName ) ;
HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
BOOL ChangeDlgItem(HWND hwndDlg);
void SetResolution(int Resolution,CAMERACAPTURE_MODE mode);
void SetFileFormat(int Format);
bool ValidateMemory(DWORD);
//HRESULT	ReadUserRegistryData(HKEY hKey,LPCWSTR lpSubKey,LPCWSTR lpValueName,DWORD* lpData);



unsigned long light;
unsigned long gain;
int exposure;
int picture_number;
bool	EnterPIMG;
bool Enable_Dialog;
int	bInStartMenu;
bool	bKeyDown;
int DuringInCapture;
int CurrentAngle;
UINT nSystemTimer;

RECT hWndrect; // 主窗口的位置
// Global Variables:
HINSTANCE			hInst;			// The current instance
HWND 				hWndDialog;
HWND                hWndMenu ;             //Menu handle
HWND				hWndMenuStill;
HMENU               hMenu;
HANDLE hThread;
HANDLE HANDLEOAEVENT;
HANDLE hEvent[6];
PSHCAMERACAPTURE pshcc_last;
bool	bFirstEnter;
bool	bCaptureWithFrame,bCaptureWithFrame_Last;
bool bModeChanged;
bool bOutOfMemory;
unsigned long VideoTime;
};
