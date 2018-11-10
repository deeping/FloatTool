//
// Read and write initialization files under wince, support ANSI and UNICODE.
// Use refer to Microsoft's initialization file format and API specification.
// Email: fanghtao # foxmail.com
//
#ifndef __PROFILE_H__
#define __PROFILE_H__
 
#include <windows.h>
 
#ifdef __cplusplus
extern "C" {
#endif
 
#define DIM(_x)  ( sizeof(_x) / sizeof(_x[0]) )
 
UINT
WINAPI
GetPrivateProfileIntA2(
					   LPCSTR lpAppName,
					   LPCSTR lpKeyName,
					   INT nDefault,
					   LPCSTR lpFileName
					   );
UINT
WINAPI
GetPrivateProfileIntW2(
					   LPCWSTR lpAppName,
					   LPCWSTR lpKeyName,
					   INT nDefault,
					   LPCWSTR lpFileName
					   );
#ifdef UNICODE
#define GetPrivateProfileInt  GetPrivateProfileIntW2
#else
#define GetPrivateProfileInt  GetPrivateProfileIntA2
#endif // !UNICODE
 
DWORD
WINAPI
GetPrivateProfileStringA2(
						  LPCSTR lpAppName,
						  LPCSTR lpKeyName,
						  LPCSTR lpDefault,
						  LPSTR lpReturnedString,
						  DWORD nSize,
						  LPCSTR lpFileName
						  );
DWORD
WINAPI
GetPrivateProfileStringW2(
						  LPCWSTR lpAppName,
						  LPCWSTR lpKeyName,
						  LPCWSTR lpDefault,
						  LPWSTR lpReturnedString,
						  DWORD nSize,
						  LPCWSTR lpFileName
						  );
#ifdef UNICODE
#define GetPrivateProfileString  GetPrivateProfileStringW2
#else
#define GetPrivateProfileString  GetPrivateProfileStringA2
#endif // !UNICODE
 
BOOL
WINAPI
WritePrivateProfileStringA2(
							LPCSTR lpAppName,
							LPCSTR lpKeyName,
							LPCSTR lpString,
							LPCSTR lpFileName
							);
BOOL
WINAPI
WritePrivateProfileStringW2(
							LPCWSTR lpAppName,
							LPCWSTR lpKeyName,
							LPCWSTR lpString,
							LPCWSTR lpFileName
							);
#ifdef UNICODE
#define WritePrivateProfileString  WritePrivateProfileStringW2
#else
#define WritePrivateProfileString  WritePrivateProfileStringA2
#endif // !UNICODE
 
#ifdef __cplusplus
};
#endif
 
#endif // __PROFILE_H__
