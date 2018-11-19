/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\netio\NDClientNetIO.h
	file base:	NDClientNetIO
	purpose:	client net IO;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CLIENT_NETIO_H__
#define __SHARE_BASE_ND_CLIENT_NETIO_H__

#include "NDTypes.h"
#include "event/timerEvent/NDTimerBoundSlot.h"

_NDSHAREBASE_BEGIN

class NDProcess;
class NDSession;
class NDProtocol;

class NDClientNetIO
{
private:
	NDSession*				m_pClientSession;

	NDProtocol*				m_pPingProtocol;
	NDTimerBoundSlotConn	m_timerPingConn;
	static const char*		s_szTimerPingProtocol;

public:
	NDClientNetIO(void);
	~NDClientNetIO(void);
	
	NDBool		connect( const char* pszHostIP, NDUint16 nPort );
	NDBool		sendToServer( NDProtocol& rProtocol, NDBool bCompression, NDBool bEncrypt, NDBool bCrc );
	NDBool		isConnected() const;
	void		disConnect();
	
	NDUint32	getSessionID() const;

	NDBool		setSessionProtocolType( NDUint8 sessionProtocolType );

	//…Ë÷√ping–≠“È;
	NDBool		setPingProtocol( NDProtocol* pPingProtocol );

	static void setProcessor( NDProcess* pProcess );

private:
	NDBool		pingFunction( const NDEventArgs& );
};


_NDSHAREBASE_END

#endif

