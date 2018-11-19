#include "StdAfx.h"
#include "TraceService.h" //
#include "TCPNetworkEngine.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

#define TIME_BREAK_READY			9000L								//中断时间
#define TIME_BREAK_CONNECT			4000L								//中断时间
#define TIME_DETECT_SOCKET			20000L								//监测时间

//////////////////////////////////////////////////////////////////////////

//动作定义
#define QUEUE_SEND_REQUEST			1									//发送标识
#define QUEUE_SAFE_CLOSE			2									//安全关闭
#define QUEUE_ALLOW_BATCH			3									//允许群发
#define QUEUE_DETECT_SOCKET			4									//检测连接

//发送请求结构
struct tagSendDataRequest
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	DWORD							dwSocketID;							//连接索引
	WORD							wDataSize;							//数据大小
	BYTE							cbSendBuf[SOCKET_TCP_BUFFER];		//发送缓冲
};

//设置群发
struct tagAllowBatchSend
{
	DWORD							dwSocketID;							//连接索引
	BYTE							cbAllow;							//允许标志
};

//安全关闭
struct tagSafeCloseSocket
{
	DWORD							dwSocketID;							//连接索引
};

//检测结构信息
struct TCP_KN_DetectSocket
{
	DWORD								dwSendTickCount;					//发送时间
	DWORD								dwRecvTickCount;					//接收时间
};

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLapped::COverLapped(enOperationType OperationType) : m_OperationType(OperationType)
{
	memset(&m_WSABuffer, 0, sizeof(m_WSABuffer));
	memset(&m_OverLapped, 0, sizeof(m_OverLapped));
}

//析构函数
COverLapped::~COverLapped()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
COverLappedSend::COverLappedSend() : COverLapped(OperationType_Send)
{
	m_WSABuffer.len = 0;
	m_WSABuffer.buf = (char *)m_cbBuffer;
}

//析构函数
COverLappedSend::~COverLappedSend()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
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

//析够函数
CServerSocketItem::~CServerSocketItem(void)
{
	//删除空闲重叠 IO
	INT_PTR iCount = m_OverLappedSendFree.GetCount();
	for (INT_PTR i = 0; i < iCount; i++) delete m_OverLappedSendFree[i];
	m_OverLappedSendFree.RemoveAll();

	//删除活动重叠 IO
	iCount = m_OverLappedSendActive.GetCount();
	for (INT_PTR i = 0; i < iCount; i++) delete m_OverLappedSendActive[i];
	m_OverLappedSendActive.RemoveAll();

	return;
}

//获取发送结构
COverLappedSend * CServerSocketItem::GetSendOverLapped()
{
	//寻找空闲结构
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

	//新建发送结构
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

//绑定对象
DWORD CServerSocketItem::Attach(SOCKET hSocket, DWORD dwClientAddr)
{
	//效验数据
	ASSERT(dwClientAddr != 0);
	ASSERT(m_bRecvIng == false);
	ASSERT(IsValidSocket() == false);
	ASSERT(hSocket != INVALID_SOCKET);

	//设置变量
	m_bNotify = false;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_hSocket = hSocket;
	m_dwClientAddr = dwClientAddr;
	m_dwRecvTickCount = GetTickCount();
	m_dwConnectTime = (DWORD)time(NULL);

	//发送通知
	m_pIServerSocketItemSink->OnSocketAcceptEvent(this);

	return GetSocketID();
}

//发送函数
bool CServerSocketItem::SendData(WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验状态
	if (m_bCloseIng == true) return false;
	if (m_wRountID != wRountID) return false;
	if (m_dwRecvPacketCount == 0) return false;
	if (IsValidSocket() == false) return false;

	//寻找发送结构
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
		//构造数据;
		TCP_Head * pHead = (TCP_Head *)pOverLappedSend->m_cbBuffer;
		pHead->CommandInfo.wMainCmdID = wMainCmdID;
		pHead->CommandInfo.wSubCmdID = wSubCmdID;
		WORD wSendSize = CYSEncrypt::EncryptBuffer(pOverLappedSend->m_cbBuffer, sizeof(TCP_Head), sizeof(pOverLappedSend->m_cbBuffer));
		pOverLappedSend->m_WSABuffer.len = wSendSize;
	}

	//设置变量
	m_dwSendPacketCount++;

	//发送数据
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

//发送函数
bool CServerSocketItem::SendData(void * pData, WORD wDataSize, WORD wMainCmdID, WORD wSubCmdID, WORD wRountID)
{
	//效验参数
	ASSERT(wDataSize <= SOCKET_TCP_BUFFER);

	//效验状态
	if (m_bCloseIng == true) return false;
	if (m_wRountID != wRountID) return false;
	if (m_dwRecvPacketCount == 0) return false;
	if (IsValidSocket() == false) return false;
	if (wDataSize > SOCKET_TCP_BUFFER) return false;

	//寻找发送结构
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
		//构造数据
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

	//设置变量
	m_dwSendPacketCount++;

	//发送数据
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

//投递接收
bool CServerSocketItem::RecvData()
{
	//效验变量
	ASSERT(m_bRecvIng == false);
	ASSERT(m_hSocket != INVALID_SOCKET);

	//判断关闭
	if (m_bCloseIng == true)
	{
		if (m_OverLappedSendActive.GetCount() == 0) CloseSocket(m_wRountID);
		return false;
	}

	//接收数据
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

//关闭连接
bool CServerSocketItem::CloseSocket(WORD wRountID)
{
	//状态判断
	if (m_wRountID != wRountID) return false;

	//关闭连接
	if (m_hSocket != INVALID_SOCKET)
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}

	//判断关闭
	if ((m_bRecvIng == false) && (m_OverLappedSendActive.GetCount() == 0)) OnCloseCompleted();

	return true;
}

//设置关闭
bool CServerSocketItem::ShutDownSocket(WORD wRountID)
{
	return true;

	//状态判断
	if (m_wRountID != wRountID) return false;
	if (m_hSocket == INVALID_SOCKET) return false;

	//设置变量
	if (m_bCloseIng == false)
	{
		m_bCloseIng = true;
		//if (m_OverLappedSendActive.GetCount() == 0) CloseSocket(wRountID);
	}

	return true;
}

//允许群发
bool CServerSocketItem::AllowBatchSend(WORD wRountID, bool bAllowBatch)
{
	//状态判断
	if (m_wRountID != wRountID) return false;
	if (m_hSocket == INVALID_SOCKET) return false;

	//设置变量
	m_bAllowBatch = bAllowBatch;

	return true;
}

//重置变量
void CServerSocketItem::ResetSocketData()
{
	//效验状态
	ASSERT(m_hSocket == INVALID_SOCKET);

	//重置数据;
	m_cbSocketType = eSocketType_TCP;

	m_wRountID++;
	m_dwRecvSize = 0;
	m_dwClientAddr = 0;
	m_dwConnectTime = 0;
	m_dwSendTickCount = 0;
	m_dwRecvTickCount = 0;
	m_dwSendPacketCount = 0;
	m_dwRecvPacketCount = 0;

	//状态变量
	m_bNotify = true;
	m_bRecvIng = false;
	m_bCloseIng = false;
	m_bAllowBatch = true;
	m_OverLappedSendFree.Append(m_OverLappedSendActive);
	m_OverLappedSendActive.RemoveAll();

	m_websocket.clear();

	return;
}

//发送完成函数
bool CServerSocketItem::OnSendCompleted(COverLappedSend * pOverLappedSend, DWORD dwThancferred)
{
	//效验变量
	ASSERT(pOverLappedSend != NULL);
	ASSERT(m_OverLappedSendActive.GetCount() > 0);
	ASSERT(pOverLappedSend == m_OverLappedSendActive[0]);

	//释放发送结构
	m_OverLappedSendActive.RemoveAt(0);
	m_OverLappedSendFree.Add(pOverLappedSend);

	//设置变量
	if (dwThancferred != 0) m_dwSendTickCount = GetTickCount();

	//判断关闭
	if (m_hSocket == INVALID_SOCKET)
	{
		m_OverLappedSendFree.Append(m_OverLappedSendActive);
		m_OverLappedSendActive.RemoveAll();
		CloseSocket(m_wRountID);
		return true;
	}

	//继续发送数据
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

	//判断关闭
	if (m_bCloseIng == true)
		CloseSocket(m_wRountID);

	return true;
}

//接收完成函数
bool CServerSocketItem::OnRecvCompleted(COverLappedRecv * pOverLappedRecv, DWORD dwThancferred)
{
	//效验数据
	ASSERT(m_bRecvIng == true);

	//设置变量
	m_bRecvIng = false;
	m_dwRecvTickCount = GetTickCount();

	//判断关闭
	if (m_hSocket == INVALID_SOCKET)
	{
		CloseSocket(m_wRountID);
		return true;
	}

	if (IsWebSocketType())
	{
		//接收数据;
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
		//接收数据;
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

		//接收完成;
		m_dwRecvSize += iRetCode;
	}

	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	TCP_Head * pHead = (TCP_Head *)m_cbRecvBuf;

	//处理数据
	try
	{
		while (m_dwRecvSize >= sizeof(TCP_Head))
		{
			//效验数据
			WORD wPacketSize = pHead->TCPInfo.wPacketSize;
			if (wPacketSize > SOCKET_TCP_BUFFER) throw TEXT("数据包超长");
			if (wPacketSize < sizeof(TCP_Head)) throw TEXT("数据包非法");
			//if (pHead->TCPInfo.cbVersion != SOCKET_TCP_VER) throw TEXT("数据包版本错误");
			if (pHead->TCPInfo.cbDataKind != SOCKET_TCP_VER) throw TEXT("数据包版本错误");
			if (m_dwRecvSize < wPacketSize) break;

			//提取数据
			CopyMemory(cbBuffer, m_cbRecvBuf, wPacketSize);

			WORD wRealySize = wPacketSize;
			if (!IsWebSocketType())
			{
				wRealySize = CYSEncrypt::CrevasseBuffer(cbBuffer, wPacketSize);
			}

			ASSERT(wRealySize >= sizeof(TCP_Head));
			m_dwRecvPacketCount++;

			//解释数据
			WORD wDataSize = wRealySize - sizeof(TCP_Head);
			void * pDataBuffer = cbBuffer + sizeof(TCP_Head);
			TCP_Command Command = ((TCP_Head *)cbBuffer)->CommandInfo;

			//内核命令
			if (Command.wMainCmdID == MDM_KN_COMMAND)
			{
				switch (Command.wSubCmdID)
				{
					case SUB_KN_DETECT_SOCKET:	//网络检测
					{
						break;
					}
					case SUB_KN_VALIDATE_SOCKET:
					{
						break;
					}
					default:
					{
						throw TEXT("非法命令码");
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
				//{	//消息处理;
				//	m_pIServerSocketItemSink->OnSocketReadEvent(Command, pDataBuffer, wDataSize, this);
				//}

				//消息处理;
				m_pIServerSocketItemSink->OnSocketReadEvent(Command, pDataBuffer, wDataSize, this);
			}

			//删除缓存数据
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

//关闭完成通知
bool CServerSocketItem::OnCloseCompleted()
{
	//效验状态
	ASSERT(m_hSocket == INVALID_SOCKET);
	ASSERT(m_OverLappedSendActive.GetCount() == 0);

	//关闭事件
	ASSERT(m_bNotify == false);
	if (m_bNotify == false)
	{
		m_bNotify = true;
		m_pIServerSocketItemSink->OnSocketCloseEvent(this);
	}

	//状态变量
	ResetSocketData();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerSocketRSThread::CServerSocketRSThread(void)
{
	m_hCompletionPort = NULL;
}

//析构函数
CServerSocketRSThread::~CServerSocketRSThread(void)
{
}

//配置函数
bool CServerSocketRSThread::InitThread(HANDLE hCompletionPort)
{
	ASSERT(hCompletionPort != NULL);
	m_hCompletionPort = hCompletionPort;
	return true;
}

//运行函数
bool CServerSocketRSThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort != NULL);

	//变量定义
	DWORD dwThancferred = 0;
	OVERLAPPED * pOverLapped = NULL;
	COverLapped * pSocketLapped = NULL;
	CServerSocketItem * pServerSocketItem = NULL;

	//等待完成端口
	BOOL bSuccess = GetQueuedCompletionStatus(m_hCompletionPort, &dwThancferred, (PULONG_PTR) & pServerSocketItem, &pOverLapped, INFINITE);
	if ((bSuccess == FALSE) && (GetLastError() != ERROR_NETNAME_DELETED)) return false;
	if ((pServerSocketItem == NULL) && (pOverLapped == NULL)) return false;

	//处理操作
	ASSERT(pOverLapped != NULL);
	ASSERT(pServerSocketItem != NULL);
	pSocketLapped = CONTAINING_RECORD(pOverLapped, COverLapped, m_OverLapped);
	switch (pSocketLapped->GetOperationType())
	{
		case OperationType_Recv:	//SOCKET 数据读取
		{
			COverLappedRecv * pOverLappedRecv = (COverLappedRecv *)pSocketLapped;
			CYSDataLocker lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->OnRecvCompleted(pOverLappedRecv, dwThancferred);
			break;
		}
		case OperationType_Send:	//SOCKET 数据发送
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

//构造函数
CSocketAcceptThread::CSocketAcceptThread(void)
{
	m_hCompletionPort = NULL;
	m_pTCPSocketManager = NULL;
	m_hListenSocket = INVALID_SOCKET;
}

//析构函数
CSocketAcceptThread::~CSocketAcceptThread(void)
{
}

//配置函数
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

//运行函数
bool CSocketAcceptThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort != NULL);
	ASSERT(m_pTCPSocketManager != NULL);

	//设置变量
	SOCKADDR_IN	SocketAddr;
	CServerSocketItem * pServerSocketItem = NULL;
	SOCKET hConnectSocket = INVALID_SOCKET;
	int nBufferSize = sizeof(SocketAddr);

	try
	{
		//监听连接
		hConnectSocket = WSAAccept(m_hListenSocket, (SOCKADDR *) & SocketAddr, &nBufferSize, NULL, NULL);
		if (hConnectSocket == INVALID_SOCKET) return false;

		//获取连接
		pServerSocketItem = m_pTCPSocketManager->ActiveSocketItem();
		if (pServerSocketItem == NULL) throw TEXT("申请连接对象失败");

		//激活对象
		CYSDataLocker lock(pServerSocketItem->GetSignedLock());
		pServerSocketItem->Attach(hConnectSocket, SocketAddr.sin_addr.S_un.S_addr);
		CreateIoCompletionPort((HANDLE)hConnectSocket, m_hCompletionPort, (ULONG_PTR)pServerSocketItem, 0);
		pServerSocketItem->RecvData();
	}
	catch (...)
	{
		//清理对象
		ASSERT(pServerSocketItem == NULL);
		if (hConnectSocket != INVALID_SOCKET)	closesocket(hConnectSocket);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSocketDetectThread::CSocketDetectThread(void)
{
	m_dwTickCount = 0;;
	m_pTCPSocketManager = NULL;
}

//析构函数
CSocketDetectThread::~CSocketDetectThread(void)
{
}

//配置函数
bool CSocketDetectThread::InitThread(CTCPNetworkEngine * pTCPSocketManager)
{
	//效验参数
	ASSERT(pTCPSocketManager != NULL);

	//设置变量
	m_dwTickCount = 0L;
	m_pTCPSocketManager = pTCPSocketManager;

	return true;
}

//运行函数
bool CSocketDetectThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_pTCPSocketManager != NULL);

	//设置间隔
	Sleep(500);
	m_dwTickCount += 500L;

	//检测连接
	if (m_dwTickCount > 20000L)
	{
		m_dwTickCount = 0L;
		m_pTCPSocketManager->DetectSocket();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
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

//析构函数
CTCPNetworkEngine::~CTCPNetworkEngine(void)
{
	//停止服务
	ConcludeService();

	//释放存储连接
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

//接口查询
void * CTCPNetworkEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPNetworkEngine, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPNetworkEngine, Guid, dwQueryVer);
	return NULL;
}

//配置端口
WORD CTCPNetworkEngine::GetServicePort()
{
	return m_wListenPort;
}

//当前端口
WORD CTCPNetworkEngine::GetCurrentPort()
{
	return m_wListenPort;
}

//设置接口
bool CTCPNetworkEngine::SetTCPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
	//状态判断
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("网络引擎处于服务状态，绑定操作忽略"), TraceLevel_Exception);
		return false;
	}

	//设置接口
	if (m_QueueServiceEvent.SetQueueServiceSink(pIUnknownEx) == false)
	{
		CTraceService::TraceString(TEXT("网络引擎与触发服务绑定失败"), TraceLevel_Exception);
		return false;
	}

	return true;
}

//设置参数
bool CTCPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect)
{
	//效验状态
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("网络引擎处于服务状态，端口绑定操作忽略"), TraceLevel_Exception);
		return false;
	}

	//判断参数
	if (wServicePort == 0)
	{
		CTraceService::TraceString(TEXT("未指定网络端口，将随机分配"), TraceLevel_Normal);
	}

	//设置变量
	m_wListenPort = wServicePort;
	m_wMaxSocketItem = wMaxConnect;
	return true;
}

//启动服务
bool CTCPNetworkEngine::StartService()
{
	DWORD i = 0;
	//效验状态
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("网络引擎重复启动，启动操作忽略"), TraceLevel_Warning);
		return true;
	}

	//绑定对象
	if (m_SendQueueService.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
	{
		CTraceService::TraceString(TEXT("网络引擎与触发服务绑定失败"), TraceLevel_Exception);
		return false;
	}

	try
	{
		//获取系统信息
		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		DWORD dwThreadCount = SystemInfo.dwNumberOfProcessors;

		//建立完成端口
		m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, SystemInfo.dwNumberOfProcessors);
		if (m_hCompletionPort == NULL) throw TEXT("网络引擎完成端口创建失败");

		//建立监听SOCKET
		struct sockaddr_in SocketAddr;
		memset(&SocketAddr, 0, sizeof(SocketAddr));
		SocketAddr.sin_addr.s_addr = INADDR_ANY;
		SocketAddr.sin_family = AF_INET;
		SocketAddr.sin_port = htons(m_wListenPort);
		m_hServerSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (m_hServerSocket == INVALID_SOCKET) throw TEXT("网络引擎监听 SOCKET 创建失败");
		int iErrorCode = bind(m_hServerSocket, (SOCKADDR*) & SocketAddr, sizeof(SocketAddr));
		if (iErrorCode == SOCKET_ERROR) throw TEXT("网络引擎监听端口被占用，端口绑定失败");
		iErrorCode = listen(m_hServerSocket, 200);
		if (iErrorCode == SOCKET_ERROR) throw TEXT("网络引擎监听端口被占用，端口监听失败");

		//获取端口
		int iLen=sizeof(SocketAddr); 
		getsockname(m_hServerSocket,(SOCKADDR*) & SocketAddr, &iLen);
		m_wListenPort=htons(SocketAddr.sin_port);

		//启动发送队列
		bool bSuccess = m_SendQueueService.StartService();
		if (bSuccess == false) throw TEXT("网络引擎发送队列服务启动失败");

		//建立读写线程
		for (i = 0; i < dwThreadCount; i++)
		{
			CServerSocketRSThread * pServerSocketRSThread = new CServerSocketRSThread();
			if (pServerSocketRSThread == NULL) throw TEXT("网络引擎读写线程服务创建失败");
			bSuccess = pServerSocketRSThread->InitThread(m_hCompletionPort);
			if (bSuccess == false) throw TEXT("网络引擎读写线程服务配置失败");
			m_SocketRSThreadArray.Add(pServerSocketRSThread);
		}

		//建立应答线程
		bSuccess = m_SocketAcceptThread.InitThread(m_hCompletionPort, m_hServerSocket, this);
		if (bSuccess == false) throw TEXT("网络引擎网络监听线程服务配置");

		//运行读写线程
		for (i = 0; i < dwThreadCount; i++)
		{
			CServerSocketRSThread * pServerSocketRSThread = m_SocketRSThreadArray[i];
			ASSERT(pServerSocketRSThread != NULL);
			bSuccess = pServerSocketRSThread->StartThread();
			if (bSuccess == false) throw TEXT("网络引擎读写线程服务启动失败");
		}

		//网络检测线程
		m_SocketDetectThread.InitThread(this);
		bSuccess = m_SocketDetectThread.StartThread();
		if (bSuccess == false) throw TEXT("网络引检测线程服务启动失败");

		//运行应答线程
		bSuccess = m_SocketAcceptThread.StartThread();
		if (bSuccess == false) throw TEXT("网络引擎监听线程服务启动失败");

		//设置变量
		m_bService = true;
	}
	catch (LPCTSTR pszError)
	{
		CTraceService::TraceString(pszError, TraceLevel_Exception);
		return false;
	}

	return true;
}

//停止服务
bool CTCPNetworkEngine::ConcludeService()
{
	//设置变量
	m_bService = false;
	m_dwLastDetect = 0L;

	//停止检测线程
	m_SocketDetectThread.ConcludeThread(INFINITE);

	//终止应答线程
	if (m_hServerSocket != INVALID_SOCKET)
	{
		closesocket(m_hServerSocket);
		m_hServerSocket = INVALID_SOCKET;
	}
	m_SocketAcceptThread.ConcludeThread(INFINITE);

	//停止发送队列
	m_SendQueueService.ConcludeService();

	//释放读写线程
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

	//关闭完成端口
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	//关闭连接
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

//应答消息
bool CTCPNetworkEngine::OnSocketAcceptEvent(CServerSocketItem * pServerSocketItem)
{
	//效验数据
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	//投递消息
	if (m_bService == false) return false;
	m_QueueServiceEvent.PostNetworkAcceptEvent(pServerSocketItem->GetSocketID(), pServerSocketItem->GetClientAddr());

	return true;
}

//网络读取消息
bool CTCPNetworkEngine::OnSocketReadEvent(TCP_Command Command, void * pBuffer, WORD wDataSize, CServerSocketItem * pServerSocketItem)
{
	//效验数据
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	//效验状态
	if (m_bService == false) return false;
	m_QueueServiceEvent.PostNetworkReadEvent(pServerSocketItem->GetSocketID(), Command, pBuffer, wDataSize);

	return true;
}

//网络关闭消息
bool CTCPNetworkEngine::OnSocketCloseEvent(CServerSocketItem * pServerSocketItem)
{
	//效验参数
	ASSERT(pServerSocketItem != NULL);
	if (NULL == pServerSocketItem) return false;

	try
	{
		//效验状态
		if (m_bService == false) return false;

		//计算时间
		WORD wIndex = pServerSocketItem->GetIndex();
		WORD wRountID = pServerSocketItem->GetRountID();
		DWORD dwClientAddr = pServerSocketItem->GetClientAddr();
		DWORD dwConnectTime = pServerSocketItem->GetConnectTime();
		//////////////////////////////////////////////////////////////////////////,这里要调整
		m_QueueServiceEvent.PostNetworkShutEvent(pServerSocketItem->GetSocketID() , dwClientAddr, dwConnectTime);

		//释放连接
		FreeSocketItem(pServerSocketItem);
	}
	catch (...) {}

	return true;
}

//通知回调函数（发送队列线程调用）
void CTCPNetworkEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	switch (wIdentifier)
	{
		case QUEUE_SEND_REQUEST:		//发送请求
		{
			//效验数据
			tagSendDataRequest * pSendDataRequest = (tagSendDataRequest *)pBuffer;
			ASSERT(wDataSize >= (sizeof(tagSendDataRequest) - sizeof(pSendDataRequest->cbSendBuf)));
			ASSERT(wDataSize == (pSendDataRequest->wDataSize + sizeof(tagSendDataRequest) - sizeof(pSendDataRequest->cbSendBuf)));

			//群发数据
			if (pSendDataRequest->dwSocketID == 0)
			{
				//获取活动项
				{
					CYSDataLocker lcok(m_CriticalSection);
					m_TempSocketItem.RemoveAll();
					m_TempSocketItem.Copy(m_ActiveSocketItem);
				}

				//循环发送数据
				CServerSocketItem * pServerSocketItem = NULL;
				for (INT_PTR i = 0; i < m_TempSocketItem.GetCount(); i++)
				{
					pServerSocketItem = m_TempSocketItem[i];
					ASSERT(pServerSocketItem != NULL);
					CYSDataLocker lock(pServerSocketItem->GetSignedLock());
					if (pServerSocketItem->IsAllowBatch())//第一次不允许？？
					{
						pServerSocketItem->SendData(pSendDataRequest->cbSendBuf, pSendDataRequest->wDataSize, pSendDataRequest->wMainCmdID,
						                            pSendDataRequest->wSubCmdID, pServerSocketItem->GetRountID());
					}
				}
			}
			else
			{
				//单项发送
				CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pSendDataRequest->dwSocketID));
				CYSDataLocker lock(pServerSocketItem->GetSignedLock());
				pServerSocketItem->SendData(pSendDataRequest->cbSendBuf, pSendDataRequest->wDataSize, pSendDataRequest->wMainCmdID,
				                            pSendDataRequest->wSubCmdID, HIWORD(pSendDataRequest->dwSocketID));
			}

			break;
		}
		case QUEUE_SAFE_CLOSE:		//安全关闭
		{
			//效验数据
			ASSERT(wDataSize == sizeof(tagSafeCloseSocket));
			tagSafeCloseSocket * pSafeCloseSocket = (tagSafeCloseSocket *)pBuffer;

			//安全关闭
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
		case QUEUE_ALLOW_BATCH:		//允许群发
		{
			//效验数据
			ASSERT(wDataSize == sizeof(tagAllowBatchSend));
			tagAllowBatchSend * pAllowBatchSend = (tagAllowBatchSend *)pBuffer;

			//设置群发
			CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(pAllowBatchSend->dwSocketID));
			CYSDataLocker lock(pServerSocketItem->GetSignedLock());
			pServerSocketItem->AllowBatchSend(HIWORD(pAllowBatchSend->dwSocketID), pAllowBatchSend->cbAllow ? true : false);

			break;
		}
		case QUEUE_DETECT_SOCKET:	//检测连接
		{
			//获取活动项
			{
				CYSDataLocker lcok(m_CriticalSection);
				m_TempSocketItem.RemoveAll();
				m_TempSocketItem.Copy(m_ActiveSocketItem);
			}

			//构造数据
			TCP_KN_DetectSocket DetectSocket;
			ZeroMemory(&DetectSocket, sizeof(DetectSocket));

			//变量定义
			WORD wRountID = 0;
			DWORD dwNowTickCount = GetTickCount();
			DWORD dwBreakTickCount = __max(dwNowTickCount - m_dwLastDetect, TIME_BREAK_READY);

			//设置变量
			m_dwLastDetect = GetTickCount();

			//检测连接
			for (INT_PTR i = 0; i < m_TempSocketItem.GetCount(); i++)
			{
				//变量定义
				CServerSocketItem * pServerSocketItem = m_TempSocketItem[i];
				DWORD dwRecvTickCount = pServerSocketItem->GetRecvTickCount();
				CYSDataLocker lock(pServerSocketItem->GetSignedLock());

				//间隔检查
				if (dwRecvTickCount >= dwNowTickCount) continue;

				//检测连接
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

				//发送数据
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

//获取空闲对象
CServerSocketItem * CTCPNetworkEngine::ActiveSocketItem()
{
	CYSDataLocker lock(m_CriticalSection);

	//获取空闲对象
	CServerSocketItem * pServerSocketItem = NULL;
	if (m_FreeSocketItem.GetCount() > 0)
	{
		INT_PTR nItemPostion = m_FreeSocketItem.GetCount() - 1;
		pServerSocketItem = m_FreeSocketItem[nItemPostion];
		ASSERT(pServerSocketItem != NULL);
		m_FreeSocketItem.RemoveAt(nItemPostion);
		m_ActiveSocketItem.Add(pServerSocketItem);
	}

	//创建新对象
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

//获取连接对象
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

//释放连接对象
bool CTCPNetworkEngine::FreeSocketItem(CServerSocketItem * pServerSocketItem)
{
	//效验参数
	ASSERT(pServerSocketItem != NULL);

	//释放对象
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

	//释放失败
	ASSERT(FALSE);
	return false;
}

//检测连接
bool CTCPNetworkEngine::DetectSocket()
{
	return m_SendQueueService.AddToQueue(QUEUE_DETECT_SOCKET, NULL, 0);
}


//发送函数
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	//效益状态
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//构造数据
	tagSendDataRequest SendRequest;
	SendRequest.wMainCmdID = wMainCmdID;
	SendRequest.wSubCmdID = wSubCmdID;
	SendRequest.dwSocketID = dwSocketID;
	SendRequest.wDataSize = 0;

	//加入发送队列
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf);
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//发送函数
bool CTCPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//效益状态
	ASSERT(m_bService == true);
	if (m_bService == false) return false;

	//效益数据
	ASSERT((wDataSize + sizeof(TCP_Head)) <= SOCKET_TCP_BUFFER);
	if ((wDataSize + sizeof(TCP_Head)) > SOCKET_TCP_BUFFER) return false;

	//构造数据
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

	//加入发送队列
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf) + wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//批量发送
bool CTCPNetworkEngine::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize, BYTE cbBatchMask)
{
	//效益状态
	if (m_bService == false) return false;

	//效益数据
	ASSERT((wDataSize + sizeof(TCP_Head)) <= SOCKET_TCP_BUFFER);
	if ((wDataSize + sizeof(TCP_Head)) > SOCKET_TCP_BUFFER) return false;

	//构造数据
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

	//加入发送队列
	WORD wSendSize = sizeof(SendRequest) - sizeof(SendRequest.cbSendBuf) + wDataSize;
	return m_SendQueueService.AddToQueue(QUEUE_SEND_REQUEST, &SendRequest, wSendSize);
}

//关闭连接
bool CTCPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
	CServerSocketItem * pServerSocketItem = EnumSocketItem(LOWORD(dwSocketID));
	if (pServerSocketItem == NULL) return false;
	CYSDataLocker lock(pServerSocketItem->GetSignedLock());
	return pServerSocketItem->CloseSocket(HIWORD(dwSocketID));
}

//设置关闭
bool CTCPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
	tagSafeCloseSocket SafeCloseSocket;
	SafeCloseSocket.dwSocketID = dwSocketID;
	return m_SendQueueService.AddToQueue(QUEUE_SAFE_CLOSE, &SafeCloseSocket, sizeof(SafeCloseSocket));
}

//允许群发
bool CTCPNetworkEngine::AllowBatchSend(DWORD dwSocketID, bool bAllow, BYTE cbBatchMask)
{
	tagAllowBatchSend AllowBatchSendNode;
	AllowBatchSendNode.dwSocketID = dwSocketID;
	AllowBatchSendNode.cbAllow = bAllow;
	return m_SendQueueService.AddToQueue(QUEUE_ALLOW_BATCH, &AllowBatchSendNode, sizeof(tagAllowBatchSend));
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * CreateTCPNetworkEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CTCPNetworkEngine * pTCPSocketEngine = NULL;
	try
	{
		pTCPSocketEngine = new CTCPNetworkEngine();
		if (pTCPSocketEngine == NULL) throw TEXT("创建失败");
		void * pObject = pTCPSocketEngine->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
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
	//接受首个包的时候,才去确定是否是WebSocket;
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
		{	//数据没有接受全当成中间包吧;
			nRet = WS_MIDDLE_FRAME;
			break;
		}

		frameHeader.cbFin = (unsigned char)pFrameData[0] >> 7;
		frameHeader.cbopcode = pFrameData[0] & 0x0f;
		frameHeader.cbMask = (unsigned char)pFrameData[1] >> 7;
		frameHeader.cbPayloadlen = pFrameData[1] & 0x7f;

		if (frameHeader.cbopcode == WS_CLOSING_FRAME)
		{	//连接关闭消息;
			nRet = WS_CLOSING_FRAME;
			break;
		}

		if (m_dwRecvSize < (frameHeader.cbPayloadlen + nCanReadOffset))
		{	//数据没有接受全当成中间包吧;
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
		{	//数据没有接受全当成中间包吧;
			nRet = WS_MIDDLE_FRAME;
			break;
		}

		if (nCapcity < frameHeader.payloadLen)
		{	//数据放不下了，跳出处理去;
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
	{	//暂不支持这么长的数据;
		return WS_ERROR_FRAME;
	}

	if (nOutCapcity < (nInSize + 4))
	{	//outMessage存储空间不够;
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
