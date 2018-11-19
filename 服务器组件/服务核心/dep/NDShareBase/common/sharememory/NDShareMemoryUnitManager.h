/********************************************************************
	created:	2014/10/03
	filename: 	d:\code\NDServer2\NDShareBase\common\sharememory\NDShareMemoryUnitPool.h
	file base:	NDShareMemoryUnitPool
	purpose:	sharememory unit pool;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com
	qq:			435337751;
*********************************************************************/
#ifndef	__SHARE_BASE_ND_SHARE_MEMORY_UNIT_MANAGER_H__
#define __SHARE_BASE_ND_SHARE_MEMORY_UNIT_MANAGER_H__


#include "NDShareMemoryAccessObject.h"
#include "NDShareBaseGlobal.h"

_NDSHAREBASE_BEGIN


template<typename T>
class NDShareMemoryUnitPool
{
private:
	NDUint16					m_nOwnPoolType;		//所有者类型;
	NDBool						m_bAlive;			//是不是存活的;
	NDShareMemoryAccessObject*	m_pAccessObjPtr;	//SMO对象的引用;
	T						  **m_pObjs;			//SMU对象数组;

public:
	NDShareMemoryUnitPool()
	{
		m_nOwnPoolType	= 0;
		m_bAlive		= NDFalse;
		m_pAccessObjPtr	= NULL;
		m_pObjs			= NULL;
	}

	~NDShareMemoryUnitPool()
	{
		m_bAlive = NDFalse;

		if ( NULL != m_pAccessObjPtr )
		{
			delete m_pAccessObjPtr;
			m_pAccessObjPtr = NULL;
		}
		if ( NULL != m_pObjs )
		{
			delete []m_pObjs;
			m_pObjs = NULL;
		}
	}

	NDBool		init( NDSM_KEY nKey, NDUint32 nUnitMax, NDUint16 nPoolType )
	{
		if ( nPoolType <= 0 )
		{
			return NDFalse;
		}

		m_pAccessObjPtr = new NDShareMemoryAccessObject;
		if ( NULL == m_pAccessObjPtr )
		{
			return NDFalse;
		}

		NDUint32 nTotalBufSize = sizeof(T) * nUnitMax + sizeof(NDSMHead);
		if ( eNDSMU_OWN_TYPE_SELF == nPoolType )
		{
			if ( NDFalse == m_pAccessObjPtr->create( nKey, nTotalBufSize ) )
			{
				return NDFalse;
			}

			m_pAccessObjPtr->setUnitCapacity( nUnitMax );
		}
		else
		{
			if ( NDFalse == m_pAccessObjPtr->attach( nKey, nTotalBufSize ) )
			{
				return NDFalse;
			}
		}

		
		m_nOwnPoolType	= nPoolType;
		m_pObjs			= new T* [nUnitMax];

		NDUint32 nTSize = sizeof(T);
		for ( NDUint32 i = 0; i < nUnitMax; ++i )
		{
			m_pObjs[i] = reinterpret_cast<T*>( m_pAccessObjPtr->getTypePtr( nTSize, i ) );
			if ( NULL == m_pObjs[i] )
			{
				return NDFalse;
			}
		}
		
		m_bAlive = NDTrue;

		return NDTrue;
	}

	T*			createObj()
	{
		if ( NDFalse == m_bAlive )
		{
			return NULL;
		}

		NDUint32 nCapacity = getPoolCapacity();
		NDUint32 nPosition = getPoolSize();
		if ( nPosition >= nCapacity )
		{
			return NULL;
		}

		if ( NDFalse == m_pAccessObjPtr->tryLockHead( m_nOwnPoolType ) )
		{
			return NULL;
		}
		nPosition = getPoolSize();

		T* pObj = m_pObjs[nPosition];
		pObj->setPoolID( nPosition );
		pObj->setSaveTime( NDShareBaseGlobal::getCurSecondTimeOfUTC() );
		pObj->tryLock( m_nOwnPoolType );
		++nPosition;
		
		m_pAccessObjPtr->setUnitSize( nPosition );

		m_pAccessObjPtr->tryUnLockHead( m_nOwnPoolType );

		return pObj;
	}

	NDBool		destroyObj( T *pObj )
	{
		if ( NDFalse == m_bAlive )
		{
			return NDFalse;
		}

		NDUint32 nPosition = getPoolSize();
		if ( nPosition == 0 )
		{
			return NDTrue;
		}
		NDUint32 nDelIndex = pObj->getPoolID();
		if ( nDelIndex >= nPosition )
		{
			return NDTrue;
		}

		if ( NDFalse == m_pAccessObjPtr->tryLockHead( m_nOwnPoolType ) )
		{
			return NDFalse;
		}
		nPosition = getPoolSize();

		--nPosition;
		T *pDelObj			= m_pObjs[nDelIndex];
		m_pObjs[nDelIndex]	= m_pObjs[nPosition];
		m_pObjs[nPosition]	= pDelObj;

		m_pObjs[nDelIndex]->setPoolID( nDelIndex );
		m_pObjs[nPosition]->setPoolID( (NDUint32)ND_INVALID_ID );
		m_pObjs[nPosition]->setSaveTime( 0 );
		m_pObjs[nPosition]->tryUnlock( m_nOwnPoolType );

		m_pAccessObjPtr->setUnitSize( nPosition );

		m_pAccessObjPtr->tryUnLockHead( m_nOwnPoolType );

		return NDTrue;
	}

	T*			getPoolObj( NDUint32 nIndex )
	{
		if ( NDFalse == m_bAlive )
		{
			return NULL;
		}

		NDUint32 nCapacity = getPoolCapacity();
		if ( nIndex >= nCapacity )
		{
			return NULL;
		}

		return m_pObjs[nIndex];
	}

	NDUint32	getPoolCapacity() const
	{
		if ( NDFalse == m_bAlive )
		{
			return 0;
		}
		return m_pAccessObjPtr->getUnitCapacity();
	}

	NDUint32	getPoolSize() const
	{
		if ( NDFalse == m_bAlive )
		{
			return 0;
		}
		return m_pAccessObjPtr->getUnitSize();
	}

	//ShareMemory的KEY;
	NDSM_KEY	getKey() const
	{
		if ( NDFalse == m_bAlive )
		{
			return 0;
		}
		return m_pAccessObjPtr->getKey();
	}

	NDBool		dumpToFile( const char* szFilePath )
	{
		if ( NULL == szFilePath || '\0' == szFilePath[0] || NULL == m_pAccessObjPtr )
		{
			return NDFalse;
		}

		return m_pAccessObjPtr->dumpToFile( szFilePath );
	}

	NDBool		mergeFromFile( const char* szFilePath )
	{
		if ( NULL == szFilePath || '\0' == szFilePath[0] || NULL == m_pAccessObjPtr )
		{
			return NDFalse;
		}

		return m_pAccessObjPtr->mergeFromFile( szFilePath );
	}

	NDUint32	getHeadVersion() const
	{
		if ( NDFalse == m_bAlive )
		{
			return 0;
		}

		return m_pAccessObjPtr->getHeadVersion();
	}

	void		setHeadVersion( NDUint32 nVersion )
	{
		if ( NDFalse == m_bAlive )
		{
			return 0;
		}

		return m_pAccessObjPtr->setHeadVersion( nVersion );
	}
};

_NDSHAREBASE_END
#endif


