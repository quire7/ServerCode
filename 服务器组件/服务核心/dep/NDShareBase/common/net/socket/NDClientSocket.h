/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDClientSocket.h
	file base:	NDClientSocket
	purpose:	client socket;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CLIENT_SOCKET_H__
#define __SHARE_BASE_ND_CLIENT_SOCKET_H__


#include "NDSocket.h"

_NDSHAREBASE_BEGIN

//NDClientSocket类约定赋值过地址和端口后,创建本地SOCKET;
class NDClientSocket : public NDSocket
{
public:
	NDClientSocket();
	NDClientSocket( const char* szPeerIP, NDUint16 nPeerPort );
	~NDClientSocket();

	//connect to remote server;
	NDBool		connect();
	NDBool		connect( const char* szPeerIP, NDUint16 nPeerPort );
	
	//close previous connection and connect to another server socket;
	NDBool		reconnect( const char* szPeerIP, NDUint16 nPeerPort );

private:
	// create socket descriptor;
	NDBool		createSocket();
};



_NDSHAREBASE_END
#endif

