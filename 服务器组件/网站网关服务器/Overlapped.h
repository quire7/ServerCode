#ifndef __OEVERLAPPED_H__
#define __OEVERLAPPED_H__

#define IoSend	1	// �����¼�
#define IoRecv	2	// �����¼�
#define IoExit	3	// �˳�ϵͳ

#define MAX_PACKAGE_SIZE	2048	// ���η��ͻ���ܰ��Ĵ�С

#define MAX_USER_PACK_SIZE	65534	// �û������������������ֽ���

struct stOverlappedBase : public OVERLAPPED
{
	UINT IoMode;
	WORD nClient;
	WSABUF WsaBuf;

	stOverlappedBase()
	{
		// init OVERLAPPED
		Internal=0;
		InternalHigh=0;
		Offset=0;
		OffsetHigh=0;
		hEvent=0;

		// init SOverlapped
		IoMode=0;
		nClient=0xFFFF;
		WsaBuf.buf=NULL;
		WsaBuf.len=0;
	}
};


struct stSendOverlapped : public stOverlappedBase
{
	char *pBuffer;			// �������ݵĻ���ָ��
	DWORD dwSentBytes;		// �Ѿ����͵��ֽ���
	DWORD dwTotalBytes;		// ��ǰ�����ܵ��ֽ���
	stSendOverlapped()
	{
		IoMode = IoSend;
		pBuffer = NULL;
		dwSentBytes = 0;
		dwTotalBytes = 0;
	}
};


struct stRecvOverlapped : public stOverlappedBase
{
	char buffer[MAX_PACKAGE_SIZE];	// ���ջ���

	stRecvOverlapped()
	{
		memset(buffer, 0, MAX_PACKAGE_SIZE);

		IoMode = IoRecv;
		WsaBuf.buf=buffer;
		WsaBuf.len=MAX_PACKAGE_SIZE;
	}
};

#endif