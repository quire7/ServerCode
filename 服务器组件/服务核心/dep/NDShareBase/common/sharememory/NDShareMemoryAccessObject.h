/********************************************************************
	created:	2014/10/03
	filename: 	d:\code\NDServer2\NDShareBase\common\sharememory\NDShareMemoryAccessObject.h
	file base:	NDShareMemoryAccessObject
	purpose:	共享内存访问对象的定义;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SHARE_MEMORY_ACCESS_OBJECT_H__
#define __SHARE_BASE_ND_SHARE_MEMORY_ACCESS_OBJECT_H__


#include "NDTypes.h"
#include "NDShareMemoryBase.h"

_NDSHAREBASE_BEGIN

class NDShareMemory;
class NDShareMemoryAccessObject
{
private:
	NDBool				m_bCreate;					//是否是创建者;
	NDShareMemory*		m_pNDShareMemory;			//ShareMemory实例指针;
	NDUint32			m_nSize;					//ShareMemory内存大小;
	char*				m_pHeader;					//ShareMemory内存头指针;
	char*				m_pDataPtr;					//ShareMemory数据指针;

public:
	NDShareMemoryAccessObject();
	~NDShareMemoryAccessObject();

	/*
	 *	创建ShareMem 访问对象(新创建);
	 *
	 *  NDSM_KEY nKey	访问键值;
	 *	NDUint32 nSize	访问数据区字节个数(sizeof(NDSMHead)+sizeof(T)*count);
	 *
	 */
	NDBool		create( NDSM_KEY nKey, NDUint32 nSize );

	/*销毁对象;*/
	void		destroy();
	
	/*
	 *	附着ShareMem 访问对象(不是新创建);
	 *	NDSM_KEY nKey	访问键值;
	 *	NDUint32 nSize	访问数据区字节个数(sizeof(NDSMHead)+sizeof(T)*count);
	 *		
	 */
	NDBool		attach( NDSM_KEY nKey, NDUint32 nSize );

	/*取消附着(不销毁);*/
	void		detach();

	/*获得数据区指针;*/
	char*		getDataPtr()	{ return m_pDataPtr; }

	/*获得大小为nSize的第nIndex个smu的数据(下标从零开始);*/
	char*		getTypePtr( NDUint32 nSize, NDUint32 nIndex );

	/*获得数据区总大小;*/
	NDUint32	getCapacity() const	{ return m_nSize; };

	NDSM_KEY	getKey() const;

	void		setHeadVersion( NDUint32 nVersion );
	NDUint32	getHeadVersion() const;

	/*设置和获得数据单元的容量;*/
	void		setUnitCapacity( NDUint32 nCapacity );
	NDUint32	getUnitCapacity() const;
	
	/*设置和获得数据单元的已经使用的个数;*/
	void		setUnitSize( NDUint32 nCurSize );
	NDUint32	getUnitSize() const;
	

	/*获得和释放头部数据的控制权;*/
	NDBool		tryLockHead( NDUint16 nLockType );
	NDBool		tryUnLockHead( NDUint16 nUnlockType );

	NDBool		dumpToFile( const char* szFilePath );
	NDBool		mergeFromFile( const char* szFilePath );
	
};

_NDSHAREBASE_END
#endif

