#pragma once

#define LAYOUT_VERTICAL 0
#define LAYOUT_HORIZONTAL 1

typedef struct FontConfigInfo
{
	TCHAR fontFileName[MAX_PATH];
	LOGFONTW lf;
	COLORREF fontColor;
	UINT fontFormat;
} FontConfigInfo;

typedef struct IconConfigInfo
{
	TCHAR iconFileName[MAX_PATH];
	int iconWidth;
	int iconHeight;
	int iconPosX;
	int iconPosY;
} IconConfigInfo;

class Configuration
{
protected:
	Configuration(void);

private:
	BOOL ParseIniFile(LPCTSTR lpFileName);
	BOOL ParseIniFile();
	BOOL ParseApp();
	BOOL ParseAutoRun();
	BOOL ParseFloatIcon();
	BOOL ParseMenu();
	static Configuration* m_pInstance;
	BOOL m_lockFont;

public:
	~Configuration(void);

	static Configuration* GetInstance();
	BOOL reLoadConfig(LPCTSTR lpFileName);
	int SetLockFont(BOOL lock);
	TCHAR m_IniFileName[MAX_PATH];
	TCHAR windowName[MAX_PATH];
	FontConfigInfo fontConfigInfo;
	IconConfigInfo iconConfigInfo;
	CStringArray AutoRunCmdStrArray;
	CStringArray menuItemCmdStrArray;
	CStringArray menuItemTextStrArray;
	int menuLayout;
	int menuItemWidth;
	int menuItemHeight;
	COLORREF menuItemColorNormal;
	COLORREF menuItemColorSelected;
	TCHAR menuBgFileName[MAX_PATH];

	int xScreen;
	int yScreen;
};
