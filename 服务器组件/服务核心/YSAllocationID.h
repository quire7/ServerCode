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
	CCriticalSection				m_CriticalSection;					//ͬ������;
	deque<int>						m_allocationIDQueue;				//���ID��queue;
	map < int, bool >				m_allocationIDMap;					//���ID��map;

	enum{ default_maxsize=10000 };

public:
	YSAllocationID(int nMaxSize = default_maxsize);
	~YSAllocationID(){};

	//���·������Size;
	void	ReAllocate( int nMaxSize );

	//���һ�����ID;
	int		PopValue();
	//�黹�����ȥ��ID;
	void	PushValue( int nID );
};



#endif
