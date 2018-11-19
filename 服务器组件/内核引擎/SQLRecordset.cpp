#include "StdAfx.h"
#include "SQLRecordset.h"
#include "DataBase.h"

SQLRecordset::SQLRecordset(CDataBaseHelper& dbHelper, bool bFetchFirst/* = true*/)
: m_pFields(NULL)
, m_nFieldCount(0)
, m_nRowCount(0)
, m_nCurrentRow(0)
, m_pRecordset(NULL)
{
	Initial(dbHelper->GetMYSQL());
	if ( bFetchFirst )
	{
		Fetch();
	}
}

SQLRecordset::SQLRecordset(MYSQL* pConn)
: m_pFields(NULL)
, m_nFieldCount(0)
, m_nRowCount(0)
, m_nCurrentRow(0)
, m_pRecordset(NULL)
{
	Initial(pConn);
}

SQLRecordset::SQLRecordset(MYSQL_RES* pRes)
: m_pFields(NULL)
, m_nFieldCount(0)
, m_nRowCount(0)
, m_nCurrentRow(0)
, m_pRecordset(NULL)
{
	Initial(pRes);
}

SQLRecordset::SQLRecordset()
: m_pFields(NULL)
, m_nFieldCount(0)
, m_nRowCount(0)
, m_nCurrentRow(0)
, m_pRecordset(NULL)
{
}

SQLRecordset::~SQLRecordset()
{
	if (m_pRecordset)
	{
		mysql_free_result(m_pRecordset);
		m_pRecordset = NULL;
	}
}

BOOL SQLRecordset::FetchAll()
{
	while (m_nRowCount > m_vtRows.size())
	{
		if (!Fetch())
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL SQLRecordset::Fetch()
{
	if (!m_pRecordset)
	{
		return FALSE;
	}

	MYSQL_ROW pRow = mysql_fetch_row(m_pRecordset);
	unsigned long* pLengths = mysql_fetch_lengths(m_pRecordset);
	if (pRow == NULL || pLengths == NULL)
	{
		return FALSE;
	}

	m_nCurrentRow = m_vtRows.size();
	m_vtRows.push_back(pRow);
	m_vtLengths.push_back(pLengths);
	return TRUE;
}

unsigned long SQLRecordset::GetFieldByName(const char* szName)
{
	if (szName == NULL)
	{
		return 0xFFFFFFFF;
	}

	for (unsigned long i = 0; i < m_nFieldCount; ++i)
	{
		if (_strcmpi(m_pFields[i].name, szName) == 0)
		{
			return i;
		}
	}
	return 0xFFFFFFFF;
}

unsigned long SQLRecordset::GetFieldCount()
{
	return m_nFieldCount;
}

unsigned long SQLRecordset::GetCount()
{
	return m_nRowCount;
}

char* SQLRecordset::Get(unsigned long field)
{
	if (m_nCurrentRow >= m_vtRows.size())
	{
		return "";
	}

	if (field >= m_nFieldCount)
	{
		return "";
	}

	char* pResult = m_vtRows[m_nCurrentRow][field];
	if (pResult != NULL)
	{
		return pResult;
	}

	return "";
}

char* SQLRecordset::Get(const char* szFieldName)
{
	return Get(GetFieldByName(szFieldName));
}

int SQLRecordset::GetInt32(unsigned long field)
{
	char* szVal = Get(field);
	if (szVal == NULL)
	{
		return 0;
	}

	return atoi(szVal);
}

__int64 SQLRecordset::GetInt64(unsigned long field)
{
	char* szVal = Get(field);
	if (szVal == NULL)
	{
		return 0;
	}

	return _atoi64(szVal);
}

float SQLRecordset::GetFloat(unsigned long field)
{
	char* szVal = Get(field);
	if (szVal == NULL)
	{
		return 0;
	}

	return (float)atof(szVal);
}

Blob SQLRecordset::GetBlob(unsigned long field)
{
	Blob objResult;
	char* szVal = Get(field);
	if (szVal != NULL)
	{
		objResult.buf = szVal;
		objResult.len = m_vtLengths[m_nCurrentRow][field];
	}
	return objResult;
}

int SQLRecordset::GetInt32ByName(const char *filed)
{
	if (!filed)
	{
		return 0xFFFFFFFF;
	}

	int idx = GetFieldByName(filed);
	if (idx == 0xFFFFFFFF)
	{
		return 0xFFFFFFFF;
	}

	return GetInt32(idx);
}

__int64 SQLRecordset::GetInt64ByName(const char *filed)
{
	if (!filed)
	{
		return ErrorSQLRS;
	}

	int idx = GetFieldByName(filed);
	if (idx == ErrorSQLRS)
	{
		return ErrorSQLRS;
	}

	return GetInt64(idx);
}

float SQLRecordset::GetFloatByName(const char *filed)
{
	if (!filed)
	{
		return 0.0f;
	}

	int idx = GetFieldByName(filed);
	if (idx == ErrorSQLRS)
	{
		return 0.0f;
	}

	return GetFloat(idx);
}

char *SQLRecordset::GetStrByName(const char *filed)
{
	if (!filed)
	{
		return 0;
	}

	int idx = GetFieldByName(filed);
	if (idx == ErrorSQLRS)
	{
		return 0;
	}

	return GetStr(idx);
}

void SQLRecordset::GetBlobByName(const char *filed, Blob &ret)
{
	if (!filed)
	{
		return;
	}

	int idx = GetFieldByName(filed);
	if (idx == 0xFFFFFFFF)
	{
		return;
	}

	char *str = Get(idx);
	if (str)
	{
		ret.buf = str;
		ret.len = m_vtLengths[m_nCurrentRow][idx];
	}
}

//获取数据
INT SQLRecordset::GetValue_INT(const char * pszItem)
{
	INT nRet = GetInt32ByName(pszItem);
	return nRet;
}

//获取数据
UINT SQLRecordset::GetValue_UINT(const char * pszItem)
{
	UINT uRet = GetInt32ByName(pszItem);
	return uRet;
}

//获取数据
LONG SQLRecordset::GetValue_LONG(const char * pszItem)
{
	LONG lRet = GetInt32ByName(pszItem);
	return lRet;
}

//获取数据
BYTE SQLRecordset::GetValue_BYTE(const char * pszItem)
{
	BYTE cbRet = GetInt32ByName(pszItem);
	return cbRet;
}

//获取数据
WORD SQLRecordset::GetValue_WORD(const char * pszItem)
{
	WORD wRet = GetInt32ByName(pszItem);
	return wRet;
}

//获取数据
DWORD SQLRecordset::GetValue_DWORD(const char * pszItem)
{
	DWORD dwRet = GetInt32ByName(pszItem);
	return dwRet;
}

//获取数据
FLOAT SQLRecordset::GetValue_FLOAT(const char * pszItem)
{
	FLOAT fRet = GetFloatByName(pszItem);
	return fRet;
}

//获取数据
DOUBLE SQLRecordset::GetValue_DOUBLE(const char * pszItem)
{
	DOUBLE dRet = GetFloatByName(pszItem);
	return dRet;
}

//获取数据
LONGLONG SQLRecordset::GetValue_LONGLONG(const char * pszItem)
{
	LONGLONG lRet = GetInt64ByName(pszItem);
	return lRet;
}

//获取数据
VOID SQLRecordset::GetValue_SystemTime(const char * pszItem, SYSTEMTIME & SystemTime)
{
	__int64 itime = GetInt64ByName(pszItem);

	FILETIME ft;
	//SYSTEMTIME st;
	ULARGE_INTEGER ularge;
	__int64 tmptimeA, tmptimeB;
	tmptimeA = itime;
	tmptimeB = itime;
	ularge.HighPart = (DWORD)(tmptimeA >> 32);
	ularge.LowPart = (DWORD)((tmptimeB << 32) >> 32);
	ft.dwLowDateTime = ularge.LowPart;
	ft.dwHighDateTime = ularge.HighPart;
	FileTimeToSystemTime(&ft, &SystemTime);
}

//获取字符串时间
VOID SQLRecordset::GetValue_SystemTime_String(const char * pszItem, SYSTEMTIME & SystemTime)
{
	char* pStrValue = GetStrByName(pszItem);

	//CHAR szString[24] = {0};
	//sprintf(szString, "%s", pStrValue);
	CString strDate;
#ifdef UNICODE
	strDate = CYSCharUtils::ANSIToUnicode(pStrValue);
#else
	strDate.Format("%s", pStrValue);
#endif // UNICODE

	COleDateTime odt;
	odt.ParseDateTime(strDate);

	if (odt.m_status == COleDateTime::valid)
	{
		VariantTimeToSystemTime(odt, &SystemTime);
	}
}

//获取字符;
VOID SQLRecordset::GetValue_String(const char * pszItem, LPSTR pszString, UINT uMaxCount)
{
	char* pStrValue = GetStrByName(pszItem);

	int nLen = 0;
#if STOR_UTF8
	nLen = _snprintf(pszString, uMaxCount - 1, "%s", CYSCharUtils::UTF8ToCString(pStrValue));
#else
	nLen = _snprintf(pszString, uMaxCount - 1, "%s", pStrValue);
#endif
	pszString[nLen] = '\0';
}

//获取字符
VOID SQLRecordset::GetValue_String(const char * pszItem, LPWSTR pszString, UINT uMaxCount)
{
	char* pStrValue = GetStrByName(pszItem);
	int nLen = swprintf(pszString, uMaxCount - 1,  L"%s", CYSCharUtils::ANSIToUnicode(pStrValue));
	pszString[nLen] = '\0';
}

BOOL SQLRecordset::Goto(unsigned long nCurrent)
{
	if (m_nCurrentRow >= m_vtRows.size())
	{
		return FALSE;
	}

	m_nCurrentRow = nCurrent;
	return TRUE;
}

char* SQLRecordset::GetStr(unsigned long field)
{
	return Get(field);
}

BOOL SQLRecordset::UpdateBlobField(CDataBase& connection,
	const char* szTable,
	const char* szKeyField,
	const char* szValueField,
	__int64 nKeyValue,
	const void* pBuf,
	unsigned long cLen,
	const char* szAddUpdate)
{
	char szBuf[100 * 1024] = { 0 };

	unsigned long n = mysql_hex_string(szBuf, (const char*)pBuf, cLen);

	return connection.ExecuteSQLNonResult("UPDATE %s SET %s=0x%s %s WHERE %s=%I64d",
		szTable, szValueField, szBuf, szAddUpdate, szKeyField, nKeyValue);
}

BOOL SQLRecordset::Initial(MYSQL* pConn)
{
	return Initial(mysql_store_result(pConn));
}

BOOL SQLRecordset::Initial(MYSQL_RES* pRes)
{
	if (pRes == m_pRecordset)
		return FALSE;
	if (m_pRecordset)
	{
		mysql_free_result(m_pRecordset);
		m_pRecordset = NULL;
	}
	m_vtRows.clear();
	m_vtLengths.clear();
	m_pFields = NULL;
	m_nFieldCount = 0;
	m_nRowCount = 0;
	m_nCurrentRow = 0;
	m_pRecordset = pRes;

	if (m_pRecordset)
	{
		m_pFields = mysql_fetch_fields(m_pRecordset);
		m_nFieldCount = mysql_num_fields(m_pRecordset);
		m_nRowCount = (unsigned long)mysql_num_rows(m_pRecordset);
	}
	return TRUE;
}

BOOL SQLRecordset::IsOpen()
{
	return m_nRowCount > 0;
}

BOOL SQLRecordset::IsEmpty()
{
	return m_nRowCount == 0;
}












