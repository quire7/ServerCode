#include "StdAfx.h"
#include "ImmediateGroup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_VALUE				0xFFFF								//无效值

//静态变量
WORD CImmediateGroup::m_wChairCount;									//椅子数目

//////////////////////////////////////////////////////////////////////////
//构造函数
CImmediateGroup::CImmediateGroup(DWORD dwMatchNO, tagGameMatchOption * pGameMatchOption, IImmediateGroupSink *pIImmediateGroupSink)
{
	//指针变量
	m_pMatchOption=pGameMatchOption;
	m_pMatchSink=pIImmediateGroupSink;

	//状态变量
	m_dwMatchNO=dwMatchNO;
	m_enMatchStatus=MatchStatus_Signup;
	m_pMatchInfo = &(m_pMatchOption->MatchType.Immediate);
	m_dwCurBase = m_pMatchInfo->dwInitalBase;

	//设置变量
	ZeroMemory(&m_MatchStartTime,sizeof(m_MatchStartTime));

	//移除元素
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
	m_MatchTableArray.RemoveAll();
    
	//设置变量
	m_LoopTimer=0;
	m_wAndroidUserCount=0;

	//获取桌子
	WORD wTableCount = __max(1, m_pMatchInfo->wStartUserCount / m_wChairCount);
	m_pMatchSink->GetNullTable(this, m_MatchTableArray, wTableCount);
}

//析构函数
CImmediateGroup::~CImmediateGroup()
{
	//杀死定时器
	AllKillMatchTimer();

	//设置状态
	m_wAndroidUserCount=0;
	m_enMatchStatus=MatchStatus_Null;

	//回收桌子
	m_pMatchSink->RecoverTable(m_MatchTableArray);

	//释放桌子
	for (int i=0; i<m_MatchTableArray.GetCount(); i++)
	{
		SafeDelete(m_MatchTableArray[i]);
	}

	//移除元素
	m_MatchTableArray.RemoveAll();
	m_OnMatchUserMap.RemoveAll();
	m_FreeUserArray.RemoveAll();
}

//接口查询
VOID* CImmediateGroup::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IMatchEventSink,Guid,dwQueryVer);
	return NULL;
}

//用户报名
bool CImmediateGroup::OnUserSignUpMatch(IServerUserItem * pUserItem)
{
	//用户判断
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//查找用户
	IServerUserItem * pIServerUserExist=NULL;
	m_OnMatchUserMap.Lookup(pUserItem->GetUserID(),pIServerUserExist);

	//用户判断
	ASSERT(pIServerUserExist==NULL);
	if (pIServerUserExist!=NULL) return false;

	//人数效验
	ASSERT(m_OnMatchUserMap.GetCount()<m_pMatchInfo->wStartUserCount);
	if (m_OnMatchUserMap.GetCount() >= m_pMatchInfo->wStartUserCount) return false;

	//设置成绩
	tagUserInfo *pUserScore = pUserItem->GetUserInfo();
	pUserScore->dwWinCount = 0L;
	pUserScore->dwLostCount = 0L;
	pUserScore->dwFleeCount = 0L;
	pUserScore->dwDrawCount = 0L;
	//pUserScore->lGrade = 0L;
	pUserScore->lScore = (SCORE)m_pMatchInfo->dwInitalScore;

	if (false==UserSitDown(pUserItem))
	{
		//构造数据包
		CMD_CM_SystemMessage Message;
		ZeroMemory(&Message, sizeof(Message));
		Message.wType = SMT_CHAT;
		_sntprintf_s(Message.szString, CountArray(Message.szString), TEXT("坐下失败!"));
		Message.wLength = CountStringBuffer(Message.szString);

		//发送数据
		WORD wSendSize = sizeof(Message) - sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);
		SendData(NULL, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &Message, wSendSize);

		return false;
	}

	//发送成绩
	SendUserScore(pUserItem);

	//设置用户
	m_OnMatchUserMap[pUserItem->GetUserID()]=pUserItem;

	//设置报名状态
	pUserItem->SetSignUpStatus(MS_SIGNUP);

	//设置报名时间
	pUserItem->SetSignUpTime(GetTickCount());

	pUserItem->SetClientReady(true);

	//发送报名状态
	BYTE cbMatchStatus=MS_SIGNUP;
	SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

	//机器判断
	if (pUserItem->IsAndroidUser() == true)
	{
		if (m_wAndroidUserCount >= m_pMatchInfo->wAndroidUserCount)
		{
			return false;
		}
		m_wAndroidUserCount++;
	}

	//提示消息
	CString strMsg;
	strMsg.Format(TEXT("UserID: %d, Name: %s"), pUserItem->GetUserID(), pUserItem->GetNickName());
	CTraceService::TraceString(strMsg, TraceLevel_Warning);

	//报名人满
	if (m_OnMatchUserMap.GetCount() >= m_pMatchInfo->wStartUserCount)
	{
		////事件通知
		//ASSERT(m_pMatchSink!=NULL);
		//if (m_pMatchSink != NULL)
		//{
		//	m_pMatchSink->SendMatchInfo(NULL);
		//}

		EfficacyStartMatch();
	}

	return true;
}

//比赛开始
void CImmediateGroup::EfficacyStartMatch()
{
	//预赛安排座位
	DWORD dwUserID=0L;
	IServerUserItem *pIServerUserItem=NULL;
	POSITION pos=m_OnMatchUserMap.GetStartPosition();

	WORD wChairID=0;
	while (pos!=NULL)
	{
		//获取用户
		m_OnMatchUserMap.GetNextAssoc(pos,dwUserID,pIServerUserItem);
		ASSERT(pIServerUserItem!=NULL);

		//设置用户比赛中
		pIServerUserItem->SetSignUpStatus(MS_MATCHING);
		pIServerUserItem->SetMatchData(this);
	}

	//设置状态	
	m_enMatchStatus=MatchStatus_FirstRound;

	//记录时间
	CTime::GetCurrentTime().GetAsSystemTime(m_MatchStartTime);
	
	//开始通知
	if (m_pMatchSink != NULL)
	{
		m_pMatchSink->OnEventMatchStart(this);
	}
	
	//设置定时器
	SetMatchGameTimer(IDI_CHECK_MATCH_GROUP,1000,TIMES_INFINITY,0);

	INT_PTR nCount = m_MatchTableArray.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		tagTableFrameInfo *ptagTableFrameInfo = m_MatchTableArray[i];
		ASSERT(ptagTableFrameInfo != NULL);
		if (ptagTableFrameInfo == NULL) continue;

		ITableFrame* pTableFrame = ptagTableFrameInfo->pTableFrame;
		ASSERT(pTableFrame != NULL);
		if (pTableFrame == NULL) continue;

		//2秒后开始游戏
		PostMatchTimer(IDI_START_TABLE_GAME, 2, (WPARAM)pTableFrame);
	}
}

//用户退赛
bool CImmediateGroup::OnUserQuitMatch(IServerUserItem *pUserItem)
{
	//用户判断
	ASSERT(pUserItem!=NULL);
	if (pUserItem==NULL) return false;

	//删除用户
	BOOL bSuccess=m_OnMatchUserMap.RemoveKey(pUserItem->GetUserID());
	if (bSuccess == false)
	{
		return false;
	}

	if (pUserItem->IsAndroidUser())
	{
		ASSERT(m_wAndroidUserCount>0);
		if (m_wAndroidUserCount > 0)
		{
			m_wAndroidUserCount--;
		}
	}

	//设置状态
	pUserItem->SetSignUpStatus(MatchStatus_Null);
	BYTE cbUserStatus=MatchStatus_Null;
	m_pMatchSink->SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbUserStatus, sizeof(cbUserStatus));

	return bSuccess?true:false;
}

//定时器
bool CImmediateGroup::OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//状态校验
	if(m_enMatchStatus==MatchStatus_Null) return true;

	switch(dwTimerID)
	{
	case IDI_CHECK_MATCH_GROUP:
		{
			CaptureMatchTimer();

			return true;
		}
	case IDI_SWITCH_WAIT_TIME:
		{
			return true;
		}
	case IDI_LASTMATCHRESULT:
		{
			//最后结束
			MatchOver();

			return true;
		}
	}

	return true;
}

//玩家坐下
bool CImmediateGroup::UserSitDown(IServerUserItem *pUserItem)
{
	INT_PTR nCount = m_MatchTableArray.GetCount();
	for (INT_PTR i = 0; i <nCount; i++)
	{
		tagTableFrameInfo *ptagTableFrameInfo = m_MatchTableArray[i];
		ASSERT(ptagTableFrameInfo != NULL);
		if (ptagTableFrameInfo == NULL) continue;

		ITableFrame* pTableFrame = ptagTableFrameInfo->pTableFrame;
		ASSERT(pTableFrame != NULL);
		if (pTableFrame == NULL) continue;

		WORD wChairCount = pTableFrame->GetChairCount();
		for (WORD j = 0; j < wChairCount; j++)
		{
			IServerUserItem* pTableUser = pTableFrame->GetTableUserItem(j);
			if ( NULL==pTableUser )
			{
				return pTableFrame->PerformSitDownAction(j, pUserItem);
			}
		}
	}

	return false;
}

//玩家起立
bool CImmediateGroup::UserStandUp(IServerUserItem *pUserItem)
{
	INT_PTR nCount = m_MatchTableArray.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		tagTableFrameInfo *ptagTableFrameInfo = m_MatchTableArray[i];
		ASSERT(ptagTableFrameInfo != NULL);
		if (ptagTableFrameInfo == NULL) continue;

		ITableFrame* pTableFrame = ptagTableFrameInfo->pTableFrame;
		ASSERT(pTableFrame != NULL);
		if (pTableFrame == NULL) continue;

		WORD wChairCount = pTableFrame->GetChairCount();
		for (WORD j = 0; j < wChairCount; j++)
		{
			IServerUserItem* pTableUser = pTableFrame->GetTableUserItem(j);
			if (pUserItem == pTableUser)
			{
				return pTableFrame->PerformStandUpAction(pUserItem);
			}
		}
	}

	return false;
}

WORD CImmediateGroup::SortMapUser(tagMatchScore score[])
{
	INT_PTR nCount=m_OnMatchUserMap.GetCount();
	POSITION pos=m_OnMatchUserMap.GetStartPosition();
	nCount=0;
	while(pos!=NULL)
	{
		m_OnMatchUserMap.GetNextAssoc(pos,score[nCount].dwUserID, score[nCount].pUserItem);
		score[nCount].dwUserID=score[nCount].pUserItem->GetUserID();
		score[nCount].lScore=score[nCount].pUserItem->GetUserScore();
		nCount++;
	}

	for(INT_PTR i = 1;i < nCount;i++)
	{
		INT_PTR left = 0,right = i - 1;
		tagMatchScore  Temp = score[i];
		while(left <= right)
		{
			INT_PTR mid = (left + right)/2;
			if (score[i].lScore > score[mid].lScore)
			{
				right = mid - 1;
			}
			else
			{
				left = mid + 1;
			}
		}

		for (INT_PTR j = i - 1; j >= left; j--)
		{
			score[j + 1] = score[j];
		}

		score[left] = Temp;
	}

	return (WORD)nCount;
}

//游戏开始
bool CImmediateGroup::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	pITableFrame->SetCellScore(m_dwCurBase);
	KillMatchTimer(IDI_CHECK_TABLE_START,(WPARAM)pITableFrame);

	tagTableFrameInfo *pTableFrameInfo=GetTableInterface(pITableFrame);
	pTableFrameInfo->bRoundTableFinish=false;
	pTableFrameInfo->bSwtichTableFinish=false;
	pTableFrameInfo->bWaitForNextInnings=false;

	//已完成局数加1
	pTableFrameInfo->cbPlayCount++;

	//发送比赛信息给桌子上的用户
	SendTableUserMatchInfo(pITableFrame, INVALID_CHAIR);

	//构造数据包
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	TCHAR szMsg[256]=TEXT("");
	_sntprintf_s(szMsg, CountArray(szMsg), TEXT("定局排名赛第%d局开始（共%d局）。"), pTableFrameInfo->cbPlayCount, m_pMatchInfo->cbPlayCount);
	lstrcpyn(SystemMessage.szString,szMsg,CountArray(SystemMessage.szString));
	SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
	SystemMessage.wType=SMT_CHAT;

	//发送数据
	WORD wSendSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+SystemMessage.wLength*sizeof(TCHAR);
	pITableFrame->SendTableData(INVALID_CHAIR,SUB_GF_SYSTEM_MESSAGE, (void*)&SystemMessage,wSendSize,MDM_GF_FRAME);

	return true;
}

//游戏结束
bool CImmediateGroup::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//记录本局和哪些用户玩过
	tagTableFrameInfo * ptagTableFrameInfo=GetTableInterface(pITableFrame);
	ASSERT(ptagTableFrameInfo);
	if (ptagTableFrameInfo==NULL)return false;
	
	if(m_enMatchStatus==MatchStatus_FirstRound)
	{
		FirstRoundRuleInningsGameEnd(pITableFrame);
	}
	//CheckExitMatch();

	//比赛结束直接返回
	if(m_enMatchStatus==MatchStatus_Null)return true;

	if(m_OnMatchUserMap.GetCount()>0)
	{
		//分数通知
		tagMatchScore *pScore = new tagMatchScore[m_OnMatchUserMap.GetCount()];
		ZeroMemory(pScore,sizeof(tagMatchScore)*m_OnMatchUserMap.GetCount());
		WORD wCount=SortMapUser(pScore);
		for (WORD i=0;i<wCount;i++)
		{
			SendUserScore(pScore[i].pUserItem);
		}
		SafeDeleteArray(pScore);
	}

	return true;
}

//发送消息
bool CImmediateGroup::SendData(IServerUserItem *pIUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (pIUserItem == NULL)
	{
		//遍历容器
		DWORD dwUserID = 0;
		IServerUserItem *pLoopUserItem = NULL;
		POSITION pos = m_OnMatchUserMap.GetStartPosition();
		while (pos != NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos, dwUserID, pLoopUserItem);
			if (pLoopUserItem != NULL)
			{
				m_pMatchSink->SendData(pLoopUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
			}
		}
	}
	else
	{
		return m_pMatchSink->SendData(pIUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
	}

	return true;
}

//发送用户分数
bool CImmediateGroup::SendUserScore(IServerUserItem * pIUserItem)
{
	//发送数据
	CMD_GR_UserScore UserScore;
	ZeroMemory(&UserScore, sizeof(UserScore));

	tagUserInfo * pUserInfo = pIUserItem->GetUserInfo();

	//构造数据
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	UserScore.UserScore.dwExperience = pUserInfo->dwExperience;

	//构造积分
	UserScore.UserScore.lScore = pUserInfo->lScore;

	return SendData(NULL, MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));
}

//发送信息
bool CImmediateGroup::SendUserMessage(IServerUserItem * pIUserItem, LPCTSTR pStrMessage)
{
	//构造数据包
	CMD_CM_SystemMessage Message;
	Message.wType = SMT_CHAT | SMT_TABLE_ROLL;
	lstrcpyn(Message.szString, pStrMessage, CountArray(Message.szString));
	Message.wLength = CountStringBuffer(Message.szString);
	WORD wSendSize = sizeof(Message) - sizeof(Message.szString) + Message.wLength*sizeof(TCHAR);

	if (pIUserItem == NULL)
	{
		//遍历容器
		DWORD dwUserID = 0;
		IServerUserItem *pLoopUserItem = NULL;
		POSITION pos = m_OnMatchUserMap.GetStartPosition();
		while (pos != NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos, dwUserID, pLoopUserItem);

			if (pLoopUserItem != NULL && pLoopUserItem->IsOnline() && pLoopUserItem->IsClientReady() && !pLoopUserItem->IsAndroidUser() )
			{
				//发送数据
				m_pMatchSink->SendData(pLoopUserItem, MDM_GF_FRAME, SUB_GF_SYSTEM_MESSAGE, &Message, wSendSize);
			}
		}
	}
	else
	{
		if (pIUserItem->IsOnline() && pIUserItem->IsClientReady() && !pIUserItem->IsAndroidUser())
		{
			//发送数据
			return m_pMatchSink->SendData(pIUserItem, MDM_GF_FRAME, SUB_GF_SYSTEM_MESSAGE, &Message, wSendSize);
		}
	}

	return true;
}

//发送状态
bool CImmediateGroup::SendUserStatus(IServerUserItem * pIUserItem)
{
	//效验参数
	ASSERT(pIUserItem != NULL);
	ASSERT(pIUserItem->IsClientReady());
	if (!pIUserItem->IsClientReady()) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus, sizeof(UserStatus));

	tagUserInfo * pUserData = pIUserItem->GetUserInfo();

	//构造数据
	UserStatus.dwUserID = pUserData->dwUserID;
	UserStatus.UserStatus.wTableID = pUserData->wTableID;
	UserStatus.UserStatus.wChairID = pUserData->wChairID;
	UserStatus.UserStatus.cbUserStatus = pUserData->cbUserStatus;

	POSITION pos = m_OnMatchUserMap.GetStartPosition();
	IServerUserItem *pIFindUserItem = NULL;
	DWORD dwUserID = 0;
	while (pos != NULL)
	{
		m_OnMatchUserMap.GetNextAssoc(pos, dwUserID, pIFindUserItem);
		if (pIFindUserItem->IsClientReady())
		{
			m_pMatchSink->SendData(pIFindUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}
	}

	return true;
}

//发送比赛信息
void CImmediateGroup::SendMatchInfo(IServerUserItem *pIUserItem)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting = (DWORD)m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal = m_pMatchInfo->wStartUserCount;

	SendData(pIUserItem, MDM_GR_MATCH, SUB_GR_MATCH_NUM, &MatchNum, sizeof(MatchNum));
}

//用户淘汰
bool CImmediateGroup::OnUserOut(IServerUserItem *pUserItem, LPCTSTR szReason, WORD wLen, WORD wRank)
{
	if (pUserItem == NULL)
	{
		return false;
	}

	pUserItem->SetSignUpStatus(MatchStatus_Null);

	//清除比赛后的一些用户
	for (INT_PTR i = 0; i<m_FreeUserArray.GetCount(); i++)
	{
		if (m_FreeUserArray[i]->GetUserID() == pUserItem->GetUserID())
		{
			m_FreeUserArray.RemoveAt(i);
			break;
		}
	}

	if (wLen>0 && szReason != NULL)
	{
		CString str(szReason, wLen);
		m_pMatchSink->SendGameMessage(pUserItem, str, SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME);
	}

	////移除参赛用户
	//RemoveMatchUserItem(pUserItem);

	BYTE cbMatchStatus = MatchStatus_Null;
	SendData(pUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

	if (pUserItem != NULL && !pUserItem->IsAndroidUser())
	{
		if (m_pMatchOption->lMatchFee > 0)
		{
			////变量定义
			//DBR_GR_MatchSignup MatchSignup;
			//ZeroMemory(&MatchSignup, sizeof(MatchSignup));

			////构造结构
			//MatchSignup.dwUserID = pUserItem->GetUserID();
			//MatchSignup.lMatchFee = m_pMatchOption->lMatchFee;
			//MatchSignup.dwInoutIndex = pUserItem->GetInoutIndex();
			//MatchSignup.dwClientAddr = pUserItem->GetClientAddr();
			//MatchSignup.dwMatchID = m_pMatchOption->dwMatchID;
			//MatchSignup.dwMatchNO = m_dwMatchNO;
			//lstrcpyn(MatchSignup.szMachineID, pUserItem->GetMachineID(), CountArray(MatchSignup.szMachineID));

			////投递数据
			//m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID, DBR_GR_MATCH_UNSIGNUP, 0, &MatchSignup, sizeof(MatchSignup));

			//发送消息				
			LPCTSTR pszMatchFeeType[] = { TEXT("游戏币"), TEXT("元宝") };
			if (m_pMatchOption->cbMatchFeeType < CountArray(pszMatchFeeType))
			{
				//构造消息
				TCHAR szMessage[128] = TEXT("");
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("该房间即将关闭，请到其他房间报名参赛。退还报名费 %I64d %s。"), m_pMatchOption->lMatchFee, pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);

				//发送消息
				m_pMatchSink->SendRoomMessage(pUserItem, szMessage, SMT_EJECT | SMT_CHAT | SMT_CLOSE_ROOM);
			}
		}
	}

	return true;
}

//用户坐下
bool CImmediateGroup::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	return true; 
}

//用户起立
bool CImmediateGroup::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

 //用户同意
bool CImmediateGroup::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	return false; 
}

//获取名次
WORD CImmediateGroup::GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame)
{
	//参数效验
	ASSERT(pUserItem!=NULL);
	if(pUserItem==NULL) return INVALID_WORD;
	//定义变量
	SCORE lUserScore=pUserItem->GetUserScore();
	WORD wRank=1;
	DWORD dwUserID=0;
	IServerUserItem *pLoopUserItem=NULL;

	if(pITableFrame==NULL)
	{
		//遍历容器
		POSITION pos=m_OnMatchUserMap.GetStartPosition();
		while(pos!=NULL)
		{
			m_OnMatchUserMap.GetNextAssoc(pos,dwUserID, pLoopUserItem);
			if (pLoopUserItem != pUserItem && pLoopUserItem->GetUserScore() > lUserScore)
			{
				wRank++;
			}
		}
	}
	else
	{
		ASSERT(pUserItem->GetTableID()==pITableFrame->GetTableID());
		if (pUserItem->GetTableID() != pITableFrame->GetTableID())
		{
			return INVALID_WORD;
		}

		for(WORD i=0; i<m_wChairCount; i++)
		{
			pLoopUserItem=pITableFrame->GetTableUserItem(i);
			if (pLoopUserItem == NULL)
			{
				continue;
			}

			if (pLoopUserItem != pUserItem && pLoopUserItem->GetUserScore()>lUserScore)
			{
				wRank++;
			}
		}
	}

	return wRank;
}

//获取桌子专用接口
tagTableFrameInfo* CImmediateGroup::GetTableInterface(ITableFrame* ITable)
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame == ITable)
		{
			return m_MatchTableArray[i];
		}
	}
	return NULL;
}

//比赛结束
void CImmediateGroup::MatchOver()
{
	//删除定时器
	AllKillMatchTimer();
	KillMatchGameTimer(IDI_CHECK_MATCH_GROUP);

	//设置比赛状态
	ASSERT(m_enMatchStatus==MatchStatus_FirstRound);
	if(m_enMatchStatus==MatchStatus_FirstRound)
	{
		m_enMatchStatus=MatchStatus_Null;
		m_pMatchSink->OnEventMatchOver(this);
	}
}

//定局排名游戏结束
void CImmediateGroup::FirstRoundRuleInningsGameEnd(ITableFrame *pITableFrame)
{
	//获取对象
	tagTableFrameInfo * ptagTableFrameInfo=GetTableInterface(pITableFrame);
	ASSERT(ptagTableFrameInfo);
	if (ptagTableFrameInfo==NULL) return;

	//设置等待下局标识
	ptagTableFrameInfo->bWaitForNextInnings=true;

	TCHAR szMessage[128]=TEXT("");

	//该桌打完指定局数
	if (ptagTableFrameInfo->cbPlayCount>=m_pMatchInfo->cbPlayCount)
	{
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("恭喜您,您已玩满%d局游戏,等其他桌完成后系统将进行排名,请您耐心等待！"), m_pMatchInfo->cbPlayCount);

		//发送提示信息
		for (WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(j);
			if(pIServerUserItem!=NULL)
			{
				m_pMatchSink->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT|SMT_TABLE_ROLL);

				//插入空闲列表
				InsertFreeGroup(pIServerUserItem);
			}
			else
			{
				ASSERT(FALSE);
				CTraceService::TraceString(TEXT("玩家信息为空1!"),TraceLevel_Exception);
			}
		}

		//设置完成标识
		SetRoundTableFinish(pITableFrame);
		SendWaitTip(pITableFrame);
		ptagTableFrameInfo->cbPlayCount=0;
	}
	else
	{
		if (m_pMatchInfo->cbSwitchTableCount > 0 && (ptagTableFrameInfo->cbPlayCount%m_pMatchInfo->cbSwitchTableCount) == 0)
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("%s"), TEXT("本局已结束，请耐心等待其他桌完成，稍后为您配桌..."));
		}
		else
		{
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("%s"), TEXT("本局已结束，请稍候，系统配桌中..."));
		}

		//发送提示信息
		for (WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(j);
			if(pIServerUserItem!=NULL)
			{
				m_pMatchSink->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_TABLE_ROLL);

				//插入空闲列表
				InsertFreeGroup(pIServerUserItem);
			}
			else
			{
				ASSERT(FALSE);
				CTraceService::TraceString(TEXT("玩家信息为空2!"),TraceLevel_Exception);
			}
		}
	}

	//所有桌都完成指定局数
	if (CheckRoundTableFinish())
	{
		SetMatchGameTimer(IDI_LASTMATCHRESULT,5000,1,0);
		return ;
	}
	else if(ptagTableFrameInfo->bRoundTableFinish==false)
	{
		//自动换桌的控制
		if(m_pMatchInfo->cbSwitchTableCount>0&&(ptagTableFrameInfo->cbPlayCount%m_pMatchInfo->cbSwitchTableCount)==0)
		{
			SetSwitchTableFinish(pITableFrame);
			SendWaitTip(pITableFrame);

			//检查该桌子是否可以换桌
			if (CheckSwitchTableFinish())
			{
				SetMatchGameTimer(IDI_SWITCH_WAIT_TIME,5000,1,0);

				return;
			}
		}
		else //不换桌
		{
			//设置定时器
			PostMatchTimer(IDI_CONTINUE_GAME,5,(WPARAM)pITableFrame);

			return;
		}
	}
}

//发送提示
VOID CImmediateGroup::SendWaitTip(ITableFrame *pTableFrame, WORD wChairID/*=INVALID_CHAIR*/)
{
	//状态校验
	if(m_enMatchStatus==MatchStatus_Null || m_enMatchStatus==MatchStatus_Signup )
		return;

	tagTableFrameInfo *pITableFameEx=GetTableInterface(pTableFrame);
	ASSERT(pITableFameEx!=NULL);
	if (pITableFameEx==NULL) return;

	//变量定义
	WORD wPlayingTable=0;
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->pTableFrame->GetGameStatus() >= GAME_STATUS_PLAY)
		{
			wPlayingTable++;
		}
	}

	//变量定义
	CMD_GR_Match_Wait_Tip WaitTip;
	ZeroMemory(&WaitTip,sizeof(WaitTip));
	WaitTip.wPlayingTable=wPlayingTable;
	WaitTip.wUserCount=(WORD)m_OnMatchUserMap.GetCount();
	WaitTip.wCurGameCount=pITableFameEx->cbPlayCount;
	WaitTip.wGameCount=m_pMatchInfo->cbPlayCount;

	lstrcpyn(WaitTip.szMatchName, m_pMatchOption->szMatchName,LEN_SERVER);

	//发送比赛信息给本桌的玩家
	for(WORD i=0;i<m_wChairCount;i++)
	{
		if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
		IServerUserItem *pTableUserItem=pTableFrame->GetTableUserItem(i);
		if(pTableUserItem==NULL) continue;
		WaitTip.wCurTableRank=GetUserRank(pTableUserItem,pTableFrame);
		if(WaitTip.wCurTableRank==INVALID_WORD) continue;


		WaitTip.wRank=GetUserRank(pTableUserItem);
		WaitTip.lScore=pTableUserItem->GetUserScore();

		m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
	}

	//发送比赛信息给其他桌的玩家
	for(WORD i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if(pTableFrame->GetTableID()==m_MatchTableArray[i]->pTableFrame->GetTableID())continue;
		if(m_MatchTableArray[i]->pTableFrame->GetGameStatus()>=GAME_STATUS_PLAY) continue; 
		for(WORD j=0;j<m_wChairCount;j++)
		{
			IServerUserItem *pTableUserItem=m_MatchTableArray[i]->pTableFrame->GetTableUserItem(j);
			if(pTableUserItem==NULL) continue;
			WaitTip.wCurTableRank=GetUserRank(pTableUserItem,m_MatchTableArray[i]->pTableFrame);
			if(WaitTip.wCurTableRank==INVALID_WORD) continue;

			WaitTip.wRank=GetUserRank(pTableUserItem);
			WaitTip.lScore=pTableUserItem->GetUserScore();

			m_pMatchSink->SendData(pTableUserItem,MDM_GF_FRAME,SUB_GR_MATCH_WAIT_TIP,&WaitTip,sizeof(WaitTip));
		}
	}
}

//发送一组信息
void CImmediateGroup::SendGroupBox(LPCTSTR pStrMessage)
{
	//构造数据包
	CMD_CM_SystemMessage Message;
	Message.wType=SMT_EJECT|SMT_CHAT;
	lstrcpyn(Message.szString,pStrMessage,CountArray(Message.szString));
	Message.wLength=CountStringBuffer(Message.szString);
	//发送数据
	WORD wSendSize=sizeof(Message)-sizeof(Message.szString)+Message.wLength*sizeof(TCHAR);
	
	SendData(NULL, MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&Message,wSendSize);
}

//设置一轮完成标记
void CImmediateGroup::SetRoundTableFinish(ITableFrame *pITableFrame)
{
	tagTableFrameInfo * ptagTableFrameInfo=GetTableInterface(pITableFrame);
	ASSERT(ptagTableFrameInfo);
	if (ptagTableFrameInfo)
	{
		ptagTableFrameInfo->bRoundTableFinish = true;
	}
}

//检查一轮是否完成
bool CImmediateGroup::CheckRoundTableFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->bRoundTableFinish == false)
		{
			return false;
		}
	}
	return true;
}

//设置换桌完成标记
void CImmediateGroup::SetSwitchTableFinish(ITableFrame *pITableFrame)
{
	tagTableFrameInfo * ptagTableFrameInfo=GetTableInterface(pITableFrame);
	ASSERT(ptagTableFrameInfo);
	if (ptagTableFrameInfo)
	{
		ptagTableFrameInfo->bSwtichTableFinish = true;
	}
}

//检查换桌是否完成
bool CImmediateGroup::CheckSwitchTableFinish()
{
	for (int i=0;i<m_MatchTableArray.GetCount();i++)
	{
		if (m_MatchTableArray[i]->bSwtichTableFinish == false)
		{
			return false;
		}
	}

	return true;
}

//设置所有用户离开
void CImmediateGroup::AllUserExitTable(ITableFrame *pITableFrame)
{
	ASSERT(pITableFrame!=NULL);
	if(pITableFrame)
	{
		//该桌用户全部离开
		for (int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem* pUserItem=pITableFrame->GetTableUserItem(i);
			if (pUserItem&&pUserItem->GetTableID() != INVALID_TABLE)
			{
				pITableFrame->PerformStandUpAction(pUserItem);
			}
		}
	}
}

//本桌继续游戏
void CImmediateGroup::ContinueGame(ITableFrame *pITableFrame)
{
	if (CheckMatchUser())
	{
		return;
	}
	
	tagTableFrameInfo * ptagTableFrameInfo=GetTableInterface(pITableFrame);
	ASSERT(ptagTableFrameInfo);
	if (ptagTableFrameInfo)
	{
		AllUserExitTable(pITableFrame);
		for(int i=0;i<m_wChairCount;i++)
		{
			IServerUserItem* pIServerUserItem=m_FreeUserArray[0];
			if(pIServerUserItem==NULL)
			{				
				ASSERT(FALSE);
				CTraceService::TraceString(TEXT("空的用户存在于等待列表中。"),TraceLevel_Exception);
				return;
			}

			//先起立在坐下 让该桌的人全部起立
			if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
			{
				for(int m=0;m<m_MatchTableArray.GetCount();m++)
				{
					if(pIServerUserItem->GetTableID()==m_MatchTableArray[m]->wTableID)
					{
						m_MatchTableArray[m]->pTableFrame->PerformStandUpAction(pIServerUserItem);

						break;
					}
				}
			}

			if (pITableFrame->PerformSitDownAction(i,pIServerUserItem))
			{
				m_FreeUserArray.RemoveAt(0);
			}
			else
			{
				CTraceService::TraceString(TEXT("坐下失败？1"),TraceLevel_Exception);
			}
		}

		//30秒后做一个检查 发现该桌还没有开始游戏 说明已经卡掉了
		PostMatchTimer(IDI_CHECK_TABLE_START,30,(WPARAM)pITableFrame);
	}
}

void CImmediateGroup::SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	if (m_pMatchSink)
	{
		m_pMatchSink->SetGameTimer(dwTimerID + 10 * m_LoopTimer, dwElapse, dwRepeat, dwBindParameter, this);
	}
}

//杀死比赛专用定时器
void CImmediateGroup::KillMatchGameTimer(DWORD dwTimerID)
{
	if (m_pMatchSink)
	{
		m_pMatchSink->KillGameTimer(dwTimerID + 10 * m_LoopTimer, this);
	}
}

//检测参赛人数
bool CImmediateGroup::CheckMatchUser()
{
	//参赛用户不够 椅子数量就要终止比赛
	DWORD nCurUserCount=(DWORD)m_OnMatchUserMap.GetCount();
	if (nCurUserCount<m_wChairCount)
	{
		ASSERT(FALSE);
		CTraceService::TraceString(TEXT("检查人数..."),TraceLevel_Exception);
		return false;

		//构造数据包
		CString szString;
		szString.Format(TEXT("抱歉，由于比赛用户强行退出比赛，人数不足 %d 人，提前结算。欢迎您继续报名参加比赛！"),m_wChairCount);
		SendGroupBox(szString);

		MatchOver();

		return true;
	}
	return false;
}

//获取优先坐下用户
WORD CImmediateGroup::GetPrecedeSitUser(DWORD dwCurTime)
{
	for(int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		tagTimeInfo* pTimerInfo=m_FreeUserArray[i]->GetTimeInfo();
		if(dwCurTime-pTimerInfo->dwEndGameTimer>m_pMatchInfo->wPrecedeTimer) return i;
	}
	return INVALID_VALUE;
}

//实现该函数可保证Free组唯一
void CImmediateGroup::InsertFreeGroup(IServerUserItem *pUserItem)
{
	if (pUserItem == NULL)
	{
		return;
	}

	DWORD dwUserID = pUserItem->GetUserID();
	for(int i=0;i<m_FreeUserArray.GetCount();i++)
	{
		IServerUserItem * pIFreeUser = m_FreeUserArray[i];
		if (pIFreeUser != NULL && pIFreeUser->GetUserID() == dwUserID)
		{
			return;
		}
	}
	m_FreeUserArray.Add(pUserItem);
}

//发送比赛信息
void CImmediateGroup::SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID)
{
	//验证参数
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return;

	tagTableFrameInfo *ptagTableFrameInfo=GetTableInterface(pITableFrame);
	if (ptagTableFrameInfo==NULL) return;

	//变量定义
	WORD wChairCount=pITableFrame->GetChairCount();
	TCHAR szMsg[256]=TEXT("");

	if(m_enMatchStatus==MatchStatus_FirstRound)
	{
		//构造数据
		CMD_GR_Match_Info MatchInfo;
		ZeroMemory(&MatchInfo,sizeof(MatchInfo));
		_sntprintf_s(MatchInfo.szTitle[0], CountArray(MatchInfo.szTitle[0]), TEXT("积分方式：累计积分"));
		_sntprintf_s(MatchInfo.szTitle[1], CountArray(MatchInfo.szTitle[1]), TEXT("游戏基数：%d"), m_dwCurBase);
		_sntprintf_s(MatchInfo.szTitle[2], CountArray(MatchInfo.szTitle[2]), TEXT("局制名称：定局排名"));
		_sntprintf_s(MatchInfo.szTitle[3], CountArray(MatchInfo.szTitle[3]), TEXT("需要完成：%d局比赛"), m_pMatchInfo->cbPlayCount);
		MatchInfo.wGameCount=ptagTableFrameInfo->cbPlayCount;
		
		//wChairID为INVALID_CHAIR发给本桌所有玩家，否则只发给指定玩家
		for(WORD i=0; i<wChairCount; i++)
		{
			//获取用户
			if(wChairID!=INVALID_CHAIR && wChairID!=i) continue;
			IServerUserItem *pIServerUserItem=pITableFrame->GetTableUserItem(i);
			if(pIServerUserItem==NULL) continue;

			//发送数据
			pITableFrame->SendTableData(i, SUB_GR_MATCH_INFO, &MatchInfo, sizeof(MatchInfo), MDM_GF_FRAME);
		}

		//发送提示
		if ((wChairID!=INVALID_CHAIR) && (ptagTableFrameInfo->bWaitForNextInnings==true))
		{
			SendWaitTip(pITableFrame,wChairID);
		}
	}

	return;
}

//捕获比赛专用定时器
void CImmediateGroup::CaptureMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		tagMatchTimer* pMatchTimer = m_MatchTimerArray[i];
		ASSERT(pMatchTimer != NULL);

		if (--pMatchTimer->iElapse <= 0)
		{
			if (pMatchTimer->dwTimerID == IDI_CONTINUE_GAME)
			{
				//本桌用户继续游戏
				ContinueGame((ITableFrame*)pMatchTimer->wParam);
			}
			else if (pMatchTimer->dwTimerID == IDI_LEAVE_TABLE)
			{
				//离开本桌
				AllUserExitTable((ITableFrame*)pMatchTimer->wParam);
			}
			else if (pMatchTimer->dwTimerID == IDI_CHECK_TABLE_START)
			{
			}
			else if (pMatchTimer->dwTimerID == IDI_START_TABLE_GAME)
			{
				//桌子开始游戏
				((ITableFrame*)pMatchTimer->wParam)->StartGame();
			}

			//删除定时器
			if (KillMatchTimer(i))
			{
				i--;
			}
			else
			{
				ASSERT(false);
			}
		}
	}
}

//投递比赛专用定时器
void CImmediateGroup::PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam, LPARAM lParam)
{
	tagMatchTimer* pMatchTimer=new tagMatchTimer;
	pMatchTimer->dwTimerID=dwTimerID;
	pMatchTimer->iElapse=iElapse;
	//pMatchTimer->dwRepeat=dwRepeat;
	pMatchTimer->wParam=wParam;
	pMatchTimer->lParam=lParam;
	m_MatchTimerArray.Add(pMatchTimer);
}

//杀死比赛专用定时器
bool CImmediateGroup::KillMatchTimer(DWORD dwTimerID, WPARAM wParam)
{
	for(int i=0;i<m_MatchTimerArray.GetCount();i++)
	{
		if(m_MatchTimerArray[i]->dwTimerID==dwTimerID&&m_MatchTimerArray[i]->wParam==wParam)
		{
			tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
			m_MatchTimerArray.RemoveAt(i);
			SafeDelete(pMatchTimer);
			return true;
		}
	}

	return false;
}

//杀死比赛专用定时器
bool CImmediateGroup::KillMatchTimer(INT_PTR dwIndexID)
{
	ASSERT(dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount());
	if( dwIndexID >= 0 && dwIndexID < m_MatchTimerArray.GetCount() )
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[dwIndexID];
		m_MatchTimerArray.RemoveAt(dwIndexID);
		SafeDelete(pMatchTimer);
		return true;
	}

	return false;
}

//全部杀死比赛专用定时器
void CImmediateGroup::AllKillMatchTimer()
{
	for(int i=0;i<m_MatchTimerArray.GetCount();/*i++*/)
	{
		tagMatchTimer* pMatchTimer=m_MatchTimerArray[i];
		m_MatchTimerArray.RemoveAt(i);
		SafeDelete(pMatchTimer);
	}
}

bool CImmediateGroup::OnEventUserReturnMatch(ITableFrame *pITableFrame, IServerUserItem * pIServerUserItem)
{
	//发送比赛信息;
	SendTableUserMatchInfo(pITableFrame, pIServerUserItem->GetChairID());

	return true;
}
