#ifndef __SHARE_BASE_ND_POINTER_FUNCTOR_SLOT_H__
#define __SHARE_BASE_ND_POINTER_FUNCTOR_SLOT_H__

#include "NDSlotFunctorBase.h"

_NDSHAREBASE_BEGIN

template<typename T>
class NDPointerFunctorSlot : public NDSlotFunctorBase
{
public:
	NDPointerFunctorSlot( T* pFunctor ) : m_pFunctor( pFunctor ) {}
	virtual NDBool operator()( const NDEventArgs& eventArgs )
	{
		return (*m_pFunctor)( eventArgs );
	}

private:
	T*	m_pFunctor;
};

_NDSHAREBASE_END
#endif

