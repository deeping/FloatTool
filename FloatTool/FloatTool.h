// FloatTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#ifdef STANDARDSHELL_UI_MODEL
#include "resource.h"
#endif

// CFloatToolApp:
// �йش����ʵ�֣������ FloatTool.cpp
//

class CFloatToolApp : public CWinApp
{
private:
	BOOL DoReloadCommand(LPCTSTR cmd);
	void DoAutoRunCommands();
	void AddCustomFontResource();
	void RemoveCustomFontResource();

public:
	CFloatToolApp();
	
// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFloatToolApp theApp;
