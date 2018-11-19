/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\function\NDDistributeID.h
	file base:	NDDistributeID
	purpose:	Distribute enable ID;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_DISTRIBUTE_ID_H__
#define __SHARE_BASE_ND_DISTRIBUTE_ID_H__

#include <deque>
using std::deque;

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDDistributeID
{
private:
	NDUint32			m_nTotalSize;			//�ܷ�����;
	NDUint32			m_nCurPos;				//�α��λ�ô�0��ʼ;
	NDUint32			m_nCount;				//�Ѿ�����ĸ���;
	deque<NDUint32>		m_idleDeque;			//�ͷŵ�ID;

public:
	NDDistributeID( NDUint32 nTotalSize = 10000 );
	~NDDistributeID();
	
	NDUint32	createID();
	void		releaseID( NDUint32 nIndex );
	void		resetSize( NDUint32 nTotalSize );

	NDUint32	getTotalSize() const;
	NDUint32	getAliveSize() const;
	NDUint32	getIdleSize()  const;

private:
	NDDistributeID( const NDDistributeID& other );
	NDDistributeID& operator = ( const NDDistributeID& other );

};


_NDSHAREBASE_END
#endif // __SHARE_BASE_ND_DISTRIBUTE_ID_H__
