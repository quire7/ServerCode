#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

#include <math.h>
#include <fstream>
#include <list>
#include <algorithm>
using namespace std;
//////////////////////////////////////////////////////////////////////////
//枚举定义
//效验类型
enum enEstimatKind
{
	EstimatKind_SendCard,			//发牌效验
	EstimatKind_OutCard,			//出牌效验
	EstimatKind_ChiPengCard,		//吃碰牌效验
	EstimatKind_GangCard,			//杠牌效验
	EstimatKind_BuGangCard,			//补杠牌效验
	EstimatKind_FirstSend,			//首轮发牌
};

enum enGANG_STATUS
{
	GS_NULL = 0,					//初始;
	GS_XIAOGANGKAIHUA = 1,			//小杠开花;
	GS_DAGANGKAIHUA = 2,			//大杠开花;
};

//抢杠胡结果
struct tagQiangGangHu
{
	WORD		wGangPaiUser;		//杠牌玩家
	BYTE		cbGangPaiCard;		//杠牌时的牌
	WORD		wQiangGangHuUser[3];//抢杠胡玩家
	void		ReInit()
	{
		wGangPaiUser = INVALID_CHAIR;
		cbGangPaiCard = 0;
		memset(wQiangGangHuUser, INVALID_CHAIR, sizeof(wQiangGangHuUser));
	}
};

////包牌结果
//struct tagBaoPaiResult
//{
//	bool	    bBaoPaiStatus;		//包牌状态
//	BYTE		cbCurUserIndex;		//包牌时，当前索引
//	WORD		wTargetUser[3];		//包牌时，胡牌玩家ID
//	BYTE		cbBaoPaiType[3];	//包牌时，包牌类型
//};

//////////////////////////////////////////////////////////////////////////
//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
private:
	static BYTE						m_cbOpenGameLottery;					//是否开启抽奖
	static LONG						m_lMaxPlazaExp;							//最大平台经验
	static CString					m_strLucyDrawCfg;						//抽奖配置

	//底注设置
protected:
	typedef std::list<WORD>::iterator IteListPlayer;
	bool							m_bGameStart;                           //游戏开始了
	LONG							m_lBaseScore;							//游戏底注
	WORD							m_wHostChairID;							//桌主ID
	bool							m_bAgreeBaseScore[GAME_PLAYER];         //同意桌主设置的底注
	std::list<WORD>					m_listPlayers;                          //当前在桌的非旁观玩家
	std::list<WORD>					m_listAgreeBaseScore;                   //当前在桌的需要确认底注的玩家

	//游戏变量
protected:
	WORD							m_wEastUser;							//首次东家
	WORD							m_wSiceCount;							//骰子点数
	WORD							m_wBankerUser;							//庄家用户
	LONG							m_lOutCardScore[GAME_PLAYER][2];		//玩家出牌得分 0:奖分 1:罚分
	LONG							m_lGangScore[GAME_PLAYER];				//玩家杠分
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//用户牌
	BYTE							m_cbHuaPaiCount[GAME_PLAYER][2];		//花牌数量 0:硬花, 1:软花
	BYTE							m_cbHuaPaiData[GAME_PLAYER][MAX_HUA_COUNT];//硬花牌数据
	bool							m_bUserTrust[GAME_PLAYER];				//是否托管
	bool							m_bUserOffline[GAME_PLAYER];			//用户掉线
	bool							m_bLastRoundBiXiaHu;					//上一轮是否比下胡
	bool							m_bChangeBanker;						//是否换庄
	bool							m_bHaveJiangFen;						//是否奖分
	bool							m_bHaveFaFen;							//是否罚分
	bool							m_bHaveBigHu;							//是否大胡
	bool							m_bHuaGang;								//是否花杠
	bool							m_bHaveBaozi;							//是否菜包

	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户
	BYTE							m_cbOutCardCount;						//出牌数目
	BYTE							m_cbOutCardData;						//出牌数据
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][72];		//丢弃记录
	BYTE							m_cbDiscardRecordCount[GAME_PLAYER];	//出牌数目记录
	BYTE							m_cbDiscardRecordCard[GAME_PLAYER][72];	//出牌数据记录
	BYTE							m_cbDiscardIndexCount[GAME_PLAYER][MAX_INDEX]; //出牌索引记录
	//BYTE							m_cbBaoZiCardData[GAME_PLAYER][2];		//包子牌 0：打出牌，1：手中留下牌

	//发牌信息
protected:
	bool							m_bSendCardStatus;						//发牌状态
	WORD							m_wSendCardUser;						//摸牌玩家
	WORD							m_wBuHuaUser;							//当前补花用户
	BYTE							m_cbSendCardData;						//发牌牌
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbLeftCardCount;						//剩余数目
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存牌
	BYTE                            m_cbMasterCheckCard[GAME_PLAYER];		//玩家要牌

	//运行变量
protected:
	WORD							m_wCheatChairID;						//作弊玩家坐位置号
	byte							m_cbLeftCheatCardCount;					//剩余补花替换牌的数量
	BYTE							m_cbCheatCard[MAX_COUNT];				//补花要换的牌

	//运行变量
protected:
	WORD							m_wResumeUser;							//还原用户
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wProvideUser;							//供应用户
	BYTE							m_cbProvideCard;						//供应牌
	WORD							m_wGangPaiProvideUser;					//杠牌提供用户

	//
protected:
	WORD							m_wFirstOutUserForFaFen;				//首个出牌玩家
	BYTE							m_cbLastOutCardData;					//上一轮出的牌
	BYTE							m_cbOutSameCardCount;					//一圈内出相同牌的数目
	BYTE							m_cbGangStatus;							//杠牌状态
	bool							m_bHuaGangStatus[GAME_PLAYER][5];		//花杠得分状态
	bool							m_bAwardStatus[GAME_PLAYER];			//出牌奖分状态
	bool							m_bPenaltyStatus[GAME_PLAYER][MAX_NORMAL];//出四张相同牌罚分状态
	tagQiangGangHu					m_QiangGangHuInfo;						//抢杠胡信息

	//用户状态
public:
	bool							m_bResponse[GAME_PLAYER];				//响应标志
	BYTE							m_cbOperateCard[GAME_PLAYER];			//操作牌
	DWORD							m_dwUserAction[GAME_PLAYER];			//用户动作
	DWORD							m_dwPerformAction[GAME_PLAYER];			//执行动作
	bool							m_bDispatchCardOver[GAME_PLAYER];		//发牌完成
	bool							m_bSendCardOver[GAME_PLAYER];			//摸牌动画完成
	bool							m_bBuHuaOver[GAME_PLAYER];				//补花动画完成
	bool							m_bFirstBuHuaOver[GAME_PLAYER];			//起手补花完成
	bool							m_bUserTianTing[GAME_PLAYER];			//玩家天听
	bool							m_bUserTing[GAME_PLAYER];				//玩家听牌状态
	bool							m_bInvalidHuStates[GAME_PLAYER];		//是否是无效的胡牌状态;
	BYTE							m_cbInvalidOperteStates[GAME_PLAYER];	//是否是无效的碰杠状态;
	//bool							m_bFirstSpecialLights[GAME_PLAYER];		//是否是首次;
	//tagSpecialLight					m_SpecialLight[GAME_PLAYER];			//特殊点亮提示;
	//tagBaoPaiResult					m_BaoPaiResult[GAME_PLAYER];			//玩家包牌结果

	//组合牌
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//组合牌

	//结束信息
protected:
	BYTE							m_cbHuPaiCard;							//胡牌牌
	tagHuPaiResult					m_HuPaiResult[GAME_PLAYER];				//胡牌结果
	tagTingPaiResult				m_TingPaiResult[GAME_PLAYER];			//听牌结果
	tagGangPaiResult				m_GangPaiResult[GAME_PLAYER];			//杠牌结果
	tagTingPaiItem					m_HaveTingPaiResult[GAME_PLAYER];		//玩家听后听牌结果

	//录像
public:
	tagGameRecord					m_kGameRecord;

	//私人场结算信息
public:
	CMD_S_Private_End_Info			m_PrivateEndInfo;

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	ITableFrame						*m_pITableFrame;						//框架接口

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	static const tagGameServiceOption *m_pGameServiceOption;				//配置参数

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
	virtual void * QueryInterface(const IID &Guid, DWORD dwQueryVer);

	//管理接口
public:
	//初始化
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//复位桌子
	virtual void RepositionSink();
	//游戏状态
	virtual bool IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//数据事件
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize){ return true; }

	//查询接口
public:
	//查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0L; }
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){ return 0L; }
	//查询服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return false; }
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){ return false; }
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){ return false; }
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore){ return; }

	//任务接口
public:
	//任务是否完成
	virtual bool IsTaskCompleted( WORD wChairID, DWORD dwTaskID, IN OUT DWORD &dwRecord );

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
	virtual bool OnActionUserFangKaCheck(WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure)  { return true; }

	//私人场
public:
	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex) { }
	//重置私人场结束信息
	virtual void ResetPrivateEndInfo();
	//获得私人场结束信息流
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//判断私人场是否结束
	virtual	bool IsPrivateEnd();

	// 录像事件
public:
	// 游戏开始
	void starGameRecord();
	// 玩家操作提示;
	void addGameRecordAction(WORD wSubCmdID, void* pSubMessage, int nMessageSize);

	//游戏事件
protected:
	//玩家设置底注
	bool OnSubSetBaseScore(WORD wChairID, LONG lBaseScore, BYTE cbActionType);
	//开始发牌
	bool OnSubShootDice(WORD wChairID);
	//发牌完成
	bool OnSubDispatchOver(WORD wChairID);
	//摸牌完成
	bool OnSubSendCardOver(WORD wChairID);
	//玩家要牌
	BYTE getSendCardData(WORD dwSendUser);
	//发送排堆;
	void sendMasterLeftCard(int nChairdID);
	//补花要替换的牌
	BYTE getBuHuaCardData(WORD wChairID);
	//补花完成
	bool OnSubBuHuaOver(WORD wChairID);
	//用户出牌
	bool OnSubOutCard(WORD wChairID, BYTE cbCardData);
	//用户操作
	bool OnSubOperateCard(WORD wChairID, DWORD dwOperateCode, BYTE cbOperateCard, bool &bHaveOperate);
	//用户抽奖
	bool OnSubUserLottery(WORD wChairID, BYTE cbLotteryIndex);
	//用户托管
	bool OnSubUserTrust(WORD wChairID, bool bTrust);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//用户操作
	//碰牌操作
	bool PerformPengPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard);
	//杠牌操作
	bool PerformGangPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bCheckQiangGangHu=true);
	//听牌操作
	bool PerformTingPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard);
	//挑战操作
	//bool PerformTiaoZhan(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bZiMo);
	//胡牌操作
	bool PerformHuPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bZiMo);
	//放弃操作
	bool PerformGiveUp(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard);

	//辅助函数
protected:
	//重置变量
	void ResetAllData(bool bAll);
	//玩家摸牌
	bool SendCardData(WORD wChairID, BYTE cbDirection, bool bBanker14=false);
	//发送操作
	bool SendOperateNotify(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//响应判断
	bool EstimateUserRespond(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//检查自己的补花、杠、听、胡操作
	bool EstimatSendCard(WORD wChairID, BYTE cbSendCard);
	//检查其他玩家的碰、杠、胡操作
	bool EstimatOutCard(WORD wChairID, BYTE cbCenterCard);
	//检查碰后的听操作
	bool EstimatChiPengCard(WORD wChairID, BYTE cbCenterCard);
	//杠操作;
	bool EstimatGangCard(WORD wChairID, BYTE cbCenterCard);
	//检查补杠后其他玩家可否胡
	bool EstimatBuGangCard(WORD wChairID, BYTE cbCenterCard);
	//检查玩家补花
	bool EstimatUserBuHua(WORD wChairID);
	//检查玩家能否换听
	bool CheckChangeTingPai(WORD wChairID, tagTingPaiResult TingPaiResult);
	//检查花杠得分
	void CheckHuaGangScore(WORD wChairID);
	//计算罚分
	void CountPayMoneyScore(CMD_S_PayMoney &stPayMoney, BYTE cbPayType, LONG lBaseHuaCount);
	//检查出牌得分
	void CheckOutCardScore(WORD wChairID);
	// 首四风;
	void CheckShouSiFeng(WORD wChairID);
	// 梅四张;
	void CheckMeiSiZhang(WORD wChairID);
	// 四连发;
	void CheckSiLianFa(WORD wChairID);
	//检测胡牌是否有包牌
	void CheckHaveBaoPaiUser(WORD wChairID);
	//检测挑战得分
	void CheckTiaoZhanScore(WORD wChairID, LONG lTiaoZhanScore[GAME_PLAYER]);
	//起手补花是否完成
	bool IsFirstBuHuaOver() { return m_bFirstBuHuaOver[0]&&m_bFirstBuHuaOver[1]&&m_bFirstBuHuaOver[2]&&m_bFirstBuHuaOver[3]; }
	//获取玩家可操作的牌
	BYTE GetOperateCard(WORD wChairID);
	//获取玩家先前胡牌类型
	DWORD GetPreHuPaiKind(WORD wChairID, BYTE cbHuPaiCard);
	//获取听牌时剩余牌数量
	void GetTingCardLastCount(tagTingPaiResult &TingPaiResult, const WORD wChairID);
	//获取牌数目(包括打出去的牌，吃、碰、明杠的牌)
	int GetCardVisualCount(const BYTE cbCardData, const WORD wChairID);	
	//自摸
	bool IsZiMo(WORD wChairID);
	//天胡
	bool IsTianHu(WORD wChairID);
	//地胡
	bool IsDiHu(WORD wChairID);
	//无花果
	bool IsWuHuaGuo(WORD wChairID);
	//压绝
	bool IsYaJue(WORD wChairID, BYTE cbHuPaiCard);
	//海底捞月
	bool IsHaiDiLaoYue(WORD wChairID);
	//小杠开花
	bool IsXiaoGangKaiHua(WORD wChairID);
	//大杠开花
	bool IsDaGangKaiHua(WORD wChairID);
	//抢杠胡
	bool IsQiangGangHu(WORD wChairID);
	//比下胡
	bool IsBiXiaHu();
	//是否有高级操作
	bool IsHaveOperateCard(DWORD dwOperateMask);
	//是否是缺一门
	bool IsQueYiMen(WORD wChairID, BYTE cbCardIndex[MAX_INDEX]);
	//是否只胡无花果
	bool IsOnlyHuWuHuaGuo(DWORD dwHuPaiKind);
	//是否有菜包子
	//bool IsBaoZi(WORD wChairID);
	//预判断是否可以成包子;
	bool AnalyseBaoZi(WORD wChairID, WORD wProvideUser, BYTE cbCenterCard);
	//三碰;
	bool IsSanPeng(WORD wChairID);
	//四碰清一色;
	bool IsSiPengQingyise(WORD wChairID, BYTE cbCenterCard, DWORD dwUserAction);
	//包子提供者;
	WORD GetBaoziProvideUser(WORD wChairID);
	//使用牌库
	void UseCardLibrary();

	//从牌堆中删除一些牌
	bool RemoveCards(BYTE cbCards[],BYTE cbCardCount,BYTE cbRemoveCards[],BYTE cbRemoveCount);
	//是否可以参与抽奖
	//bool IsLuckyDrawEnable( WORD wChairID )const;

	//发送设置底注消息
	bool SendSetBaseScore(WORD wChairID, BYTE cbBaseScoreType);
	//检测通知底注
	bool CheckNotifyBaseScore();
	//检测玩家金币
	bool CheckMoneyLimit(LONG lScoreLimit);
	//踢出玩家
	void SendKickOutMsg(WORD ChairID);

	//发送点亮消息;
	//void SendSpecialLightMsg(WORD wChairID);

public:
	//设置游戏配置
	static void SetGameServerOpertion(tagGameServiceOption *pGso);
	//读取服务器配置
	static bool ReadServerConfig();
	//进程目录
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);

	//私人场辅助函数;
public:
	//是否有相应规则;
	bool IsHasGameRule(eNJMJRuleEnum eType);	
	//是否有包子;
	bool IsGameRuleBaoZi();
	//是否是进园子;
	bool IsGameRuleJYZ();

	// 扣分记录
	void PayMoneyLog(CMD_S_PayMoney & stPayMoney);
	void MyLog(TCHAR *szLog, ...);
};

//////////////////////////////////////////////////////////////////////////

#endif