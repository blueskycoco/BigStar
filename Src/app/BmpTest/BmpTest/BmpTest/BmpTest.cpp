// BmpTest.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "BmpTest.h"
#include "BmpTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBmpTestApp

BEGIN_MESSAGE_MAP(CBmpTestApp, CWinApp)
END_MESSAGE_MAP()


// CBmpTestApp ����
CBmpTestApp::CBmpTestApp()
	: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CBmpTestApp ����
CBmpTestApp theApp;

// CBmpTestApp ��ʼ��

BOOL CBmpTestApp::InitInstance()
{
    // ��Ӧ�ó����ʼ���ڼ䣬Ӧ����һ�� SHInitExtraControls �Գ�ʼ��
    // ���� Windows Mobile �ض��ؼ����� CAPEDIT �� SIPPREF��
    SHInitExtraControls();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CBmpTestDlg dlg;
	m_pMainWnd = &dlg;
	if(dlg.Create(CBmpTestDlg::IDD, NULL))
	{
		MSG msg;
		while(GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		dlg.DestroyWindow();
	}
/*	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}*/

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
