#ifndef SQL_RECORDSET_HEAD_FILE
#define SQL_RECORDSET_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

#include "DataBase.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////////////

struct Blob
{
	char* buf;
	unsigned long len;

	Blob(const void* _buf = NULL, unsigned long _len = 0)
	{
		buf = (char*)_buf;
		len = _len;
	}
};

/**
* \brief record sets.
*/
class KERNEL_ENGINE_CLASS SQLRecordset
{
public:
	SQLRecordset();
	SQLRecordset(CDataBaseHelper& dbHelper, bool bFetchFirst=true);
	SQLRecordset(MYSQL* pConn);
	SQLRecordset(MYSQL_RES* pRes);
	~SQLRecordset();
public:
	BOOL FetchAll();
	BOOL Fetch();
	BOOL Goto(unsigned long nCurrent);
public:
	unsigned long GetFieldByName(const char* szName);
	unsigned long GetCount();
	unsigned long GetFieldCount();
public:
	BOOL Initial(MYSQL* pConn);
	BOOL Initial(MYSQL_RES* pRes);
public:
	BOOL IsOpen();
	BOOL IsEmpty();
public:
	char* Get(unsigned long field);
	char* Get(const char* szFieldName);

public:
	int	GetInt32(unsigned long field);
	char* GetStr(unsigned long field);
	__int64 GetInt64(unsigned long field);
	float GetFloat(unsigned long field);
	Blob GetBlob(unsigned long field);

	/**
	* get record value by name.
	*/
	int   GetInt32ByName(const char *filed);
	__int64 GetInt64ByName(const char *filed);
	float GetFloatByName(const char *filed);
	char *GetStrByName(const char *filed);
	void GetBlobByName(const char *filed, Blob &ret);

	//获取数据
	INT GetValue_INT(const char * pszItem);
	//获取数据
	UINT GetValue_UINT(const char * pszItem);
	//获取数据
	LONG GetValue_LONG(const char * pszItem);
	//获取数据
	BYTE GetValue_BYTE(const char * pszItem);
	//获取数据
	WORD GetValue_WORD(const char * pszItem);
	//获取数据
	DWORD GetValue_DWORD(const char * pszItem);
	//获取数据
	FLOAT GetValue_FLOAT(const char * pszItem);
	//获取数据
	DOUBLE GetValue_DOUBLE(const char * pszItem);
	//获取数据
	LONGLONG GetValue_LONGLONG(const char * pszItem);
	//获取数据
	VOID GetValue_SystemTime(const char * pszItem, SYSTEMTIME & SystemTime);
	//获取字符串时间
	VOID GetValue_SystemTime_String(const char * pszItem, SYSTEMTIME & SystemTime);
	//获取字符
	VOID GetValue_String(const char * pszItem, LPSTR pszString, UINT uMaxCount);
	//获取字符
	VOID GetValue_String(const char * pszItem, LPWSTR pszString, UINT uMaxCount);

public:
	static BOOL UpdateBlobField(CDataBase& connection,
		const char* szTable,
		const char* szKeyField,
		const char* szValueField,
		__int64 nKeyValue,
		const void* pBuf, unsigned long cLen,
		const char* szAddUpdate = "");
private:
	typedef std::vector<MYSQL_ROW> MYSQL_ROWS;
	typedef std::vector<unsigned long*> MYSQL_LENGTHS;

	MYSQL_ROWS m_vtRows;
	MYSQL_LENGTHS m_vtLengths;
	MYSQL_FIELD* m_pFields;
	MYSQL_RES* m_pRecordset;
	unsigned long m_nFieldCount;
	unsigned long m_nRowCount;
	unsigned long m_nCurrentRow;
};

class KERNEL_ENGINE_CLASS CleanupResults
{
public:
	CleanupResults(CDataBaseHelper& dbHelper)
	{
		m_pConnection = dbHelper.GetInterface();
	}

	CleanupResults(IDataBase* pConnection)
	{
		m_pConnection = pConnection;
	}

	~CleanupResults()
	{
		if (m_pConnection)
		{
			m_pConnection->CleanupResults();
		}
	}
private:
	IDataBase* m_pConnection;
};

//////////////////////////////////////////////////////////////////////////////////

#endif