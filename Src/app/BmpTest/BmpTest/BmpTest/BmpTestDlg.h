// BmpTestDlg.h : 头文件
//

#pragma once

// CBmpTestDlg 对话框
class CBmpTestDlg : public CDialog
{
// 构造
public:
	CBmpTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CBmpTestDlg();
// 对话框数据
	enum { IDD = IDD_BMPTEST_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	CWinThread* m_pUIThread;     //线程
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	static UINT CALLBACK ThreadProc(LPVOID lParam);

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()

public:


	CBitmapButton SleepBT, ShutDownBT,STDBT;

	afx_msg void OnPaint();
	afx_msg void OnBnClickedSleepbutton();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSTD();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	HBRUSH m_BrCtl;
	HBITMAP m_Bkgnd;
	HBRUSH GetBkBrush(HWND hWnd, UINT nID , HBITMAP hBmBk);
	HANDLE InitSerial(void);
	HANDLE g_hSerial;

	afx_msg void OnClickPowerOff();
protected:
	virtual void PreInitDialog();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};
