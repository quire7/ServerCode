/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\event\NDEventSet.h
	file base:	NDEventSet
	purpose:	事件集;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_EVENT_SET_H__
#define __SHARE_BASE_ND_EVENT_SET_H__

#include "NDEvent.h"
#include "stl/NDSTLFunctor.h"

_NDSHAREBASE_BEGIN

//事件集,它既做了事件的容器,也作为了一个事件的中心进行分发事件;
class NDEventSet
{
private:
	typedef std::map<char*, NDEvent*, cStrLess>	NDEventMap;
	typedef NDEventMap::iterator				NDEventMapIter;

	NDBool		m_bMuted;	//静音开关;
	NDEventMap	m_Events;	//事件集;
	
public:
	NDEventSet();
	virtual ~NDEventSet();

	//添加事件;
	//@param[in] name 事件名;
	void				addEvent( const char* name );

	//移除事件;
	//@param[in] name 事件名;
	void				removeEvent( const char* name );

	//移除所有事件;
	void				removeAllEvents();

	//事件是否存在;
	//@param[in] name 事件名;
	bool				isEventPresent( const char* name );

	//订阅事件;
	//@param[in] name 事件名;
	//@param[in] subscriber 事件订阅者;
	virtual NDEvent::NDConnection	subscribeEvent( const char* name, NDSubFunctorSlot subscriber );

	//@param[in] name 事件名;
	//@param[in] group 事件组别号;
	//@param[in] subscriber 事件订阅者;
	virtual NDEvent::NDConnection	subscribeEvent( const char* name, NDGroup group, NDSubFunctorSlot subscriber);
	
	//发送事件;
	//@param[in] name 事件名;
	//@param[in] args 事件参数;
	//@param[in] eventNamespace 事件命名空间;
	virtual void					fireEvent( const char* name, NDEventArgs& args, const char* eventNamespace = "" );

	//是否不响应事件;
	NDBool							isMuted() const;

	//设置是否响应事件;
	void							setMutedState(NDBool setting);

protected:
	//获取事件;
	NDEvent*						getEventObject( const char* name, NDBool autoAdd = NDFalse );

	//处理本地事件;
	void							fireEvent_impl( const char* name, NDEventArgs& args );

	NDEventSet(NDEventSet&) {}
	NDEventSet& operator=(NDEventSet&) {return *this;}
};

_NDSHAREBASE_END

#endif
