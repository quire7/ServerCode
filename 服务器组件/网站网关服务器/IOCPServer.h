#ifndef __IOCPSERVER_H__
#define __IOCPSERVER_H__

#include <map>
#include "IDAllocator.h"
#include "Overlapped.h"
#include "TCPSocket.h"
#include "IOThreads.h"

using namespace std;

void ErrorExit(LPTSTR lpszFunction);

class CTCPSocket;

typedef map<WORD,CTCPSocket*> SocketMap;
typedef CArray<CIOWorkerThread *> CIOWorkerThreadPtrArray;

class CIOCPServer
{
public:
	CIOCPServer(void);
	virtual ~CIOCPServer(void);

	//启动
	bool Start(WORD nMaxClient, WORD nPort);
	//停止
	void Stop();

	// 有新的客户端连接时，产生的事件
	virtual CTCPSocket* onCreateTCPSocket(WORD nClient, WORD socket, DWORD dwClientAddr);
	
	// 断开客户端
	bool Disconnect(WORD nClient);

	// 发送数据
	bool SendData(WORD nClient, void* data, DWORD len);

public:
	// 插入socketmap，并分配一个ID
	CTCPSocket* CreateTCPSocket(SOCKET socket, DWORD dwClientAddr);
	WORD InserTCPSocket(CTCPSocket* tcpSocket);
	void RemoveTCPSocket(WORD nClient);
	CTCPSocket* GetTCPSocket(WORD nClient);
	bool PostRecv(WORD nClient, SOCKET socket);

public:
	// 事件通知
	virtual int OnAccept(WORD nClient) {return 0;}
	virtual int OnClose(WORD nClient) {return 0;}
	virtual int OnSend(WORD nClient, char* pData, DWORD dwDataLen) {return 0;}
	virtual int OnReceive(WORD nClient, char* pData, DWORD dwDataLen) {return 0;}
	virtual int OnError(WORD nClient, int iError) {return 0;}

private:
	CIOAcceptThread				m_IOAcceptThread;				//应答线程
	CIOWorkerThreadPtrArray		m_IOWorkerThreadArray;			//读写线程

	SOCKET m_hServerSocket;
	HANDLE m_hCompletionPort;
	SocketMap m_mapSocket;

	CCriticalSection m_cs;
	IDAllocator m_idAllocator;
};


#endif