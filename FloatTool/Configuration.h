#pragma once


class Configuration
{
protected:
	Configuration(void);

private:
	void ParseIniFile();
	TCHAR mIniFileName[_MAX_FNAME];
	static Configuration* pInstance;

public:
	~Configuration(void);

	static Configuration* GetInstance();

	CStringArray AutoRunCmdStrArray;
	CStringArray menuItemCmdStrArray;
	int menuItemWidth;
	int menuItemHeight;
	COLORREF menuItemColorNormal;
	COLORREF menuItemColorSelected;
	TCHAR menuBgFileName[_MAX_FNAME];
	TCHAR menuIconFileName[_MAX_FNAME];
	int xScreen;
	int yScreen;
};
