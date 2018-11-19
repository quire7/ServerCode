/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\function\NDSingleton.h
	file base:	NDSingleton
	purpose:	singleton pattern;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SINGLETON_H__
#define __SHARE_BASE_ND_SINGLETON_H__


#include <cassert>

_NDSHAREBASE_BEGIN

template<typename T>
class NDSingleton
{
public:
	NDSingleton()
	{	
		/// If you hit this assert, this singleton already exists -- you can't create another one!;
		assert( m_pSingleton == 0 );
		m_pSingleton = static_cast<T*>(this);
	}

	virtual ~NDSingleton() { m_pSingleton = 0; }

	static T& getSingleton()	{ assert( m_pSingleton ); return (*m_pSingleton); }
	static T* getSingletonPtr()	{ return ( m_pSingleton ); }

protected:
	/// Singleton pointer, must be set to 0 prior to creating the object;
	static T*	m_pSingleton;
};

/// Should be placed in the appropriate .cpp file somewhere;
#define initialiseSingleton( type ) \
	template <> type* NDSingleton < type >::m_pSingleton = 0


_NDSHAREBASE_END

#endif
