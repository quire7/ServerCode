#include "stdafx.h"

void SaveLog(LPCTSTR pstrFormat, ...)
{
	CString strText;
	try
	{
		// format and write the data we were given
		va_list args;
		va_start(args, pstrFormat);
		strText.FormatV(pstrFormat, args);
		va_end(args);
	}
	catch(CMemoryException *e)
	{
		e->Delete();
	}
	catch(...)
	{
		return ;
	}

	static CString strFilePath;
	if (strFilePath.IsEmpty())
	{
		TCHAR *szPath=strFilePath.GetBuffer(MAX_PATH);
		GetModuleFileName(AfxGetInstanceHandle(),szPath,MAX_PATH);
		PathRemoveFileSpec(szPath);
		strFilePath.ReleaseBuffer();
	}

	CTime cTime = CTime::GetCurrentTime();

	CFile mFile;
	if (mFile.Open(strFilePath+cTime.Format(_T("\\Log\\WS_%y-%m-%d.txt")),CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite,NULL)==TRUE)
	{
		mFile.SeekToEnd();
		strText.Insert(0, cTime.Format(_T("[%H:%M:%S]")));
		strText += _T("\r\n");
		mFile.Write(strText.GetBuffer(), strText.GetLength());
		mFile.Close();
	}
}

short AfxHexValue(char chIn)
{
	unsigned char ch = (unsigned char)chIn;
	if (ch >= '0' && ch <= '9')
		return (short)(ch - '0');
	if (ch >= 'A' && ch <= 'F')
		return (short)(ch - 'A' + 10);
	if (ch >= 'a' && ch <= 'f')
		return (short)(ch - 'a' + 10);
	return -1;
}

BOOL AfxIsUnsafeUrlChar(char chIn)
{
	unsigned char ch = (unsigned char)chIn;
	switch(ch)
	{
		case ';': case '\\': case '?': case '@': case '&':
		case '=': case '+': case '$': case ',': case ' ':
		case '<': case '>': case '#': case '%': case '\"':
		case '{': case '}': case '|':
		case '^': case '[': case ']': case '`':
			return TRUE;
		default:
		{
			if (ch < 32 || ch > 126)
				return TRUE;
			return FALSE;
		}
	}
}

CString URLDecode(LPCTSTR lpszURL)
{
	CString strResult = _T("");

	// Convert all escaped characters in lpszURL to their real values
	int nValue = 0;
	char ch;
	BOOL bContinue = TRUE;
	while ((ch = *lpszURL) != 0)
	{
		if (bContinue)
		{
			if (ch == '%')
			{
				if ((*(lpszURL+1) == '\0') || (*(lpszURL+2) == '\0'))
				{
					bContinue = FALSE;
					break;
				}
				ch = *(++lpszURL);
				
				// currently assuming 2 hex values after '%' as per the RFC 2396 document
				nValue = 16*AfxHexValue(ch);
				nValue+= AfxHexValue(*(++lpszURL));
				strResult += (char)nValue;
			}
			else 
			// non-escape character
			{
				if (bContinue)
					strResult += ch;
			}
		}
		lpszURL++;
	}
	// replace '+' with " "
    strResult.Replace(_T("+"), _T(" "));
	
	return strResult;
}

enum
{
	UNICODE_CALC_SIZE = 1,
	UNICODE_GET_BYTES = 2
};

//将unicode转义字符序列转换为内存中的unicode字符串
int unicode_bytes(char* p_unicode_escape_chars, wchar_t *bytes, int flag)
{
	int unicode_count = 0;
	int length = strlen(p_unicode_escape_chars);

	for (int char_index = 0; char_index<length; char_index++)
	{
		char unicode_hex[5];
		memset(unicode_hex, 0, 5);

		char ascii[2];
		memset(ascii, 0, 2);

		if (*(p_unicode_escape_chars + char_index) == '\\')
		{
			char_index++;
			if (char_index<length)
			{
				if (*(p_unicode_escape_chars + char_index) == 'u')
				{
					if (flag == UNICODE_GET_BYTES)
					{
						memcpy(unicode_hex, p_unicode_escape_chars + char_index + 1, 4);

						//sscanf不可以使用unsigned short类型
						//否则：Run-Time Check Failure #2 - Stack around the variable 'a' was corrupted.

						unsigned int a = 0;
						sscanf_s(unicode_hex, "%04x", &a);
						bytes[unicode_count++] = a;
					}
					else if (flag == UNICODE_CALC_SIZE)
					{
						unicode_count++;
					}

					char_index += 4;
				}
			}
		}
		else
		{
			if (flag == UNICODE_GET_BYTES)
			{
				memcpy(ascii, p_unicode_escape_chars + char_index, 1);
				unsigned int a = 0;
				sscanf_s(ascii, "%c", &a);
				bytes[unicode_count++] = a;
			}
			else if (flag == UNICODE_CALC_SIZE)
			{
				unicode_count++;
			}
		}
	}

	return unicode_count;
}

string UnEscape(char* p_unicode_escape_chars)
{
	int nBytes = unicode_bytes(p_unicode_escape_chars, NULL, UNICODE_CALC_SIZE);

	wchar_t *p_bytes = new wchar_t[nBytes + sizeof(wchar_t)];
	unicode_bytes(p_unicode_escape_chars, p_bytes, UNICODE_GET_BYTES);
	p_bytes[nBytes] = 0;

	USES_CONVERSION;
	string cs_return = W2A((wchar_t*)p_bytes);

	delete[] p_bytes;
	return cs_return;
}