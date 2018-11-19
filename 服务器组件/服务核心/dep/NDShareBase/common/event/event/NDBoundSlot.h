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

//������,�������Ӷ����ߺ��¼�;
//��������:�����ߵ�ָ��,�����ĵ��¼�,�����ߵ�����;
class NDBoundSlot
{
private:
	NDGroup					m_nGroup;			///< �����߷����;
	NDSubFunctorSlot*		m_pSubFunctorSlot;	///< ������ָ��;
	NDEvent*				m_pEvent;			///< �󶨵����¼�;
public:
	NDBoundSlot( NDGroup group, const NDSubFunctorSlot& refNDSubFunctorSlot, NDEvent& refEvent );
	NDBoundSlot( const NDBoundSlot& other );
	~NDBoundSlot();
	
	//�Ƿ��Ѿ�����;
	NDBool					connected() const;

	//�Ͽ�����;
	//@note �������������:���������,ȡ���¼��Ķ���;
	void					disconnect();

	//�ж�ʵ���ϵĶ����߶����Ƿ���ͬһ��;
	bool					operator==(const NDBoundSlot& other) const;

	//�ж�ʵ���ϵĶ����߶�����ͬһ��;
	bool					operator!=(const NDBoundSlot& other) const;

private:
	friend	class NDEvent;

	NDBoundSlot&			operator=( const NDBoundSlot& other );
};


_NDSHAREBASE_END

#endif
