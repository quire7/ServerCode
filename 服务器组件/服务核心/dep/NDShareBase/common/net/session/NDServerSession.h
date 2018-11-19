/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\session\NDServerSession.h
	file base:	NDServerSession
	purpose:	server session;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SERVER_SESSION_H__
#define __SHARE_BASE_ND_SERVER_SESSION_H__


#include "NDSession.h"

_NDSHAREBASE_BEGIN

class NDServerSocket;
class NDServerSession : public NDSession
{
private:
	NDTime					m_nLastPingSecondTime;		//客户端上次的PING包的时间(如果需要回复client的话,这个值的放在基类中);


public:
	NDServerSession( NDUint32 nSessionID );
	~NDServerSession();

	void					destroy();

	void					setLastPingSecondTime( NDTime nLastPingTime )	{ m_nLastPingSecondTime = nLastPingTime; };
	NDTime					getLastPingSecondTime() const					{ return m_nLastPingSecondTime; };


	
};



_NDSHAREBASE_END
#endif

