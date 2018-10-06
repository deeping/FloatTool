// FloatTool.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "FloatTool.h"
#include "FloatWnd.h"
#include "Configuration.h"
#include "Util.h"

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

	//ִ�������е��Զ�������Ŀ
	int i=0;
	//�������<reload>���������µ������ļ�
	for(i=0;i <  pcfg->AutoRunCmdStrArray.GetCount();i++){
		if(wcsstr(pcfg->AutoRunCmdStrArray.GetAt(i), _T("<reload"))!=NULL){
			if(!Util::DoReloadCommand(pcfg->AutoRunCmdStrArray.GetAt(i))){
				TCHAR msgBuffer[MAX_PATH]={0};
				wsprintf(msgBuffer, _T("run '%s' failed!"), pcfg->AutoRunCmdStrArray.GetAt(i));
				AfxMessageBox(msgBuffer);
			}
			break;
		}
	}

	//����<reload>����
	TCHAR fileName[MAX_PATH]={0};
	for(i=0;i < pcfg->AutoRunCmdStrArray.GetCount();i++){
		if(wcsstr(pcfg->AutoRunCmdStrArray.GetAt(i), _T("<reload"))!=NULL){
			continue;
		}else {
			Util::RelativeToAbsolutePath(pcfg->AutoRunCmdStrArray.GetAt(i), fileName, sizeof(fileName));
			if(!Util::ExecuteExCommand(fileName)){
				TCHAR msgBuffer[MAX_PATH]={0};
				wsprintf(msgBuffer, _T("autorun item%d/%d %s failed!"), i, pcfg->AutoRunCmdStrArray.GetCount(), pcfg->AutoRunCmdStrArray.GetAt(i));
				AfxMessageBox(msgBuffer);
			}
		}
	}

	CFloatWnd* pWnd = new CFloatWnd;
	m_pMainWnd = pWnd;
	pWnd->ShowWindow(SW_SHOW);
	//pWnd->UpdateWindow();
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return TRUE;
}
