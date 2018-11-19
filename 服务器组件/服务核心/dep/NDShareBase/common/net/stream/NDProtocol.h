/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\stream\NDProtocol.h
	file base:	NDProtocol
	purpose:	protocol base class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_PROTOCOL_H__
#define __SHARE_BASE_ND_PROTOCOL_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDOStream;
class NDIStream;

class NDProtocol
{
public:
	const NDUint16 m_unProtocolID;
public:
	NDProtocol( NDUint16 unProtocolID ) : m_unProtocolID( unProtocolID ) {}

	virtual ~NDProtocol(){}
	virtual NDBool		serialize( NDOStream& rOStream ) = 0;
	virtual NDBool		deserialize( NDIStream& rIStream ) = 0;

	virtual NDUint16	getSize() const = 0;					// 定长协议为固定大小,变长为最大大小;
	virtual void		clear() {};								// whether must have or not;

private:
	NDProtocol(const NDProtocol& other);
	NDProtocol& operator=(const NDProtocol& other);
};


_NDSHAREBASE_END
#endif
