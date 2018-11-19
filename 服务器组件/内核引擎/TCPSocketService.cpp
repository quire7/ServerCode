#include "StdAfx.h"
#include "TraceService.h"
#include "TCPSocketService.h"


//�궨��
#define ID_SOCKET_WND				10								//SOCKET ���� ID
#define WM_SOCKET_NOTIFY			WM_USER+10						//SOCKET ��Ϣ

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CTCPSocketService, CWnd)
	ON_MESSAGE(WM_SOCKET_NOTIFY, OnSocketNotify)
END_MESSAGE_MAP()

//��������
//////////////////////////////////////////////////////////////////////////
//���캯��
CTCPSocketService::CTCPSocketService(void)
{
	//�ָ�����
	m_hSocket = INVALID_SOCKET;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;
	m_wServiceID = 0;
	m_cbSocketStatus = SOCKET_STATUS_IDLE;
	m_wRecvSize = 0;
	m_dwServerIP = INADDR_NONE;
	m_wPort = 0;
	m_bSuccess = false;
}

//��������
CTCPSocketService::~CTCPSocketService(void)
{
	if (m_bService)
		ConcludeService();
}

//�����ӿ�
//////////////////////////////////////////////////////////////////////////
//�ӿڲ�ѯ
void * CTCPSocketService::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketService, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketService, Guid, dwQueryVer);
	return NULL;
}

//����ӿ�
//////////////////////////////////////////////////////////////////////////
//��������
bool CTCPSocketService::StartService()
{
	//Ч��״̬
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("��������ظ�������������������"), TraceLevel_Warning);
		return true;
	}

	//�󶨶���
	if (m_QueueService.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
	{
		CTraceService::TraceString(TEXT("��������봥�������ʧ��"), TraceLevel_Exception);
		return false;
	}

	try
	{
		//�󶨶���
		if (m_QueueService.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
		{
			CTraceService::TraceString(TEXT("���������봥�������ʧ��"), TraceLevel_Exception);
			return false;
		}

// 		m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
// 		if (m_hSocket == INVALID_SOCKET) return false;
		if ((m_hWnd == NULL) && (!Create(NULL, NULL, WS_CHILD, CRect(0, 0, 0, 0), GetDesktopWindow(), ID_SOCKET_WND, NULL)))
			return false;

		if (m_QueueService.StartService())
		{
			m_bService = true;
		}
	}
	catch (LPCTSTR pszError)
	{
		CTraceService::TraceString(pszError, TraceLevel_Exception);
		return false;
	}

	return m_bService;
}

//ֹͣ����
bool CTCPSocketService::ConcludeService()
{
	//���ñ���
	m_bService = false;

	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		WSAAsyncSelect(m_hSocket, GetSafeHwnd(), WM_SOCKET_NOTIFY, 0);
		m_hSocket = INVALID_SOCKET;
	}

	//�ָ�����
	m_hSocket = INVALID_SOCKET;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;
	m_cbSocketStatus = SOCKET_STATUS_IDLE;
	m_wRecvSize = 0;
	m_dwServerIP = INADDR_NONE;
	m_wPort = 0;
	m_bSuccess = false;

	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	m_QueueService.ConcludeService();

	return true;
}

//���ýӿ�
//////////////////////////////////////////////////////////////////////////
//���ú���
bool CTCPSocketService::SetServiceID(WORD wServiceID)
{
	m_wServiceID = wServiceID;
	return true;
}

//���ýӿ�
bool CTCPSocketService::SetTCPSocketEvent(IUnknownEx * pIUnknownEx)
{
	//״̬�ж�
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("����������ڷ���״̬���󶨲�������"), TraceLevel_Warning);
		return false;
	}

	//���ýӿ�
	if (m_QueueServiceEvent.SetQueueServiceSink(pIUnknownEx) == false)
	{
		CTraceService::TraceString(TEXT("��������봥�������ʧ��"), TraceLevel_Warning);
		return false;
	}

	return true;
}

//���ܽӿ�
//////////////////////////////////////////////////////////////////////////
//�ر�����
bool CTCPSocketService::CloseSocket()
{
	//�ر�����
	bool bClose = (m_hSocket != INVALID_SOCKET);
	if (bClose)
		return m_QueueService.AddToQueue(QUEUE_SAFE_CLOSE, NULL, 0);

	//�ָ�����
	m_hSocket = INVALID_SOCKET;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;
	m_cbSocketStatus = SOCKET_STATUS_IDLE;
	m_wRecvSize = 0;
	m_dwServerIP = INADDR_NONE;
	m_wPort = 0;

	return true;
}

//���ӵ�ַ
bool CTCPSocketService::Connect(DWORD dwServerIP, WORD wPort)
{
	if (m_bSuccess && ntohl(dwServerIP) == m_dwTempServerIP)//��¼��������BUG -- 
		m_dwServerIP = ntohl(dwServerIP);
	else m_dwServerIP = dwServerIP;

	m_wPort = wPort;
	m_dwTempServerIP = m_dwServerIP;

	try
	{
		return m_QueueService.AddToQueue(QUEUE_CONNECT_REQUEST, NULL, 0);
	}
	catch (LPCTSTR pszError)
	{
		CloseSocket(SHUT_REASON_EXCEPTION);
		throw pszError;
	}
	catch (...)
	{
		CloseSocket(SHUT_REASON_EXCEPTION);
		throw TEXT("���Ӳ���δ֪�쳣����");
	}

	return false;
}

//���ӵ�ַ
bool CTCPSocketService::Connect(LPCTSTR szServerIP, WORD wPort)
{
	//Ч������
	ASSERT(wPort != 0);
	ASSERT(szServerIP != NULL);
	if ((szServerIP == NULL) || (wPort == 0)) return false;
	return Connect(TranslateAddr(szServerIP), wPort);
}

//���ͺ���
bool CTCPSocketService::SendData(WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч��״̬
	if (m_hSocket == INVALID_SOCKET) return false;
	if (m_cbSocketStatus != SOCKET_STATUS_CONNECT) return false;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead = (TCP_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID = wMainCmdID;
	pHead->CommandInfo.wSubCmdID = wSubCmdID;

	//��������
	WORD wSendSize = CYSEncrypt::EncryptBuffer(cbDataBuffer, sizeof(TCP_Head), sizeof(cbDataBuffer));

	//���ñ���
	m_dwSendPacketCount++;

	//��������
	return SendDataBuffer(cbDataBuffer, wSendSize);
}

//֪ͨ�ص�
void CTCPSocketService::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	switch (wIdentifier)
	{
		case QUEUE_SEND_REQUEST:
		{
			ASSERT(pBuffer);
			ASSERT(wDataSize > 0);
			if (!pBuffer || wDataSize <= 0) return;

			//��������
			WORD wSended = 0;
			while (wSended < wDataSize)
			{
				int iErrorCode = send(m_hSocket, (char *)pBuffer + wSended, wDataSize - wSended, 0);
				if (iErrorCode == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						return;
					}
					return;
				}
				wSended += iErrorCode;
			}

			break;
		}

		case QUEUE_CONNECT_REQUEST:
		{
			//Ч��״̬
			if (m_dwServerIP == INADDR_NONE) throw TEXT("Ŀ���������ַ��ʽ����ȷ��������ٴγ��ԣ�");
			if (m_wPort == 0) throw TEXT("Ŀ���������ַ��ʽ����ȷ��������ٴγ��ԣ�");

			//���� SOCKET
			m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (m_hSocket == INVALID_SOCKET) throw TEXT("SOCKET ����ʧ��");

			//Ч�����
			ASSERT(m_hSocket != INVALID_SOCKET);
			ASSERT(m_cbSocketStatus == SOCKET_STATUS_IDLE);

			int iErrorCode = WSAAsyncSelect(m_hSocket, m_hWnd, WM_SOCKET_NOTIFY, FD_READ | FD_CONNECT | FD_CLOSE);
			if (iErrorCode == SOCKET_ERROR) throw TEXT("���ڲ����ڴ���");

			//��д��������ַ
			SOCKADDR_IN SocketAddr;
			memset(&SocketAddr, 0, sizeof(SocketAddr));
			SocketAddr.sin_family = AF_INET;
			SocketAddr.sin_port = htons(m_wPort);
			SocketAddr.sin_addr.S_un.S_addr = m_dwServerIP;

			iErrorCode = connect(m_hSocket, (sockaddr*) & SocketAddr, sizeof(sockaddr));
			if (SOCKET_ERROR == iErrorCode)
				iErrorCode = WSAGetLastError();

			break;
		}

		case QUEUE_SAFE_CLOSE:
		{
			if (m_hSocket != INVALID_SOCKET)
			{
				closesocket(m_hSocket);
				WSAAsyncSelect(m_hSocket, GetSafeHwnd(), WM_SOCKET_NOTIFY, 0);
				m_hSocket = INVALID_SOCKET;
			}

			//�ָ�����
			m_hSocket = INVALID_SOCKET;
			m_dwSendTickCount = 0;
			m_dwRecvTickCount = 0;
			m_dwSendPacketCount = 0;
			m_dwRecvPacketCount = 0;
			m_cbSocketStatus = SOCKET_STATUS_IDLE;
			m_wRecvSize = 0;
			m_dwServerIP = INADDR_NONE;
			m_wPort = 0;

			break;
		}

		case QUEUE_DETECT_SOCKET:
		{

		}

		default:
		{
			ASSERT(FALSE);
			return;
		}
	}
}

//���ͺ���
bool CTCPSocketService::SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч��״̬
	if (m_hSocket == INVALID_SOCKET) return false;
//	if (m_cbSocketStatus != SOCKET_STATUS_CONNECT) return false;

	//Ч���С
	ASSERT(wDataSize <= SOCKET_TCP_BUFFER);
	if (wDataSize > SOCKET_TCP_BUFFER) return false;

	//��������
	BYTE cbDataBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead = (TCP_Head *)cbDataBuffer;
	pHead->CommandInfo.wMainCmdID = wMainCmdID;
	pHead->CommandInfo.wSubCmdID = wSubCmdID;
	if (wDataSize > 0)
	{
		ASSERT(pData != NULL);
		CopyMemory(pHead + 1, pData, wDataSize);
	}

	//��������
	WORD wSendSize = CYSEncrypt::EncryptBuffer(cbDataBuffer, sizeof(TCP_Head) + wDataSize, sizeof(cbDataBuffer));

	//���ñ���
	m_dwSendPacketCount++;

	//��������
	return SendDataBuffer(cbDataBuffer, wSendSize);
}

//��������
bool CTCPSocketService::SendDataBuffer(void * pBuffer, WORD wSendSize)
{
	//Ч�����
	ASSERT(wSendSize != 0);
	ASSERT(pBuffer != NULL);
	if (NULL == pBuffer || 0 == wSendSize) return false;

	return m_QueueService.AddToQueue(QUEUE_SEND_REQUEST, pBuffer, wSendSize);
}

//�ر�����
VOID CTCPSocketService::CloseSocket(BYTE cbShutReason)
{
	//�ر�����
	m_QueueServiceEvent.PostTCPSocketShutEvent(m_wServiceID, cbShutReason);
	CloseSocket();
}

//���ͷ�������ַ
DWORD CTCPSocketService::TranslateAddr(LPCTSTR pszServerAddr)
{
	//ת����ַ
	DWORD dwServerIP = inet_addr((LPCSTR)CT2A(pszServerAddr));
	if (dwServerIP == INADDR_NONE)
	{
		LPHOSTENT lpHost = gethostbyname((LPCSTR)CT2A(pszServerAddr));
		if (lpHost == NULL) return INADDR_NONE;
		dwServerIP = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	return dwServerIP;
}

//��������
LRESULT CTCPSocketService::OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam)
{
	//�ж�״̬
	int iErrorCode = WSAGETSELECTERROR(lParam);
	if (iErrorCode == 0)
	{
		m_cbSocketStatus = SOCKET_STATUS_CONNECT;
		m_bSuccess = true;
		//����֪ͨ
		m_QueueServiceEvent.PostTCPSocketLinkEvent(m_wServiceID, iErrorCode);
	}
	else CloseSocket(SHUT_REASON_TIME_OUT);

	return 1;
}

//�����ȡ
LRESULT CTCPSocketService::OnSocketNotifyRead(WPARAM wParam, LPARAM lParam)
{
	try
	{
		//��ȡ����
		int iRetCode = recv(m_hSocket, (char *)m_cbRecvBuf + m_wRecvSize, sizeof(m_cbRecvBuf) - m_wRecvSize, 0);
		if (iRetCode == SOCKET_ERROR) throw TEXT("�������ӹرգ���ȡ����ʧ��");
		ASSERT(m_dwSendPacketCount > 0);
		m_wRecvSize += iRetCode;
		m_dwRecvTickCount = GetTickCount() / 1000L;

		//��������
		WORD wPacketSize = 0;
		BYTE cbDataBuffer[SOCKET_TCP_BUFFER+sizeof(TCP_Head)];
		TCP_Head * pHead = (TCP_Head *)m_cbRecvBuf;

		while (m_wRecvSize >= sizeof(TCP_Head))
		{
			//Ч�����
			wPacketSize = pHead->TCPInfo.wPacketSize;
			ASSERT(pHead->TCPInfo.cbDataKind == SOCKET_TCP_VER);
			//ASSERT(pHead->TCPInfo.cbVersion == SOCKET_TCP_VER);
			ASSERT(wPacketSize <= (SOCKET_TCP_BUFFER + sizeof(TCP_Head)));
			//if (pHead->TCPInfo.cbVersion != SOCKET_TCP_VER) throw TEXT("���ݰ��汾����");
			if (pHead->TCPInfo.cbDataKind != SOCKET_TCP_VER) throw TEXT("���ݰ��汾����");
			if (wPacketSize > (SOCKET_TCP_BUFFER + sizeof(TCP_Head))) throw TEXT("���ݰ�̫��");
			if (m_wRecvSize < wPacketSize) return 1;

			//��������
			m_dwRecvPacketCount++;
			CopyMemory(cbDataBuffer, m_cbRecvBuf, wPacketSize);
			m_wRecvSize -= wPacketSize;
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf + wPacketSize, m_wRecvSize);

			//��������
			WORD wRealySize = CYSEncrypt::CrevasseBuffer(cbDataBuffer, wPacketSize);
			ASSERT(wRealySize >= sizeof(TCP_Head));

			//��������
			WORD wDataSize = wRealySize - sizeof(TCP_Head);
			void * pDataBuffer = cbDataBuffer + sizeof(TCP_Head);
			TCP_Command Command = ((TCP_Head *)cbDataBuffer)->CommandInfo;

			//�ں�����
			if (Command.wMainCmdID == MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
					case SUB_KN_DETECT_SOCKET:	//������
					{
						//��������
						SendData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, pDataBuffer, wDataSize);
						break;
					}
				}
				continue;
			}

			//��������
			bool bSuccess = m_QueueServiceEvent.PostTCPSocketReadEvent(m_wServiceID, Command, pDataBuffer, wDataSize);
			if ( !bSuccess )
			{
				CString strTmp;
				strTmp.Format(_T("MainID: %d SubID: %d DataSize: %d"), Command.wMainCmdID, Command.wSubCmdID, wDataSize);
				TRACE(strTmp);
				throw TEXT("�������ݰ�����ʧ��");
			}
		}
	}
	catch ( TCHAR *e )
	{
		TRACE(e);
		CloseSocket(SHUT_REASON_EXCEPTION);
	}
	catch (...)
	{
		TRACE(_T("δ֪�쳣"));
		CloseSocket(SHUT_REASON_EXCEPTION);
	}

	return 1;
}

//����ر�
LRESULT CTCPSocketService::OnSocketNotifyClose(WPARAM wParam, LPARAM lParam)
{
	CloseSocket(SHUT_REASON_REMOTE);
	return 1;
}

//SOCKET ��Ϣ�������
LRESULT	CTCPSocketService::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:	//��������
		{
			return OnSocketNotifyConnect(wParam, lParam);
		}
		case FD_READ:		//���ݶ�ȡ
		{
			return OnSocketNotifyRead(wParam, lParam);
		}
		case FD_CLOSE:		//����ر�
		{
			return OnSocketNotifyClose(wParam, lParam);
		}
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////////
//����������
extern "C" __declspec(dllexport) void * CreateTCPSocketService(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CTCPSocketService * pTCPSocketService = NULL;
	try
	{
		pTCPSocketService = new CTCPSocketService();
		if (pTCPSocketService == NULL) throw TEXT("����ʧ��");
		void * pObject = pTCPSocketService->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pTCPSocketService);
	return NULL;
}