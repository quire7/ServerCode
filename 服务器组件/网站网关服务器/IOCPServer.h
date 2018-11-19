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

	//����
	bool Start(WORD nMaxClient, WORD nPort);
	//ֹͣ
	void Stop();

	// ���µĿͻ�������ʱ���������¼�
	virtual CTCPSocket* onCreateTCPSocket(WORD nClient, WORD socket, DWORD dwClientAddr);
	
	// �Ͽ��ͻ���
	bool Disconnect(WORD nClient);

	// ��������
	bool SendData(WORD nClient, void* data, DWORD len);

public:
	// ����socketmap��������һ��ID
	CTCPSocket* CreateTCPSocket(SOCKET socket, DWORD dwClientAddr);
	WORD InserTCPSocket(CTCPSocket* tcpSocket);
	void RemoveTCPSocket(WORD nClient);
	CTCPSocket* GetTCPSocket(WORD nClient);
	bool PostRecv(WORD nClient, SOCKET socket);

public:
	// �¼�֪ͨ
	virtual int OnAccept(WORD nClient) {return 0;}
	virtual int OnClose(WORD nClient) {return 0;}
	virtual int OnSend(WORD nClient, char* pData, DWORD dwDataLen) {return 0;}
	virtual int OnReceive(WORD nClient, char* pData, DWORD dwDataLen) {return 0;}
	virtual int OnError(WORD nClient, int iError) {return 0;}

private:
	CIOAcceptThread				m_IOAcceptThread;				//Ӧ���߳�
	CIOWorkerThreadPtrArray		m_IOWorkerThreadArray;			//��д�߳�

	SOCKET m_hServerSocket;
	HANDLE m_hCompletionPort;
	SocketMap m_mapSocket;

	CCriticalSection m_cs;
	IDAllocator m_idAllocator;
};


#endif