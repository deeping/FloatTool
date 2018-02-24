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
	void UpdateSelectItem(CPoint point);
	int m_wndWidth;
	int m_wndHight;
	int m_mnItemCount;
	int m_mnSelectedItem;
	int menuItemWidth;
	int m_mnItemHight;
	int xScreen;
	int yScreen;
	COLORREF m_ColorNormal;
	COLORREF m_ColorSelected;
	COLORREF m_ColorBackground;
	Configuration* m_pcfg;


public:
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


