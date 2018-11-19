#include "StdAfx.h"
#include <vector>
#include "YSAllocationID.h"
#include "YSDataLocker.h"

using std::vector;
YSAllocationID::YSAllocationID(int nMaxSize)
{
	ReAllocate(nMaxSize);
}

void YSAllocationID::ReAllocate(int nMaxSize)
{
	int nSize = m_allocationIDMap.size();
	if ( (nSize + 1) >= nMaxSize )
	{	// +1是因为除去零;
		return;
	}
	CYSDataLocker DataLocker(m_CriticalSection);
	m_allocationIDQueue.clear();
	m_allocationIDMap.clear();

	vector<int > allocationIDVec;
	for (int i = 1; i < nMaxSize; ++i)
	{
		allocationIDVec.push_back(i);
	}

	int nVecSize	= nMaxSize - 1;
	int nVecIndex	= 0;
	int nRandValue	= 0;
	nSize			= 0;
	do{
		srand((nMaxSize - nSize) * 10 + nSize + time(NULL));
		nVecIndex	= rand() % nVecSize;
		nRandValue	= allocationIDVec[nVecIndex];
		
		m_allocationIDQueue.push_back(nRandValue);
		m_allocationIDMap.insert(std::make_pair(nRandValue, true));

		allocationIDVec.erase(allocationIDVec.begin() + nVecIndex);
		nVecSize = allocationIDVec.size();

		++nSize;
	} while ((nSize + 1) < nMaxSize);
}

int YSAllocationID::PopValue()
{
	CYSDataLocker DataLocker(m_CriticalSection);
	if (m_allocationIDQueue.empty())
	{
		return 0;
	}

	int nRet = m_allocationIDQueue.front();
	m_allocationIDQueue.pop_front();

	map < int, bool >::iterator iterFind = m_allocationIDMap.find(nRet);
	if ( iterFind != m_allocationIDMap.end() )
	{	//设置使用标志;
		iterFind->second = false;
	}

	return nRet;
}

void YSAllocationID::PushValue(int nID)
{
	CYSDataLocker DataLocker(m_CriticalSection);
	map < int, bool >::iterator iterFind = m_allocationIDMap.find(nID);
	if (iterFind == m_allocationIDMap.end())
	{	//没有找到;
		return;
	}
	if (iterFind->second == true)
	{	//还没有使用;
		return;
	}

	iterFind->second = true;
	m_allocationIDQueue.push_back(nID);
}

