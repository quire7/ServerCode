#ifndef YS_DATA_LOCKER_HEAD_FILE
#define YS_DATA_LOCKER_HEAD_FILE

#pragma once

#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
class SERVICE_CORE_CLASS CYSDataLocker
{
	//��������
private:
	INT								m_nLockCount;					//��������
	CCriticalSection &				m_CriticalSection;				//��������

	//��������
public:
	//���캯��
	CYSDataLocker(CCriticalSection & CriticalSection, bool bLockAtOnce=true);
	//��������
	virtual ~CYSDataLocker();

	//��������
public:
	//��������
	VOID Lock();
	//�������� 
	VOID UnLock();

	//״̬����
public:
	//��������
	inline INT GetLockCount() { return m_nLockCount; }
};

//////////////////////////////////////////////////////////////////////////////////

#endif