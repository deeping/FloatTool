/***************************************
 * IniFile.h
 * 说明：在WinCe平台读写 INI 文件
 * by hoodlum1980
 * 2009.08.03 
 ***************************************/
#ifndef __INIFILE_H_BY_HOODLUM1980
#define __INIFILE_H_BY_HOODLUM1980

//是否在WINCE平台上
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
    #define t_sopen            _wsopen        //注意WinCe上不支持！
    #define t_fopen         _wfopen
    #define t_fgets            fgetws
    #define t_fprintf        fwprintf    //文件格式化写入
    #define t_sprintf        swprintf    //格式化文本
    #define t_strcpy        wcscpy
    #define t_strncpy        wcsncpy        //拷贝指定个数的字符
    #define t_strcat        wcscat        //append a string
    #define t_strtol        wcstol
    #define t_strlen        wcslen
    #define t_strcmp        wcscmp
    #define t_stricmp        _wcsicmp    //忽略大小写的字符串比较
    #define t_strncmp        wcsncmp        //比较n个字符
    #define t_strchr        wcschr        //find a character in a string
    #define t_strrchr        wcsrchr        //从结尾向前查找字符

#else  //ASCII CODE

    #define t_sopen            _sopen        //注意WinCe上不支持！
    #define t_fopen         fopen
    #define t_fgets            fgets        //读取一行文本
    #define t_fprintf        fprintf        //文件格式化写入
    #define t_sprintf        sprintf        //格式化文本
    #define t_strcpy        strcpy
    #define t_strncpy        strncpy        //拷贝指定个数的字符
    #define t_strcat        strcat        //append a string
    #define t_strtol        strtol        //把字符串转换成long（int32）
    #define t_strlen        strlen
    #define t_strcmp        strcmp        //比较字符串
    #define t_stricmp        _stricmp    //忽略大小写的字符串比较
    #define t_strncmp        strncmp        //比较n个字符
    #define t_strchr        strchr        //查找字符
    #define t_strrchr        strrchr        //从结尾向前查找字符

#endif

//CeWritePrivateProfileString 方法用到的辅助标记
#define MODE_DELETE_SECTION        11
#define MODE_OVERWRITE_SECTION    12
#define MODE_APPEND_SECTION        13
#define MODE_DELETE_KEY            21
#define MODE_OVERWRITE_KEY        22
#define MODE_APPEND_KEY            23

#define LINESIZE                260    //行缓冲区大小

DWORD CeGetPrivateProfileString(
    LPCTSTR lpAppName,                //section name: [lpAppName]
    LPCTSTR lpKeyName,                //lpKeyName=lpReturnedString
    LPCTSTR lpDefault,                //未找到时的默认值
    LPTSTR lpReturnedString,        //[out] 查找到的结果
    DWORD nSize,                    //[in]lpReturnedString的字符数,注意单位不是字节！
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

//在PC平台上可以调用_chsize函数调整文件大小，但是在WINCE平台上
//由于不支持，所以必须注意当文件尺寸应该缩小时，文件尾部内容不确定！！！！
BOOL CeWritePrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,    //要修改的KEY，如果为NULL，会删除整个Section
    LPCTSTR lpString,    //要写入的值，如果为NULL，则会删除这个KEY
    LPCTSTR lpFileName
    );

//重写某个Section，注意和 PC API 的区别是，这里不保证原子性操作
BOOL CeWritePrivateProfileSection(
    LPCTSTR lpAppName,    //section name
    LPCTSTR lpString,    //key1=val1 \0 key2=val2 \0\0
    LPCTSTR lpFileName
    );

//==============================================
//        以下是我增加的函数（在API中没有）
//==============================================
DWORD CeGetPrivateProfileKeyNames(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,                //缓冲区的字符数
    LPCTSTR lpFileName
    );

#endif