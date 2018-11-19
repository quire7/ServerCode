/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\event\NDEvent.h
	file base:	NDEvent
	purpose:	事件;
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
	typedef NDRefCounted<NDBoundSlot>	NDConnection;	///< 信号连接器;

	//区域连接器;
	//@note 出域自动断开连接;
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

	SlotContainer	m_slots;	///< 连接器集合;
	const char*		m_name;		///< 事件名;
public:
	NDEvent( const char* szName );
	~NDEvent();

	/// 获取事件名;
	const char*			getName() const { return m_name; }

	///订阅事件;
	///@param[in] subscriber 事件订阅者;
	NDConnection		subscribe( const NDSubFunctorSlot& slot );

	///订阅事件;
	///@param[in] group 事件组别号;
	///@param[in] subscriber 事件订阅者;
	NDConnection		subscribe( NDGroup group, const NDSubFunctorSlot& slot );

	///仿函数;
	void				operator()( NDEventArgs& args );

private:
	///取消连接;
	friend void NDShareBase::NDBoundSlot::disconnect();

	///取消订阅;
	void				unsubscribe( const NDBoundSlot& slot );

	NDEvent(const NDEvent&) {}
	NDEvent& operator=(const NDEvent&)  {return *this;}
};



_NDSHAREBASE_END

#endif
