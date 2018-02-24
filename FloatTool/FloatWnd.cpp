// TransparentWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "FloatWnd.h"

// CFloatWnd

IMPLEMENT_DYNAMIC(CFloatWnd, CWnd)


CFloatWnd::CFloatWnd()
{
	TRACE(_T("CFloatWnd::CFloatWnd\n"));
	xScreen = ::GetSystemMetrics( SM_CXSCREEN );
	yScreen = ::GetSystemMetrics( SM_CYSCREEN );
	m_wndHeight = m_wndWidth = 48;

	//初始化区域，屏幕右侧
	CRect rect(xScreen-m_wndWidth, (yScreen/2)-m_wndHeight, xScreen, yScreen/2);
	CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(0),
		_T("CFloatWnd"),
		WS_POPUP | WS_BORDER  ,
		rect,
		NULL,
		NULL,
		NULL );

	//设置图标
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	pMenuWnd = new CMenuWnd(this);
}

CFloatWnd::~CFloatWnd()
{
	delete pMenuWnd;
	TRACE(_T("CFloatWnd::~CFloatWnd\n"));
}


BEGIN_MESSAGE_MAP(CFloatWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CFloatWnd 消息处理程序

void CFloatWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TRACE(_T("CFloatWnd::OnLButtonDown %d,%d\n"), point.x, point.y);

	GetWindowRect(&m_wndOldRect);
	m_rcMoveRect = m_wndOldRect;

	SetCapture();

	m_ptDown = point;
	m_bLBtnDown = TRUE;

	DrawMoveRect(m_rcMoveRect);

	CWnd::OnLButtonDown(nFlags, point);
}

void CFloatWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bLBtnDown){
		//计算当前点与按下点之间的偏移
		int xOffset = point.x - m_ptDown.x;
		int yOffset = point.y - m_ptDown.y;

		if(abs(yOffset)>5||abs(yOffset)>5){
			DrawMoveRect(m_rcMoveRect);

			//重新计算区域
			m_rcMoveRect.OffsetRect(xOffset,yOffset);

			//修正区域,避免超出屏幕范围
			if( m_rcMoveRect.left<0){
				m_rcMoveRect.left = 0;
				m_rcMoveRect.right = m_wndWidth;
			}else if(m_rcMoveRect.left>xScreen-m_wndWidth){
				m_rcMoveRect.left = xScreen-m_wndWidth;
				m_rcMoveRect.right = xScreen;
			}
			if(m_rcMoveRect.top<0){
				m_rcMoveRect.top = 0;
				m_rcMoveRect.bottom=m_wndHeight;
			}else if(m_rcMoveRect.top>yScreen-m_wndHeight){
				m_rcMoveRect.top = yScreen-m_wndHeight;
				m_rcMoveRect.bottom=yScreen;
			}

			DrawMoveRect(m_rcMoveRect);
			m_ptDown = point;
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CFloatWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int x = m_rcMoveRect.left, y = m_rcMoveRect.top;
	CRect clientRect,overlapRect;
	overlapRect = m_wndOldRect&m_rcMoveRect;
	pMenuWnd->GetClientRect(&clientRect);

	ReleaseCapture();
	if(m_bLBtnDown){
		//与原区域没有重叠则移动窗口，否则显示菜单
		if(overlapRect.Width()==0){
			TRACE(_T("MoveWindow %d,%d\n"),m_rcMoveRect.left, m_rcMoveRect.top);
			MoveWindow(m_rcMoveRect);
		}else if(clientRect.PtInRect(point)){
			TRACE(_T("click %d,%d,show menu!\n"),point.x, point.y);
			if(overlapRect.Width()>0){
				//修正菜单显示位置，不能超出屏幕
				if(x > xScreen-clientRect.Width()){
					x = xScreen-clientRect.Width();
				}
				if(y >yScreen-clientRect.Height()){
					y = yScreen-clientRect.Height();
				}
			}
			pMenuWnd->SetWindowPos(this,x, y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}
	m_bLBtnDown = FALSE;
	DrawMoveRect(m_rcMoveRect);

	CWnd::OnLButtonUp(nFlags, point);
}

void CFloatWnd::OnPaint()
{
	TRACE(_T("CFloatWnd::OnPaint\n"));
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	CRect rect;
	GetClientRect(rect);

	//画背景
	CBrush backBrush;
	backBrush.CreateSolidBrush(RGB(113,129,137));
	CBrush* pOldBrush=dc.SelectObject(&backBrush);
	dc.FillRect(&rect,&backBrush);
	dc.SelectObject(pOldBrush); 

	//画图标
	dc.DrawIcon(8,8,m_hIcon);
}

void CFloatWnd::DrawMoveRect(CRect &rect)
{
	//TRACE(_T("CFloatWnd::DrawMoveRect\n"));
	HDC hScreenDc = ::GetDC(NULL);
	int nOldRop = SetROP2(hScreenDc, R2_XORPEN);
	HPEN hOldPen = (HPEN)SelectObject(hScreenDc, (HPEN)WHITE_PEN);

	DrawFocusRect(hScreenDc, &rect);
	SetROP2(hScreenDc, nOldRop);
	SelectObject(hScreenDc, hOldPen);
	::ReleaseDC(NULL, hScreenDc);
}
