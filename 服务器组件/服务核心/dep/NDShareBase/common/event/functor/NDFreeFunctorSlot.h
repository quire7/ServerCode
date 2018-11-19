#ifndef __SHARE_BASE_ND_FREE_FUNCTOR_SLOT_H__
#define __SHARE_BASE_ND_FREE_FUNCTOR_SLOT_H__

#include "NDSlotFunctorBase.h"

_NDSHAREBASE_BEGIN

class NDFreeFunctorSlot : public NDSlotFunctorBase
{
public:
	typedef NDBool (*pSlotFunction)( const NDEventArgs& eventArgs );

	NDFreeFunctorSlot( pSlotFunction slotFunction ) : m_pSlotFunction( slotFunction ) {}
	virtual NDBool operator()( const NDEventArgs& eventArgs )
	{
		return (*m_pSlotFunction)( eventArgs );
	}

private:
	pSlotFunction m_pSlotFunction;
};

_NDSHAREBASE_END
#endif

