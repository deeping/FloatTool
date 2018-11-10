//
// Read and write initialization files under wince, support ANSI and UNICODE.
// Use refer to Microsoft's initialization file format and API specification.
// Email: fanghtao # foxmail.com
//

#include "stdafx.h"
#include "profile.h"

enum _encode
{
	ANSI = 0,
	UNCODE,
};

class CProfile
{
private:
	FILE *fp;
	long file_encode;
	long param_encode;
	char szreadbuf[1024];
	char szparambuf[1024];
	void *pfilename;
	void *psection;
	void *pkey;
	void *pstring;
	void *pstrbuf;
	long str_size;
	long str_len;
	int Read(void *szbuf, int bufsize);
	int Write(int section, int key, int end);
public:
	int Create(const void *filename, int encode);
	int Open(const void *filename, int encode);
	int Close();
	int Truncate(const void *filename, int filesize);
	int SetParam(void *section, void *key, void *string);
	int ReadString(void *szbuf, int bufsize);
	int WriteString();
};

//////////////////////////////////////////////////////////////////////////

int CProfile::Create(const void *filename, int encode)
{
	if (!filename) return -1;
	if (encode == ANSI)
		fp = fopen((PCSTR)filename, "wb");
	else  // UNICODE
		fp = _wfopen((PCWSTR)filename, L"wb");
	if (!fp) return -1;
	pfilename = (void*)filename;
	param_encode = encode;
	file_encode = ANSI;
	return 0;
}

int CProfile::Open(const void *filename, int encode)
{
	unsigned char szcode[4] = {0};
	if (!filename) return -1;
	if (encode == ANSI)
		fp = fopen((PCSTR)filename, "rb+");
	else  // UNICODE
		fp = _wfopen((PCWSTR)filename, L"rb+");
	if (!fp) return -1;
	fread(szcode, 1, sizeof(szcode), fp);
	if (szcode[0] == 0xff && szcode[1] == 0xfe)
	{
		fseek(fp, 2, SEEK_SET);
		file_encode = UNCODE;
	}
	else {
		fseek(fp, 0, SEEK_SET);
		file_encode = ANSI;
	}
	pfilename = (void*)filename;
	param_encode = encode;
	return 0;
}

int CProfile::Close()
{
	if (fp)
	{
		fclose(fp);
		fp = NULL;
	}
	return 0;
}

int CProfile::Truncate(const void *filename, int filesize)
{
	TCHAR szPath[MAX_PATH];
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if (param_encode == ANSI)
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)filename, -1, szPath, DIM(szPath));
	else  // UNICODE
		wcscpy(szPath, (LPCWSTR)filename);
	Close();
	hFile = CreateFile(szPath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile) return -1;
	SetFilePointer(hFile, filesize, NULL, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return 0;
}

int CProfile::SetParam(void *section, void *key, void *string)
{
	if (param_encode == file_encode)
	{
		psection = section;
		pkey = key;
		pstring = string;
		if (!pstring) {
			pstrbuf = szparambuf;
			if (param_encode == ANSI)
				str_size = sizeof(szparambuf);
			else
				str_size = sizeof(szparambuf)/sizeof(TCHAR);
		}
	}
	else if (param_encode == ANSI)  // ANSI->UNICODE
	{
		TCHAR *pBuf = (TCHAR*)szparambuf;
		int size = sizeof(szparambuf) / sizeof(TCHAR);
		int len = 0;
		if (section) {
			len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)section, -1, pBuf, size);
			psection = pBuf;
			pBuf += len;
			size -= len;
		} else psection = NULL;
		if (key) {
			len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)key, -1, pBuf, size);
			pkey = pBuf;
			pBuf += len;
			size -= len;
		} else pkey = NULL;
		if (string) {
			len = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)string, -1, pBuf, size);
			pstring = pBuf;
			pBuf += len;
			size -= len;
		} else pstring = NULL;
		if (!pstring) {
			pstrbuf = pBuf;
			str_size = size;
		}
	}
	else  // UNICODE->ANSI
	{
		char *pBuf = szparambuf;
		int size = sizeof(szparambuf);
		int len = 0;
		if (section) {
			len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)section, -1, pBuf, size, NULL, NULL);
			psection = pBuf;
			pBuf += len;
			size -= len;
		} else psection = NULL;
		if (key) {
			len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)key, -1, pBuf, size, NULL, NULL);
			pkey = pBuf;
			pBuf += len;
			size -= len;
		} else pkey = NULL;
		if (string) {
			len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)string, -1, pBuf, size, NULL, NULL);
			pstring = pBuf;
			pBuf += len;
			size -= len;
		} else pstring = NULL;
		if (!pstring) {
			pstrbuf = pBuf;
			str_size = size;
		}
	}
	return 0;
}

int CProfile::Read(void *szbuf, int bufsize)
{
	if (str_len == 0) return 0;
	if (param_encode == file_encode)
	{
		if (param_encode == ANSI)
			memcpy(szbuf, pstrbuf, sizeof(char)*str_size);
		else  // UNICODE
			memcpy(szbuf, pstrbuf, sizeof(TCHAR)*str_size);
	}
	else if (param_encode == ANSI)  // UNICODE->ANSI
	{
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pstrbuf, str_size, (LPSTR)szbuf, bufsize, NULL, NULL);
	}
	else  // ANSI->UNICODE
	{
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pstrbuf, str_size, (LPWSTR)szbuf, bufsize);
	}
	return str_len;
}

int CProfile::ReadString(void *szbuf, int bufsize)
{
	int match = 0;
	int size = (bufsize < str_size) ? bufsize : str_size;
	int len = 0;
	if (!fp || !pstrbuf) return 0;
	str_size = 0;
	str_len = 0;
	if (file_encode == ANSI)
	{
		char *pstr = (char*)pstrbuf;
		char *pbuf = NULL;
		char *pos = NULL;
		while (!feof(fp))
		{
			pbuf = fgets(szreadbuf, sizeof(szreadbuf), fp);
			if (!pbuf) break;
			if (pbuf[0] == ';') continue;  // comment line
			else if (pbuf[0] == '[')
			{
				if (pos = strchr(++pbuf, ']'))  // section line
				{
					if (match) break;
					pos[0] = '\0';
					if (!psection)  // return all section names
					{
						len = (int)(pos - pbuf) + 1;
						len = (len < (size-2)) ? len : (size-2);
						strncpy(pstr, pbuf, len);
						str_len += len;
						pstr += len;
						size -= len;
						str_size = str_len + 1;
						pstr[0] = '\0';
						if (size == 2) {
							str_size = str_len + 2;
							pstr[1] = '\0';
							break;
						}
					}
					else if (0 == strcmp(pbuf, (char*)psection))  // match section
					{
						match = 1;
					}
				}
			}
			else if (match)  // key lines of match section
			{
				if (pos = strchr(pbuf, '='))
				{
					while (pbuf[0] == ' ') pbuf++;
					len = (int)(pos - pbuf);
					while (len > 0 && pbuf[len-1] == ' ') len--;
					pbuf[len] = '\0';
					if (!pkey)  // return all key names of match section
					{
						len = len + 1;
						len = (len < (size-2)) ? len : (size-2);
						strncpy(pstr, pbuf, len);
						str_len += len;
						pstr += len;
						size -= len;
						str_size = str_len + 1;
						pstr[0] = '\0';
						if (size == 2) {
							str_size = str_len + 2;
							pstr[1] = '\0';
							break;
						}
					}
					else if (0 == strcmp(pbuf, (char*)pkey))  // match key
					{
						pos++;
						while (pos[0] == ' ') pos++;
						len = (int)strlen(pos);
						while ( (len > 0) &&
							(pos[len-1] == ' ' || pos[len-1] == '\r' || pos[len-1] == '\n') )
							len--;
						len = (len < (size-1)) ? len : (size-1);
						strncpy(pstr, pos, len);
						pstr[len] = '\0';
						str_size = len + 1;
						str_len = len;
						break;
					}
				}
			}
		}
	}
	else  // UNICODE
	{
		TCHAR *pstr = (TCHAR*)pstrbuf;
		TCHAR *pbuf = NULL;
		TCHAR *pos = NULL;
		while (!feof(fp))
		{
			pbuf = fgetws((TCHAR*)szreadbuf, sizeof(szreadbuf)/sizeof(TCHAR), fp);
			if (!pbuf) break;
			if (pbuf[0] == ';') continue;  // comment line
			else if (pbuf[0] == '[')
			{
				if (pos = wcschr(++pbuf, ']'))  // section line
				{
					if (match) break;
					pos[0] = '\0';
					if (!psection)  // return all section names
					{
						len = (int)(pos - pbuf) + 1;
						len = (len < (size-2)) ? len : (size-2);
						wcsncpy(pstr, pbuf, len);
						str_len += len;
						pstr += len;
						size -= len;
						str_size = str_len + 1;
						pstr[0] = '\0';
						if (size == 2) {
							str_size = str_len + 2;
							pstr[1] = '\0';
							break;
						}
					}
					else if (0 == wcscmp(pbuf, (TCHAR*)psection))  // match section
					{
						match = 1;
					}
				}
			}
			else if (match)  // key lines of match section
			{
				if (pos = wcschr(pbuf, '='))
				{
					while (pbuf[0] == ' ') pbuf++;
					len = (int)(pos - pbuf);
					while (len > 0 && pbuf[len-1] == ' ') len--;
					pbuf[len] = '\0';
					if (!pkey)  // return all key names of match section
					{
						len = len + 1;
						len = (len < (size-2)) ? len : (size-2);
						wcsncpy(pstr, pbuf, len);
						str_len += len;
						pstr += len;
						size -= len;
						str_size = str_len + 1;
						pstr[0] = '\0';
						if (size == 2) {
							str_size = str_len + 2;
							pstr[1] = '\0';
							break;
						}
					}
					else if (0 == wcscmp(pbuf, (TCHAR*)pkey))  // match key
					{
						pos++;
						while (pos[0] == ' ') pos++;
						len = (int)wcslen(pos);
						while ( (len > 0) &&
							(pos[len-1] == ' ' || pos[len-1] == '\r' || pos[len-1] == '\n') )
							len--;
						len = (len < (size-1)) ? len : (size-1);
						wcsncpy(pstr, pos, len);
						pstr[len] = '\0';
						str_size = len + 1;
						str_len = len;
						break;
					}
				}
			}
		}
	}
	return Read(szbuf, bufsize);
}

int CProfile::Write(int section, int key, int end)
{
	char *pread = szreadbuf;
	int r_offset = end;
	int w_offset = 0;
	int r_len = 0;
	int w_len = 0;
	if (pkey && pstring)  // write string
	{
		char szwritebuf[1024] = {0};
		char *pwrite = szwritebuf;
		char *ptemp = "";
		char flag = 0;
		if (-1 != key)
			w_offset = key;
		else 
			w_offset = end;
		if (file_encode == ANSI)
		{
			char ctemp = '\n';
			fseek(fp, w_offset-sizeof(char), SEEK_SET);
			fread(&ctemp, sizeof(char), 1, fp);
			if (ctemp != '\n') ptemp = "\r\n";
			if (-1 != section)
				sprintf(pwrite, "%s%s=%s\r\n", ptemp, pkey, pstring);
			else
				sprintf(pwrite, "%s[%s]\r\n%s=%s\r\n", ptemp, psection, pkey, pstring);
			w_len = (int)strlen(pwrite);
		}
		else  // UNICODE
		{
			TCHAR ctemp = '\n';
			fseek(fp, w_offset-sizeof(TCHAR), SEEK_SET);
			fread(&ctemp, sizeof(TCHAR), 1, fp);
			if (ctemp != '\n') ptemp = "\r\n";
			if (-1 != section)
				swprintf((TCHAR*)pwrite, L"%S%s=%s\r\n", ptemp, pkey, pstring);
			else
				swprintf((TCHAR*)pwrite, L"%S[%s]\r\n%s=%s\r\n", ptemp, psection, pkey, pstring);
			w_len = (int)wcslen((TCHAR*)pwrite)*sizeof(TCHAR);
		}
		if (w_len < r_offset - w_offset) flag = 1;
		while (w_len)
		{
			if (fseek(fp, r_offset, SEEK_SET)) return -1;
			r_len = (int)fread(pread, 1, sizeof(szreadbuf), fp);
			r_offset += r_len;
			if (fseek(fp, w_offset, SEEK_SET)) return -1;
			w_len = (int)fwrite(pwrite, 1, w_len, fp);
			w_offset += w_len;
			ptemp = pread;
			pread = pwrite;
			pwrite = ptemp;
			w_len = r_len;
		}
		if (flag) return Truncate(pfilename, w_offset);
	}
	else  // delete
	{
		if (!pkey && -1 != section) w_offset = section;  // delete section
		else if (!pstring && -1 != key) w_offset = key;  // delete key
		if (w_offset)
		{
			do {if (fseek(fp, r_offset, SEEK_SET)) return -1;
			r_len = (int)fread(pread, 1, sizeof(szreadbuf), fp);
			r_offset += r_len;
			if (fseek(fp, w_offset, SEEK_SET)) return -1;
			w_len = (int)fwrite(pread, 1, r_len, fp);
			w_offset += w_len;
			} while (r_len);
			return Truncate(pfilename, w_offset);
		}
	}
	return 0;
}

int CProfile::WriteString()
{
	int section_start = -1;
	int key_start = -1;
	int start = -1;
	int end = -1;
	int len = 0;
	if (!fp || !psection) return -1;
	if (file_encode == ANSI)
	{
		char *pstr = (char*)pstring;
		char *pbuf = NULL;
		char *pos = NULL;
		while (!feof(fp))
		{
			start = ftell(fp);
			pbuf = fgets(szreadbuf, sizeof(szreadbuf), fp);
			if (!pbuf) break;
			if (pbuf[0] == ';') continue;  // comment line
			else if (pbuf[0] == '[')
			{
				if (pos = strchr(++pbuf, ']'))  // section line
				{
					if (-1 != section_start) {
						end = start;
						break;
					}
					pos[0] = '\0';
					if (0 == strcmp(pbuf, (char*)psection))  // match section
					{
						section_start = start;
					}
				}
			}
			else if (-1 != section_start && pkey)  // key lines of match section
			{
				if (pos = strchr(pbuf, '='))
				{
					while (pbuf[0] == ' ') pbuf++;
					len = (int)(pos - pbuf);
					while (len > 0 && pbuf[len-1] == ' ') len--;
					pbuf[len] = '\0';
					if (0 == strcmp(pbuf, (char*)pkey))  // match key
					{
						key_start = start;
						end = ftell(fp);
						break;
					}
				}
			}
		}
	}
	else  // UNICODE
	{
		TCHAR *pstr = (TCHAR*)pstring;
		TCHAR *pbuf = NULL;
		TCHAR *pos = NULL;
		while (!feof(fp))
		{
			start = ftell(fp);
			pbuf = fgetws((TCHAR*)szreadbuf, sizeof(szreadbuf)/sizeof(TCHAR), fp);
			if (!pbuf) break;
			if (pbuf[0] == ';') continue;  // comment line
			else if (pbuf[0] == '[')
			{
				if (pos = wcschr(++pbuf, ']'))  // section line
				{
					if (-1 != section_start) {
						end = start;
						break;
					}
					pos[0] = '\0';
					if (0 == wcscmp(pbuf, (TCHAR*)psection))  // match section
					{
						section_start = start;
					}
				}
			}
			else if (-1 != section_start && pkey)  // key lines of match section
			{
				if (pos = wcschr(pbuf, '='))
				{
					while (pbuf[0] == ' ') pbuf++;
					len = (int)(pos - pbuf);
					while (len > 0 && pbuf[len-1] == ' ') len--;
					pbuf[len] = '\0';
					if (0 == wcscmp(pbuf, (TCHAR*)pkey))  // match key
					{
						key_start = start;
						end = ftell(fp);
						break;
					}
				}
			}
		}
	}
	if (-1 == end) end = ftell(fp);
	return Write(section_start, key_start, end);
}

//////////////////////////////////////////////////////////////////////////

UINT
WINAPI
GetPrivateProfileIntA2(
					   LPCSTR lpAppName,
					   LPCSTR lpKeyName,
					   INT nDefault,
					   LPCSTR lpFileName
					   )
{
	char szReturn[20] = {0};
	if (!lpAppName || !lpKeyName || !lpFileName)
	{
		return nDefault;
	}
	GetPrivateProfileStringA2(lpAppName, lpKeyName, NULL, szReturn, DIM(szReturn), lpFileName);
	if (strlen(szReturn) > 0)
	{
		return (UINT)_atoi64(szReturn);
	}
	else
	{
		return nDefault;
	}
}

UINT
WINAPI
GetPrivateProfileIntW2(
					   LPCWSTR lpAppName,
					   LPCWSTR lpKeyName,
					   INT nDefault,
					   LPCWSTR lpFileName
					   )
{
	TCHAR szReturn[20] = {0};
	if (!lpAppName || !lpKeyName || !lpFileName)
	{
		return nDefault;
	}
	GetPrivateProfileStringW2(lpAppName, lpKeyName, NULL, szReturn, DIM(szReturn), lpFileName);
	if (wcslen(szReturn) > 0)
	{
		return (UINT)_wtoi64(szReturn);
	}
	else
	{
		return nDefault;
	}
}

DWORD
WINAPI
GetPrivateProfileStringA2(
						  LPCSTR lpAppName,
						  LPCSTR lpKeyName,
						  LPCSTR lpDefault,
						  LPSTR lpReturnedString,
						  DWORD nSize,
						  LPCSTR lpFileName
						  )
{
	CProfile profile;
	DWORD dwLen = 0;

	if (!lpReturnedString || nSize < 1) return 0;
	if (nSize == 1)
	{
		lpReturnedString[0] = '\0';
		return 0;
	}
	if (0 == profile.Open(lpFileName, ANSI))
	{
		profile.SetParam((LPSTR)lpAppName, (LPSTR)lpKeyName, NULL);
		dwLen = profile.ReadString(lpReturnedString, nSize);
		profile.Close();
	}
	if (dwLen == 0)
	{
		if (!lpDefault) lpReturnedString[0] = '\0';
		else
		{
			dwLen = (DWORD)strlen(lpDefault);
			dwLen = (dwLen < (nSize-1)) ? dwLen : (nSize-1);
			strncpy(lpReturnedString, lpDefault, dwLen);
			lpReturnedString[dwLen] = '\0';
		}
	}
	return dwLen;
}

DWORD
WINAPI
GetPrivateProfileStringW2(
						  LPCWSTR lpAppName,
						  LPCWSTR lpKeyName,
						  LPCWSTR lpDefault,
						  LPWSTR lpReturnedString,
						  DWORD nSize,
						  LPCWSTR lpFileName
						  )
{
	CProfile profile;
	DWORD dwLen = 0;

	if (!lpReturnedString || nSize < 1) return 0;
	if (nSize == 1)
	{
		lpReturnedString[0] = '\0';
		return 0;
	}
	if (0 == profile.Open(lpFileName, UNCODE))
	{
		profile.SetParam((LPWSTR)lpAppName, (LPWSTR)lpKeyName, NULL);
		dwLen = profile.ReadString(lpReturnedString, nSize);
		profile.Close();
	}
	if (dwLen == 0)
	{
		if (!lpDefault) lpReturnedString[0] = '\0';
		else
		{
			dwLen = (DWORD)wcslen(lpDefault);
			dwLen = (dwLen < (nSize-1)) ? dwLen : (nSize-1);
			wcsncpy(lpReturnedString, lpDefault, dwLen);
			lpReturnedString[dwLen] = '\0';
		}
	}
	return dwLen;
}

BOOL
WINAPI
WritePrivateProfileStringA2(
							LPCSTR lpAppName,
							LPCSTR lpKeyName,
							LPCSTR lpString,
							LPCSTR lpFileName
							)
{
	CProfile profile;

	if (!lpFileName || !lpAppName)
	{
		return FALSE;
	}
	if (-1 == profile.Open(lpFileName, ANSI))
	{
		if (-1 == profile.Create(lpFileName, ANSI))
		{
			return FALSE;
		}
	}
	profile.SetParam((LPSTR)lpAppName, (LPSTR)lpKeyName, (LPSTR)lpString);
	if (profile.WriteString())
	{
		profile.Close();
		return FALSE;
	}
	else
	{
		profile.Close();
		return TRUE;
	}
}

BOOL
WINAPI
WritePrivateProfileStringW2(
							LPCWSTR lpAppName,
							LPCWSTR lpKeyName,
							LPCWSTR lpString,
							LPCWSTR lpFileName
							)
{
	CProfile profile;

	if (!lpFileName || !lpAppName)
	{
		return FALSE;
	}
	if (-1 == profile.Open(lpFileName, UNCODE))
	{
		if (-1 == profile.Create(lpFileName, UNCODE))
		{
			return FALSE;
		}
	}
	profile.SetParam((LPWSTR)lpAppName, (LPWSTR)lpKeyName, (LPWSTR)lpString);
	if (profile.WriteString())
	{
		profile.Close();
		return FALSE;
	}
	else
	{
		profile.Close();
		return TRUE;
	}
}
