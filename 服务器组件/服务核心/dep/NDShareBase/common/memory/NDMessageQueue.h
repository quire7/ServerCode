/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\NDMessageQueue.h
	file base:	NDMessageQueue
	purpose:	message queue;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_MESSAGE_QUEUE_H__
#define __SHARE_BASE_ND_MESSAGE_QUEUE_H__

#include <deque>
using std::deque;

#include "thread/NDCondition.h"

_NDSHAREBASE_BEGIN

template<typename T>
class NDMessageQueue
{
public:
	typedef deque<T>		TMessageDeque;
public:
	NDMessageQueue() : m_bchange( NDTrue ), m_cur_num( 0 ), m_condition()
	{
		clear();
	}

	~NDMessageQueue()
	{
		clear();
	}

	inline void push( T element )
	{
		m_condition.beginSynchronized();

		if ( NDTrue == m_bchange )
		{
			m_mostly_deque.push_back( element );
		}
		else
		{
			m_spare_deque.push_back( element );
		}
		++m_cur_num;

		//多线程调用函数pop()或者get_deque()函数(他们都调用了m_condition.wait())时
		//这个地方用m_condition.broadcast(). 单线程调用时最好用m_condition.signal();
		m_condition.broadcast();

		m_condition.endSynchronized();
	}

	inline NDBool pop_nowait( T& r_element )
	{
		m_condition.beginSynchronized();

		if (m_cur_num == 0)
		{
			m_condition.endSynchronized();
			return NDFalse;
		}

		TMessageDeque *pDeque = NULL;
		if ( NDTrue == m_bchange)
		{
			pDeque = &m_mostly_deque;
		}
		else
		{
			pDeque = &m_spare_deque;
		}

		r_element = pDeque->front();
		pDeque->pop_front();

		--m_cur_num;

		m_condition.endSynchronized();

		return NDTrue;
	}

	inline NDBool pop( T& r_element )
	{
		m_condition.beginSynchronized();

		if ( m_cur_num == 0 )
		{
			m_condition.wait();
		}

		TMessageDeque *pDeque = NULL;
		if ( NDTrue == m_bchange)
		{
			pDeque = &m_mostly_deque;
		}
		else
		{
			pDeque = &m_spare_deque;
		}

		r_element = pDeque->front();
		pDeque->pop_front();

		--m_cur_num;

		m_condition.endSynchronized();

		return NDTrue;
	}

	inline NDBool get_deque_nowait( TMessageDeque*& pDeque )
	{
		m_condition.beginSynchronized();

		if (m_cur_num == 0)
		{
			m_condition.endSynchronized();
			return NDFalse;
		}

		if (NDTrue == m_bchange)
		{
			pDeque = &m_mostly_deque;
			m_bchange = NDFalse;
		}
		else
		{
			pDeque = &m_spare_deque;
			m_bchange = NDTrue;
		}

		m_cur_num = 0;

		m_condition.endSynchronized();

		return NDTrue;
	}

	inline NDBool get_deque( TMessageDeque*& pDeque )
	{
		m_condition.beginSynchronized();

		if (m_cur_num == 0)
		{
			m_condition.wait();
		}

		if (NDTrue == m_bchange)
		{
			pDeque = &m_mostly_deque;
			m_bchange = NDFalse;
		}
		else
		{
			pDeque = &m_spare_deque;
			m_bchange = NDTrue;
		}

		m_cur_num = 0;

		m_condition.endSynchronized();

		return NDTrue;
	}


protected:
	inline void clear()
	{
		m_mostly_deque.clear();
		m_spare_deque.clear();
	}

private:
	NDBool					m_bchange;
	NDUint32				m_cur_num;

	NDCondition				m_condition;

	TMessageDeque			m_mostly_deque;
	TMessageDeque			m_spare_deque;
};

_NDSHAREBASE_END

#endif


