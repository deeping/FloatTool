#pragma once
#include "Configuration.h"

// CMenuWnd

class CMenuWnd : public CWnd
{
	DECLARE_DYNAMIC(CMenuWnd)

public:
	CMenuWnd(CWnd* pParentWnd);
	virtual ~CMenuWnd();

protected:
	DECLARE_MESSAGE_MAP()
	HICON m_hIcon;

private:
	void DrawItemFrame(int index,CDC& dc, COLORREF crColor);
	int PointToSelectItem(CPoint point);
	void OnReloadCfg();
	void OnMenuItemSelected(int item);
	void UpdateSelectedFrame(int item);
	BOOL GetPathFromCmdParam(LPCTSTR cmd, TCHAR *path);
	BOOL ParseAndExecCommand(LPCTSTR cmd);
	int m_mnItemCount;
	int m_mnSelectedItem;
	Configuration* m_pcfg;

public:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


