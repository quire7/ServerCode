#ifndef YS_CHAR_UTILS_HEAD_FILE
#define YS_CHAR_UTILS_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//×Ö·û¼¯×ª»»
class SERVICE_CORE_CLASS CYSCharUtils
{
public:
	CYSCharUtils();
	~CYSCharUtils();

public:
	//ANSI to Unicode
	static CStringW ANSIToUnicode(const CStringA& str);

	//Unicode to ANSI
	static CStringA UnicodeToANSI(const CStringW& str);

	//Convert to ANSI
	static CStringA ConvertToANSI(const CString& str);

	//UTF-8 to Unicode
	static CStringW UTF8ToUnicode(const CStringA& str);

	//Unicode to UTF-8
	static CStringA UnicodeToUTF8(const CStringW& str);

	//GB2312 to UTF-8
	static void GB2312ToUtf8(LPCTSTR lpcSrcStr, CString& strUtf8Out);

	//Convert to UTF-8;
	static CStringA ConvertToUTF8(const CString& str);

	//UTF-8 to CString;
	static CString	UTF8ToCString(const char* utf8Str);
};

//////////////////////////////////////////////////////////////////////////////////

#endif