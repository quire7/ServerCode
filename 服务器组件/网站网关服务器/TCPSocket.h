#pragma once

#include <winsock2.h>
#include "ByteBuffer.h"
#include "Overlapped.h"
#include "IOCPServer.h"


// ������Ϣ�Ľӿ�
class MessageProcesser
{
public:
	MessageProcesser(){}
	virtual ~MessageProcesser(){}
	virtual bool processMessage(void *data, DWORD cmdLen) = 0;
};


class CIOCPServer;

// Ĭ�����ݰ���ʽ
// WORD | BIN
// Size | Datas
// ǰ�����ֽڴ����С��������ָ�����ȵ�����

class CTCPSocket : public MessageProcesser
{
	friend class CIOCPServer;

public:
	CTCPSocket(WORD id, WORD socket, DWORD dwClientAddr, CIOCPServer* pIOCPServer);
	virtual ~CTCPSocket(void);

	WORD getID() { return m_wID; }

	SOCKET getSocket() { return m_hSocket; }
	//��ȡ��ַ
	DWORD getClientAddr() { return m_dwClientAddr; }

	// ��������
	virtual bool sendData(void* data, DWORD len);

public:
	// ������յ�������
	virtual bool processRecvData(void* data, DWORD len);

	// �����������Ϣ����
	virtual bool processMessage(void *data, DWORD cmdLen){return true;}

public:
	CIOCPServer* m_pIOCPServer;

private:
	WORD		m_wID;
	SOCKET		m_hSocket;
	DWORD		m_dwClientAddr;
	ByteBuffer	m_recvBuffer;
};
