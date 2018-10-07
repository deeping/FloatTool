#include "StdAfx.h"
#include "Configuration.h"
#include "IniFile.h"

Configuration* Configuration::m_pInstance = NULL;

Configuration::Configuration(void)
{
	TRACE(_T("Configuration::Configuration\n"));
	m_lockFont = false;

	xScreen = ::GetSystemMetrics( SM_CXSCREEN );
	yScreen = ::GetSystemMetrics( SM_CYSCREEN );

	//获取ini文件路径
	GetModuleFileName(AfxGetInstanceHandle(), m_IniFileName, MAX_PATH);

	TCHAR *pos = wcsrchr (m_IniFileName,'.');
	if(pos!=NULL){
		*(pos+1)=0;
		wcscat_s(m_IniFileName, sizeof(m_IniFileName), _T("ini"));
	}

	ParseIniFile();
}

Configuration::~Configuration(void)
{
	TRACE(_T("Configuration::~Configuration\n"));
}

int Configuration::SetLockFont(BOOL lock)
{
	m_lockFont = lock;
	return 0;
}

BOOL Configuration::ParseIniFile(LPCTSTR lpFileName)
{
	TRACE(_T("Configuration::ParseIniFile(%s)\n"), lpFileName);
	wcscpy_s(m_IniFileName, sizeof(m_IniFileName), lpFileName);

	return ParseIniFile();
}

//////////////////////////////////////////////////////////////////////////
/*
函数: ParseIniFile
功能: 解析ini文件
*/
//////////////////////////////////////////////////////////////////////////
BOOL Configuration::ParseIniFile()
{
	TRACE(_T("Configuration::ParseIniFile() %s\n"), m_IniFileName);
	AutoRunCmdStrArray.RemoveAll();
	menuItemCmdStrArray.RemoveAll();
	menuItemTextStrArray.RemoveAll();
	//memset(&fontConfigInfo, 0, sizeof(fontConfigInfo));
	if(!ParseApp()){
		TRACE(_T("Configuration ParseApp failed!\n"));
	}
	if(!ParseAutoRun()){
		TRACE(_T("Configuration ParseAutoRun failed!\n"));
	}
	if(!ParseFloatIcon()){
		TRACE(_T("Configuration ParseFloatIcon failed!\n"));
	}
	if(!ParseMenu()){
		TRACE(_T("Configuration ParseMenu failed!\n"));
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: ParseApp
功能: 解析[app]
*/
//////////////////////////////////////////////////////////////////////////
BOOL Configuration::ParseApp()
{
	TCHAR value[MAX_PATH];
	DWORD ret = 0;

	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("app"), _T("window_name"), _T("WND_FLOATTOOL"), value, MAX_PATH,m_IniFileName);
	wcscpy_s(windowName, sizeof(windowName), value);

	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("app"), _T("font_resource"), _T(""), value, wcslen(value),m_IniFileName);
	wcscpy_s(fontConfigInfo.fontFileName, sizeof(fontConfigInfo.fontFileName), value);

	TRACE(_T("[app] window_name='%s' font_resource='%s'\n"), windowName, fontConfigInfo.fontFileName);
	return true;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: ParseAutoRun
功能: 解析[autorun]
*/
//////////////////////////////////////////////////////////////////////////
BOOL Configuration::ParseAutoRun()
{
	TCHAR key[MAX_PATH];
	TCHAR value[MAX_PATH];
	DWORD ret = 0;
	int i;
	//提取自动运行命令
	for(i=0;;i++){
		memset(key, 0, MAX_PATH);
		memset(value, 0, MAX_PATH);
		swprintf_s(key, sizeof(key), _T("cmd%d"), i);

		CeGetPrivateProfileString(_T("autorun"), key, _T(""), value, MAX_PATH,m_IniFileName);
		if(wcslen(value)==0){
			break;
		}else{
			TRACE(_T("[autorun] %s=%s len=%d\n"), key, value, wcslen(value));
			AutoRunCmdStrArray.Add(value);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: ParseFloatIcon
功能: 解析[icon]
*/
//////////////////////////////////////////////////////////////////////////
BOOL Configuration::ParseFloatIcon()
{
	TCHAR value[MAX_PATH];
	DWORD ret = 0;
	//提取悬浮图标背景
	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("icon"), _T("background"), _T(""), value, wcslen(value),m_IniFileName);
	if(wcslen(value)==0){
		GetModuleFileName(AfxGetInstanceHandle(), iconConfigInfo.iconFileName, MAX_PATH);
		TCHAR *pos = wcsrchr (iconConfigInfo.iconFileName,'\\');
		if(pos!=NULL){
			*(pos+1)=0;
			wcscat_s(iconConfigInfo.iconFileName, sizeof(iconConfigInfo.iconFileName), _T("icon.bmp"));
		}
	}else{
		wcscpy_s(iconConfigInfo.iconFileName, sizeof(iconConfigInfo.iconFileName), value);
	}

	//提取悬浮图标属性配置
	iconConfigInfo.iconWidth = CeGetPrivateProfileInt(_T("icon"), _T("width"), 0, m_IniFileName);
	iconConfigInfo.iconHeight = CeGetPrivateProfileInt(_T("icon"), _T("height"), 0, m_IniFileName);
	if(iconConfigInfo.iconWidth<=0 || iconConfigInfo.iconHeight<=0){
		iconConfigInfo.iconWidth = iconConfigInfo.iconHeight = 48;
	}
	if(iconConfigInfo.iconWidth>xScreen){
		iconConfigInfo.iconWidth = xScreen;
	}
	if(iconConfigInfo.iconHeight>yScreen){
		iconConfigInfo.iconHeight = yScreen;
	}

	iconConfigInfo.iconPosX = CeGetPrivateProfileInt(_T("icon"), _T("x"), 0, m_IniFileName);
	iconConfigInfo.iconPosY = CeGetPrivateProfileInt(_T("icon"), _T("y"), 0, m_IniFileName);
	if(iconConfigInfo.iconPosX<0 || iconConfigInfo.iconPosY<0){
		iconConfigInfo.iconPosX = xScreen;iconConfigInfo.iconPosY = yScreen/3;
	}
	if(iconConfigInfo.iconPosX>xScreen-iconConfigInfo.iconWidth){
		iconConfigInfo.iconPosX = xScreen-iconConfigInfo.iconWidth;
	}
	if(iconConfigInfo.iconPosY>yScreen-iconConfigInfo.iconHeight){
		iconConfigInfo.iconPosY = yScreen-iconConfigInfo.iconHeight;
	}
	TRACE(_T("[icon] fileName=%s, width=%d height=%d, pos(%d,%d)\n"), iconConfigInfo.iconFileName, iconConfigInfo.iconWidth, iconConfigInfo.iconHeight, iconConfigInfo.iconPosX, iconConfigInfo.iconPosY);

	return true;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: ParseMenu
功能: 解析[menu]
*/
//////////////////////////////////////////////////////////////////////////
BOOL Configuration::ParseMenu()
{
	TCHAR key[MAX_PATH];
	TCHAR value[MAX_PATH];
	TCHAR cmd[MAX_PATH];
	TCHAR text[MAX_PATH];
	DWORD ret = 0;
	int i;
	//提取菜单选项命令
	for(i=0;;i++){
		//文本
		memset(key, 0, MAX_PATH);
		memset(cmd, 0, MAX_PATH);
		memset(text, 0, MAX_PATH);
		swprintf_s(key, sizeof(key), _T("item%d_text"), i);

		CeGetPrivateProfileString(_T("menu"), key, _T(""), text, MAX_PATH,m_IniFileName);
		menuItemTextStrArray.Add(text);

		//命令
		memset(key, 0, MAX_PATH);
		swprintf_s(key, sizeof(key), _T("item%d_cmd"), i);

		CeGetPrivateProfileString(_T("menu"), key, _T(""), cmd, MAX_PATH,m_IniFileName);

		if(wcslen(cmd)==0){
			break;
		}else{
			TRACE(_T("[menu] item%d [%s] {%s}\n"), i, text, cmd);
			menuItemCmdStrArray.Add(cmd);
		}
	}

	//提取菜单背景
	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("background"), _T(""), value, wcslen(value),m_IniFileName);
	if(wcslen(value)==0){
		GetModuleFileName(AfxGetInstanceHandle(), menuBgFileName, MAX_PATH);
		TCHAR *pos = wcsrchr (menuBgFileName,'\\');
		if(pos!=NULL){
			*(pos+1)=0;
			wcscat_s(menuBgFileName, sizeof(menuBgFileName), _T("background.bmp"));
		}
	}else{
		wcscpy_s(menuBgFileName, sizeof(menuBgFileName), value);
	}
	TRACE(_T("[menu] BgFileName='%s'\n"), menuBgFileName);

	//提取菜单选项属性配置
	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("layout"), _T("vertical"), value, wcslen(value),m_IniFileName);
	if(_wcsicmp(value, _T("vertical"))==0){
		menuLayout = LAYOUT_VERTICAL;
	}else{
		menuLayout = LAYOUT_HORIZONTAL;
	}

	//字体配置
	if(!m_lockFont){
		memset(value, 0, MAX_PATH);
		CeGetPrivateProfileString(_T("menu"), _T("font_name"), _T(""), value, wcslen(value),m_IniFileName);
		wcscpy_s(fontConfigInfo.lf.lfFaceName, sizeof(fontConfigInfo.lf.lfFaceName), value);

		fontConfigInfo.lf.lfHeight = CeGetPrivateProfileInt(_T("menu"), _T("font_size"), 0, m_IniFileName);
		if(fontConfigInfo.lf.lfHeight<=0){
			fontConfigInfo.lf.lfHeight = 0;
		}

		memset(value, 0, MAX_PATH);
		CeGetPrivateProfileString(_T("menu"), _T("font_color"), _T("0xFFFFFF"), value, t_strlen(value),m_IniFileName);
		fontConfigInfo.fontColor = _wtoi(value);

		memset(value, 0, MAX_PATH);
		CeGetPrivateProfileString(_T("menu"), _T("font_format"), _T("center"), value, t_strlen(value),m_IniFileName);
		if(_wcsicmp(value, _T("center"))==0){
			fontConfigInfo.fontFormat = DT_CENTER|DT_VCENTER;
		}else if(_wcsicmp(value, _T("left"))==0){
			fontConfigInfo.fontFormat = DT_LEFT|DT_VCENTER;
		}else if(_wcsicmp(value, _T("right"))==0){
			fontConfigInfo.fontFormat = DT_RIGHT|DT_VCENTER;
		}

		TRACE(_T("[menu] font file='%s' faceName='%s' charSet=%d width=%d color=%#x\n"), fontConfigInfo.fontFileName, fontConfigInfo.lf.lfFaceName, fontConfigInfo.lf.lfCharSet, fontConfigInfo.lf.lfHeight, fontConfigInfo.fontColor);
	}

	menuItemWidth = CeGetPrivateProfileInt(_T("menu"), _T("item_width"), 0, m_IniFileName);
	menuItemHeight = CeGetPrivateProfileInt(_T("menu"), _T("item_height"), 0, m_IniFileName);
	if(menuItemWidth<=0 || menuItemHeight<=0){
		menuItemWidth = xScreen/3;
		menuItemHeight = yScreen/10;
	}

	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_color_normal"), _T("0xAAAAAA"), value, t_strlen(value),m_IniFileName);
	menuItemColorNormal = _wtoi(value);

	memset(value, 0, MAX_PATH);
	CeGetPrivateProfileString(_T("menu"), _T("item_color_selected"), _T("0xAAAAAA"), value, t_strlen(value),m_IniFileName);
	menuItemColorSelected = _wtoi(value);

	if(menuItemColorNormal<0 || menuItemColorNormal>0xFFFFFF){
		menuItemColorNormal = RGB(170,170,170);
	}
	if(menuItemColorSelected<0 || menuItemColorSelected>0xFFFFFF){
		menuItemColorSelected = RGB(255,127,0);
	}
	TRACE(_T("[menu] layout=%d, width=%d height=%d, color(normal=%#x, selected=%#x)\n"),
		menuLayout, menuItemWidth, menuItemHeight, menuItemColorNormal, menuItemColorSelected);

	return true;
}

BOOL Configuration::reLoadConfig(LPCTSTR lpFileName)
{
	if(lpFileName==NULL){
		return ParseIniFile(m_IniFileName);
	}else{
		return ParseIniFile(lpFileName);
	}
}

Configuration * Configuration::GetInstance()
{
	if(m_pInstance == NULL){
		m_pInstance = new Configuration();
	}
	return m_pInstance;
}
