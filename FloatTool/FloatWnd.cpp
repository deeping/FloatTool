// TransparentWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "FloatWnd.h"
#include "Util.h"

// CFloatWnd

IMPLEMENT_DYNAMIC(CFloatWnd, CWnd)


CFloatWnd::CFloatWnd()
{
	int wndWidth;
	int wndHeight;
	TRACE(_T("CFloatWnd::CFloatWnd\n"));
	m_pcfg = Configuration::GetInstance();
	wndWidth = m_pcfg->iconConfigInfo.iconWidth;
	wndHeight = m_pcfg->iconConfigInfo.iconHeight;

	//��ʼ��������Ļ�Ҳ�
	CRect rect(m_pcfg->iconConfigInfo.iconPosX, m_pcfg->iconConfigInfo.iconPosY, m_pcfg->iconConfigInfo.iconPosX+wndWidth, m_pcfg->iconConfigInfo.iconPosY+wndHeight);
	CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(0),
		m_pcfg->windowName,
		WS_POPUP | WS_BORDER ,
		rect,
		NULL,
		NULL,
		NULL );

	//����ͼ��
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//SetTimer(1,1000,NULL);

	pMenuWnd = new CMenuWnd(this);
}

CFloatWnd::~CFloatWnd()
{
	delete pMenuWnd;
	TRACE(_T("CFloatWnd::~CFloatWnd\n"));
}


BEGIN_MESSAGE_MAP(CFloatWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CFloatWnd ��Ϣ�������

void CFloatWnd::OnTimer(UINT_PTR nIDEvent)
{     
	TRACE(_T("CDeskTopDlg::OnTimer\n"));
	if(!IsWindowVisible()){
		this->ShowWindow(SW_SHOW);
	}
}

void CFloatWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rect;
	GetClientRect(rect);

	if(m_bLBtnDown){
		//���㵱ǰ���밴�µ�֮���ƫ��
		int xOffset = point.x - m_ptDown.x;
		int yOffset = point.y - m_ptDown.y;

		if(abs(xOffset)>5||abs(yOffset)>5){
			DrawMoveRect(m_rcMoveRect);

			//���¼�������
			m_rcMoveRect.OffsetRect(xOffset,yOffset);

			//��������,���ⳬ����Ļ��Χ
			if( m_rcMoveRect.left<0){
				m_rcMoveRect.left = 0;
				m_rcMoveRect.right = rect.Width();
			}else if(m_rcMoveRect.left > m_pcfg->xScreen-rect.Width()){
				m_rcMoveRect.left = m_pcfg->xScreen - rect.Width();
				m_rcMoveRect.right = m_pcfg->xScreen;
			}
			if(m_rcMoveRect.top<0){
				m_rcMoveRect.top = 0;
				m_rcMoveRect.bottom=rect.Height();
			}else if(m_rcMoveRect.top>m_pcfg->yScreen-rect.Height()){
				m_rcMoveRect.top = m_pcfg->yScreen-rect.Height();
				m_rcMoveRect.bottom=m_pcfg->yScreen;
			}

			DrawMoveRect(m_rcMoveRect);
			m_ptDown = point;
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CFloatWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int x = m_rcMoveRect.left, y = m_rcMoveRect.top;
	CRect menuRect,overlapRect;
	CRect rect;
	GetClientRect(rect);
	overlapRect = m_wndOldRect&m_rcMoveRect;
	pMenuWnd->GetClientRect(&menuRect);

	ReleaseCapture();
	if(m_bLBtnDown){
		//��ԭ����û���ص����ƶ����ڣ�������ʾ�˵�
		TRACE(_T("CFloatWnd::OnLButtonUp overlapRect %d,%d\n"),overlapRect.Width(), overlapRect.Height());
		if(overlapRect.Width()<(rect.Width())||overlapRect.Height()<(rect.Height())){
			TRACE(_T("CFloatWnd::OnLButtonUp MoveWindow %d,%d\n"),m_rcMoveRect.left, m_rcMoveRect.top);
			MoveWindow(m_rcMoveRect);
		}else if(point.x>=0&&point.y>=0){
			TRACE(_T("CFloatWnd::OnLButtonUp click %d,%d,show menu!\n"),point.x, point.y);
			//�����˵���ʾλ�ã����ܳ�����Ļ
			if(menuRect.Width() > m_pcfg->xScreen){
				x = 0;
			}else if(x > m_pcfg->xScreen - menuRect.Width()){
				x = m_pcfg->xScreen - menuRect.Width()-2;
			}
			if(menuRect.Height() > m_pcfg->yScreen){
				y = 0;
			}else if(y >m_pcfg->yScreen-menuRect.Height()){
				y = m_pcfg->yScreen-menuRect.Height()-2;
			}

			//��ʾ�˵�
			pMenuWnd->SetWindowPos(this,x, y,0,0,SWP_NOSIZE|SWP_SHOWWINDOW);
		}else{
			TRACE(_T("CFloatWnd::OnLButtonUp point(%d,%d)\n"),point.x, point.y);
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
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()
	CRect rect;
	GetClientRect(rect);

	//��ͼ��
	if(!Util::LoadBitmapFile(m_pcfg->iconConfigInfo.iconFileName,dc)){
		CBrush backBrush;
		backBrush.CreateSolidBrush(RGB(113,129,137));
		CBrush* pOldBrush=dc.SelectObject(&backBrush); 
		dc.FillRect(&rect,&backBrush);
		dc.SelectObject(pOldBrush); 
		dc.DrawIcon(8,8,m_hIcon); 
	}
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
