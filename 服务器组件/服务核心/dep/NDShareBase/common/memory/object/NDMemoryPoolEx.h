
/********************************************************************
	created:	2011/09/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\object\NDMemoryPoolEx.h
	file base:	NDMemoryPoolEx
	purpose:	����չ�Ķ����ڴ��;
	author:		chenbenqian,fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/

#ifndef __SHARE_BASE_ND_MEMORYPOOL_EX_H__
#define __SHARE_BASE_ND_MEMORYPOOL_EX_H__

#include "NDtypes.h"

#include <new>
#include <list>
#include <map>
#include <typeinfo>

#include "NDCLibFun.h"
#include "file/NDLog.h"


#define	  NDMEMORYPOOLSIZE_MIN			(500)
#define	  NDMEMORYPOOLSIZE_MAX			(100000)
#define	  NDMEMORYPOOLSIZE_INCRE		(500)

_NDSHAREBASE_BEGIN

// �ڴ�Ԥ����������;
struct NDPreMemoryParam
{
	NDUint32 nMinNum;				// Ԥ������Сֵ;
	NDUint32 nMaxNum;				// Ԥ�������ֵ;
	NDUint32 nIncreNum;				// ��������;

	NDPreMemoryParam()
	{
		clear();
	}

	~NDPreMemoryParam()
	{
		clear();
	}

	void clear()
	{
		nMinNum 	= 0;
		nMaxNum 	= 0;
		nIncreNum	= 0;
	}
};

// ����ͳ�Ƹ����ڴ涯��;
struct NDMemoryAssignUse
{
	NDUint32 nUseNum;				// �Ѿ�ʹ������;
	NDUint32 nAssignNum;			// �Ѿ���������;

	NDMemoryAssignUse()
	{
		clear();
	}

	~NDMemoryAssignUse()
	{
		clear();
	}

	void clear()
	{
		nAssignNum	= 0;
		nUseNum		= 0;
	}
};

class NDMemoryPoolBase
{
public:
	virtual ~NDMemoryPoolBase(){}

	virtual NDBool	init() = 0;

	// ��ʼ��ջָ���ڴ�;
	virtual NDBool	pushPreStackMem(NDUint32 nSize) = 0;

	// �ӳ����������ڴ�;
	virtual void*	mallocMemory() = 0;

	// �����ڴ������;
	virtual NDBool	freeMemory(void* pVoid) = 0;

	virtual const std::type_info& type() = 0;

public:
	NDMemoryAssignUse		m_MemWatch;			// �ڴ�ʹ�ü��;
	NDPreMemoryParam		m_MemPrepay;		// �ڴ�������;
};

template<typename T>
class NDMemoryPoolEx : public NDMemoryPoolBase
{
public:
	NDMemoryPoolEx();
	~NDMemoryPoolEx();

public:
	NDBool	init();

	//��ʼ��ջָ���ڴ�
	NDBool	pushPreStackMem( NDUint32 nSize );

	// �ӳ����������ڴ�
	void*	mallocMemory();

	// �����ڴ������
	NDBool	freeMemory(void* pVoid);

	const std::type_info& type();
	
private:
	std::list<T*>				m_FreeMemory;			// ���п��õ��ڴ�;
	std::map<T*,NDBool>			m_AllMemory;			// �����ѷ�����ڴ�;
};

template<typename T>
NDMemoryPoolEx<T>::NDMemoryPoolEx()
{
	m_MemPrepay.nMinNum		= NDMEMORYPOOLSIZE_MIN;
	m_MemPrepay.nMaxNum		= NDMEMORYPOOLSIZE_MAX;
	m_MemPrepay.nIncreNum	= NDMEMORYPOOLSIZE_INCRE;
}

template<typename T>
NDMemoryPoolEx<T>::~NDMemoryPoolEx()
{
	typename std::map<T*,NDBool>::iterator iter		= m_AllMemory.begin();
	typename std::map<T*,NDBool>::iterator iterEnd	= m_AllMemory.end();
	for ( ; iter != iterEnd; ++iter )
	{
		T* pInstance = iter->first;
		SAFE_FREE( pInstance )
	}
}


template<typename T>
NDBool NDMemoryPoolEx<T>::init()
{
	return pushPreStackMem(m_MemPrepay.nMinNum);
}

template<typename T>
NDBool NDMemoryPoolEx<T>::pushPreStackMem( NDUint32 nSize )
{
	if( m_MemWatch.nAssignNum >= m_MemPrepay.nMaxNum )
	{	//���������ó�����������;
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], AssignNum:[%u] beyond MaxNum:[%u].", typeid(T).name(), m_MemWatch.nAssignNum, m_MemPrepay.nMaxNum );
	}
	for( NDUint32 i = 0; i < nSize; ++i )
	{
		T* pInstance = (T*)malloc(sizeof(T));	//Ԥ�����ڴ滺��;
		if( NULL != pInstance )
		{
			m_AllMemory.insert( std::make_pair( pInstance, NDFalse ) );
			m_FreeMemory.push_back(pInstance);
			++m_MemWatch.nAssignNum;
		}
		else
		{
			NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], pushPreStackMem malloc memory error.", typeid(T).name());

			return NDFalse;
		}
	}

	if ( m_MemWatch.nAssignNum >= ( NDMEMORYPOOLSIZE_MAX * 5 ) )
	{
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], AssignNum:[%u].", typeid(T).name(), m_MemWatch.nAssignNum );
	}

	return NDTrue;
}

template<typename T>
void* NDMemoryPoolEx<T>::mallocMemory()
{
	void* pData = NULL;

	if( m_FreeMemory.empty() )
	{
		if ( NDFalse == pushPreStackMem( m_MemPrepay.nIncreNum ) )
		{
			return NULL;
		}
	}

	if( m_FreeMemory.empty() )
	{
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], mallocMemory is empty.", typeid(T).name() );
	
		return NULL;
	}

	//m_AllMemory ���� ;
	T* pInstance =m_FreeMemory.front();
	typename std::map<T*, NDBool>::iterator it = m_AllMemory.find( pInstance );
	if (it == m_AllMemory.end())
	{
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], mallocMemory fail, m_AllMemory not find address:[%p].", typeid(T).name(), pInstance );

		return NULL;
	}
	if (it->second)
	{
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], mallocMemory fail, Memory has allocated address:[%p].", typeid(T).name(), pInstance );
	}
	it->second = true;//û�ж������Ļ���־һ�� ��������;

	pData = (void*)pInstance;
	if ( pData )
	{
		new(pData) T;//��λnew������;
	}
	m_FreeMemory.pop_front();
	++m_MemWatch.nUseNum;

	return pData;
}

template<typename T>
NDBool NDMemoryPoolEx<T>::freeMemory( void* pVoid )
{
	if( NULL == pVoid )
		return NDFalse;
	
	//m_AllMemory ����;
	//�����ǲ����Լ����ڴ�;
	T* pInstance = (T*)pVoid;
	typename std::map<T*, NDBool>::iterator it = m_AllMemory.find( pInstance );
	if ( m_AllMemory.end() == it )
	{
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], freeMemory fail, m_AllMemory isn't exist address:[%p].", typeid(T).name(), pInstance );

		return NDFalse;
	}
	//�Ƿ��ι黹;
	if( NDFalse == it->second )
	{
		NDLOG_ERROR( " [MemoryPoolEX] Memory type:[%s], freeMemory fail, m_AllMemory already freeMemory address:[%p].", typeid(T).name(), pInstance );
	
		
		return NDFalse;
	}
	//û������ ��һ�� ����ȥ~~;
	it->second = false;
	pInstance->~T();
	m_FreeMemory.push_back(pInstance);
	m_MemWatch.nUseNum--;

	return NDTrue;
}

template<typename T>
const std::type_info& NDMemoryPoolEx<T>::type()
{
	return typeid(T);
}

_NDSHAREBASE_END
#endif

