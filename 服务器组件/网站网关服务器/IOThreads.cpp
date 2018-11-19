#include "StdAfx.h"
#include "IOCPServer.h"
#include "IOThreads.h"

//���캯��
CIOWorkerThread::CIOWorkerThread(void)
{
	m_hCompletionPort = NULL;
	m_pIOCPServer = NULL;
}

//��������
CIOWorkerThread::~CIOWorkerThread(void)
{
}

//���ú���
bool CIOWorkerThread::InitThread(HANDLE hCompletionPort, CIOCPServer* pIOCPServer)
{
	ASSERT(hCompletionPort != NULL);
	m_hCompletionPort = hCompletionPort;
	m_pIOCPServer = pIOCPServer;
	return true;
}

//���к���
bool CIOWorkerThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort != NULL);

	SOCKET socket = NULL;
	DWORD   BytesTransferred = 0;
	stOverlappedBase* pSOverlapped = NULL;

	BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &BytesTransferred, (PULONG_PTR)&socket, (LPOVERLAPPED*)&pSOverlapped, INFINITE);
	if ( !bSuccess ) 
	{   
		DWORD dwLastError = GetLastError();
		// 64����ű�ʾ"ָ�������������ٿ���"���ͻ����쳣�˳��������������
		if ( dwLastError != 64 ) {
			TRACE1("GetQueuedCompletionStatus   ���������´��� %d\n",   GetLastError());
		}
	}

	// HasOverlappedIoCompleted
	if (pSOverlapped==NULL&&BytesTransferred==0) {
		TRACE("�˳�����\n");
		return false;
	}

	if (BytesTransferred == 0) {
		TRACE("���ܵ�0���ֽڵ����ݣ��ͻ��˿��ܶϿ�������\n");
		WORD nClient = pSOverlapped->nClient;
		delete pSOverlapped;
		pSOverlapped=NULL;
		m_pIOCPServer->Disconnect(nClient);
		return true;
	}

	if (pSOverlapped == NULL) {
		TRACE("pSOverlapped == NULL\n");
		return true;
	}

	if (pSOverlapped->IoMode == IoSend) {
		// �����¼�
		stSendOverlapped* pSendOverlapped = (stSendOverlapped*)pSOverlapped;
		WORD nClient = pSendOverlapped->nClient;
		if (pSendOverlapped->dwSentBytes + BytesTransferred < pSendOverlapped->dwTotalBytes) {
			TRACE("========���ַ������ݲ����������������========\n");
			pSendOverlapped->dwSentBytes += BytesTransferred;
			pSendOverlapped->WsaBuf.buf = pSendOverlapped->pBuffer + pSendOverlapped->dwSentBytes;
			pSendOverlapped->WsaBuf.len = pSendOverlapped->dwTotalBytes - pSendOverlapped->dwSentBytes;

			// �����¼�
			DWORD SendBytes = 0;
			if ( WSASend(socket, &pSendOverlapped->WsaBuf, 1, &SendBytes, 0,
				(LPOVERLAPPED)pSendOverlapped, NULL)   ==   SOCKET_ERROR )
			{   
				delete[] pSendOverlapped->pBuffer;
				pSendOverlapped->pBuffer=NULL;
				delete pSendOverlapped;
				pSendOverlapped=NULL;

				int iError = WSAGetLastError();
				if ( iError != ERROR_IO_PENDING )
				{   
					TRACE1("IOCPServer::ServerWorkerThread - WSASend() ���������´���   %d\n",   iError);
					// �쳣�����¼�
					m_pIOCPServer->OnError(nClient, iError);
					// �ر�SOCKET
					m_pIOCPServer->Disconnect(nClient);
					return 0;
				}   
			}   

		}
		else
		{
			m_pIOCPServer->OnSend(nClient, pSendOverlapped->pBuffer, pSendOverlapped->dwTotalBytes);

			delete[] pSendOverlapped->pBuffer;
			pSendOverlapped->pBuffer=NULL;
			delete pSendOverlapped;
			pSendOverlapped=NULL;
		}

	}
	else if (pSOverlapped->IoMode == IoRecv)
	{
		// �����¼�
		stRecvOverlapped* pRecvOverlapped = (stRecvOverlapped*)pSOverlapped;
		WORD nClient = pRecvOverlapped->nClient;
		char* pBuf = pRecvOverlapped->buffer;

		// ���������¼�
		m_pIOCPServer->OnReceive(nClient, pBuf, BytesTransferred);

		// ������յ������ݰ�
		CTCPSocket* tcpSocket = m_pIOCPServer->GetTCPSocket(nClient);
		ASSERT(tcpSocket!=NULL);

		bool success = false;
		try
		{
			success = tcpSocket->processRecvData(pBuf, BytesTransferred);
		}
		catch(...)
		{
			ASSERT(0);
			TRACE("�������ݴ����쳣!");
		}
		delete pRecvOverlapped;
		pRecvOverlapped=NULL;

		// ���ͽ�������
		if (success) m_pIOCPServer->PostRecv(nClient, socket);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CIOAcceptThread::CIOAcceptThread(void)
{
	m_hCompletionPort = NULL;
	m_pIOCPServer = NULL;
	m_hListenSocket = INVALID_SOCKET;
}

//��������
CIOAcceptThread::~CIOAcceptThread(void)
{
}

//���ú���
bool CIOAcceptThread::InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CIOCPServer* pIOCPServer)
{
	ASSERT(hCompletionPort != NULL);
	ASSERT(pIOCPServer != NULL);
	ASSERT(hListenSocket != INVALID_SOCKET);
	m_hListenSocket = hListenSocket;
	m_hCompletionPort = hCompletionPort;
	m_pIOCPServer = pIOCPServer;
	return true;
}

//���к���
bool CIOAcceptThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort != NULL);
	ASSERT(m_pIOCPServer != NULL);

	// ������������
	SOCKADDR_IN	saClient;
	int iClientSize = sizeof(saClient);
	SOCKET AcceptSocket = WSAAccept(m_hListenSocket, (SOCKADDR *)&saClient, &iClientSize, NULL, NULL);
	if ( AcceptSocket == SOCKET_ERROR )
	{
		// �쳣�����¼�
		m_pIOCPServer->OnError(INVALID_ID, WSAGetLastError());
		return false;
	}

	CTCPSocket* tcpSocket = m_pIOCPServer->CreateTCPSocket(AcceptSocket, saClient.sin_addr.S_un.S_addr);
	WORD nClientId = m_pIOCPServer->InserTCPSocket(tcpSocket);
	if (nClientId == INVALID_ID) {
		TRACE("����TCPSocket����ʧ�ܣ���ȷ�϶����Ƿ�Ϊ��\n");
		// �ر�Socket
		if (closesocket(AcceptSocket) == SOCKET_ERROR) {   
			ErrorExit(_T("closesocket"));
			return false;
		}
		return true;
	}

	// �����¼�
	m_pIOCPServer->OnAccept(nClientId);

	// ����ǰ���ӽ����Ŀͻ���SOCKET����IO��ɶ˿�
	HANDLE handle = CreateIoCompletionPort((HANDLE)AcceptSocket, m_hCompletionPort, AcceptSocket, 0);
	if ( handle == NULL )
	{
		ErrorExit(_T("CreateIoCompletionPort"));
		return false;
	}

	// ����һ����������
	m_pIOCPServer->PostRecv(nClientId, AcceptSocket);

	return true;
}