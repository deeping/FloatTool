#include "StdAfx.h"
#include "Util.h"
#include "Configuration.h"

//////////////////////////////////////////////////////////////////////////
/*
函数: RelativeToAbsolutePath
参数: rel为相对路径
abs为绝对路径
功能: 相对路径转绝对路径
*/
//////////////////////////////////////////////////////////////////////////
DWORD Util::RelativeToAbsolutePath(LPCTSTR rel, LPTSTR abs, int len)
{
	ASSERT(NULL != abs);

	DWORD ret = 0;
	int relCount = 0;
	int parentDirCount = 0;
	TCHAR *pos = (TCHAR *)rel;
	TCHAR *pos1;
	TCHAR path[MAX_PATH]={0};
	GetModuleDirectory(path,sizeof(path));

	pos=(TCHAR *)rel;
	while(*pos!='\\'&&*pos!=0){pos++;}
	if(*pos=='\\'){
		if(*(pos-1)=='.' && *(pos-2)!='.'){//当前目录'./'
			wcscat_s(path, sizeof(path), pos+1);
		}else if(*(pos-1)=='.' && *(pos-2)=='.'){//上一级目录'../'
			wcscat_s(path, sizeof(path), pos-2);
		}else{//绝对路径
			wcscpy_s(abs, len, pos);
			return ret;
		}
	}

	while(1){
		pos=(TCHAR *)path;
		pos=wcsstr(pos, _T("..\\"));
		if(pos!=NULL){
			pos1=pos-1;
			if(pos1!=path){
				while(*--pos1!='\\'){}
				wcsncpy(pos1+1,pos+3,wcslen(pos+3)+1);
			}else{
				TRACE(_T("path='%s'is not reachable\n"), path);
				ret =-1;
				break;
			}
		}else{
			break;
		}
	}

	wcscpy_s(abs, len, path);
	TRACE(_T("rel='%s'--->abs='%s'\n"), rel, abs);

	return ret;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: HideOrShowTaskBar
参数: szFIleName为bmp文件路径
dc为加载bmp图片的DC
功能: 加载bmp图片
*/
//////////////////////////////////////////////////////////////////////////
bool Util::LoadBitmapFile(LPCTSTR szFIleName,CDC& dc)
{
	ASSERT(NULL != szFIleName);
	TCHAR fileName[MAX_PATH]={0};
	RelativeToAbsolutePath(szFIleName, fileName, sizeof(fileName));

	CDC memdc;
	memdc.CreateCompatibleDC(&dc);
	CBitmap   bmBkgnd, *pOldBitmap = NULL;
	HBITMAP   bitmap=NULL;
	bmBkgnd.Detach();
	bitmap = (HBITMAP)::SHLoadDIBitmap(fileName);
	if(bitmap!=NULL)
	{
		bmBkgnd.Attach(bitmap);
		pOldBitmap = memdc.SelectObject(&bmBkgnd);
		dc.BitBlt(0,0,800,480,&memdc,0,0,SRCCOPY);
		memdc.SelectObject(&pOldBitmap);
		bmBkgnd.DeleteObject();
		memdc.DeleteDC();
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: DoReloadCommand
参数: cmd为<reload>命令及参数
功能: 重新加载配置文件
*/
//////////////////////////////////////////////////////////////////////////
BOOL Util::DoReloadCommand(LPCTSTR cmd)
{
	TCHAR fileName[MAX_PATH]={0};
	TCHAR *pos = (TCHAR *)cmd;

	pos=(TCHAR *)wcsstr(cmd, _T("<reload"));
	if(pos!=NULL){
		wcsncpy_s(fileName,sizeof(fileName), pos+7, wcslen(pos)+1);
		pos=fileName;
		while(*pos!='>'&&*pos!=0){pos++;}
		if(*pos=='>'){*pos=0;}

		RelativeToAbsolutePath(fileName, fileName, sizeof(fileName));

		TRACE(_T("fileName='%s' len=%d attr=%#x\n"), fileName, wcslen(fileName), GetFileAttributes(fileName));
		if(GetFileAttributes(fileName)!=INVALID_FILE_SIZE){
			return Configuration::GetInstance()->reLoadConfig(fileName);
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: ExecuteExCommand
功能: 执行可执行文件，并返回执行结果
返回: true成功,false失败
*/
//////////////////////////////////////////////////////////////////////////
BOOL Util::ExecuteExCommand(LPCTSTR lpFile)
{
	ASSERT(NULL != lpFile);

	TCHAR fileName[MAX_PATH]={0};
	RelativeToAbsolutePath(lpFile, fileName, sizeof(fileName));

	SHELLEXECUTEINFO   ExecuteInfo;
	ExecuteInfo.cbSize = sizeof(ExecuteInfo);
	ExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	ExecuteInfo.hwnd = NULL;
	ExecuteInfo.lpVerb = NULL;   
	ExecuteInfo.lpFile = fileName;
	ExecuteInfo.lpParameters = NULL;
	ExecuteInfo.lpDirectory = NULL;
	ExecuteInfo.nShow = SW_HIDE; 

	BOOL bResult = ShellExecuteEx(&ExecuteInfo); 
	if(!bResult && (int)ExecuteInfo.hInstApp <= 32){
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
/*
函数: HideOrShowTaskBar
功能: 显示或者隐藏任务栏
*/
//////////////////////////////////////////////////////////////////////////
void Util::HideOrShowTaskBar()
{
	HWND hwndTaskBar=FindWindow(TEXT("HHTaskBar"),NULL);
	if(hwndTaskBar){
		if(IsWindowVisible(hwndTaskBar)){
			ShowWindow(hwndTaskBar,SW_HIDE);
		}else{
			ShowWindow(hwndTaskBar,SW_SHOW);

		}
	}else{
		TRACE(_T("can't FindWindow taskBar\n"));
	}
}

//////////////////////////////////////////////////////////////////////////
/*
函数: GetModuleDirectory
功能: 获取当前程序路径
参数: spPath为路径指针
*/
//////////////////////////////////////////////////////////////////////////
void Util::GetModuleDirectory(TCHAR* spPath, int len)
{
	ASSERT(NULL != spPath);

	GetModuleFileName(AfxGetInstanceHandle(), spPath, len);  
	TCHAR *pos = wcsrchr (spPath,'\\');
	if(pos!=NULL){
		*(pos+1)=0;
	}
}

//////////////////////////////////////////////////////////////////////////
/*
函数: GetFilePathFromTime
功能: 将当前程序路径与当前时间合成文件路径
参数: spFilePath为路径指针
*/
//////////////////////////////////////////////////////////////////////////
void Util::GetBmpFilePathFromTime(TCHAR* spFilePath, int len)  
{
	ASSERT(NULL != spFilePath);

	TCHAR strTime[MAX_PATH] = {0};
	SYSTEMTIME nowTime;
	GetLocalTime(&nowTime);
	swprintf_s(strTime, sizeof(strTime), TEXT("%04d-%02d-%02d_%02d%02d%02d"),
		nowTime.wYear, nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute, nowTime.wSecond);

	GetModuleDirectory(spFilePath, len);
	wcscat_s(spFilePath, len, strTime);
	wcscat_s(spFilePath, len, _T(".bmp"));
}

//////////////////////////////////////////////////////////////////////////
/*
函数: SaveHwndToBmpFile
功能: 屏幕截图并保存到当前程序目录下
参数: hWnd窗口句柄，为空则为整个屏幕
lpszPath为保存bmp图片的路径
*/
//////////////////////////////////////////////////////////////////////////
BOOL Util::SaveHwndToBmpFile(HWND hWnd, LPCTSTR lpszPath)
{  
	ASSERT(NULL != lpszPath);  

	int iScaleWidth = 0;  
	int iScaleHight = 0;  

	HDC hSrcDC = NULL;  
	if (NULL == hWnd)   
	{  
		hWnd = ::GetDesktopWindow();    
		iScaleWidth = ::GetSystemMetrics(SM_CXSCREEN);  
		iScaleHight = ::GetSystemMetrics(SM_CYSCREEN);  
		hSrcDC = ::GetWindowDC(NULL);     
	}  
	else  
	{  
		RECT rc = {0};  
		GetWindowRect(hWnd, &rc);  
		iScaleWidth = rc.right - rc.left;   
		iScaleHight = rc.bottom - rc.top;  
		hSrcDC = ::GetWindowDC(hWnd);   
	}  

	ASSERT(hSrcDC);  

	DEBUGMSG(TRUE, (TEXT("MainWnd::WndProc:iScaleWidth = %d, iScaleHight = %d\n"), iScaleWidth, iScaleHight));  

	BOOL      bReturn      = TRUE;     
	HANDLE    hFile        = NULL;  
	DWORD     dwWritten    = 0;  
	DWORD     dwOffsetSize = 0;     
	DWORD     dwBufferSize = 0;     
	WORD      wBitCount    = 32;  
	HBITMAP   hbitmapSave  = NULL;     
	HBITMAP   hbitmapOld   = NULL;     
	HDC       hDcDev       = NULL;     
	HDC       hDcMem       = NULL;     
	BYTE      *pBmpBuffer  = NULL;     
	int       iBits        = 0;     
	BITMAP           stSrcbmp;     
	BITMAPINFO       srcdibbmap;     
	BITMAPFILEHEADER bmFileHeader;  

	SecureZeroMemory(&stSrcbmp, sizeof(BITMAP));     
	SecureZeroMemory(&srcdibbmap, sizeof(BITMAPINFO));     
	SecureZeroMemory(&bmFileHeader, sizeof(BITMAPFILEHEADER));    

	// Fill bitmap information constructor     
	srcdibbmap.bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);     
	srcdibbmap.bmiHeader.biWidth          = iScaleWidth;  
	srcdibbmap.bmiHeader.biHeight         = iScaleHight;  
	srcdibbmap.bmiHeader.biPlanes         = 1;     
	srcdibbmap.bmiHeader.biBitCount       = wBitCount;     
	srcdibbmap.bmiHeader.biCompression    = BI_RGB;  

	hDcMem = CreateCompatibleDC(NULL/*hSrcDC*/);    

	if (NULL == hDcMem)     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     
	hbitmapSave = CreateDIBSection(hDcMem, &srcdibbmap, DIB_RGB_COLORS, (void**)&pBmpBuffer, NULL, 0);     
	if (NULL == hbitmapSave)     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     
	hbitmapOld  = (HBITMAP)SelectObject(hDcMem, hbitmapSave);     
	if (NULL == hbitmapOld)     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     

	if(!::BitBlt(hDcMem, 0, 0, iScaleWidth, iScaleHight, hSrcDC, 0, 0, SRCCOPY))   
	{     
		bReturn = FALSE;     
		goto Exit;     
	}    

	if (0 == GetObject(hbitmapSave, sizeof(stSrcbmp), &stSrcbmp))     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}  

	dwBufferSize = stSrcbmp.bmWidth * stSrcbmp.bmHeight * wBitCount / 8;     
	dwOffsetSize = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);     

	// Fill bitmap header constructor     
	bmFileHeader.bfType = 0x4D42;          
	bmFileHeader.bfSize = dwOffsetSize + dwBufferSize;     
	bmFileHeader.bfReserved1 = 0;     
	bmFileHeader.bfReserved2 = 0;     
	bmFileHeader.bfOffBits   = dwOffsetSize;     

	hFile = CreateFile(lpszPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	if (INVALID_HANDLE_VALUE == hFile)     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     
	if (!WriteFile(hFile, &bmFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL))     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     
	if (!WriteFile(hFile, &srcdibbmap, sizeof(BITMAPINFO), &dwWritten, NULL))     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     
	if (!WriteFile(hFile, pBmpBuffer, dwBufferSize, &dwWritten, NULL))     
	{     
		bReturn = FALSE;     
		goto Exit;     
	}     

	::SelectObject(hDcMem, hbitmapOld);  
	::DeleteObject(hbitmapSave);  
	::DeleteDC(hDcMem);  
	::ReleaseDC(hWnd, hSrcDC);  

	bReturn = TRUE;     
Exit:     
	if (INVALID_HANDLE_VALUE != hFile)     
	{     
		CloseHandle(hFile);     
		hFile = INVALID_HANDLE_VALUE;     
	}     
	return bReturn;    
}