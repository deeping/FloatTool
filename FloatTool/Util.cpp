#include "StdAfx.h"
#include "Util.h"
#include "IniFile.h"

//////////////////////////////////////////////////////////////////////////
/*
����: HideOrShowTaskBar
����: szFIleNameΪbmp�ļ�·��
dcΪ����bmpͼƬ��DC
����: ��ʾ��������������
*/
//////////////////////////////////////////////////////////////////////////
bool Util::LoadBitmapFile(LPCTSTR szFIleName,CDC& dc)
{
	CDC memdc;
	memdc.CreateCompatibleDC(&dc);
	CBitmap   bmBkgnd, *pOldBitmap = NULL;
	HBITMAP   bitmap=NULL;
	bmBkgnd.Detach();
	bitmap = (HBITMAP)::SHLoadDIBitmap(szFIleName);
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
����: ExecuteExCommand
����: ִ�п�ִ���ļ���������ִ�н��
����: true�ɹ�,falseʧ��
*/
//////////////////////////////////////////////////////////////////////////
BOOL Util::ExecuteExCommand(LPCTSTR lpFile)
{
	ASSERT(NULL != lpFile);

	SHELLEXECUTEINFO   ExecuteInfo;
	ExecuteInfo.cbSize = sizeof(ExecuteInfo);
	ExecuteInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	ExecuteInfo.hwnd = NULL;
	ExecuteInfo.lpVerb = NULL;   
	ExecuteInfo.lpFile = lpFile;
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
����: HideOrShowTaskBar
����: ��ʾ��������������
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
����: GetModulePath
����: ��ȡ��ǰ����·��
����: spPathΪ·��ָ��
*/
//////////////////////////////////////////////////////////////////////////
void Util::GetModulePath(TCHAR* spPath, int len)
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
����: GetFilePathFromTime
����: ����ǰ����·���뵱ǰʱ��ϳ��ļ�·��
����: spFilePathΪ·��ָ��
*/
//////////////////////////////////////////////////////////////////////////
void Util::GetBmpFilePathFromTime(TCHAR* spFilePath, int len)  
{
	ASSERT(NULL != spFilePath);

	TCHAR strTime[MAX_PATH] = {0};
	SYSTEMTIME nowTime;
	GetLocalTime(&nowTime);
	_stprintf(strTime, TEXT("%04d-%02d-%02d_%02d%02d%02d"),
		nowTime.wYear, nowTime.wMonth, nowTime.wDay, nowTime.wHour, nowTime.wMinute, nowTime.wSecond);

	GetModulePath(spFilePath, len);
	t_strcat(spFilePath, strTime);
	t_strcat(spFilePath, _T(".bmp"));
}

//////////////////////////////////////////////////////////////////////////
/*
����: SaveHwndToBmpFile
����: ��Ļ��ͼ�����浽��ǰ����Ŀ¼��
����: hWnd���ھ����Ϊ����Ϊ������Ļ
lpszPathΪ����bmpͼƬ��·��
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