#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "robottlogic.h"
//////////////////////////////////////////////////////////////////////////
//辅助时间
#define TIME_LESS					1									//最少时间

#define IDI_ANDROID_ITEM_SINK		100

//游戏时间
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+0)			//开始定时器
#define IDI_USER_SCORE_END			(IDI_ANDROID_ITEM_SINK+1)			//结算结束
#define IDI_USER_FAN_END			(IDI_ANDROID_ITEM_SINK+2)			//翻牌动画结束
#define IDI_USER_ENDPEICARD			(IDI_ANDROID_ITEM_SINK+3)			//机器人配牌完毕操作
#define IDI_USER_PEICARD			(IDI_ANDROID_ITEM_SINK+4)			//配牌操作
#define IDI_SET_BASESCORE           (IDI_ANDROID_ITEM_SINK+6)			//设置底注
#define IDI_REQUEST					(IDI_ANDROID_ITEM_SINK+5)			//机器人强退
#define IDI_USER_ROBOTDELAY         (IDI_ANDROID_ITEM_SINK+7)			//机器人操作延迟
#define IDI_USER_ADD_SCORE          (IDI_ANDROID_ITEM_SINK+8)			//机器人跟加放弃操作延迟
#define IDI_ADDTAKEGOLD             (IDI_ANDROID_ITEM_SINK+9)           //增加带入金币

//时间标识
#define TIME_START_GAME				6									//开始定时器
#define TIME_USER_ADD_SCORE			3									//加注定时器
#define	TIME_USER_COMPARE_CARD		3									//比牌定时器
//游戏对话框
class CAndroidUserItemSink : public IAndroidUserItemSink
{
public:
	LONG							m_lCellScore;							//单元下注	
	LONG							m_lTableScore[GAME_PLAYER];				//下注数目	
	LONG							m_lTurnMaxScore[GAME_PLAYER];			//剩余的最大下注
	LONG							m_lUserMaxScore[GAME_PLAYER];			//携带的最大注
	LONG                            m_lTurnLessScore;                       //最小下注
	LONG							m_lAddLessScore;                        //最小加注
	BYTE							m_cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态	
	WORD                            m_wCurrentUser;                         //当前用户
	int								m_iChipPoolCount;                       //

	BYTE                            m_bSetBaseScore;
	BYTE							m_cbBalanceCount;						//平衡次数
	CRobottLogic                    m_RobottLogic;
    LONG                            m_lAddTakeGoldMin;        				//带入金币最小限制
	LONG                            m_lAddTakeGoldMax;        				//最大限制
	LONG                            m_lSelfTakeGold;          				//自身携带多少金币
	//控件变量
public:
	IAndroidUserItem *				m_pIAndroidUserItem;					//用户接口	
	BYTE							m_cbHandCardData[GAME_PLAYER][2];       //桌面扑克,0小牌，1中牌，2大牌
	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//是否有效
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool RepositionSink();

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户离开
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户积分
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户状态
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//用户段位
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//机器人设置
	bool OnGameRobot(const void * pBuffer, WORD wDataSize);
    //玩家增加带入金币
	bool OnUserAddTakeGold(const void * pBuffer, WORD wDataSize);
	//用户放弃
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//用户跟，让，加注
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//一轮结束
	bool OnSubTurnOver(const void * pBuffer, WORD wDataSize);
	//中心扑克
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);	
	//游戏结束
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//设置底注
	bool OnSubSetBaseScore(const void * pBuffer, WORD wDataSize);
	//清空变量
	void OnSubClear();
	//打赏处理
	bool OnSubDaShang(void * pData, WORD wDataSize);
	//辅助函数
	bool RobotGameMessage(WORD wChairID, LONG lScore,BYTE ID);
};

//////////////////////////////////////////////////////////////////////////

#endif
