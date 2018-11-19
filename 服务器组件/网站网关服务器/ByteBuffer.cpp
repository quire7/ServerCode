#include "StdAfx.h"
#include ".\bytebuffer.h"

ByteBuffer::ByteBuffer(unsigned int size)
{
	m_buffer.resize(size);
	m_curPos = 0;
	m_curSize = 0;
}

ByteBuffer::~ByteBuffer(void)
{
}

size_t ByteBuffer::size()
{
	return m_curSize;
}

void ByteBuffer::dump()
{
	TRACE("---------Begin Dump-----------\n");
	for (unsigned int i=0; i<m_curSize; i++) {
		TRACE1("%c", m_buffer[i]);	
	}
	TRACE("\n---------End Dump-----------\n");
}

void ByteBuffer::prevWrite(unsigned int size)
{
	if (m_curSize + size >= m_buffer.size()) {
		m_buffer.resize(m_buffer.size()*2);
	}
}

bool ByteBuffer::write(const void* data, unsigned int size)
{
	prevWrite(size);

	memcpy(&m_buffer[m_curPos],data,size);
	m_curPos += size;
	m_curSize += size;

	return true;
}

bool ByteBuffer::read(void* data, unsigned int size)
{
	if (size > m_curSize)
		return false;

	memcpy(data,&m_buffer[0],size);
	memmove(&m_buffer[0],&m_buffer[m_curPos-m_curSize+size],m_curSize-size);
	m_curPos -= size;
	m_curSize -= size;

	return true;
}

bool ByteBuffer::get(void* data, unsigned int size, unsigned int pos)
{
	if (pos + size > m_curSize)
		return false;

	memcpy(data,&m_buffer[0] + pos, size);

	return true;
}
