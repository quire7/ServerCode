#pragma once

enum
{
	STATUS_REQUEST	= 0,
	STATUS_HEADER	= 1,
	STATUS_BODY		= 2,
	STATUS_SIMPLE	= 3,
	STATUS_DONE	= 4,
};

class CParseData
{
public:
	CParseData(void);
	~CParseData(void);

public:
	BOOL ExecutePage(LPCTSTR lpszFileName);
	void ReaderHeaders(CString& strHeaders);
	BOOL GetServerVariable(LPTSTR lpszVariable, CString &strValue);
	//BOOL GetQueryString(LPTSTR lpszVariable, CString &strValue);

	//void SplitString(LPCTSTR lpszString, TCHAR chSep, CStringArray& aryString);

	int GetRequestMethod() { return m_nRequestMethod; }
	bool ParseRequests(LPBYTE pData, int nLen);
	void ProcessRequests();
	CString DecodeData(LPBYTE cbData, int nLen);

	json::Value ParseParams();

	//是否是JSON数据
	bool IsJsonData() { return m_bJsonData; }

	CString m_QueryParams;
	CString m_FormVars;

private:
	void ParseQueryString(LPCTSTR lpszParams, json::Value& val);

private:
	int			m_nStatus;
	int			m_nRequestMethod;
	bool		m_bJsonData;

	CString		m_strBuffer;
	CStringList m_strLines;

	CSimpleMap<CString, CString> m_Headers;
	CSimpleMap<CString, CString> m_QueryStringVars;
};
