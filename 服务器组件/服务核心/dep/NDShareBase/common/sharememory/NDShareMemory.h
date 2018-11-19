/********************************************************************
	created:	2014/10/03
	filename: 	d:\code\NDServer2\NDShareBase\common\sharememory\NDShareMemory.h
	file base:	NDShareMemory
	purpose:	�����ڴ�Ĺ�����ͷ�;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SHARE_MEMORY_H__
#define __SHARE_BASE_ND_SHARE_MEMORY_H__


#include "NDTypes.h"
#include "NDShareMemoryBase.h"

_NDSHAREBASE_BEGIN

class NDShareMemory
{
private:
	NDBool			m_bCreate;			//�Ƿ��Ǵ�����;
	NDSMHandle		m_NDSMHandle;		//�����ڴ�ľ��(����˵��ID);
public:
	NDShareMemory();
	~NDShareMemory();

	/*	����ShareMemory�ڴ���;
	 *	
	 *	nKey   ����ShareMemory�Ĺؼ�ֵ;
	 *  nSize  ������С;
	 *
	 *	���ض�ӦShareMemory�Ƿ񴴽��ɹ�;
	 */
	NDBool			createShareMemory( NDSM_KEY nKey, NDUint32 nSize );

	/* ɾ��ShareMemory; */
	void			destoryShareMemory();

	/*	��ShareMemory�ڴ���;
	 *	
	 *	nKey   ��ShareMemory�Ĺؼ�ֵ;
	 *  nSize  �򿪴�С;
	 *
	 *	���ض�ӦShareMemory�Ƿ�򿪳ɹ�;
	 */
	NDBool			openShareMemory( NDSM_KEY nKey, NDUint32 nSize );

	/*	�ر�ShareMemory; */
	void			closeShareMemory();

	/*	ӳ��ShareMemory�ڴ���;
	 *
	 *  ���� ShareMemory������ָ��;
	 */
	char*			mapShareMemory();
	
	/*	�ر�ӳ��ShareMemory�ڴ���;
	 *
	 *	pMemoryPtr	ShareMemory������ָ��;
	 */	
	void			unMapShareMem( char* pMemoryPtr );
};

_NDSHAREBASE_END

#endif

