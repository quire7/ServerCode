/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\event\NDBoundSlot.h
	file base:	NDBoundSlot
	purpose:	bound functor slot;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/

#ifndef __SHARE_BASE_ND_BOUND_SLOT_H__
#define __SHARE_BASE_ND_BOUND_SLOT_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDSubFunctorSlot;
class NDEvent;

//连接器,用于连接订阅者和事件;
//它持有着:订阅者的指针,被订阅的事件,订阅者的组别号;
class NDBoundSlot
{
private:
	NDGroup					m_nGroup;			///< 订阅者分组号;
	NDSubFunctorSlot*		m_pSubFunctorSlot;	///< 订阅者指针;
	NDEvent*				m_pEvent;			///< 绑定到的事件;
public:
	NDBoundSlot( NDGroup group, const NDSubFunctorSlot& refNDSubFunctorSlot, NDEvent& refEvent );
	NDBoundSlot( const NDBoundSlot& other );
	~NDBoundSlot();
	
	//是否已经连接;
	NDBool					connected() const;

	//断开连接;
	//@note 所做的事情就是:清除订阅者,取消事件的订阅;
	void					disconnect();

	//判断实际上的订阅者对象是否是同一个;
	bool					operator==(const NDBoundSlot& other) const;

	//判断实际上的订阅者对象不是同一个;
	bool					operator!=(const NDBoundSlot& other) const;

private:
	friend	class NDEvent;

	NDBoundSlot&			operator=( const NDBoundSlot& other );
};


_NDSHAREBASE_END

#endif
