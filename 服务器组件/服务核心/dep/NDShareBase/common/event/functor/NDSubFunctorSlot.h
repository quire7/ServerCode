/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\functor\NDSubFunctorSlot.h
	file base:	NDSubFunctorSlot
	purpose:	function functor;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SUB_FUNCTOR_SLOT_H__
#define __SHARE_BASE_ND_SUB_FUNCTOR_SLOT_H__


#include "NDFreeFunctorSlot.h"
#include "NDMemberFunctorSlot.h"
#include "NDPointerFunctorSlot.h"
#include "NDReferenceFunctorSlotBinder.h"
#include "NDReferenceFunctorSlot.h"
#include "NDCopyFunctorSlot.h"

_NDSHAREBASE_BEGIN

class NDSubFunctorSlot
{
public:
	NDSubFunctorSlot(){}
	~NDSubFunctorSlot()
	{
	}

	void cleanup()
	{
		if (NULL != m_pFunctorBase)
		{
			delete m_pFunctorBase;
			m_pFunctorBase = NULL;
		}
	}

	//Global function functor
	NDSubFunctorSlot( NDFreeFunctorSlot::pSlotFunction pSlotFunction ) : m_pFunctorBase( new NDFreeFunctorSlot(pSlotFunction) ) {}

	//Member function functor
	template<typename T>
	NDSubFunctorSlot( NDBool (T::*pFunction)(const NDEventArgs& eventArgs), T* pObject )
		: m_pFunctorBase( new NDMemberFunctorSlot<T>(pFunction, pObject) )
	{
		//printf(" m_pFunctorBase = %p. \n ", m_pFunctorBase );
	}
	
	//functor function pointer
	template<typename T>
	NDSubFunctorSlot( T* pFunctor )
		: m_pFunctorBase( new NDPointerFunctorSlot<T>(pFunctor) )
	{
		//printf(" m_pFunctorBase = %p. \n ", m_pFunctorBase );
	}


	//functor function reference
	template<typename T>
	NDSubFunctorSlot( const NDReferenceFunctorSlotBinder<T>& refFunctorBinder )
		: m_pFunctorBase( new NDReferenceFunctorSlot<T>(refFunctorBinder.m_refFunctor) )
	{
		//printf(" m_pFunctorBase = %p. \n ", m_pFunctorBase );
	}

	//functor function copy
	template<typename T>
	NDSubFunctorSlot( const T& refFunctor )
		: m_pFunctorBase( new NDCopyFunctorSlot<T>(refFunctor) )
	{
		//printf(" m_pFunctorBase = %p. \n ", m_pFunctorBase );
	}

	NDBool operator()( const NDEventArgs& eventArgs )
	{
		return (*m_pFunctorBase)( eventArgs );
	}

	void	SetEnumFlag( E_NDSlotFunctorBase eFlag ) 
	{ 
		if ( NULL != m_pFunctorBase )
		{
			m_pFunctorBase->SetEnumFlag( eFlag );
		}
	}

	E_NDSlotFunctorBase	GetEnumFlag() 
	{
		if ( NULL != m_pFunctorBase )
		{
			return m_pFunctorBase->GetEnumFlag();
		}
		
		return E_NDSlotFunctorBase_DELETE;
	}

	NDBool connected() const
	{
		return ( NULL != m_pFunctorBase );
	}

private:
	NDSlotFunctorBase*	m_pFunctorBase;
};

_NDSHAREBASE_END
#endif

