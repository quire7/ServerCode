#ifndef __SHARE_BASE_ND_REFERENCE_FUNCTOR_SLOT_H__
#define __SHARE_BASE_ND_REFERENCE_FUNCTOR_SLOT_H__

#include "NDSlotFunctorBase.h"

_NDSHAREBASE_BEGIN

template<typename T>
class NDReferenceFunctorSlot : public NDSlotFunctorBase
{
public:
	NDReferenceFunctorSlot( T& refFunctor ) : m_refFunctor( refFunctor ) {}
	virtual NDBool operator()( const NDEventArgs& eventArgs )
	{
		return (m_refFunctor)( eventArgs );
	}

private:
	T&	m_refFunctor;
};

_NDSHAREBASE_END
#endif

