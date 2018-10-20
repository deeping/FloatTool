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
		TRACE(_T("CFloatToolApp already exists\n"));
		return FALSE;
	}

	DoAutoRunCommands();

	AddCustomFontResource();
	//ResetFontRegedit();

	//��������
	CFloatWnd* pWnd = new CFloatWnd;
	m_pMainWnd = pWnd;
	pWnd->ShowWindow(SW_SHOW);

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return TRUE;
}

int CFloatToolApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���

	RemoveCustomFontResource();
	TRACE(_T("CFloatToolApp::ExitInstance\n"));
	return CWinApp::ExitInstance();
}

void CFloatToolApp::DoAutoRunCommands()
{
	TCHAR msgBuffer[MAX_PATH]={0};
	Configuration* m_pcfg = Configuration::GetInstance();

	//ִ�������е��Զ�������Ŀ
	int i=0;
	//�������<reload>���������µ������ļ�
	for(i=0;i <  m_pcfg->AutoRunCmdStrArray.GetCount();i++){
		if(wcsstr(m_pcfg->AutoRunCmdStrArray.GetAt(i), _T("<reload"))!=NULL){
			if(!Util::DoReloadCommand(m_pcfg->AutoRunCmdStrArray.GetAt(i))){
				memset(msgBuffer, 0, sizeof(msgBuffer));
				wsprintf(msgBuffer, _T("run '%s' failed!"), m_pcfg->AutoRunCmdStrArray.GetAt(i));
				AfxMessageBox(msgBuffer);
			}
			break;
		}
	}

	//����<reload>����
	TCHAR fileName[MAX_PATH]={0};
	for(i=0;i < m_pcfg->AutoRunCmdStrArray.GetCount();i++){
		if(wcsstr(m_pcfg->AutoRunCmdStrArray.GetAt(i), _T("<reload"))!=NULL){
			continue;
		}else {
			Util::RelativeToAbsolutePath(m_pcfg->AutoRunCmdStrArray.GetAt(i), fileName, sizeof(fileName));
			if(!Util::ExecuteExCommand(fileName)){
				memset(msgBuffer, 0, sizeof(msgBuffer));
				wsprintf(msgBuffer, _T("autorun item%d/%d %s failed!"), i, m_pcfg->AutoRunCmdStrArray.GetCount(), m_pcfg->AutoRunCmdStrArray.GetAt(i));
				AfxMessageBox(msgBuffer);
			}
		}
	}
}

void CFloatToolApp::AddCustomFontResource()
{
	TCHAR msgBuffer[MAX_PATH]={0};
	Configuration* m_pcfg = Configuration::GetInstance();

	if(wcslen(m_pcfg->fontConfigInfo.fontFileName)!=0){
		TCHAR fontFileName[MAX_PATH]={0};
		Util::RelativeToAbsolutePath(m_pcfg->fontConfigInfo.fontFileName, fontFileName, sizeof(fontFileName));
		if(GetFileAttributes(fontFileName)!=INVALID_FILE_SIZE){
			if(!AddFontResource(fontFileName)){
				memset(msgBuffer, 0, sizeof(msgBuffer));
				wsprintf(msgBuffer, _T("add font resource %s failed!"), m_pcfg->fontConfigInfo.fontFileName);
				AfxMessageBox(msgBuffer);
			}else{
				::SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
			}
		}else{
			memset(msgBuffer, 0, sizeof(msgBuffer));
			wsprintf(msgBuffer, _T("font file '%s' not found!"), m_pcfg->fontConfigInfo.fontFileName);
			AfxMessageBox(msgBuffer);
		}
	}
}
 
void CFloatToolApp::RemoveCustomFontResource()
{
	TCHAR msgBuffer[MAX_PATH]={0};
	Configuration* m_pcfg = Configuration::GetInstance();

	if(wcslen(m_pcfg->fontConfigInfo.fontFileName)!=0){
		TCHAR fontFileName[MAX_PATH]={0};
		Util::RelativeToAbsolutePath(m_pcfg->fontConfigInfo.fontFileName, fontFileName, sizeof(fontFileName));
		if(GetFileAttributes(fontFileName)!=INVALID_FILE_SIZE){
			TRACE(_T("CFloatToolApp::RemoveFontResource '%s'\n"), fontFileName);
			if(!RemoveFontResource(fontFileName))
			{
				TCHAR msgBuffer[MAX_PATH];
				wsprintf(msgBuffer, _T("remove font resource %s failed!"), m_pcfg->fontConfigInfo.fontFileName);
			}else{
				::SendMessage( HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
			}
		}
	}
}
