#include "Stdafx.h"
#include "ParseData.h"

#define PACKET_SIZE		4096
#define BUFFER_OVERFLOW 8192

#define REQUEST_METHOD_POST 0
#define REQUEST_METHOD_GET	1

inline int HexToInt( char ch )   
{   
	if( ch >= '0' && ch <= '9' )   
		return( ch - '0' );   

	if( ( ch >= 'a'  && ch <= 'f') || ( ch >= 'A' && ch <= 'F' ) )   
		return tolower( ch ) - 'a' + 10;   

	return 0;   
}

CParseData::CParseData(void)
{
	m_nStatus = STATUS_REQUEST;

	// default request method is POST
	m_nRequestMethod = REQUEST_METHOD_POST;

	m_QueryParams = "";
	m_FormVars = "";
	m_strBuffer = "";
	m_bJsonData = false;
}

CParseData::~CParseData(void)
{
}

bool CParseData::ParseRequests(LPBYTE pData, int nLen)
{
	//m_strBuffer = DecodeData(pData, nLen);
	m_strBuffer = pData;

	CString strTemp;
	int nIndex;

	while(!m_strBuffer.IsEmpty())
	{
		nIndex = m_strBuffer.Find(_T("\r\n"));
		if (nIndex != -1)
		{
			strTemp = m_strBuffer.Left(nIndex);
			m_strBuffer = m_strBuffer.Mid(nIndex + 2);
			m_strLines.AddTail(strTemp);
		}
		else
		{
			if (m_strBuffer.GetLength() > BUFFER_OVERFLOW)
			{
				CTraceService::TraceString(_T("Buffer overflow: DOS attack?"), TraceLevel_Normal);
				return false;
			}
			else
			{
				m_strLines.AddTail(m_strBuffer);
			}
			break;
		}
	}

	return true;
}

void CParseData::ProcessRequests()
{
	CString strMethod, strURL, strVersion;
	CString strBuff;

	while(!m_strLines.IsEmpty())
	{
		strBuff = m_strLines.RemoveHead();

		// post data ?
		if (strBuff.IsEmpty())
		{
			m_nStatus = STATUS_BODY;
			continue;
		}

		switch(m_nStatus)
		{
		case STATUS_REQUEST:
			{
				int nIndex = strBuff.Find(' ');
				if (nIndex != -1)
				{
					strMethod = strBuff.Left(nIndex);

					// strip URL + version
					strBuff = strBuff.Mid(nIndex + 1);

					strBuff.TrimLeft();
					strBuff.TrimRight();

					nIndex = strBuff.Find(' ');

					if(nIndex != -1)
					{
						// Change any %x's to the appropriate char
						strURL = URLDecode(strBuff.Left(nIndex));

						strBuff.TrimRight();

						// get HTTP version number
						strVersion = strBuff.Mid(nIndex + 1);
					}
					else
					{
						// simple request
						strURL = URLDecode(strBuff);
					}
				}

				// add url to header collection
				m_Headers.Add(_T("url"), strURL);

				// check for arguments
				nIndex = strURL.Find('?');
				if (nIndex != -1)
				{
					// save query params
					m_QueryParams = strURL.Mid(nIndex+1);
					// strip from file name....
					strURL = strURL.Left(nIndex);
				}

				// add scriptname to header collection
				m_Headers.Add(_T("script_name"), strURL);
				m_Headers.Add(_T("path_info"), strURL);

				m_nStatus = STATUS_HEADER;

				// log method
				//g_EventService.ShowEventNotify(strMethod + " " + strURL);

				break;
			}
		case STATUS_HEADER:
			{
				int nIndex = strBuff.Find(':');
				if(nIndex != -1)
				{
					CString strName = strBuff.Left(nIndex);
					CString strValue = strBuff.Mid(nIndex + 1);
					strName.Remove(' ');
					strValue.Remove(' ');
					strBuff.Format(_T("HEADER: Name: %s, Value: %s"), strName, strValue);
					TRACE1("%s\n", strBuff);

					// add item to header collection
					m_Headers.Add(strName, strValue);

					if (0 == strName.CompareNoCase(_T("Content-Type")) && 0 == strValue.CompareNoCase(_T("application/json")))
					{
						m_bJsonData = true;
					}
				}
				break;
			}
		case STATUS_BODY:
			{
				TRACE(strBuff);
				m_FormVars += strBuff;
				if (!m_bJsonData)
				{
					m_FormVars += "&";
				}

				break;
			}
		}

		if (strMethod.IsEmpty())
			continue;
	}

	strMethod.MakeUpper();
	if (strMethod == _T("GET"))
	{
		m_nRequestMethod = REQUEST_METHOD_GET;
	}
	else
	{
		m_nRequestMethod = REQUEST_METHOD_POST;
	}
}

void CParseData::ReaderHeaders(CString& strHeaders)
{
	CString strKey, strValue;

	// Iterate through the entire map
	for(int i=0; i<m_Headers.GetSize(); i++)
	{
		strKey = m_Headers.GetKeyAt(i);
		strValue = m_Headers.GetValueAt(i);

		strHeaders += strKey;
		strHeaders += _T(": ");
		strHeaders += strValue;
		strHeaders += _T("\r\n");
	} 
	strHeaders += _T("\r\n");
}

BOOL CParseData::GetServerVariable(LPTSTR lpszVariable, CString &strValue)
{
	CString strKey;

	// Iterate through the entire map
	for(int i=0; i<m_Headers.GetSize(); i++)
	{
		strKey = m_Headers.GetKeyAt(i);
		if (strKey.CompareNoCase(lpszVariable) == 0)
		{
			strValue = m_Headers.GetValueAt(i);
			return TRUE;
		}
	} 
	// not found
	return FALSE;
}

//BOOL CParseData::GetQueryString(LPTSTR lpszVariable, CString &strValue)
//{
//	CString strKey;
//
//	// Iterate through the entire map
//	for(int i=0; i<m_QueryStringVars.GetSize(); i++)
//	{
//		strKey = m_QueryStringVars.GetKeyAt(i);
//		if (strKey.CompareNoCase(lpszVariable) == 0)
//		{
//			strValue = m_QueryStringVars.GetValueAt(i);
//			return TRUE;
//		}
//	} 
//
//	return FALSE;
//}
//
void CParseData::ParseQueryString(LPCTSTR lpszParams, json::Value& val)
{
	CString strSub;
	int nCount=0;

	while(AfxExtractSubString(strSub, lpszParams, nCount++, '&'))
	{
		int nPos = strSub.Find('=');
		if (nPos != -1)
		{
			CStringA strKey = CYSCharUtils::ConvertToANSI(strSub.Left(nPos));
			//m_QueryStringVars.Add(strKey, strSub.Mid(nPos+1));
			CStringA strValue = CYSCharUtils::ConvertToANSI(strSub.Mid(nPos + 1));
			val[strKey] = strValue.GetBuffer();
		}
	}
}
//
//void CParseData::SplitString(LPCTSTR lpszString, TCHAR chSep, CStringArray& aryString)
//{
//	CString strSub;
//	int nCount=0;
//
//	while(AfxExtractSubString(strSub, lpszString, nCount++, chSep))
//	{
//		aryString.Add(strSub);
//	}
//}

json::Value CParseData::ParseParams()
{
	json::Value val;
	if (m_bJsonData)
	{
		CStringA strA = CYSCharUtils::ConvertToANSI(m_FormVars);
		string strData = UnEscape(strA.GetBuffer());
		val = json::Deserialize(strData);
	}
	else
	{
		if (REQUEST_METHOD_POST == m_nRequestMethod)
		{
			//SaveLog(_T("%s"), m_FormVars);
			ParseQueryString(m_FormVars, val);
		}
		else
		{
			//SaveLog(_T("%s"), m_QueryParams);
			ParseQueryString(m_QueryParams, val);
		}
	}

	return val;
}

CString CParseData::DecodeData(LPBYTE cbData, int nLen)
{
	CString strOut;
	LPBYTE pOutBuf = (LPBYTE)strOut.GetBuffer(nLen);

	if ( pOutBuf )
	{
		LPBYTE pOutTmp = pOutBuf;

		while(*cbData)
		{
			if ( '%'==*cbData )
			{
				*pOutTmp++ = char( HexToInt( *(++cbData) ) << 4 | HexToInt( *(++cbData) ) );
			}
			else if ( '+'==*cbData ) *pOutTmp++ = ' ';
			else *pOutTmp++ = *cbData;

			cbData++;
		}

		*pOutTmp = '\0';

		strOut.ReleaseBuffer();
	}

	return strOut;
}

