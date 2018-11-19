#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <list>
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
	//玩家变量
protected:
	WORD							m_wDUser;								//D玩家
	WORD							m_wCurrentUser;							//当前玩家
	WORD                            m_wMUser[2];                            //盲注玩家

	//玩家状态
protected:
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbGiveUpStatus[GAME_PLAYER];			//放弃状态

	//加注信息
protected:
	LONG							m_lCellScore;							//单元下注
	LONG							m_lTurnLessScore;						//最小下注
	LONG							m_lAddLessScore;						//加最小注
	LONG							m_lTurnMaxScore[GAME_PLAYER];			//最大下注
	LONG							m_lBalanceScore;						//平衡下注
	LONG                            m_lAllScore;							//总下注
	LONG							m_lTableScore[GAME_PLAYER];				//本轮下注
	LONG							m_lTotalScore[GAME_PLAYER];				//累计下注
	LONG							m_lUserMaxScore[GAME_PLAYER];			//最大下注  玩家总的带入量
	LONG                            m_TabelScoreBuffer[GAME_PLAYER];        //本次下注
	LONG                            m_aryChipPool[GAME_PLAYER + 1];			//边池筹码
	LONG                            m_aryEndScore[GAME_PLAYER];				//结束积分
	//LONG                            m_OnPoolBuffer;							
	WORD							m_wOperaCount;							//操作次数	
	BYTE                            m_ChipPoolCount;                        //边池数量  
	BYTE                            m_SuoHaCountBuffer[4];                  //每一轮梭哈的玩家数量
	BYTE							m_cbShowHand[GAME_PLAYER];				//梭哈用户
	BYTE							m_cbBalanceCount;						//游戏到第几轮
	BYTE                            m_cbChipTypeID;							//下注类型
	BYTE							m_cbHandChair[GAME_PLAYER][GAME_PLAYER];//每个奖池中参与的玩家
	BYTE							m_cbHandChairCount[GAME_PLAYER];		//每个奖池中参与的玩家数量

	//配置参数
	static LONG                     m_ISetDJPK[9];
	bool                            m_bFristPlayStatus[GAME_PLAYER];
	bool                            m_cbShowHandBuffer[GAME_PLAYER];
	bool                            m_GameOver;								//游戏结束
	bool                            m_SendCardEnd[GAME_PLAYER];             //发牌结束	
	LONG                            m_UserAllScore;
	int                             m_UserWinOk;                            //是否启动换牌机制
	int                             m_nPassCardCount;                       //一圈下注里面让牌人数

	//增加带入金币上下限
	//int                             m_lAddTakeMinTimes;    					//增加带入金币最小限倍率
	//int                             m_lAddTakeMaxTimes;    					//增加带入金币最大限倍率
	float                           m_fTableVipLimit;      					//专属房间 9.5倍

	LONG                            m_lUserTakeGold[GAME_PLAYER];			//玩家带入金币数量
	//int                             m_nDaShangCount[GAME_PLAYER];			//玩家打赏次数

	//底注设置
	WORD                            m_pHostChairID;
	bool                            m_bGameStart;                           //游戏开始了
	BYTE                            m_bAgreeBase[GAME_PLAYER];              //同意桌主设置的底注
	LONG                            m_lBaseScore;                           //游戏底注
	typedef std::list<WORD>::iterator IteListPlayer;
	std::list<WORD>					m_listPlayers;                          //当前在桌的非旁观玩家
	std::list<WORD>					m_listConfirmBase;                       //当前在桌的需要确认底注的玩家

	CMD_S_Robot						m_GameRobot;

	//扑克信息
protected:
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbCenterCardData[MAX_CENTERCOUNT];	//中心扑克
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//手上扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						* m_pITableFrame;						//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数

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
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void RepositionSink();

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0; }
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return true; }
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){ return false; }
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){ return false; }
	//设置积分
	virtual void SetGameBaseScore(LONG lBaseScore) { }

	//信息接口
public:
	//游戏状态
	virtual bool IsUserPlaying(WORD wChairID);
	//返回游戏中锁定金币
	virtual SCORE GetGameLockGold(IServerUserItem * pIServerUserItem);
	//动作事件 
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//消息事件
protected:
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//加注事件
	bool OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp);
	//动画结束
	bool OnPlayEnd(WORD wChairID, BYTE PlayId);
	//玩家增加带入金币
	bool OnUserAddTakeGold(WORD wChairID, LONG lAddTake);
	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//辅助函数
public:
	//读取配置文件
	BOOL SetDzpkInit(BYTE ID = 0);
	//交换牌
	bool CardExchange(BYTE ID);
	//杀死所有定时器
	void KillTimerAll();
	//一轮下注完成后操作
	bool OnChipRendEnd();
	//写入真实玩家总收入
	bool WriteAllScore(LONG UserScore[], LONG Tax[]);
	//计算玩家的最大下注
	void CalUserMaxChipGold(WORD wChairID);
	//计算可设置带入金币最小最大限制
	void CalRangeTakeGold(WORD wChairID, LONG & lMin, LONG & lMax);

	//底注相关
	bool IsGenreGold(void);																	//金币场设置底注 并且不是固定底注
	void DoUserSitDown(IServerUserItem * pIServerUser);										//玩家入桌处理 给玩家发送确认底注
	bool SendSetBaseScore(IServerUserItem * pIServerUser, BYTE bSetBaseScore = 1);			// 1 设置 0 等待 2 确认
	bool CheckAgreeBase();																	//给没有确认底注的玩家 发送确认消息
	bool CheckMoneyLimit(LONG lScoreLimit);													//检查玩家的钱是否够最小游戏币限制
	void SendKickOutMsg(WORD ChairID);
	bool OnUserSetBase(CMD_C_SetBaseScore* pBaseScore, IServerUserItem * pIServerUserItem);	//设置底注 客户端响应
	bool OnUserDaShang(WORD wChairID);

	bool OnTimerKickOut();

	//私人场接口;
public:
	//设置私人场信息;
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD	bGameRuleIdex) {};
	//重置私人场结束信息;
	virtual void ResetPrivateEndInfo() {};
	//获得私人场结束信息流;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend) {};
	//判断私人场是否结束;
	virtual	bool IsPrivateEnd() { return false; };

};

//////////////////////////////////////////////////////////////////////////

#endif