/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\stream\NDOStream.h
	file base:	NDOStream
	purpose:	output stream;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_OSTREAM_H__
#define __SHARE_BASE_ND_OSTREAM_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDOStream
{
public:
	virtual ~NDOStream() {}
	virtual NDBool		write( const void* pChar, NDUint32 nSize ) = 0;
	virtual NDUint32	getTotalSize() const = 0;
};

#ifndef NDOSTREAM_WRITE
#define NDOSTREAM_WRITE( stream, pvoid, size )\
	if ( NDFalse == stream.write( (const void*)(pvoid), size ) )\
	{\
		return NDFalse;\
	}
#endif

_NDSHAREBASE_END

#endif

