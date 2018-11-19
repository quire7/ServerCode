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
	NDUint8		m_nFlag;				//Timer�¼���״̬;
	const char*	m_szName;				//Timer�¼�������;
	NDUint64	m_nStepMSTimes;			//ÿ���ٺ���ִ��һ��Timer�¼�;(0:һ�����¼�,m_nCurProMSTimesִ��������;����0��ѭ���¼�);
	NDUint64	m_nProcessMSTimes;		//��ǰִ��Timer�¼�����ʱ��;
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
	NDSubFunctorSlot*		m_pSubscriber;		///< ������ָ��;
	NDTimerEventArgs		m_NDTimerEventArgs;	///< �����߲���;

public:
	NDTimerBoundSlot( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs  );
	NDTimerBoundSlot( const NDTimerBoundSlot& other );
	~NDTimerBoundSlot();

	///�Ƿ��Ѿ�����;
	NDBool	connected() const;
	///�Ͽ�����;
	void	disconnect();

	//�Ƿ���ѭ���¼�;
	NDBool	isLoopTimerEvent() const;

	//�޸�ѭ���¼��´�ִ�е�ʱ��;
	void	modifyFireEventTimes();

	//����¼�ִ�е�ʱ��;
	NDTime	getFireEventTimes() const;

	//���ִ���¼���ʱ����;
	NDTime	getFireEventIntervalTimes() const;

	//���ִ���¼�������;
	const char* getFireEventName() const;

	//ִ�г�ʱ���¼�;
	NDBool	fireTimerEvent();

	///�ж�ʵ���ϵĶ����߶����Ƿ���ͬһ��;
	bool	operator==( const NDTimerBoundSlot& other ) const;
	bool	operator!=( const NDTimerBoundSlot& other ) const;

private:
	NDTimerBoundSlot& operator=( const NDTimerBoundSlot& other );
};

typedef NDRefCounted<NDTimerBoundSlot>		NDTimerBoundSlotConn;


_NDSHAREBASE_END

#endif

