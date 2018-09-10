// FloatTool.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "FloatTool.h"
#include "FloatWnd.h"
#include "Configuration.h"
#include "IniFile.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFloatToolApp

BEGIN_MESSAGE_MAP(CFloatToolApp, CWinApp)
END_MESSAGE_MAP()


// CFloatToolApp 构造
CFloatToolApp::CFloatToolApp()
: CWinApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CFloatToolApp 对象
CFloatToolApp theApp;

// CFloatToolApp 初始化

BOOL CFloatToolApp::InitInstance()
{
	TRACE(_T("CFloatToolApp::InitInstance\n"));

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//互斥量，只运行一个实例  
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, _T("FloatTool_App")); 

	// 互斥量存在，则程序已有一个实例运行
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		TRACE(_T("FloatTool_App already exists\n"));
		return FALSE;
	}

	Configuration* pcfg = Configuration::GetInstance();

	//执行配置中的自动运行项目
	int i=0;
	//如果存在<reload>命令，则解析新的配置文件
	for(i=0;i <  pcfg->AutoRunCmdStrArray.GetCount();i++){
		if(wcsstr(pcfg->AutoRunCmdStrArray.GetAt(i), _T("<reload"))!=NULL){
			DoReloadCommand(pcfg->AutoRunCmdStrArray.GetAt(i));
			break;
		}
	}

	//忽略<reload>命令
	for(i=0;i < pcfg->AutoRunCmdStrArray.GetCount();i++){
		if(wcsstr(pcfg->AutoRunCmdStrArray.GetAt(i), _T("<reload"))!=NULL){
			continue;
		}else if(!Util::ExecuteExCommand(pcfg->AutoRunCmdStrArray.GetAt(i))){
			TCHAR msgBuffer[MAX_PATH]={0};
			wsprintf(msgBuffer, _T("autorun item%d/%d %s failed!"), i, pcfg->AutoRunCmdStrArray.GetCount(), pcfg->AutoRunCmdStrArray.GetAt(i));
			AfxMessageBox(msgBuffer);
		}else{
			TRACE(_T("autorun item%d %s succeed!\n"), i,pcfg->AutoRunCmdStrArray.GetCount(),pcfg->AutoRunCmdStrArray.GetAt(i));
		}
	}

	CFloatWnd* pWnd = new CFloatWnd;
	m_pMainWnd = pWnd;
	pWnd->ShowWindow(SW_SHOW);
	//pWnd->UpdateWindow();
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return TRUE;
}

BOOL CFloatToolApp::DoReloadCommand(LPCTSTR cmd)
{
	TCHAR buf[MAX_PATH]={0};
	TCHAR *pos = buf;
	t_strcpy(buf, cmd);
	BOOL ret;

	while(*pos!='\\'&&*pos!=0){pos++;}

	if(*pos=='\\'){
		t_strncpy(buf, pos, t_strlen(pos)+1);
		pos=buf;
		while(*pos!='>'&&*pos!=0){pos++;}
		if(*pos=='>'){*pos=0;}

		TRACE(_T("fileName=%s len=%d\n"), buf, t_strlen(buf));
		if(GetFileAttributes(buf)==0xFFFFFFFF){
			ret = false;
		}else{
			ret = Configuration::GetInstance()->reLoadConfig(buf);
		}
	}else{
		ret = false;
	}

	if(!ret){
		TCHAR msgBuffer[MAX_PATH]={0};
		wsprintf(msgBuffer, _T("reload %s failed!"), buf);
		AfxMessageBox(msgBuffer);
	}else{
		TRACE(_T("reload %s succeed\n"), buf);
	}

	return ret;
}
