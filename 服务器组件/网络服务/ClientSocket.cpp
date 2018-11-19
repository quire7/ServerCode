#include "Stdafx.h"
#include "ClientSocket.h"
#include "Ws2tcpip.h"

//////////////////////////////////////////////////////////////////////////

//宏定义
#define ID_SOCKET_WND				10								//SOCKET 窗口 ID
#define WM_SOCKET_NOTIFY			WM_USER+10						//SOCKET 消息

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClientSocket, CWnd)
	ON_MESSAGE(WM_SOCKET_NOTIFY,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CClientSocket::CClientSocket()
{
	m_wRecvSize=0;
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_dwSendPacketCount=0;
	m_dwRecvPacketCount=0;
	//m_cbSocketStatus=SHUT_REASON_NORMAL;
	m_hSocket=INVALID_SOCKET;
	m_pIClientSocketSink = NULL;
	//m_ProxyInfo.wProxyPort=0;
	//m_ProxyInfo.cbProxyType=PROXY_NONE;
	m_cbSocketStatus=SOCKET_STATUS_IDLE;
}

//析构函数
CClientSocket::~CClientSocket()
{
	CloseSocket();
}

//接口查询
void * CClientSocket::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientSocket, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientSocket, Guid, dwQueryVer);
	return NULL;
}

//设置接口
bool CClientSocket::SetTCPSocketSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIClientSocketSink = (IClientSocketSink *)pIUnknownEx->QueryInterface(IID_IClientSocketSink, VER_IClientSocketSink);
	ASSERT(m_pIClientSocketSink != NULL);
	return (m_pIClientSocketSink != NULL);
}

//获取接口
void * CClientSocket::GetTCPSocketSink(const IID & Guid, DWORD dwQueryVer)
{
	if (m_pIClientSocketSink == NULL) return NULL;
	return m_pIClientSocketSink->QueryInterface(Guid, dwQueryVer);
}

//连接服务器
BYTE CClientSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	//效验参数
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);

	//效验状态
	if (m_hSocket!=INVALID_SOCKET) throw TEXT("连接 SOCKET 句柄已经存在");
	if (m_cbSocketStatus!=SOCKET_STATUS_IDLE) throw TEXT("连接状态不是等待连接状态");
	if (dwServerIP==INADDR_NONE) throw TEXT("目标服务器地址格式不正确，请检查后再次尝试！");

	//设置参数
	m_wRecvSize=0;
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0x12345678;
	m_dwRecvXorKey=0x12345678;
	m_dwSendTickCount=GetTickCount()/1000L;
	m_dwRecvTickCount=GetTickCount()/1000L;

	try
	{
		//建立 SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (m_hSocket==INVALID_SOCKET) throw TEXT("SOCKET 创建失败");
		if ((m_hWnd==NULL)&&(!Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),GetDesktopWindow(),ID_SOCKET_WND,NULL)))
			throw TEXT("SOCKET 内部窗口创建失败");

		//填写服务器地址
		m_wSocketID=m_hSocket;
		SOCKADDR_IN SocketAddr;
		memset(&SocketAddr,0,sizeof(SocketAddr));
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

		//连接服务器
		int iErrorCode=0;
		WSASetLastError(0);
		//if (m_ProxyInfo.cbProxyType!=PROXY_NONE)
		//{
		//	throw TEXT("未支持代理服务器");
		//}
		//else
		{
			//绑定窗口
			iErrorCode=WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE);
			if (iErrorCode==SOCKET_ERROR) throw TEXT("绑定内部窗口错误");

			//连接服务器
			iErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
			if (iErrorCode==SOCKET_ERROR)
			{
				iErrorCode=WSAGetLastError();
				if (iErrorCode!=WSAEWOULDBLOCK)
				{
					static TCHAR szBuffer[64];
					_sntprintf_s(szBuffer,sizeof(szBuffer),TEXT("连接发生错误，错误代码 [ %d ]"),iErrorCode);
					throw szBuffer;
				}
			}

			//设置变量
			m_cbSocketStatus=SOCKET_STATUS_WAIT;
		}

		return CONNECT_SUCCESS;
	}
	catch (LPCTSTR pszError)
	{
		CloseSocket();
		throw pszError;
		return CONNECT_FAILURE;
	}
	catch (...)
	{
		CloseSocket();
		throw TEXT("连接产生未知异常错误");
		return CONNECT_EXCEPTION;
	}
}

//连接服务器
BYTE CClientSocket::Connect(const LPCTSTR pszServerIP, WORD wPort)
{
	//效验数据
	ASSERT(wPort!=0);
	ASSERT(pszServerIP!=NULL);
	if ((pszServerIP==NULL)||(wPort==0)) return false;
	return Connect(TranslateAddr(pszServerIP),wPort);
}

//发送函数
WORD CClientSocket::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//效验状态
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//构造数据
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;
	pHead->CommandInfo.wSubCmdID=wSubCmdID;

	//加密数据
	WORD wSendSize = CYSEncrypt::EncryptBuffer(cbDataBuffer, sizeof(TCP_Head), sizeof(cbDataBuffer));

	m_dwSendPacketCount++;

	//发送数据
	return (WORD)SendDataBuffer(cbDataBuffer,wSendSize);
}

//发送函数
WORD CClientSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验状态
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//效验大小
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_TCP_PACKET) return false;

	//构造数据
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(pHead+1,pData,wDataSize);
	}

	//加密数据
	WORD wSendSize = CYSEncrypt::EncryptBuffer(cbDataBuffer, sizeof(TCP_Head) + wDataSize, sizeof(cbDataBuffer));
	m_dwSendPacketCount++;

	//发送数据
	return (WORD)SendDataBuffer(cbDataBuffer, wSendSize);
}

//关闭连接
void CClientSocket::CloseSocket()
{
	//关闭连接
	bool bClose=(m_hSocket!=INVALID_SOCKET);
	m_cbSocketStatus=SOCKET_STATUS_IDLE;
	if (m_hSocket!=INVALID_SOCKET)
	{
		WSAAsyncSelect(m_hSocket,GetSafeHwnd(),WM_SOCKET_NOTIFY,0);
		closesocket(m_hSocket);
		m_hSocket=INVALID_SOCKET;
		m_cbSocketStatus=SOCKET_STATUS_IDLE;
	}
	if ((bClose == true) && (m_pIClientSocketSink != NULL))
	{
		ASSERT(m_pIClientSocketSink != NULL);
		try { m_pIClientSocketSink->OnEventClientSocketShut(m_wSocketID, SHUT_REASON_NORMAL); }
		catch (...) {}
	}

	//恢复数据
	m_wRecvSize=0;
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0;
	m_dwRecvXorKey=0;
	m_dwSendTickCount=0;
	m_dwRecvTickCount=0;
	m_dwSendPacketCount=0;
	m_dwRecvPacketCount=0;
	return;
}

//发送数据
DWORD CClientSocket::SendDataBuffer(void * pBuffer, WORD wSendSize)
{
	//效验参数
	ASSERT(wSendSize!=0);
	ASSERT(pBuffer!=NULL);
	
	//发送数据
	WORD wSended=0;
	while (wSended<wSendSize)
	{
		int iErrorCode=send(m_hSocket,(char *)pBuffer+wSended,wSendSize-wSended,0);
		if (iErrorCode==SOCKET_ERROR)
		{
			if (WSAGetLastError()==WSAEWOULDBLOCK)
			{
				m_dwSendTickCount=GetTickCount()/1000L;
				return true;
			}
			return false;
		}
		wSended+=iErrorCode;
	}
	m_dwSendTickCount=GetTickCount()/1000L;

	return true;
}

//解释服务器地址
DWORD CClientSocket::TranslateAddr(LPCTSTR pszServerAddr)
{
	//转化地址
	//DWORD dwServerIP=inet_addr((LPCSTR)CT2A(pszServerAddr));
	//if (dwServerIP==INADDR_NONE)
	//{
	//	LPHOSTENT lpHost=gethostbyname((LPCSTR)CT2A(pszServerAddr));
	//	if (lpHost==NULL) return INADDR_NONE;
	//	dwServerIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	//}
	//return dwServerIP;

	struct in_addr addr = { 0 };
	inet_pton(AF_INET, CT2A(pszServerAddr), (void*)&addr);
	DWORD dwServerIP = addr.s_addr;
	if (dwServerIP == INADDR_NONE)
	{
		struct addrinfo *result = NULL;
		struct addrinfo *ptr = NULL;
		struct addrinfo hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		DWORD dwRetval = getaddrinfo(CT2A(pszServerAddr), 0, &hints, &result);
		if (dwRetval != 0)
		{
			return INADDR_NONE;
		}
		else
		{
			int i = 1;
			for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
			{
				printf("getaddrinfo response %d\n", i++);
				printf("\tFlags: 0x%x\n", ptr->ai_flags);
				printf("\tFamily: ");
				switch (ptr->ai_family)
				{
				case AF_UNSPEC:
					printf("Unspecified\n");
					break;
				case AF_INET:
				{
					printf("AF_INET (IPv4)\n");
					struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
					printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr));
					break;
				}
				case AF_INET6:
				{
					printf("AF_INET6 (IPv6)\n");
					// the InetNtop function is available on Windows Vista and later
					// sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
					// printf("\tIPv6 address %s\n",
					//    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

					// We use WSAAddressToString since it is supported on Windows XP and later
					LPSOCKADDR sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
					// The buffer length is changed by each call to WSAAddresstoString
					// So we need to set it for each iteration through the loop for safety
					char ipstringbuffer[46];
					DWORD ipbufferlength = 46;
					int iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
						ipstringbuffer, &ipbufferlength);
					if (iRetval)
						printf("WSAAddressToString failed with %u\n", WSAGetLastError());
					else
						printf("\tIPv6 address %s\n", ipstringbuffer);
					break;
				}
				case AF_NETBIOS:
					printf("AF_NETBIOS (NetBIOS)\n");
					break;
				default:
					printf("Other %ld\n", ptr->ai_family);
					break;
				}
			}
		}
	}
	return dwServerIP;
}

//解释错误
/*LPCTSTR CClientSocket::GetConnectError(int iErrorCode, LPTSTR pszBuffer, WORD wBufferSize)
{
	//效验参数
	ASSERT(pszBuffer!=NULL);
	if (pszBuffer==NULL) return NULL;

	//解释错误
	switch (iErrorCode)
	{
	case 0:					//没有错误
		{
			lstrcpyn(pszBuffer,TEXT("操作执行成功"),wBufferSize);
			break;
		}
	case WSAEADDRNOTAVAIL:	//地址格式错误
		{
			lstrcpyn(pszBuffer,TEXT("目标服务器地址格式不正确，请检查后再次尝试！"),wBufferSize);
			break;
		}
	case WSAECONNREFUSED:	//服务器没有启动
		{
			lstrcpyn(pszBuffer,TEXT("目标服务器繁忙或者没有启动！"),wBufferSize);
			break;
		}
	case WSAETIMEDOUT:		//连接超时
		{
			lstrcpyn(pszBuffer,TEXT("连接超时，可能是目标服务器不存在或者服务器地址格式不正确！"),wBufferSize);
			break;
		}
	case WSAEHOSTUNREACH:
		{
			lstrcpyn(pszBuffer,TEXT("网络连接失败，请检查是否已经成功拨号和连接 Internet ！"),wBufferSize);
			break;
		}
	default:				//默认错误
		{
			_snprintf(pszBuffer,wBufferSize,TEXT("连接错误号：%ld，详细错误信息请参考操作帮助手册！"),iErrorCode);
			break;
		}
	}

	return pszBuffer;
}*/

//网络连接
LRESULT CClientSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//判断状态
	int iErrorCode=WSAGETSELECTERROR(lParam);
	if (iErrorCode==0) m_cbSocketStatus=SOCKET_STATUS_CONNECT;
	else CloseSocket();

	//发送通知
	//TCHAR szErrorDesc[128]=TEXT("");
	//GetConnectError(iErrorCode,szErrorDesc,sizeof(szErrorDesc));
	m_pIClientSocketSink->OnEventClientSocketLink(m_wSocketID, iErrorCode);

	return 1;
}

//网络读取
LRESULT CClientSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//读取数据
		int iRetCode=recv(m_hSocket,(char *)m_cbRecvBuf+m_wRecvSize,sizeof(m_cbRecvBuf)-m_wRecvSize,0);
		if (iRetCode==SOCKET_ERROR) throw TEXT("网络连接关闭，读取数据失败");
		ASSERT(m_dwSendPacketCount>0);
		m_wRecvSize+=iRetCode;
		m_dwRecvTickCount=GetTickCount()/1000L;

		//变量定义
		WORD wPacketSize=0;
		BYTE cbDataBuffer[SOCKET_TCP_PACKET+sizeof(TCP_Head)];
		TCP_Head * pHead=(TCP_Head *)m_cbRecvBuf;

		while (m_wRecvSize>=sizeof(TCP_Head))
		{
			//效验参数
			wPacketSize=pHead->TCPInfo.wPacketSize;
			//ASSERT(pHead->TCPInfo.cbVersion==SOCKET_TCP_VER);
			ASSERT(pHead->TCPInfo.cbDataKind == SOCKET_TCP_VER);
			ASSERT(wPacketSize<=(SOCKET_TCP_PACKET+sizeof(TCP_Head)));
			//if (pHead->TCPInfo.cbVersion!=SOCKET_TCP_VER) throw TEXT("数据包版本错误");
			if (pHead->TCPInfo.cbDataKind!=SOCKET_TCP_VER) throw TEXT("数据包版本错误");
			if (wPacketSize>(SOCKET_TCP_PACKET+sizeof(TCP_Head))) throw TEXT("数据包太大");
			if (m_wRecvSize<wPacketSize) return 1;

			//拷贝数据
			m_dwRecvPacketCount++;
			CopyMemory(cbDataBuffer,m_cbRecvBuf,wPacketSize);
			m_wRecvSize-=wPacketSize;
			MoveMemory(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);

			//解密数据
			WORD wRealySize = CYSEncrypt::CrevasseBuffer(cbDataBuffer, wPacketSize);
			ASSERT(wRealySize>=sizeof(TCP_Head));

			//解释数据
			WORD wDataSize=wRealySize-sizeof(TCP_Head);
			void * pDataBuffer=cbDataBuffer+sizeof(TCP_Head);
			TCP_Command Command=((TCP_Head *)cbDataBuffer)->CommandInfo;

			//内核命令
			if (Command.wMainCmdID==MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
				case SUB_KN_DETECT_SOCKET:	//网络检测
					{
						//发送数据
						SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,pDataBuffer,wDataSize);
						break;
					}
				}
				continue;
			}

			//处理数据
			bool bSuccess = m_pIClientSocketSink->OnEventClientSocketRead(m_wSocketID, Command, pDataBuffer, wDataSize);
			if (bSuccess==false) throw TEXT("网络数据包处理失败");
		};
	}
	catch (...) 
	{ 
		CloseSocket(); 
	}

	return 1;
}

//网络关闭
LRESULT CClientSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//m_cbSocketStatus=SHUT_REASON_NORMAL;
	CloseSocket();
	return 1;
}

//SOCKET 消息处理程序
LRESULT	CClientSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:	//网络连接
		{ 
			return OnSocketNotifyConnect(wParam,lParam); 
		}
	case FD_READ:		//数据读取
		{ 
			return OnSocketNotifyRead(wParam,lParam); 
		}
	case FD_CLOSE:		//网络关闭
		{ 
			return OnSocketNotifyClose(wParam,lParam);	
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
DECLARE_CREATE_MODULE(ClientSocket)

//////////////////////////////////////////////////////////////////////////
