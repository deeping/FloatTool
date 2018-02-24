// FloatTool.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FloatTool.h"
#include "FloatWnd.h"
#include "Configuration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFloatToolApp

BEGIN_MESSAGE_MAP(CFloatToolApp, CWinApp)
END_MESSAGE_MAP()


// CFloatToolApp ����
CFloatToolApp::CFloatToolApp()
: CWinApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CFloatToolApp ����
CFloatToolApp theApp;

// CFloatToolApp ��ʼ��

BOOL CFloatToolApp::InitInstance()
{
	TRACE(_T("CFloatToolApp::InitInstance\n"));

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//��������ֻ����һ��ʵ��  
    HANDLE m_hMutex = CreateMutex(NULL, FALSE, _T("FloatTool_App")); 

    // ���������ڣ����������һ��ʵ������
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        TRACE(_T("FloatTool_App already exists\n"));
		return FALSE;
	}

	Configuration* pcfg = Configuration::GetInstance();
	CFloatWnd* pWnd = new CFloatWnd;
	m_pMainWnd = pWnd;
	pWnd->ShowWindow(SW_SHOW);
	pWnd->UpdateWindow();

	TRACE(_T("CFloatToolApp AutoRunCmd size=%d\n"), pcfg->AutoRunCmdStrArray.GetSize());
	TRACE(_T("CFloatToolApp AutoRunCmd count=%d\n"), pcfg->AutoRunCmdStrArray.GetCount());

	//ִ�������е��Զ�������Ŀ
	SHELLEXECUTEINFO   ExecuteInfo;
	for(int i=0;i < pcfg->AutoRunCmdStrArray.GetCount();i++){
		TRACE(_T("%d/%d auto run %s\n"), i,pcfg->AutoRunCmdStrArray.GetCount(),pcfg->AutoRunCmdStrArray.GetAt(i));

		ExecuteInfo.cbSize = sizeof(ExecuteInfo);
		ExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;  
		ExecuteInfo.hwnd = NULL;
		ExecuteInfo.lpVerb = NULL;   
		ExecuteInfo.lpFile = pcfg->AutoRunCmdStrArray.GetAt(i);
		ExecuteInfo.lpParameters = NULL;
		ExecuteInfo.lpDirectory = NULL;
		ExecuteInfo.nShow = SW_HIDE; 

		BOOL bResult = ShellExecuteEx(&ExecuteInfo); 
		if(!bResult && (int)ExecuteInfo.hInstApp <= 32){    
			TCHAR msgBuffer[MAX_PATH];
			wsprintf(msgBuffer, _T("autorun item%d %s failed!"), i, ExecuteInfo.lpFile);
			AfxMessageBox(msgBuffer);
		}  
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return TRUE;
}
