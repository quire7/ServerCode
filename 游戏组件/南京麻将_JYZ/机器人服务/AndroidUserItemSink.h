#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
//#include <algorithm>
//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//机器人时间变量
public:
	static BYTE						m_cbMinReadyTime;						//最小准备时间
	static BYTE						m_cbMaxReadyTime;						//最大准备时间
	static BYTE						m_cbMinOperateCardTime;					//最小操作时间
	static BYTE						m_cbMaxOperateCardTime;					//最大操作时间
	static BYTE						m_cbMinBaseScoreTime;					//最小设置底注时间
	static BYTE						m_cbMaxBaseScoreTime;					//最大设置底注时间

	//用户变量
protected:
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wCurrentUser;							//当前用户
	LONG                            m_lBaseScore;							//房间底注
	BYTE                            m_cbSetBaseScoreStatus;					//设置底注状态

	//状态变量
protected:
	bool							m_bHaveGameEnd;							//游戏是否结束
	bool							m_bTingStatus;							//听牌状态
	DWORD							m_dwActionMask;							//动作掩码
	BYTE							m_cbActionCard;							//动作牌	

	tagTingPaiResult                m_TingResult;							//听牌结果
	tagGangPaiResult                m_GangResult;							//杠牌结果
	//牌
protected:
	BYTE							m_cbLeftCardCount;						//剩余数目
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//手中牌
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存牌

	//组合牌
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];			//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];		//组合牌

	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户
	BYTE							m_cbOutCardData;						//出牌牌
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];		//丢弃记录
	BYTE							m_cbSendCardData;						//发送牌

	//控件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	IAndroidUserItem *				m_pIAndroidUserItem;					//用户接口


	//函数定义
public:
	//构造函数
	CAndroidUserItemSink();
	//析构函数
	virtual ~CAndroidUserItemSink();

	typedef bool (CAndroidUserItemSink::*pSortFun)(BYTE cbCardLeft,BYTE cbCardRight);
	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
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
	//处理设置底注消息
	bool OnSubSetBaseScore(void *pData, WORD wDataSize);
	//掷骰子消息
	bool OnSubShootDice(const void *pBuffer, WORD wDataSize);
	//庄家信息
	bool OnSubGameStart(void * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(void * pData, WORD wDataSize);
	//用户出牌
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//发牌消息
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//补花
	bool OnSubUserBuHua(const void *pBuffer, WORD wDataSize);
	//操作提示
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//操作结果
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//校验出牌
	bool VerdictOutCard(BYTE cbPreCardData);
	//处理函数
protected:
	//重置
	void ReInit();
	//是否幺牌
	bool IsYaoCard( BYTE cbCardData );
	//出牌判断
	bool OnOutCard();
	//吃碰杠听胡判断
	bool OnOperateCard();
	//出牌操作
	void DoOutCard( BYTE cbOutCard );
	//吃碰杠听胡操作
	void DoOperateCard( DWORD dwOperateCode,BYTE cbOperateCard );
	//计算孤牌
	BYTE CalIsolatedCard(BYTE cbCalCards[],BYTE cbIsolateCard[],bool bGetAllIsolate = true);
	//计算左右相邻牌的数量
	BYTE CalNearCardNum(BYTE cbCard);
	//最优操作
	DWORD GetTheBestOperate(DWORD dwOperateCode,BYTE cbOperateCard);
	//计算牌在桌面出现多少张 包含自己手牌
	BYTE CalCardCountInTable(BYTE cbCardData);
	//计算牌在自己手牌有多少张
	BYTE CalCardCountInHand(BYTE cbCardData);
	//从可以打出的牌中间挑选最优出牌
	BYTE ChooseIsolatedCard(IN BYTE cbCanOutCard[],IN BYTE cbCount);
	//获得指定张数的牌
	BYTE GetCardFromCount(OUT BYTE cbCard[],IN const int nCardCount);
	//能否组成顺子 组成顺子类型
	DWORD GetCardShunType(BYTE cbCard);
	//判断手牌对子数量
	BYTE GetDuiZiNum();
	//打缺门 
	bool OutCardSameColor();
	//打七对
	bool OutCardQiDui( IN BYTE cbCanOutCard[],IN BYTE cbCount);
	//获取相同花色牌
	BYTE GetSameColorCard(OUT BYTE cbSameColorCard[],IN BYTE cbColor);
	//从听牌数据中找剩余胡牌张数最多的可以打的牌
	BYTE GetBestTingPutCard(const tagTingPaiResult & TingPaiData);
	//混乱牌
	void RandCardData( BYTE cbCardData[], BYTE cbMaxCount);
	//排序牌组
	void SortCardHeap(BYTE cbCardData[],BYTE cbMaxCount, pSortFun pFun);
	//根据已经打出数量及是否是幺九排序 幺九优先
	bool CompareCard( BYTE cbCard1, BYTE cbCard2 );

public:
	//读取机器人配置
	static bool ReadAndroidConfig();
	//获取机器人操作时间
	static UINT GetAndroidOpTime( UINT uOpType );
	//进程目录
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
};

//////////////////////////////////////////////////////////////////////////

#endif