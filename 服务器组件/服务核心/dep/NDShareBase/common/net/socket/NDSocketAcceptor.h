/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\socket\NDSocketAcceptor.h
	file base:	NDSocketAcceptor
	purpose:	socket acceptor;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SOCKET_ACCEPTOR_H__
#define __SHARE_BASE_ND_SOCKET_ACCEPTOR_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDSocket;
class NDSocketAcceptor
{
private:
	SOCKET	m_nListenSocketID;
public:
	NDSocketAcceptor();
	~NDSocketAcceptor();

	//Initialize a passive-mode acceptor socket;
	NDBool	open( NDUint16 nPort );

	//Accept a connection;
	NDBool	accept( NDSocket* pSocket, NDUint32& refErrorCode );

	//isAccept is m_nListenSocketID can read;
	NDBool	isAccept();
};



_NDSHAREBASE_END
#endif

