#pragma once

class Util
{
public:
	static bool LoadBitmapFile(LPCTSTR szFIleName,CDC& dc);
	static BOOL ExecuteExCommand(LPCTSTR lpFile);
	static void HideOrShowTaskBar();
	static void GetModulePath(TCHAR* spPath, int len);
	static void GetBmpFilePathFromTime(TCHAR* spFilePath, int len);
	static BOOL SaveHwndToBmpFile(HWND hWnd, LPCTSTR lpszPath);
};
