/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\event\timerEvent\NDTimerEventManager.h
	file base:	NDTimerEventManager
	purpose:	time wheel manager;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_TIMER_EVENT_MANAGER_H__
#define __SHARE_BASE_ND_TIMER_EVENT_MANAGER_H__


#include "NDTimerBoundSlot.h"

_NDSHAREBASE_BEGIN

typedef struct stNodeLink {
	stNodeLink *prev;
	stNodeLink *next;
	stNodeLink() {prev = next = this;} //circle;
}SNodeLink;

typedef struct stTimerNode {
	SNodeLink				link;
	NDTimerBoundSlotConn	timerBoundConn;
	stTimerNode( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs ) 
	: timerBoundConn( new NDTimerBoundSlot( refSubFunctorSlot, refTimerEventArgs ) ) {}
}STimerNode;

typedef struct stWheel {
	SNodeLink	*spokes;
	NDUint32	size;
	NDUint32	spokeindex;
	stWheel(NDUint32 n) : size(n), spokeindex(0){ 
		spokes = new SNodeLink[n];
	}
	~stWheel() { 
		if (spokes) {
			for (NDUint32 j = 0; j < size; ++j) {
				SNodeLink *link = (spokes + j)->next;
				while (link != spokes + j) {
					STimerNode *node = (STimerNode *)link;
					link = node->link.next;
					delete node;
				}
			}
			delete []spokes; 
			spokes = NULL;
		}
	}
}SWheel;


#define GRANULARITY (100) //100ms
#define WHEEL_BITS1 (8)
#define WHEEL_BITS2 (6)
#define WHEEL_SIZE1 (1 << WHEEL_BITS1) //256
#define WHEEL_SIZE2 (1 << WHEEL_BITS2) //64
#define WHEEL_MASK1 (WHEEL_SIZE1 - 1)
#define WHEEL_MASK2 (WHEEL_SIZE2 - 1)
#define WHEEL_NUM	(5)

class NDTimerEventManager
{
private:
	NDTime						m_nStartMSTime;			//开启的时间;
	SNodeLink					m_readnodes;			//可以执行的时间节点;
	SWheel*						m_wheels[WHEEL_NUM];	//时间轮;


	static NDTimerEventManager*	s_pNDTimerEventManager;
public:
	~NDTimerEventManager();

	static	NDTimerEventManager*getInstance();
	static	void				releaseInstance();

	//add timer;
	NDTimerBoundSlotConn		addTimer( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs );
	//检测timelist那些超时;
	void						detectTimerList();

	//打印时间轮内的所有timer;
	NDBool						printTimerList( const NDEventArgs& );

private:
	void						addTimerNode( NDTime nMillSeconds, STimerNode* node );

	void						removeTimer( STimerNode* node );

	void						addToReadyNode( STimerNode* node );

	void						doTimeOutCallBack();

	NDUint32					cascade( NDUint32 nWheelIndex );
	
private:
	NDTimerEventManager();
	NDTimerEventManager( const NDTimerEventManager& other );
	NDTimerEventManager& operator=( const NDTimerEventManager& other );
};




_NDSHAREBASE_END

#endif

