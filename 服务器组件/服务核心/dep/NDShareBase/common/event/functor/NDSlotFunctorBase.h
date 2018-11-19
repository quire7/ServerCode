#ifndef __SHARE_BASE_ND_SLOT_FUNCTOR_BASE_H__
#define __SHARE_BASE_ND_SLOT_FUNCTOR_BASE_H__

#include "NDTypes.h"
#include "NDShareBaseEnums.h"


_NDSHAREBASE_BEGIN

class NDEventArgs;

class NDSlotFunctorBase
{
public:
	NDSlotFunctorBase():m_byEnumFlag(E_NDSlotFunctorBase_RUN){};

	virtual ~NDSlotFunctorBase(){}
	virtual NDBool operator()( const NDEventArgs& eventArgs ) = 0;

	void					SetEnumFlag( E_NDSlotFunctorBase eFlag ) { m_byEnumFlag = (NDUint8)eFlag; }
	E_NDSlotFunctorBase		GetEnumFlag() { return (E_NDSlotFunctorBase)m_byEnumFlag; }

private:
	NDUint8	m_byEnumFlag;
};

_NDSHAREBASE_END
#endif

