/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDMutex.h
	file base:	NDMutex
	purpose:	mutex;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_MUTEX_H__
#define __SHARE_BASE_ND_MUTEX_H__

#include "NDTypes.h"
#include "thread/NDAtomicInt32.h"

_NDSHAREBASE_BEGIN

class NDSysLock
{
public:
	NDSysLock(){};
	virtual ~NDSysLock(){};

	virtual void	lock()	=0;
	virtual void	unlock()=0;
};


class NDSpinlock : public NDSysLock
{
private:
	NDAtomicInt32	m_nAtomicInt32;
	
public:
	NDSpinlock() : m_nAtomicInt32(0){};
	~NDSpinlock(){};
	void lock()
	{
		while( m_nAtomicInt32.compareAndSet(0, 1) == 1 )
		{
#ifdef WIN32
			Sleep(0);
#else
			usleep(0);
#endif
		}
	}
	void unlock()
	{
		m_nAtomicInt32.compareAndSet( 1, 0 );
	}
};

class NDMutexLock : public NDSysLock
{
private:
	NDSysLock*		m_pMutexLock;

public:
	NDMutexLock();
	~NDMutexLock();

	void lock();
	void unlock();
};


class NDGuardLock
{
private:
	NDSysLock*	m_pNDSysLock;

private:
	NDGuardLock( const NDGuardLock& refConstGuardlock );
	NDGuardLock& operator=( const NDGuardLock& refOther );

public:
	NDGuardLock( NDSysLock& refSysLock ) : m_pNDSysLock( &refSysLock )
	{
		m_pNDSysLock->lock();
	}
	~NDGuardLock()
	{
		m_pNDSysLock->unlock();
	}
};

_NDSHAREBASE_END
#endif

