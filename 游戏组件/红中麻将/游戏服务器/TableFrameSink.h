#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//枚举定义

//效验类型
enum enEstimatKind
{
	EstimatKind_OutCard,			//出牌效验;
	EstimatKind_GangCard,			//杠牌效验;
};

//杠牌得分
struct tagGangScore
{
	BYTE		cbGangCount;							//杠个数;
	LONGLONG	lScore[MAX_WEAVE][GAME_PLAYER];			//每个杠得分;
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//游戏变量
protected:
	WORD							m_wBankerUser;							//庄家用户;
	WORD							m_wEastUser;							//首次东家;
	LONGLONG						m_lGameScore[GAME_PLAYER];				//游戏得分;
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//用户扑克;
	bool							m_bTrustee[GAME_PLAYER];				//是否托管;
	tagGangScore					m_GangScore[GAME_PLAYER];				//

	WORD							m_wLostFanShu[GAME_PLAYER][GAME_PLAYER];//


	BYTE							m_cbCardDataNiao[MAX_NIAO_CARD];		// 鸟牌;
	BYTE							m_cbNiaoCount;							//鸟牌个数;
	BYTE							m_cbNiaoPick;							//中鸟个数;
	BYTE							m_cbOneShootScore;						//一码中特分数;

	bool							m_bCanTrustee;							//是否托管;

	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户;
	BYTE							m_cbOutCardData;						//出牌扑克;
	BYTE							m_cbOutCardCount;						//出牌数目;
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目;
	BYTE							m_cbDiscardCard[GAME_PLAYER][55];		//丢弃记录;

	//发牌信息
protected:
	BYTE							m_cbSendCardData;						//发牌扑克;
	BYTE							m_cbSendCardCount;						//发牌数目;
	BYTE							m_cbLeftCardCount;						//剩余数目;
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存扑克;
	BYTE							m_cbRepertoryCard_HZ[MAX_REPERTORY_HZ];	//库存扑克;

	BYTE                            m_cbMasterCheckCard[GAME_PLAYER];		//玩家要牌;

	//运行变量
protected:
	WORD							m_wResumeUser;							//还原用户;
	WORD							m_wCurrentUser;							//当前用户;
	WORD							m_wProvideUser;							//供应用户;
	BYTE							m_cbProvideCard;						//供应扑克;

	//状态变量
protected:
	bool							m_bSendStatus;							//发牌状态;
	bool							m_bGangStatus;							//抢杆状态;
	bool							m_bGangOutStatus;						//

	//用户状态
public:
	bool							m_bResponse[GAME_PLAYER];				//响应标志;
	DWORD							m_cbUserAction[GAME_PLAYER];			//用户动作;
	BYTE							m_cbOperateCard[GAME_PLAYER];			//操作扑克;
	DWORD							m_cbPerformAction[GAME_PLAYER];			//执行动作;

	LONGLONG						m_lStartHuScore[GAME_PLAYER];			//起手胡分数;
	CChiHuRight						m_StartHuRight[GAME_PLAYER];			//起手胡权位;

	tagTingPaiResult				m_TingPaiResult[GAME_PLAYER];			//听牌结果;

	//组合扑克
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//组合数目;
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//组合扑克;

	//结束信息
protected:
	BYTE							m_cbChiHuCard;							//吃胡扑克;
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//吃胡结果;
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];				//
	WORD							m_wProvider[GAME_PLAYER];				//

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑;
	ITableFrame						* m_pITableFrame;						//框架接口;
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数;

	BYTE							m_cbGameTypeIdex;						//游戏类型;
	DWORD							m_dwGameRuleIdex;						//游戏规则;
	BYTE							m_cbMagicCardIndex;
	BYTE							m_cbMagicCardData;

	//函数定义;
public:
	//构造函数;
	CTableFrameSink();
	//析构函数;
	virtual ~CTableFrameSink();

	//基础接口;
public:
	//释放对象;
	virtual VOID Release() { }
	//接口查询;
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口;
public:
	//初始化;
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子;
	virtual VOID RepositionSink();

	//查询接口;
public:
	//查询限额;
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem){  return 0; };
	//最少积分;
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0; };
	//查询是否扣服务费;
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//比赛接口;
public:
	//设置基数;
	virtual void SetGameBaseScore(LONG lBaseScore){};

	//游戏事件;
public:
	//游戏开始;
	virtual bool OnEventGameStart();
	//游戏结束;
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景;
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	void Shuffle(BYTE* RepertoryCard,int nCardCount); //洗牌;

	//事件接口;
public:
	//定时器事件;
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件;
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//积分事件;
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return false; }

	//网络接口;
public:
	//游戏消息处理;
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理;
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件;
public:
	//用户断线;
	virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户重入;
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下;
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立;
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意;
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize) { return true; }

	//游戏事件;
protected:
	//用户出牌;
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData);
	//用户操作;
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard, bool bCheckQiangGangHu = true);
	//用户托管;
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	
public:
	bool hasRule(DWORD cbRule);

	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);

	//判断和处理4个红中的情况;
	BYTE AnalyseChiHuCardZZ_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight);

	BYTE AnalyseChiHuCardCS_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight);

	bool DisposeZZXiaohu(const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//私人场接口;
public:
	//设置私人场信息;
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex);
	//重置私人场结束信息;
	virtual void ResetPrivateEndInfo();
	//获得私人场结束信息流;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//判断私人场是否结束;
	virtual	bool IsPrivateEnd();

	//辅助函数;
protected:
	//发送操作;
	bool SendOperateNotify();
	//派发扑克;
	bool DispatchCardData(WORD wCurrentUser,bool bTail=false);
	//响应判断;
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);

	//
	void ProcessChiHuUser( WORD wChairId, bool bGiveUp );
	//
	void FiltrateRight( WORD wChairId,CChiHuRight &chr );

	//获得扎码数(或者鸟);
	BYTE GetNiaoCardNum(WORD wHuChairID );
	void setNiaoCard( WORD wHuChairID );

	//是否包含有红中牌;
	bool HasHongZhongCard( WORD wChairID );
	//计算一码中特的点数;
	BYTE CalcOneShootScore( BYTE cbCardData );

	//是否听牌,摸牌后及吃碰后,检查听;
	bool IsTingPai(WORD wChairID);

	// 获取要牌;
	BYTE GetSendCardData(WORD dwSendUser);
};

//////////////////////////////////////////////////////////////////////////

#endif
