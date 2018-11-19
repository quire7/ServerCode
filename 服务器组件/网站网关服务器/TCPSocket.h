#pragma once

#include <winsock2.h>
#include "ByteBuffer.h"
#include "Overlapped.h"
#include "IOCPServer.h"


// 处理消息的接口
class MessageProcesser
{
public:
	MessageProcesser(){}
	virtual ~MessageProcesser(){}
	virtual bool processMessage(void *data, DWORD cmdLen) = 0;
};


class CIOCPServer;

// 默认数据包格式
// WORD | BIN
// Size | Datas
// 前两个字节代表大小，后面是指定长度的数据

class CTCPSocket : public MessageProcesser
{
	friend class CIOCPServer;

public:
	CTCPSocket(WORD id, WORD socket, DWORD dwClientAddr, CIOCPServer* pIOCPServer);
	virtual ~CTCPSocket(void);

	WORD getID() { return m_wID; }

	SOCKET getSocket() { return m_hSocket; }
	//获取地址
	DWORD getClientAddr() { return m_dwClientAddr; }

	// 发送数据
	virtual bool sendData(void* data, DWORD len);

public:
	// 处理接收到的数据
	virtual bool processRecvData(void* data, DWORD len);

	// 处理处理过的消息数据
	virtual bool processMessage(void *data, DWORD cmdLen){return true;}

public:
	CIOCPServer* m_pIOCPServer;

private:
	WORD		m_wID;
	SOCKET		m_hSocket;
	DWORD		m_dwClientAddr;
	ByteBuffer	m_recvBuffer;
};
