#pragma once

class Util
{
public:
	static DWORD RelativeToAbsolutePath(LPCTSTR rel, LPTSTR abs, int len);
	static bool LoadBitmapFile(LPCTSTR szFIleName,CDC& dc);
	static BOOL DoReloadCommand(LPCTSTR cmd);
	static BOOL ExecuteExCommand(LPCTSTR lpFile);
	static void HideOrShowTaskBar();
	static void GetModulePath(TCHAR* spPath, int len);
	static void GetBmpFilePathFromTime(TCHAR* spFilePath, int len);
	static BOOL SaveHwndToBmpFile(HWND hWnd, LPCTSTR lpszPath);
};
