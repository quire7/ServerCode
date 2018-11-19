/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDSockOps.h
	file base:	NDSockOps
	purpose:	socket base operation;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SOCK_OPS_H__
#define __SHARE_BASE_ND_SOCK_OPS_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

struct NDSocketAddress;
struct NDSockOps
{
public:
	// Create file descriptor for socket i/o operations.;
	static SOCKET		createTCPFileDescriptor(); 

	// Disable blocking send/recv calls.;
	static NDBool		setNonblocking( SOCKET* pFd );

	// judge socket non blocking;
	static NDBool		isNonblocking( SOCKET* pFd );

	// Enable blocking send/recv calls.;
	static NDBool		setblocking( SOCKET* pFd );

	// Disable nagle buffering algorithm;
	static NDBool		setDisableBuffering( SOCKET* pFd );

	// Enables nagle buffering algorithm;
	static NDBool		setEnableBuffering( SOCKET* pFd );

	// Set internal buffer size to socket.;
	static NDBool		setRecvBufferSize( SOCKET* pFd , NDUint32 size);

	// get internal buffer size from socket.;
	static NDUint32		getRecvBufferSize( SOCKET* pFd );

	// Set internal buffer size to socket.;
	static NDBool		setSendBufferSize( SOCKET* pFd , NDUint32 size);

	// get internal buffer size from socket.;
	static NDUint32		getSendBufferSize( SOCKET* pFd );

	// Sets SO_REUSEADDR;
	static NDBool		setReuseAddr( SOCKET* pFd );

	// judge socket is reuse address;
	static NDBool		isReuseAddr( SOCKET* pFd );

	// Sets SO_LINGER (avoid TIME_WAIT);
	static NDBool		setLinger( SOCKET* pFd );

	// shutdown socket write pipe;
	static void			shutdownSendPipe( SOCKET* pFd );

	// Closes socket completely.;
	static void			closeSocket( SOCKET* pFd );

	//judge socket is valid;
	static NDBool		isValidSocket( SOCKET* pFd );

	// get amount of data in socket input buffer;
	static NDUint32		getSocketRecvBufferBytes( SOCKET* pFd );

	// bind port to pServerSocketFd socket;
	static NDBool		bind( SOCKET* pServerSocketFd, NDUint16 nPort );

	// set pServerSocketFd socket listen;
	static NDBool		listen( SOCKET* pServerSocketFd, NDUint32 backlog );

	// accept a client connection;
	static NDBool		accept( SOCKET* pServerSocketFd, SOCKET* pClientSocketFd, NDSocketAddress& refClientAddr, NDUint32& refErrorCode );

	//client connect socket;
	static NDBool		connect( SOCKET* pFd, const char* szPeerIP, NDUint16 nPeerPort );

	//send buffer data to socket;
	static NDBool		send( SOCKET* pFd, const void* buf, NDUint32 len, NDUint32& refRealSendLen, NDUint32 flags );

	//recv buffer data from socket;
	static NDBool		recv( SOCKET* pFd, void* buf, NDUint32 len, NDUint32& refRealRecvLen, NDUint32 flags);

	//judge socket read event;
	static NDBool		isReadEvent( SOCKET* pFd );

	//judge socket write event;
	static NDBool		isWriteEvent( SOCKET* pFd );

private:
	static NDBool		setsockoption( SOCKET s, int level, int optname, const void* optval, int optlen );

	static NDBool		getsockoption( SOCKET s, int level, int optname, void* optval,int* optlen );

	static NDBool		ioctlsocket( SOCKET s, long cmd, unsigned long *argp );
};


_NDSHAREBASE_END
#endif

