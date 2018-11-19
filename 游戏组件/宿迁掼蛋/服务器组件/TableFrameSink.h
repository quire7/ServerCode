#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////
//游戏桌子类;
class CTableFrameSink : public ITableFrameSink,public ITableUserAction
{
	//控制变量;
private:
	WORD							m_wFriendFirst;						//友方优先;
	BYTE							m_cbUserTrustee[GAME_PLAYER];		//用户托管 0:NO  1:User 2:System;
	bool							m_bUserOffline[GAME_PLAYER];		//玩家掉线;

	//游戏变量;
protected:
	WORD							m_wBankerUser;						//庄家用户;
	WORD							m_wCurrentUser;						//当前玩家;
	WORD							m_wLastOutUser;						//上家出牌玩家;
	bool                            m_bGameEnd;							//游戏结束;

	//运行信息;
protected:
	WORD							m_wTurnWiner;						//胜利玩家;
	BYTE							m_cbTurnCardType;					//扑克类型
	BYTE							m_cbTurnCardCount;					//出牌数目;
	BYTE							m_cbTurnCardData[27];				//出牌列表;
	BYTE							m_cbMagicCardData[MAX_COUNT];		//变幻扑克

	//游戏状态;
	Series                          m_stSeries;                         //游戏级数;
	BYTE                            m_cbRanking[4];                     //游戏名次;
	BYTE							m_cbUserCurRank[4];					//玩家局即时排名;
	BYTE                            m_cbMark;                           //已完人数;
	BYTE                            m_cbGameStatus;						//胜利类型;
	BYTE							m_cbPayTributeStatus;				//进、回贡状态;
	BYTE                            m_cbPayTribute[4];                  //是否进贡;

	BYTE							m_cbRequestTimes[4];				//请求离开次数;
	list<WORD>						m_lstRequestUserID;					//请求退出玩家ID列表;
	BYTE							m_cbResponseStatus[4];				//回答状态;
	bool							m_bHaveSendRequestQuit;				//已经发送;

	//辅助变量;
protected:
    BYTE                            m_cbTributeCard[4];					//进贡扑克;
	bool							m_bTributeStatus[4];				//进贡状态;
	WORD							m_wTributeBigID;					//进贡大的玩家;
	WORD							m_aryRetTributeTarget[4];			//回贡对象;

	//扑克信息;
protected:
	BYTE							m_cbHandCardCount[4];				//扑克数目;
	BYTE							m_cbHandCardData[4][28];			//手上扑克;

	//级数变量;
protected:
	LONG							m_lBaseScore;						//游戏底注;

	//组件变量;
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑;

	ITableFrame						* m_pITableFrame;					//框架接口;
	tagCustomRule					* m_pGameCustomRule;				//自定规则;
	const tagGameServiceOption		* m_pGameServiceOption;				//配置参数;

	//属性变量;
protected:
	static const WORD				m_wPlayerCount;						//游戏人数;

	//函数定义;
public:
	//构造函数;
	CTableFrameSink();
	//析构函数;
	virtual ~CTableFrameSink();

	//基础接口;
public:
	//释放对象;
	virtual void Release() { if (IsValid()) delete this; return ; }
	//是否有效;
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询;
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口;
public:
	//初始化;
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子;
	virtual VOID RepositionSink();

	//游戏事件;
public:
	//游戏开始;
	virtual bool OnEventGameStart();
	//游戏结束;
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景;
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);


	//事件接口;
public:
	//定时器事件;
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//数据事件;
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//积分事件;
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return true; }

	//比赛接口;
public:
	//设置基数;
	virtual void SetGameBaseScore(LONG lBaseScore){};

	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex){ ; }
	//重置私人场结束信息;
	virtual void ResetPrivateEndInfo();
	//获得私人场结束信息流;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//判断私人场是否结束;
	virtual	bool IsPrivateEnd();

	//查询接口;
public:
	//查询限额;
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0; }
	//最少积分;
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//查询是否扣服务费;
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return true; };

	//网络接口;
public:
	//游戏消息处理;
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理;
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件;
public:
	//用户断线;
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入;
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户坐下;
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立;
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意;
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//房卡数量判断;
	virtual bool OnActionUserFangKaCheck(WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure) { return true; }

	//游戏事件;
protected:

	//用户出牌;
	bool OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
	//用户放弃;
	bool OnSubPassCard(WORD wChairID);
    //用户进贡;
	bool OnSubPayTribute(WORD wChairID, const void * pDataBuffer);
	//用户回贡;
	bool OnSubReturnTribute(WORD wChairID, const void * pDataBuffer);
	//用户托管;
	bool OnSubTrustee(WORD wChairID, const void* pDataBuffer);

	//用户申请退出;
	bool OnSubRequestQuit(const void * pDataBuffer);
	//回复用户退出申请;
	bool OnSubResponseQuit(WORD wMeChiarID, const void * pDatabuffer);
	
	//功能函数;
public:
	//游戏开始;
	bool OnGameStart();
	//进(回)贡超时;
	bool OnTributeTimeOut(BYTE cbTributeType);
	//出牌超时;
	bool OnOutCardTimeOut();
	//发送请求退出消息;
	void SendRequestQuitMsg();
	//回复请求处理;
	bool OnDoResponseQuit(WORD wMeChairID, WORD wRequestID, BYTE cbResponseQuitType);


	//私人场结算信息;
public:
	CMD_S_Private_End_Info			m_PrivateEndInfo;
};

//////////////////////////////////////////////////////////////////////////

#endif