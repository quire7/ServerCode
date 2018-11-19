/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\NDByteBufferPool.h
	file base:	NDByteBufferPool
	purpose:	NDByteBuffer Pool;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_BYTE_BUFFER_POOL_H__
#define __SHARE_BASE_ND_BYTE_BUFFER_POOL_H__

#include "NDTypes.h"
#include "memory/object/NDObjectPool.h"
#include "NDByteBuffer.h"


_NDSHAREBASE_BEGIN

class NDByteBufferPool
{
private:
	NDObjectPool<NDByteBuffer>	m_NDByteBufferObjPool;
	static NDByteBufferPool*	m_spNDByteBufferPool;
public:
	static NDByteBufferPool*	getInstance();
	static void					releaseInstance();

	~NDByteBufferPool(void);

	NDByteBuffer*				createByteBuffer();
	NDBool						destroyByteBuffer( NDByteBuffer* pByteBuffer );

private:
	NDByteBufferPool();
	NDByteBufferPool(const NDByteBufferPool& other);
	NDByteBufferPool& operator = (const NDByteBufferPool& other);
};

_NDSHAREBASE_END
#endif
