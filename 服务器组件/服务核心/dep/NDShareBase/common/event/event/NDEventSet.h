/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\event\NDEventSet.h
	file base:	NDEventSet
	purpose:	�¼���;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_EVENT_SET_H__
#define __SHARE_BASE_ND_EVENT_SET_H__

#include "NDEvent.h"
#include "stl/NDSTLFunctor.h"

_NDSHAREBASE_BEGIN

//�¼���,���������¼�������,Ҳ��Ϊ��һ���¼������Ľ��зַ��¼�;
class NDEventSet
{
private:
	typedef std::map<char*, NDEvent*, cStrLess>	NDEventMap;
	typedef NDEventMap::iterator				NDEventMapIter;

	NDBool		m_bMuted;	//��������;
	NDEventMap	m_Events;	//�¼���;
	
public:
	NDEventSet();
	virtual ~NDEventSet();

	//����¼�;
	//@param[in] name �¼���;
	void				addEvent( const char* name );

	//�Ƴ��¼�;
	//@param[in] name �¼���;
	void				removeEvent( const char* name );

	//�Ƴ������¼�;
	void				removeAllEvents();

	//�¼��Ƿ����;
	//@param[in] name �¼���;
	bool				isEventPresent( const char* name );

	//�����¼�;
	//@param[in] name �¼���;
	//@param[in] subscriber �¼�������;
	virtual NDEvent::NDConnection	subscribeEvent( const char* name, NDSubFunctorSlot subscriber );

	//@param[in] name �¼���;
	//@param[in] group �¼�����;
	//@param[in] subscriber �¼�������;
	virtual NDEvent::NDConnection	subscribeEvent( const char* name, NDGroup group, NDSubFunctorSlot subscriber);
	
	//�����¼�;
	//@param[in] name �¼���;
	//@param[in] args �¼�����;
	//@param[in] eventNamespace �¼������ռ�;
	virtual void					fireEvent( const char* name, NDEventArgs& args, const char* eventNamespace = "" );

	//�Ƿ���Ӧ�¼�;
	NDBool							isMuted() const;

	//�����Ƿ���Ӧ�¼�;
	void							setMutedState(NDBool setting);

protected:
	//��ȡ�¼�;
	NDEvent*						getEventObject( const char* name, NDBool autoAdd = NDFalse );

	//�������¼�;
	void							fireEvent_impl( const char* name, NDEventArgs& args );

	NDEventSet(NDEventSet&) {}
	NDEventSet& operator=(NDEventSet&) {return *this;}
};

_NDSHAREBASE_END

#endif
