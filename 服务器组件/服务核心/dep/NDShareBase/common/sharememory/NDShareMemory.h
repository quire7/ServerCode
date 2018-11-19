/********************************************************************
	created:	2014/10/03
	filename: 	d:\code\NDServer2\NDShareBase\common\sharememory\NDShareMemory.h
	file base:	NDShareMemory
	purpose:	共享内存的构造和释放;
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
	NDBool			m_bCreate;			//是否是创建者;
	NDSMHandle		m_NDSMHandle;		//共享内存的句柄(或者说是ID);
public:
	NDShareMemory();
	~NDShareMemory();

	/*	创建ShareMemory内存区;
	 *	
	 *	nKey   创建ShareMemory的关键值;
	 *  nSize  创建大小;
	 *
	 *	返回对应ShareMemory是否创建成功;
	 */
	NDBool			createShareMemory( NDSM_KEY nKey, NDUint32 nSize );

	/* 删除ShareMemory; */
	void			destoryShareMemory();

	/*	打开ShareMemory内存区;
	 *	
	 *	nKey   打开ShareMemory的关键值;
	 *  nSize  打开大小;
	 *
	 *	返回对应ShareMemory是否打开成功;
	 */
	NDBool			openShareMemory( NDSM_KEY nKey, NDUint32 nSize );

	/*	关闭ShareMemory; */
	void			closeShareMemory();

	/*	映射ShareMemory内存区;
	 *
	 *  返回 ShareMemory的数据指针;
	 */
	char*			mapShareMemory();
	
	/*	关闭映射ShareMemory内存区;
	 *
	 *	pMemoryPtr	ShareMemory的数据指针;
	 */	
	void			unMapShareMem( char* pMemoryPtr );
};

_NDSHAREBASE_END

#endif

