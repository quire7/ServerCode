#include "Stdafx.h"
#include "ClientSocket.h"
#include "Ws2tcpip.h"

//////////////////////////////////////////////////////////////////////////

//�궨��
#define ID_SOCKET_WND				10								//SOCKET ���� ID
#define WM_SOCKET_NOTIFY			WM_USER+10						//SOCKET ��Ϣ

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClientSocket, CWnd)
	ON_MESSAGE(WM_SOCKET_NOTIFY,OnSocketNotify)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
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

//��������
CClientSocket::~CClientSocket()
{
	CloseSocket();
}

//�ӿڲ�ѯ
void * CClientSocket::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientSocket, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientSocket, Guid, dwQueryVer);
	return NULL;
}

//���ýӿ�
bool CClientSocket::SetTCPSocketSink(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIClientSocketSink = (IClientSocketSink *)pIUnknownEx->QueryInterface(IID_IClientSocketSink, VER_IClientSocketSink);
	ASSERT(m_pIClientSocketSink != NULL);
	return (m_pIClientSocketSink != NULL);
}

//��ȡ�ӿ�
void * CClientSocket::GetTCPSocketSink(const IID & Guid, DWORD dwQueryVer)
{
	if (m_pIClientSocketSink == NULL) return NULL;
	return m_pIClientSocketSink->QueryInterface(Guid, dwQueryVer);
}

//���ӷ�����
BYTE CClientSocket::Connect(DWORD dwServerIP, WORD wPort)
{
	//Ч�����
	ASSERT(m_hSocket==INVALID_SOCKET);
	ASSERT(m_cbSocketStatus==SOCKET_STATUS_IDLE);

	//Ч��״̬
	if (m_hSocket!=INVALID_SOCKET) throw TEXT("���� SOCKET ����Ѿ�����");
	if (m_cbSocketStatus!=SOCKET_STATUS_IDLE) throw TEXT("����״̬���ǵȴ�����״̬");
	if (dwServerIP==INADDR_NONE) throw TEXT("Ŀ���������ַ��ʽ����ȷ��������ٴγ��ԣ�");

	//���ò���
	m_wRecvSize=0;
	m_cbSendRound=0;
	m_cbRecvRound=0;
	m_dwSendXorKey=0x12345678;
	m_dwRecvXorKey=0x12345678;
	m_dwSendTickCount=GetTickCount()/1000L;
	m_dwRecvTickCount=GetTickCount()/1000L;

	try
	{
		//���� SOCKET
		m_hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if (m_hSocket==INVALID_SOCKET) throw TEXT("SOCKET ����ʧ��");
		if ((m_hWnd==NULL)&&(!Create(NULL,NULL,WS_CHILD,CRect(0,0,0,0),GetDesktopWindow(),ID_SOCKET_WND,NULL)))
			throw TEXT("SOCKET �ڲ����ڴ���ʧ��");

		//��д��������ַ
		m_wSocketID=m_hSocket;
		SOCKADDR_IN SocketAddr;
		memset(&SocketAddr,0,sizeof(SocketAddr));
		SocketAddr.sin_family=AF_INET;
		SocketAddr.sin_port=htons(wPort);
		SocketAddr.sin_addr.S_un.S_addr=dwServerIP;

		//���ӷ�����
		int iErrorCode=0;
		WSASetLastError(0);
		//if (m_ProxyInfo.cbProxyType!=PROXY_NONE)
		//{
		//	throw TEXT("δ֧�ִ��������");
		//}
		//else
		{
			//�󶨴���
			iErrorCode=WSAAsyncSelect(m_hSocket,m_hWnd,WM_SOCKET_NOTIFY,FD_READ|FD_CONNECT|FD_CLOSE);
			if (iErrorCode==SOCKET_ERROR) throw TEXT("���ڲ����ڴ���");

			//���ӷ�����
			iErrorCode=connect(m_hSocket,(SOCKADDR *)&SocketAddr,sizeof(SocketAddr));
			if (iErrorCode==SOCKET_ERROR)
			{
				iErrorCode=WSAGetLastError();
				if (iErrorCode!=WSAEWOULDBLOCK)
				{
					static TCHAR szBuffer[64];
					_sntprintf_s(szBuffer,sizeof(szBuffer),TEXT("���ӷ������󣬴������ [ %d ]"),iErrorCode);
					throw szBuffer;
				}
			}

			//���ñ���
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
		throw TEXT("���Ӳ���δ֪�쳣����");
		return CONNECT_EXCEPTION;
	}
}

//���ӷ�����
BYTE CClientSocket::Connect(const LPCTSTR pszServerIP, WORD wPort)
{
	//Ч������
	ASSERT(wPort!=0);
	ASSERT(pszServerIP!=NULL);
	if ((pszServerIP==NULL)||(wPort==0)) return false;
	return Connect(TranslateAddr(pszServerIP),wPort);
}

//���ͺ���
WORD CClientSocket::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч��״̬
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;
	pHead->CommandInfo.wSubCmdID=wSubCmdID;

	//��������
	WORD wSendSize = CYSEncrypt::EncryptBuffer(cbDataBuffer, sizeof(TCP_Head), sizeof(cbDataBuffer));

	m_dwSendPacketCount++;

	//��������
	return (WORD)SendDataBuffer(cbDataBuffer,wSendSize);
}

//���ͺ���
WORD CClientSocket::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч��״̬
	if (m_hSocket==INVALID_SOCKET) return false;
	if (m_cbSocketStatus!=SOCKET_STATUS_CONNECT) return false;

	//Ч���С
	ASSERT(wDataSize<=SOCKET_TCP_PACKET);
	if (wDataSize>SOCKET_TCP_PACKET) return false;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead=(TCP_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID=wMainCmdID;
	pHead->CommandInfo.wSubCmdID=wSubCmdID;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(pHead+1,pData,wDataSize);
	}

	//��������
	WORD wSendSize = CYSEncrypt::EncryptBuffer(cbDataBuffer, sizeof(TCP_Head) + wDataSize, sizeof(cbDataBuffer));
	m_dwSendPacketCount++;

	//��������
	return (WORD)SendDataBuffer(cbDataBuffer, wSendSize);
}

//�ر�����
void CClientSocket::CloseSocket()
{
	//�ر�����
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

	//�ָ�����
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

//��������
DWORD CClientSocket::SendDataBuffer(void * pBuffer, WORD wSendSize)
{
	//Ч�����
	ASSERT(wSendSize!=0);
	ASSERT(pBuffer!=NULL);
	
	//��������
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

//���ͷ�������ַ
DWORD CClientSocket::TranslateAddr(LPCTSTR pszServerAddr)
{
	//ת����ַ
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

//���ʹ���
/*LPCTSTR CClientSocket::GetConnectError(int iErrorCode, LPTSTR pszBuffer, WORD wBufferSize)
{
	//Ч�����
	ASSERT(pszBuffer!=NULL);
	if (pszBuffer==NULL) return NULL;

	//���ʹ���
	switch (iErrorCode)
	{
	case 0:					//û�д���
		{
			lstrcpyn(pszBuffer,TEXT("����ִ�гɹ�"),wBufferSize);
			break;
		}
	case WSAEADDRNOTAVAIL:	//��ַ��ʽ����
		{
			lstrcpyn(pszBuffer,TEXT("Ŀ���������ַ��ʽ����ȷ��������ٴγ��ԣ�"),wBufferSize);
			break;
		}
	case WSAECONNREFUSED:	//������û������
		{
			lstrcpyn(pszBuffer,TEXT("Ŀ���������æ����û��������"),wBufferSize);
			break;
		}
	case WSAETIMEDOUT:		//���ӳ�ʱ
		{
			lstrcpyn(pszBuffer,TEXT("���ӳ�ʱ��������Ŀ������������ڻ��߷�������ַ��ʽ����ȷ��"),wBufferSize);
			break;
		}
	case WSAEHOSTUNREACH:
		{
			lstrcpyn(pszBuffer,TEXT("��������ʧ�ܣ������Ƿ��Ѿ��ɹ����ź����� Internet ��"),wBufferSize);
			break;
		}
	default:				//Ĭ�ϴ���
		{
			_snprintf(pszBuffer,wBufferSize,TEXT("���Ӵ���ţ�%ld����ϸ������Ϣ��ο����������ֲᣡ"),iErrorCode);
			break;
		}
	}

	return pszBuffer;
}*/

//��������
LRESULT CClientSocket::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//�ж�״̬
	int iErrorCode=WSAGETSELECTERROR(lParam);
	if (iErrorCode==0) m_cbSocketStatus=SOCKET_STATUS_CONNECT;
	else CloseSocket();

	//����֪ͨ
	//TCHAR szErrorDesc[128]=TEXT("");
	//GetConnectError(iErrorCode,szErrorDesc,sizeof(szErrorDesc));
	m_pIClientSocketSink->OnEventClientSocketLink(m_wSocketID, iErrorCode);

	return 1;
}

//�����ȡ
LRESULT CClientSocket::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//��ȡ����
		int iRetCode=recv(m_hSocket,(char *)m_cbRecvBuf+m_wRecvSize,sizeof(m_cbRecvBuf)-m_wRecvSize,0);
		if (iRetCode==SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");
		ASSERT(m_dwSendPacketCount>0);
		m_wRecvSize+=iRetCode;
		m_dwRecvTickCount=GetTickCount()/1000L;

		//��������
		WORD wPacketSize=0;
		BYTE cbDataBuffer[SOCKET_TCP_PACKET+sizeof(TCP_Head)];
		TCP_Head * pHead=(TCP_Head *)m_cbRecvBuf;

		while (m_wRecvSize>=sizeof(TCP_Head))
		{
			//Ч�����
			wPacketSize=pHead->TCPInfo.wPacketSize;
			//ASSERT(pHead->TCPInfo.cbVersion==SOCKET_TCP_VER);
			ASSERT(pHead->TCPInfo.cbDataKind == SOCKET_TCP_VER);
			ASSERT(wPacketSize<=(SOCKET_TCP_PACKET+sizeof(TCP_Head)));
			//if (pHead->TCPInfo.cbVersion!=SOCKET_TCP_VER) throw TEXT("���ݰ��汾����");
			if (pHead->TCPInfo.cbDataKind!=SOCKET_TCP_VER) throw TEXT("���ݰ��汾����");
			if (wPacketSize>(SOCKET_TCP_PACKET+sizeof(TCP_Head))) throw TEXT("���ݰ�̫��");
			if (m_wRecvSize<wPacketSize) return 1;

			//��������
			m_dwRecvPacketCount++;
			CopyMemory(cbDataBuffer,m_cbRecvBuf,wPacketSize);
			m_wRecvSize-=wPacketSize;
			MoveMemory(m_cbRecvBuf,m_cbRecvBuf+wPacketSize,m_wRecvSize);

			//��������
			WORD wRealySize = CYSEncrypt::CrevasseBuffer(cbDataBuffer, wPacketSize);
			ASSERT(wRealySize>=sizeof(TCP_Head));

			//��������
			WORD wDataSize=wRealySize-sizeof(TCP_Head);
			void * pDataBuffer=cbDataBuffer+sizeof(TCP_Head);
			TCP_Command Command=((TCP_Head *)cbDataBuffer)->CommandInfo;

			//�ں�����
			if (Command.wMainCmdID==MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
				case SUB_KN_DETECT_SOCKET:	//������
					{
						//��������
						SendData(MDM_KN_COMMAND,SUB_KN_DETECT_SOCKET,pDataBuffer,wDataSize);
						break;
					}
				}
				continue;
			}

			//��������
			bool bSuccess = m_pIClientSocketSink->OnEventClientSocketRead(m_wSocketID, Command, pDataBuffer, wDataSize);
			if (bSuccess==false) throw TEXT("�������ݰ�����ʧ��");
		};
	}
	catch (...) 
	{ 
		CloseSocket(); 
	}

	return 1;
}

//����ر�
LRESULT CClientSocket::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	//m_cbSocketStatus=SHUT_REASON_NORMAL;
	CloseSocket();
	return 1;
}

//SOCKET ��Ϣ�������
LRESULT	CClientSocket::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:	//��������
		{ 
			return OnSocketNotifyConnect(wParam,lParam); 
		}
	case FD_READ:		//���ݶ�ȡ
		{ 
			return OnSocketNotifyRead(wParam,lParam); 
		}
	case FD_CLOSE:		//����ر�
		{ 
			return OnSocketNotifyClose(wParam,lParam);	
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//����������
DECLARE_CREATE_MODULE(ClientSocket)

//////////////////////////////////////////////////////////////////////////
