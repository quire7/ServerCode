/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\session\NDSession.h
	file base:	NDSession
	purpose:	session base class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SESSION_H__
#define __SHARE_BASE_ND_SESSION_H__


#include "net/socket/NDSocket.h"

_NDSHAREBASE_BEGIN

class NDByteBuffer;
class NDSysLock;

class NDSession
{
protected:
	NDSocket*				m_pSocket;
	NDUint32				m_nSessionID;				//会话的唯一标志ID;
	NDUint32				m_nSocketSendBuf;			//m_pSocket的发送缓存区大小;

	NDUint8					m_nDisconnectionType;		//断开连接类型(用于区分是我方主动断开连接还是对方主动断开连接);
	NDUint8					m_nProtocolType;			//session要处理的协议类型;
	NDUint8					m_nEventFlag;				//事件标志(EIOEventType);
	NDBool					m_bAlive;					//会话是否存活标志;
	NDBool					m_bSendPipe;				//发送管道是否可用;

	NDUint32				m_nRecvPacketNum;			//记录接受消息包的数量;
	NDUint32				m_nSendPacketNum;			//记录发送消息包的数量;
	NDUint32				m_nRecvPacketBytes;			//记录接受消息包的字节数;
	NDUint32				m_nSendPacketBytes;			//记录发送消息包的字节数;


	NDByteBuffer*			m_pSendBuffer;				//发送缓冲区;
	NDByteBuffer*			m_pRecvBuffer;				//接收缓冲区;
	NDSysLock*				m_pSendDataMutex;			//发送缓冲区锁;
	NDSysLock*				m_pRecvDataMutex;			//接收缓冲区锁;

	static PParsePacketFun	s_pParsePacket;				//解析包的函数;

public:
	NDSession( NDUint32 nSessionID );
	virtual ~NDSession();

	
	virtual void			destroy()=0;
	NDBool					initSession();
	NDBool					releaseSession();

	NDUint32				getSessionID() const		{ return m_nSessionID; };
	NDUint32				getRecvPacketNum() const	{ return m_nRecvPacketNum; };
	NDUint32				getSendPacketNum() const	{ return m_nSendPacketNum; };
	NDUint32				getRecvPacketBytes() const	{ return m_nRecvPacketBytes; };
	NDUint32				getSendPacketBytes() const	{ return m_nSendPacketBytes; };

	NDSocket*				getSocket();
	SOCKET					getSOCKET() const;
	const char*				getPeerIP() const;			
	NDUint16				getPeerPort() const;
	NDBool					isValidAddr() const;
	NDBool					isConnected() const;

	void					setDisconnectionType( NDUint8 nDisconnectionType )	{ m_nDisconnectionType = nDisconnectionType; }
	NDUint8					getDisconnectionType() const						{ return m_nDisconnectionType; }

	void					setProtocolType( NDUint8 nProtocolType )	{ m_nProtocolType = nProtocolType; };
	NDUint8					getProtocolType() const						{ return m_nProtocolType; }


	NDBool					sendData( const NDByteBuffer& refMsgBuf );
	NDBool					sendData( const char* pszBuf, NDUint32 nSize );

	NDBool					handleRead();
	NDBool					handleWrite();
	NDBool					handleClose();
	
	static	void			setParsePacketFun( PParsePacketFun pParsePacketFun );
protected:
	NDBool					setSocketOptions();
	NDBool					shutdownSendPipe();

	NDBool					parseRecvData();



private:
	//优化事件,在需要发送的时候才注册WRITE_EVENT;
	NDBool					registerWriteEvent();
	NDBool					unregisterWriteEvent();
};



_NDSHAREBASE_END
#endif

