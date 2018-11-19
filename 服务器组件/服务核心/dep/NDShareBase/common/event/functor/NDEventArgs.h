#ifndef __SHARE_BASE_ND_EVENT_ARGS_H__
#define __SHARE_BASE_ND_EVENT_ARGS_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDEventArgs
{
public:
	NDEventArgs() : m_nhandled(0) {}
	virtual ~NDEventArgs(){}

	NDUint32	m_nhandled;		//处理计数;
};


_NDSHAREBASE_END
#endif

