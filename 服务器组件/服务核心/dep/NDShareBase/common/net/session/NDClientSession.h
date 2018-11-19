/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\session\NDClientSession.h
	file base:	NDClientSession
	purpose:	client session;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CLIENT_SESSION_H__
#define __SHARE_BASE_ND_CLIENT_SESSION_H__


#include "NDSession.h"

_NDSHAREBASE_BEGIN


class NDClientSession : public NDSession
{
private:

public:
	NDClientSession( NDUint32 nSessionID );
	~NDClientSession();

	void			destroy();

	NDBool			connect( const char* pszHostIP, NDUint16 nPort );
};



_NDSHAREBASE_END
#endif

