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

	// �ӻ����ĩβд������(��������Ӵ�С)
	bool write(const void* data, unsigned int size);

	// �ӻ����ͷ����ȡ����(������С��С)
	bool read(void* data, unsigned int size);

	// ��ָ��λ��ȡָ����С�����������ݣ������ı仺���С
	bool get(void* data, unsigned int size, unsigned int pos = 0);

	// ȡ�û������Ĵ�С(�����ݵ��ֽ���)
	size_t size();

protected:
	void prevWrite(unsigned int size);

private:
	vector<byte> m_buffer;
	unsigned int m_curPos;
	unsigned int m_curSize;
};
