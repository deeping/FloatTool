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
	TCHAR menuBackFileName[_MAX_FNAME];
	int xScreen;
	int yScreen;
};
