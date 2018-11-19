#include "StdAfx.h"
#include "TableFrameSink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数

//////////////////////////////////////////////////////////////////////////

#define	IDI_SO_OPERATE							12							//代打定时器
#define	TIME_SO_OPERATE							40000						//代打定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量	
	m_lExitScore=0;	
	m_wBankerUser=INVALID_CHAIR;
	m_wFisrtCallUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//用户状态
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_bCallStatus,sizeof(m_bCallStatus));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_bOxCard[i]=0xff;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询--检测相关信息版本
VOID * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;
    
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	return true;
}

//复位桌子
VOID  CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_lExitScore=0;	
	m_wCurrentUser=INVALID_CHAIR;
	//m_wOperaCount=0;

	//用户状态
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_bCallStatus,sizeof(m_bCallStatus));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_bOxCard[i]=0xff;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	return;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_TK_CALL);

	//用户状态
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);

		if(pIServerUser==NULL)
		{
			m_cbPlayStatus[i]=USEX_NULL;
		}
		else
		{
			m_cbPlayStatus[i]=USEX_PLAYING;
		}
	}

	//首局随机始叫
	if(m_wFisrtCallUser==INVALID_CHAIR)
	{
		m_wFisrtCallUser=rand()%m_wPlayerCount;
	}
	else
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//始叫用户
	while(m_cbPlayStatus[m_wFisrtCallUser]!=USEX_PLAYING)
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//当前用户
	m_wCurrentUser=m_wFisrtCallUser;

	//设置变量
	CMD_S_CallBanker CallBanker;
	CallBanker.wCallBanker=m_wCurrentUser;
	CallBanker.bFirstTimes=true;

	//发送数据
	/*for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==USEX_NULL)continue;
		m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}*/
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			WORD m_wWinTimes[GAME_PLAYER],m_wWinCount[GAME_PLAYER];
			ZeroMemory(m_wWinCount,sizeof(m_wWinCount));
			ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));

			//保存扑克
			BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
			CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

			//庄家倍数
			ASSERT(m_bOxCard[m_wBankerUser]<2);
			if(m_bOxCard[m_wBankerUser]==TRUE)
				m_wWinTimes[m_wBankerUser]=m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser],MAX_COUNT);
			else m_wWinTimes[m_wBankerUser]=1;

			//对比玩家
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;

				ASSERT(m_bOxCard[i]<2);

				//对比扑克
				if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,m_bOxCard[i],m_bOxCard[m_wBankerUser])) 
				{
					m_wWinCount[i]++;
					//获取倍数
					if(m_bOxCard[i]==TRUE)
						m_wWinTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
					else m_wWinTimes[i]=1;
				}
				else
				{
					m_wWinCount[m_wBankerUser]++;
				}
			}

			//统计得分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;

				WORD j=i;
				if(m_wWinCount[j]>0)	//闲家胜利
				{
					GameEnd.lGameScore[j]=m_lTableScore[j]*m_wWinTimes[j];
					GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[j];
					m_lTableScore[j]=0;
				}
				else					//庄家胜利
				{
					GameEnd.lGameScore[j]=(-1)*m_lTableScore[j]*m_wWinTimes[m_wBankerUser];
					GameEnd.lGameScore[m_wBankerUser]+=(-1)*GameEnd.lGameScore[j];
					m_lTableScore[j]=0;
				}
			}

			//闲家强退分数	
			GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

			//处理负数
			if(GameEnd.lGameScore[m_wBankerUser]<0L)
			{
				//庄家积分
				IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(m_wBankerUser);
				LONGLONG lBankerScore=pIServerUser->GetUserScore();

				//不够赔偿,客户要求
				if(lBankerScore+GameEnd.lGameScore[m_wBankerUser]<0L)
				{
					//玩家得分情况
					LONGLONG lBankerGetScore=lBankerScore;

					//闲家总得分
					LONGLONG lIdlerGetScore=(-1)*GameEnd.lGameScore[m_wBankerUser];

					//庄家总得分
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						else if(GameEnd.lGameScore[i]<0L)		
						{
							lBankerGetScore+=(-1)*GameEnd.lGameScore[i];
						}
					}

					//失分概率 1-（庄家本钱+赢闲家的钱）/∑(赢的闲家下注数*闲家倍数)
					ASSERT(lIdlerGetScore>lBankerGetScore);

					//统计得分
					LONGLONG dTemp = 0;
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(GameEnd.lGameScore[i]>0L)
						{
							dTemp = GameEnd.lGameScore[i]-GameEnd.lGameScore[i]*(lIdlerGetScore-lBankerGetScore)/lIdlerGetScore;
							GameEnd.lGameScore[i] = dTemp;
						}
					}

					//扣除总分
					GameEnd.lGameScore[m_wBankerUser]=(-1)*lBankerScore;
				}
			}

			//离开用户
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//扣税变量
			WORD wRevenue=m_pGameServiceOption->wRevenueRatio;

			//积分税收
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
			for(WORD i=0;i<m_wPlayerCount;i++)
			{

				if(m_cbPlayStatus[i]==USEX_NULL) continue;
				if(GameEnd.lGameScore[i]>0L)
				{
					GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]); 
					GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
					ScoreInfoArray[i].cbType=SCORE_TYPE_WIN;
				}
				else
					ScoreInfoArray[i].cbType=SCORE_TYPE_LOSE;
				ScoreInfoArray[i].lRevenue=GameEnd.lGameTax[i];                                  
                ScoreInfoArray[i].lScore=GameEnd.lGameScore[i];
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]!=USEX_NULL));

			//设置状态
			m_cbPlayStatus[wChairID]=USEX_NULL;
			m_lTurnMaxScore[wChairID]=0;

			//定义变量
			CMD_S_PlayerExit PlayerExit;
			PlayerExit.wPlayerID=wChairID;

			//发送信息
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==wChairID || (m_cbPlayStatus[i]==USEX_NULL))continue;
				m_pITableFrame->SendTableData(i,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));
			}
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));

			WORD m_wWinTimes[GAME_PLAYER];
			ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));
			if (m_pITableFrame->GetGameStatus()>GS_TK_CALL)
			{
				if (wChairID==m_wBankerUser)	//庄家强退
				{
					//定义变量
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));
					ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));

					BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
					CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

					//得分倍数
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						m_wWinTimes[i]=(m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING)?(1):(m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT));
					}

					//统计得分 已下或没下
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						GameEnd.lGameScore[i]=m_lTableScore[i]*m_wWinTimes[i];
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
						m_lTableScore[i]=0;
					}

					//闲家强退分数 
					GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

					//处理负数
					if(GameEnd.lGameScore[m_wBankerUser]<0L)
					{
						//庄家积分
						IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
						LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

						//不够赔偿,客户要求
						if(lBankerScore+GameEnd.lGameScore[m_wBankerUser]<0L)
						{
							//玩家得分情况
							LONGLONG lBankerGetScore=lBankerScore;
							LONGLONG lIdlerGetScore=0;
							for (WORD i=0;i<m_wPlayerCount;i++)
							{
								if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
								if(GameEnd.lGameScore[i]>0L)			//闲家总得分
								{
									lIdlerGetScore+=GameEnd.lGameScore[i];
								}
							}

							//失分概率 1-（庄家本钱+赢闲家的钱）/∑(赢的闲家下注数*闲家倍数)
							ASSERT(lIdlerGetScore>lBankerGetScore);

							//统计得分
							for (WORD i=0;i<m_wPlayerCount;i++)
							{
								if(GameEnd.lGameScore[i]>0L)
								{
									GameEnd.lGameScore[i]-=(GameEnd.lGameScore[i]*
										(lIdlerGetScore-lBankerGetScore)/lIdlerGetScore);
								}
							}

							//扣除总分
							GameEnd.lGameScore[m_wBankerUser]=(-1)*lBankerScore;
						}
					}

					//离开用户
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
					}

					//扣税变量
					WORD wRevenue=m_pGameServiceOption->wRevenueRatio;

					//积分税收
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(GameEnd.lGameScore[i]>0L)
						{
							GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]); 
							GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
						}
					}

					//发送信息
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || (m_cbPlayStatus[i]==USEX_NULL))continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

					//修改积分
					tagScoreInfo ScoreInfo[GAME_PLAYER];
					ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]!=USEX_PLAYING && i!=m_wBankerUser) continue;
						ScoreInfo[i].lRevenue=GameEnd.lGameTax[i];
						ScoreInfo[i].lScore=GameEnd.lGameScore[i];
						if(i==wChairID) 
							ScoreInfo[i].cbType =SCORE_TYPE_FLEE;
						else
						{
							if(GameEnd.lGameScore[i]>0)
								ScoreInfo[i].cbType=SCORE_TYPE_WIN;
							else
								ScoreInfo[i].cbType=SCORE_TYPE_LOSE;
						}
					}
					m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

					//获取用户
					IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(m_wBankerUser);
			
					//结束游戏
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
				}
				else						//闲家强退
				{
					tagScoreInfo ScoreInfo[GAME_PLAYER];
					ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

					//已经下注
					if (m_lTableScore[wChairID]>0L)
					{
						ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));

						//用户扑克
						BYTE cbUserCardData[MAX_COUNT];
						CopyMemory(cbUserCardData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);

						//用户倍数
						m_wWinTimes[m_wBankerUser]=(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)?(1):(m_GameLogic.GetTimes(cbUserCardData,MAX_COUNT));

						//修改积分
						LONGLONG lScore=-m_lTableScore[wChairID]*m_wWinTimes[m_wBankerUser];
						m_lExitScore+=(-m_wWinTimes[m_wBankerUser]*lScore);
						m_lTableScore[wChairID]=(-m_wWinTimes[m_wBankerUser]*lScore);
						////
						ScoreInfo[wChairID].lScore = lScore;
						ScoreInfo[wChairID].cbType = SCORE_TYPE_FLEE;
						
						//获取用户
						IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
					}

					//玩家人数
					WORD wUserCount=0;
					for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==USEX_PLAYING)wUserCount++;

					//结束游戏
					if(wUserCount==1)
					{
						//定义变量
						CMD_S_GameEnd GameEnd;
						ZeroMemory(&GameEnd,sizeof(GameEnd));
						ASSERT(m_lExitScore>=0L); 

						//统计得分
						GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;
						GameEnd.lGameTax[m_wBankerUser]=m_pITableFrame->CalculateRevenue(m_wBankerUser,GameEnd.lGameScore[m_wBankerUser]);
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameTax[m_wBankerUser];

						//离开用户
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if(m_lTableScore[i]>0)
								GameEnd.lGameScore[i]=-m_lTableScore[i];
						}

						//发送信息
						m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
						m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

						WORD nZero = 0;
						for (nZero = 0; nZero < m_wPlayerCount; nZero++)
						{
							if (m_lTableScore[nZero] != 0)
							{
								break;
							}
						}
							
						if (nZero != m_wPlayerCount)
						{
							//修改积分
							LONGLONG lRevenue = GameEnd.lGameTax[m_wBankerUser];
							LONGLONG lScore=GameEnd.lGameScore[m_wBankerUser];
		

							if(m_wBankerUser!=INVALID_CHAIR)
							{
								ScoreInfo[m_wBankerUser].lScore =lScore;
								ScoreInfo[m_wBankerUser].lRevenue = lRevenue;
								ScoreInfo[m_wBankerUser].cbType =SCORE_TYPE_WIN;
							}
					
							//获取用户
							IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
						}
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
						//结束游戏
						m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);		
					}
					else if	(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)
					{
						if(m_lTableScore[wChairID]==0L)
							OnUserAddScore(wChairID,0);
						else 
						//写入积分
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
					}
					else if (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING && m_bOxCard[wChairID]==0xff)
					{
						OnUserOpenCard(wChairID,0);
						//写入积分
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
					}
				}
			}
			else 
			{
				//玩家人数
				WORD wUserCount=0;
				for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==USEX_PLAYING)wUserCount++;

				//结束游戏
				if(wUserCount==1)
				{
					//定义变量
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));

					//发送信息
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(/*i==m_wBankerUser || */m_cbPlayStatus[i]==USEX_NULL)continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

					//结束游戏
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);			
				}
				else if(m_wCurrentUser==wChairID)OnUserCallBanker(wChairID,0);
			}

			return true;
		}
	}

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_TK_CALL:	//叫庄状态
		{
			//构造数据
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//设置变量
			StatusCall.wCallBanker=m_wCurrentUser;
			CopyMemory(StatusCall.cbPlayStatus,m_cbPlayStatus,sizeof(StatusCall.cbPlayStatus));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
		}
	case GS_TK_SCORE:	//下注状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//加注信息
			StatusScore.lTurnMaxScore=m_lTurnMaxScore[wChairID];
			StatusScore.wBankerUser=m_wBankerUser;
			CopyMemory(StatusScore.cbPlayStatus,m_cbPlayStatus,sizeof(StatusScore.cbPlayStatus));

			//设置积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
				StatusScore.lTableScore[i]=m_lTableScore[i];
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_TK_PLAYING:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//设置信息
			StatusPlay.lTurnMaxScore=m_lTurnMaxScore[wChairID];
			StatusPlay.wBankerUser=m_wBankerUser;
			CopyMemory(StatusPlay.bOxCard,m_bOxCard,sizeof(StatusPlay.bOxCard));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(StatusPlay.cbPlayStatus));

			//设置扑克
			for (WORD i=0;i< m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
				WORD j= i;
				StatusPlay.lTableScore[j]=m_lTableScore[j];
				CopyMemory(StatusPlay.cbHandCardData[j],m_cbHandCardData[j],MAX_COUNT);
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}
	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch(wTimerID)
	{
	case IDI_SO_OPERATE:
		{
			//删除时间
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//游戏状态
			switch( m_pITableFrame->GetGameStatus() )
			{
			case GS_TK_CALL:			//用户叫庄
				{
					OnUserCallBanker(m_wCurrentUser, 0);
					break;
				}
			case GS_TK_SCORE:			//下注操作
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0L || m_cbPlayStatus[i]!=USEX_PLAYING || i==m_wBankerUser)continue;
						OnUserAddScore(i,m_lTurnMaxScore[i]/8);
					}

					break;
				}
			case GS_TK_PLAYING:			//用户开牌
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_bOxCard[i]<2 || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						OnUserOpenCard(i, 0);
					}

					break;
				}
			default:
				{
					break;
				}
			}

			if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
				m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
			return true;
		}
	}

	return false;
}

//游戏消息处理 
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	bool bResult=false;
	switch (wSubCmdID)
	{
	case SUB_C_CALL_BANKER:			//用户叫庄
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallBanker));
			if (wDataSize!=sizeof(CMD_C_CallBanker)) return false;

			//变量定义
			CMD_C_CallBanker * pCallBanker=(CMD_C_CallBanker *)pData;

			//用户效验
			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (pUserInfo->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserInfo->wChairID));
			if (!IsUserPlaying(pUserInfo->wChairID)) return true;

			//消息处理
			bResult=OnUserCallBanker(pUserInfo->wChairID,pCallBanker->bBanker);
			break;
		}
	case SUB_C_ADD_SCORE:			//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//用户效验
			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (pUserInfo->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserInfo->wChairID));
			if (!IsUserPlaying(pUserInfo->wChairID)) return true;

			//消息处理
			bResult=OnUserAddScore(pUserInfo->wChairID,pAddScore->lScore);
			break;
		}
	case SUB_C_OPEN_CARD:			//用户摊牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OxCard));
			if (wDataSize!=sizeof(CMD_C_OxCard)) return false;

			//变量定义
			CMD_C_OxCard * pOxCard=(CMD_C_OxCard *)pData;

			//用户效验
			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (pUserInfo->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserInfo->wChairID]==USEX_PLAYING);
			if(m_cbPlayStatus[pUserInfo->wChairID]!=USEX_PLAYING)return true;

			//消息处理
			bResult=OnUserOpenCard(pUserInfo->wChairID,pOxCard->bOX);
			break;
		}
	}

	//操作定时器
	if(bResult)
	{
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
		return true;
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(bLookonUser==false && m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
		m_cbPlayStatus[pIServerUserItem->GetChairID()]=USEX_DYNAMIC;

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(bLookonUser==false)	m_cbPlayStatus[wChairID]=USEX_NULL;
	return true;
}


//叫庄事件
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, BYTE bBanker)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_CALL) return true;
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return true;

	//设置变量
	m_bCallStatus[wChairID]=TRUE;

	//叫庄人数
	WORD wCallUserCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==USEX_PLAYING && m_bCallStatus[i]==TRUE) wCallUserCount++;
		else if(m_cbPlayStatus[i]!=USEX_PLAYING) wCallUserCount++;
	}

	//下注开始
	if(bBanker==TRUE || wCallUserCount==m_wPlayerCount)
	{
		//始叫用户
		m_wBankerUser=wChairID;
		m_wCurrentUser=INVALID_CHAIR;

		//过滤最后一个叫庄用户强退情况
		while(m_cbPlayStatus[m_wBankerUser]!=USEX_PLAYING)
		{
			m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;
		}

		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_SCORE);

		//庄家积分
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
		LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

		//玩家人数
		WORD wUserCount=0;
		for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==USEX_PLAYING )wUserCount++;

		//最大下注
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=USEX_PLAYING || i==m_wBankerUser)continue;

			//获取用户
			pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//获取积分
			LONGLONG lUserScore=pIServerUserItem->GetUserScore();

			//金币检验
			ASSERT(lUserScore>=m_pGameServiceOption->lCellScore);

			//下注变量 客户要求
			m_lTurnMaxScore[i]=__min(lBankerScore/(wUserCount-1)/5,lUserScore/5);
		}

		//设置变量
		//m_wOperaCount=0;
		CMD_S_GameStart GameStart;
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.lTurnMaxScore=0;

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==USEX_NULL )continue;
			GameStart.lTurnMaxScore=m_lTurnMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	else		 //用户叫庄
	{
		//查找下个玩家
		do{
			m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		}while(m_cbPlayStatus[m_wCurrentUser]!=USEX_PLAYING);

		//设置变量
		CMD_S_CallBanker CallBanker;
		CallBanker.wCallBanker=m_wCurrentUser;
		CallBanker.bFirstTimes=false;

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}
	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

	//金币效验
	if(m_cbPlayStatus[wChairID]==USEX_PLAYING)
	{
		ASSERT(lScore>0 && lScore<=m_lTurnMaxScore[wChairID]);
		if (lScore<=0 || lScore>m_lTurnMaxScore[wChairID]) return false;
	}
	else //没下注玩家强退
	{
		ASSERT(lScore==0);
		if (lScore!=0) return false;
	}

	if(lScore>0L)
	{
		//下注金币
		m_lTableScore[wChairID]=lScore;

		//构造数据
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.lAddScoreCount=m_lTableScore[wChairID];

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	}

	//下注人数
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_lTableScore[i]>0L && m_cbPlayStatus[i]==USEX_PLAYING)bUserCount++;
		else if(m_cbPlayStatus[i]!=USEX_PLAYING || i==m_wBankerUser)bUserCount++;
	}

	//闲家全到
	if(bUserCount==m_wPlayerCount)
	{
		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_PLAYING);

		//构造数据
		CMD_S_SendCard SendCard;
		ZeroMemory(SendCard.cbCardData,sizeof(SendCard.cbCardData));

		//获取扑克
		BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
		m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;

			//派发扑克
			CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
		}
		
		//发送扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;

			//派发扑克
			CopyMemory(SendCard.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
		}

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	}

	return true;
}

//摊牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE bOx)
{
	//状态效验
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING) return true;
	if (m_bOxCard[wChairID]!=0xff) return true;

	//效验数据
	ASSERT(bOx<=TRUE);
	if(bOx>TRUE)return false;

	//效验数据
	if(bOx)
	{
		ASSERT(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0);
		if(!(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0))return false;
	}
	else if(m_cbPlayStatus[wChairID]==USEX_PLAYING)
	{
		if(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>=OX_THREE_SAME)bOx=TRUE;
	}

	//牛牛数据
	m_bOxCard[wChairID] = bOx;

	//摊牌人数
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_bOxCard[i]<2 && m_cbPlayStatus[i]==USEX_PLAYING)bUserCount++;
		else if(m_cbPlayStatus[i]!=USEX_PLAYING)bUserCount++;
	}

	 //构造变量
	CMD_S_Open_Card OpenCard;
	ZeroMemory(&OpenCard,sizeof(OpenCard));

	//设置变量
	OpenCard.bOpen=bOx;
	OpenCard.wPlayerID=wChairID;

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	

	//结束游戏
	if(bUserCount == m_wPlayerCount)
	{
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//扑克分析
void CTableFrameSink::AnalyseCard()
{
	//机器人数
	bool bIsAiBanker = false;
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			ASSERT(m_cbPlayStatus[i]==USEX_PLAYING);
			
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
				if(!bIsAiBanker && i==m_wBankerUser)bIsAiBanker = true;
			}
			wPlayerCount++; 
		}
	}

	//全部机器
	if(wPlayerCount == wAiCount || wAiCount==0)return;

	//扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//牛牛数据
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	//排列扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//变量定义
	LONGLONG lAndroidScore=0;

	//倍数变量
	BYTE cbCardTimes[GAME_PLAYER];
	ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

	//查找倍数
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==USEX_PLAYING)
		{
			cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
		}
	}

	//机器庄家
	if(bIsAiBanker)
	{
		//对比扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//用户过滤
			if ((i==m_wBankerUser)||(m_cbPlayStatus[i]!=USEX_PLAYING)) continue;

			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//机器过滤
			if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser())) continue;

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore-=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}
	else//用户庄家
	{
		//对比扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//用户过滤
			if ((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser())) continue;

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore+=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}

	//库存判断
	if ((m_lStockScore+lAndroidScore)<0)
	{
		//变量定义
		WORD wWinUser=INVALID_CHAIR;

		//判断最大
		bool bCardMax = (bIsAiBanker)?true:false;

		//查找数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//用户过滤
			if (m_cbPlayStatus[i]!=USEX_PLAYING) continue;

			//设置用户
			if (wWinUser==INVALID_CHAIR)
			{
				wWinUser=i;
				continue;
			}

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wWinUser],MAX_COUNT,bUserOxData[i],bUserOxData[wWinUser])==bCardMax)
			{
				wWinUser=i;
			}
		}

		//交换数据
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);
		CopyMemory(m_cbHandCardData[m_wBankerUser],m_cbHandCardData[wWinUser],MAX_COUNT);
		CopyMemory(m_cbHandCardData[wWinUser],cbTempData,MAX_COUNT);
	}

	return;
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==USEX_PLAYING);
	if(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==USEX_PLAYING)return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
