#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

#include "AILogic.h"
//////////////////////////////////////////////////////////////////////////

//机器人类;
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//机器人时间变量;
private:
	static BYTE						m_cbMinSetTableTime;						//最小设置时间
	static BYTE						m_cbMaxSetTableTime;						//最大设置时间
	static BYTE						m_cbMinStartTime;							//最小准备时间
	static BYTE						m_cbMaxStartTime;							//最大准备时间
	static BYTE						m_cbMinTributeTime;							//最小进贡时间
	static BYTE						m_cbMaxTributeTime;							//最大进贡时间
	static BYTE						m_cbMinBackTributeTime;						//最小回贡时间
	static BYTE						m_cbMaxBackTributeTime;						//最大回贡时间
	static BYTE						m_cbMinOutCardTime;							//最小出牌时间
	static BYTE						m_cbMaxOutCardTime;							//最大出牌时间
	static BYTE						m_cbMinSetSeries;							//设置的最小级数
	static BYTE						m_cbMaxSetSeries;							//设置的最大级数
	static bool						m_bLoadConfig;								//是否加载过配置

	//扑克变量;
public:
	BYTE							m_cbHandCardCount;							//扑克数目
	BYTE							m_cbHandCardData[MAX_COUNT];				//手上扑克
	BYTE							m_cbTurnCardCount;							//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];				//出牌列表
	BYTE							m_cbAllCardCount[GAME_PLAYER];				//各玩家手牌数
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];	//各玩家手牌数据
	BYTE							m_cbTributeCard;							//进贡牌
	WORD							m_wFromUser;								//来自用户

	//用户变量;
	WORD							m_wCurrentUser;								//当前操作
	WORD							m_wOutCardUser;								//出牌玩家
	WORD							m_wRequestQuitID;							//请求玩家

	BYTE							m_cbCurSeries;								//级数
	LONG							m_lBaseScore;								//底注

	//控件变量;
protected:
	CAILogic						m_GameLogic;								//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;						//用户接口

	//函数定义;
public:
	//构造函数;
	CAndroidUserItemSink();
	//析构函数;
	virtual ~CAndroidUserItemSink();

	//基础接口;
public:
	//释放对象;
	virtual VOID Release() { delete this; }
	//接口查询;
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口;
public:
	//初始接口;
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口;
	virtual bool RepositionSink();

	//游戏事件;
public:
	//时间消息;
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息;
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息;
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息;
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件;
public:
	//用户进入;
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//用户离开;
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//用户积分;
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//用户状态;
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//用户段位;
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}

	//消息处理;
protected:
	//游戏开始;
	bool OnSubSendCard(void * pData, WORD wDataSize);
	//进贡通知;
	bool OnSubTributeCardNotify(void * pData, WORD wDataSize);
	//进贡扑克;
	bool OnSubTributeOneCard(void * pData, WORD wDataSize);
	//进贡结束;
	bool OnSubTributeCardResult(void * pData, WORD wDataSize);
	//回贡;
	bool OnSubReturnTribute(void * pData, WORD wDataSize);
	//游戏开始;
	bool OnSubStartOutCard(void * pData, WORD wDataSize);
	//用户出牌;
	bool OnSubOutCard(void * pData, WORD wDataSize);
	//用户放弃;
	bool OnSubPassCard(void * pData, WORD wDataSize);
	//游戏结束;
	bool OnSubGameEnd(void * pData, WORD wDataSize);
	//用户申请退出;
	bool OnSubRequestQuit(void * pData, WORD wDataSize);
	//获取玩家所有手牌;
	bool OnSubGetAllCard(void * pData, WORD wDataSize);

	//功能函数;
public:
	//读取机器人配置;
	static bool ReadAndroidConfig();

protected:
	//获取机器人操作时间;
	UINT GetAndroidOpTime(UINT uOpType);
	//出牌函数;
	void SearchOutCardData();
};

//////////////////////////////////////////////////////////////////////////

#endif