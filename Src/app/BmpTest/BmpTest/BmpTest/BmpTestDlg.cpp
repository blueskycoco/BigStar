// BmpTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BmpTest.h"
#include "BmpTestDlg.h"
#include <Pm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define ShutDownCode 0xdeadbeef
// CBmpTestDlg 对话框

BOOL set=FALSE;
//HANDLE hOpenDev;
DEVMODE g_devmode={0};
HANDLE APPEvent = NULL;
#define APP_NOTIFY_EVENT (TEXT("POWER APP KEY"))

CBmpTestDlg::CBmpTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBmpTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	if (!SleepBT.LoadBitmaps(IDB_BITMAP2,IDB_BITMAP3) ||
        !ShutDownBT.LoadBitmaps(IDB_BITMAP4,IDB_BITMAP5)||!STDBT.LoadBitmaps(IDB_BITMAP6,IDB_BITMAP8))     //装载按键图片
    {
        TRACE0("Failed to load bitmaps for buttons\n");
        AfxThrowResourceException();
    }
}

CBmpTestDlg::~CBmpTestDlg()
{
	if(g_hSerial!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hSerial);
	}
/*	if(hOpenDev!=INVALID_HANDLE_VALUE)
	{
		CloseHandle(hOpenDev);
	}*/
}

void CBmpTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBmpTestDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_BN_CLICKED(SleepBUTTON, &CBmpTestDlg::OnBnClickedSleepbutton)
	ON_BN_CLICKED(CANCEL_BUTTON, &CBmpTestDlg::OnBnClickedCancel)
	ON_BN_CLICKED(STD_BUTTON, &CBmpTestDlg::OnBnClickedButtonSTD)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(SHUTDOWN_BUTTON, &CBmpTestDlg::OnClickPowerOff)
END_MESSAGE_MAP()


// CBmpTestDlg 消息处理程序

BOOL CBmpTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	VERIFY(SleepBT.SubclassDlgItem(SleepBUTTON, this));
    SleepBT.SizeToContent();
    VERIFY(ShutDownBT.SubclassDlgItem(SHUTDOWN_BUTTON, this));
    ShutDownBT.SizeToContent();
	VERIFY(STDBT.SubclassDlgItem(STD_BUTTON, this));
    STDBT.SizeToContent();

	APPEvent = CreateEvent(NULL, FALSE, FALSE, APP_NOTIFY_EVENT); 	        //中断事件
	if(APPEvent==INVALID_HANDLE_VALUE)
	{
		//TRACE0("Failed to initial serial!\n");
		return FALSE;
	}

	m_Bkgnd =::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP1) );
	if( m_Bkgnd)
	{
	  m_BrCtl=GetBkBrush(GetSafeHwnd(),IDC_STATIC_1, m_Bkgnd );
	}
	g_hSerial=InitSerial();
	if(g_hSerial==INVALID_HANDLE_VALUE)
	{
		TRACE0("Failed to initial serial!\n");
		return FALSE;
	}
	/*hOpenDev=CreateFile(TEXT("KEY2:"),GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hOpenDev == INVALID_HANDLE_VALUE)
	{
		TRACE0("Failed to open button dev!\n");
		return FALSE;
	}*/
	m_pUIThread=AfxBeginThread(AFX_THREADPROC(ThreadProc),this);

	SetWindowPos(&wndTopMost,   0,0,0,0,   SWP_NOSIZE|SWP_NOMOVE); 

	HANDLE hevReloadActivityTimeouts = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("PowerManager/ReloadActivityTimeouts"));
	if (hevReloadActivityTimeouts) 
	{	

		SetEvent(hevReloadActivityTimeouts);
		RETAILMSG(1,(TEXT("SET event ok!\r\n")));
		CloseHandle(hevReloadActivityTimeouts);
	}  


	


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}



#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CBmpTestDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_BMPTEST_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_BMPTEST_DIALOG));
	}
}
#endif

void CBmpTestDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CDC memDC;
	CBitmap bmBkgnd;
	bmBkgnd.Attach( m_Bkgnd );

	memDC.CreateCompatibleDC(&dc);
	CBitmap *pOldBm = memDC.SelectObject( &bmBkgnd );

	BITMAP bm;
	bmBkgnd.GetObject(sizeof(bm), &bm);

	dc.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &memDC, 0, 0, SRCCOPY );
	memDC.SelectObject( pOldBm );
	bmBkgnd.Detach();

	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()

}

void CBmpTestDlg::OnBnClickedSleepbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	//SetSystemPowerState(NULL,POWER_STATE_SUSPEND,NULL);
	SetSystemPowerState(NULL, POWER_STATE_RESET, POWER_FORCE);
	ShowWindow(SW_HIDE);
}

void CBmpTestDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//ShowWindow(SW_HIDE);
	CloseHandle(g_hSerial);
	//DEVMODE devmode={0};
	if(set==TRUE){
	g_devmode.dmSize=sizeof(DEVMODE);
	//devmode.dmDisplayOrientation=DMDO_0;
	g_devmode.dmFields=DM_DISPLAYORIENTATION;
	ChangeDisplaySettingsEx(NULL,&g_devmode,NULL,0,NULL);
	RETAILMSG(1,(TEXT("Rotate Screen again \r\n")));
	}
	PostQuitMessage(0);
}

void CBmpTestDlg::OnBnClickedButtonSTD()
{
	// TODO: 在此添加控件通知处理程序代码
	//DeviceIoControl(hOpenDev,ShutDownCode,NULL,0,NULL,0,NULL,NULL);    //设置对应的寄存器的值  下面执行的是 STD
	//SetSystemPowerState(NULL,POWER_STATE_SUSPEND,NULL);
	ShowWindow(SW_HIDE);
}


UINT CBmpTestDlg::ThreadProc(LPVOID lParam)
{
	CBmpTestDlg *dlg = (CBmpTestDlg *)lParam;	
	while(1)
	{
		//RETAILMSG(1,(TEXT("Enter waiting thread.....\r\n")));
		WaitForSingleObject(APPEvent,INFINITE);
		DEVMODE devmode={0};
		g_devmode.dmSize=sizeof(DEVMODE);
			//devmode.dmDisplayOrientation=DMDO_90;
		g_devmode.dmFields=DM_DISPLAYORIENTATION;
		ChangeDisplaySettingsEx(NULL,&g_devmode,NULL,CDS_TEST,NULL);
		if(g_devmode.dmDisplayOrientation!=DMDO_90)
		{		
			RETAILMSG(1,(TEXT("Rotate Screen %d\r\n"),g_devmode.dmDisplayOrientation));
			devmode.dmSize=sizeof(DEVMODE);
			devmode.dmDisplayOrientation=DMDO_90;
			devmode.dmFields=DM_DISPLAYORIENTATION;
			ChangeDisplaySettingsEx(NULL,&devmode,NULL,0,NULL);
			set=TRUE;
		}
		dlg->ShowWindow(SW_SHOW);
		//RETAILMSG(1,(TEXT("Thread Run.....\r\n")));
	}
    return  500; 
}


HBRUSH CBmpTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性

	if((pWnd->GetDlgCtrlID()   ==   IDC_STATIC_1)||(pWnd->GetDlgCtrlID()   ==   IDC_STATIC_2)||(pWnd->GetDlgCtrlID()   ==   IDC_STATIC_3))
	{
        pDC->SetTextColor(RGB(255, 255, 255));
        pDC->SetBkMode( TRANSPARENT ) ;
        return m_BrCtl;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

HBRUSH CBmpTestDlg::GetBkBrush(HWND hWnd, UINT nID , HBITMAP hBmBk)
{
#define _X(x) (x.left)
#define _Y(x) (x.top)
#define _W(x)     (x.right - x.left)
#define _H(x) (x.bottom - x.top)
	HWND hWndCtrl;
	hWndCtrl = ::GetDlgItem( hWnd, nID );
	HBRUSH hBrushCtrl = NULL;
	if( NULL != hWndCtrl )
	{
		RECT rcCtrl;
		::GetWindowRect( hWndCtrl, &rcCtrl );
		::ScreenToClient(hWnd, (LPPOINT)&rcCtrl);
		::ScreenToClient(hWnd, ((LPPOINT)&rcCtrl)+1);
		HDC hDC = ::GetDC(hWnd);
		HDC hMemDCBk = CreateCompatibleDC( hDC );
		HDC hMemDCCtrl = CreateCompatibleDC( hDC );

		HBITMAP hBmCtrl = CreateCompatibleBitmap( hDC, _W(rcCtrl), _H(rcCtrl) );
		HBITMAP hBmOldBk;
		HBITMAP hBmOldCtrl;

		hBmOldBk = (HBITMAP) ::SelectObject( hMemDCBk, hBmBk );
		hBmOldCtrl = (HBITMAP) ::SelectObject( hMemDCCtrl, hBmCtrl );

		::BitBlt( hMemDCCtrl, 0, 0, _W(rcCtrl), _H(rcCtrl), hMemDCBk, _X(rcCtrl), _Y(rcCtrl), SRCCOPY );

		::SelectObject(hMemDCCtrl, hBmOldCtrl );
		::SelectObject(hMemDCBk, hBmOldBk );
	    
		hBrushCtrl = ::CreatePatternBrush( hBmCtrl );

		DeleteObject( hBmCtrl );

		::DeleteDC( hMemDCBk );
		::DeleteDC( hMemDCCtrl );
		::ReleaseDC( hWnd, hDC );
	}
	return hBrushCtrl;
}

HANDLE CBmpTestDlg::InitSerial(void)
{
	HANDLE m_hSerial;
	DCB SerPort;
	COMMTIMEOUTS CommTimeouts;

	m_hSerial = CreateFile(L"COM3:",GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(m_hSerial == INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL,_T("Create Serial Fail"),NULL,MB_OK);
		return INVALID_HANDLE_VALUE;
	}

	SerPort.DCBlength = sizeof(DCB);
	GetCommState(m_hSerial,&SerPort);
	SerPort.BaudRate = 9600;
	SerPort.ByteSize = 8;
	SerPort.Parity = NOPARITY;
	SerPort.StopBits = ONESTOPBIT;

	if(! SetCommState(m_hSerial,&SerPort))
	{
		::MessageBox(NULL,_T("Set Serial Fail"),NULL,MB_OK);
		return INVALID_HANDLE_VALUE;
	}
	GetCommTimeouts(m_hSerial,&CommTimeouts);
	CommTimeouts.ReadIntervalTimeout = MAXDWORD;
	CommTimeouts.ReadTotalTimeoutConstant = 10;
	CommTimeouts.ReadTotalTimeoutMultiplier = 10;
	CommTimeouts.WriteTotalTimeoutMultiplier = 50;
	CommTimeouts.WriteTotalTimeoutConstant = 100;

	if(!SetCommTimeouts(m_hSerial,&CommTimeouts))
	{
		::MessageBox(NULL,_T("Set SerialTime Fail"),NULL,MB_OK);
		return INVALID_HANDLE_VALUE;
	}

	return m_hSerial;
}

void CBmpTestDlg::OnClickPowerOff()
{
	DWORD dwNumber;
	UCHAR buff[7]={0xde,0xad,0x55,0xaa,0xbe,0xef};
	// TODO: 在此添加控件通知处理程序代码
	if(g_hSerial!=INVALID_HANDLE_VALUE)
	{
		WriteFile(g_hSerial,buff,6,&dwNumber,NULL);
		//RETAILMSG(1,(TEXT("%d writed!!!\r\n"),dwNumber));
	}
	ShowWindow(SW_HIDE);
}

void CBmpTestDlg::PreInitDialog()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::PreInitDialog();
}

BOOL CBmpTestDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialog::PreCreateWindow(cs);
}
