/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\object\NDObjectPool.h
	file base:	NDObjectPool
	purpose:	object pool;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_OBJECT_POOL_H__
#define __SHARE_BASE_ND_OBJECT_POOL_H__

#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "thread/NDMutex.h"
#include "function/NDDistributeID.h"

_NDSHAREBASE_BEGIN

template< typename T >
class NDObjectPool
{

private:
	NDDistributeID		m_distributeID;
	NDMutexLock			m_Mutex;

	T**					m_pObjs;

public:
	NDObjectPool()
	{
		m_pObjs = NULL;
	}

	~NDObjectPool()
	{
		clear();
	}

	void clear()
	{
		if ( NULL != m_pObjs )
		{
			NDUint32 nTotalSize = m_distributeID.getTotalSize();
			for ( NDUint32 i = 0; i < nTotalSize; ++i )
			{
				if ( NULL != m_pObjs[i] )
				{
					delete m_pObjs[i];
					m_pObjs[i] = NULL;
				}
			}
			delete [] m_pObjs;
			m_pObjs = NULL;
		}
	}

	NDBool		init( NDUint32 nMaxSize )
	{
		if ( nMaxSize == 0 )
		{
			return NDFalse;
		}

		m_distributeID.resetSize( nMaxSize );

		m_pObjs = new T* [nMaxSize];
		for ( NDUint32 i = 0; i < nMaxSize; ++i )
		{
			m_pObjs[i] = new T;
			if ( NULL == m_pObjs[i] )
			{
				return NDFalse;
			}
		}

		return NDTrue;
	}

	T*			createObj()
	{
		NDGuardLock locker( m_Mutex );
		if ( 0 == m_distributeID.getIdleSize() )
		{
			if ( NDFalse == resetSize( m_distributeID.getTotalSize() * 2 ) )
			{
				return NULL;
			}
		}

		NDUint32 nPoolID = m_distributeID.createID();
		T* pObj = m_pObjs[nPoolID];
		pObj->setPoolID( nPoolID );

		return pObj;
	}

	NDBool		destroyObj( T* pObj )
	{
		if ( NULL == pObj )
		{
			return NDFalse;
		}

		NDGuardLock locker( m_Mutex );
		NDUint32 nPoolID = pObj->getPoolID();
		if ( nPoolID == (NDUint32)ND_INVALID_ID )
		{
			//ND_INVALID_ID为所有OBJ的默认值
			return NDFalse;
		}
		if ( nPoolID >= m_distributeID.getTotalSize() )
		{
			return NDFalse;
		}

		m_distributeID.releaseID( nPoolID );

		T* pDelObj = m_pObjs[nPoolID];
		pDelObj->setPoolID( (NDUint32)ND_INVALID_ID );

		return NDTrue;
	}


	NDUint32	getTotalSize() const
	{
		return m_distributeID.getTotalSize();
	}

	NDUint32	getAliveSize() const
	{
		return m_distributeID.getAliveSize();
	}

	NDUint32	getIdleSize()  const
	{
		return m_distributeID.getIdleSize();
	}


	NDBool resetSize( NDUint32 nSize )
	{
		T** pObjsSave = m_pObjs;

		m_pObjs = new T* [nSize];
		if ( NULL == m_pObjs )
		{
			return NDFalse;
		}
		memset( m_pObjs, 0, sizeof(T*) * nSize );

		NDUint32 nLastMaxSize = m_distributeID.getTotalSize();
		if ( NULL != pObjsSave )
		{
			memcpy( m_pObjs, pObjsSave, sizeof(T*) * nLastMaxSize );

			delete []pObjsSave;
			pObjsSave = NULL;
		}

		for ( ; nLastMaxSize < nSize; ++nLastMaxSize )
		{
			m_pObjs[nLastMaxSize] = new T;
			if ( NULL == m_pObjs[nLastMaxSize] )
			{
				return NDFalse;
			}
		}

		m_distributeID.resetSize( nSize ); 
		return NDTrue;
	}

};


_NDSHAREBASE_END
#endif //__SHARE_BASE_ND_OBJECT_POOL_H__

