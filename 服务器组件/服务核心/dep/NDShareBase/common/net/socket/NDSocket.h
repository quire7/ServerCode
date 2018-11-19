/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDSocket.h
	file base:	NDSocket
	purpose:	socket base class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SOCKET_H__
#define __SHARE_BASE_ND_SOCKET_H__


#include "NDTypes.h"
#include "NDSocketAddress.h"

_NDSHAREBASE_BEGIN

class NDSocket
{
protected:
	SOCKET			m_nSocketID;

	NDSocketAddress	m_peerAddr;

public:
	NDSocket();
	NDSocket( const char* szPeerIP, NDUint16 nPeerPort );
	virtual ~NDSocket();
	//NDSocket( SOCKET nSocketID ) : m_nSocketID( nSocketID ){};

	// close connection;
	void				close();
	void				shutdownSendPipe();

	//get socket descriptor;
	SOCKET				getSOCKET() const	{ return m_nSocketID; };

	SOCKET&				getSOCKETRef()		{ return m_nSocketID; };
	NDSocketAddress&	getPeerAddr()		{ return m_peerAddr; };

	//check socket is valid;
	NDBool				isValidSocket();

	// send data to peer;
	NDBool				send( const void* buf, NDUint32 len, NDUint32& refRealSendLen, NDUint32 flags = 0);

	// receive data from peer;
	NDBool				recv( void* buf, NDUint32 len, NDUint32& refRealRecvLen, NDUint32 flags = 0 );

	// get amount of data in socket input buffer;
	NDUint32			getRecvBufBytes();

public:
	// get/set socket's nonblocking status;
	NDBool				isNonblocking();
	NDBool				setNonblocking();

	// get/set socket's reuseAddr;
	NDBool				isReuseAddr();
	NDBool				setReuseAddr();

	// Disable/Enables nagle buffering algorithm;
	NDBool				setDisableBuffering();
	NDBool				setEnableBuffering();

	// get/set socket receive buffer size;
	NDUint32			getRecvBufferSize();
	NDBool				setRecvBufferSize( NDUint32 nSize );

	// get/set socket send buffer size;
	NDUint32			getSendBufferSize();
	NDBool				setSendBufferSize( NDUint32 nSize );

	//set/get peer IP and port;
	NDBool				setPeerIP( const char* pStrIP )	{ return m_peerAddr.setIP( pStrIP );	};
	void				setPeerPort( NDUint16 nPort )	{ m_peerAddr.setPort( nPort );	};

	const char*			getPeerIP() const	{ return m_peerAddr.getIP(); };
	NDUint16			getPeerPort() const { return m_peerAddr.getPort(); };
	NDBool				isValidAddr() const { return m_peerAddr.isValidAddr(); };

};





_NDSHAREBASE_END
#endif

