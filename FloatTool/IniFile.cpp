//������ char* �� UNICODE��
//�����ַ�������ʹ�� TEXT("aa") ���� _T("aa") �ĸ�ʽ���Զ���Ӧ char* �� UNICODE��
//������غ�����t_ǰ׺
//IniFile: ��ȡINI FILE�ļ򵥽���!��ν�򵥣�Ҳ���ǽ�������򵥣������ļ���ʽҪ�����
//[1]�κ��ַ���ǰ��Ҫ�пո�ʹ����������Բ�����ǰ���trim��
//            ��������"Key1=Val", ��������" Key1 = Val "
//[2]������ע�ͣ���һ���ַ�������Ӣ�ķֺ�';'
//
#include "StdAfx.h"
#include "IniFile.h"

//��appname��section���ж�ȡstring����key
DWORD CeGetPrivateProfileString(
    LPCTSTR lpAppName,                //section name: [lpAppName]
    LPCTSTR lpKeyName,                //lpKeyName=lpReturnedString
    LPCTSTR lpDefault,                //δ�ҵ�ʱ��Ĭ��ֵ
    LPTSTR lpReturnedString,    //[out] ���ҵ��Ľ��
    DWORD nSize,                            //[in]lpReturnedString���ַ���,ע�ⵥλ�����ֽڣ�
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
        //����Ĭ��ֵ
        t_strcpy(lpReturnedString, lpDefault);
        ret = t_strlen(lpReturnedString); 
        return ret;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������UNICODE�����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }
        
        //����Ѱ�ҵ� section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //�����Ƿ��� [section]
            //������������Ҫ��Section��
            //�����һ�еĿ���Ƿ�������section���ȼ�2�� [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            //��ȡsectionǰ��� Key �ĳ���
            keyLength = t_strlen(lpKeyName);
            bFindSection = true;            
            continue;
        }
        
        //����Key, Section End?
        if(line[0]=='[') break; //��������һ��
            
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
        //����Ĭ��ֵ
        t_strcpy(lpReturnedString, lpDefault); 
    }
    ret = t_strlen(lpReturnedString); 
    return ret;
}


//��ȡһ��intֵ
UINT CeGetPrivateProfileInt(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,
    int nDefault,
    LPCTSTR lpFileName
    )
{
    long ret = nDefault; //����ֵ
    FILE *stream;
    bool bFindVal = false;
    bool bFindSection = false;
    TCHAR line[ LINESIZE ];
    size_t sectionLength, keyLength, lineLength;
    
    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        //����Ĭ��ֵ
        return nDefault;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������UNICODE�����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }
        
        //����Ѱ�ҵ� section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //�����Ƿ��� [section]
            //������������Ҫ��Section��
            //�����һ�еĿ���Ƿ�������section���ȼ�2�� [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            //��ȡsectionǰ��� Key �ĳ���
            keyLength = t_strlen(lpKeyName);
            bFindSection = true;            
            continue;
        }
        
        //����Key, Section End?
        if(line[0]=='[') break; //��������һ��
            
        if(lineLength < keyLength+1 || line[keyLength] != '=') continue; //"KeyName="
        if(t_strncmp(line, lpKeyName, keyLength)!=0) continue;
        //Now We Get the Key! 
        TCHAR *pStopChar = NULL;
        ret = t_strtol(line + keyLength + 1, &pStopChar, 10); //Ĭ��Ϊ10����
        //Now It's done.
        bFindVal = true;
        break;
    }
    
    fclose(stream);
    return ret;
}


//��ȡĳ��Section��������С�key=value����ʽ���ַ������ϣ���0�ַ��ָ�
//��βʹ������0�ַ�
//������д�룺"key1=value1 \0 key2=value2 \0 \0 "
//����ֵ��ʾд�뻺�������ַ���, ��������β��0�ַ���
//��������������������еļ�ֵ�ԣ��򷵻�ֵ = (nSize-2)
DWORD CeGetPrivateProfileSection(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,                //���������ַ���
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0; //����ֵ���������ַ�����
    DWORD remainSize = nSize - 2; //��������ǰ�����ܹ����ɵ��ַ�����
    DWORD copySize;                //����ѭ������Ҫ�������ַ�����
    FILE *stream;
    bool bFindSection = false; //�Ƿ��Ѿ��ҵ�Section
    TCHAR line[ LINESIZE ];   //�л�����
    LPTSTR pItem; //ָ��ǰ��ֵ�Ե�д���ַ
    size_t sectionLength, lineLength;
    

    pItem = lpReturnedString; //ָ�򻺳�����ʼ��ַ

    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        return ret;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //�������Ƿ���ʣ��ռ䣿
        if(remainSize <= 0) break;

        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������UNICODE�����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }
        
        //����Ѱ�ҵ� section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //�����Ƿ��� [section]
            //������������Ҫ��Section��
            //�����һ�еĿ���Ƿ�������section���ȼ�2�� [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            bFindSection = true;            
            continue;
        }
        
        //����Key, Section End?
        if(line[0]=='[') break; //��������һ��

        //copy the line to buffer, ע��ncpy���Ḵ�ƽ�β��0�ַ�
        copySize = min( remainSize, lineLength );
        t_strncpy(pItem, line, copySize);
        //׷��һ��0�ַ�
        pItem[copySize] = 0;

        //��С������ʣ���ַ�����remainSize���͵�ǰд��λ��pItem
        ret += (copySize + 1); //��1��Ϊ��ͳ�ƽ�β��0�ַ�
        remainSize -= (copySize + 1);
        pItem += (copySize + 1);
    }
    
    fclose(stream);

    if(bFindSection)
    {
        //�ٴζԻ�����׷��һ��0 �ַ�
        *pItem = 0;
    }
    return ret;
}


//��ȡһ��ini�ļ�������section��name��������������
//ע���ϵͳAPI�������ǣ�ϵͳAPI�Ķ�ȡ��ԭ���Եģ�����ȡʱ�������޸�ini�ļ�������
//�����ǵĺ���δ�ر�֤��һ��
DWORD CeGetPrivateProfileSectionNames(
    LPTSTR lpszReturnBuffer,
    DWORD nSize,
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0;                    //����ֵ���������ַ�����
    DWORD remainSize = nSize - 2;    //��������ǰ�����ܹ����ɵ��ַ�����
    DWORD copySize;                    //����ѭ������Ҫ�������ַ�����
    TCHAR line[ LINESIZE ];            //�л�����
    TCHAR *pSectionEndChar;            //']'�ַ�ָ��
    LPTSTR pItem;                    //ָ��ǰ��ֵ�Ե�д���ַ
    FILE *stream;                    //��ָ��
    size_t lineLength;                //���ַ�����
    
    pItem = lpszReturnBuffer; //ָ�򻺳�����ʼ��ַ

    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        return ret;
    }
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //�������Ƿ���ʣ��ռ䣿
        if(remainSize <= 0) break;

        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������ UNICODE �����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }

        if(line[0] != '[') continue; //�����Ƿ��� [section]
        //�ҵ���һ��Section����ʼ����
        //copy the section name to buffer, ע��ncpy���Ḵ�ƽ�β��0�ַ�

        //LINE: "[sectionName]"
        //       |           |
        //     line      pSectionEndChar

        //�ҳ���=���ַ���λ��
        pSectionEndChar = t_strchr(line, ']');
        if(pSectionEndChar != NULL)
        {
            //�ҵ��ˡ�=���ַ�����pEqualChar - line����key�ĳ���
            copySize = min( remainSize,  pSectionEndChar - line - 1 );
        }
        else
        {
            //�����в����ڡ�]���ַ������ںϷ��ļ���˵������ִ������
            copySize = min( remainSize, lineLength - 1 );
        }

        t_strncpy(pItem, line+1, copySize);

        //׷��һ��0�ַ�
        pItem[copySize] = 0;

        //��С������ʣ���ַ�����remainSize���͵�ǰд��λ��pItem
        ret += (copySize + 1); //��1��Ϊ��ͳ�ƽ�β��0�ַ�
        remainSize -= (copySize + 1);
        pItem += (copySize + 1);
    }

    fclose(stream);
    //�ٴζԻ�����׷��һ��0 �ַ�
    *pItem = 0;
    return ret;
}

//
BOOL CeWritePrivateProfileString(
    LPCTSTR lpAppName,
    LPCTSTR lpKeyName,    //Ҫ�޸ĵ�KEY�����ΪNULL����ɾ������Section
    LPCTSTR lpString,    //Ҫд���ֵ�����ΪNULL�����ɾ�����KEY
    LPCTSTR lpFileName
    )
{
    FILE *stream;
    void *pVoid = NULL; //�ļ��ĺ�벿��
    bool bFindKey = false;
    bool bFindSection = false;
    TCHAR line[ LINESIZE ];
    size_t sectionLength, keyLength, lineLength, nBytesRead = 0;
    LONG nInsertPos = -1, nCopyPos = -1, nFileEndPos, nPos; //�ļ�ָ��λ��
    LONG nSectionBegin = -1, nKeyBegin = -1, nNextKey = -1, nNextSection = -1;
    BYTE mode = 0;
    
    //��� sectionName ΪNULL�����سɹ�
    if(lpAppName == NULL)
        return true;

    //r+: Opens for both reading and writing. (The file must exist.)
    stream = t_fopen(lpFileName, _T("r+"));
    if(stream == NULL)
    {
        return false;
    }

    //��ȡһ��mode��Ĭ��ֵ
    if(lpKeyName == NULL)
        mode = MODE_DELETE_SECTION;
    else if(lpString == NULL)
        mode = MODE_DELETE_KEY;
    else
        mode = MODE_OVERWRITE_KEY;

    sectionLength = t_strlen(lpAppName);
    
    //ÿ�ζ���ǰ�������ļ�ָ��λ��
    while(nPos = ftell(stream), t_fgets(line, LINESIZE, stream) != NULL)
    {
        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������UNICODE�����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }
        
        //����Ѱ�ҵ� section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //�����Ƿ��� [section]
            //������������Ҫ��Section��
            //�����һ�еĿ���Ƿ�������section���ȼ�2�� [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            //��ȡsectionǰ��� Key �ĳ���
            if(lpKeyName != NULL)
                keyLength = t_strlen(lpKeyName);
            
            nSectionBegin = nPos;

            bFindSection = true;        
            continue;
        }
        
        //Section�ҵ��ˣ�
        //Section End ?
        if(line[0]=='[') 
        {
            nNextSection = nPos;
            break; //��������һ��
        }

        //�Ƿ���Ҫ����KEY��
        if(lpKeyName != NULL)
        {            
            if(lineLength < keyLength+1 || line[keyLength] != '=') continue; //"KeyName="
            if(t_strncmp(line, lpKeyName, keyLength) != 0) continue;
            //Now We Get the Key! 
            nKeyBegin = nPos;
            nNextKey = ftell(stream); //Ҫ��������ʼλ��
            //Now It's done.
            bFindKey = true;
            break;
        }
    }

    //����Ѿ������ļ�β������׷�ӻ���
    if(feof(stream))
        t_fprintf(stream, _T("\r\n"));

    if(nNextSection < 0) nNextSection = ftell(stream);
    if(nNextKey < 0) nNextKey = ftell(stream);

    //�������ٴθ���modeֵ
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

    //׷��һ���µ�Section
    if(mode == MODE_APPEND_SECTION)
    {
        t_fprintf(stream, _T("\r\n[%s]\r\n%s=%s\r\n"), lpAppName, lpKeyName, lpString);
        fclose(stream);
        return true;
    }

    //�Ȱ��ļ��ĺ�벿�ֿ������ڴ�
    fseek(stream, 0, SEEK_END);
    nFileEndPos = ftell(stream);
    if(nCopyPos >= 0 && nCopyPos < nFileEndPos)
    {
        //�����ڴ���Ϊ������
        pVoid = malloc(nFileEndPos - nCopyPos + 1);

        if(pVoid == NULL)
        {
            fclose(stream);
            return false; //���ڴ治��
        }
        fseek(stream, nCopyPos, SEEK_SET);
        nBytesRead = fread(pVoid, 1, nFileEndPos - nCopyPos + 1, stream);
    }

    //д���µ�valueֵ
    fseek(stream, nInsertPos, SEEK_SET);
    if(lpKeyName != NULL && lpString != NULL)
        t_fprintf(stream, _T("%s=%s\r\n"), lpKeyName, lpString);

    //���ڰ��ļ��ĺ�벿��д���ļ���
    if(pVoid != NULL && nBytesRead > 0)
    {
        fwrite(pVoid, 1, nBytesRead, stream);
        free(pVoid);
    }

    //��ʱ��β���ܻ���һЩ���ݣ�����ԭ����ini�ļ�
    //���ǰ���д��ע��
    nPos = ftell(stream);
    fclose(stream);

    //����ļ���С�ˣ���ô������Ҫ�����ļ���С
    if(nPos < nFileEndPos)
    {
#ifdef WINCE    //WINCEƽ̨
        HANDLE handle = CreateFile(
            lpFileName, //LPCTSTR lpFileName
            GENERIC_WRITE, //DOWRD dwDesiredAccess,
            0, //DWORD dwShareMode, �ǹ���ģʽ
            NULL, //LPSECURITY_ATTRIBUTES lpSecurityAttributes, ignored
            OPEN_EXISTING, //DWORD dwCreationDispostion, 
            FILE_ATTRIBUTE_NORMAL, //DWORD dwFlagsAndAttributes, 
            NULL//HANDLE hTemplateFile, ignored
            ); 

        if(handle != NULL)
        {
            //�ƶ��ļ�ָ��
            SetFilePointer(handle, nPos, NULL, FILE_BEGIN);
            //����EOF
            SetEndOfFile(handle);
            //�ر�
            CloseHandle(handle);
        }

#else            //PC ƽ̨
        int handle = t_sopen(lpFileName, _O_RDWR, _SH_DENYRW);
        if(handle > 0)
        {
            //�޸��ļ���С
            _chsize(handle, nPos);
            //�ر��ļ�
            _close(handle);
        }
#endif //
    }
    return TRUE;
}


//��дĳ��Section��ע��� PC API �������ǣ����ﲻ��֤ԭ���Բ���
BOOL CeWritePrivateProfileSection(
    LPCTSTR lpAppName,    //section name
    LPCTSTR lpString,    //key1=val1 \0 key2=val2 \0\0
    LPCTSTR lpFileName
    )
{
    FILE *stream;
    void *pVoid = NULL; //�ļ��ĺ�벿��
    bool bFindSection = false;
    TCHAR line[ LINESIZE ]; //�л�����
    LPCTSTR pItem = lpString;
    size_t sectionLength, lineLength, nBytesRead = 0;
    LONG nFileEndPos, nPos; //�ļ�ָ��λ��
    LONG nSectionBegin = -1, nNextSection = -1;
    
    //��� sectionName ΪNULL������ʧ��
    if(lpAppName == NULL || lpString == NULL)
        return false;

    //r+: Opens for both reading and writing. (The file must exist.)
    stream = t_fopen(lpFileName, _T("r+"));
    if(stream == NULL)
    {
        return false;
    }

    sectionLength = t_strlen(lpAppName);
    
    //ÿ�ζ���ǰ�������ļ�ָ��λ��
    while(nPos = ftell(stream), t_fgets(line, LINESIZE, stream) != NULL)
    {
        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������UNICODE�����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }
        
        //����Ѱ�ҵ� section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //�����Ƿ��� [section]
            //������������Ҫ��Section��
            //�����һ�еĿ���Ƿ�������section���ȼ�2�� [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            nSectionBegin = nPos;
            bFindSection = true;        
            continue;
        }
        
        //Section�ҵ��ˣ�
        //Section End ?
        if(line[0]=='[') 
        {
            nNextSection = nPos;
            break; //��������һ��
        }
    }

    //����Ѿ������ļ�β������׷�ӻ���
    if(nNextSection < 0) nNextSection = ftell(stream);

    //׷��һ���µ�Section
    if(!bFindSection)
    {
        nSectionBegin = ftell(stream);
    }

    //��дSection
    //�Ȱ��ļ��ĺ�벿�ֿ������ڴ�
    fseek(stream, 0, SEEK_END);
    nFileEndPos = ftell(stream);
    if(nNextSection >= 0 && nNextSection < nFileEndPos)
    {
        //�����ڴ���Ϊ������
        pVoid = malloc(nFileEndPos - nNextSection + 1);

        if(pVoid == NULL)
        {
            fclose(stream);
            return false; //���ڴ治��
        }
        fseek(stream, nNextSection, SEEK_SET);
        nBytesRead = fread(pVoid, 1, nFileEndPos - nNextSection + 1, stream);
    }
    
    //����д��key = val
    fseek(stream, nSectionBegin, SEEK_SET);
    //�ٴ�д��[section]����������ھͻ�׷��
    t_fprintf(stream, _T("[%s]\r\n"), lpAppName);
    while(*pItem)
    {
        t_fprintf(stream, _T("%s\r\n"), pItem);
        pItem += t_strlen(pItem) + 1; //�ƶ�����һ��
    }

    //���ڰ��ļ��ĺ�벿��д���ļ���
    if(pVoid != NULL)
    {
        fwrite(pVoid, 1, nBytesRead, stream);
        free(pVoid);
    }

    //��ʱ��β���ܻ���һЩ���ݣ�����ԭ����ini�ļ�
    //���ǰ���д��ע��
    nPos = ftell(stream); //��ǰ�ļ�λ��
    fclose(stream);

    //����ļ���С�ˣ���ô������Ҫ�����ļ���С
    if(nPos < nFileEndPos)
    {
#ifdef WINCE    //WINCEƽ̨
        HANDLE handle = CreateFile(
            lpFileName, //LPCTSTR lpFileName
            GENERIC_WRITE, //DOWRD dwDesiredAccess,
            0, //DWORD dwShareMode, �ǹ���ģʽ
            NULL, //LPSECURITY_ATTRIBUTES lpSecurityAttributes, ignored
            OPEN_EXISTING, //DWORD dwCreationDispostion, 
            FILE_ATTRIBUTE_NORMAL, //DWORD dwFlagsAndAttributes, 
            NULL//HANDLE hTemplateFile, ignored
            ); 

        if(handle != NULL)
        {
            //�ƶ��ļ�ָ��
            SetFilePointer(handle, nPos, NULL, FILE_BEGIN);
            //����EOF
            SetEndOfFile(handle);
            //�ر�
            CloseHandle(handle);
        }

#else            //PC ƽ̨
        int handle = t_sopen(lpFileName, _O_RDWR, _SH_DENYRW);
        if(handle > 0)
        {
            //�޸��ļ���С
            _chsize(handle, nPos);
            //�ر��ļ�
            _close(handle);
        }
#endif //
    }
    return TRUE;
}


//===========================================================
//        �����������ӵĺ�����API��û�У�
//===========================================================

//
//��ȡĳ��section�µ����е�Key����
//��ȡĳ��Section��������С�key��ʽ���ַ������ϣ���0�ַ��ָ�
//��βʹ������0�ַ�
//������д�룺"key1 \0 key2 \0 \0 "
//����ֵ��ʾд�뻺�������ַ���, ��������β��0�ַ���
//��������������������еļ�ֵ�ԣ��򷵻�ֵ = (nSize-2)

//ע�⣺�˺����������� API ��Ҳû�еġ������ҵ�����ӵ�
//
DWORD CeGetPrivateProfileKeyNames(
    LPCTSTR lpAppName,
    LPTSTR lpReturnedString,
    DWORD nSize,                //���������ַ���
    LPCTSTR lpFileName
    )
{
    DWORD ret = 0;                    //����ֵ���������ַ�����
    DWORD remainSize = nSize - 2;    //��������ǰ�����ܹ����ɵ��ַ�����
    DWORD copySize;                    //����ѭ������Ҫ�������ַ�����
    bool bFindSection = false;        //�Ƿ��Ѿ��ҵ�Section
    TCHAR line[ LINESIZE ];            //�л�����
    LPTSTR pItem;                    //ָ��ǰ��ֵ�Ե�д���ַ
    TCHAR *pEqualChar;                //�Ⱥ��ַ��������е�λ��
    FILE *stream;                    //��ָ��
    size_t sectionLength, lineLength;

    
    pItem = lpReturnedString; //ָ�򻺳�����ʼ��ַ

    stream = t_fopen(lpFileName, _T("r"));
    if(stream == NULL)
    {
        return ret;
    }
    
    sectionLength = t_strlen(lpAppName);
    
    while(t_fgets(line, LINESIZE, stream) != NULL)
    {
        //�������Ƿ���ʣ��ռ䣿
        if(remainSize <= 0) break;

        //����ע���кͿ���
        if(line[0] == 0 || line[0] == ';') continue;
        lineLength = t_strlen(line);
        //ע�⣺��LF��0xa���ַ��滻��0������UNICODE�����¿��ܳ��ֽ�β��LF��
        if(line[ lineLength - 1 ] == 0x0a)
        {
            line[ lineLength - 1 ] = 0;
            lineLength--;
            //ע���ʱ���ܻ��Ϊ���ַ���
            if(lineLength == 0) continue;
        }
        
        //����Ѱ�ҵ� section
        if(!bFindSection)
        {
            if(line[0] != '[') continue; //�����Ƿ��� [section]
            //������������Ҫ��Section��
            //�����һ�еĿ���Ƿ�������section���ȼ�2�� [lpAppName]
            if(line[sectionLength + 1] != ']') continue;
            if(t_strncmp(line+1, lpAppName, sectionLength) != 0) continue;
            //Now Section will appear on next line 
            bFindSection = true;            
            continue;
        }
        
        //����Key, Section End?
        if(line[0]=='[') break; //��������һ��


        //copy the keyname to buffer, ע��ncpy���Ḵ�ƽ�β��0�ַ�

        //LINE: "keyName = "
        //       |       |
        //     line   pEqualChar        

        //�ҳ���=���ַ���λ��
        pEqualChar = t_strchr(line, '=');
        if(pEqualChar != NULL)
        {
            //�ҵ��ˡ�=���ַ�����pEqualChar - line����key�ĳ���
            copySize = min( remainSize,  pEqualChar - line );
        }
        else
        {
            //�����в����ڡ�=���ַ������ںϷ��ļ���˵������ִ������
            copySize = min( remainSize, lineLength );
        }

        t_strncpy(pItem, line, copySize);
        //���һ��0�ַ�
        pItem[copySize] = 0;

        //��С������ʣ���ַ�����remainSize���͵�ǰд��λ��pItem
        ret += (copySize + 1); //��1��Ϊ��ͳ�ƽ�β��0�ַ�
        remainSize -= (copySize + 1);
        pItem += (copySize + 1);
    }
    
    fclose(stream);

    if(bFindSection)
    {
        //�ٴζԻ�����׷��һ��0 �ַ�
        *pItem = 0;
    }
    return ret;
}