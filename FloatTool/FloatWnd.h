#pragma once
#include "MenuWnd.h"
#include "Configuration.h"

// CFloatWnd

class CFloatWnd : public CWnd
{
	DECLARE_DYNAMIC(CFloatWnd)

public:
	CFloatWnd();
	void DrawMoveRect(CRect &rect);
	virtual ~CFloatWnd();

protected:
	DECLARE_MESSAGE_MAP()
	HICON m_hIcon;
	unsigned short m_BitmapID;
	bool m_bLBtnDown;
	CRect m_rcMoveRect;
	CRect m_wndOldRect;
	CPoint m_ptDown;
	int m_wndWidth;
	int m_wndHeight;
	int xScreen;
	int yScreen;
	CMenuWnd* pMenuWnd;
	Configuration* m_pcfg;
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


