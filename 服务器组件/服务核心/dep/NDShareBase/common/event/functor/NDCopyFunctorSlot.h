#ifndef __SHARE_BASE_ND_COPY_FUNCTOR_SLOT_H__
#define __SHARE_BASE_ND_COPY_FUNCTOR_SLOT_H__

#include "NDSlotFunctorBase.h"

_NDSHAREBASE_BEGIN

template<typename T>
class NDCopyFunctorSlot : public NDSlotFunctorBase
{
public:
	NDCopyFunctorSlot( const T& refFunctor ) : m_copyFunctor( refFunctor ) {}
	virtual NDBool operator()( const NDEventArgs& eventArgs )
	{
		return (m_copyFunctor)( eventArgs );
	}

private:
	T	m_copyFunctor;
};

_NDSHAREBASE_END
#endif
