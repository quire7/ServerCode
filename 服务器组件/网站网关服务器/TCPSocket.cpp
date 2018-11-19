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

	// ��ѹ������
	m_recvBuffer.write(data, len);

	// �����ǰ�����СС��2���ֽ�
	if (m_recvBuffer.size() < HEADER_SIZE)
		return true;

	// �ж�ǰ�������ֽڣ���������ȣ�
	WORD size = 0;
	m_recvBuffer.get(&size, sizeof(size));
	if (size < HEADER_SIZE || size > MAX_USER_PACK_SIZE) {
		printf("�û��������������ƣ��޷�����, id=%d\n", m_wID);
		m_pIOCPServer->Disconnect(m_wID);
		return false;
	}

	// ��������ݲ�����������������
	if (size > m_recvBuffer.size()) {
		return true;
	}

	// ���ݳ���ȡ������
	BYTE userPack[MAX_USER_PACK_SIZE];
	m_recvBuffer.read(userPack, size);

	// ��ȡ�������ݽ�����Ϣ������������
	return processMessage(userPack, size);
}
