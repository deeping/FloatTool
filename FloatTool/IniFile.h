/***************************************
 * IniFile.h
 * ˵������WinCeƽ̨��д INI �ļ�
 * by hoodlum1980
 * 2009.08.03 
 ***************************************/
#ifndef __INIFILE_H_BY_HOODLUM1980
#define __INIFILE_H_BY_HOODLUM1980

//�Ƿ���WINCEƽ̨��
#ifndef WINCE
#define WINCE
#endif

#include "StdAfx.h"
#ifndef WINCE
    #include <io.h>        //for _sopen
    #include <fcntl.h>    //for _O_RDWT
    #include <share.h>    // for _SH_DENYRW
#endif

#ifdef  UNICODE   // r_winnt
    #define t_sopen            _wsopen        //ע��WinCe�ϲ�֧�֣�
    #define t_fopen         _wfopen
    #define t_fgets            fgetws
    #define t_fprintf        fwprintf    //�ļ���ʽ��д��
    #define t_sprintf        swprintf    //��ʽ���ı�
    #define t_strcpy        wcscpy
    #define t_strncpy        wcsncpy        //����ָ���������ַ�
    #define t_strcat        wcscat        //append a string
    #define t_strtol        wcstol
    #define t_strlen        wcslen
    #define t_strcmp        wcscmp
    #define t_stricmp        _wcsicmp    //���Դ�Сд���ַ����Ƚ�
    #define t_strncmp        wcsncmp        //�Ƚ�n���ַ�
    #define t_strchr        wcschr        //find a character in a string
    #define t_strrchr        wcsrchr        //�ӽ�β��ǰ�����ַ�

#else  //ASCII CODE

    #define t_sopen            _sopen        //ע��WinCe�ϲ�֧�֣�
    #define t_fopen         fopen
    #define t_fgets            fgets        //��ȡһ���ı�
    #define t_fprintf        fprintf        //�ļ���ʽ��д��
    #define t_sprintf        sprintf        //��ʽ���ı�
    #define t_strcpy        strcpy
    #define t_strncpy        strncpy        //����ָ���������ַ�
    #define t_strcat        strcat        //append a string
    #define t_strtol        strtol        //���ַ���ת����long��int32��
    #define t_strlen        strlen
    #define t_strcmp        strcmp        //�Ƚ��ַ���
    #define t_stricmp        _stricmp    //���Դ�Сд���ַ����Ƚ�
    #define t_strncmp        strncmp        //�Ƚ�n���ַ�
    #define t_strchr        strchr        //�����ַ�
    #define t_strrchr        strrchr        //�ӽ�β��ǰ�����ַ�

#endif

//CeWritePrivateProfileString �����õ��ĸ������
#define MODE_DELETE_SECTION        11
#define MODE_OVERWRITE_SECTION    12
#define MODE_APPEND_SECTION        13
#define MODE_DELETE_KEY            21
#define MODE_OVERWRITE_KEY        22
#define MODE_APPEND_KEY            23

#define LINESIZE                260    //�л�������С

DWORD CeGetPrivateProfileString(
    LPCTSTR lpAppName,                //section name: [lpAppName]
    LPCTSTR lpKeyName,                //lpKeyName=lpReturnedString
    LPCTSTR lpDefault,                //δ�ҵ�ʱ��Ĭ��ֵ
    LPTSTR lpReturnedString,        //[out] ���ҵ��Ľ��
    DWORD nSize,                    //[in]lpReturnedString���ַ���,ע�ⵥλ�����ֽڣ�
    LPCTSTR lpFileName
    );

UINT CeGetPrivateProfileInt(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    int nDefault,
    LPCTSTR lpFileName
    );

DWORD CeGetPrivateProfileSection(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,
    LPCTSTR lpFileName
    );

DWORD CeGetPrivateProfileSectionNames(
    LPTSTR lpszReturnBuffer,
    DWORD nSize,
    LPCTSTR lpFileName
    );

//��PCƽ̨�Ͽ��Ե���_chsize���������ļ���С��������WINCEƽ̨��
//���ڲ�֧�֣����Ա���ע�⵱�ļ��ߴ�Ӧ����Сʱ���ļ�β�����ݲ�ȷ����������
BOOL CeWritePrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,    //Ҫ�޸ĵ�KEY�����ΪNULL����ɾ������Section
    LPCTSTR lpString,    //Ҫд���ֵ�����ΪNULL�����ɾ�����KEY
    LPCTSTR lpFileName
    );

//��дĳ��Section��ע��� PC API �������ǣ����ﲻ��֤ԭ���Բ���
BOOL CeWritePrivateProfileSection(
    LPCTSTR lpAppName,    //section name
    LPCTSTR lpString,    //key1=val1 \0 key2=val2 \0\0
    LPCTSTR lpFileName
    );

//==============================================
//        �����������ӵĺ�������API��û�У�
//==============================================
DWORD CeGetPrivateProfileKeyNames(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,                //���������ַ���
    LPCTSTR lpFileName
    );

#endif