#pragma once

#include <Windows.h>
#include <vector>
using namespace std;

class ByteBuffer
{
public:
	ByteBuffer(unsigned int size = 1024);
	virtual ~ByteBuffer(void);

	// debug function
	void dump();

	// 从缓存的末尾写入数据(缓存会增加大小)
	bool write(const void* data, unsigned int size);

	// 从缓存的头部读取数据(缓存会减小大小)
	bool read(void* data, unsigned int size);

	// 从指定位置取指定大小的连续的数据，但不改变缓存大小
	bool get(void* data, unsigned int size, unsigned int pos = 0);

	// 取得缓存区的大小(即数据的字节数)
	size_t size();

protected:
	void prevWrite(unsigned int size);

private:
	vector<byte> m_buffer;
	unsigned int m_curPos;
	unsigned int m_curSize;
};
