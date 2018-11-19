#ifndef __OEVERLAPPED_H__
#define __OEVERLAPPED_H__

#define IoSend	1	// 发送事件
#define IoRecv	2	// 接收事件
#define IoExit	3	// 退出系统

#define MAX_PACKAGE_SIZE	2048	// 单次发送或接受包的大小

#define MAX_USER_PACK_SIZE	65534	// 用户封包，单个包的最大字节数

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
	char *pBuffer;			// 发送数据的缓存指针
	DWORD dwSentBytes;		// 已经发送的字节数
	DWORD dwTotalBytes;		// 当前包的总的字节数
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
	char buffer[MAX_PACKAGE_SIZE];	// 接收缓存

	stRecvOverlapped()
	{
		memset(buffer, 0, MAX_PACKAGE_SIZE);

		IoMode = IoRecv;
		WsaBuf.buf=buffer;
		WsaBuf.len=MAX_PACKAGE_SIZE;
	}
};

#endif