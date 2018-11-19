
/********************************************************************
	created:	2011/09/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\object\NDMemoryPoolExManager.h
	file base:	NDMemoryPoolExManager
	purpose:	可扩展的对象内存池管理器;
	author:		chenbenqian,fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/

#ifndef __SHARE_BASE_ND_MEMORYPOOL_EX_MANAGER_H__
#define __SHARE_BASE_ND_MEMORYPOOL_EX_MANAGER_H__

#include <vector>

#include "thread/NDMutex.h"
#include "event/timerEvent/NDTimerBoundSlot.h"

#include "NDMemoryPoolEx.h"



_NDSHAREBASE_BEGIN

class NDMemoryPoolExManager
{
private:
	NDMutexLock							m_Lock;					// 内存池数组锁;
	NDTimerBoundSlotConn				m_timerPrintMemInfoConn;// PrintMemInfo定时器的关联器;
	std::vector<NDMemoryPoolBase*>		m_MemoryPoolVec;		// 内存池数组;

	
	static	const char*					s_szTimerPrintMemInfo;
	static	NDMemoryPoolExManager*		s_pNDMemoryPoolExManager;
public:
	~NDMemoryPoolExManager();

	static	NDMemoryPoolExManager*	getInstance();
	static	void					releaseInstance();

	//创建内存池;
	template<typename T>
	NDBool	createMemoryPool( NDUint32 nMinNum = NDMEMORYPOOLSIZE_MIN, NDUint32 nMaxNum = NDMEMORYPOOLSIZE_MAX, NDUint32 nIncreNum = NDMEMORYPOOLSIZE_INCRE );

	//初始化(主要是把printMemInfo加入定时器中);
	NDBool	init();

	//清除;
	void	release();

	//获得一块指定类型的内存;
	template<typename T>
	T*		mallocMemory( T* &pData );

	//回收一块内存;
	template<typename T>
	NDBool	freeMemory( T* pData );

private:
	//定时打印内存当前信息;
	NDBool	printMemInfo( const NDEventArgs& );

private:
	NDMemoryPoolExManager();
	NDMemoryPoolExManager( const NDMemoryPoolExManager& other );
	NDMemoryPoolExManager& operator=( const NDMemoryPoolExManager& other );
};

template<typename T>
NDBool NDMemoryPoolExManager::createMemoryPool( NDUint32 nMinNum, NDUint32 nMaxNum, NDUint32 nIncreNum )
{
	NDMemoryPoolBase* pMemoryPoolBase = new NDMemoryPoolEx<T>;
	if ( pMemoryPoolBase )
	{
		pMemoryPoolBase->m_MemPrepay.nMinNum = nMinNum;
		pMemoryPoolBase->m_MemPrepay.nMaxNum = nMaxNum;
		pMemoryPoolBase->m_MemPrepay.nIncreNum = nIncreNum;
		m_MemoryPoolVec.push_back(pMemoryPoolBase);
		return NDTrue;
	}
	return NDFalse;
}

template<typename T>
T* NDMemoryPoolExManager::mallocMemory( T* &pData )
{
	NDGuardLock Lock(m_Lock);

	std::vector<NDMemoryPoolBase*>::iterator itBegin	= m_MemoryPoolVec.begin();
	std::vector<NDMemoryPoolBase*>::iterator itEnd		= m_MemoryPoolVec.end();

	for ( ; itBegin != itEnd; ++itBegin )
	{
		if ( (*itBegin)->type() == typeid(T) )
		{
			pData = (T*)((*itBegin)->mallocMemory());
			return pData;
		}
	}


	NDMemoryPoolEx<T>* pMemoryPool = new NDMemoryPoolEx<T>;
	if ( pMemoryPool == NULL )
	{
		return NULL;
	}
	m_MemoryPoolVec.push_back(pMemoryPool);
	if( !pMemoryPool->init() )
	{
		return NULL;
	}
	pData = (T*)(pMemoryPool->mallocMemory());
	return pData;
}

template<typename T>
NDBool NDMemoryPoolExManager::freeMemory( T* pData )
{
	if ( NULL == pData )
	{
		return NDFalse;
	}
	

	NDGuardLock	Lock(m_Lock);


	std::vector<NDMemoryPoolBase*>::iterator itBegin	= m_MemoryPoolVec.begin();
	std::vector<NDMemoryPoolBase*>::iterator itEnd		= m_MemoryPoolVec.end();
	for ( ; itBegin != itEnd; ++itBegin )
	{
		if ( (*itBegin) != NULL && (*itBegin)->type() == typeid(*pData) )
		{
			return ( (*itBegin)->freeMemory((void*)pData) );
		}
	}

	return NDFalse;
}

_NDSHAREBASE_END
//*****************************************************************************
#endif//__SHARE_BASE_ND_MEMORYPOOL_EX_MANAGER_H__

