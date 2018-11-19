#include "StdAfx.h"
#include "ImmediateMatch.h"
#include "..\游戏服务器\DataBasePacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_VALUE				0xFFFF								//无效值

//////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+100)		//分配用户

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CImmediateMatch::CImmediateMatch()
{
	//比赛配置
	m_pMatchOption=NULL;	
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//设置指针
	m_ppITableFrame=NULL;
	m_pCurMatchGroup=NULL;

	//内核接口
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//服务接口
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
	m_pIServerUserItemSink=NULL;

	memset(&m_MatchDesc, 0, sizeof(m_MatchDesc));
}

CImmediateMatch::~CImmediateMatch(void)
{
	SafeDeleteArray(m_ppITableFrame);
	m_aryFreeTableID.RemoveAll();
	
	m_LoopTimer.RemoveAll();
	SafeDelete(m_pCurMatchGroup);
	m_MatchGroup.Append(m_OverMatchGroup);
	for (int i=0; i<m_MatchGroup.GetCount(); i++)
	{
		SafeDelete(m_MatchGroup[i]);
	}

	m_MatchGroup.RemoveAll();
	m_OverMatchGroup.RemoveAll();
}

//接口查询
VOID* CImmediateMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);	
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//启动通知
void CImmediateMatch::OnStartService()
{
	//创建分组
	if (m_pCurMatchGroup==NULL)
	{
		m_pCurMatchGroup=new CImmediateGroup((DWORD)time(NULL),m_pMatchOption,this);
	}

	//最多同时9组编号
	for (int i=0;i<79;i++)
	{
		m_LoopTimer.Add(i);
	}
	m_pITimerEngine->SetTimer(IDI_CHECK_OVER_MATCH,60000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_DELETE_OVER_MATCH, 6000,TIMES_INFINITY,0);
//#ifdef _DEBUG
//	m_pITimerEngine->SetTimer(IDI_ANDROID_SIGNUP,1000,TIMES_INFINITY,0);
//#else
//	m_pITimerEngine->SetTimer(IDI_ANDROID_SIGNUP,3000,TIMES_INFINITY,0);
//#endif
}

//绑定桌子
bool CImmediateMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//创建钩子
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//设置接口
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//初始化接口
bool CImmediateMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//服务配置
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;
	m_pImmediate = &(m_pMatchOption->MatchType.Immediate);

	//内核组件
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//服务组件		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//创建桌子数组
	CImmediateGroup::m_wChairCount = m_pGameServiceAttrib->wChairCount;
	if (m_pMatchOption->cbMatchType == MATCH_TYPE_IMMEDIATE)
	{
		WORD wStartUserCount = m_pImmediate->wStartUserCount;
		if (CImmediateGroup::m_wChairCount > wStartUserCount)
		{
			CImmediateGroup::m_wChairCount = wStartUserCount;
		}
	}

	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];

		for (WORD i = 0; i < m_pGameServiceOption->wTableCount; i++)
		{
			m_aryFreeTableID.Add(i);
		}
	}

	return true;
}

//时间事件
bool CImmediateMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	if(dwTimerID>=IDI_GROUP_TIMER_START)
	{
		for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
		{
			//查看是哪场比赛的定时器
			if (m_MatchGroup[i]->m_LoopTimer==((dwTimerID-IDI_GROUP_TIMER_START)/10))
			{
				m_MatchGroup[i]->OnTimeMessage(dwTimerID-(m_MatchGroup[i]->m_LoopTimer*10),dwBindParameter);
			}
		}
		return true;
	}

	switch(dwTimerID)
	{
	case IDI_CHECK_OVER_MATCH:
		{
			//定时检测一下 那些比赛人数不够被卡的问题
			for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
			{
				//参赛用户小于椅子数量 就要结束
				m_MatchGroup[i]->CheckMatchUser();
			}
			return true;
		}
	case IDI_DELETE_OVER_MATCH:
		{
			for (int i=0;m_OverMatchGroup.GetCount();i++)
			{
				CImmediateGroup * pOverMatch=m_OverMatchGroup[i];
				m_OverMatchGroup.RemoveAt(i--);
				SafeDelete(pOverMatch);
			}
			return true;
		}
	}

	return true;
}


//数据库事件
bool CImmediateMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//报名结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//报名失败
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//加入比赛
			if(m_pCurMatchGroup->OnUserSignUpMatch(pIServerUserItem)==false)
			{
				SendRoomMessage(pIServerUserItem, TEXT("报名失败"),SMT_CHAT);
				return false;
			}

			//报名成功
			m_OnMatchUserItem.Add(pIServerUserItem);
			//SendMatchInfo(NULL);
			if(m_pIGameServiceFrame!=NULL)
			{
				SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchSignupResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pIServerUserItem->GetUserInfo()->dwExperience;

				//发送数据
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//退赛结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//提取数据
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//退费失败
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//设置状态
			pIServerUserItem->SetSignUpStatus(MatchStatus_Null);

			//变量定义
			LPCTSTR pszMatchFeeType[]={TEXT("游戏币"),TEXT("元宝")};
			if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
			{
				//退费通知
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("退赛成功，退还报名费%I64d%s！"), m_pMatchOption->lMatchFee, pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}

			//金币更新
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchSignupResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pIServerUserItem->GetUserInfo()->dwExperience;

				//发送数据
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}	
	case DBO_GR_MATCH_RANK_LIST:			//比赛排行
		{
			//参数校验
			ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//奖励结果
		{
			//参数效验
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBR_GR_MatchRewardResult)) return false;

			//提取数据
			DBR_GR_MatchRewardResult * pMatchRewardResult = (DBR_GR_MatchRewardResult*)pData;

			//奖励成功
			if(pMatchRewardResult->bResultCode==true)
			{
				//构造结构
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//设置变量
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchRewardResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pMatchRewardResult->dwCurrExperience;

				//发送数据
				SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	}
	return true;
}

//命令消息
bool CImmediateMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//比赛费用
		{
			//参数效验
			ASSERT(wDataSize==sizeof(SCORE));
			if(wDataSize!=sizeof(SCORE)) return false;

			//提取数据
			SCORE lMatchFee=*(SCORE*)pData;
			if(lMatchFee!=m_pMatchOption->lMatchFee)return false;

			//变量定义
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//构造结构
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();
			MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//退出比赛
		{
			//参数效验
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			OnEventUserQuitMatch(pIServerUserItem,0,0,dwSocketID);

			return true;
		}
	}
	return true;
}

//用户登录
bool CImmediateMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//return SendMatchInfo(pIServerUserItem);
	pIServerUserItem->SetClientReady(true);
	return true;
}

//用户登出
bool CImmediateMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//进入事件
bool CImmediateMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//处理消息
	CMD_GR_LogonUserID * pLogonUserID = (CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword) - 1] = 0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID) - 1] = 0;

	//变量定义
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwClientAddr = dwUserIP;
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	LogonUserID.dwMatchID = m_pMatchOption->dwMatchID;
	LogonUserID.dwMatchNO = m_pMatchOption->dwMatchNO;
	lstrcpyn(LogonUserID.szPassword, pLogonUserID->szPassword, CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID, pLogonUserID->szMachineID, CountArray(LogonUserID.szMachineID));

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID, DBR_GR_LOGON_USERID, dwSocketID, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//用户参赛
bool CImmediateMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL)
	{
		CTraceService::TraceString(TEXT("用户指针为空!"),TraceLevel_Warning);
		return true;
	}

	if(CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule))
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，由于系统维护，该房间禁止用户报名比赛！。"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_pCurMatchGroup==NULL || m_pCurMatchGroup->m_enMatchStatus==MatchStatus_Wait)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，该房间参赛人数已满，请稍后再报名或者进入另一房间比赛。"),SMT_CHAT);
		return true;
	}

	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]->GetUserID()==pIServerUserItem->GetUserID())
		{
			SendRoomMessage(pIServerUserItem, TEXT("您已经成功报名，不能重复报名！"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	if(m_LoopTimer.GetCount()==0)
	{
		SendRoomMessage(pIServerUserItem, TEXT("抱歉，比赛编号不够请稍等。"),SMT_CHAT);
		return true;
	}

	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("对不起，您当前的状态不允许参加比赛！"),SMT_CHAT);
		return true;
	}

	if (m_pImmediate->dwInitalScore==0&&pIServerUserItem->GetUserScore()<m_pImmediate->dwInitalBase*m_pImmediate->wMinEnterGold+m_pMatchOption->lMatchFee)
	{
		TCHAR szMsg[256]=TEXT("");
		_sntprintf_s(szMsg, CountArray(szMsg), TEXT("对不起，您当前的金币低于了%d，不能参加比赛！"),
			m_pImmediate->dwInitalBase*m_pImmediate->wMinEnterGold+m_pMatchOption->lMatchFee);
		SendRoomMessage(pIServerUserItem, szMsg,SMT_CHAT|SMT_EJECT);
		return true;
	}

	//真人扣报名费
	if(m_pMatchOption->lMatchFee>0 && pIServerUserItem->IsAndroidUser()==false)
	{
		ASSERT(m_pIGameServiceFrame!=NULL);
		if(m_pIGameServiceFrame==NULL)
		{
			CTraceService::TraceString(TEXT("服务框架指针为空!"),TraceLevel_Warning);
			return true;
		}

		//构造结构
		CMD_GR_Match_Fee MatchFee;
		MatchFee.lMatchFee=m_pMatchOption->lMatchFee;

		//构造提示
		if(MatchFee.lMatchFee>0)
		{
			//变量定义
			LPCTSTR pszMatchFeeType[]={TEXT("游戏币"),TEXT("元宝")};
			if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
			{
				_sntprintf_s(MatchFee.szNotifyContent, CountArray(MatchFee.szNotifyContent), TEXT("参赛将扣除报名费 %I64d %s，确认要参赛吗？"), m_pMatchOption->lMatchFee, pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);
			}
		}

		//发送消息
		WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

		return true;
	}

	//机器人报名
	if(m_pMatchOption->lMatchFee==0 || pIServerUserItem->IsAndroidUser()==true)
	{
		//机器数判断
		if(pIServerUserItem->IsAndroidUser()==true && (m_pCurMatchGroup->m_wAndroidUserCount >= 
		   m_pImmediate->wAndroidUserCount))
		{
			return true;
		}

		return OnEventSocketMatch(SUB_GR_MATCH_FEE,&m_pMatchOption->lMatchFee,sizeof(SCORE),pIServerUserItem,dwSocketID);
	}

	//报名成功
	m_OnMatchUserItem.Add(pIServerUserItem);
	//SendMatchInfo(NULL);
	if(m_pIGameServiceFrame!=NULL)
	{
		SCORE lScore=SCORE(m_pMatchOption->lMatchFee)*-1;

		SendRoomMessage(pIServerUserItem, TEXT("恭喜，报名成功，请等待系统为您安排座位！"),SMT_CHAT);
	}

	return true;
}

//用户退赛
bool CImmediateMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//正在游戏状态
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//正在比赛阶段
	BYTE cbUserEnlist=pIServerUserItem->GetSignUpStatus();
	if (cbUserEnlist==MS_MATCHING)
	{
		return true;
	}

	//否则玩家退赛
	if(RemoveMatchUserItem(pIServerUserItem))
	{
		//从正在比赛的组中删除该玩家
		INT_PTR nGroupCount=m_MatchGroup.GetCount();
		for (INT_PTR i=0;i<nGroupCount;i++)
		{
			CImmediateGroup *pMatch=m_MatchGroup[i];
			if(pMatch->OnUserQuitMatch(pIServerUserItem))
			{
				//if(pMatch->m_enMatchStatus!=MatchStatus_Signup && pMatch->m_enMatchStatus!=MatchStatus_Wait)
				//{
				//	SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
				//}
				if (pMatch->m_enMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
				}
				return true;
			}
		}

		//从正在报名的组中删除该玩家
		if(m_pCurMatchGroup->OnUserQuitMatch(pIServerUserItem))
		{
			//退还报名费
			if(m_pMatchOption->lMatchFee>0 && !pIServerUserItem->IsAndroidUser())
			{
				//变量定义
				DBR_GR_MatchSignup MatchSignup;
				MatchSignup.dwUserID=pIServerUserItem->GetUserID();

				//构造结构
				MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
				MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
				MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
				MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
				MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
				lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

				//投递数据
				m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchSignup,sizeof(MatchSignup));
			}

			if (m_pCurMatchGroup->m_enMatchStatus!=MatchStatus_Null)
			{
				SendRoomMessage(pIServerUserItem, TEXT("退赛成功，欢迎您继续参加其他比赛！"), SMT_CHAT|SMT_EJECT);
			}

			//SendMatchInfo(NULL);
			return true;
		}
		//SendMatchInfo(NULL);
	}

	return false;
}

//用户积分
bool CImmediateMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//获取对象
	CImmediateGroup * pImmediateGroup = static_cast<CImmediateGroup *>(pIServerUserItem->GetMatchData());
	if(pImmediateGroup==NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,cbReason);
	}

	//发送分数
	pImmediateGroup->SendUserScore(pIServerUserItem);

	//变量定义
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

	//变量定义
	CMD_GR_UserScore UserScore;

	//构造数据
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	UserScore.UserScore.dwExperience = pUserInfo->dwExperience;

	//构造积分
	UserScore.UserScore.lScore = pUserInfo->lScore;
	UserScore.UserScore.lScore += pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore += pIServerUserItem->GetFrozenedScore();

	//发送数据
	m_pIGameServiceFrame->SendDataToTable(pIServerUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));

	//即时写分
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//提取积分
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//比赛信息
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.dwMatchNO=pImmediateGroup->m_dwMatchNO;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);
	}

	return true;
}

//用户状态
bool CImmediateMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//清除数据
	if (pIServerUserItem->GetUserStatus() == US_FREE)
	{
		pIServerUserItem->SetMatchData(NULL);
	}

	//回调接口
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wOldTableID,wOldChairID);
	}

	return true;
}

//用户权限
bool CImmediateMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//报名人满，开始比赛
bool CImmediateMatch::OnEventMatchStart(CImmediateGroup *pMatch)
{
	ASSERT(pMatch==m_pCurMatchGroup&&m_LoopTimer.GetCount()>0);
	
	//编号不够 不能报名的
	pMatch->m_LoopTimer=m_LoopTimer[0];
	m_LoopTimer.RemoveAt(0);
	m_MatchGroup.Add(pMatch);

	//准备新的一组比赛
	m_pCurMatchGroup=new CImmediateGroup((DWORD)time(NULL),m_pMatchOption,this);

	//SendMatchInfo(NULL);
	BYTE cbMatchStatus=MS_MATCHING;
	pMatch->SendData(NULL, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

	return true;
}


//比赛结束
bool CImmediateMatch::OnEventMatchOver(CImmediateGroup *pMatch)
{
	//比赛结束
	if(pMatch!=NULL)
	{
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver,sizeof(MatchOver));

		//构造数据						
		MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
		MatchOver.dwMatchNO=pMatch->m_dwMatchNO;		
		MatchOver.cbMatchType=MATCH_TYPE_IMMEDIATE;
		MatchOver.wServerID=m_pGameServiceOption->wServerID;
		
		//获取时间
		MatchOver.MatchStartTime=pMatch->m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(0,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
	}

	return true;
}

//关闭定时器
bool CImmediateMatch::KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)
{
	m_pITimerEngine->KillTimer(dwTimerID);

	return false;
}

//设置定时器
bool CImmediateMatch::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)
{
	return m_pITimerEngine->SetTimer(dwTimerID,dwElapse,dwRepeat,dwBindParameter);
}


//发送数据
bool CImmediateMatch::SendMatchInfo(IServerUserItem * pIServerUserItem)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting=(DWORD)m_pCurMatchGroup->m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal=m_pImmediate->wStartUserCount;
	if(pIServerUserItem==NULL)
	{
		m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		return true;
	}

	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			BYTE cbMatchStatus=pIServerUserItem->GetSignUpStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
			return true;
		}
	}

	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount=pUserScore->dwLostCount=pUserScore->dwFleeCount=pUserScore->dwDrawCount=pUserScore->cbGender=0;
	if((SCORE)m_pImmediate->dwInitalScore!=0)
	{
		pUserScore->lScore=(LONG)m_pImmediate->dwInitalScore;
	}

	//SendGroupUserScore(pIServerUserItem, NULL);
	return true;
}

//发送游戏消息
bool CImmediateMatch::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	return m_pIGameServiceFrame->SendGameMessage(pIServerUserItem, lpszMessage, wMessageType);
}

//发送消息
bool CImmediateMatch::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	return m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, lpszMessage, wMessageType);
}

//发送数据
bool CImmediateMatch::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (pIServerUserItem != NULL)
	{
		return m_pIGameServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	else
	{
		return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, wMainCmdID, wSubCmdID, pData, wDataSize);
	}

	return true;
}

//为首轮插入空桌子
void CImmediateMatch::GetNullTable(CImmediateGroup* pImmediateGroup, CTableFrameMananerArray& aryTableFrameInfo, WORD wTableCount)
{
	ASSERT(m_aryFreeTableID.GetCount() >= wTableCount);
	if (m_aryFreeTableID.GetCount()<wTableCount)
	{
		return;
	}

	//插入桌子
	for (WORD i = 0; i < wTableCount; i++)
	{
		WORD wFreeTableID = m_aryFreeTableID[i];
		m_aryFreeTableID.RemoveAt(i);

		tagTableFrameInfo * pTableFrameInfo = new tagTableFrameInfo;
		ZeroMemory(pTableFrameInfo, sizeof(tagTableFrameInfo));
		pTableFrameInfo->wTableID = wFreeTableID;
		pTableFrameInfo->pTableFrame = m_ppITableFrame[wFreeTableID];
		pTableFrameInfo->dwBaseScore = m_pImmediate->dwInitalBase;
		ITableFrameHook * pFrameHook = QUERY_OBJECT_PTR_INTERFACE(m_ppITableFrame[wFreeTableID]->GetTableFrameHook(), ITableFrameHook);
		pFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(pImmediateGroup, IUnknownEx));
		aryTableFrameInfo.Add(pTableFrameInfo);
	}
}

//回收桌子
void CImmediateMatch::RecoverTable(CTableFrameMananerArray& aryTableFrameInfo)
{
	INT_PTR nCount = aryTableFrameInfo.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		m_aryFreeTableID.Add(aryTableFrameInfo[i]->wTableID);
	}
}

//获取一个空闲的机器人
IAndroidUserItem * CImmediateMatch::GetFreeAndroidUserItem()
{

	if(m_pAndroidUserManager->GetAndroidCount()==0) return NULL;

	//机器状态
	tagAndroidUserInfo AndroidSimulate;
	m_pAndroidUserManager->GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE|ANDROID_PASSIVITY|ANDROID_INITIATIVE);

	if (AndroidSimulate.wFreeUserCount == 0)
	{
		return NULL;
	}
	else
	{
		//避免已报名的机器人重复安排
		for(WORD i=0; i<AndroidSimulate.wFreeUserCount;i++)
		{
			IAndroidUserItem * pIAndroidUser = AndroidSimulate.pIAndroidUserFree[i];
			if (pIAndroidUser->GetMeUserItem()->GetSignUpStatus() == MatchStatus_Null)
			{
				ASSERT(pIAndroidUser->GetUserID() == pIAndroidUser->GetMeUserItem()->GetUserID());
				return pIAndroidUser;
			}
		}
	}

	return NULL;
}

//写入奖励
bool CImmediateMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL && pMatchRankInfo!=NULL);
	if(pIServerUserItem==NULL || pMatchRankInfo==NULL) return false;

	//写入判断
	bool bWriteReward=(pMatchRankInfo->lRewardGold+pMatchRankInfo->dwRewardIngot+pMatchRankInfo->dwRewardExperience)>0;

	//写入奖励
	if(bWriteReward==true)
	{
		//变量定义
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult,sizeof(MatchResult));

		//比赛奖励
		MatchResult.lGold += pMatchRankInfo->lRewardGold;
		MatchResult.dwIngot+=pMatchRankInfo->dwRewardIngot;
		MatchResult.dwExperience+=pMatchRankInfo->dwRewardExperience;

		//获奖提示
		_sntprintf_s(MatchResult.szDescribe, CountArray(MatchResult.szDescribe), TEXT("%s, 恭喜您在%s中获得第%d名,奖励如下："),
			pIServerUserItem->GetNickName(),m_pMatchOption->szMatchName,pMatchRankInfo->wRankID);

		//更新用户信息
		tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
		//pUserInfo->lIngot+=pMatchRankInfo->dwRewardIngot;
		pUserInfo->dwExperience+=pMatchRankInfo->dwRewardExperience;
		//SendGroupUserScore(pIServerUserItem, NULL);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
		}
	}
	else
	{
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("比赛已结束，恭喜您获得第%d名，欢迎您参加其他场次的比赛！"), pMatchRankInfo->wRankID);
		SendGameMessage(pIServerUserItem, szMessage, SMT_CLOSE_GAME);
		SendRoomMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT);

		return true;
	}

	//写入记录
	if(m_pIDataBaseEngine!=NULL)
	{
		//变量定义
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));
		
		//构造结构
		MatchReward.dwUserID=pIServerUserItem->GetUserID();	
		MatchReward.lRewardGold = pMatchRankInfo->lRewardGold;
		MatchReward.dwRewardIngot=pMatchRankInfo->dwRewardIngot;
		MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
		MatchReward.dwClientAddr=pIServerUserItem->GetClientAddr();

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	}

	return true;
}

//删除用户
bool CImmediateMatch::DeleteUserItem(DWORD dwUserIndex)
{
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwUserIndex,0,0L);
	}
	catch (...)
	{
		//错误断言
		ASSERT(FALSE);
		return false;
	}
	return true;
}

//移除参赛用户
bool CImmediateMatch::RemoveMatchUserItem(IServerUserItem *pIServerUserItem)
{
	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//获取用户
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];
		
		//用户判断
		if (pOnLineUserItem==pIServerUserItem)
		{
			ASSERT(pOnLineUserItem->GetUserID()==pIServerUserItem->GetUserID());

			m_OnMatchUserItem.RemoveAt(i);
			return true;
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
