#ifndef IMMEDIATE_GROUP_HEAD_FILE
#define IMMEDIATE_GROUP_HEAD_FILE
#pragma once

//包含文件
#include "TableFrameHook.h"
#include "MatchServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
//时钟定义
#define IDI_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+1)					//删除结束的比赛组
#define IDI_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+2)					//检查一场比赛 
#define IDI_GROUP_TIMER_START		(IDI_MATCH_MODULE_START+10)					//比赛组内的定时器

//时钟定义
#define IDI_CHECK_MATCH_GROUP		(IDI_GROUP_TIMER_START+1)					//轮巡所有开赛比赛组 
#define IDI_SWITCH_WAIT_TIME		(IDI_GROUP_TIMER_START+2)					//换桌等待时间 
#define IDI_LASTMATCHRESULT			(IDI_GROUP_TIMER_START+3)					//结束比赛

//时钟定义
#define IDI_CONTINUE_GAME			1											//继续游戏
#define IDI_LEAVE_TABLE				2											//离开桌子
#define IDI_CHECK_TABLE_START		3											//检查分配桌子是否开始(解决有可能准备后不开始情况)
#define IDI_START_TABLE_GAME		4

///////////////////////////////////////////////////////////////////////////////////////////////////////
//结构定义

//比赛状态
enum enMatchStatus
{
	MatchStatus_Null=0,										//无状态
	MatchStatus_Signup,										//报名中
	MatchStatus_Wait,										//等待桌子
	MatchStatus_FirstRound									//首轮比赛
};

//比赛定时器
struct tagMatchTimer
{
	DWORD					dwTimerID;						//定时器ID
	int						iElapse;						//执行间隔秒	
	WPARAM					wParam;							//附加参数
	LPARAM					lParam;							//附加参数
};

//分数单项
struct tagMatchScore
{
	DWORD					dwUserID;						//用户I D
	SCORE					lScore;							//用户成绩	
	IServerUserItem *		pUserItem;						//用户接口
};

//排名单项
struct tagMatchRanking
{
	DWORD					dwUserID;						//用户I D
	SCORE					lScore;							//用户分数
	LONG					lExpand;						//扩展值
	IServerUserItem *		pUserItem;						//用户接口
};

//比赛桌详情
struct tagTableFrameInfo 
{
	ITableFrame		*		pTableFrame;					//桌子接口
	WORD					wTableID;						//桌子编号
	DWORD					dwBaseScore;					//基础分数	
	BYTE					cbPlayCount;					//游戏局数
	bool					bRoundTableFinish;				//完成标识
	bool					bSwtichTableFinish;				//换桌完成
	bool					bWaitForNextInnings;			//等待下局
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//类型定义

//数组定义
typedef CYSArray<WORD>				CMatchTableIDArray;
typedef CYSArray<tagMatchTimer *>	CUserMatchTimerArray;
typedef CYSArray<tagMatchScore *>	CUserMatchScoreArray;
typedef CYSArray<IServerUserItem *>	CMatchUserItemArray;
typedef CYSArray<tagTableFrameInfo *>	CTableFrameMananerArray;

//字典定义
typedef CMap<DWORD,DWORD,IServerUserItem *, IServerUserItem *>		CUserItemMap;
typedef CMap<IServerUserItem *, IServerUserItem *, DWORD, DWORD>	CUserSeatMap;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//接口定义

//类型声明
class CImmediateGroup;

//分组回调
interface IImmediateGroupSink
{
	//比赛事件
public:
	//开始比赛
	virtual bool OnEventMatchStart(CImmediateGroup *pMatch)=NULL;
	//比赛结束
	virtual bool OnEventMatchOver(CImmediateGroup *pMatch)=NULL;

	//定时器接口
public:
	//删除定时器
	virtual bool KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)=NULL;
	//设置定时器
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)=NULL;	

	//发送函数
public:
	//发送消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//发送消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//发送数据
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	

	//功能函数
public:
	//获取空的桌子
	virtual void GetNullTable(CImmediateGroup* pImmediateGroup, CTableFrameMananerArray& aryTableFrameInfo, WORD wTableCount) = NULL;
	//回收桌子
	virtual void RecoverTable(CTableFrameMananerArray& aryTableFrameInfo) = NULL;
	//获取机器人
	virtual IAndroidUserItem * GetFreeAndroidUserItem()=NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//比赛分组
class CImmediateGroup : public IMatchEventSink
{
	friend class CImmediateMatch;

	//状态信息
protected:
	DWORD							m_dwCurBase;					//当前基数
	DWORD							m_dwMatchNO;					//比赛编号
	enMatchStatus					m_enMatchStatus;				//比赛阶段
	BYTE							m_LoopTimer;					//循环定时器
	WORD							m_wAndroidUserCount;			//机器人个数
	SYSTEMTIME						m_MatchStartTime;				//比赛开始

	//指针变量
protected:
	tagGameMatchOption *			m_pMatchOption;					//比赛规则
	IImmediateGroupSink *			m_pMatchSink;					//管理回调
	tagGameMatchOption::unGameMatchType::tagImmediateMatch* m_pMatchInfo;

	//存储变量
protected:
	CUserItemMap					m_OnMatchUserMap;				//参赛用户
	CMatchUserItemArray				m_FreeUserArray;				//空闲用户
	CTableFrameMananerArray			m_MatchTableArray;				//预定比赛桌子
	CUserMatchTimerArray			m_MatchTimerArray;				//比赛专用定时器	

	//静态变量
protected:
	static WORD						m_wChairCount;					//椅子数目

	//函数定义
public:
	//构造函数
	CImmediateGroup(DWORD dwMatchNO, tagGameMatchOption * pGameMatchOption, IImmediateGroupSink *pIImmediateGroupSink);
	//析构函数
	virtual ~CImmediateGroup();

	//基础接口
public:
	//释放对象
	virtual VOID Release(){}
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//功能函数
public:
	//游戏开始
	virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	//游戏结束
	virtual bool OnEventGameEnd(ITableFrame *pITableFrame, WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送消息
	virtual bool SendData(IServerUserItem *pUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送用户分数
	virtual bool SendUserScore(IServerUserItem * pIUserItem);
	//发送信息
	virtual bool SendUserMessage(IServerUserItem * pIUserItem, LPCTSTR pStrMessage);
	//发送状态
	virtual bool SendUserStatus(IServerUserItem * pIUserItem);

	//发送比赛信息
	virtual void SendMatchInfo(IServerUserItem *pUserItem);

	//用户淘汰
	virtual bool OnUserOut(IServerUserItem *pUserItem, LPCTSTR szReason, WORD wLen, WORD wRank);

	//用户事件
public:
	//玩家返赛
	virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起来
	virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户报名
	bool OnUserSignUpMatch(IServerUserItem *pUserItem);
	//用户退赛
	bool OnUserQuitMatch(IServerUserItem *pUserItem);

	//辅助函数
protected:
	//比赛结束
	void MatchOver();
	//效验比赛开始
	void EfficacyStartMatch();
	//检测参赛人数
	bool CheckMatchUser();
	//玩家排序
	WORD SortMapUser(tagMatchScore score[]);
	//定时器消息
	bool OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//玩家坐下
	bool UserSitDown(IServerUserItem *pUserItem);
	//玩家起立
	bool UserStandUp(IServerUserItem *pUserItem);
	//获取排名
	WORD GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame = NULL);
	//发送提示
	VOID SendWaitTip(ITableFrame *pTableFrame, WORD wChairID = INVALID_CHAIR);
	//获取桌子专用接口
	tagTableFrameInfo* GetTableInterface(ITableFrame* ITable);
	//发送一组信息
	void SendGroupBox(LPCTSTR pStrMessage);
	//设置所有用户离开
	void AllUserExitTable(ITableFrame *pITableFrame);
	//本桌继续游戏
	void ContinueGame(ITableFrame *pITableFrame);
	//获取优先坐下用户
	WORD GetPrecedeSitUser(DWORD dwCurTime);
	//实现该函数可保证Free组唯一
	void InsertFreeGroup(IServerUserItem *pUserItem);
	//发送比赛信息
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//定局排名游戏结束
	void FirstRoundRuleInningsGameEnd(ITableFrame *pITableFrame);

	//标记函数
protected:
	//检测标记
	bool CheckRoundTableFinish();
	//检测标记
	bool CheckSwitchTableFinish();
	//设置标记
	void SetRoundTableFinish(ITableFrame *pITableFrame);
	//设置标记
	void SetSwitchTableFinish(ITableFrame *pITableFrame);

	//定时器函数
protected:
	//捕获定时器
	void CaptureMatchTimer();
	//杀死定时器
	void AllKillMatchTimer();
	//杀死定时器
	bool KillMatchTimer(INT_PTR dwIndexID);
	//杀死定时器
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//投递定时器
	void PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam = 0, LPARAM lParam = 0);
	//杀死定时器
	void KillMatchGameTimer(DWORD dwTimerID);
	//设定定时器
	void SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);

	//游戏事件;
public:
	//用户断线
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; };
	//游戏记录
	virtual bool WriteTableScore(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData) { return true; };
	//判断房卡是否够
	virtual bool OnActionUserFangKaCheck(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure) { return true; };

	//私人场用户事件
	virtual bool AddPrivateAction(ITableFrame* pTbableFrame, DWORD dwChairID, BYTE	bActionIdex) { return true; };
	//统计私人场玩家局数
	virtual bool addPrivatePlayCout(ITableFrame* pTbableFrame, WORD wCout) { return true; };
};

#endif