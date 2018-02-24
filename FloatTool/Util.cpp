#include "StdAfx.h"
#include "Util.h"


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