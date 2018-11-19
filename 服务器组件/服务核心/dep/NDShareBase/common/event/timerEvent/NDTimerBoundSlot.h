/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\timerEvent\NDTimerBoundSlot.h
	file base:	NDTimerBoundSlot
	purpose:	timer event bound slot;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_TIMER_BOUND_SLOT_H__
#define __SHARE_BASE_ND_TIMER_BOUND_SLOT_H__

#include "NDShareBaseEnums.h"

#include "event/functor/NDEventArgs.h"
#include "function/NDRefCounted.h"

_NDSHAREBASE_BEGIN

class NDTimerEventArgs : public NDEventArgs
{
public:
	NDUint8		m_nFlag;				//Timer事件的状态;
	const char*	m_szName;				//Timer事件的名称;
	NDUint64	m_nStepMSTimes;			//每多少毫秒执行一次Timer事件;(0:一次性事件,m_nCurProMSTimes执行完销毁;大于0是循环事件);
	NDUint64	m_nProcessMSTimes;		//当前执行Timer事件具体时间;
public:
	NDTimerEventArgs( const char* szName, NDUint64 nStepMSTimes, NDUint64 nCurProMSTimes ):
	  m_nFlag( E_NDSlotFunctorBase_RUN ), m_szName( szName ), m_nStepMSTimes( nStepMSTimes ), m_nProcessMSTimes( nCurProMSTimes ) {}
	  NDTimerEventArgs( const NDTimerEventArgs& other ) : 
	  m_nFlag( other.m_nFlag ), m_szName( other.m_szName ), m_nStepMSTimes( other.m_nStepMSTimes ), m_nProcessMSTimes( other.m_nProcessMSTimes ) {}
};

class NDSubFunctorSlot;
class NDTimerBoundSlot
{
private:
	NDSubFunctorSlot*		m_pSubscriber;		///< 订阅者指针;
	NDTimerEventArgs		m_NDTimerEventArgs;	///< 订阅者参数;

public:
	NDTimerBoundSlot( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs  );
	NDTimerBoundSlot( const NDTimerBoundSlot& other );
	~NDTimerBoundSlot();

	///是否已经连接;
	NDBool	connected() const;
	///断开连接;
	void	disconnect();

	//是否是循环事件;
	NDBool	isLoopTimerEvent() const;

	//修改循环事件下次执行的时间;
	void	modifyFireEventTimes();

	//获得事件执行的时间;
	NDTime	getFireEventTimes() const;

	//获得执行事件的时间间隔;
	NDTime	getFireEventIntervalTimes() const;

	//获得执行事件的名称;
	const char* getFireEventName() const;

	//执行超时的事件;
	NDBool	fireTimerEvent();

	///判断实际上的订阅者对象是否是同一个;
	bool	operator==( const NDTimerBoundSlot& other ) const;
	bool	operator!=( const NDTimerBoundSlot& other ) const;

private:
	NDTimerBoundSlot& operator=( const NDTimerBoundSlot& other );
};

typedef NDRefCounted<NDTimerBoundSlot>		NDTimerBoundSlotConn;


_NDSHAREBASE_END

#endif

