#pragma once

#include "..\�������\ServiceCoreHead.h"

class CIOCPServer;

//////////////////////////////////////////////////////////////////////////

//��д�߳���
class CIOWorkerThread : public CYSThread
{
	//��������
protected:
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	CIOCPServer*					m_pIOCPServer;

	//��������
public:
	//���캯��
	CIOWorkerThread(void);
	//��������
	virtual ~CIOWorkerThread(void);

	//���ܺ���
public:
	//���ú���
	bool InitThread(HANDLE hCompletionPort, CIOCPServer* pIOCPServer);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//Ӧ���̶߳���
class CIOAcceptThread : public CYSThread
{
	//��������
protected:
	SOCKET							m_hListenSocket;					//��������
	HANDLE							m_hCompletionPort;					//��ɶ˿�
	CIOCPServer*					m_pIOCPServer;

	//��������
public:
	//���캯��
	CIOAcceptThread(void);
	//��������
	virtual ~CIOAcceptThread(void);

	//���ܺ���
public:
	//���ú���
	bool InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CIOCPServer* pIOCPServer);

	//���غ���
private:
	//���к���
	virtual bool OnEventThreadRun();
};

