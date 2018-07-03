// MenuWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "FloatTool.h"
#include "MenuWnd.h"
#include "Configuration.h"
#include "IniFile.h"
#include "Util.h"


// CMenuWnd

IMPLEMENT_DYNAMIC(CMenuWnd, CWnd)


CMenuWnd::CMenuWnd(CWnd* pParentWnd)
{
	TRACE(_T("CMenuWnd::CMenuWnd\n"));
	m_pcfg = Configuration::GetInstance();
	TRACE(_T("CMenuWnd::CMenuWnd Item menuItemWidth=%d,menuItemHeight=%d,count=%d\n"), m_pcfg->menuItemHeight, m_pcfg->menuItemWidth, m_pcfg->menuItemCmdStrArray.GetCount());

	m_ColorNormal = m_pcfg->menuItemColorNormal;
	m_ColorSelected = m_pcfg->menuItemColorSelected;
	m_ColorBackground = RGB(113,129,137);

	xScreen = m_pcfg->xScreen;
	yScreen = m_pcfg->yScreen;

	CPoint point(xScreen,yScreen);

	m_mnItemCount = m_pcfg->menuItemCmdStrArray.GetCount()+1;
	m_mnSelectedItem = -1;
	menuItemWidth = m_pcfg->menuItemWidth;
	m_mnItemHight = m_pcfg->menuItemHeight;
	m_wndHight = m_mnItemCount*m_mnItemHight;
	m_wndWidth = menuItemWidth;

	if(point.x > xScreen-m_wndWidth){
		point.x = xScreen-m_wndWidth;
	}

	if(point.y > yScreen-m_wndHight){
		point.y = yScreen-m_wndHight;
	}

	TRACE(_T("CMenuWnd::CMenuWnd Width=%d,Hight=%d\n"), m_wndWidth, m_wndHight);

	CRect rect(point.x, point.y, point.x+m_wndWidth, point.y+m_wndHight);
	CreateEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		AfxRegisterWndClass(0),
		_T("CMenuWnd"),
		WS_POPUP | WS_BORDER ,
		rect,
		pParentWnd,
		NULL,
		NULL );
}

CMenuWnd::~CMenuWnd()
{
	TRACE(_T("CMenuWnd::~CMenuWnd\n"));
}


BEGIN_MESSAGE_MAP(CMenuWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CMenuWnd 消息处理程序


void CMenuWnd::OnPaint()
{
	TRACE(_T("CMenuWnd::OnPaint\n"));
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	CRect rect;
	GetClientRect(rect);

	dc.SetBkMode(TRANSPARENT);

	//画背景
	if(!Util::LoadBitmapFile(m_pcfg->menuBgFileName,dc)){
		CBrush backBrush;
		backBrush.CreateSolidBrush(m_ColorBackground);
		CBrush* pOldBrush=dc.SelectObject(&backBrush); 
		dc.FillRect(&rect,&backBrush);
		dc.SelectObject(pOldBrush); 
	}

	//画选项
	for(int i=0;i<=m_mnItemCount;i++){
		DrawItemFrame(i,dc,m_ColorNormal);
	}

	if(m_mnItemCount==1){
		CFont font,*pOldFont;
		font.CreateFont(
			46, // nHeight
			46, // nWidth
			0, // nEscapement
			0, // nOrientation
			FW_NORMAL, // nWeight
			FALSE, // bItalic
			FALSE, // bUnderline
			0, // cStrikeOut
			ANSI_CHARSET, // nCharSet
			OUT_DEFAULT_PRECIS, // nOutPrecision
			CLIP_DEFAULT_PRECIS, // nClipPrecision
			DEFAULT_QUALITY, // nQuality
			DEFAULT_PITCH | FF_SWISS,
			_T("Arial") // nPitchAndFamily Arial
			); 
		pOldFont = dc.SelectObject(&font);
		dc.DrawTextW(_T("Exit"),&rect,DT_CENTER);
		dc.SelectObject(pOldFont);
	}

	m_mnSelectedItem = -1;
}

void CMenuWnd::OnKillFocus(CWnd* pNewWnd)
{
	TRACE(_T("CMenuWnd::OnKillFocus\n"));
	CWnd::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码

	ShowWindow(SW_HIDE);
}

void CMenuWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateSelectItem(point);

	CWnd::OnLButtonDown(nFlags, point);
}

void CMenuWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	UpdateSelectItem(point);

	CWnd::OnMouseMove(nFlags, point);
}

void CMenuWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACE(_T("CMenuWnd::OnLButtonUp select %d\n"), m_mnSelectedItem);
	SHELLEXECUTEINFO   ExecuteInfo;

	if(m_mnSelectedItem < m_pcfg->menuItemCmdStrArray.GetCount()){
		TRACE(_T("CMenuWnd::OnLButtonUp %d/%d run %s\n"), m_mnSelectedItem, m_pcfg->menuItemCmdStrArray.GetCount(),m_pcfg->menuItemCmdStrArray.GetAt(m_mnSelectedItem));

		if(t_strcmp(m_pcfg->menuItemCmdStrArray.GetAt(m_mnSelectedItem), _T("null"))!=0){
			ExecuteInfo.cbSize = sizeof(ExecuteInfo);
			ExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;  
			ExecuteInfo.hwnd = NULL;
			ExecuteInfo.lpVerb = NULL;   
			ExecuteInfo.lpFile = m_pcfg->menuItemCmdStrArray.GetAt(m_mnSelectedItem);
			ExecuteInfo.lpParameters = NULL;
			ExecuteInfo.lpDirectory = NULL;
			ExecuteInfo.nShow = SW_HIDE; 

			BOOL bResult = ShellExecuteEx(&ExecuteInfo); 
			if(!bResult && (int)ExecuteInfo.hInstApp <= 32){    
				TCHAR msgBuffer[MAX_PATH];
				wsprintf(msgBuffer, _T("run item%d %s failed!"), m_mnSelectedItem, ExecuteInfo.lpFile);
				AfxMessageBox(msgBuffer);
			}
		}
	}else{
		TRACE(_T("CMenuWnd::OnLButtonUp selected item%d exit\n"), m_mnSelectedItem);
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMenuWnd::DrawItemFrame(int index,CDC& dc, COLORREF crColor)
{
	//TRACE(_T("CMenuWnd::DrawItemFrame item%d\n"), index);
	int bordeWidth = 1;
	CRect cmnItemRect(bordeWidth, m_mnItemHight*index+bordeWidth, menuItemWidth-(bordeWidth*2), m_mnItemHight*(index+1)-(bordeWidth*2));
	CBrush itemBrush;
	itemBrush.CreateSolidBrush(crColor);
	CBrush* pOldBrush=dc.SelectObject(&itemBrush); 

	dc.FrameRect(&cmnItemRect, &itemBrush);

	dc.SelectObject(pOldBrush); 
}

void CMenuWnd::UpdateSelectItem(CPoint point)
{
	CRect rect(0,0,menuItemWidth,m_mnItemHight);
	int i;
	for(i=0;i<=m_mnItemCount;i++){
		rect.top = m_mnItemHight*i;
		rect.bottom = m_mnItemHight*(i+1);
		if(rect.PtInRect(point)){
			break;
		}
	}

	if(i!=m_mnSelectedItem){
		CClientDC dc(this);
		DrawItemFrame(m_mnSelectedItem, dc, m_ColorNormal);
		m_mnSelectedItem = i;
		DrawItemFrame(m_mnSelectedItem, dc, m_ColorSelected);
	}
}

