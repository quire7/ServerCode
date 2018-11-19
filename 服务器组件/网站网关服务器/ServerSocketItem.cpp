#include "stdafx.h"
#include "IOCPServer.h"
#include "ServerSocketItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PACKET_SIZE		4096
#define BUFFER_OVERFLOW 8192

#define REQUEST_METHOD_POST 0
#define REQUEST_METHOD_GET	1

CServerSocketItem::CServerSocketItem(WORD id, WORD socket, DWORD dwClientAddr, CIOCPServer* pIOCPServer, IClientSocket* pClientSocket) : CTCPSocket(id, socket, dwClientAddr, pIOCPServer)
{
	m_pClientSocket = pClientSocket;
}


CServerSocketItem::~CServerSocketItem()
{
}

bool CServerSocketItem::Close()
{
	m_pIOCPServer->Disconnect(getID());

	return true;
}

bool CServerSocketItem::processRecvData(void* data, DWORD len)
{
	switch (len)
	{
	case 0:
		SendResult(_T("{\"result\":%d}"), 1);
		break;

	case SOCKET_ERROR:
		if (GetLastError() != WSAEWOULDBLOCK) 
		{
			TCHAR szError[256];
			_tprintf_s(szError, sizeof(szError), _T("ERROR in OnReceive(): %d"), GetLastError());
			SaveLog(_T("%s"), szError, TraceLevel_Exception);
		}
		break;

	default:
		if (len != SOCKET_ERROR && len != 0)
		{
			try
			{
				if ( false==m_ParseData.ParseRequests((LPBYTE)data, len) )
				{
					SendResult(_T("{\"result\":%d}"), 1);
					break;
				}

				m_ParseData.ProcessRequests();

				CString strUrl;
				m_ParseData.GetServerVariable(_T("script_name"), strUrl);

				if ( strUrl.Compare(_T("/active.do"))==0 )
				{
					bool bRet = ExecuteRequest();
					if ( bRet ) break;
				}
				else
				{
					SendResult(_T("{\"result\":%d}"), 1);
					if ( strUrl == _T("/favicon.ico") ) break;
				}

				CTraceService::TraceString(_T("请求地址错误: ") + strUrl, TraceLevel_Exception);
			}
			catch(...)
			{
				SendResult(_T("{\"result\":%d}"), 1);
				throw ;
			}
		}	
		break;
	}

	return true;
}

bool CServerSocketItem::ExecuteRequest()
{
	json::Value obj = m_ParseData.ParseParams();
	if (!obj.HasKey("way"))
	{
		return false;
	}

	int nWay = obj["way"].ToInt();

	switch(nWay)
	{
	case eWebNull:
	case eWebBuyProp:
		{
			WebBuyPropRequest(obj["data"]);
			break;
		}
	default:
		{
			return false;
		}
	}

	return true;
}

//购买道具请求
bool CServerSocketItem::WebBuyPropRequest(json::Value& val)
{
	//CString strData;
	//strData.Format(_T("way=%d&uid=%s&cnum=%s&inum=%s&prop=%s&orderid=%s&%s"), eWebBuyProp, strUid, strGoldCount, strIngotCount, strPropList, strOrderID, VERIFY_KEY);

	//TCHAR szMD5Result[33];
	//CYSEncrypt::MD5Encrypt(strData, szMD5Result);

	//if ( strSign.CompareNoCase(szMD5Result) == 0 )
	{
		CMD_CS_S_BuyPropFromWeb BuyProp;
		ZeroMemory(&BuyProp, sizeof(CMD_CS_S_BuyPropFromWeb));
		BuyProp.dwUserID = val["uid"].ToInt();

		json::Array ary = val["data"].ToArray();
		for (int i = 0; i < ary.size(); i++)
		{
			BuyProp.aryDBID[i] = ary[i]["DBID"].ToInt();
			BuyProp.aryPropID[i] = ary[i]["PropID"].ToShort();
			BuyProp.aryPropCount[i] = ary[i]["PropCount"].ToInt64();
			BuyProp.cbBuyNum++;
			TRACE2("PropID=%d, PropCount=%d\n", BuyProp.aryPropID[i], BuyProp.aryPropCount[i]);
		}

		if (BuyProp.cbBuyNum == 0)
		{
			return SendResult(_T("{\"result\":%d}"), BUY_PROP_VERIFY_ERROR);
		}

		m_pClientSocket->SendData(MDM_CS_WEB_SERVICE, SUB_CS_C_BUY_PROP_FROM_WEB, &BuyProp, sizeof(BuyProp));

		SendResult(_T("{\"result\":%d}"), BUY_PROP_SUCCESSED);
	}
	//else
	//{
	//	SendResult(_T("{\"result\":%d}"), BUY_PROP_VERIFY_ERROR);
	//	SaveLog("%s", "请求验证错误!");
	//}

	return true;
}

bool CServerSocketItem::HttpResponse(LPCTSTR lpszMessage)
{
	CString strHeader;
	CString strResult = lpszMessage;

	// HTTP version
	strHeader = _T("HTTP/1.0 ");

	// status
	strHeader += _T("200 OK\r\n");
	// servername
	strHeader += _T("Server: Web Server\r\n");
	// content type
	strHeader += _T("Content-Type: text/html\r\n");
	// file length
	CString strLength;
	strLength.Format(_T("Content-Length: %d\r\n"), strResult.GetLength());
	strHeader += strLength;

	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);

	// get internet time
	TCHAR buff[INTERNET_RFC1123_BUFSIZE] = {0};
	InternetTimeFromSystemTime(&systemTime, INTERNET_RFC1123_FORMAT, buff, INTERNET_RFC1123_BUFSIZE);

	// modified	
	strHeader += _T("Last-Modified: ");
	strHeader += CString(buff);

	//在http1.1中，client和server都是默认对方支持长链接的， 
	//如果client使用http1.1协议，但又不希望使用长链接，则需要在header中指明connection的值为close；
	//如果server方也不想支持长链接，则在response中也需要明确说明connection的值为close.
	// connection
	strHeader += _T("Connection: close");

	strHeader += _T("\r\n\r\n");
	strHeader += lpszMessage;

	CStringA strData = CYSCharUtils::ConvertToANSI(strHeader);

	m_pIOCPServer->SendData(getID(), strData.GetBuffer(), strData.GetLength());

	//Close();

	return true;
}

//发送结果
bool CServerSocketItem::SendResult(LPCTSTR pstrFormat, ...)
{
	try
	{
		CString strResult;

		// format and write the data we were given
		va_list args;
		va_start(args, pstrFormat);
		strResult.FormatV(pstrFormat, args);
		va_end(args);
		
		HttpResponse(strResult);
	}
	catch(CMemoryException *e)
	{
		e->Delete();
		return false;
	}
	catch(...)
	{
		return false;
	}

	return true;
}
