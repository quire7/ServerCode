#include "StdAfx.h"
#include "IDAllocator.h"

IDAllocator::IDAllocator(unsigned short max)
{
	resetMaxID(max);
}

IDAllocator::~IDAllocator(void)
{
	m_idMap.clear();
}

void IDAllocator::resetMaxID(unsigned short max)
{
	m_max = max;
	m_idMap.clear();
	for (unsigned short i=0; i<max; i++) {
		m_idMap[i] = i;
	}
}

unsigned short IDAllocator::allocateID()
{
	if (m_idMap.empty()) {
		TRACE("ID已经分配完，无法分配新的ID号\n");
		return INVALID_ID;
	}

	map<unsigned short,unsigned short>::iterator it = m_idMap.begin();
	unsigned short id = it->second;
	m_idMap.erase(it);
	return id;
}

bool IDAllocator::recycleID(unsigned short id)
{
#ifdef _DEBUG
	map<unsigned short,unsigned short>::iterator it = m_idMap.find(id);
	if (it != m_idMap.end()) {
		TRACE1("ID出现冲突，有相同ID存在，ID=%u\n",id);
		return false;
	}
#endif

	if (id > m_max) {
		TRACE1("非法的ID号，无法回收，ID=%u\n",id);
		return false;
	}
	
	m_idMap[id] = id;
	return true;
}

