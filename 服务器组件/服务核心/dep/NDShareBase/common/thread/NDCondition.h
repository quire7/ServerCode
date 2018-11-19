/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDCondition.h
	file base:	NDCondition
	purpose:	condition;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CONDITION_H__
#define __SHARE_BASE_ND_CONDITION_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDConditionImpl;
class NDCondition
{
private:
	NDConditionImpl*		m_pNDConditionImpl;
public:
	explicit NDCondition();
	~NDCondition();

	void		beginSynchronized();
	void		endSynchronized();

	NDBool		wait();
	NDBool		wait( NDUint32 nMilliSecondsTimeOut );

	void		signal();
	void		broadcast();
};


_NDSHAREBASE_END
#endif

