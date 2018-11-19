#include "StdAfx.h"
#include "IOCPServer.h"
#include <process.h>
#include <stdio.h>
#include <assert.h>

void ErrorExit(LPTSTR lpszFunction) 
{ 
	TCHAR szBuf[128]; 
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	_stprintf_s(szBuf, sizeof(szBuf), _T("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);

	TRACE(szBuf);
	//MessageBox(NULL, szBuf, "Error", MB_OK); 

	LocalFree(lpMsgBuf);
	//ExitProcess(dw); 
}

CIOCPServer::CIOCPServer(void) : m_idAllocator(4096)
{
	m_mapSocket.clear();
	m_hCompletionPort = NULL;
	m_hServerSocket = INVALID_SOCKET;
}

CIOCPServer::~CIOCPServer(void)
{
	Stop();
}

bool CIOCPServer::Start(WORD nMaxClient, WORD nPort)
{
	// 重新设置ID分配器的大小
	m_idAllocator.resetMaxID(nMaxClient);

	// 加载WS2_32.DLL
	WSADATA wsaData;
	ZeroMemory(&wsaData, sizeof(WSADATA));
	int Ret = 0;
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	if ( (Ret=WSAStartup(wVersionRequested, &wsaData)) != 0 )
	{
		ErrorExit(_T("WSAStartup"));
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
		printf("Socket版本不一致\n");
		return false;
	}


	// 获得本地系统的主机名和IP地址
	hostent* localHost;
	char* localIP;
	localHost = gethostbyname("");
	localIP = inet_ntoa (*(struct in_addr *)*localHost->h_addr_list);

	// 创建一个监听用的Socket 
	// 第3个参数IPPROTO_TCP
	m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_hServerSocket == INVALID_SOCKET)
	{
		ErrorExit(_T("WSASocket"));
		return false;   
	}

	// 绑定IP和端口
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	//SockAddr.sin_addr.s_addr = inet_addr(localIP);	// 只允许指定IP的客户端连接
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	SockAddr.sin_port = htons(nPort);
	if ( bind(m_hServerSocket, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR )
	{   
		ErrorExit(_T("bind"));
		return false;   
	}

	// 准备监听Socket连接
	if ( listen(m_hServerSocket, SOMAXCONN) == SOCKET_ERROR )
	{   
		ErrorExit(_T("listen"));
		return false;   
	}

	//---------------------------
	//		完成端口相关代码段
	//---------------------------

	// 创建IO完成端口
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	if (m_hCompletionPort == NULL)
	{
		ErrorExit(_T("CreateIoCompletionPort"));
		return false; 
	}

	bool bSuccess = false;
	// 根据当前系统的处理器数量，创建2倍的工作线程
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);
	for (UINT i=0; i<SystemInfo.dwNumberOfProcessors*2; i++)
	{
		CIOWorkerThread * pIOWorkerThread = new CIOWorkerThread();
		if (pIOWorkerThread == NULL)
		{
			ErrorExit(_T("网络引擎读写线程服务创建失败"));
			return false; 
		}
		bSuccess = pIOWorkerThread->InitThread(m_hCompletionPort, this);
		if ( !bSuccess )
		{
			ErrorExit(_T("网络引擎读写线程服务配置失败"));
			return false; 
		}
		m_IOWorkerThreadArray.Add(pIOWorkerThread);
	}

	// 创建Accept线程函数
	bSuccess = m_IOAcceptThread.InitThread(m_hCompletionPort, m_hServerSocket, this);
	if ( !bSuccess )
	{
		ErrorExit(_T("网络引擎网络监听线程服务配置"));
		return false;
	}

	//运行读写线程
	for (DWORD i = 0; i < SystemInfo.dwNumberOfProcessors*2; i++)
	{
		CIOWorkerThread* pIOWorkerThread = m_IOWorkerThreadArray[i];
		ASSERT(pIOWorkerThread != NULL);
		bSuccess = pIOWorkerThread->StartThread();
		if ( !bSuccess ) throw TEXT("网络引擎读写线程服务启动失败");
	}

	//运行应答线程
	bSuccess = m_IOAcceptThread.StartThread();
	if ( !bSuccess ) throw TEXT("网络引擎监听线程服务启动失败");

	return true;
}

//停止
void CIOCPServer::Stop()
{
	//终止应答线程
	if (m_hServerSocket != INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
	}
	m_IOAcceptThread.ConcludeThread(INFINITE);

	//释放读写线程
	INT_PTR nCount = m_IOWorkerThreadArray.GetCount(), i = 0;
	if (m_hCompletionPort != NULL)
	{
		for (i = 0; i < nCount; i++) PostQueuedCompletionStatus(m_hCompletionPort, 0, NULL, NULL);
	}
	for (i = 0; i < nCount; i++)
	{
		CIOWorkerThread * pSocketThread = m_IOWorkerThreadArray[i];
		ASSERT(pSocketThread != NULL);
		pSocketThread->ConcludeThread(INFINITE);
		SafeDelete(pSocketThread);
	}
	m_IOWorkerThreadArray.RemoveAll();

	//关闭完成端口
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	WSACleanup();
	if (!m_mapSocket.empty())
	{
		for (SocketMap::iterator it=m_mapSocket.begin(); it!=m_mapSocket.end(); it++)
		{
			CTCPSocket* pTCPSocket = it->second;
			delete pTCPSocket;
			pTCPSocket=NULL;
		}
		m_mapSocket.clear();
	}
}

bool CIOCPServer::Disconnect(WORD nClient)
{
	CTCPSocket* tcpSocket = this->GetTCPSocket(nClient);
	if (tcpSocket == NULL) {
		return false;
	}

	SOCKET socket = tcpSocket->getSocket();

	// 关闭SOCKET事件
	this->OnClose(nClient);
	this->RemoveTCPSocket(nClient);

	// 关闭Socket
	if (closesocket(socket) == SOCKET_ERROR) {   
		ErrorExit(_T("closesocket"));
		return false;
	}

	return true;
}

bool CIOCPServer::SendData(WORD nClient, void* data, DWORD len)
{
	if (data == NULL || len == 0)
		return false;

	CTCPSocket* tcpSocket = this->GetTCPSocket(nClient);
	if (tcpSocket == NULL)
		return false;

	// 创建一个发送完成事件
	stSendOverlapped* pSendOverlapped = new stSendOverlapped;
	pSendOverlapped->nClient = nClient;
	pSendOverlapped->pBuffer = new char[len];
	//memset(pSendOverlapped->pBuffer, 0, len);
	memcpy(pSendOverlapped->pBuffer, data, len);
	//StringCchCopy(pSendOverlapped->pBuffer, len, (char*)data);
	pSendOverlapped->WsaBuf.buf = pSendOverlapped->pBuffer;
	pSendOverlapped->WsaBuf.len = len;
	pSendOverlapped->dwSentBytes = 0;
	pSendOverlapped->dwTotalBytes = len;
	
	// 发送事件
	DWORD SendBytes = 0;
	if ( WSASend(tcpSocket->getSocket(), &pSendOverlapped->WsaBuf, 1, &SendBytes, 0,
		(LPOVERLAPPED)pSendOverlapped, NULL)   ==   SOCKET_ERROR )
	{   
		delete[] pSendOverlapped->pBuffer;
		pSendOverlapped->pBuffer=NULL;

		delete pSendOverlapped;
		pSendOverlapped=NULL;

		int iError = WSAGetLastError();
		if ( iError != ERROR_IO_PENDING )
		{   
			// WSAECONNRESET(10054) 远程客户端强制关闭连接
			TRACE1("CIOCPServer::SendData - WSASend() 发生了如下错误：   %d\n",   iError);
			// 异常错误事件
			OnError(nClient, iError);
			// 关闭SOCKET
			Disconnect(nClient);
			return false;
		}   
	}   

	return true;
}

CTCPSocket* CIOCPServer::onCreateTCPSocket(WORD nClient, WORD socket, DWORD dwClientAddr)
{
	CTCPSocket* tcpSocket = new CTCPSocket(nClient, socket, dwClientAddr, this);
	return tcpSocket;
}

CTCPSocket* CIOCPServer::CreateTCPSocket(SOCKET socket, DWORD dwClientAddr)
{
	WORD newID = INVALID_ID;
	CTCPSocket* tcpSocket = NULL;
	m_cs.Lock();
	newID = m_idAllocator.allocateID();

	tcpSocket = onCreateTCPSocket(newID, static_cast<WORD>(socket), dwClientAddr);
	if (tcpSocket == NULL) m_idAllocator.recycleID(newID);
	m_cs.Unlock();
	
	return tcpSocket;
}

WORD CIOCPServer::InserTCPSocket(CTCPSocket* tcpSocket)
{
	if (tcpSocket == NULL) return INVALID_ID;

	WORD nClient = tcpSocket->getID();
	m_cs.Lock();

	SocketMap::iterator it = m_mapSocket.find(nClient);
	if (it != m_mapSocket.end()) {
		CString strInfo;
		strInfo.Format(_T("CIOCPServer::InserTCPSocket 出现相同ID的情况 id=%d\n"), nClient);
		::OutputDebugString(strInfo);
	}

	m_mapSocket[nClient] = tcpSocket;
	m_cs.Unlock();
	return nClient;
}

void CIOCPServer::RemoveTCPSocket(WORD nClient)
{
	m_cs.Lock();
	SocketMap::iterator it = m_mapSocket.find(nClient);
	CTCPSocket* tcpSocket = NULL;
	if (it != m_mapSocket.end()) {
		tcpSocket = it->second;
		delete tcpSocket;
		tcpSocket = NULL;
		m_mapSocket.erase(it);
	}
	m_idAllocator.recycleID(nClient);
	m_cs.Unlock();
}

CTCPSocket* CIOCPServer::GetTCPSocket(WORD nClient)
{
	CTCPSocket* tcpSocket = NULL;
	m_cs.Lock();
	SocketMap::iterator it = m_mapSocket.find(nClient);
	if (it != m_mapSocket.end()) {
		tcpSocket = it->second;
	}
	m_cs.Unlock();
	return tcpSocket;
}

bool CIOCPServer::PostRecv(WORD nClient, SOCKET socket)
{
	DWORD RecvBytes = 0;
	DWORD Flags = 0; 
	stRecvOverlapped* pRecvOverlapped = new stRecvOverlapped;
	pRecvOverlapped->nClient = nClient;

	if ( WSARecv(socket, &pRecvOverlapped->WsaBuf, 1, &RecvBytes, &Flags,
		(LPOVERLAPPED)pRecvOverlapped, NULL)   ==   SOCKET_ERROR )   
	{   
		int iError = WSAGetLastError();
		if ( iError != ERROR_IO_PENDING )
		{   
			// 异常错误
			OnError(nClient, iError);
			Disconnect(nClient);
			delete pRecvOverlapped;
			pRecvOverlapped=NULL;
			return false;
		}   
	}  

	return true;
}