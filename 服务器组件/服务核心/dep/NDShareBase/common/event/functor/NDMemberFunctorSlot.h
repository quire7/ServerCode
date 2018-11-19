#ifndef __SHARE_BASE_ND_MEMBER_FUNCTOR_SLOT_H__
#define __SHARE_BASE_ND_MEMBER_FUNCTOR_SLOT_H__

#include "NDSlotFunctorBase.h"

_NDSHAREBASE_BEGIN

template<typename T>
class NDMemberFunctorSlot : public NDSlotFunctorBase
{
public:
	typedef NDBool (T::*pSlotFunction)( const NDEventArgs& eventArgs );

	NDMemberFunctorSlot( pSlotFunction slotFunction, T* tObject ) : m_pSlotFunction( slotFunction ), m_tObject( tObject ) {}
	virtual NDBool operator()( const NDEventArgs& eventArgs )
	{
		return (m_tObject->*m_pSlotFunction)( eventArgs );
	}

private:
	pSlotFunction m_pSlotFunction;
	T*			  m_tObject;
};

_NDSHAREBASE_END
#endif

