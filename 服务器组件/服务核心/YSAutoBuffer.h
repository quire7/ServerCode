#ifndef _YSAUTO_BUFFER_H_
#define _YSAUTO_BUFFER_H_

#include "ServiceCoreHead.h"

class SERVICE_CORE_CLASS AUTO_BUFFER
{
private:
	int		m_used_size;
	int		m_buff_size;
	int		m_alloc_granularity;
	char	*m_ptr;

public:
	AUTO_BUFFER(int init_size, int alloc_granularity = 0)
	{
		if (alloc_granularity == 0)
			m_alloc_granularity = init_size;
		else
			m_alloc_granularity = alloc_granularity;

		m_used_size = 0;
		m_buff_size = init_size;
		m_ptr = new char[init_size];
	}

	~AUTO_BUFFER()
	{
		delete []m_ptr;
		m_ptr = NULL;
	}

	void Put(char * in_buff, int in_buff_size)
	{
		if (m_used_size + in_buff_size > m_buff_size)
		{
			while (m_buff_size < m_used_size + in_buff_size)
				m_buff_size += m_alloc_granularity;

			char * temp_new = new char[m_buff_size];
			char * temp_old = m_ptr;

			memcpy(temp_new, temp_old, m_used_size);

			delete []temp_old;
			m_ptr = temp_new;
		}

		memcpy(m_ptr + m_used_size, in_buff, in_buff_size);
		m_used_size += in_buff_size;
	}

	int Size()
	{
		return m_used_size;
	}

	char* Get()
	{
		return m_ptr;
	}
};

#endif