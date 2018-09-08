#pragma once


class Configuration
{
protected:
	Configuration(void);

private:
	BOOL ParseIniFile(LPCTSTR lpFileName);
	TCHAR mIniFileName[MAX_PATH];
	static Configuration* pInstance;

public:
	~Configuration(void);

	static Configuration* GetInstance();
	BOOL reLoadConfig(LPCTSTR lpFileName);

	TCHAR modulePath[MAX_PATH];
	CStringArray AutoRunCmdStrArray;
	CStringArray menuItemCmdStrArray;
	int menuLayout;
	int menuItemWidth;
	int menuItemHeight;
	COLORREF menuItemColorNormal;
	COLORREF menuItemColorSelected;
	TCHAR menuBgFileName[MAX_PATH];
	TCHAR iconFileName[MAX_PATH];
	int iconWidth;
	int iconHeight;
	int iconPosX;
	int iconPosY;
	int xScreen;
	int yScreen;
};
