/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\event\NDEvent.h
	file base:	NDEvent
	purpose:	�¼�;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_EVENT_H__
#define __SHARE_BASE_ND_EVENT_H__

#include <map>

#include "function/NDRefCounted.h"

#include "NDBoundSlot.h"

_NDSHAREBASE_BEGIN

class NDEventArgs;

class NDEvent
{
public:
	typedef NDRefCounted<NDBoundSlot>	NDConnection;	///< �ź�������;

	//����������;
	//@note �����Զ��Ͽ�����;
	class ScopedConnection : public NDConnection
	{
	public:
		ScopedConnection() {}

		~ScopedConnection(){ disconnect(); }

		ScopedConnection(const NDEvent::NDConnection& connection) : m_connection(connection)
		{}

		ScopedConnection& operator=(const NDEvent::NDConnection& connection)
		{
			m_connection = connection;
			return *this;
		}

		bool connected() const
		{ 
			return m_connection.isvaild() ? m_connection->connected() : false;
		}

		void disconnect()
		{
			if (m_connection.isvaild()) m_connection->disconnect(); 
		}

	private:
		NDEvent::NDConnection m_connection;
	};

private:
	typedef std::multimap<NDGroup, NDConnection> SlotContainer;

	SlotContainer	m_slots;	///< ����������;
	const char*		m_name;		///< �¼���;
public:
	NDEvent( const char* szName );
	~NDEvent();

	/// ��ȡ�¼���;
	const char*			getName() const { return m_name; }

	///�����¼�;
	///@param[in] subscriber �¼�������;
	NDConnection		subscribe( const NDSubFunctorSlot& slot );

	///�����¼�;
	///@param[in] group �¼�����;
	///@param[in] subscriber �¼�������;
	NDConnection		subscribe( NDGroup group, const NDSubFunctorSlot& slot );

	///�º���;
	void				operator()( NDEventArgs& args );

private:
	///ȡ������;
	friend void NDShareBase::NDBoundSlot::disconnect();

	///ȡ������;
	void				unsubscribe( const NDBoundSlot& slot );

	NDEvent(const NDEvent&) {}
	NDEvent& operator=(const NDEvent&)  {return *this;}
};



_NDSHAREBASE_END

#endif
