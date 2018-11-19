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
	NDUint32				m_nSessionID;				//�Ự��Ψһ��־ID;
	NDUint32				m_nSocketSendBuf;			//m_pSocket�ķ��ͻ�������С;

	NDUint8					m_nDisconnectionType;		//�Ͽ���������(�����������ҷ������Ͽ����ӻ��ǶԷ������Ͽ�����);
	NDUint8					m_nProtocolType;			//sessionҪ�����Э������;
	NDUint8					m_nEventFlag;				//�¼���־(EIOEventType);
	NDBool					m_bAlive;					//�Ự�Ƿ����־;
	NDBool					m_bSendPipe;				//���͹ܵ��Ƿ����;

	NDUint32				m_nRecvPacketNum;			//��¼������Ϣ��������;
	NDUint32				m_nSendPacketNum;			//��¼������Ϣ��������;
	NDUint32				m_nRecvPacketBytes;			//��¼������Ϣ�����ֽ���;
	NDUint32				m_nSendPacketBytes;			//��¼������Ϣ�����ֽ���;


	NDByteBuffer*			m_pSendBuffer;				//���ͻ�����;
	NDByteBuffer*			m_pRecvBuffer;				//���ջ�����;
	NDSysLock*				m_pSendDataMutex;			//���ͻ�������;
	NDSysLock*				m_pRecvDataMutex;			//���ջ�������;

	static PParsePacketFun	s_pParsePacket;				//�������ĺ���;

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
	//�Ż��¼�,����Ҫ���͵�ʱ���ע��WRITE_EVENT;
	NDBool					registerWriteEvent();
	NDBool					unregisterWriteEvent();
};



_NDSHAREBASE_END
#endif

