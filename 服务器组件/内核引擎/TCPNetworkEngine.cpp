#include "StdAfx.h"
#include "TraceService.h" //
#include "TCPNetworkEngine.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#define TIME_BREAK_READY			9000L								//�ж�ʱ��
#define TIME_BREAK_CONNECT			4000L								//�ж�ʱ��
#define TIME_DETECT_SOCKET			20000L								//���ʱ��

//////////////////////////////////////////////////////////////////////////

//��������
#define QUEUE_SEND_REQUEST			1									//���ͱ�ʶ
#define QUEUE_SAFE_CLOSE			2									//��ȫ�ر�
#define QUEUE_ALLOW_BATCH			3									//����Ⱥ��
#define QUEUE_DETECT_SOCKET			4									//�������

//��������ṹ
struct tagSendDataRequest
{
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
	DWORD							dwSocketID;							//��������
	WORD							wDataSize;							//���ݴ�С
	BYTE							cbSendBuf[SOCKET_TCP_BUFFER];		//���ͻ���
};

//����Ⱥ��
struct tagAllowBatchSend
{
	DWORD							dwSocketID;							//��������
	BYTE							cbAllow;							//�����־
};

//��ȫ�ر�
struct tagSafeCloseSocket
{
	DWORD							dwSocketID;							//��������
};

//���ṹ��Ϣ
struct TCP_KN_DetectSocket
{
	DWORD								dwSendTickCount;					//����ʱ��
	DWORD								dwRecvTickCount;					//����ʱ��
};

//////////////////////////////////////////////////////////////////////////

//���캯��
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	memset(&m_WSABuffer, 0, sizeof(m_WSABuffer));
	memset(&m_OverLapped, 0, sizeof(m_OverLapped));
}

//��������
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
COverLappedSend::COverLappedSend() : COverLapped(OperationType_Send)
{
	m_WSABuffer.len = 0;
	m_WSABuffer.buf = (char *)m_cbBuffer;
}

//��������
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerSocketItem::CServerSocketItem(WORD wIndex, IServerSocketItemSink * pIServerSocketItemSink)
		: m_wIndex(wIndex), m_pIServerSocketItemSink(pIServerSocketItemSink)
{
	m_cbSocketType = eSocketType_TCP;

	m_wRountID = 1;
	m_dwRecvSize = 0;
	m_bNotify = true;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_bAllowBatch = true;
	m_dwClientAddr = 0;
	m_dwConnectTime = 0;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;
	m_hSocket = INVALID_SOCKET;

	m_websocket.clear();
}

//��������
CServerSocketItem::~CServerSocketItem(void)
{
	//ɾ�������ص� IO
	INT_PTR iCount = m_OverLappedSendFree.GetCount();
	for (INT_PTR i = 0; i < iCount; i++) delete m_OverLappedSendFree[i];
	m_OverLappedSendFree.RemoveAll();

	//ɾ����ص� IO
	iCount = m_OverLappedSendActive.GetCount();
	for (INT_PTR i = 0; i < iCount; i++) delete m_OverLappedSendActive[i];
	m_OverLappedSendActive.RemoveAll();

	return;
}

//��ȡ���ͽṹ
COverLappedSend * CServerSocketItem::GetSendOverLapped()
{
	//Ѱ�ҿ��нṹ
	COverLappedSend * pOverLappedSend = NULL;
	INT_PTR nFreeCount = m_OverLappedSendFree.GetCount();
	if (nFreeCount > 0)
	{
		pOverLappedSend = m_OverLappedSendFree[nFreeCount-1];
		ASSERT(pOverLappedSend != NULL);
		m_OverLappedSendFree.RemoveAt(nFreeCount - 1);
		m_OverLappedSendActive.Add(pOverLappedSend);
		return pOverLappedSend;
	}

	//�½����ͽṹ
	try
	{
		pOverLappedSend = new COverLappedSend;
		ASSERT(pOverLappedSend != NULL);
		m_OverLappedSendActive.Add(pOverLappedSend);
		return pOverLappedSend;
	}
	catch (...) { }
	return NULL;
}

//�󶨶���
DWORD CServerSocketItem::Attach(SOCKET hSocket, DWORD dwClientAddr)
{
	//Ч������
	ASSERT(dwClientAddr != 0);
	ASSERT(m_bRecvIng == false);
	ASSERT(IsValidSocket() == false);
	ASSERT(hSocket != INVALID_SOCKET);

	//���ñ���
	m_bNotify = false;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_hSocket = hSocket;
	m_dwClientAddr = dwClientAddr;
	m_dwRecvTickCount = GetTickCount();
	m_dwConnectTime = (DWORD)time(NULL);

	//����֪ͨ
	m_pIServerSocketItemSink->OnSocketAcceptEvent(this);

	return GetSocketID();
}

//���ͺ���
bool CServerSocketItem::SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//Ч��״̬
	if (m_bCloseIng == true) return false;
	if (m_wRountID != wRountID) return false;
	if (m_dwRecvPacketCount == 0) return false;
	if (IsValidSocket() == false) return false;

	//Ѱ�ҷ��ͽṹ
	COverLappedSend * pOverLappedSend = GetSendOverLapped();
	ASSERT(pOverLappedSend != NULL);
	if (pOverLappedSend == NULL) return false;

	if (IsWebSocketType())
	{
		TCP_Head tcpHead;
		tcpHead.CommandInfo.wMainCmdID = wMainCmdID;
		tcpHead.CommandInfo.wSubCmdID = wSubCmdID;
		tcpHead.TCPInfo.cbCheckCode = 0;
		tcpHead.TCPInfo.cbDataKind = DK_MAPPED;
		tcpHead.TCPInfo.wPacketSize = sizeof(TCP_Head);
		m_websocket.EncodeFrame((char*)&tcpHead, sizeof(tcpHead), (char*)pOverLappedSend->m_cbBuffer, sizeof(pOverLappedSend->m_cbBuffer), pOverLappedSend->m_WSABuffer.len);
	}
	else
	{
		//��������;
		TCP_Head * pHead = (TCP_Head *)pOverLappedSend->m_cbBuffer;
		pHead->CommandInfo.wMainCmdID = wMainCmdID;
		pHead->CommandInfo.wSubCmdID = wSubCmdID;
		WORD wSendSize = CYSEncrypt::EncryptBuffer(pOverLappedSend->m_cbBuffer, sizeof(TCP_Head), sizeof(pOverLappedSend->m_cbBuffer));
		pOverLappedSend->m_WSABuffer.len = wSendSize;
	}

	//���ñ���
	m_dwSendPacketCount++;

	//��������
	if (m_OverLappedSendActive.GetCount() == 1)
	{
		DWORD dwThancferred = 0;
		int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
		if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			OnSendCompleted(pOverLappedSend, 0L);
			return false;
		}
	}

	return true;
}

//���ͺ���
bool CServerSocketItem::SendData(void * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//Ч�����
	ASSERT(wDataSize <= SOCKET_TCP_BUFFER);

	//Ч��״̬
	if (m_bCloseIng == true) return false;
	if (m_wRountID != wRountID) return false;
	if (m_dwRecvPacketCount == 0) return false;
	if (IsValidSocket() == false) return false;
	if (wDataSize > SOCKET_TCP_BUFFER) return false;

	//Ѱ�ҷ��ͽṹ
	COverLappedSend * pOverLappedSend = GetSendOverLapped();
	ASSERT(pOverLappedSend != NULL);
	if (pOverLappedSend == NULL) return false;

	if (IsWebSocketType())
	{
		char cbBuffer[SOCKET_TCP_BUFFER] = { 0 };
		TCP_Head tcpHead;
		tcpHead.CommandInfo.wMainCmdID = wMainCmdID;
		tcpHead.CommandInfo.wSubCmdID = wSubCmdID;
		tcpHead.TCPInfo.cbCheckCode = 0;
		tcpHead.TCPInfo.cbDataKind = DK_MAPPED;
		tcpHead.TCPInfo.wPacketSize = sizeof(TCP_Head) + wDataSize;
		memcpy(cbBuffer, &tcpHead, sizeof(tcpHead));
		if (wDataSize > 0)
		{
			memcpy(cbBuffer + sizeof(tcpHead), pData, wDataSize);
		}
		m_websocket.EncodeFrame(cbBuffer, sizeof(tcpHead) + wDataSize, (char*)pOverLappedSend->m_cbBuffer, sizeof(pOverLappedSend->m_cbBuffer), pOverLappedSend->m_WSABuffer.len);
	}
	else
	{
		//��������
		TCP_Head * pHead = (TCP_Head *)pOverLappedSend->m_cbBuffer;
		pHead->CommandInfo.wMainCmdID = wMainCmdID;
		pHead->CommandInfo.wSubCmdID = wSubCmdID;
		if (wDataSize > 0)
		{
			ASSERT(pData != NULL);
			memcpy(pHead + 1, pData, wDataSize);
		}
		WORD wSendSize = CYSEncrypt::EncryptBuffer(pOverLappedSend->m_cbBuffer, sizeof(TCP_Head) + wDataSize, sizeof(pOverLappedSend->m_cbBuffer));
		pOverLappedSend->m_WSABuffer.len = wSendSize;
	}

	//���ñ���
	m_dwSendPacketCount++;

	//��������
	if (m_OverLappedSendActive.GetCount() == 1)
	{
		DWORD dwThancferred = 0;
		int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
		if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			OnSendCompleted(pOverLappedSend, 0L);
			return false;
		}
	}

	return true;
}

//Ͷ�ݽ���
bool CServerSocketItem::RecvData()
{
	//Ч�����
	ASSERT(m_bRecvIng == false);
	ASSERT(m_hSocket != INVALID_SOCKET);

	//�жϹر�
	if (m_bCloseIng == true)
	{
		if (m_OverLappedSendActive.GetCount() == 0) CloseSocket(m_wRountID);
		return false;
	}

	//��������
	m_bRecvIng = true;
	DWORD dwThancferred = 0, dwFlags = 0;
	int iRetCode = WSARecv(m_hSocket, &m_OverLappedRecv.m_WSABuffer, 1, &dwThancferred, &dwFlags, &m_OverLappedRecv.m_OverLapped, NULL);
	if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
	{
		m_bRecvIng = false;
		CloseSocket(m_wRountID);
		return false;
	}

	return true;
}

//�ر�����
bool CServerSocketItem::CloseSocket(WORD wRountID)
{
	//״̬�ж�
	if (m_wRountID != wRountID) return false;

	//�ر�����
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	//�жϹر�
	if ((m_bRecvIng == false) && (m_OverLappedSendActive.GetCount() == 0)) OnCloseCompleted();

	return true;
}

//���ùر�
bool CServerSocketItem::ShutDownSocket(WORD wRountID)
{
	return true;

	//״̬�ж�
	if (m_wRountID != wRountID) return false;
	if (m_hSocket == INVALID_SOCKET) return false;

	//���ñ���
	if (m_bCloseIng == false)
	{
		m_bCloseIng = true;
		//if (m_OverLappedSendActive.GetCount() == 0) CloseSocket(wRountID);
	}

	return true;
}

//����Ⱥ��
bool CServerSocketItem::AllowBatchSend(WORD wRountID, bool bAllowBatch)
{
	//״̬�ж�
	if (m_wRountID != wRountID) return false;
	if (m_hSocket == INVALID_SOCKET) return false;

	//���ñ���
	m_bAllowBatch = bAllowBatch;

	return true;
}

//���ñ���
void CServerSocketItem::ResetSocketData()
{
	//Ч��״̬
	ASSERT(m_hSocket == INVALID_SOCKET);

	//��������;
	m_cbSocketType = eSocketType_TCP;

	m_wRountID++;
	m_dwRecvSize = 0;
	m_dwClientAddr = 0;
	m_dwConnectTime = 0;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;

	//״̬����
	m_bNotify = true;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_bAllowBatch = true;
	m_OverLappedSendFree.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();

	m_websocket.clear();

	return;
}

//������ɺ���
bool CServerSocketItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//Ч�����
	ASSERT(pOverLappedSend != NULL);
	ASSERT(m_OverLappedSendActive.GetCount() > 0);
	ASSERT(pOverLappedSend == m_OverLappedSendActive[0]);

	//�ͷŷ��ͽṹ
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendFree.Add(pOverLappedSend);

	//���ñ���
	if (dwThancferred != 0) m_dwSendTickCount = GetTickCount();

	//�жϹر�
	if (m_hSocket == INVALID_SOCKET)
	{
		m_OverLappedSendFree.Append(m_OverLappedSendActive);
		m_OverLappedSendActive.RemoveAll();
		CloseSocket(m_wRountID);
		return true;
	}

	//������������
	if (m_OverLappedSendActive.GetCount() > 0)
	{
		DWORD dwThancferred = 0;
		pOverLappedSend = m_OverLappedSendActive[0];
		ASSERT(pOverLappedSend != NULL);
		int iRetCode = WSASend(m_hSocket, &pOverLappedSend->m_WSABuffer, 1, &dwThancferred, 0, &pOverLappedSend->m_OverLapped, NULL);
		if ((iRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
		{
			m_OverLappedSendFree.Append(m_OverLappedSendActive);
			m_OverLappedSendActive.RemoveAll();
			CloseSocket(m_wRountID);
			return false;
		}
		return true;
	}

	//�жϹر�
	if (m_bCloseIng == true)
		CloseSocket(m_wRountID);

	return true;
}

//������ɺ���
bool CServerSocketItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//Ч������
	ASSERT(m_bRecvIng == true);

	//���ñ���
	m_bRecvIng = false;
	m_dwRecvTickCount = GetTickCount();

	//�жϹر�
	if (m_hSocket == INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	if (IsWebSocketType())
	{
		//��������;
		int iRetCode = recv(m_hSocket, (char *)m_websocket.m_cbRecvBuf + m_websocket.m_dwRecvSize, sizeof(m_websocket.m_cbRecvBuf) - m_websocket.m_dwRecvSize, 0);
		if (iRetCode <= 0)
		{
			CloseSocket(m_wRountID);
			return true;
		}

		m_websocket.m_dwRecvSize += iRetCode;

		int nFrameRes = m_websocket.DecodeFrame((char *)m_cbRecvBuf + m_dwRecvSize, sizeof(m_cbRecvBuf) - m_dwRecvSize, m_dwRecvSize);
		if (WS_CLOSING_FRAME == nFrameRes || WS_ERROR_FRAME == nFrameRes)
		{
			CloseSocket(m_wRountID);
			return true;
		}
		else if (WS_MIDDLE_FRAME == nFrameRes)
		{
			return RecvData();
		}
	}
	else
	{
		//��������;
		int iRetCode = recv(m_hSocket, (char *)m_cbRecvBuf + m_dwRecvSize, sizeof(m_cbRecvBuf) - m_dwRecvSize, 0);
		if (iRetCode <= 0)
		{
			CloseSocket(m_wRountID);
			return true;
		}

		if (m_dwRecvPacketCount == 0)
		{
			m_cbRecvBuf[iRetCode] = '\0';

			if (bool bWebSocket = m_websocket.ParseWebSocketHandShake((char*)m_cbRecvBuf, iRetCode))
			{
				m_dwRecvPacketCount++;
				m_cbSocketType = eSocketType_WebSocket;

				if (SOCKET_ERROR == send(m_hSocket, m_websocket.m_szHandShakeResponse, strlen(m_websocket.m_szHandShakeResponse), 0))
				{
					CloseSocket(m_wRountID);
					return true;
				}

				return RecvData();
			}
		}

		//�������;
		m_dwRecvSize += iRetCode;
	}

	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead = (TCP_Head *)m_cbRecvBuf;

	//��������
	try
	{
		while (m_dwRecvSize >= sizeof(TCP_Head))
		{
			//Ч������
			WORD wPacketSize = pHead->TCPInfo.wPacketSize;
			if (wPacketSize > SOCKET_TCP_BUFFER) throw TEXT("���ݰ�����");
			if (wPacketSize < sizeof(TCP_Head)) throw TEXT("���ݰ��Ƿ�");
			//if (pHead->TCPInfo.cbVersion != SOCKET_TCP_VER) throw TEXT("���ݰ��汾����");
			if (pHead->TCPInfo.cbDataKind != SOCKET_TCP_VER) throw TEXT("���ݰ��汾����");
			if (m_dwRecvSize < wPacketSize) break;

			//��ȡ����
			CopyMemory(cbBuffer, m_cbRecvBuf, wPacketSize);

			WORD wRealySize = wPacketSize;
			if (!IsWebSocketType())
			{
				wRealySize = CYSEncrypt::CrevasseBuffer(cbBuffer, wPacketSize);
			}

			ASSERT(wRealySize >= sizeof(TCP_Head));
			m_dwRecvPacketCount++;

			//��������
			WORD wDataSize = wRealySize - sizeof(TCP_Head);
			void * pDataBuffer = cbBuffer + sizeof(TCP_Head);
			TCP_Command Command = ((TCP_Head *)cbBuffer)->CommandInfo;

			//�ں�����
			if (Command.wMainCmdID == MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
					case SUB_KN_DETECT_SOCKET:	//������
					{
						break;
					}
					case SUB_KN_VALIDATE_SOCKET:
					{
						break;
					}
					default:
					{
						throw TEXT("�Ƿ�������");
					}
				}
			}
			else
			{
				//if (IsWebSocketType())
				//{	//just for test modify by fxd;
				//	DWORD dwSendSize = 0;
				//	char szBuf[1024] = { 0 };

				//	if (WS_EMPTY_FRAME == m_websocket.EncodeFrame((char*)m_cbRecvBuf, wPacketSize, szBuf, sizeof(szBuf), dwSendSize))
				//	{
				//		if (SOCKET_ERROR == send(m_hSocket, szBuf, dwSendSize, 0))
				//		{
				//			CloseSocket(m_wRountID);
				//			return true;
				//		}
				//	}
				//}
				//else
				//{	//��Ϣ����;
				//	m_pIServerSocketItemSink->OnSocketReadEvent(Command, pDataBuffer, wDataSize, this);
				//}

				//��Ϣ����;
				m_pIServerSocketItemSink->OnSocketReadEvent(Command, pDataBuffer, wDataSize, this);
			}

			//ɾ����������
			m_dwRecvSize -= wPacketSize;
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf + wPacketSize, m_dwRecvSize);
		}
	}
	catch (LPCTSTR strExp)
	{
		::OutputDebugString(strExp);
		CloseSocket(m_wRountID);
		return false;
	}
	catch (...)
	{
		CloseSocket(m_wRountID);
		return false;
	}

	return RecvData();
}

//�ر����֪ͨ
bool CServerSocketItem::OnCloseCompleted()
{
	//Ч��״̬
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(m_OverLappedSendActive.GetCount() == 0);

	//�ر��¼�
	ASSERT(m_bNotify == false);
	if (m_bNotify == false)
	{
		m_bNotify = true;
		m_pIServerSocketItemSink->OnSocketCloseEvent(this);
	}

	//״̬����
	ResetSocketData();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerSocketRSThread::CServerSocketRSThread(void)
{
	m_hCompletionPort = NULL;
}

//��������
CServerSocketRSThread::~CServerSocketRSThread(void)
{
}

//���ú���
bool CServerSocketRSThread::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort != NULL);
	m_hCompletionPort = hCompletionPort;
	return true;
}

//���к���
bool CServerSocketRSThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort != NULL);

	//��������
	DWORD dwThancferred = 0;
	OVERLAPPED * pOverLapped = NULL;
	COverLapped * pSocketLapped = NULL;
	CServerSocketItem * pServerSocketItem = NULL;

	//�ȴ���ɶ˿�
	BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &dwThancferred, (PULONG_PTR) & pServerSocketItem, &pOverLapped, INFINITE);
	if ((bSuccess == FALSE) && (GetLastError() != ERROR_NETNAME_DELETED)) return false;
	if ((pServerSocketItem == NULL) && (pOverLapped == NULL)) return false;

	//�������
	ASSERT(pOverLapped != NULL);
	ASSERT(pServerSocketItem != NULL);
	pSocketLapped = CONTAINING_RECORD(pOverLapped, COverLapped, m_OverLapped);
	switch (pSocketLapped->GetOperationType())
	{
		case OperationType_Recv:	//SOCKET ���ݶ�ȡ
		{
			COverLappedRecv * pOverLappedRecv = (COverLappedRecv *)pSocketLapped;
			CYSDataLocker lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnRecvCompleted(pOverLappedRecv, dwThancferred);
			break;
		}
		case OperationType_Send:	//SOCKET ���ݷ���
		{
			COverLappedSend * pOverLappedSend = (COverLappedSend *)pSocketLapped;
			CYSDataLocker lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnSendCompleted(pOverLappedSend, dwThancferred);
			break;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSocketAcceptThread::CSocketAcceptThread(void)
{
	m_hCompletionPort = NULL;
	m_pTCPSocketManager = NULL;
	m_hListenSocket = INVALID_SOCKET;
}

//��������
CSocketAcceptThread::~CSocketAcceptThread(void)
{
}

//���ú���
bool CSocketAcceptThread::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CTCPNetworkEngine * pTCPSocketManager)
{
	ASSERT(hCompletionPort != NULL);
	ASSERT(pTCPSocketManager != NULL);
	ASSERT(hListenSocket != INVALID_SOCKET);
	m_hListenSocket = hListenSocket;
	m_hCompletionPort = hCompletionPort;
	m_pTCPSocketManager = pTCPSocketManager;
	return true;
}

//���к���
bool CSocketAcceptThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort != NULL);
	ASSERT(m_pTCPSocketManager != NULL);

	//���ñ���
	SOCKADDR_IN	SocketAddr;
	CServerSocketItem * pServerSocketItem = NULL;
	SOCKET hConnectSocket = INVALID_SOCKET;
	int nBufferSize = sizeof(SocketAddr);

	try
	{
		//��������
		hConnectSocket = WSAAccept(m_hListenSocket, (SOCKADDR *) & SocketAddr, &nBufferSize, NULL, NULL);
		if (hConnectSocket == INVALID_SOCKET) return false;

		//��ȡ����
		pServerSocketItem = m_pTCPSocketManager->ActiveSocketItem();
		if (pServerSocketItem == NULL) throw TEXT("�������Ӷ���ʧ��");

		//�������
		CYSDataLocker lock(pServerSocketItem->GetSignedLock());
		pServerSocketItem->Attach(hConnectSocket, SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket, m_hCompletionPort, (ULONG_PTR)pServerSocketItem, 0);
		pServerSocketItem->RecvData();
	}
	catch (...)
	{
		//�������
		ASSERT(pServerSocketItem == NULL);
		if (hConnectSocket != INVALID_SOCKET)	closesocket(hConnectSocket);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSocketDetectThread::CSocketDetectThread(void)
{
	m_dwTickCount = 0;;
	m_pTCPSocketManager = NULL;
}

//��������
CSocketDetectThread::~CSocketDetectThread(void)
{
}

//���ú���
bool CSocketDetectThread::InitThread(CTCPNetworkEngine * pTCPSocketManager)
{
	//Ч�����
	ASSERT(pTCPSocketManager != NULL);

	//���ñ���
	m_dwTickCount = 0L;
	m_pTCPSocketManager = pTCPSocketManager;

	return true;
}

//���к���
bool CSocketDetectThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_pTCPSocketManager != NULL);

	//���ü��
	Sleep(500);
	m_dwTickCount += 500L;

	//�������
	if (m_dwTickCount > 20000L)
	{
		m_dwTickCount = 0L;
		m_pTCPSocketManager->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTCPNetworkEngine::CTCPNetworkEngine(void)
{
	m_bService = false;
	m_wListenPort = 0;
	m_dwLastDetect = 0;
	m_wMaxSocketItem = 0;
	m_hCompletionPort = NULL;
	m_hServerSocket = INVALID_SOCKET;

	return;
}

//��������
CTCPNetworkEngine::~CTCPNetworkEngine(void)
{
	//ֹͣ����
	ConcludeService();

	//�ͷŴ洢����
	CServerSocketItem * pSocketItem = NULL;
	for (INT_PTR i = 0; i < m_StorageSocketItem.GetCount(); i++)
	{
		pSocketItem = m_StorageSocketItem[i];
		ASSERT(pSocketItem != NULL);
		SafeDelete(pSocketItem);
	}
	m_StorageSocketItem.RemoveAll();

	return;
}

//�ӿڲ�ѯ
void * CTCPNetworkEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPNetworkEngine, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPNetworkEngine, Guid, dwQueryVer);
	return NULL;
}

//���ö˿�
WORD CTCPNetworkEngine::GetServicePort()
{
	return m_wListenPort;
}

//��ǰ�˿�
WORD CTCPNetworkEngine::GetCurrentPort()
{
	return m_wListenPort;
}

//���ýӿ�
bool CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
	//״̬�ж�
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("�������洦�ڷ���״̬���󶨲�������"), TraceLevel_Exception);
		return false;
	}

	//���ýӿ�
	if (m_QueueServiceEvent.SetQueueServiceSink(pIUnknownEx) == false)
	{
		CTraceService::TraceString(TEXT("���������봥�������ʧ��"), TraceLevel_Exception);
		return false;
	}

	return true;
}

//���ò���
bool CTCPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect)
{
	//Ч��״̬
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("�������洦�ڷ���״̬���˿ڰ󶨲�������"), TraceLevel_Exception);
		return false;
	}

	//�жϲ���
	if (wServicePort == 0)
	{
		CTraceService::TraceString(TEXT("δָ������˿ڣ����������"), TraceLevel_Normal);
	}

	//���ñ���
	m_wListenPort = wServicePort;
	m_wMaxSocketItem = wMaxConnect;
	return true;
}

//��������
bool CTCPNetworkEngine::StartService()
{
	DWORD i = 0;
	//Ч��״̬
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("���������ظ�������������������"), TraceLevel_Warning);
		return true;
	}

	//�󶨶���
	if (m_SendQueueService.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
	{
		CTraceService::TraceString(TEXT("���������봥�������ʧ��"), TraceLevel_Exception);
		return false;
	}

	try
	{
		//��ȡϵͳ��Ϣ
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadCount = SystemInfo.dwNumberOfProcessors;

		//������ɶ˿�
		m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, SystemInfo.dwNumberOfProcessors);
		if (m_hCompletionPort == NULL) throw TEXT("����������ɶ˿ڴ���ʧ��");

		//��������SOCKET
		struct sockaddr_in SocketAddr;
		memset(&SocketAddr, 0, sizeof(SocketAddr));
		SocketAddr.sin_addr.s_addr = INADDR_ANY;
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(m_wListenPort);
		m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_hServerSocket == INVALID_SOCKET) throw TEXT("����������� SOCKET ����ʧ��");
		int iErrorCode = bind(m_hServerSocket, (SOCKADDR*) & SocketAddr, sizeof(SocketAddr));
		if (iErrorCode == SOCKET_ERROR) throw TEXT("������������˿ڱ�ռ�ã��˿ڰ�ʧ��");
		iErrorCode = listen(m_hServerSocket, 200);
		if (iErrorCode == SOCKET_ERROR) throw TEXT("������������˿ڱ�ռ�ã��˿ڼ���ʧ��");

		//��ȡ�˿�
		int iLen=sizeof(SocketAddr); 
		getsockname(m_hServerSocket,(SOCKADDR*) & SocketAddr, &iLen);
		m_wListenPort=htons(SocketAddr.sin_port);

		//�������Ͷ���
		bool bSuccess = m_SendQueueService.StartService();
		if (bSuccess == false) throw TEXT("�������淢�Ͷ��з�������ʧ��");

		//������д�߳�
		for (i = 0; i < dwThreadCount; i++)
		{
			CServerSocketRSThread * pServerSocketRSThread = new CServerSocketRSThread();
			if (pServerSocketRSThread == NULL) throw TEXT("���������д�̷߳��񴴽�ʧ��");
			bSuccess = pServerSocketRSThread->InitThread(m_hCompletionPort);
			if (bSuccess == false) throw TEXT("���������д�̷߳�������ʧ��");
			m_SocketRSThreadArray.Add(pServerSocketRSThread);
		}

		//����Ӧ���߳�
		bSuccess = m_SocketAcceptThread.InitThread(m_hCompletionPort, m_hServerSocket, this);
		if (bSuccess == false) throw TEXT("����������������̷߳�������");

		//���ж�д�߳�
		for (i = 0; i < dwThreadCount; i++)
		{
			CServerSocketRSThread * pServerSocketRSThread = m_SocketRSThreadArray[i];
			ASSERT(pServerSocketRSThread != NULL);
			bSuccess = pServerSocketRSThread->StartThread();
			if (bSuccess == false) throw TEXT("���������д�̷߳�������ʧ��");
		}

		//�������߳�
		m_SocketDetectThread.InitThread(this);
		bSuccess = m_SocketDetectThread.StartThread();
		if (bSuccess == false) throw TEXT("����������̷߳�������ʧ��");

		//����Ӧ���߳�
		bSuccess = m_SocketAcceptThread.StartThread();
		if (bSuccess == false) throw TEXT("������������̷߳�������ʧ��");

		//���ñ���
		m_bService = true;
	}
	catch (LPCTSTR pszError)
	{
		CTraceService::TraceString(pszError, TraceLevel_Exception);
		return false;
	}

	return true;
}

//ֹͣ����
bool CTCPNetworkEngine::ConcludeService()
{
	//���ñ���
	m_bService = false;
	m_dwLastDetect = 0L;

	//ֹͣ����߳�
	m_SocketDetectThread.ConcludeThread(INFINITE);

	//��ֹӦ���߳�
	if (m_hServerSocket != INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
	}
	m_SocketAcceptThread.ConcludeThread(INFINITE);

	//ֹͣ���Ͷ���
	m_SendQueueService.ConcludeService();

	//�ͷŶ�д�߳�
	INT_PTR nCount = m_SocketRSThreadArray.GetCount(), i = 0;
	if (m_hCompletionPort != NULL)
	{
		for (i = 0; i < nCount; i++) PostQueuedCompletionStatus(m_hCompletionPort, 0, NULL, NULL);
	}
	for (i = 0; i < nCount; i++)
	{
		CServerSocketRSThread * pSocketThread = m_SocketRSThreadArray[i];
		ASSERT(pSocketThread != NULL);
		pSocketThread->ConcludeThread(INFINITE);
		SafeDelete(pSocketThread);
	}
	m_SocketRSThreadArray.RemoveAll();

	//�ر���ɶ˿�
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	//�ر�����
	CServerSocketItem * pServerSocketItem = NULL;
	for (i = 0; i < m_ActiveSocketItem.GetCount(); i++)
	{
		pServerSocketItem = m_ActiveSocketItem[i];
		ASSERT(pServerSocketItem != NULL);
		pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
		pServerSocketItem->ResetSocketData();
	}
	m_FreeSocketItem.Append(m_ActiveSocketItem);
	m_ActiveSocketItem.RemoveAll();

	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	return true;
}

//Ӧ����Ϣ
bool CTCPNetworkEngine::OnSocketAcceptEvent(CServerSocketItem * pServerSocketItem)
{
	//Ч������
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	//Ͷ����Ϣ
	if (m_bService == false) return false;
	m_QueueServiceEvent.PostNetworkAcceptEvent(pServerSocketItem->GetSocketID(), pServerSocketItem->GetClientAddr());

	return true;
}

//�����ȡ��Ϣ
bool CTCPNetworkEngine::OnSocketReadEvent(TCP_Command Command, void * pBuffer, WORD wDataSize, CServerSocketItem * pServerSocketItem)
{
	//Ч������
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	//Ч��״̬
	if (m_bService == false) return false;
	m_QueueServiceEvent.PostNetworkReadEvent(pServerSocketItem->GetSocketID(), Command, pBuffer, wDataSize);

	return true;
}

//����ر���Ϣ
bool CTCPNetworkEngine::OnSocketCloseEvent(CServerSocketItem * pServerSocketItem)
{
	//Ч�����
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	try
	{
		//Ч��״̬
		if (m_bService == false) return false;

		//����ʱ��
		WORD wIndex = pServerSocketItem->GetIndex();
		WORD wRountID = pServerSocketItem->GetRountID();
		DWORD dwClientAddr = pServerSocketItem->GetClientAddr();
		DWORD dwConnectTime = pServerSocketItem->GetConnectTime();
		//////////////////////////////////////////////////////////////////////////,����Ҫ����
		m_QueueServiceEvent.PostNetworkShutEvent(pServerSocketItem->GetSocketID() , dwClientAddr, dwConnectTime);

		//�ͷ�����
		FreeSocketItem(pServerSocketItem);
	}
	catch (...) {}

	return true;
}

//֪ͨ�ص����������Ͷ����̵߳��ã�
void CTCPNetworkEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	switch (wIdentifier)
	{
		case QUEUE_SEND_REQUEST:		//��������
		{
			//Ч������
			tagSendDataRequest * pSendDataRequest = (tagSendDataRequest *)pBuffer;
			ASSERT(wDataSize >= (sizeof(tagSendDataRequest) - sizeof(pSendDataRequest->cbSendBuf)));
			ASSERT(wDataSize == (pSendDataRequest->wDataSize + sizeof(tagSendDataRequest) - sizeof(pSendDataRequest->cbSendBuf)));

			//Ⱥ������
			if (pSendDataRequest->dwSocketID == 0)
			{
				//��ȡ���
				{
					CYSDataLocker lcok(m_CriticalSection);
					m_TempSocketItem.RemoveAll();
					m_TempSocketItem.Copy(m_ActiveSocketItem);
				}

				//ѭ����������
				CServerSocketItem * pServerSocketItem = NULL;
				for (INT_PTR i = 0; i < m_TempSocketItem.GetCount(); i++)
				{
					pServerSocketItem = m_TempSocketItem[i];
					ASSERT(pServerSocketItem != NULL);
					CYSDataLocker lock(pServerSocketItem->GetSignedLock());
					if (pServerSocketItem->IsAllowBatch())//��һ�β�������
					{
						pServerSocketItem->SendData(pSendDataRequest->cbSendBuf, pSendDataRequest->wDataSize, pSendDataRequest->wMainCmdID,
						                            pSendDataRequest->wSubCmdID, pServerSocketItem->GetRountID());
					}
				}
			}
			else
			{
				//�����
				CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pSendDataRequest->dwSocketID));
				CYSDataLocker lock(pServerSocketItem->GetSignedLock());
				pServerSocketItem->SendData(pSendDataRequest->cbSendBuf, pSendDataRequest->wDataSize, pSendDataRequest->wMainCmdID,
				                            pSendDataRequest->wSubCmdID, HIWORD(pSendDataRequest->dwSocketID));
			}

			break;
		}
		case QUEUE_SAFE_CLOSE:		//��ȫ�ر�
		{
			//Ч������
			ASSERT(wDataSize == sizeof(tagSafeCloseSocket));
			tagSafeCloseSocket * pSafeCloseSocket = (tagSafeCloseSocket *)pBuffer;

			//��ȫ�ر�
			try
			{
				CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pSafeCloseSocket->dwSocketID));
				CYSDataLocker lock(pServerSocketItem->GetSignedLock());
				pServerSocketItem->ShutDownSocket(HIWORD(pSafeCloseSocket->dwSocketID));
			}
			catch (...)
			{
			}
			break;
		}
		case QUEUE_ALLOW_BATCH:		//����Ⱥ��
		{
			//Ч������
			ASSERT(wDataSize == sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend = (tagAllowBatchSend *)pBuffer;

			//����Ⱥ��
			CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pAllowBatchSend->dwSocketID));
			CYSDataLocker lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->AllowBatchSend(HIWORD(pAllowBatchSend->dwSocketID), pAllowBatchSend->cbAllow ? true : false);

			break;
		}
		case QUEUE_DETECT_SOCKET:	//�������
		{
			//��ȡ���
			{
				CYSDataLocker lcok(m_CriticalSection);
				m_TempSocketItem.RemoveAll();
				m_TempSocketItem.Copy(m_ActiveSocketItem);
			}

			//��������
			TCP_KN_DetectSocket DetectSocket;
			ZeroMemory(&DetectSocket, sizeof(DetectSocket));

			//��������
			WORD wRountID = 0;
			DWORD dwNowTickCount = GetTickCount();
			DWORD dwBreakTickCount = __max(dwNowTickCount - m_dwLastDetect, TIME_BREAK_READY);

			//���ñ���
			m_dwLastDetect = GetTickCount();

			//�������
			for (INT_PTR i = 0; i < m_TempSocketItem.GetCount(); i++)
			{
				//��������
				CServerSocketItem * pServerSocketItem = m_TempSocketItem[i];
				DWORD dwRecvTickCount = pServerSocketItem->GetRecvTickCount();
				CYSDataLocker lock(pServerSocketItem->GetSignedLock());

				//������
				if (dwRecvTickCount >= dwNowTickCount) continue;

				//�������
				if (pServerSocketItem->IsReadySend() == true)
				{
					if ((dwNowTickCount - dwRecvTickCount) > dwBreakTickCount)
					{
						pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
						continue;
					}
				}
				else
				{
					if ((dwNowTickCount - dwRecvTickCount) > TIME_BREAK_CONNECT)
					{
						pServerSocketItem->CloseSocket(pServerSocketItem->GetRountID());
						continue;
					}
				}

				//��������
				if (pServerSocketItem->IsReadySend() == true)
				{
					wRountID = pServerSocketItem->GetRountID();
					DetectSocket.dwSendTickCount = GetTickCount();
					pServerSocketItem->SendData(&DetectSocket, sizeof(DetectSocket), MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET, wRountID);
				}
			}

			break;
		}
		default:
		{
			ASSERT(FALSE);
		}
	}

	return;
}

//��ȡ���ж���
CServerSocketItem * CTCPNetworkEngine::ActiveSocketItem()
{
	CYSDataLocker lock(m_CriticalSection);

	//��ȡ���ж���
	CServerSocketItem * pServerSocketItem = NULL;
	if (m_FreeSocketItem.GetCount() > 0)
	{
		INT_PTR nItemPostion = m_FreeSocketItem.GetCount() - 1;
		pServerSocketItem = m_FreeSocketItem[nItemPostion];
		ASSERT(pServerSocketItem != NULL);
		m_FreeSocketItem.RemoveAt(nItemPostion);
		m_ActiveSocketItem.Add(pServerSocketItem);
	}

	//�����¶���
	if (pServerSocketItem == NULL)
	{
		WORD wStorageCount = (WORD)m_StorageSocketItem.GetCount();
		if (wStorageCount < m_wMaxSocketItem)
		{
			try
			{
				pServerSocketItem = new CServerSocketItem(wStorageCount, this);
				if (pServerSocketItem == NULL) return NULL;
				m_StorageSocketItem.Add(pServerSocketItem);
				m_ActiveSocketItem.Add(pServerSocketItem);
			}
			catch (...)
			{
				return NULL;
			}
		}
	}

	return pServerSocketItem;
}

//��ȡ���Ӷ���
CServerSocketItem * CTCPNetworkEngine::EnumSocketItem(WORD wIndex)
{
	CYSDataLocker lock(m_CriticalSection);
	if (wIndex < m_StorageSocketItem.GetCount())
	{
		CServerSocketItem * pServerSocketItem = m_StorageSocketItem[wIndex];
		ASSERT(pServerSocketItem != NULL);
		return pServerSocketItem;
	}
	return NULL;
}

//�ͷ����Ӷ���
bool CTCPNetworkEngine::FreeSocketItem(CServerSocketItem * pServerSocketItem)
{
	//Ч�����
	ASSERT(pServerSocketItem != NULL);

	//�ͷŶ���
	CYSDataLocker lock(m_CriticalSection);
	INT_PTR nActiveCount = m_ActiveSocketItem.GetCount();
	for (int i = 0; i < nActiveCount; i++)
	{
		if (pServerSocketItem == m_ActiveSocketItem[i])
		{
			m_ActiveSocketItem.RemoveAt(i);
			m_FreeSocketItem.Add(pServerSocketItem);
			return true;
		}
	}

	//�ͷ�ʧ��
	ASSERT(FALSE);
	return false;
}

//�������
bool CTCPNetworkEngine::DetectSocket()
{
	return m_SendQueueService.AddToQueue(QUEUE_DETECT_SOCKET, NULL, 0);
}


//���ͺ���
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч��״̬
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//��������
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = dwSocketID;
	SendRequest.wDataSize = 0;

	//���뷢�Ͷ���
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf);
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//���ͺ���
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//Ч��״̬
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//Ч������
	ASSERT((wDataSize + sizeof(TCP_Head)) <= SOCKET_TCP_BUFFER);
	if ((wDataSize + sizeof(TCP_Head)) > SOCKET_TCP_BUFFER) return false;

	//��������
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = dwSocketID;
	SendRequest.wDataSize = wDataSize;
	if (wDataSize > 0)
	{
		ASSERT(pData != NULL);
		CopyMemory(SendRequest.cbSendBuf, pData, wDataSize);
	}

	//���뷢�Ͷ���
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf) + wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//��������
bool CTCPNetworkEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize, BYTE cbBatchMask)
{
	//Ч��״̬
	if (m_bService == false) return false;

	//Ч������
	ASSERT((wDataSize + sizeof(TCP_Head)) <= SOCKET_TCP_BUFFER);
	if ((wDataSize + sizeof(TCP_Head)) > SOCKET_TCP_BUFFER) return false;

	//��������
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = 0;
	SendRequest.wDataSize = wDataSize;
	if (wDataSize > 0)
	{
		ASSERT(pData != NULL);
		CopyMemory(SendRequest.cbSendBuf, pData, wDataSize);
	}

	//���뷢�Ͷ���
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf) + wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//�ر�����
bool CTCPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
	CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(dwSocketID));
	if (pServerSocketItem == NULL) return false;
	CYSDataLocker lock(pServerSocketItem->GetSignedLock());
	return pServerSocketItem->CloseSocket(HIWORD(dwSocketID));
}

//���ùر�
bool CTCPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
	tagSafeCloseSocket SafeCloseSocket;
	SafeCloseSocket.dwSocketID = dwSocketID;
	return m_SendQueueService.AddToQueue(QUEUE_SAFE_CLOSE, &SafeCloseSocket, sizeof(SafeCloseSocket));
}

//����Ⱥ��
bool CTCPNetworkEngine::AllowBatchSend(DWORD dwSocketID, bool bAllow, BYTE cbBatchMask)
{
	tagAllowBatchSend AllowBatchSendNode;
	AllowBatchSendNode.dwSocketID = dwSocketID;
	AllowBatchSendNode.cbAllow = bAllow;
	return m_SendQueueService.AddToQueue(QUEUE_ALLOW_BATCH, &AllowBatchSendNode, sizeof(tagAllowBatchSend));
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) void * CreateTCPNetworkEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CTCPNetworkEngine * pTCPSocketEngine = NULL;
	try
	{
		pTCPSocketEngine = new CTCPNetworkEngine();
		if (pTCPSocketEngine == NULL) throw TEXT("����ʧ��");
		void * pObject = pTCPSocketEngine->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pTCPSocketEngine);
	return NULL;
}



//////////////////////////////////////////////////////////////////////////

CWebSocket::CWebSocket()
{
	clear();
}

CWebSocket::~CWebSocket()
{

}


void CWebSocket::clear()
{
	m_cbWSStatus = eWSStatus_Invalid;
	memset(m_szHandShakeResponse, 0, sizeof(m_szHandShakeResponse));
	m_dwRecvSize = 0;
	memset(m_cbRecvBuf, 0, sizeof(m_cbRecvBuf));
}

bool CWebSocket::ParseWebSocketHandShake(char *szBuffer, int nBufSize)
{
	static const char* endStr = "\r\n\r\n";
	static const char* wsurl = "GET /ws ";
	// send handshake;
	static char* pHandshake = "HTTP/1.1 101 Web Socket Protocol Handshake\r\n"
		"Upgrade: WebSocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Accept: %s\r\n\r\n";
	//�����׸�����ʱ��,��ȥȷ���Ƿ���WebSocket;
	if (nBufSize >= 8)
	{	//"GET /ws "
		if (strncmp(szBuffer, wsurl, strlen(wsurl)) == 0)
		{
			ws_req_t reqParam;
			if (ParseWebSocketHandShakeRequest(szBuffer, &reqParam) == 0 && strncmp(szBuffer + nBufSize - 4, endStr, strlen(endStr)) == 0)
			{
				sprintf_s(m_szHandShakeResponse, sizeof(m_szHandShakeResponse), pHandshake, GenerateWebSocketKey(reqParam.sec_websocket_key).c_str());
				m_cbWSStatus = eWSStatus_Connect;
				return true;
			}
		}
	}

	return false;
}

int CWebSocket::ParseWebSocketHandShakeRequest(const char *s_req, ws_req_t *ws_req)
{
	if (!s_req || !ws_req) {
		return -1;
	}

	char tmp[4096] = { 0 };

	int len = strlen(s_req);
	memcpy(tmp, s_req, len);

	char *delim = "\r\n";
	char *p = NULL, *q = NULL;

	p = strtok(tmp, delim);
	if (p) {
		//printf("%s\n", p);
		ws_req->req = p;
		while (p = strtok(NULL, delim)) {
			//printf("%s\n", p);
			if ((q = strstr(p, ":")) != NULL) {
				*q = '\0';
				if (strcmp(p, "Connection") == 0) {
					while (*++q == ' ');
					ws_req->connection = q;
				}
				if (strcmp(p, "Upgrade") == 0) {
					while (*++q == ' ');
					ws_req->upgrade = q;
				}
				if (strcmp(p, "Host") == 0) {
					while (*++q == ' ');
					ws_req->host = q + 1;
				}
				if (strcmp(p, "Origin") == 0) {
					while (*++q == ' ');
					ws_req->origin = q;
				}
				if (strcmp(p, "Cookie") == 0) {
					while (*++q == ' ');
					ws_req->cookie = q;
				}
				if (strcmp(p, "Sec-WebSocket-Key") == 0) {
					while (*++q == ' ');
					ws_req->sec_websocket_key = q;
				}
				if (strcmp(p, "Sec-WebSocket-Version") == 0) {
					while (*++q == ' ');
					ws_req->sec_websocket_version = q;
				}
			}
		}
	}
	return 0;
}

string CWebSocket::GenerateWebSocketKey(const string &key)
{
	//sha-1;
	string tmp = key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	unsigned int msg_digest[5] = { 0 };

	SHA1 sha1;
	sha1.update(tmp);
	sha1.final(msg_digest);

	for (int i = 0; i < 5; i++) {
		msg_digest[i] = htonl(msg_digest[i]);
	}

	//base64 encode
	return base64_encode((const unsigned char*)msg_digest, 20);
}

int CWebSocket::DecodeFrame(char* outMessage, DWORD nCapcity, DWORD& recvSize)
{
	int nIndex = 0;
	int nRet = WS_OPENING_FRAME;

	DWORD nTempCapcity = nCapcity;

	const char* pFrameData = (char*)m_cbRecvBuf;

	do
	{
		CWebSocketFrameHeader frameHeader;
		memset(&frameHeader, 0, sizeof(frameHeader));

		int nCanReadOffset = 2;
		if (m_dwRecvSize < nCanReadOffset)
		{	//����û�н���ȫ�����м����;
			nRet = WS_MIDDLE_FRAME;
			break;
		}

		frameHeader.cbFin = (unsigned char)pFrameData[0] >> 7;
		frameHeader.cbopcode = pFrameData[0] & 0x0f;
		frameHeader.cbMask = (unsigned char)pFrameData[1] >> 7;
		frameHeader.cbPayloadlen = pFrameData[1] & 0x7f;

		if (frameHeader.cbopcode == WS_CLOSING_FRAME)
		{	//���ӹر���Ϣ;
			nRet = WS_CLOSING_FRAME;
			break;
		}

		if (m_dwRecvSize < (frameHeader.cbPayloadlen + nCanReadOffset))
		{	//����û�н���ȫ�����м����;
			nRet = WS_MIDDLE_FRAME;
			break;
		}

		if (frameHeader.cbPayloadlen == 126)
		{
			unsigned short length = 0;
			memcpy(&length, pFrameData + nCanReadOffset, 2);
			nCanReadOffset += 2;

			frameHeader.payloadLen = ntohs(length);
		}
		else if (frameHeader.cbPayloadlen == 127)
		{
			unsigned long long length = 0;
			memcpy(&length, pFrameData + nCanReadOffset, 8);
			nCanReadOffset += 8;

			frameHeader.payloadLen = length;
		}
		else
		{
			frameHeader.payloadLen = frameHeader.cbPayloadlen;
		}

		if (m_dwRecvSize < (frameHeader.payloadLen + nCanReadOffset))
		{	//����û�н���ȫ�����м����;
			nRet = WS_MIDDLE_FRAME;
			break;
		}

		if (nCapcity < frameHeader.payloadLen)
		{	//���ݷŲ����ˣ���������ȥ;
			nRet = WS_MIDDLE_FRAME;
			break;
		}

		if (frameHeader.cbMask == 1)
		{
			for (int i = 0; i < 4; ++i)
			{
				frameHeader.cbMaskingKey[i] = pFrameData[nCanReadOffset + i];
			}
			nCanReadOffset += 4;
			for (SCORE i = 0; i < frameHeader.payloadLen; ++i)
			{
				int j = i % 4;
				outMessage[nIndex + i] = pFrameData[nCanReadOffset + i] ^ frameHeader.cbMaskingKey[j];
			}
		}
		else
		{
			memcpy(outMessage + nIndex, pFrameData + nCanReadOffset, frameHeader.payloadLen);
		}



		nCanReadOffset += frameHeader.payloadLen;
		nIndex += frameHeader.payloadLen;
		recvSize += frameHeader.payloadLen;
		nCapcity -= frameHeader.payloadLen;

		m_dwRecvSize -= nCanReadOffset;
		if (m_dwRecvSize > 0)
		{
			memmove(m_cbRecvBuf, m_cbRecvBuf + nCanReadOffset, m_dwRecvSize);
			pFrameData = (char*)m_cbRecvBuf;
		}

	} while (m_dwRecvSize > 0);

	if (nTempCapcity != nCapcity)
	{
		nRet = WS_OPENING_FRAME;
	}

	return nRet;
}

int CWebSocket::EncodeFrame(char* inMessage, DWORD nInSize, char* outMessage, DWORD nOutCapcity, DWORD& nOutSize)
{
	int nRet = WS_EMPTY_FRAME;

	if (nInSize > 32767)
	{	//�ݲ�֧����ô��������;
		return WS_ERROR_FRAME;
	}

	if (nOutCapcity < (nInSize + 4))
	{	//outMessage�洢�ռ䲻��;
		return WS_ERROR_FRAME;
	}

	nOutSize = 0;

	outMessage[0] = (0x80 | WS_BINARY_FRAME);

	if (nInSize < 126)
	{
		outMessage[1] = nInSize;
		nOutSize += 2;
	}
	else
	{
		outMessage[1] = 126;
		unsigned short len = htons(nInSize);
		memcpy(&outMessage[2], &len, 2);

		nOutSize += 4;
	}

	memcpy(outMessage + nOutSize, inMessage, nInSize);

	nOutSize += nInSize;

	return nRet;
}
