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

	int wndHight = m_pcfg->menuLayout==LAYOUT_VERTICAL?m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemHeight:m_pcfg->menuItemHeight;
	int	wndWidth = m_pcfg->menuLayout==LAYOUT_VERTICAL?m_pcfg->menuItemWidth:m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemWidth;
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
		m_pcfg->windowName,
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

INT CALLBACK EnumFontNameProc(LOGFONT *plf, TEXTMETRIC* ptm, INT nFontType, LPARAM lParam)
{
	TRACE(_T("fontName=%s charset=%d\n"), plf->lfFaceName, ptm->tmCharSet);

	CStringArray* sysFonts = (CStringArray*) lParam;
	if (sysFonts != NULL)
	{
		sysFonts->Add(CString(plf->lfFaceName));
	}

	Configuration *pcfg = Configuration::GetInstance();
	if(wcscmp(plf->lfFaceName,pcfg->fontConfigInfo.lf.lfFaceName)==0){
		TRACE(_T("EnumFontNameProc find font '%s',set charSet=%d\n"), plf->lfFaceName, ptm->tmCharSet);
		pcfg->fontConfigInfo.lf.lfCharSet = ptm->tmCharSet;
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CMenuWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CMenuWnd 消息处理程序
// CFloatWnd 消息处理程序
int CMenuWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	TRACE(_T("CMenuWnd::OnCreate\n"));

	//更新字体编码
	HDC hdc = ::GetDC(*this);
	::EnumFontFamilies(hdc, (LPTSTR) NULL, (FONTENUMPROC)EnumFontNameProc,NULL);
	::ReleaseDC(*this, hdc);

	return 0;
}

void CMenuWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACE(_T("CMenuWnd::OnClose\n"));

	CWnd::OnClose();
}

void CMenuWnd::OnTimer(UINT_PTR nIDEvent)
{     
	TRACE(_T("CMenuWnd::OnTimer %#x\n"), nIDEvent);

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
	GetWindowRect(rect);
	TRACE(_T("GetWindowRect %d,%d,%d,%d\n"), rect.left,rect.top, rect.right, rect.bottom);
	GetClientRect(&rect);

	//画背景
	if(!Util::LoadBitmapFile(m_pcfg->menuBgFileName,dc)){
		CBrush backBrush;
		backBrush.CreateSolidBrush(RGB(0,0,0));
		CBrush* pOldBrush=dc.SelectObject(&backBrush); 
		dc.FillRect(&rect,&backBrush);
		dc.SelectObject(pOldBrush); 
	}

	//画选项
	HFONT hfont = CreateFontIndirect(&m_pcfg->fontConfigInfo.lf);
	HFONT oldfont = (HFONT)SelectObject(dc,hfont);
	COLORREF oldTextColor = dc.SetTextColor(m_pcfg->fontConfigInfo.fontColor);
	int oldBkMode = dc.SetBkMode(TRANSPARENT);

	TCHAR lfFaceName[LF_FACESIZE];
	if(GetTextFace(dc,sizeof(lfFaceName), lfFaceName)){
		TRACE(_T("lfFaceName=%s\n"), lfFaceName);
	}
	TEXTMETRIC tm;
	if(GetTextMetrics(dc,&tm)){
		TRACE(_T("tmCharSet=%d tmHeight=%d\n"), tm.tmCharSet, tm.tmHeight);
	}

	if(m_pcfg->menuItemCmdStrArray.GetCount()==0){
		DrawItemFrame(0,dc,m_pcfg->menuItemColorNormal);
		GetItemRect(0,&rect);
		rect.DeflateRect(4,4);
		dc.DrawTextW(_T("exit"), &rect, m_pcfg->fontConfigInfo.fontFormat);	
	}else{
		for(int i=0;i<m_pcfg->menuItemCmdStrArray.GetCount();i++){
			DrawItemFrame(i,dc,m_pcfg->menuItemColorNormal);
			GetItemRect(i,&rect);
			rect.DeflateRect(4,4);
			dc.DrawTextW(m_pcfg->menuItemTextStrArray.GetAt(i), &rect, m_pcfg->fontConfigInfo.fontFormat);	
		}
	}
	dc.SetBkMode(oldBkMode);
	dc.SetTextColor(oldTextColor );
	dc.SelectObject(oldfont);
	DeleteObject(hfont);

	m_selectedItem = -1;
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
	TRACE(_T("CMenuWnd::OnLButtonUp select %d/%d\n"), m_selectedItem, m_pcfg->menuItemCmdStrArray.GetCount());

	if(m_selectedItem < m_pcfg->menuItemCmdStrArray.GetCount()&& m_selectedItem >=0){
		OnMenuItemSelected(m_selectedItem);
	}else{
		TRACE(_T("CMenuWnd::OnLButtonUp item%d is invalid!\n"), m_selectedItem);
		if(m_pcfg->menuItemCmdStrArray.GetCount()<=0){
			AfxGetMainWnd()->SendMessage(WM_CLOSE);
		}
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMenuWnd::GetItemRect(int index,LPRECT lpRect)
{
	ASSERT(NULL != lpRect);

	if(index<0){
		TRACE(_T("CMenuWnd::GetItemRect item%d invalid!\n"), index);
		return;
	}

	int left, top, right, bottom;

	if(m_pcfg->menuLayout==LAYOUT_VERTICAL){
		left=0;
		top=m_pcfg->menuItemHeight*index;
		right=m_pcfg->menuItemWidth;
		bottom=m_pcfg->menuItemHeight*(index+1);
	}else{
		left=m_pcfg->menuItemWidth*index;
		top=0;
		right=m_pcfg->menuItemWidth*(index+1);
		bottom=m_pcfg->menuItemHeight;
	}
	lpRect->left=left;
	lpRect->top=top;
	lpRect->right=right;
	lpRect->bottom=bottom;
}

void CMenuWnd::DrawItemFrame(int index,CDC& dc, COLORREF crColor)
{
	if(index<0){
		TRACE(_T("CMenuWnd::DrawItemFrame item%d,skip!\n"), index);
		return;
	}

	CRect rect;
	GetItemRect(index, &rect);
	rect.DeflateRect(2,2);

	CBrush brush;
	brush.CreateSolidBrush(crColor);
	CBrush* pOldBrush=dc.SelectObject(&brush); 

	dc.FrameRect(&rect, &brush);
	dc.SelectObject(pOldBrush); 
}

void CMenuWnd::UpdateSelectedFrame(int item)
{
	if(item!=m_selectedItem){
		CClientDC dc(this);
		DrawItemFrame(m_selectedItem, dc, m_pcfg->menuItemColorNormal);
		m_selectedItem = item;
		DrawItemFrame(m_selectedItem, dc, m_pcfg->menuItemColorSelected);
	}
}

int CMenuWnd::PointToSelectItem(CPoint point)
{
	CRect rect;
	for(int i=0;i<m_pcfg->menuItemCmdStrArray.GetCount();i++){
		GetItemRect(i, &rect);
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
	int wndHight = m_pcfg->menuLayout==LAYOUT_VERTICAL?m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemHeight:m_pcfg->menuItemHeight;
	int	wndWidth = m_pcfg->menuLayout==LAYOUT_VERTICAL?m_pcfg->menuItemWidth:m_pcfg->menuItemCmdStrArray.GetCount()*m_pcfg->menuItemWidth;

	//更新字体编码
	HDC hdc = ::GetDC(*this);
	::EnumFontFamilies(hdc, (LPTSTR) NULL, (FONTENUMPROC)EnumFontNameProc,NULL);
	::ReleaseDC(*this, hdc);

	//更新菜单位置
	CRect rect;
	GetWindowRect(rect);
	if(wndWidth > m_pcfg->xScreen){
		rect.MoveToX(0);
	}else if(rect.left+wndWidth>m_pcfg->xScreen){
		rect.MoveToX(m_pcfg->xScreen-wndWidth);
	}
	if(wndHight > m_pcfg->yScreen){
		rect.MoveToY(0);
	}else if(rect.top+wndHight>m_pcfg->yScreen){
		rect.MoveToY(m_pcfg->yScreen-wndHight);
	}

	MoveWindow(rect.left,rect.top,wndWidth,wndHight,false);
	RedrawWindow();

	//更新图标位置
	AfxGetMainWnd()->GetWindowRect(&rect);
	if(rect.left+m_pcfg->iconConfigInfo.iconWidth>m_pcfg->xScreen){
		rect.MoveToX(m_pcfg->xScreen-m_pcfg->iconConfigInfo.iconWidth);
	}
	if(rect.top+m_pcfg->iconConfigInfo.iconHeight>m_pcfg->yScreen){
		rect.MoveToY(m_pcfg->yScreen-m_pcfg->iconConfigInfo.iconHeight);
	}
	AfxGetMainWnd()->MoveWindow(rect.left,rect.top,m_pcfg->iconConfigInfo.iconWidth,m_pcfg->iconConfigInfo.iconHeight,false);
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
		if(_wcsicmp(cmd, _T("<reload $font>"))==0){
			BuildFontSelectConfig();
		}else if(!Util::DoReloadCommand(cmd)){
			return false;
		}
		OnReloadCfg();
	}else if(_wcsicmp(cmd, _T("<screenshot>"))==0){
		//截屏前先隐藏当前程序窗口
		ShowWindow(SW_HIDE);
		AfxGetMainWnd()->ShowWindow(SW_HIDE);

		//1秒后开始截图
		SetTimer(1,1000,NULL);
	}else if(wcsstr(cmd, _T("<setfont"))!=NULL){
		Configuration *pcfg = Configuration::GetInstance();
		memset(pcfg->fontConfigInfo.lf.lfFaceName,0,sizeof(pcfg->fontConfigInfo.lf.lfFaceName));
		wcsncpy_s(pcfg->fontConfigInfo.lf.lfFaceName, sizeof(pcfg->fontConfigInfo.lf.lfFaceName),cmd+9, wcslen(cmd+9)-1);
		TRACE(_T("pcfg->fontConfigInfo.lf.lfFaceName =%s\n"), pcfg->fontConfigInfo.lf.lfFaceName);

		//更新字体编码
		HDC hdc = ::GetDC(*this);
		::EnumFontFamilies(hdc, (LPTSTR) NULL, (FONTENUMPROC)EnumFontNameProc,NULL);
		::ReleaseDC(*this, hdc);
		RedrawWindow();
		pcfg->SetLockFont(true);
	}else if(_wcsicmp(cmd, _T(""))!=0){
		return Util::ExecuteExCommand(cmd);
	}

	return true;
}

void CMenuWnd::BuildFontSelectConfig()
{
	TRACE(_T("CMenuWnd::BuildFontSelectConfig\n"));
	TCHAR buffer[MAX_PATH]={0};
	Configuration *pcfg = Configuration::GetInstance();
	CStringArray *menuItemCmdStrArray = &pcfg->menuItemCmdStrArray;
	CStringArray *menuItemTextStrArray = &pcfg->menuItemTextStrArray;
	menuItemCmdStrArray->RemoveAll();
	menuItemTextStrArray->RemoveAll();

	//pcfg->menuItemHeight=32;
	pcfg->fontConfigInfo.lf.lfHeight = 0;
	wcscpy_s(pcfg->menuBgFileName, sizeof(pcfg->menuBgFileName), _T(""));
	//添加返回
	memset(buffer, 0, sizeof(buffer));
	swprintf_s(buffer, sizeof(buffer), _T("<reload %s>"), pcfg->m_IniFileName);
	menuItemCmdStrArray->Add(buffer);

	memset(buffer, 0, sizeof(buffer));
	swprintf_s(buffer, sizeof(buffer), _T("%s"), _T("<    Back"));
	menuItemTextStrArray->Add(buffer);

	//添加字体列表
	HDC hdc = ::GetDC(*this);
	m_mSysFonts.RemoveAll();

	if (::EnumFontFamilies(hdc, (LPTSTR) NULL, (FONTENUMPROC)EnumFontNameProc,(LPARAM)&(m_mSysFonts)) != 0)
	{
		for (int i = 0; i< m_mSysFonts.GetCount(); i++)
		{
			memset(buffer, 0, sizeof(buffer));
			swprintf_s(buffer, sizeof(buffer), _T("<setfont %s>"), m_mSysFonts.GetAt(i));
			menuItemCmdStrArray->Add(buffer);

			memset(buffer, 0, sizeof(buffer));
			swprintf_s(buffer, sizeof(buffer), _T("%s"), m_mSysFonts.GetAt(i));
			menuItemTextStrArray->Add(buffer);
		}
	}
	::ReleaseDC(*this, hdc);
}
