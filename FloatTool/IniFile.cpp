//适用于 char* 和 UNICODE，
//所有字符串必须使用 TEXT("aa") 或者 _T("aa") 的格式（自动适应 char* 或 UNICODE）
//所有相关函数加t_前缀
//IniFile: 读取INI FILE的简单解析!所谓简单，也就是解析代码简单，但对文件格式要求更高
//[1]任何字符串前后不要有空格（使解析代码可以不考虑前后的trim）
//            例如允许"Key1=Val", 而不允许" Key1 = Val "
//[2]允许有注释，第一个字符必须是英文分号';'
//
#include "StdAfx.h"
#include "IniFile.h"

//从appname（section）中读取string类型key
DWORD CeGetPrivateProfileString(
    LPCTSTR lpAppName,                //section name: [lpAppName]
    LPCTSTR lpKeyName,                //lpKeyName=lpReturnedString
    LPCTSTR lpDefault,                //未找到时的默认值
    LPTSTR lpReturnedString,    //[out] 查找到的结果
    DWORD nSize,                            //[in]lpReturnedString的字符数,注意单位不是字节！
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0;
    FILE *stream;
    bool bFindVal = false;
    bool bFindSection = false;
    TCHAR line[ LINESIZE ];
    size_t sectionLength, keyLength, lineLength;
    
    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        //设置默认值
        t_strcpy(lpReturnedString, lpDefault);
        ret = t_strlen(lpReturnedString); 
        return ret;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在UNICODE环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }
        
        //尝试寻找到 section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //本行是否是 [section]
            //这里是我们想要的Section吗？
            //检查这一行的宽度是否正好是section长度加2， [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            //读取section前求出 Key 的长度
            keyLength = t_strlen(lpKeyName);
            bFindSection = true;            
            continue;
        }
        
        //查找Key, Section End?
        if(line[0]=='[') break; //遇到了下一个
            
        if(lineLength < keyLength+1 || line[keyLength] != '=') continue; //"KeyName="
        if(t_strncmp(line, lpKeyName, keyLength)!=0) continue;
        //Now We Get the Key! 
        t_strcpy(lpReturnedString, line + keyLength + 1);
        //Now It's done.
        bFindVal = true;
        break;
    }
    
    fclose(stream);
    if(!bFindVal)
    {
        //设置默认值
        t_strcpy(lpReturnedString, lpDefault); 
    }
    ret = t_strlen(lpReturnedString); 
    return ret;
}


//读取一个int值
UINT CeGetPrivateProfileInt(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    int nDefault,
    LPCTSTR lpFileName
    )
{
    long ret = nDefault; //返回值
    FILE *stream;
    bool bFindVal = false;
    bool bFindSection = false;
    TCHAR line[ LINESIZE ];
    size_t sectionLength, keyLength, lineLength;
    
    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        //设置默认值
        return nDefault;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在UNICODE环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }
        
        //尝试寻找到 section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //本行是否是 [section]
            //这里是我们想要的Section吗？
            //检查这一行的宽度是否正好是section长度加2， [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            //读取section前求出 Key 的长度
            keyLength = t_strlen(lpKeyName);
            bFindSection = true;            
            continue;
        }
        
        //查找Key, Section End?
        if(line[0]=='[') break; //遇到了下一个
            
        if(lineLength < keyLength+1 || line[keyLength] != '=') continue; //"KeyName="
        if(t_strncmp(line, lpKeyName, keyLength)!=0) continue;
        //Now We Get the Key! 
        TCHAR *pStopChar = NULL;
        ret = t_strtol(line + keyLength + 1, &pStopChar, 10); //默认为10进制
        //Now It's done.
        bFindVal = true;
        break;
    }
    
    fclose(stream);
    return ret;
}


//获取某个Section下面的所有“key=value”形式的字符串集合，以0字符分割
//结尾使用两个0字符
//缓冲区写入："key1=value1 \0 key2=value2 \0 \0 "
//返回值表示写入缓冲区的字符数, 不包括结尾的0字符。
//如果缓冲区不够容纳所有的键值对，则返回值 = (nSize-2)
DWORD CeGetPrivateProfileSection(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,                //缓冲区的字符数
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0; //返回值，拷贝的字符数量
    DWORD remainSize = nSize - 2; //缓冲区当前所能能够接纳的字符数量
    DWORD copySize;                //本次循环中需要拷贝的字符数量
    FILE *stream;
    bool bFindSection = false; //是否已经找到Section
    TCHAR line[ LINESIZE ];   //行缓冲区
    LPTSTR pItem; //指向当前键值对的写入地址
    size_t sectionLength, lineLength;
    

    pItem = lpReturnedString; //指向缓冲区起始地址

    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        return ret;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //缓冲区是否还有剩余空间？
        if(remainSize <= 0) break;

        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在UNICODE环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }
        
        //尝试寻找到 section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //本行是否是 [section]
            //这里是我们想要的Section吗？
            //检查这一行的宽度是否正好是section长度加2， [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            bFindSection = true;            
            continue;
        }
        
        //查找Key, Section End?
        if(line[0]=='[') break; //遇到了下一个

        //copy the line to buffer, 注意ncpy不会复制结尾的0字符
        copySize = min( remainSize, lineLength );
        t_strncpy(pItem, line, copySize);
        //追加一个0字符
        pItem[copySize] = 0;

        //缩小缓冲区剩余字符数量remainSize，和当前写入位置pItem
        ret += (copySize + 1); //加1是为了统计结尾的0字符
        remainSize -= (copySize + 1);
        pItem += (copySize + 1);
    }
    
    fclose(stream);

    if(bFindSection)
    {
        //再次对缓冲区追加一个0 字符
        *pItem = 0;
    }
    return ret;
}


//获取一个ini文件中所有section的name，拷贝到缓冲区
//注意和系统API的区别是，系统API的读取是原子性的，即读取时不允许修改ini文件的内容
//而我们的函数未必保证这一点
DWORD CeGetPrivateProfileSectionNames(
    LPTSTR lpszReturnBuffer,
    DWORD nSize,
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0;                    //返回值，拷贝的字符数量
    DWORD remainSize = nSize - 2;    //缓冲区当前所能能够接纳的字符数量
    DWORD copySize;                    //本次循环中需要拷贝的字符数量
    TCHAR line[ LINESIZE ];            //行缓冲区
    TCHAR *pSectionEndChar;            //']'字符指针
    LPTSTR pItem;                    //指向当前键值对的写入地址
    FILE *stream;                    //流指针
    size_t lineLength;                //行字符长度
    
    pItem = lpszReturnBuffer; //指向缓冲区起始地址

    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        return ret;
    }
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //缓冲区是否还有剩余空间？
        if(remainSize <= 0) break;

        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在 UNICODE 环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }

        if(line[0] != '[') continue; //本行是否是 [section]
        //找到了一个Section，开始拷贝
        //copy the section name to buffer, 注意ncpy不会复制结尾的0字符

        //LINE: "[sectionName]"
        //       |           |
        //     line      pSectionEndChar

        //找出‘=’字符的位置
        pSectionEndChar = t_strchr(line, ']');
        if(pSectionEndChar != NULL)
        {
            //找到了‘=’字符，（pEqualChar - line）是key的长度
            copySize = min( remainSize,  pSectionEndChar - line - 1 );
        }
        else
        {
            //本行中不存在‘]’字符，对于合法文件来说不会出现此种情况
            copySize = min( remainSize, lineLength - 1 );
        }

        t_strncpy(pItem, line+1, copySize);

        //追加一个0字符
        pItem[copySize] = 0;

        //缩小缓冲区剩余字符数量remainSize，和当前写入位置pItem
        ret += (copySize + 1); //加1是为了统计结尾的0字符
        remainSize -= (copySize + 1);
        pItem += (copySize + 1);
    }

    fclose(stream);
    //再次对缓冲区追加一个0 字符
    *pItem = 0;
    return ret;
}

//
BOOL CeWritePrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,    //要修改的KEY，如果为NULL，会删除整个Section
    LPCTSTR lpString,    //要写入的值，如果为NULL，则会删除这个KEY
    LPCTSTR lpFileName
    )
{
    FILE *stream;
    void *pVoid = NULL; //文件的后半部分
    bool bFindKey = false;
    bool bFindSection = false;
    TCHAR line[ LINESIZE ];
    size_t sectionLength, keyLength, lineLength, nBytesRead = 0;
    LONG nInsertPos = -1, nCopyPos = -1, nFileEndPos, nPos; //文件指针位置
    LONG nSectionBegin = -1, nKeyBegin = -1, nNextKey = -1, nNextSection = -1;
    BYTE mode = 0;
    
    //如果 sectionName 为NULL，返回成功
    if(lpAppName == NULL)
        return true;

    //r+: Opens for both reading and writing. (The file must exist.)
    stream = t_fopen(lpFileName, _T("r+"));
    if(stream == NULL)
    {
        return false;
    }

    //先取一次mode的默认值
    if(lpKeyName == NULL)
        mode = MODE_DELETE_SECTION;
    else if(lpString == NULL)
        mode = MODE_DELETE_KEY;
    else
        mode = MODE_OVERWRITE_KEY;

    sectionLength = t_strlen(lpAppName);
    
    //每次读行前，保存文件指针位置
    while(nPos = ftell(stream), t_fgets(line, LINESIZE, stream) != NULL)
    {
        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在UNICODE环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }
        
        //尝试寻找到 section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //本行是否是 [section]
            //这里是我们想要的Section吗？
            //检查这一行的宽度是否正好是section长度加2， [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            //读取section前求出 Key 的长度
            if(lpKeyName != NULL)
                keyLength = t_strlen(lpKeyName);
            
            nSectionBegin = nPos;

            bFindSection = true;        
            continue;
        }
        
        //Section找到了，
        //Section End ?
        if(line[0]=='[') 
        {
            nNextSection = nPos;
            break; //遇到了下一个
        }

        //是否需要查找KEY？
        if(lpKeyName != NULL)
        {            
            if(lineLength < keyLength+1 || line[keyLength] != '=') continue; //"KeyName="
            if(t_strncmp(line, lpKeyName, keyLength) != 0) continue;
            //Now We Get the Key! 
            nKeyBegin = nPos;
            nNextKey = ftell(stream); //要拷贝的起始位置
            //Now It's done.
            bFindKey = true;
            break;
        }
    }

    //如果已经到达文件尾部，则追加换行
    if(feof(stream))
        t_fprintf(stream, _T("\r\n"));

    if(nNextSection < 0) nNextSection = ftell(stream);
    if(nNextKey < 0) nNextKey = ftell(stream);

    //遍历后再次更新mode值
    if(mode == MODE_DELETE_SECTION)
    {
        if(!bFindSection)
        {
            fclose(stream);
            return true;
        }
        else
        {
            nInsertPos = nSectionBegin;
            nCopyPos = nNextSection;
        }
    }
    if(mode == MODE_DELETE_KEY)
    {
        if(!bFindKey)
        {
            fclose(stream);
            return true;
        }
        else
        {
            nInsertPos = nKeyBegin;
            nCopyPos = nNextKey;
        }
    }
    if(mode == MODE_OVERWRITE_KEY)
    {
        if(!bFindSection)
        {
            mode = MODE_APPEND_SECTION;
        }
        else
        {
            if(bFindKey)
            {
                nInsertPos = nKeyBegin;
                nCopyPos = nNextKey;
            }
            else
            {
                mode = MODE_APPEND_KEY;
                nInsertPos = nNextSection;
                nCopyPos = nNextSection;
            }
        }
    }

    //追加一个新的Section
    if(mode == MODE_APPEND_SECTION)
    {
        t_fprintf(stream, _T("\r\n[%s]\r\n%s=%s\r\n"), lpAppName, lpKeyName, lpString);
        fclose(stream);
        return true;
    }

    //先把文件的后半部分拷贝到内存
    fseek(stream, 0, SEEK_END);
    nFileEndPos = ftell(stream);
    if(nCopyPos >= 0 && nCopyPos < nFileEndPos)
    {
        //分配内存作为缓冲区
        pVoid = malloc(nFileEndPos - nCopyPos + 1);

        if(pVoid == NULL)
        {
            fclose(stream);
            return false; //堆内存不足
        }
        fseek(stream, nCopyPos, SEEK_SET);
        nBytesRead = fread(pVoid, 1, nFileEndPos - nCopyPos + 1, stream);
    }

    //写入新的value值
    fseek(stream, nInsertPos, SEEK_SET);
    if(lpKeyName != NULL && lpString != NULL)
        t_fprintf(stream, _T("%s=%s\r\n"), lpKeyName, lpString);

    //现在把文件的后半部分写回文件中
    if(pVoid != NULL && nBytesRead > 0)
    {
        fwrite(pVoid, 1, nBytesRead, stream);
        free(pVoid);
    }

    //此时结尾可能还有一些内容，属于原来的ini文件
    //我们把它写成注释
    nPos = ftell(stream);
    fclose(stream);

    //如果文件变小了，那么我们需要更改文件大小
    if(nPos < nFileEndPos)
    {
#ifdef WINCE    //WINCE平台
        HANDLE handle = CreateFile(
            lpFileName, //LPCTSTR lpFileName
            GENERIC_WRITE, //DOWRD dwDesiredAccess,
            0, //DWORD dwShareMode, 非共享模式
            NULL, //LPSECURITY_ATTRIBUTES lpSecurityAttributes, ignored
            OPEN_EXISTING, //DWORD dwCreationDispostion, 
            FILE_ATTRIBUTE_NORMAL, //DWORD dwFlagsAndAttributes, 
            NULL//HANDLE hTemplateFile, ignored
            ); 

        if(handle != NULL)
        {
            //移动文件指针
            SetFilePointer(handle, nPos, NULL, FILE_BEGIN);
            //设置EOF
            SetEndOfFile(handle);
            //关闭
            CloseHandle(handle);
        }

#else            //PC 平台
        int handle = t_sopen(lpFileName, _O_RDWR, _SH_DENYRW);
        if(handle > 0)
        {
            //修改文件大小
            _chsize(handle, nPos);
            //关闭文件
            _close(handle);
        }
#endif //
    }
    return TRUE;
}


//重写某个Section，注意和 PC API 的区别是，这里不保证原子性操作
BOOL CeWritePrivateProfileSection(
    LPCTSTR lpAppName,    //section name
    LPCTSTR lpString,    //key1=val1 \0 key2=val2 \0\0
    LPCTSTR lpFileName
    )
{
    FILE *stream;
    void *pVoid = NULL; //文件的后半部分
    bool bFindSection = false;
    TCHAR line[ LINESIZE ]; //行缓冲区
    LPCTSTR pItem = lpString;
    size_t sectionLength, lineLength, nBytesRead = 0;
    LONG nFileEndPos, nPos; //文件指针位置
    LONG nSectionBegin = -1, nNextSection = -1;
    
    //如果 sectionName 为NULL，返回失败
    if(lpAppName == NULL || lpString == NULL)
        return false;

    //r+: Opens for both reading and writing. (The file must exist.)
    stream = t_fopen(lpFileName, _T("r+"));
    if(stream == NULL)
    {
        return false;
    }

    sectionLength = t_strlen(lpAppName);
    
    //每次读行前，保存文件指针位置
    while(nPos = ftell(stream), t_fgets(line, LINESIZE, stream) != NULL)
    {
        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在UNICODE环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }
        
        //尝试寻找到 section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //本行是否是 [section]
            //这里是我们想要的Section吗？
            //检查这一行的宽度是否正好是section长度加2， [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            nSectionBegin = nPos;
            bFindSection = true;        
            continue;
        }
        
        //Section找到了，
        //Section End ?
        if(line[0]=='[') 
        {
            nNextSection = nPos;
            break; //遇到了下一个
        }
    }

    //如果已经到达文件尾部，则追加换行
    if(nNextSection < 0) nNextSection = ftell(stream);

    //追加一个新的Section
    if(!bFindSection)
    {
        nSectionBegin = ftell(stream);
    }

    //覆写Section
    //先把文件的后半部分拷贝到内存
    fseek(stream, 0, SEEK_END);
    nFileEndPos = ftell(stream);
    if(nNextSection >= 0 && nNextSection < nFileEndPos)
    {
        //分配内存作为缓冲区
        pVoid = malloc(nFileEndPos - nNextSection + 1);

        if(pVoid == NULL)
        {
            fclose(stream);
            return false; //堆内存不足
        }
        fseek(stream, nNextSection, SEEK_SET);
        nBytesRead = fread(pVoid, 1, nFileEndPos - nNextSection + 1, stream);
    }
    
    //逐行写入key = val
    fseek(stream, nSectionBegin, SEEK_SET);
    //再次写入[section]，如果不存在就会追加
    t_fprintf(stream, _T("[%s]\r\n"), lpAppName);
    while(*pItem)
    {
        t_fprintf(stream, _T("%s\r\n"), pItem);
        pItem += t_strlen(pItem) + 1; //移动到下一行
    }

    //现在把文件的后半部分写回文件中
    if(pVoid != NULL)
    {
        fwrite(pVoid, 1, nBytesRead, stream);
        free(pVoid);
    }

    //此时结尾可能还有一些内容，属于原来的ini文件
    //我们把它写成注释
    nPos = ftell(stream); //当前文件位置
    fclose(stream);

    //如果文件变小了，那么我们需要更改文件大小
    if(nPos < nFileEndPos)
    {
#ifdef WINCE    //WINCE平台
        HANDLE handle = CreateFile(
            lpFileName, //LPCTSTR lpFileName
            GENERIC_WRITE, //DOWRD dwDesiredAccess,
            0, //DWORD dwShareMode, 非共享模式
            NULL, //LPSECURITY_ATTRIBUTES lpSecurityAttributes, ignored
            OPEN_EXISTING, //DWORD dwCreationDispostion, 
            FILE_ATTRIBUTE_NORMAL, //DWORD dwFlagsAndAttributes, 
            NULL//HANDLE hTemplateFile, ignored
            ); 

        if(handle != NULL)
        {
            //移动文件指针
            SetFilePointer(handle, nPos, NULL, FILE_BEGIN);
            //设置EOF
            SetEndOfFile(handle);
            //关闭
            CloseHandle(handle);
        }

#else            //PC 平台
        int handle = t_sopen(lpFileName, _O_RDWR, _SH_DENYRW);
        if(handle > 0)
        {
            //修改文件大小
            _chsize(handle, nPos);
            //关闭文件
            _close(handle);
        }
#endif //
    }
    return TRUE;
}


//===========================================================
//        以下是我增加的函数（API中没有）
//===========================================================

//
//获取某个section下的所有的Key名，
//获取某个Section下面的所有“key形式的字符串集合，以0字符分割
//结尾使用两个0字符
//缓冲区写入："key1 \0 key2 \0 \0 "
//返回值表示写入缓冲区的字符数, 不包括结尾的0字符。
//如果缓冲区不够容纳所有的键值对，则返回值 = (nSize-2)

//注意：此函数是在桌面 API 中也没有的。而是我单独添加的
//
DWORD CeGetPrivateProfileKeyNames(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,                //缓冲区的字符数
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0;                    //返回值，拷贝的字符数量
    DWORD remainSize = nSize - 2;    //缓冲区当前所能能够接纳的字符数量
    DWORD copySize;                    //本次循环中需要拷贝的字符数量
    bool bFindSection = false;        //是否已经找到Section
    TCHAR line[ LINESIZE ];            //行缓冲区
    LPTSTR pItem;                    //指向当前键值对的写入地址
    TCHAR *pEqualChar;                //等号字符的在行中的位置
    FILE *stream;                    //流指针
    size_t sectionLength, lineLength;

    
    pItem = lpReturnedString; //指向缓冲区起始地址

    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        return ret;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //缓冲区是否还有剩余空间？
        if(remainSize <= 0) break;

        //忽略注释行和空行
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //注意：把LF（0xa）字符替换成0，这在UNICODE环境下可能出现结尾是LF）
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //注意此时可能会成为空字符串
            if(lineLength == 0) continue;
        }
        
        //尝试寻找到 section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //本行是否是 [section]
            //这里是我们想要的Section吗？
            //检查这一行的宽度是否正好是section长度加2， [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            bFindSection = true;            
            continue;
        }
        
        //查找Key, Section End?
        if(line[0]=='[') break; //遇到了下一个


        //copy the keyname to buffer, 注意ncpy不会复制结尾的0字符

        //LINE: "keyName = "
        //       |       |
        //     line   pEqualChar        

        //找出‘=’字符的位置
        pEqualChar = t_strchr(line, '=');
        if(pEqualChar != NULL)
        {
            //找到了‘=’字符，（pEqualChar - line）是key的长度
            copySize = min( remainSize,  pEqualChar - line );
        }
        else
        {
            //本行中不存在‘=’字符，对于合法文件来说不会出现此种情况
            copySize = min( remainSize, lineLength );
        }

        t_strncpy(pItem, line, copySize);
        //最佳一个0字符
        pItem[copySize] = 0;

        //缩小缓冲区剩余字符数量remainSize，和当前写入位置pItem
        ret += (copySize + 1); //加1是为了统计结尾的0字符
        remainSize -= (copySize + 1);
        pItem += (copySize + 1);
    }
    
    fclose(stream);

    if(bFindSection)
    {
        //再次对缓冲区追加一个0 字符
        *pItem = 0;
    }
    return ret;
}