#ifndef __SHARE_BASE_ND_REFERENCE_FUNCTOR_SLOT_BINDER_H__
#define __SHARE_BASE_ND_REFERENCE_FUNCTOR_SLOT_BINDER_H__

_NDSHAREBASE_BEGIN

template<typename T>
struct NDReferenceFunctorSlotBinder 
{
public:
	NDReferenceFunctorSlotBinder( T& refFunctor ) : m_refFunctor( refFunctor ) {}
	T&	m_refFunctor;
};

_NDSHAREBASE_END
#endif

