/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\stream\NDProtocolPacket.h
	file base:	NDProtocolPacket
	purpose:	protocol packet;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef	__SHARE_BASE_ND_PROTOCOL_PACKET_H__
#define __SHARE_BASE_ND_PROTOCOL_PACKET_H__


#include "NDTypes.h"
#include "authentic/NDCrypt.h"

#include <vector>
using std::vector;

_NDSHAREBASE_BEGIN

#ifndef ND_PDHMSG
#define ND_PDHMSG
#define ND_PDHMSG_ENCRYPT		0X0001		//data encrypt;
#define ND_PDHMSG_COMPRESSION	0X0002		//data compression;
#define ND_PDHMSG_CRC32			0X0004		//CRC CODE;
#define ND_PDHMSG_PROTO_NUM		0X0008		//protocol number;
#endif

struct NDProDataHead
{
	NDUint16	m_nBitWise;					//composition bit;
	NDUint16	m_nBodySize;				//protocol body size;
	NDUint16	m_nProtocolID;				//protocol ID;
};

class NDByteBuffer;
class NDProtocol;

class NDProtocolPacket
{
private:
	static NDCrypt				s_encrypt;

	static NDUint8				s_nMaxSessionType;
	static NDUint16				s_specialProtocolArray[2];		//存储特殊处理协议的协议号起始ID;
	static vector<NDUint16>		s_protocolNumVec;				//存储某种协议类型要处理的协议号起始ID;

public:
	static void		setEncrptKey( const NDUint8* szKey, NDUint32 nLen );
	static NDBool	composePacket( NDByteBuffer& outMsgBuf, NDProtocol& protocol, NDUint16 nProDataHeadBitWise );
	static NDBool	parsePacket( NDByteBuffer& decryptBuf, NDByteBuffer& encryptBuf, const NDProDataHead& dataHeader );
	static NDBool	parsePacket( NDByteBuffer& recvMsgOriginalBuf, NDParseSessionDataEx& refNDParseSessionDataEx );

	static void		setMaxSessionType( NDUint8 nMaxSessionType );
	static void		setSpecialProtocol( NDUint16 nSpecialProtocolStart, NDUint16 nSpecialProtocolEnd );
	static NDBool	setDisposeSessionProtocol( NDUint8 nSessionType, NDUint16 nProtocolStart, NDUint16 nProtocolEnd );
private:
	//解析包并放入处理队列;
	static NDBool	parsePacketAndPutQueue( NDByteBuffer* pParseBuffer, const NDProDataHead& refDataHeader );

	//判断协议ID是否是这条Session(链路)的协议;
	static NDBool	isSessionProtocolID( NDUint8 nSessionProtocolType, NDUint16 nProtocolID );

};

_NDSHAREBASE_END
#endif

