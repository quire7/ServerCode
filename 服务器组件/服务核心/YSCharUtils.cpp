#include "stdafx.h"
#include "YSCharUtils.h"

CYSCharUtils::CYSCharUtils()
{
}


CYSCharUtils::~CYSCharUtils()
{
}

//ANSI to Unicode
CStringW CYSCharUtils::ANSIToUnicode(const CStringA& str)
{
	int len = str.GetLength();

	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	wchar_t * pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, str, - 1, (LPWSTR)pUnicode, unicodeLen);

	CStringW rt = (wchar_t*)pUnicode;
	delete pUnicode;
	pUnicode = NULL;
	return rt;
}

//Unicode to ANSI
CStringA CYSCharUtils::UnicodeToANSI(const CStringW& str)
{
	// wide char to multi char
	int iTextLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char* pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP, 0, str, - 1, pElementText, iTextLen, NULL, NULL);
	CStringA strText = pElementText;
	delete[] pElementText;
	pElementText = NULL;
	return strText;
}

//Convert to ANSI
CStringA CYSCharUtils::ConvertToANSI(const CString& str)
{
#ifdef UNICODE
	return UnicodeToANSI(str);
#else
	return str;
#endif // UNICODE
}

//Convert to UTF-8;
CStringA CYSCharUtils::ConvertToUTF8(const CString& str)
{
#ifdef UNICODE
	return UnicodeToUTF8(str);
#else
	return  CYSCharUtils::UnicodeToUTF8(ANSIToUnicode(str));
#endif // UNICODE
}


//UTF-8 to Unicode
CStringW CYSCharUtils::UTF8ToUnicode(const CStringA& str)
{
	int unicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, str,	-1, NULL, 0);
	wchar_t *pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, str, -1, (LPWSTR)pUnicode, unicodeLen);
	CStringW rt = (wchar_t*)pUnicode;
	delete pUnicode;
	pUnicode = NULL;
	return rt;
}

//Unicode to UTF-8
CStringA CYSCharUtils::UnicodeToUTF8(const CStringW& str)
{
	// wide char to multi char
	int iTextLen = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	char* pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_UTF8, 0, str, -1, pElementText, iTextLen, NULL, NULL);
	CStringA strText = pElementText;
	delete[] pElementText;
	pElementText = NULL;
	return strText;
}

void CYSCharUtils::GB2312ToUtf8(LPCTSTR lpcSrcStr, CString& strUtf8Out)
{
	USES_CONVERSION;
	LPCWSTR pwcStrData = A2CW((LPCSTR)lpcSrcStr);

	strUtf8Out = CYSCharUtils::UnicodeToUTF8(pwcStrData);
}

CString CYSCharUtils::UTF8ToCString(const char* utf8Str)
{
#ifdef UNICODE
	return UTF8ToUnicode( utf8Str );
#else
	return UnicodeToANSI( UTF8ToUnicode(utf8Str) );
#endif
}
