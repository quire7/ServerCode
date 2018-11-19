#pragma once

#include "..\服务核心\ServiceCoreHead.h"

class CIOCPServer;

//////////////////////////////////////////////////////////////////////////

//读写线程类
class CIOWorkerThread : public CYSThread
{
	//变量定义
protected:
	HANDLE							m_hCompletionPort;					//完成端口
	CIOCPServer*					m_pIOCPServer;

	//函数定义
public:
	//构造函数
	CIOWorkerThread(void);
	//析构函数
	virtual ~CIOWorkerThread(void);

	//功能函数
public:
	//配置函数
	bool InitThread(HANDLE hCompletionPort, CIOCPServer* pIOCPServer);

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
};

//////////////////////////////////////////////////////////////////////////

//应答线程对象
class CIOAcceptThread : public CYSThread
{
	//变量定义
protected:
	SOCKET							m_hListenSocket;					//监听连接
	HANDLE							m_hCompletionPort;					//完成端口
	CIOCPServer*					m_pIOCPServer;

	//函数定义
public:
	//构造函数
	CIOAcceptThread(void);
	//析构函数
	virtual ~CIOAcceptThread(void);

	//功能函数
public:
	//配置函数
	bool InitThread(HANDLE hCompletionPort, SOCKET hListenSocket, CIOCPServer* pIOCPServer);

	//重载函数
private:
	//运行函数
	virtual bool OnEventThreadRun();
};

