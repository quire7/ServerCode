#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//数据库类
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//友元定义
	friend class CServiceUnits;

	//辅助变量
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//登录失败
	DBO_GR_LogonSuccess				m_LogonSuccess;						//登录成功

	CRedisInstance					m_RedisInstance;					//缓存服务

	CYSArray<WORD>					m_wArrDailyTaskID;					//日常任务数量

	//账号数据库
protected:
	CDataBaseHelper					m_AccountsDBModule;					//账号数据库

	//游戏币数据库
protected:
	CDataBaseHelper					m_TreasureDBModule;					//游戏币数据库

	//平台数据库
protected:
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//配置变量
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//组件变量
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//引擎接口
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//查询接口
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//数据接口

	//组件变量
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //数据协调

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置参数
public:
	//自定配置
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//服务属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//服务配置
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//获取对象
public:
	//获取对象
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//获取对象
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//功能接口
public:
	//投递结果
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//登录函数
protected:
	//I D 登录
	bool OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch=false);
	//用户重连
	bool OnRequestLogonReLine(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//系统处理
protected:
	//游戏写分
	bool OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//离开房间
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//游戏记录
	bool OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载机器
	bool OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载机器
	bool OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//加载道具
	bool OnRequestLoadGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载用户道具
	bool OnRequestLoadUserProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//礼物请求
	bool OnRequestPropertyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//系统消息
	bool OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛报名
	bool OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//退出比赛
	bool OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛开始
	bool OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛结束
	bool OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//比赛奖励
	bool OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//比赛淘汰
	bool OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//发送邮件
	bool OnRequestSendEMail(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//刷新在线人数
	bool OnRequestRefreshOnlineUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//银行服务
protected:
	//存入游戏币
	bool OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//提取游戏币
	bool OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//查询银行
	bool OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//任务服务
protected:
	//领取奖励
	bool OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//加载任务
	bool OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//查询任务
	bool OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//任务查询
	bool OnQueryTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess);
	//初始化玩家任务
	bool InitUserTask(DWORD dwContextID, DBO_GR_LogonSuccess m_LogonSuccess);
	//任务跟进
	bool OnRequestTaskProgress(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//兑换服务
protected:
	//兑换游戏币
	bool OnRequestExchangeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//辅助函数
private:
	//解锁机器
	bool UnLockAndroidUser();
	//登录结果
	VOID OnLogonDisposeResult(DWORD dwContextID, LPCTSTR lpsPassword, ValueMap& vMap);
	//登录结果
	VOID OnLogonDisposeResult(DWORD dwContextID, SQLRecordset& rs, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbDeviceType = DEVICE_TYPE_PC);
	//银行结果
	VOID OnInsureDisposeResult(DWORD dwContextID, SQLRecordset& rs, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbActivityGame = FALSE);
	//更新缓存数据
	void UpdateCachedData(DWORD dwUserID, tagVariationInfo& VariationInfo, BYTE cbCardType, LPCTSTR pszCardData);
	//获取玩家任务
	void GetUserTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess);
	//玩家进入房间
	void UserEnterGameRoom();
	//从缓存中获取玩家道具
	void GetUserPropFromRedis(DWORD dwContextID, DWORD dwUserID);
	//修改缓存中玩家道具数量
	void ModifyUserPropNumFromRedis(DWORD dwDBID, DWORD dwUserID, WORD wPropID, int nChangeValue);

	//私人场服务
protected:
	bool OnRequestPrivateInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	bool OnRequestCreatePrivate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	bool OnRequestCreatePrivateCost(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//私人房游戏记录;
	bool OnRequestPrivateGameRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//私人房单场游戏记录;
	bool OnRequestSinglePrivateGameRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//私人房结束;
	bool OnRequestPrivateGameEnd(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

#endif