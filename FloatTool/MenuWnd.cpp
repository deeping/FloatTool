// MenuWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "FloatTool.h"
#include "MenuWnd.h"
#include "Configuration.h"
#include "Util.h"


// CMenuWnd

IMPLEMENT_DYNAMIC(CMenuWnd, CWnd)


CMenuWnd::CMenuWnd(CWnd* pParentWnd)
{
	m_pcfg = Configuration::GetInstance();
	TRACE(_T("CMenuWnd::CMenuWnd Item Width=%d,Height=%d,count=%d\n"),
		m_pcfg->menuItemHeight, m_pcfg->menuItemWidth, m_pcfg->menuItemCmdStrArray.GetCount());

	int wndHight = m_pcfg->menuLayout==0?m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemHeight:m_pcfg->menuItemHeight;
	int	wndWidth = m_pcfg->menuLayout==0?m_pcfg->menuItemWidth:m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemWidth;
	if(wndHight<=0)wndHight = 48;
	if(wndWidth<=0)wndWidth = 48;

	CPoint point(m_pcfg->xScreen,m_pcfg->yScreen);
	if(point.x > m_pcfg->xScreen-wndWidth){
		point.x = m_pcfg->xScreen-wndWidth;
	}

	if(point.y > m_pcfg->yScreen-wndHight){
		point.y = m_pcfg->yScreen-wndHight;
	}

	TRACE(_T("CMenuWnd::CMenuWnd Width=%d,Hight=%d\n"), wndWidth, wndHight);

	CRect rect(point.x, point.y, point.x+wndWidth, point.y+wndHight);
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
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CMenuWnd 消息处理程序

void CMenuWnd::OnTimer(UINT_PTR nIDEvent)
{     
	TRACE(_T("CMenuWnd::OnTimer\n"));

	//截屏
	TCHAR bmpFileName[MAX_PATH]={0};
	Util::GetBmpFilePathFromTime(bmpFileName, sizeof(bmpFileName));
	TRACE(_T("[screenshot] %s\n"), bmpFileName);
	Util::SaveHwndToBmpFile(NULL,bmpFileName);

	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	KillTimer(1);
}

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
		backBrush.CreateSolidBrush(RGB(113,129,137));
		CBrush* pOldBrush=dc.SelectObject(&backBrush); 
		dc.FillRect(&rect,&backBrush);
		dc.SelectObject(pOldBrush); 
	}

	//画选项
	for(int i=0;i<=m_pcfg->menuItemCmdStrArray.GetCount();i++){
		DrawItemFrame(i,dc,m_pcfg->menuItemColorNormal);
	}

	if(m_pcfg->menuItemCmdStrArray.GetCount()==0){
		DrawItemFrame(0,dc,m_pcfg->menuItemColorNormal);
		CFont font,*pOldFont;
		int fontSize=m_pcfg->menuItemHeight>m_pcfg->menuItemWidth?m_pcfg->menuItemWidth:m_pcfg->menuItemHeight;
		font.CreateFont(
			fontSize, // nHeight
			fontSize, // nWidth
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
	TRACE(_T("CMenuWnd::OnLButtonDown (%d,%d)\n"),point.x, point.y);
	int i=PointToSelectItem(point);
	UpdateSelectedFrame(i);

	CWnd::OnLButtonDown(nFlags, point);
}

void CMenuWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int i=PointToSelectItem(point);
	UpdateSelectedFrame(i);

	CWnd::OnMouseMove(nFlags, point);
}

void CMenuWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACE(_T("CMenuWnd::OnLButtonUp select %d/%d\n"), m_mnSelectedItem, m_pcfg->menuItemCmdStrArray.GetCount());

	if(m_mnSelectedItem < m_pcfg->menuItemCmdStrArray.GetCount()&& m_mnSelectedItem >=0){
		OnMenuItemSelected(m_mnSelectedItem);
	}else{
		TRACE(_T("CMenuWnd::OnLButtonUp item%d is invalid!\n"), m_mnSelectedItem);
		if(m_pcfg->menuItemCmdStrArray.GetCount()<=0){
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
		}
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMenuWnd::DrawItemFrame(int index,CDC& dc, COLORREF crColor)
{
	if(index<0){
		TRACE(_T("CMenuWnd::DrawItemFrame item%d,skip!\n"), index);
		return;
	}

	int bordeWidth = 1;
	POINT topLeft;
	POINT bottomRight;
	if(m_pcfg->menuLayout==0){
		topLeft.x=bordeWidth;
		topLeft.y=m_pcfg->menuItemHeight*index+bordeWidth;
		bottomRight.x=m_pcfg->menuItemWidth-(bordeWidth*2);
		bottomRight.y=m_pcfg->menuItemHeight*(index+1)-(bordeWidth*2);
	}else{
		topLeft.x=m_pcfg->menuItemWidth*index+bordeWidth;;
		topLeft.y=bordeWidth;
		bottomRight.x=m_pcfg->menuItemWidth*(index+1)-(bordeWidth*2);
		bottomRight.y=m_pcfg->menuItemHeight-(bordeWidth*2);
	}

	CRect cmnItemRect(topLeft, bottomRight);
	CBrush itemBrush;
	itemBrush.CreateSolidBrush(crColor);
	CBrush* pOldBrush=dc.SelectObject(&itemBrush); 

	dc.FrameRect(&cmnItemRect, &itemBrush);

	dc.SelectObject(pOldBrush); 
}

void CMenuWnd::UpdateSelectedFrame(int item)
{
	if(item!=m_mnSelectedItem){
		CClientDC dc(this);
		DrawItemFrame(m_mnSelectedItem, dc, m_pcfg->menuItemColorNormal);
		m_mnSelectedItem = item;
		DrawItemFrame(m_mnSelectedItem, dc, m_pcfg->menuItemColorSelected);
	}
}

int CMenuWnd::PointToSelectItem(CPoint point)
{
	CRect rect(0,0,m_pcfg->menuItemWidth,m_pcfg->menuItemHeight);
	//TRACE(_T("CMenuWnd::PointToSelectItem (%d,%d)\n"),point.x, point.y);

	for(int i=0;i<m_pcfg->menuItemCmdStrArray.GetCount();i++){
		if(m_pcfg->menuLayout==0){
			rect.left=0;
			rect.top = m_pcfg->menuItemHeight*i;
			rect.right=m_pcfg->menuItemWidth;
			rect.bottom = m_pcfg->menuItemHeight*(i+1);
		}else{
			rect.left=m_pcfg->menuItemWidth*i;
			rect.top = 0;
			rect.right=m_pcfg->menuItemWidth*(i+1);
			rect.bottom = m_pcfg->menuItemHeight;
		}

		if(rect.PtInRect(point)){
			return i;
			break;
		}
	}
	return -1;
}

void CMenuWnd::OnMenuItemSelected(int item)
{
	TCHAR cmd[MAX_PATH]={0};
	TCHAR *pos = NULL;
	int count=0;

	wcscpy_s(cmd, sizeof(cmd), m_pcfg->menuItemCmdStrArray.GetAt(item));
	while(1){
		pos=wcschr(cmd,_T('|'));
		if(pos==NULL){
			if(!ParseAndExecCommand(cmd)){
				TRACE(_T("run item%d cmd%d '%s' failed!\n"), item, count, cmd);
				TCHAR msgBuffer[MAX_PATH];
				wsprintf(msgBuffer, _T("run item%d cmd%d '%s' failed!"), item, count, cmd);
				AfxMessageBox(msgBuffer);
			}else{
				TRACE(_T("run item%d cmd%d '%s' succeed!\n"), item, count, cmd);
			}
			break;
		}
		*pos=0;
		count++;

		if(!ParseAndExecCommand(cmd)){
			TRACE(_T("run item%d cmd%d '%s' failed!\n"), item, count, cmd);
			TCHAR msgBuffer[MAX_PATH];
			wsprintf(msgBuffer, _T("run item%d cmd%d '%s' failed!"), item, count, cmd);
			AfxMessageBox(msgBuffer);
		}else{
			TRACE(_T("run item%d cmd%d '%s' succeed!\n"), item, count, cmd);
		}

		pos++;
		wcsncpy_s(cmd, sizeof(cmd), pos, wcslen(pos)+1);
	}
}

void CMenuWnd::OnReloadCfg()
{
	int wndHight = m_pcfg->menuLayout==0?m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemHeight:m_pcfg->menuItemHeight;
	int	wndWidth = m_pcfg->menuLayout==0?m_pcfg->menuItemWidth:m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemWidth;

	CRect rect;
	GetWindowRect(rect);
	MoveWindow(rect.left,rect.top,wndWidth,wndHight,false);
	RedrawWindow();

	AfxGetMainWnd()->GetWindowRect(&rect);
	AfxGetMainWnd()->MoveWindow(rect.left,rect.top,m_pcfg->iconWidth,m_pcfg->iconHeight,false);
	AfxGetMainWnd()->RedrawWindow();
}

BOOL CMenuWnd::ParseAndExecCommand(LPCTSTR cmd)
{
	ASSERT(NULL != cmd);
	int count=0;

	TRACE(_T("CMenuWnd::ParseAndExecCommand:'%s'\n"), cmd);

	if(_wcsicmp(cmd, _T("<null>"))==0){
		TRACE(_T("cmd is '%s' do nothing!\n"), cmd);
	}else if(_wcsicmp(cmd, _T("<exit>"))==0){
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}else if(_wcsicmp(cmd, _T("<taskbar>"))==0){
		Util::HideOrShowTaskBar();
	}else if(_wcsicmp(cmd, _T("<hide>"))==0){
		ShowWindow(SW_HIDE);
	}else if(_wcsicmp(cmd, _T("<show>"))==0){
		ShowWindow(SW_SHOW);
	}else if(wcsstr(cmd, _T("<sleep"))!=NULL){
		int sleepMs=0;
		count = swscanf_s(cmd, _T("<sleep%*[^0-9]%d>"),&sleepMs);
		TRACE(_T("count=%d\n"), count);
		if(count==1){
			TRACE(_T("sleep %dms\n"), sleepMs);
			Sleep(sleepMs);
			return true;
		}else{
			return false;
		}
	}else if(wcsstr(cmd, _T("<reload"))!=NULL){
		if(!Util::DoReloadCommand(cmd)){
			return false;
		}
		OnReloadCfg();
	}else if(_wcsicmp(cmd, _T("<screenshot>"))==0){
		//截屏前先隐藏当前程序窗口
		ShowWindow(SW_HIDE);
		AfxGetMainWnd()->ShowWindow(SW_HIDE);

		//1秒后开始截图
		SetTimer(1,1000,NULL);
	}else if(_wcsicmp(cmd, _T(""))!=0){
		return Util::ExecuteExCommand(cmd);
	}

	return true;
}