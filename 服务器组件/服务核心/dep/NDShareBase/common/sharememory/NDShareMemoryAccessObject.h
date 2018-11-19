/********************************************************************
	created:	2014/10/03
	filename: 	d:\code\NDServer2\NDShareBase\common\sharememory\NDShareMemoryAccessObject.h
	file base:	NDShareMemoryAccessObject
	purpose:	�����ڴ���ʶ���Ķ���;
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
	NDBool				m_bCreate;					//�Ƿ��Ǵ�����;
	NDShareMemory*		m_pNDShareMemory;			//ShareMemoryʵ��ָ��;
	NDUint32			m_nSize;					//ShareMemory�ڴ��С;
	char*				m_pHeader;					//ShareMemory�ڴ�ͷָ��;
	char*				m_pDataPtr;					//ShareMemory����ָ��;

public:
	NDShareMemoryAccessObject();
	~NDShareMemoryAccessObject();

	/*
	 *	����ShareMem ���ʶ���(�´���);
	 *
	 *  NDSM_KEY nKey	���ʼ�ֵ;
	 *	NDUint32 nSize	�����������ֽڸ���(sizeof(NDSMHead)+sizeof(T)*count);
	 *
	 */
	NDBool		create( NDSM_KEY nKey, NDUint32 nSize );

	/*���ٶ���;*/
	void		destroy();
	
	/*
	 *	����ShareMem ���ʶ���(�����´���);
	 *	NDSM_KEY nKey	���ʼ�ֵ;
	 *	NDUint32 nSize	�����������ֽڸ���(sizeof(NDSMHead)+sizeof(T)*count);
	 *		
	 */
	NDBool		attach( NDSM_KEY nKey, NDUint32 nSize );

	/*ȡ������(������);*/
	void		detach();

	/*���������ָ��;*/
	char*		getDataPtr()	{ return m_pDataPtr; }

	/*��ô�СΪnSize�ĵ�nIndex��smu������(�±���㿪ʼ);*/
	char*		getTypePtr( NDUint32 nSize, NDUint32 nIndex );

	/*����������ܴ�С;*/
	NDUint32	getCapacity() const	{ return m_nSize; };

	NDSM_KEY	getKey() const;

	void		setHeadVersion( NDUint32 nVersion );
	NDUint32	getHeadVersion() const;

	/*���úͻ�����ݵ�Ԫ������;*/
	void		setUnitCapacity( NDUint32 nCapacity );
	NDUint32	getUnitCapacity() const;
	
	/*���úͻ�����ݵ�Ԫ���Ѿ�ʹ�õĸ���;*/
	void		setUnitSize( NDUint32 nCurSize );
	NDUint32	getUnitSize() const;
	

	/*��ú��ͷ�ͷ�����ݵĿ���Ȩ;*/
	NDBool		tryLockHead( NDUint16 nLockType );
	NDBool		tryUnLockHead( NDUint16 nUnlockType );

	NDBool		dumpToFile( const char* szFilePath );
	NDBool		mergeFromFile( const char* szFilePath );
	
};

_NDSHAREBASE_END
#endif

