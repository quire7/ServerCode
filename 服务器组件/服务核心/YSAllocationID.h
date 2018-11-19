#ifndef _YS_ALLOCATION_ID_H_
#define _YS_ALLOCATION_ID_H_

#include <deque>
#include <map>

#include "ServiceCoreHead.h"

using std::deque;
using std::map;

class SERVICE_CORE_CLASS YSAllocationID
{
private:
	CCriticalSection				m_CriticalSection;					//同步对象;
	deque<int>						m_allocationIDQueue;				//随机ID的queue;
	map < int, bool >				m_allocationIDMap;					//随机ID的map;

	enum{ default_maxsize=10000 };

public:
	YSAllocationID(int nMaxSize = default_maxsize);
	~YSAllocationID(){};

	//重新分配更大Size;
	void	ReAllocate( int nMaxSize );

	//获得一个随机ID;
	int		PopValue();
	//归还分配出去的ID;
	void	PushValue( int nID );
};



#endif
