#include "StdAfx.h"
#include "YSDataLocker.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CYSDataLocker::CYSDataLocker(CCriticalSection & CriticalSection, bool bLockAtOnce) 
	: m_CriticalSection(CriticalSection)
{
	//���ñ���
	m_nLockCount=0;

	//��������
	if (bLockAtOnce==true)
	{
		Lock();
	}

	return;
}

//��������
CYSDataLocker::~CYSDataLocker()
{
	//�������
	while (m_nLockCount>0)
	{
		UnLock();
	}

	return;
}

//��������
VOID CYSDataLocker::Lock()
{
	//��������
	m_CriticalSection.Lock();

	//���ñ���
	m_nLockCount++;

	return;
}

//��������
VOID CYSDataLocker::UnLock()
{
	//Ч��״̬
	ASSERT(m_nLockCount>0);
	if (m_nLockCount==0) return;

	//���ñ���
	m_nLockCount--;

	//�������
	m_CriticalSection.Unlock();

	return;
}

//////////////////////////////////////////////////////////////////////////////////
