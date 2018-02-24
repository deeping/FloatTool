#include "StdAfx.h"
#include "Configuration.h"
#include "IniFile.h"

Configuration* Configuration::pInstance = NULL;

Configuration::Configuration(void)
{
	TRACE(_T("Configuration::Configuration\n"));

	xScreen = ::GetSystemMetrics( SM_CXSCREEN );
	yScreen = ::GetSystemMetrics( SM_CYSCREEN );

	//��ȡini�ļ�·��
	GetModuleFileName(AfxGetInstanceHandle(), mIniFileName, _MAX_FNAME);

	TCHAR *pos = wcsrchr (mIniFileName,'.');
	if(pos!=NULL){
		*(pos+1)=0;
		t_strcat(mIniFileName, _T("ini"));
		TRACE(_T("Configuration::Configuration %s\n"), mIniFileName);
	}

	ParseIniFile();
}

Configuration::~Configuration(void)
{
	TRACE(_T("Configuration::~Configuration\n"));
}

void Configuration::ParseIniFile()
{
	TCHAR key_buffer[MAX_PATH];
	TCHAR value_buffer[MAX_PATH];
	DWORD ret = 0;
	int i;

	//��ȡ�Զ���������
	for(i=0;;i++){
		memset(key_buffer, 0, MAX_PATH);
		memset(value_buffer, 0, MAX_PATH);

		t_strcat(key_buffer, _T("cmd"));
		_itow(i, value_buffer, 10);
		t_strcat(key_buffer, value_buffer);

		memset(value_buffer, 0, MAX_PATH);
		CeGetPrivateProfileString(_T("autorun"), key_buffer, _T(""), value_buffer, _MAX_FNAME,mIniFileName);
		TRACE(_T("autorun %s=%s len=%d\n"), key_buffer, value_buffer, t_strlen(value_buffer));

		if(wcslen(value_buffer)==0){
			break;
		}else{
			TRACE(_T("AutoRunCmdStrArray %d\n"), i);
			AutoRunCmdStrArray.Add(value_buffer);
		}
	}

	//��ȡ�˵�ѡ������
	for(i=0;;i++){
		memset(key_buffer, 0, MAX_PATH);
		memset(value_buffer, 0, MAX_PATH);

		t_strcat(key_buffer, _T("item"));
		_itow(i, value_buffer, 10);
		t_strcat(key_buffer, value_buffer);
		t_strcat(key_buffer, _T("_cmd"));

		memset(value_buffer, 0, MAX_PATH);
		CeGetPrivateProfileString(_T("menu"), key_buffer, _T(""), value_buffer, _MAX_FNAME,mIniFileName);
		TRACE(_T("menu %s=%s len=%d\n"), key_buffer, value_buffer, t_strlen(value_buffer));

		if(wcslen(value_buffer)==0){
			break;
		}else{
			menuItemCmdStrArray.Add(value_buffer);
		}
	}

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("background"), _T(""), value_buffer, t_strlen(value_buffer),mIniFileName);
	if(wcslen(value_buffer)==0){
		GetModuleFileName(AfxGetInstanceHandle(), menuBackFileName, _MAX_FNAME);
		TCHAR *pos = wcsrchr (menuBackFileName,'\\');
		if(pos!=NULL){
			*(pos+1)=0;
			t_strcat(menuBackFileName, _T("background.bmp"));
		}
	}else{
		wcscpy(menuBackFileName, value_buffer);
	}
	TRACE(_T("menuBackFileName=%s\n"), menuBackFileName);

	//��ȡ�˵���������
	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_width"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	menuItemWidth = _wtoi(value_buffer);
	TRACE(_T("menuItemWidth=%d\n"), menuItemWidth);

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_height"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	menuItemHeight = _wtoi(value_buffer);
	TRACE(_T("m_mnItemheight=%d\n"), menuItemHeight);

	if(menuItemWidth==-1 || menuItemHeight==-1){
		menuItemWidth = xScreen/3;
		menuItemHeight = yScreen/10;
	}
}

Configuration * Configuration::GetInstance()
{
	if(pInstance == NULL){
		pInstance = new Configuration();
	}
	return pInstance;
}
