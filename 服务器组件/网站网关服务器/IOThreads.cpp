#include "StdAfx.h"
#include "IOCPServer.h"
#include "IOThreads.h"

//构造函数
CIOWorkerThread::CIOWorkerThread(void)
{
	m_hCompletionPort = NULL;
	m_pIOCPServer = NULL;
}

//析构函数
CIOWorkerThread::~CIOWorkerThread(void)
{
}

//配置函数
bool CIOWorkerThread::InitThread(HANDLE hCompletionPort, CIOCPServer* pIOCPServer)
{
	ASSERT(hCompletionPort != NULL);
	m_hCompletionPort = hCompletionPort;
	m_pIOCPServer = pIOCPServer;
	return true;
}

//运行函数
bool CIOWorkerThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort != NULL);

	SOCKET socket = NULL;
	DWORD   BytesTransferred = 0;
	stOverlappedBase* pSOverlapped = NULL;

	BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &BytesTransferred, (PULONG_PTR)&socket, (LPOVERLAPPED*)&pSOverlapped, INFINITE);
	if ( !bSuccess ) 
	{   
		DWORD dwLastError = GetLastError();
		// 64错误号表示"指定的网络名不再可用"，客户端异常退出会产生这个错误号
		if ( dwLastError != 64 ) {
			TRACE1("GetQueuedCompletionStatus   发生了如下错误： %d\n",   GetLastError());
		}
	}

	// HasOverlappedIoCompleted
	if (pSOverlapped==NULL&&BytesTransferred==0) {
		TRACE("退出程序\n");
		return false;
	}

	if (BytesTransferred == 0) {
		TRACE("接受到0个字节的数据！客户端可能断开了连接\n");
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
		// 发送事件
		stSendOverlapped* pSendOverlapped = (stSendOverlapped*)pSOverlapped;
		WORD nClient = pSendOverlapped->nClient;
		if (pSendOverlapped->dwSentBytes + BytesTransferred < pSendOverlapped->dwTotalBytes) {
			TRACE("========出现发送数据不完整的情况！！！========\n");
			pSendOverlapped->dwSentBytes += BytesTransferred;
			pSendOverlapped->WsaBuf.buf = pSendOverlapped->pBuffer + pSendOverlapped->dwSentBytes;
			pSendOverlapped->WsaBuf.len = pSendOverlapped->dwTotalBytes - pSendOverlapped->dwSentBytes;

			// 发送事件
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
					TRACE1("IOCPServer::ServerWorkerThread - WSASend() 发生了如下错误：   %d\n",   iError);
					// 异常错误事件
					m_pIOCPServer->OnError(nClient, iError);
					// 关闭SOCKET
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
		// 接收事件
		stRecvOverlapped* pRecvOverlapped = (stRecvOverlapped*)pSOverlapped;
		WORD nClient = pRecvOverlapped->nClient;
		char* pBuf = pRecvOverlapped->buffer;

		// 触发接收事件
		m_pIOCPServer->OnReceive(nClient, pBuf, BytesTransferred);

		// 处理接收到的数据包
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
			TRACE("接收数据处理异常!");
		}
		delete pRecvOverlapped;
		pRecvOverlapped=NULL;

		// 发送接收请求
		if (success) m_pIOCPServer->PostRecv(nClient, socket);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CIOAcceptThread::CIOAcceptThread(void)
{
	m_hCompletionPort = NULL;
	m_pIOCPServer = NULL;
	m_hListenSocket = INVALID_SOCKET;
}

//析构函数
CIOAcceptThread::~CIOAcceptThread(void)
{
}

//配置函数
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

//运行函数
bool CIOAcceptThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort != NULL);
	ASSERT(m_pIOCPServer != NULL);

	// 处理连接请求
	SOCKADDR_IN	saClient;
	int iClientSize = sizeof(saClient);
	SOCKET AcceptSocket = WSAAccept(m_hListenSocket, (SOCKADDR *)&saClient, &iClientSize, NULL, NULL);
	if ( AcceptSocket == SOCKET_ERROR )
	{
		// 异常错误事件
		m_pIOCPServer->OnError(INVALID_ID, WSAGetLastError());
		return false;
	}

	CTCPSocket* tcpSocket = m_pIOCPServer->CreateTCPSocket(AcceptSocket, saClient.sin_addr.S_un.S_addr);
	WORD nClientId = m_pIOCPServer->InserTCPSocket(tcpSocket);
	if (nClientId == INVALID_ID) {
		TRACE("插入TCPSocket对象失败，请确认对象是否为空\n");
		// 关闭Socket
		if (closesocket(AcceptSocket) == SOCKET_ERROR) {   
			ErrorExit(_T("closesocket"));
			return false;
		}
		return true;
	}

	// 连接事件
	m_pIOCPServer->OnAccept(nClientId);

	// 给当前连接进来的客户端SOCKET创建IO完成端口
	HANDLE handle = CreateIoCompletionPort((HANDLE)AcceptSocket, m_hCompletionPort, AcceptSocket, 0);
	if ( handle == NULL )
	{
		ErrorExit(_T("CreateIoCompletionPort"));
		return false;
	}

	// 发送一个接收请求
	m_pIOCPServer->PostRecv(nClientId, AcceptSocket);

	return true;
}