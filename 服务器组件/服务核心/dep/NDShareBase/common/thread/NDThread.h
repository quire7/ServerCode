/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDThread.h
	file base:	NDThread
	purpose:	thread class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_THREAD_H__
#define __SHARE_BASE_ND_THREAD_H__

#include "NDTypes.h"
#include "NDShareBaseEnums.h"
#include "NDShareBaseMacros.h"

#ifdef WIN32
#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <process.h>
#elif defined(LINUX)
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#endif

_NDSHAREBASE_BEGIN

//不要改成PIMPL模式,因为static函数的问题吧;
class NDThread
{
private:
	ThreadID			m_nThreadID;
	EThreadStatus		m_eThreadStatus;
	char				m_szName[ND_THREAD_NAME_MAX];

#ifdef WIN32
	HANDLE				m_hThread;
#endif

public:
	NDThread();
	virtual ~NDThread();
	
	void	start();
	void	exit( void* pRetVal );

	virtual	void	run();
	virtual void	stop();

	ThreadID		getThreadID() const					{ return m_nThreadID; };

	EThreadStatus	getStatus() const					{ return m_eThreadStatus; };
	void			setStatus( EThreadStatus eStatus )	{ m_eThreadStatus = eStatus; };

	const char*		getName() const						{ return m_szName; };
	void			setName( const char* szName );


#ifdef WIN32
	static	unsigned int WINAPI myThreadProcess( void* pDerivedThread );
#elif defined(LINUX)
	static	void*	myThreadProcess( void* pDerivedThread );
#endif

};

_NDSHAREBASE_END
#endif

