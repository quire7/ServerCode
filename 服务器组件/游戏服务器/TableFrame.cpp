#include "StdAfx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//断线定义
#define IDI_OFF_LINE				(TIME_TABLE_SINK_RANGE+1)			//断线标识
#define MAX_OFF_LINE				3									//断线次数
#define TIME_OFF_LINE				60000L								//断线时间

//开始超时
#define IDI_START_OVERTIME		(TIME_TABLE_SINK_RANGE+2)			    //开始超时
#define IDI_START_OVERTIME_END	(TIME_TABLE_SINK_RANGE+22)			    //开始超时
#ifndef _DEBUG
#define TIME_OVERTIME				30000L								//超时时间
#else
#define TIME_OVERTIME               5000L                               //超时时间
#endif

//////////////////////////////////////////////////////////////////////////////////

//游戏记录
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;
CGameCardLibraryArray				CTableFrame::m_GameCardLibraryBuffer;

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrame::CTableFrame()
{
	//固有属性
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;

	//标志变量
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bTableInitFinish=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//游戏变量
	m_lCellScore=0L;
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wDrawCount=0;

	//时间变量
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//动态属性
	m_dwTableOwnerID=0L;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//断线变量
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//配置信息
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件接口
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//扩张接口
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;
	m_pIMatchTableAction=NULL;

	//数据接口
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//比赛接口
	m_pITableFrameHook=NULL;

	//用户数组
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pITableFrameHook);

	//用户积分
	for (INT_PTR i = 0; i < m_GameScoreRecordBuffer.GetCount(); i++)
	{
		//获取对象
		ASSERT(m_GameScoreRecordBuffer[i] != NULL);
		tagGameScoreRecord * pGameScoreRecord = m_GameScoreRecordBuffer[i];
		SafeDelete(pGameScoreRecord);
	}
	m_GameScoreRecordBuffer.RemoveAll();

	return;
}

//接口查询
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//开始游戏
bool CTableFrame::StartGame()
{
	//游戏状态
	ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//保存变量
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//设置状态
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//开始时间
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//开始设置
	if (bGameStarted==false)
	{
		//状态变量
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

		//设置用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//设置用户
			if (pIServerUserItem!=NULL)
			{
				//锁定游戏币
				if (m_pGameServiceOption->lServiceScore>0L)
				{
					m_lFrozenedScore[i]=m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
				}

				//设置状态
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus != US_OFFLINE) && (cbUserStatus != US_PLAYING))
				{
					pIServerUserItem->SetUserStatus(US_PLAYING, m_wTableID, i);
				}
			}
		}

		//发送状态
		if (bTableStarted != m_bTableStarted)
		{
			SendTableStatus();
		}
	}

	//比赛通知
	bool bStart=true;
	if (m_pITableFrameHook != NULL)
	{
		bStart = m_pITableFrameHook->OnEventGameStart(this, m_wChairCount);
	}

	//通知事件
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink != NULL&&bStart)
	{
		m_pITableFrameSink->OnEventGameStart();
	}

	return true;
}

//解散游戏
bool CTableFrame::DismissGame()
{
	//状态判断
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//结束游戏
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//设置状态
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//设置变量
		m_bTableStarted=false;

		//发送状态
		SendTableStatus();
	}

	return true;
}

//结束游戏
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//效验状态
	ASSERT(m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//保存变量
	bool bDrawStarted=m_bDrawStarted;

	//设置状态
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	//游戏记录
	RecordGameScore(bDrawStarted);
	
	//结束设置
	if (m_bGameStarted==false)
	{
		//变量定义
		bool bOffLineWait=false;

		//设置用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//用户处理
			if (pIServerUserItem!=NULL)
			{
				//tagTimeInfo* TimeInfo=pIServerUserItem->GetTimeInfo();
				////游戏时间
				//DWORD dwCurrentTime=(DWORD)time(NULL);
				//TimeInfo->dwEndGameTimer=dwCurrentTime;

				//解锁游戏币
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//设置状态
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//断线处理;
					bOffLineWait=true;
					if(m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
					{
						PerformStandUpAction(pIServerUserItem);

						// 私人场重置还原状态;
						if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
						{
							pIServerUserItem->SetReUserStatus(US_SIT);
						}
					}
					else
					{
						pIServerUserItem->SetClientReady(true);
						pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
					}
				}
				else
				{
					//设置状态
					pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);					
				}
			}
		}

		//删除时间
		if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);
	}

	//通知比赛
	if (m_pITableFrameHook != NULL)
	{
		m_pITableFrameHook->OnEventGameEnd(this, 0, NULL, cbGameStatus);
	}

	//重置桌子
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink != NULL)
	{
		m_pITableFrameSink->RepositionSink();
	}

	//踢出检测
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			bool isBenefitInfo = false;

			//救济金
			//if (pIServerUserItem->IsAndroidUser() == false)
			//{
			//	SCORE lAllScore = pIServerUserItem->GetUserScore() + pIServerUserItem->GetTrusteeScore() + pIServerUserItem->GetUserInsure();

			//	// 小于限制发送救济金, 领取次数限制
			//	if ((lAllScore < m_pBenefitInfo->lGoldLimit) 
			//		&& (pIServerUserItem->GetBenefitTimes() < m_pBenefitInfo->cbReceiveTimes))
			//	{
			//		// 领取时间间隔控制
			//		SYSTEMTIME SystemTime;
			//		GetLocalTime(&SystemTime);
			//		
			//		COleDateTime dTimeF(SystemTime);
			//		COleDateTime dTimeS(pIServerUserItem->GetLastBenefitTime());
			//		
			//		double nSecnonSpan = 0;
			//		if (dTimeS.m_status == COleDateTime::valid)	//有效时间值
			//		{
			//			COleDateTimeSpan dTimeSpan = dTimeF - dTimeS;
			//			nSecnonSpan = dTimeSpan.GetTotalSeconds();
			//		}
			//		else
			//		{
			//			nSecnonSpan = m_pBenefitInfo->wTimeIntervals + 1;	//无效时间值，可以发放救济金
			//		}
			//		
			//		// 可领取救济金
			//		if (nSecnonSpan > m_pBenefitInfo->wTimeIntervals)
			//		{
			//			//通知数据库领取
			//			SendBenefitInfo(pIServerUserItem, SystemTime);

			//			//发送消息
			//			SendGameMessage(pIServerUserItem, m_pBenefitInfo->szPromptMsg, SMT_EJECT);

			//			isBenefitInfo = true;
			//		}
			//	}
			//}

			//积分限制
			//if ((m_pGameServiceOption->lMinTableScore!=0L)&&(pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore))
			//SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore * m_pGameServiceOption->iExchangeRatio;
			SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore;
			if ((lMinTableScore != 0L) && (pIServerUserItem->GetUserScore()<lMinTableScore))
			{
				// 不可领取救济金时候，才显示积分不足
				if (isBenefitInfo == false)
				{
					//构造提示
					TCHAR szDescribe[128] = TEXT("");
					if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游金币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), lMinTableScore);
					}
					else
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("您的游戏金币少于 ") SCORE_STRING TEXT("，不能继续游戏！"), lMinTableScore);
					}

					//发送消息
					if (pIServerUserItem->IsAndroidUser() == true)
					{
						//SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_CLOSE_GAME | SMT_CLOSE_ROOM | SMT_EJECT);
						SendGameMessage(pIServerUserItem, szDescribe, SMT_CHAT | SMT_EJECT);
					}
					else
					{
						SendGameMessage(pIServerUserItem, szDescribe, SMT_EJECT);
					}
				}

				WORD wOldChairID = pIServerUserItem->GetChairID();

				//用户起立
				PerformStandUpAction(pIServerUserItem);

				if (pIServerUserItem->IsAndroidUser() == false)
				{
					// 金币不足的玩家直接旁观，并提示玩家显示充值界面
					PerformLookonAction(wOldChairID, pIServerUserItem);
				}
				continue;
			}

			//关闭判断
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("由于系统维护，当前游戏桌子禁止用户继续游戏！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CLOSE_GAME);

				//用户起立
				PerformStandUpAction(pIServerUserItem);
				continue;
			}

			//机器起立
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//机器处理
				if (pIServerUserItem->IsAndroidUser()==true)
				{
					//绑定信息
					CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//查找机器
					IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
					if(pIAndroidUserItem==NULL) continue;

					//离开判断
					if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
					{
						//起立处理
						PerformStandUpAction(pIServerUserItem);

						continue;
					}

					//起立判断
					if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
					{
						//起立处理
						PerformStandUpAction(pIServerUserItem);

						continue;
					}												
				}
			}

			////启动定时
			//if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			//{
			//	SetGameTimer(IDI_START_OVERTIME + i, TIME_OVERTIME, 1, i);
			//}
		}		
	}

	//结束桌子
	ConcludeTable();

	//发送状态
	SendTableStatus();

	return true;
}

//结束桌子
bool CTableFrame::ConcludeTable()
{
	//结束桌子
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//人数判断
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//模式判断
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
	}

	return true;
}

//写入积分
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	//效验参数
	ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)&&(ScoreInfo.cbType==SCORE_TYPE_NULL)) return false;

	//获取用户
	ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");

	//写入积分
	if (pIServerUserItem!=NULL)
	{
		//变量定义
		DWORD dwUserMemal=0L;
		SCORE lRevenueScore=__min(m_lFrozenedScore[wChairID],m_pGameServiceOption->lServiceScore);

		//扣服务费
		if (m_pGameServiceOption->lServiceScore>0L 
			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		{
			//扣服务费
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//解锁游戏币
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//奖牌计算
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*0.01f/1000L);
		}

		//游戏时间
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if (dwPlayGameTime != INVALID_DWORD)
		{
			dwPlayTimeCount = dwPlayGameTime;
		}

		//变量定义
		//tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();
		//
		////道具判断
		//if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		//{
		//	if (ScoreInfo.lScore>0L)
		//	{
		//		//四倍积分
		//		if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_FOURE_SCORE)!=0)
		//		{
		//			//变量定义
		//			DWORD dwValidTime=pUserProperty->PropertyInfo[1].wPropertyCount*pUserProperty->PropertyInfo[1].dwValidNum;
		//			if(pUserProperty->PropertyInfo[1].dwEffectTime+dwValidTime>dwCurrentTime)
		//			{
		//				//积分翻倍
		//				ScoreInfo.lScore *= 4;
		//				_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 四倍积分卡 ]，得分翻四倍！)"),pIServerUserItem->GetNickName());
		//			}
		//			else
		//			{
		//				pUserProperty->wPropertyUseMark&=~PT_USE_MARK_FOURE_SCORE;
		//			}
		//		} //双倍积分
		//		else if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_DOUBLE_SCORE)!=0)
		//		{
		//			//变量定义
		//			DWORD dwValidTime=pUserProperty->PropertyInfo[0].wPropertyCount*pUserProperty->PropertyInfo[0].dwValidNum;
		//			if (pUserProperty->PropertyInfo[0].dwEffectTime+dwValidTime>dwCurrentTime)
		//			{
		//				//积分翻倍
		//				ScoreInfo.lScore*=2L;
		//				_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 双倍积分卡 ]，得分翻倍！"), pIServerUserItem->GetNickName());
		//			}
		//			else
		//			{
		//				pUserProperty->wPropertyUseMark&=~PT_USE_MARK_DOUBLE_SCORE;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		//附身符
		//		if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_POSSESS)!=0)
		//		{
		//			//变量定义
		//			DWORD dwValidTime=pUserProperty->PropertyInfo[3].wPropertyCount*pUserProperty->PropertyInfo[3].dwValidNum;
		//			if(pUserProperty->PropertyInfo[3].dwEffectTime+dwValidTime>dwCurrentTime)
		//			{
		//				//积分翻倍
		//				ScoreInfo.lScore = 0;
		//				_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] 使用了[ 护身符卡 ]，积分不变！"),pIServerUserItem->GetNickName());
		//			}
		//			else
		//			{
		//				pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_POSSESS;
		//			}
		//		}
		//	}
		//}

		//写入积分
		DWORD dwWinExperience=(ScoreInfo.cbType==SCORE_TYPE_WIN)?m_pGameServiceOption->dwWinExperience:m_pGameServiceOption->dwFailExperience;
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore,ScoreInfo.lGrade,ScoreInfo.lRevenue,dwUserMemal,ScoreInfo.cbType,dwPlayTimeCount,dwWinExperience);

		//游戏记录
		if (pIServerUserItem->IsAndroidUser()==false && CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//变量定义
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//查询库存
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//获取对象
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//删除对象
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//创建对象
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//创建对象
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("游戏记录对象创建失败");
				}
				catch (...)
				{
					ASSERT(FALSE);
				}
			}

			//记录数据
			if (pGameScoreRecord!=NULL)
			{
				//用户信息
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//用户信息
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//成绩信息
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;

				//附加信息
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//机器人免税
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//插入数据
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//游戏记录
		if(dwGameMemal != INVALID_DWORD || dwPlayGameTime!=INVALID_DWORD)
		{
			DWORD dwStartGameTime=INVALID_DWORD;
			if(dwPlayGameTime!=INVALID_DWORD)
			{
				dwStartGameTime=(dwCurrentTime-dwPlayGameTime);
			}
			RecordGameScore(true, dwStartGameTime);
		}
	}
	else
	{
		//离开用户
		CTraceService::TraceString(TEXT("系统暂时未支持离开用户的补分操作处理！"),TraceLevel_Exception);

		return false;
	}

	//广播消息
	if (szMessage[0]!=0)
	{
		//变量定义
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//游戏玩家
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//发送消息
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//旁观用户
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//发送消息
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//写入积分
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData)
{
	//效验参数
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//写入分数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (ScoreInfoArray[i].cbType!=SCORE_TYPE_NULL)
		{
			WriteUserScore(i,ScoreInfoArray[i]);
		}
	}

	if (m_pITableFrameHook)
	{
		m_pITableFrameHook->WriteTableScore(this, ScoreInfoArray, wScoreCount, kData);
	}

	return true;
}

//计算税收
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//效验参数
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;

	//计算税收
	if ((m_pGameServiceOption->wRevenueRatio>0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//获取用户
		ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//计算税收
		SCORE lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/REVENUE_DENOMINATOR;

		return lRevenue;
	}

	return 0L;
}

//消费限额
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//用户效验
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//查询额度
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//效验额度
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//寻找用户
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//桌子用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem != NULL) && (pIServerUserItem->GetUserID() == dwUserID))
		{
			return pIServerUserItem;
		}
	}

	//旁观用户
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem != NULL) && (pIServerUserItem->GetUserID() == dwUserID))
		{
			return pIServerUserItem;
		}
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//游戏用户
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//效验参数
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//获取用户
	return m_TableUserItemArray[wChairID];
}

//旁观用户
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//设置时间
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine != NULL)
	{
		m_pITimerEngine->SetTimer(dwEngineTimerID + dwTimerID, dwElapse, dwRepeat, dwBindParameter);
	}

	return true;
}

//删除时间
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine != NULL)
	{
		m_pITimerEngine->KillTimer(dwEngineTimerID + dwTimerID);
	}

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送数据
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//效验状态
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//发送数据
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//用户群发
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//发送数据
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//发送数据
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//发送数据
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

	} while (true);

	return true;
}

//发送消息
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	WORD wEnumIndex=0;

	//发送消息
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//发送消息
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//枚举用户
	do
	{
		//获取用户
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送消息
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//房间消息
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送消息
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//游戏消息
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送消息
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//发送场景
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//发送场景
	ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//设置接口
bool CTableFrame::SetTableFrameHook(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	if(pIUnknownEx==NULL) return false;

	//类型判断
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH | GAME_GENRE_PRIVATE)) == 0) return false;

	//查询接口
	m_pITableFrameHook=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameHook);
	m_pIMatchTableAction=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableUserAction);

	return true;
}

//断线事件
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户变量
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	IServerUserItem * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//游戏用户
	if (cbUserStatus!=US_LOOKON)
	{
		//效验用户
		ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//私人类型;
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
		{
			pIServerUserItem->SetReUserStatus(pIServerUserItem->GetUserStatus());
			pIServerUserItem->SetUserStatus(US_OFFLINE, m_wTableID, wChairID);
			//掉线通知;
			if (m_pIMatchTableAction != NULL) m_pIMatchTableAction->OnActionUserOffLine(wChairID, pIServerUserItem);

			return true;
		}

		//断线处理
		//if ((cbUserStatus==US_PLAYING)&&(m_wOffLineCount[wChairID]<MAX_OFF_LINE))
		if (cbUserStatus == US_PLAYING)
		{
			//用户设置
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//比赛类型
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				if(pIServerUserItem->IsTrusteeUser()==false)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(true);

					//掉线通知
					if (m_pITableUserAction != NULL)
					{
						m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
					}
				}

				return true;
			}

			//掉线通知
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			}
			
			////断线处理
			//if (m_dwOffLineTime[wChairID]==0L)
			//{
			//	//设置变量
			//	m_wOffLineCount[wChairID]++;
			//	m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

			//	//时间设置
			//	WORD wOffLineCount=GetOffLineUserCount();
			//	if (wOffLineCount == 1)
			//	{
			//		SetGameTimer(IDI_OFF_LINE, TIME_OFF_LINE, 1, wChairID);
			//	}
			//}

			return true;
		}
	}

	//用户起立
	PerformStandUpAction(pIServerUserItem,true);
	
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		if (pIServerUserItem->IsTrusteeUser() == true)
		{
			return true;
		}
	}

	//删除用户
	ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
	pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//积分事件
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//通知游戏
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//时间事件
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//回调事件
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//事件处理
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//断线事件
		{
			//效验状态
			ASSERT(m_bGameStarted==true);
			if (m_bGameStarted==false) return false;

			//变量定义
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//寻找用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//位置判断
			ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair==INVALID_CHAIR) return false;

			//用户判断
			ASSERT(dwBindParameter<m_wChairCount);
			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//时间计算
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//设置时间
				dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE-2000L);
				SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//获取用户
			ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//结束游戏
			if (pIServerUserItem!=NULL)
			{
				//设置变量
				m_dwOffLineTime[wOffLineChair]=0L;

				//用户起立
				PerformStandUpAction(pIServerUserItem);
			}

			//继续时间
			if (m_bGameStarted==true)
			{
				//变量定义
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//寻找用户
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//设置时间
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//时间计算
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//设置时间
					dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE-2000L);
					SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE-dwLapseTime,1,wOffLineChair);
				}
			}

			return true;
		}
	}

	//未开始超时
	if(dwTimerID >= IDI_START_OVERTIME && dwTimerID <=IDI_START_OVERTIME_END)
	{
		//变量定义
		WORD wChair=(WORD)dwBindParameter;

		//
		if(wChair != (WORD)(dwTimerID-IDI_START_OVERTIME)) return false;

		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChair);

		//超时处理
		if(pIServerUserItem && pIServerUserItem->GetUserStatus()==US_SIT)
		{
			//用户起立
			PerformStandUpAction(pIServerUserItem);
		}

		return false;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//游戏事件
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架事件
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//游戏处理
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//默认处理
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//游戏配置
		{
			//效验参数
 			ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//变量定义
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//断线清理
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//设置变量
				m_dwOffLineTime[wChairID]=0L;

				//删除时间
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0) KillGameTimer(IDI_OFF_LINE);
			}

			//设置状态
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus != US_LOOKON)
			{
				m_bAllowLookon[wChairID] = pGameOption->cbAllowLookon ? true : false;
			}

			//发送状态
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			////发送消息
			//TCHAR szMessage[128]=TEXT("");
			//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("欢迎您进入“%s”游戏，祝您游戏愉快！"),m_pGameServiceAttrib->szGameName);
			//m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

			//发送场景
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);



			//取消托管
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				if(pIServerUserItem->IsTrusteeUser()==true)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(false);
				}

				////回归通知;
				//if(m_pITableFrameHook) m_pITableFrameHook->OnUserBackMatch(this,pIServerUserItem);
			}

			//回归通知;
			if (m_pITableFrameHook != NULL) m_pITableFrameHook->OnEventUserReturnMatch(pIServerUserItem);

			//开始判断;
			if ((cbUserStatus == US_READY) && (EfficacyStartGame(wChairID) == true))
			{
				StartGame();
			}

			return true;
		}
	case SUB_GF_USER_READY:		//用户准备
		{
			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//效验状态
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//效验状态
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//防作弊分组判断
			if((CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//发送消息
				LPCTSTR pszMessage=TEXT("系统重新分配桌子，请稍后！");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//发送消息
				m_pIMainServiceFrame->InsertDistribute(pIServerUserItem);

				////用户起立
				//PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//事件通知
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
			{
				return true;
			}

			//事件通知
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);

				// 金币带入设置

			}

			//删除定时
			if(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) KillGameTimer(IDI_START_OVERTIME+wChairID);

			//开始判断
			if (EfficacyStartGame(wChairID)==false)
			{
				//设置状态
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//用户聊天
		{
			//变量定义
			CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

			//效验参数
			ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
			ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
			//ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));
			ASSERT(wDataSize == (sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString) + CountStringBuffer(pUserChat->szChatString)));

			//效验参数
			if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
			//if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;
			if (wDataSize != (sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString) + CountStringBuffer(pUserChat->szChatString)))
			{
				return false;
			}

			//目标用户
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//构造消息
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//字符过滤
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//构造数据
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//发送数据
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			SendTableData(INVALID_CHAIR, SUB_GF_USER_CHAT, &UserChat, wSendSize, MDM_GF_FRAME);
			SendLookonData(INVALID_CHAIR, SUB_GF_USER_CHAT, &UserChat, wSendSize, MDM_GF_FRAME);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//用户表情
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//变量定义
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//目标用户
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//状态判断
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，当前游戏房间禁止游戏聊天！"),SMT_CHAT);
				return true;
			}

			//权限判断
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			try
			{
				if (pUserExpression->wPropertyID > 0)
				{
					//查找道具信息
					tagPropertyInfo * pPropertyInfo = CGamePropertyManager::GetInstance()->SearchPropertyItem(pUserExpression->wPropertyID);

					//有效效验
					if (pPropertyInfo == NULL)
					{
						SendGameMessage(pIServerUserItem, TEXT("抱歉，游戏中不存在魔法表情包，无法使用！"), SMT_EJECT | SMT_CHAT);
						return true;
					}

					SYSTEMTIME SystemTime;
					GetLocalTime(&SystemTime);

					CMD_GR_C_Property Property;
					ZeroMemory(&Property, sizeof(Property));
					Property.wPropertyIndex = pUserExpression->wPropertyID;
					Property.wItemCount = 0;

					DWORD dwPropDBID = 0;
					//是否有道具
					EN_PROPERTY_ERROR error = pIServerUserItem->UsedProperty(&Property, SystemTime, dwPropDBID);

					//道具使用失败
					if (error != EN_PROPERTY_ERROR::EN_PROP_SUCC)
					{
						CString str;
						switch (error)
						{
						case EN_PROP_NULL:
						case EN_PROP_NUMBER:
							str.Format(_T("抱歉，您还没有魔法表情包，请前往商城购买！"));
							break;
						case EN_PROP_OVER:
							str.Format(_T("抱歉，您所拥有的魔法表情包已过期，请前往商城购买！"));
							break;
						default:
							break;
						}

						SendGameMessage(pIServerUserItem, str, SMT_EJECT | SMT_CHAT);
						return true;
					}
				}

				//构造消息
				CMD_GR_S_UserExpression UserExpression;
				ZeroMemory(&UserExpression, sizeof(UserExpression));

				//构造数据
				UserExpression.wItemIndex = pUserExpression->wItemIndex;
				UserExpression.dwSendUserID = pIServerUserItem->GetUserID();
				UserExpression.dwTargetUserID = pUserExpression->dwTargetUserID;

				SendTableData(INVALID_CHAIR, SUB_GF_USER_EXPRESSION, &UserExpression, sizeof(UserExpression), MDM_GF_FRAME);
				SendLookonData(INVALID_CHAIR, SUB_GF_USER_EXPRESSION, &UserExpression, sizeof(UserExpression), MDM_GF_FRAME);
			}
			catch (...)
			{
				//错误信息
				CTraceService::TraceString(TEXT("用户聊天表情发送异常"), TraceLevel_Exception);
			}

			return true;

		}
	case SUB_GF_MAGIC_EXPRESSION:	//魔法表情
		{
			//效验参数
			ASSERT(wDataSize == sizeof(CMD_GF_C_MagicExpression));
			if (wDataSize != sizeof(CMD_GF_C_MagicExpression)) return false;

			//变量定义
			CMD_GF_C_MagicExpression * pUserExpression = (CMD_GF_C_MagicExpression *)pData;

			//目标用户
			if ((pUserExpression->wTargetChairID != INVALID_CHAIR) && (GetTableUserItem(pUserExpression->wTargetChairID) == NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			////权限判断
			//if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight()) == false)
			//{
			//	SendGameMessage(pIServerUserItem, TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！"), SMT_EJECT | SMT_CHAT);
			//	return true;
			//}

			//必须做在桌子上
			if ((pIServerUserItem->GetChairID() == INVALID_CHAIR)||(pIServerUserItem->GetUserStatus() == US_LOOKON))
			{
				SendGameMessage(pIServerUserItem, TEXT("抱歉，您不在游戏中，无法发送魔法表情！"), SMT_EJECT | SMT_CHAT);
				return true;
			}

			try
			{
				//VIP权限判断

				//积分判断
				//SCORE lTrusteeScore = pIServerUserItem->GetTrusteeScore() * m_pGameServiceOption->iExchangeRatio;
				SCORE lTrusteeScore = pIServerUserItem->GetTrusteeScore();
				if (lTrusteeScore < m_pGameServiceOption->lMagicExpScore)
				{
					SendGameMessage(pIServerUserItem, TEXT("抱歉，您的积分不足，无法发送魔法表情！"), SMT_EJECT | SMT_CHAT);
					return true;
				}

				//积分消费
				if (pIServerUserItem->WriteOtherScore(m_pGameServiceOption->lMagicExpScore, TASK_TYPE_MAGIC_EXP) == false)
				{
					SendGameMessage(pIServerUserItem, TEXT("抱歉，您的积分不足，无法发送魔法表情！"), SMT_EJECT | SMT_CHAT);
					return true;
				}

				//构造消息
				CMD_GF_S_MagicExpression UserExpression;
				ZeroMemory(&UserExpression, sizeof(UserExpression));

				//构造数据
				UserExpression.wItemIndex = pUserExpression->wItemIndex;
				UserExpression.wSendChairID = pIServerUserItem->GetChairID();
				UserExpression.wTargetChairID = pUserExpression->wTargetChairID;
				SendTableData(INVALID_CHAIR, SUB_GF_MAGIC_EXPRESSION, &UserExpression, sizeof(UserExpression), MDM_GF_FRAME);

			}
			catch (...)
			{
				//错误信息
				CTraceService::TraceString(TEXT("魔法表情发送异常"), TraceLevel_Exception);
			}

			return true;
		}
	case SUB_GF_PROPERTY_SEND:		//道具使用
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_GR_C_Property));
		if (wDataSize != sizeof(CMD_GR_C_Property)) return false;

		//变量定义
		CMD_GR_C_Property * pProperty = (CMD_GR_C_Property *)pData;

		//数据效验
		ASSERT(pProperty->wItemCount>0);
		if (pProperty->wItemCount == 0) return false;
		
		IServerUserItem * pTargetUesrItem = GetTableUserItem(pProperty->wTargetChairID);

		//目标用户
		if ((pProperty->wTargetChairID != INVALID_CHAIR) && (pTargetUesrItem == NULL))
		{
			ASSERT(FALSE);
			return true;
		}

		//自己必须做在桌子上
		if ((pIServerUserItem->GetChairID() == INVALID_CHAIR) || (pIServerUserItem->GetUserStatus() == US_LOOKON))
		{
			SendGameMessage(pIServerUserItem, TEXT("抱歉，您不在游戏中，无法赠送魅力道具！"), SMT_EJECT | SMT_CHAT);
			return true;
		}

		try
		{
			//查找道具信息
			tagPropertyInfo * pPropertyInfo = CGamePropertyManager::GetInstance()->SearchPropertyItem(pProperty->wPropertyIndex);

			//有效效验
			if (pPropertyInfo == NULL)
			{
				SendGameMessage(pIServerUserItem, TEXT("抱歉，游戏中不存在该魅力道具，无法赠送！"), SMT_EJECT | SMT_CHAT);
				return true;
			}

			SYSTEMTIME SystemTime;
			GetLocalTime(&SystemTime);

			//是否有道具
			DWORD dwPropDBID = 0;
			EN_PROPERTY_ERROR error = pIServerUserItem->UsedProperty(pProperty, SystemTime, dwPropDBID);

			//道具使用失败
			if (error != EN_PROPERTY_ERROR::EN_PROP_SUCC)
			{
				CString str;
				switch (error)
				{
				case EN_PROP_NULL:
					str.Format(_T("抱歉，您无该魅力道具，请前往商城购买！"));
					break;
				case EN_PROP_NUMBER:
					str.Format(_T("抱歉，您所拥有该魅力道具数量不足，请前往商城购买！"));
					break;
				case EN_PROP_OVER:
					str.Format(_T("抱歉，您所拥有该魅力道具已过期，请前往商城购买！"));
					break;
				default:
					break;
				}

				SendGameMessage(pIServerUserItem, str, SMT_EJECT | SMT_CHAT);
				return true;
			}

			//修改接受魅力道具玩家魅力
			pTargetUesrItem->ModifyUserProperty(0, pPropertyInfo->lRecvLoveLiness);

			//通知数据库更新
			DBR_GR_PropertyRequest PropertyRequest;
			ZeroMemory(&PropertyRequest, sizeof(PropertyRequest));

			PropertyRequest.dwDBID = dwPropDBID;
			PropertyRequest.wItemCount = pProperty->wItemCount;
			PropertyRequest.wPropertyIndex = pProperty->wPropertyIndex;
			PropertyRequest.dwSourceUserID = pIServerUserItem->GetUserID();
			PropertyRequest.dwTargetUserID = pTargetUesrItem->GetUserID();
			PropertyRequest.lRecvLoveLiness = pPropertyInfo->lRecvLoveLiness;

			PropertyRequest.wTableID = m_wTableID;
			PropertyRequest.dwInoutIndex = pIServerUserItem->GetInoutIndex();
			PropertyRequest.dwClientAddr = pIServerUserItem->GetClientAddr();
			PropertyRequest.UsedTime = SystemTime;
			lstrcpyn(PropertyRequest.szMachineID, pIServerUserItem->GetMachineID(), CountArray(PropertyRequest.szMachineID));

			m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_PROPERTY_REQUEST, 0, &PropertyRequest, sizeof(PropertyRequest));

			//构造消息
			CMD_GF_S_Property sProperty;
			ZeroMemory(&sProperty, sizeof(sProperty));

			//构造数据
			sProperty.wPropertyIndex = pProperty->wPropertyIndex;
			sProperty.wItemCount = pProperty->wItemCount;
			sProperty.lRecvLoveLiness = pPropertyInfo->lRecvLoveLiness;
			sProperty.wSendChairID = pIServerUserItem->GetChairID();
			sProperty.wTargetChairID = pProperty->wTargetChairID;
			SendTableData(INVALID_CHAIR, SUB_GF_PROPERTY_SUCC, &sProperty, sizeof(CMD_GF_S_Property), MDM_GF_FRAME);
		}
		catch (...)
		{
			//错误信息
			CTraceService::TraceString(TEXT("魅力道具使用异常"), TraceLevel_Exception);
		}

		return true;
	}
	case SUB_GF_LOOKON_CONFIG:		//旁观配置
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//变量定义
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//目标用户
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//旁观处理
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//构造消息
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//发送消息
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//设置判断
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//设置变量
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//构造消息
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//发送消息
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//发送消息
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_GF_TAKE_GOLD:		// 带入金币
		{
			//效验参数
			ASSERT(wDataSize == sizeof(CMD_GF_TakeGold));
			if (wDataSize < sizeof(CMD_GF_TakeGold)) return false;

			//变量定义
			CMD_GF_TakeGold * pTakeGold = (CMD_GF_TakeGold *)pData;

			if (pTakeGold->lScore <= 0) return false;
			

			//获取属性
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//效验状态
			ASSERT(GetTableUserItem(wChairID) == pIServerUserItem);
			if (GetTableUserItem(wChairID) != pIServerUserItem) return false;

			//效验状态
			switch (cbUserStatus)
			{
			case US_LOOKON:		// 不允许带入
			{
				return true;
			}
			case US_PLAYING:	// 记录带入金额，游戏结束后带入
			{
				//带入积分
				SCORE lScore = pIServerUserItem->TakeGold(pTakeGold->lScore, false);
				return true;
			}
			case US_READY:		// 直接带入
			case US_SIT:
			{
 			    //带入积分
				SCORE lScore = pIServerUserItem->TakeGold(pTakeGold->lScore);
				
				//使用用户坐下操作来更新服务器中桌面玩家带入金币信息
				m_pITableUserAction->OnActionUserSitDown(wChairID, pIServerUserItem, false);

				//变量定义
				CMD_GR_UserScore UserScore;
				ZeroMemory(&UserScore, sizeof(UserScore));
				UserScore.dwUserID = pIServerUserItem->GetUserID();
				UserScore.UserScore.lScore = lScore;
				UserScore.UserScore.lHoldScore = lScore + pIServerUserItem->GetTrusteeScore();
				UserScore.UserScore.lHoldScore += pIServerUserItem->GetFrozenedScore();
				SendTableData(INVALID_CHAIR, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore), MDM_GR_USER);

				return true;
			}
			default:
				break;
			}

			return true;
		}
	}

	return false;
}

//游戏局数
WORD CTableFrame::GetDrawCount()
{
	return m_wDrawCount;
}

//获取空位
WORD CTableFrame::GetNullChairID()
{
	//椅子搜索
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//随机空位
WORD CTableFrame::GetRandNullChairID()
{
	//椅子搜索
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
		}
	}

	return INVALID_CHAIR;
}

//用户数目
WORD CTableFrame::GetSitUserCount()
{
	//变量定义
	WORD wUserCount=0;

	//数目统计
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//旁观数目
WORD CTableFrame::GetLookonUserCount()
{
	//获取数目
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//断线数目
WORD CTableFrame::GetOffLineUserCount()
{
	//变量定义
	WORD wOffLineCount=0;

	//断线人数
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//桌子状况
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//设置变量
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//准备判断
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//最少数目
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//所有准备
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//配对开始
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//时间控制
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//默认模式
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//配置桌子
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//设置变量
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//配置参数
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//组件接口
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;
	m_pBenefitInfo = TableFrameParameter.pBenefitInfo;

	//创建桌子
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}
	
	//设置变量
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//设置桌子
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//设置标识
	m_bTableInitFinish=true;

	//扩展接口
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);

	return true;
}

//起立动作
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative/*=false*/)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);

	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	// 旁观玩家优先处理
	if (cbUserStatus == US_LOOKON)
	{
		//起立处理
		for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
		{
			if (pIServerUserItem == m_LookonUserItemArray[i])
			{
				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

				//用户状态
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

				return true;
			}
		}

		return true;
	}

	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//游戏用户
	if ((m_bGameStarted==true)&&((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)))
	{
		//比赛类型
		if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
		{
			//设置托管
			pIServerUserItem->SetTrusteeUser(true);

			//掉线通知
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			}

			return true;
		}

		//结束游戏
		BYTE cbConcludeReason=(cbUserStatus==US_OFFLINE)?GER_NETWORK_ERROR:GER_USER_LEAVE;
		m_pITableFrameSink->OnEventGameConclude(wChairID,pIServerUserItem,cbConcludeReason);

		//离开判断
		if (m_TableUserItemArray[wChairID]!=pIServerUserItem) return true;
	}

	if ((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) && (bInitiative==true))
	{
		if (pIServerUserItem->IsTrusteeUser()==false)
		{
			pIServerUserItem->SetTrusteeUser(true);

			//掉线通知
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			}
		}	
		return true;
	}

	//设置变量 桌子上游戏玩家站起
	if (pIServerUserItem==pITableUserItem)
	{
		//删除定时
		if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
		{
			KillGameTimer(IDI_START_OVERTIME + wChairID);
		}

		//设置变量
		m_TableUserItemArray[wChairID]=NULL;

		//解锁游戏币
		if (m_lFrozenedScore[wChairID]!=0L)
		{
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//事件通知
		if (m_pITableUserAction!=NULL)
		{
			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//事件通知
		if (m_pIMatchTableAction != NULL)
		{
			m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, false);
		}

#ifdef _DEBUG
		if (pIServerUserItem->IsAndroidUser())
		{
			CString strMsg;
			strMsg.Format("机器人ID:%d,昵称:%s,站起桌子:%d,椅子:%d", pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
			//提示消息
			CTraceService::TraceString(strMsg, TraceLevel_Normal);
		}
#endif

		//用户状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//变量定义
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//设置变量
		m_wUserCount=wTableUserCount;

		//桌子信息
		if (wTableUserCount==0)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		//踢走旁观
		//if (wTableUserCount==0)
		//{
		//	for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		//	{
		//		SendGameMessage(m_LookonUserItemArray[i],TEXT("此游戏桌的所有玩家已经离开了！"),SMT_CLOSE_GAME|SMT_EJECT);
		//	}
		//}

		//结束桌子
		ConcludeTable();

		//开始判断
		if (EfficacyStartGame(INVALID_CHAIR)==true)
		{
			StartGame();
		}

		//发送状态
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted()))
		{
			SendTableStatus();
		}

		//防作弊分组判断
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
		{
			//防作弊分组判断
			if(IsTableStarted()==false && CheckDistribute())
			{
				for(int i=0; i<m_wChairCount; i++)
				{
					if(m_TableUserItemArray[i] != NULL && m_TableUserItemArray[i]->GetUserStatus()==US_READY)
					{
						//发送消息
						LPCTSTR pszMessage=TEXT("由玩家离开，系统重新分配桌子，请稍后！");
						SendGameMessage(m_TableUserItemArray[i],pszMessage,SMT_CHAT);

						//发送消息
						m_pIMainServiceFrame->InsertDistribute(m_TableUserItemArray[i]);

						////用户起立
						//PerformStandUpAction(m_TableUserItemArray[i]);
					}
				}
			}
		}

		return true;
	}
	else
	{
		////起立处理
		//for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		//{
		//	if (pIServerUserItem==m_LookonUserItemArray[i])
		//	{
		//		//删除子项
		//		m_LookonUserItemArray.RemoveAt(i);

		//		//事件通知
		//		if (m_pITableUserAction!=NULL)
		//		{
		//			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
		//		}

		//		//事件通知
		//		if (m_pIMatchTableAction != NULL)
		//		{
		//			m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, true);
		//		}

		//		//用户状态
		//		pIServerUserItem->SetClientReady(false);
		//		pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//		return true;
		//	}
		//}

		//错误断言
		ASSERT(FALSE);
	}

	return true;
}

bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//效验参数
	//ASSERT((pIServerUserItem != NULL) && (wChairID < m_wChairCount));
	ASSERT(pIServerUserItem != NULL);
	ASSERT((pIServerUserItem->GetTableID() == INVALID_TABLE) && (pIServerUserItem->GetChairID() == INVALID_CHAIR));

	//变量定义
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule = pIServerUserItem->GetUserRule();

	//模拟处理
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser() == false)
	{
		//定义变量
		CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;

		//查找机器
		for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem = m_TableUserItemArray[i];
			if (pIUserItem == NULL) continue;
			if (pIUserItem->IsAndroidUser() == false)break;

			//获取参数
			tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(), pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter = pIAndroidUserItem->GetAndroidParameter();

			//模拟判断
			if ((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE) != 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY) == 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE) == 0)
			{
				//SendRequestFailure(pIServerUserItem, TEXT("抱歉，当前游戏桌子禁止用户旁观！"), REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_TABLE);
				return false;
			}

			break;
		}
	}


	//旁观判断
	if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule) == true
		&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) == false))
	{
		//if ((pITableUserItem != NULL) && (pITableUserItem->IsAndroidUser() == true))
		{
			//SendRequestFailure(pIServerUserItem, TEXT("抱歉，当前游戏房间禁止用户旁观！"), REQUEST_FAILURE_NORMAL);
			SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_ROOM);
			return false;
		}
	}

	//状态判断
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule) == true) && (pIServerUserItem->GetMasterOrder() == 0))
	{
		//SendRequestFailure(pIServerUserItem, TEXT("抱歉，当前游戏房间禁止用户旁观！"), REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_ROOM);
		return false;
	}

	//密码效验
	if ((IsTableLocked() == true) && (pIServerUserItem->GetMasterOrder() == 0L) && (lstrcmp(pUserRule->szPassword, m_szEnterPassword) != 0))
	{
		//SendRequestFailure(pIServerUserItem, TEXT("游戏桌进入密码不正确，不能旁观游戏！"), REQUEST_FAILURE_PASSWORD);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_PASSW);
		return false;
	}

	//设置用户
	m_LookonUserItemArray.Add(pIServerUserItem);

	//用户状态
	//pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetClientReady(true);
	pIServerUserItem->SetUserStatus(US_LOOKON, m_wTableID, wChairID);

	//事件通知
	if (m_pITableUserAction != NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID, pIServerUserItem, true);
	}

	//事件通知
	if (m_pIMatchTableAction != NULL)
	{
		m_pIMatchTableAction->OnActionUserSitDown(wChairID, pIServerUserItem, true);
	}

	return true;
}

//坐下动作
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//变量定义
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//积分变量
	//SCORE lUserScore=pIServerUserItem->GetUserScore();
	//SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	//SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	//状态判断
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		//SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户进入！"),REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_ENTER);
		return false;
	}

	//模拟处理
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//定义变量
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//查找机器
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//获取参数
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//模拟判断
			if((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
			{
				//SendRequestFailure(pIServerUserItem,TEXT("抱歉，当前游戏桌子禁止用户进入！"),REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_ENTER);
				return false;
			}

			break;
		}
	}

	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//游戏状态
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		//SendRequestFailure(pIServerUserItem,TEXT("游戏已经开始了，现在不能进入游戏桌！"),REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_ENTER_START);
		return false;
	}

	//椅子判断
	if (pITableUserItem!=NULL)
	{
		//防作弊
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return false;

		//构造信息
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！"),pITableUserItem->GetNickName());

		//发送信息
		//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

		return false;
	}

	//密码效验
	if(m_wChairCount <= MAX_CHAIR_GENERAL)
	{
		//防作弊
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule) == false)
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))
			{
				//SendRequestFailure(pIServerUserItem,TEXT("游戏桌进入密码不正确，不能加入游戏！"),REQUEST_FAILURE_PASSWORD);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_PASSW);
				return false;
			}
		}
	}

	//规则效验
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyEnterTableScoreRule(wChairID,pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyIPAddress(pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyScoreRule(pIServerUserItem)==false) return false;

	//扩展效验
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//变量定义
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//坐下效验
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//发送信息
			//SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);
			SendRequestFailure(pIServerUserItem, RequestResult.cbFailureCode, EN_MOBILE_CODE::EN_MC_NULL, RequestResult.szFailureReason);

			return false;
		}
	}

#ifdef _DEBUG
	if (pIServerUserItem->IsAndroidUser())
	{
		CString strMsg;
		strMsg.Format("机器人ID:%d,昵称:%s,坐下桌子:%d,椅子:%d", pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
		//提示消息
		CTraceService::TraceString(strMsg, TraceLevel_Normal);
	}
#endif

	//设置变量
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	m_wDrawCount=0;

	//用户状态
	if ((IsGameStarted()==false)||(m_cbStartMode!=START_MODE_TIME_CONTROL))
	{
		if (CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)==false && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
		}
		else
		{
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				pIServerUserItem->SetClientReady(pIServerUserItem->IsTrusteeUser());
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
		}
	}
	else
	{
		//设置变量
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//锁定游戏币
		if (m_pGameServiceOption->lServiceScore>0L)
		{
			m_lFrozenedScore[wChairID]=m_pGameServiceOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
		}

		//设置状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	//设置变量
	m_wUserCount=GetSitUserCount();

	//桌子信息
	if (GetSitUserCount()==1)
	{
		//状态变量
		bool bTableLocked=IsTableLocked();

		//设置变量
		m_dwTableOwnerID=pIServerUserItem->GetUserID();
		lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));

		//发送状态
		if (bTableLocked != IsTableLocked())
		{
			SendTableStatus();
		}
	}

	//启动定时
	//if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	//{
	//	SetGameTimer(IDI_START_OVERTIME + wChairID, TIME_OVERTIME, 1, wChairID);
	//}

	//事件通知
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
	}

	//事件通知
	if (m_pIMatchTableAction != NULL)
	{
		m_pIMatchTableAction->OnActionUserSitDown(wChairID, pIServerUserItem, false);
	}

	// 玩家坐下，如果是手机玩家，将桌子上所有玩家发送给自己
	if ( m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	{
		for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem = m_TableUserItemArray[i];
			if (pIUserItem == NULL) continue;
			if (pIUserItem->GetUserID() == pIServerUserItem->GetUserID()) continue;

			if (m_pIMainServiceFrame!=NULL)
			{
				m_pIMainServiceFrame->SendUserInfo(pIUserItem, pIServerUserItem->GetBindIndex());
			}
		}
	}

	// 玩家坐下，将自己发送给桌子上的其他手机玩家
	for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
	{
		//获取用户
		IServerUserItem *pISerUserItem = m_TableUserItemArray[i];
		if (pISerUserItem == NULL) continue;
		if (pISerUserItem->GetUserID() == pIServerUserItem->GetUserID()) continue;

		if (m_pIMainServiceFrame != NULL)
		{
			m_pIMainServiceFrame->SendUserInfo(pIServerUserItem, pISerUserItem->GetBindIndex());
		}
	}

	// 玩家坐下，把自己发送给手机旁观玩家
	for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
	{
		IServerUserItem *pIServerUserItemLookon = m_LookonUserItemArray[i];
		if (pIServerUserItemLookon == NULL) continue;
		if (pIServerUserItemLookon->GetUserID() == pIServerUserItem->GetUserID()) continue;

		if (m_pIMainServiceFrame != NULL)
		{
			m_pIMainServiceFrame->SendUserInfo(pIServerUserItem, pIServerUserItemLookon->GetBindIndex());
		}
	}

	//if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	//{
	//	//开始判断
	//	if (EfficacyStartGame(INVALID_CHAIR)==true)
	//	{
	//		StartGame(); 
	//	}
	//}

	return true;
}

//恢复旁观
bool CTableFrame::PerformRecoerLookOn(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (wChairID == INVALID_CHAIR || pIServerUserItem == nullptr)
	{
		return false;
	}

	//设置用户
	m_LookonUserItemArray.Add(pIServerUserItem);

	//用户状态
	pIServerUserItem->SetClientReady(true);
	pIServerUserItem->SetUserStatus(US_LOOKON, m_wTableID, wChairID);

	return true;
}

//单元积分
VOID CTableFrame::SetCellScore(LONG lCellScore)
{
	m_lCellScore=lCellScore;
	if (m_bTableInitFinish==true)
	{
		SendTableStatus();
		m_pITableFrameSink->SetGameBaseScore(lCellScore);
	}
}

//桌子状态
bool CTableFrame::SendTableStatus()
{
	//变量定义
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//构造数据
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.lCellScore=m_lCellScore;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//电脑数据
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//手机数据

	return true;
}

//手机请求失败
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, BYTE cbErrType, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure, sizeof(RequestFailure));

	//构造数据
	RequestFailure.cbErrType = cbErrType;
	RequestFailure.lErrorCode = lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString, pszDescribe, CountArray(RequestFailure.szDescribeString));

	//发送数据
	WORD wDataSize = CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize = sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_REQUEST_FAILURE, &RequestFailure, wHeadSize + wDataSize);

	return true;
}

//开始效验
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	//状态判断
	if (m_bGameStarted==true) return false;

	//模式过滤
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//准备人数
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//用户统计
		if (pITableUserItem!=NULL)
		{
			//状态判断
			if (pITableUserItem->IsClientReady() == false)
			{
				return false;
			}
			if ((wReadyChairID != i) && (pITableUserItem->GetUserStatus() != US_READY))
			{
				return false;
			}

			//用户计数
			wReadyUserCount++;
		}
	}

	//开始处理
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//所有准备
		{
			//数目判断
			if (wReadyUserCount>=2L) return true;

			return false;
		}
	case START_MODE_FULL_READY:			//满人开始
		{
			//人数判断
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//配对开始
		{
			//数目判断
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//位置判断
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//获取用户
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//位置过滤
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//地址效验
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//百人不限制
	if(m_wChairCount > MAX_CHAIR_GENERAL) return true;

	//地址效验
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
		{
			pTableUserRule=pITableUserItem->GetUserRule();
			if (pTableUserRule->bLimitSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}

	//地址效验
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					//发送信息
					//LPCTSTR pszDescribe=TEXT("此游戏桌玩家设置了不跟相同 IP 地址的玩家游戏，您 IP 地址与此玩家的 IP 地址相同，不能加入游戏！");
					//SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_IP_SAME);
					return false;
				}
				else
				{
					//发送信息
					/*LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);*/
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_YOU_IP_SAME);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//获取用户
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//获取用户
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						//LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
						//SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
						SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_YOU_IP_SAME);
						return false;
					}
				}
			}
		}
	}
	return true;
}

//积分效验
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//管理员不受限制
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//规则判断
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//百人不限制
	if(m_wChairCount > MAX_CHAIR_GENERAL) return true;

	//变量定义
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//积分范围
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//规则效验
		if (pITableUserItem!=NULL)
		{
			//获取规则
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//逃率效验
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				//发送信息
				//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

				return false;
			}

			//胜率效验
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//构造信息
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

				//发送信息
				//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

				return false;
			}

			//积分效验
			if (pTableUserRule->bLimitGameScore==true)
			{
				//最高积分
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					//发送信息
					//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

					return false;
				}

				//最低积分
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//构造信息
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

					//发送信息
					//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

					return false;
				}
			}
		}
	}

	return true;
}

//积分效验
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//积分变量
	//SCORE lUserScore = pIServerUserItem->GetUserScore() + pIServerUserItem->GetTrusteeScore()*m_pGameServiceOption->iExchangeRatio;
	//SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore*m_pGameServiceOption->iExchangeRatio;
	SCORE lUserScore = pIServerUserItem->GetUserScore() + pIServerUserItem->GetTrusteeScore();
	SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//构造信息
		TCHAR szDescribe[128]=TEXT("");
		if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的游戏币，您的游戏币不够，不能加入！"), __max(lMinTableScore, lLessEnterScore));
		}
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的比赛币，您的比赛币不够，不能加入！"), __max(lMinTableScore, lLessEnterScore));
		}
		else
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("加入游戏至少需要 ") SCORE_STRING TEXT(" 的游戏积分，您的积分不够，不能加入！"), __max(lMinTableScore, lLessEnterScore));
		}

		//发送信息
		//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NOSCORE, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

		return false;
	}

	return true;
}

//检查分配
bool CTableFrame::CheckDistribute()
{
	//防作弊
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
	{
		//桌子状况
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//用户起立
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//游戏记录
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	if (bDrawStarted==true)
	{
		//写入记录
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//变量定义
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//设置变量
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//游戏时间
			if(dwStartGameTime!=INVALID_DWORD)
			{
				CTime startTime(dwStartGameTime);
				startTime.GetAsSystemTime(GameScoreRecord.SystemTimeStart);
			}
			else
			{
				GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			}
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//用户积分
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//获取对象
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//用户数目
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//奖牌统计
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//统计信息
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//成绩信息
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//投递数据
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//清理记录
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}

void CTableFrame::ExportInformation(LPCTSTR pszString, bool bError/*=false */)
{
	if (pszString != NULL && *pszString != 0)
	{
		CTraceService::TraceString(pszString, (bError ? TraceLevel_Exception : TraceLevel_Debug));
	}
}

void CTableFrame::ExportFormatInfo(bool bError, LPCTSTR pszFormat, ...)
{
	if (pszFormat != NULL && *pszFormat != 0)
	{
		CString str;
		va_list ptr;
		va_start(ptr, pszFormat);
		str.FormatV(pszFormat, ptr);
		va_end(ptr);
		CTraceService::TraceString(str, (bError ? TraceLevel_Exception : TraceLevel_Debug));
	}
}

//发送救济金
void CTableFrame::SendBenefitInfo(IServerUserItem * pIServerUserItem, SYSTEMTIME SystemTime)
{
	if (pIServerUserItem == NULL) return;
	
	DBR_GR_EMAIL EmailData;
	ZeroMemory(&EmailData, sizeof(EmailData));

	EmailData.dwRecUserID = pIServerUserItem->GetUserID();
	EmailData.cbType = EN_EMAIL_SYSTIME;
	//EmailData.wRewardPropID = m_pBenefitInfo->RewardPropID;
	CopyMemory(EmailData.szRewardProp, m_pBenefitInfo->szRewardProp, sizeof(m_pBenefitInfo->szRewardProp));
	CopyMemory(&EmailData.createTime, &SystemTime, sizeof(SYSTEMTIME));
	CopyMemory(EmailData.szEmailTitle, m_pBenefitInfo->szEmailTitle, sizeof(m_pBenefitInfo->szEmailTitle));
	
	TCHAR szTime[32] = TEXT("");
	_sntprintf_s(szTime, CountArray(szTime),TEXT("%d-%02d-%02d %02d:%02d:%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
	_sntprintf_s(EmailData.szEmailMsg, CountArray(EmailData.szEmailMsg), m_pBenefitInfo->szEmailMsg, szTime);

	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_SEND_EMAIL, 0, &EmailData, sizeof(EmailData));

	return;
}

//////////////////////////////////////////////////////////////////////////////////
//设置私人场信息
void CTableFrame::SetPrivateInfo(tagPrivateFrameParameter &PriavateFrame)
{
	CopyMemory(&m_PrivateFramePrarameter, &PriavateFrame, sizeof(tagPrivateFrameParameter));
}

//获取私人场配置
tagPrivateFrameParameter* CTableFrame::GetPrivateFrameInfo()
{
	return &m_PrivateFramePrarameter;
}

//获取当前完成数据
tagPrivateFrameRecordInfo* CTableFrame::GetPrivateFrameRecord()
{
	return &m_PrivateFrameRecord;
}

//统计私人场玩家局数
VOID  CTableFrame::addPrivatePlayCout(WORD wCout)
{
	m_PrivateFrameRecord.cbFinishCout += wCout;

	if (m_pITableFrameHook)
	{
		m_pITableFrameHook->addPrivatePlayCout(this, wCout);
	}
}

//重置私人场结束信息;
void CTableFrame::ResetPrivateEndInfo()
{
	ZeroMemory(&m_PrivateFrameRecord, sizeof(m_PrivateFrameRecord));

	if (m_pITableFrameSink)
	{
		m_pITableFrameSink->ResetPrivateEndInfo();
	}
}

//获得私人场结束信息流
void CTableFrame::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{
	if (m_pITableFrameSink)
	{
		m_pITableFrameSink->GetPrivateEndInfo(kDataStream, bSend);
	}
}

//判断私人场是否结束
bool CTableFrame::IsPrivateEnd()
{
	// 判断服务器维护;
	if (CServerRule::IsForfendGameStart(m_pGameServiceOption->dwServerRule) == true)
	{
		return true;
	}

	// 游戏是否结束;
	if (m_pITableFrameSink)
	{
		return m_pITableFrameSink->IsPrivateEnd();
	}

	return false;
}


//获取牌库
tagGameCardLibrary* CTableFrame::GetGameCardLibrary(DWORD dwCustomID/* = 0xFFFFFFFF*/)
{
	INT_PTR nLibCount = m_GameCardLibraryBuffer.GetCount();

	if (0xFFFFFFFF == dwCustomID)
	{
		if (nLibCount > 0L)
		{
			INT_PTR nIndex = rand() % nLibCount;
			return m_GameCardLibraryBuffer[nIndex];
		}
	}
	else
	{
		if (nLibCount > 0L && dwCustomID <= CServerUserItem::g_dwCardLibCount)
		{
			//return m_GameCardLibraryBuffer[dwIndex];
			for (INT_PTR i = 0; i < nLibCount; i++)
			{
				tagGameCardLibrary* pCardLib = m_GameCardLibraryBuffer[i];
				if (pCardLib->dwCustomID == dwCustomID)
				{
					return pCardLib;
				}
			}
		}
	}

	return NULL;
}

//增加牌库
void CTableFrame::AddGameCardLibrary(tagGameCardLibrary* pGameCard)
{
	m_GameCardLibraryBuffer.Add(pGameCard);
}

//修改牌库
void CTableFrame::UpdateGameCardLibrary(DWORD dwLibID, DWORD dwCustomID, BYTE cbCardCount, BYTE* pCardData)
{
	ASSERT(pCardData != NULL);
	INT_PTR nCount = m_GameCardLibraryBuffer.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		tagGameCardLibrary* pCardLib = m_GameCardLibraryBuffer[i];
		if (pCardLib->dwLibID == dwLibID)
		{
			pCardLib->cbCount = cbCardCount;
			pCardLib->dwCustomID = dwCustomID;

			ZeroMemory(pCardLib->cbData, sizeof(pCardLib->cbData));
			memcpy(pCardLib->cbData, pCardData, cbCardCount*sizeof(BYTE));

			break;
		}
	}
}

//删除牌库
void CTableFrame::ClearGameCardLibrary()
{
	for (INT_PTR i = 0; i < m_GameCardLibraryBuffer.GetCount(); i++)
	{
		tagGameCardLibrary* pCardLib = m_GameCardLibraryBuffer[i];
		SafeDelete(pCardLib);
	}

	m_GameCardLibraryBuffer.RemoveAll();
}









////////////////////////////////////////////////////////////////////////////////////////////
