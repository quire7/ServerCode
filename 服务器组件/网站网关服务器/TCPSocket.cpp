#include "StdAfx.h"
#include "TCPSocket.h"

CTCPSocket::CTCPSocket(WORD id, WORD socket, DWORD dwClientAddr, CIOCPServer* pIOCPServer)
{
	m_wID = id;
	m_hSocket = socket;
	m_dwClientAddr = dwClientAddr;
	m_pIOCPServer = pIOCPServer;
}

CTCPSocket::~CTCPSocket(void)
{
}

bool CTCPSocket::sendData(void* data, DWORD len)
{
	if (m_hSocket == INVALID_SOCKET)
		return false;

	if (data == NULL || len == 0)
		return false;

	return m_pIOCPServer->SendData(m_wID, data, len);
}

bool CTCPSocket::processRecvData(void* data, DWORD len)
{
	const int HEADER_SIZE = 2;

	// 先压入数据
	m_recvBuffer.write(data, len);

	// 如果当前缓存大小小于2个字节
	if (m_recvBuffer.size() < HEADER_SIZE)
		return true;

	// 判断前面两个字节（代表包长度）
	WORD size = 0;
	m_recvBuffer.get(&size, sizeof(size));
	if (size < HEADER_SIZE || size > MAX_USER_PACK_SIZE) {
		printf("用户封包超过最大限制，无法处理, id=%d\n", m_wID);
		m_pIOCPServer->Disconnect(m_wID);
		return false;
	}

	// 如果包数据不够，继续接收数据
	if (size > m_recvBuffer.size()) {
		return true;
	}

	// 根据长度取出数据
	BYTE userPack[MAX_USER_PACK_SIZE];
	m_recvBuffer.read(userPack, size);

	// 把取出的数据交给消息处理器来处理
	return processMessage(userPack, size);
}
