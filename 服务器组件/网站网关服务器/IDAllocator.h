#pragma once

#ifndef __IDALLOCATOR_H__
#define __IDALLOCATOR_H__

#include <map>
using namespace std;

// ID分配器，ID是从0开始分配的。

#define INVALID_ID	0xFFFF

class IDAllocator
{
public:
	IDAllocator(unsigned short max);
	virtual ~IDAllocator(void);

	// 重设最大ID,谨慎使用
	void resetMaxID(unsigned short max);

	// 分配一个ID
	unsigned short allocateID();

	// 回收一个ID
	bool recycleID(unsigned short id);

private:
	unsigned short m_max;	// 最大数值
	map<unsigned short,unsigned short> m_idMap;
};

#endif