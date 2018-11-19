/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDServerSocket.h
	file base:	NDServerSocket
	purpose:	server socket;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SERVER_SOCKET_H__
#define __SHARE_BASE_ND_SERVER_SOCKET_H__


#include "NDSocket.h"

_NDSHAREBASE_BEGIN


class NDServerSocket : public NDSocket
{
public:
	NDServerSocket():NDSocket() {};
	~NDServerSocket(){};
};



_NDSHAREBASE_END
#endif

