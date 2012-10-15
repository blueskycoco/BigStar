// BmpTestDlg.h : ͷ�ļ�
//

#pragma once

// CBmpTestDlg �Ի���
class CBmpTestDlg : public CDialog
{
// ����
public:
	CBmpTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CBmpTestDlg();
// �Ի�������
	enum { IDD = IDD_BMPTEST_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;
	CWinThread* m_pUIThread;     //�߳�
	// ���ɵ���Ϣӳ�亯��
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
