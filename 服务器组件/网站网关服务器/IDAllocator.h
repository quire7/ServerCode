#pragma once

#ifndef __IDALLOCATOR_H__
#define __IDALLOCATOR_H__

#include <map>
using namespace std;

// ID��������ID�Ǵ�0��ʼ����ġ�

#define INVALID_ID	0xFFFF

class IDAllocator
{
public:
	IDAllocator(unsigned short max);
	virtual ~IDAllocator(void);

	// �������ID,����ʹ��
	void resetMaxID(unsigned short max);

	// ����һ��ID
	unsigned short allocateID();

	// ����һ��ID
	bool recycleID(unsigned short id);

private:
	unsigned short m_max;	// �����ֵ
	map<unsigned short,unsigned short> m_idMap;
};

#endif