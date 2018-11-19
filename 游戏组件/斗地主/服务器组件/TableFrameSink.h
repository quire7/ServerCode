#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	WORD							m_wFirstUser;						//首叫用户
	WORD							m_wBankerUser;						//庄家用户
	WORD							m_wCurrentUser;						//当前玩家
	BYTE							m_cbOutCardCount[GAME_PLAYER];		//出牌次数
	BYTE							m_cbWaitTime;						//等待时间
	WORD							m_wTimerControl;					//时间控制	
	bool							m_bUserTrustee[GAME_PLAYER];		//托管标志

	//托管信息
	bool							m_bOffLineTrustee;					//离线托管

	//炸弹信息
protected:
	BYTE							m_cbBombCount;						//炸弹个数
	BYTE							m_cbEachBombCount[GAME_PLAYER];		//炸弹个数

	//叫分信息
protected:
	BYTE							m_cbCallScoreCount;					//叫分次数
	BYTE							m_cbBankerScore;					//庄家叫分
	BYTE							m_cbScoreInfo[GAME_PLAYER];			//叫分信息

	//出牌信息
protected:
	WORD							m_wTurnWiner;						//胜利玩家
	BYTE							m_cbTurnCardCount;					//出牌数目
	BYTE							m_cbTurnCardData[MAX_COUNT];		//出牌数据

	//扑克信息
protected:
	BYTE							m_cbBankerCard[3];					//游戏底牌
	BYTE							m_cbHandCardCount[GAME_PLAYER];		//扑克数目
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];	//手上扑克

	//服务控制
protected:
	HINSTANCE						m_hControlInst;
	IServerControl*					m_pServerControl;

	//组件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CHistoryScore					m_HistoryScore;						//历史成绩


	//组件接口
protected:
	ITableFrame	*					m_pITableFrame;						//框架接口
	tagCustomRule *					m_pGameCustomRule;					//自定规则
	tagGameServiceOption *			m_pGameServiceOption;				//游戏配置
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//游戏属性

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID RepositionSink();
	//配置桌子
	virtual bool Initialization(IUnknownEx * pIUnknownEx);

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore);

	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex){ ; }
	//重置私人场结束信息
	virtual void ResetPrivateEndInfo();
	//获得私人场结束信息流
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//判断私人场是否结束
	virtual	bool IsPrivateEnd();

	//私人场辅助函数;
public:
	//是否有相应规则;
	bool	IsHasGameRule(eDDZRuleEnum eType);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//房卡数量判断
	virtual bool OnActionUserFangKaCheck(WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure) { return true; }

	//游戏事件
protected:
	//用户放弃
	bool OnUserPassCard(WORD wChairID);
	//用户叫分
	bool OnUserCallScore(WORD wChairID, BYTE cbCallScore);
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
	//作弊用户
	bool OnCheatCard(IServerUserItem * pIServerUserItem);
	//发送作弊信息
	void SendCheatCard();
	//断线托管
	bool IsOfflineTrustee();

	//私人场结算信息
public:
	CMD_S_Private_End_Info			m_PrivateEndInfo;
};

//////////////////////////////////////////////////////////////////////////////////

#endif