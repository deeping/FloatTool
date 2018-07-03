#include "StdAfx.h"
#include "Configuration.h"
#include "IniFile.h"

Configuration* Configuration::pInstance = NULL;

Configuration::Configuration(void)
{
	TRACE(_T("Configuration::Configuration\n"));

	xScreen = ::GetSystemMetrics( SM_CXSCREEN );
	yScreen = ::GetSystemMetrics( SM_CYSCREEN );

	//获取ini文件路径
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

	//提取自动运行命令
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

	//提取菜单选项命令
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

	//提取悬浮图标背景
	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("icon"), _T("background"), _T(""), value_buffer, t_strlen(value_buffer),mIniFileName);
	if(wcslen(value_buffer)==0){
		GetModuleFileName(AfxGetInstanceHandle(), iconFileName, _MAX_FNAME);
		TCHAR *pos = wcsrchr (iconFileName,'\\');
		if(pos!=NULL){
			*(pos+1)=0;
			t_strcat(iconFileName, _T("icon.bmp"));
		}
	}else{
		wcscpy(iconFileName, value_buffer);
	}
	TRACE(_T("iconFileName=%s\n"), iconFileName);

	//提取悬浮图标属性配置
	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("icon"), _T("width"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	iconWidth = _wtoi(value_buffer);
	TRACE(_T("iconWidth=%d\n"), iconWidth);

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("icon"), _T("height"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	iconHeight = _wtoi(value_buffer);
	TRACE(_T("iconHeight=%d\n"), iconHeight);

	if(iconWidth<=0 || iconHeight<=0){
		iconWidth = iconHeight = 32;
	}
	if(iconWidth>xScreen){
		iconWidth = xScreen;
	}
	if(iconHeight>yScreen){
		iconHeight = yScreen;
	}

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("icon"), _T("x"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	iconPosX = _wtoi(value_buffer);
	TRACE(_T("x=%d\n"), iconPosX);

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("icon"), _T("y"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	iconPosY = _wtoi(value_buffer);
	TRACE(_T("y=%d\n"), iconPosY);

	if(iconPosX<0 || iconPosY<0){
		iconPosX = xScreen;
		iconPosY = yScreen/3;
	}
	if(iconPosX>xScreen-iconWidth){
		iconPosX = xScreen-iconWidth;
	}
	if(iconPosY>yScreen-iconHeight){
		iconPosY = yScreen-iconHeight;
	}

	//提取菜单背景
	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("background"), _T(""), value_buffer, t_strlen(value_buffer),mIniFileName);
	if(wcslen(value_buffer)==0){
		GetModuleFileName(AfxGetInstanceHandle(), menuBgFileName, _MAX_FNAME);
		TCHAR *pos = wcsrchr (menuBgFileName,'\\');
		if(pos!=NULL){
			*(pos+1)=0;
			t_strcat(menuBgFileName, _T("background.bmp"));
		}
	}else{
		wcscpy(menuBgFileName, value_buffer);
	}
	TRACE(_T("menuBgFileName=%s\n"), menuBgFileName);

	//提取菜单选项属性配置
	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_width"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	menuItemWidth = _wtoi(value_buffer);
	TRACE(_T("menuItemWidth=%d\n"), menuItemWidth);

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_height"), _T("-1"), value_buffer, t_strlen(value_buffer),mIniFileName);
	menuItemHeight = _wtoi(value_buffer);
	TRACE(_T("menuItemHeight=%d\n"), menuItemHeight);

	if(menuItemWidth==-1 || menuItemHeight==-1){
		menuItemWidth = xScreen/3;
		menuItemHeight = yScreen/10;
	}

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_color_normal"), _T("0xAAAAAA"), value_buffer, t_strlen(value_buffer),mIniFileName);
	menuItemColorNormal = _wtoi(value_buffer);
	TRACE(_T("menuItemColorNormal=%#x\n"), menuItemColorNormal);

	if(menuItemColorNormal<0 || menuItemColorNormal>0xFFFFFF){
		menuItemColorNormal = RGB(170,170,170);
	}

	memset(value_buffer, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_color_selected"), _T("0xFF7F00"), value_buffer, t_strlen(value_buffer),mIniFileName);
	menuItemColorSelected = _wtoi(value_buffer);
	TRACE(_T("menuItemColorSelected=%#x\n"), menuItemColorSelected);

	if(menuItemColorNormal<0 || menuItemColorNormal>0xFFFFFF){
		menuItemColorNormal = RGB(255,127,0);
	}
}

Configuration * Configuration::GetInstance()
{
	if(pInstance == NULL){
		pInstance = new Configuration();
	}
	return pInstance;
}
