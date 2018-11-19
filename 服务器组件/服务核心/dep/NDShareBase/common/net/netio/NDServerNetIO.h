/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\netio\NDServerNetIO.h
	file base:	NDServerNetIO
	purpose:	server net IO;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SERVER_NETIO_H__
#define __SHARE_BASE_ND_SERVER_NETIO_H__

#include "NDTypes.h"
#include "net/socket/NDSocketEnv.h"


_NDSHAREBASE_BEGIN

struct NDSocketAddress;

class NDSocketAcceptor;
class NDProtocol;
class NDProcess;

class NDServerNetIO
{
private:
	static NDSocketEnv		m_pNDSocketEnv;
	NDSocketAcceptor*		m_pNDSocketAcceptor;
	
public:
	NDServerNetIO(void);
	~NDServerNetIO(void);

	NDBool		startServer( const char* pHostIP, NDUint16 nPort );

	//accept socket connection;
	//nMaxConnectionPer param is per max connection socket;
	NDBool		accept( NDUint8 nMaxConnectionPer );

	//send protocol packet to client;
	NDBool		sendToClient( NDProtocol& rProtocol, NDUint32 nSessionID, NDBool bCompression, NDBool bEncrypt, NDBool bCrc );
	
	//disconnect client;
	NDBool		disConnect( NDUint32 nSessionID );
	NDBool		getClientAddress( NDUint32 nSessionID, NDSocketAddress& refClientAddress );


	
	NDBool		setSessionProtocolType( NDUint32 nSessionID, NDUint8 nSessionProtocolType );


	//设置处理函数;
	static	NDBool		setProcessor( NDProcess* pProcess );
	//设置通用客户端断开连接时的内部向上层抛出的DisconnectNtyProtocol;
	static	NDBool		setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol );

	static	NDBool		checkSessionTimerFunction();

};


_NDSHAREBASE_END
#endif

