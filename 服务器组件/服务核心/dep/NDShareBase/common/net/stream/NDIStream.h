/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\stream\NDIStream.h
	file base:	NDIStream
	purpose:	input stream;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_ISTREAM_H__
#define __SHARE_BASE_ND_ISTREAM_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDIStream
{
public:
	virtual ~NDIStream() {}
	virtual NDBool		read( void* pChar, NDUint32 nSize ) = 0;
	virtual NDUint32	getTotalSize() const = 0;
};

#ifndef NDISTREAM_READ
#define NDISTREAM_READ( stream, pvoid, size )\
		if ( NDFalse == stream.read( (void*)(pvoid), size ) )\
		{\
			return NDFalse;\
		}
#endif

_NDSHAREBASE_END

#endif

