#include "StdAfx.h"
#include "TableFrameSink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//��Ϸ����

//////////////////////////////////////////////////////////////////////////

#define	IDI_SO_OPERATE							12							//����ʱ��
#define	TIME_SO_OPERATE							40000						//����ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����	
	m_lExitScore=0;	
	m_wBankerUser=INVALID_CHAIR;
	m_wFisrtCallUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//�û�״̬
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_bCallStatus,sizeof(m_bCallStatus));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_bOxCard[i]=0xff;

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ע��Ϣ
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ--��������Ϣ�汾
VOID * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;
    
	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);
	//��Ϸ����
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	return true;
}

//��λ����
VOID  CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	m_lExitScore=0;	
	m_wCurrentUser=INVALID_CHAIR;
	//m_wOperaCount=0;

	//�û�״̬
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_bCallStatus,sizeof(m_bCallStatus));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_bOxCard[i]=0xff;

	//�˿˱���
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//��ע��Ϣ
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	return;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//����״̬
	m_pITableFrame->SetGameStatus(GS_TK_CALL);

	//�û�״̬
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
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

	//�׾����ʼ��
	if(m_wFisrtCallUser==INVALID_CHAIR)
	{
		m_wFisrtCallUser=rand()%m_wPlayerCount;
	}
	else
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//ʼ���û�
	while(m_cbPlayStatus[m_wFisrtCallUser]!=USEX_PLAYING)
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//��ǰ�û�
	m_wCurrentUser=m_wFisrtCallUser;

	//���ñ���
	CMD_S_CallBanker CallBanker;
	CallBanker.wCallBanker=m_wCurrentUser;
	CallBanker.bFirstTimes=true;

	//��������
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

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			WORD m_wWinTimes[GAME_PLAYER],m_wWinCount[GAME_PLAYER];
			ZeroMemory(m_wWinCount,sizeof(m_wWinCount));
			ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));

			//�����˿�
			BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
			CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

			//ׯ�ұ���
			ASSERT(m_bOxCard[m_wBankerUser]<2);
			if(m_bOxCard[m_wBankerUser]==TRUE)
				m_wWinTimes[m_wBankerUser]=m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser],MAX_COUNT);
			else m_wWinTimes[m_wBankerUser]=1;

			//�Ա����
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;

				ASSERT(m_bOxCard[i]<2);

				//�Ա��˿�
				if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,m_bOxCard[i],m_bOxCard[m_wBankerUser])) 
				{
					m_wWinCount[i]++;
					//��ȡ����
					if(m_bOxCard[i]==TRUE)
						m_wWinTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
					else m_wWinTimes[i]=1;
				}
				else
				{
					m_wWinCount[m_wBankerUser]++;
				}
			}

			//ͳ�Ƶ÷�
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;

				WORD j=i;
				if(m_wWinCount[j]>0)	//�м�ʤ��
				{
					GameEnd.lGameScore[j]=m_lTableScore[j]*m_wWinTimes[j];
					GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[j];
					m_lTableScore[j]=0;
				}
				else					//ׯ��ʤ��
				{
					GameEnd.lGameScore[j]=(-1)*m_lTableScore[j]*m_wWinTimes[m_wBankerUser];
					GameEnd.lGameScore[m_wBankerUser]+=(-1)*GameEnd.lGameScore[j];
					m_lTableScore[j]=0;
				}
			}

			//�м�ǿ�˷���	
			GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

			//������
			if(GameEnd.lGameScore[m_wBankerUser]<0L)
			{
				//ׯ�һ���
				IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(m_wBankerUser);
				LONGLONG lBankerScore=pIServerUser->GetUserScore();

				//�����⳥,�ͻ�Ҫ��
				if(lBankerScore+GameEnd.lGameScore[m_wBankerUser]<0L)
				{
					//��ҵ÷����
					LONGLONG lBankerGetScore=lBankerScore;

					//�м��ܵ÷�
					LONGLONG lIdlerGetScore=(-1)*GameEnd.lGameScore[m_wBankerUser];

					//ׯ���ܵ÷�
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						else if(GameEnd.lGameScore[i]<0L)		
						{
							lBankerGetScore+=(-1)*GameEnd.lGameScore[i];
						}
					}

					//ʧ�ָ��� 1-��ׯ�ұ�Ǯ+Ӯ�мҵ�Ǯ��/��(Ӯ���м���ע��*�мұ���)
					ASSERT(lIdlerGetScore>lBankerGetScore);

					//ͳ�Ƶ÷�
					LONGLONG dTemp = 0;
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(GameEnd.lGameScore[i]>0L)
						{
							dTemp = GameEnd.lGameScore[i]-GameEnd.lGameScore[i]*(lIdlerGetScore-lBankerGetScore)/lIdlerGetScore;
							GameEnd.lGameScore[i] = dTemp;
						}
					}

					//�۳��ܷ�
					GameEnd.lGameScore[m_wBankerUser]=(-1)*lBankerScore;
				}
			}

			//�뿪�û�
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//��˰����
			WORD wRevenue=m_pGameServiceOption->wRevenueRatio;

			//����˰��
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

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//�û�ǿ��
	case GER_NETWORK_ERROR:
		{
			//Ч�����
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]!=USEX_NULL));

			//����״̬
			m_cbPlayStatus[wChairID]=USEX_NULL;
			m_lTurnMaxScore[wChairID]=0;

			//�������
			CMD_S_PlayerExit PlayerExit;
			PlayerExit.wPlayerID=wChairID;

			//������Ϣ
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
				if (wChairID==m_wBankerUser)	//ׯ��ǿ��
				{
					//�������
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));
					ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));

					BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
					CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

					//�÷ֱ���
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						m_wWinTimes[i]=(m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING)?(1):(m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT));
					}

					//ͳ�Ƶ÷� ���»�û��
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
						GameEnd.lGameScore[i]=m_lTableScore[i]*m_wWinTimes[i];
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
						m_lTableScore[i]=0;
					}

					//�м�ǿ�˷��� 
					GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

					//������
					if(GameEnd.lGameScore[m_wBankerUser]<0L)
					{
						//ׯ�һ���
						IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
						LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

						//�����⳥,�ͻ�Ҫ��
						if(lBankerScore+GameEnd.lGameScore[m_wBankerUser]<0L)
						{
							//��ҵ÷����
							LONGLONG lBankerGetScore=lBankerScore;
							LONGLONG lIdlerGetScore=0;
							for (WORD i=0;i<m_wPlayerCount;i++)
							{
								if(i==m_wBankerUser || m_cbPlayStatus[i]!=USEX_PLAYING)continue;
								if(GameEnd.lGameScore[i]>0L)			//�м��ܵ÷�
								{
									lIdlerGetScore+=GameEnd.lGameScore[i];
								}
							}

							//ʧ�ָ��� 1-��ׯ�ұ�Ǯ+Ӯ�мҵ�Ǯ��/��(Ӯ���м���ע��*�мұ���)
							ASSERT(lIdlerGetScore>lBankerGetScore);

							//ͳ�Ƶ÷�
							for (WORD i=0;i<m_wPlayerCount;i++)
							{
								if(GameEnd.lGameScore[i]>0L)
								{
									GameEnd.lGameScore[i]-=(GameEnd.lGameScore[i]*
										(lIdlerGetScore-lBankerGetScore)/lIdlerGetScore);
								}
							}

							//�۳��ܷ�
							GameEnd.lGameScore[m_wBankerUser]=(-1)*lBankerScore;
						}
					}

					//�뿪�û�
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
					}

					//��˰����
					WORD wRevenue=m_pGameServiceOption->wRevenueRatio;

					//����˰��
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(GameEnd.lGameScore[i]>0L)
						{
							GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]); 
							GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
						}
					}

					//������Ϣ
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || (m_cbPlayStatus[i]==USEX_NULL))continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

					//�޸Ļ���
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

					//��ȡ�û�
					IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(m_wBankerUser);
			
					//������Ϸ
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
				}
				else						//�м�ǿ��
				{
					tagScoreInfo ScoreInfo[GAME_PLAYER];
					ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

					//�Ѿ���ע
					if (m_lTableScore[wChairID]>0L)
					{
						ZeroMemory(m_wWinTimes,sizeof(m_wWinTimes));

						//�û��˿�
						BYTE cbUserCardData[MAX_COUNT];
						CopyMemory(cbUserCardData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);

						//�û�����
						m_wWinTimes[m_wBankerUser]=(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)?(1):(m_GameLogic.GetTimes(cbUserCardData,MAX_COUNT));

						//�޸Ļ���
						LONGLONG lScore=-m_lTableScore[wChairID]*m_wWinTimes[m_wBankerUser];
						m_lExitScore+=(-m_wWinTimes[m_wBankerUser]*lScore);
						m_lTableScore[wChairID]=(-m_wWinTimes[m_wBankerUser]*lScore);
						////
						ScoreInfo[wChairID].lScore = lScore;
						ScoreInfo[wChairID].cbType = SCORE_TYPE_FLEE;
						
						//��ȡ�û�
						IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
					}

					//�������
					WORD wUserCount=0;
					for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==USEX_PLAYING)wUserCount++;

					//������Ϸ
					if(wUserCount==1)
					{
						//�������
						CMD_S_GameEnd GameEnd;
						ZeroMemory(&GameEnd,sizeof(GameEnd));
						ASSERT(m_lExitScore>=0L); 

						//ͳ�Ƶ÷�
						GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;
						GameEnd.lGameTax[m_wBankerUser]=m_pITableFrame->CalculateRevenue(m_wBankerUser,GameEnd.lGameScore[m_wBankerUser]);
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameTax[m_wBankerUser];

						//�뿪�û�
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if(m_lTableScore[i]>0)
								GameEnd.lGameScore[i]=-m_lTableScore[i];
						}

						//������Ϣ
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
							//�޸Ļ���
							LONGLONG lRevenue = GameEnd.lGameTax[m_wBankerUser];
							LONGLONG lScore=GameEnd.lGameScore[m_wBankerUser];
		

							if(m_wBankerUser!=INVALID_CHAIR)
							{
								ScoreInfo[m_wBankerUser].lScore =lScore;
								ScoreInfo[m_wBankerUser].lRevenue = lRevenue;
								ScoreInfo[m_wBankerUser].cbType =SCORE_TYPE_WIN;
							}
					
							//��ȡ�û�
							IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
						}
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
						//������Ϸ
						m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);		
					}
					else if	(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)
					{
						if(m_lTableScore[wChairID]==0L)
							OnUserAddScore(wChairID,0);
						else 
						//д�����
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
					}
					else if (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING && m_bOxCard[wChairID]==0xff)
					{
						OnUserOpenCard(wChairID,0);
						//д�����
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
					}
				}
			}
			else 
			{
				//�������
				WORD wUserCount=0;
				for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==USEX_PLAYING)wUserCount++;

				//������Ϸ
				if(wUserCount==1)
				{
					//�������
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));

					//������Ϣ
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(/*i==m_wBankerUser || */m_cbPlayStatus[i]==USEX_NULL)continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

					//������Ϸ
					m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);			
				}
				else if(m_wCurrentUser==wChairID)OnUserCallBanker(wChairID,0);
			}

			return true;
		}
	}

	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:		//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore=0L;

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_TK_CALL:	//��ׯ״̬
		{
			//��������
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//���ñ���
			StatusCall.wCallBanker=m_wCurrentUser;
			CopyMemory(StatusCall.cbPlayStatus,m_cbPlayStatus,sizeof(StatusCall.cbPlayStatus));

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
		}
	case GS_TK_SCORE:	//��ע״̬
		{
			//��������
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//��ע��Ϣ
			StatusScore.lTurnMaxScore=m_lTurnMaxScore[wChairID];
			StatusScore.wBankerUser=m_wBankerUser;
			CopyMemory(StatusScore.cbPlayStatus,m_cbPlayStatus,sizeof(StatusScore.cbPlayStatus));

			//���û���
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
				StatusScore.lTableScore[i]=m_lTableScore[i];
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_TK_PLAYING:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//������Ϣ
			StatusPlay.lTurnMaxScore=m_lTurnMaxScore[wChairID];
			StatusPlay.wBankerUser=m_wBankerUser;
			CopyMemory(StatusPlay.bOxCard,m_bOxCard,sizeof(StatusPlay.bOxCard));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(StatusPlay.cbPlayStatus));

			//�����˿�
			for (WORD i=0;i< m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
				WORD j= i;
				StatusPlay.lTableScore[j]=m_lTableScore[j];
				CopyMemory(StatusPlay.cbHandCardData[j],m_cbHandCardData[j],MAX_COUNT);
			}

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}
	//Ч�����
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch(wTimerID)
	{
	case IDI_SO_OPERATE:
		{
			//ɾ��ʱ��
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//��Ϸ״̬
			switch( m_pITableFrame->GetGameStatus() )
			{
			case GS_TK_CALL:			//�û���ׯ
				{
					OnUserCallBanker(m_wCurrentUser, 0);
					break;
				}
			case GS_TK_SCORE:			//��ע����
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0L || m_cbPlayStatus[i]!=USEX_PLAYING || i==m_wBankerUser)continue;
						OnUserAddScore(i,m_lTurnMaxScore[i]/8);
					}

					break;
				}
			case GS_TK_PLAYING:			//�û�����
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

//��Ϸ��Ϣ���� 
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	bool bResult=false;
	switch (wSubCmdID)
	{
	case SUB_C_CALL_BANKER:			//�û���ׯ
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_CallBanker));
			if (wDataSize!=sizeof(CMD_C_CallBanker)) return false;

			//��������
			CMD_C_CallBanker * pCallBanker=(CMD_C_CallBanker *)pData;

			//�û�Ч��
			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (pUserInfo->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(IsUserPlaying(pUserInfo->wChairID));
			if (!IsUserPlaying(pUserInfo->wChairID)) return true;

			//��Ϣ����
			bResult=OnUserCallBanker(pUserInfo->wChairID,pCallBanker->bBanker);
			break;
		}
	case SUB_C_ADD_SCORE:			//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//��������
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

			//�û�Ч��
			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (pUserInfo->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(IsUserPlaying(pUserInfo->wChairID));
			if (!IsUserPlaying(pUserInfo->wChairID)) return true;

			//��Ϣ����
			bResult=OnUserAddScore(pUserInfo->wChairID,pAddScore->lScore);
			break;
		}
	case SUB_C_OPEN_CARD:			//�û�̯��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_OxCard));
			if (wDataSize!=sizeof(CMD_C_OxCard)) return false;

			//��������
			CMD_C_OxCard * pOxCard=(CMD_C_OxCard *)pData;

			//�û�Ч��
			tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
			if (pUserInfo->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(m_cbPlayStatus[pUserInfo->wChairID]==USEX_PLAYING);
			if(m_cbPlayStatus[pUserInfo->wChairID]!=USEX_PLAYING)return true;

			//��Ϣ����
			bResult=OnUserOpenCard(pUserInfo->wChairID,pOxCard->bOX);
			break;
		}
	}

	//������ʱ��
	if(bResult)
	{
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
		return true;
	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�����¼�
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(bLookonUser==false && m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
		m_cbPlayStatus[pIServerUserItem->GetChairID()]=USEX_DYNAMIC;

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(bLookonUser==false)	m_cbPlayStatus[wChairID]=USEX_NULL;
	return true;
}


//��ׯ�¼�
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, BYTE bBanker)
{
	//״̬Ч��
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_CALL) return true;
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return true;

	//���ñ���
	m_bCallStatus[wChairID]=TRUE;

	//��ׯ����
	WORD wCallUserCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==USEX_PLAYING && m_bCallStatus[i]==TRUE) wCallUserCount++;
		else if(m_cbPlayStatus[i]!=USEX_PLAYING) wCallUserCount++;
	}

	//��ע��ʼ
	if(bBanker==TRUE || wCallUserCount==m_wPlayerCount)
	{
		//ʼ���û�
		m_wBankerUser=wChairID;
		m_wCurrentUser=INVALID_CHAIR;

		//�������һ����ׯ�û�ǿ�����
		while(m_cbPlayStatus[m_wBankerUser]!=USEX_PLAYING)
		{
			m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;
		}

		//����״̬
		m_pITableFrame->SetGameStatus(GS_TK_SCORE);

		//ׯ�һ���
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
		LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

		//�������
		WORD wUserCount=0;
		for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==USEX_PLAYING )wUserCount++;

		//�����ע
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=USEX_PLAYING || i==m_wBankerUser)continue;

			//��ȡ�û�
			pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//��ȡ����
			LONGLONG lUserScore=pIServerUserItem->GetUserScore();

			//��Ҽ���
			ASSERT(lUserScore>=m_pGameServiceOption->lCellScore);

			//��ע���� �ͻ�Ҫ��
			m_lTurnMaxScore[i]=__min(lBankerScore/(wUserCount-1)/5,lUserScore/5);
		}

		//���ñ���
		//m_wOperaCount=0;
		CMD_S_GameStart GameStart;
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.lTurnMaxScore=0;

		//��������
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==USEX_NULL )continue;
			GameStart.lTurnMaxScore=m_lTurnMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	else		 //�û���ׯ
	{
		//�����¸����
		do{
			m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		}while(m_cbPlayStatus[m_wCurrentUser]!=USEX_PLAYING);

		//���ñ���
		CMD_S_CallBanker CallBanker;
		CallBanker.wCallBanker=m_wCurrentUser;
		CallBanker.bFirstTimes=false;

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}
	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
	//״̬Ч��
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

	//���Ч��
	if(m_cbPlayStatus[wChairID]==USEX_PLAYING)
	{
		ASSERT(lScore>0 && lScore<=m_lTurnMaxScore[wChairID]);
		if (lScore<=0 || lScore>m_lTurnMaxScore[wChairID]) return false;
	}
	else //û��ע���ǿ��
	{
		ASSERT(lScore==0);
		if (lScore!=0) return false;
	}

	if(lScore>0L)
	{
		//��ע���
		m_lTableScore[wChairID]=lScore;

		//��������
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.lAddScoreCount=m_lTableScore[wChairID];

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	}

	//��ע����
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_lTableScore[i]>0L && m_cbPlayStatus[i]==USEX_PLAYING)bUserCount++;
		else if(m_cbPlayStatus[i]!=USEX_PLAYING || i==m_wBankerUser)bUserCount++;
	}

	//�м�ȫ��
	if(bUserCount==m_wPlayerCount)
	{
		//����״̬
		m_pITableFrame->SetGameStatus(GS_TK_PLAYING);

		//��������
		CMD_S_SendCard SendCard;
		ZeroMemory(SendCard.cbCardData,sizeof(SendCard.cbCardData));

		//��ȡ�˿�
		BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
		m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;

			//�ɷ��˿�
			CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
		}
		
		//�����˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;

			//�ɷ��˿�
			CopyMemory(SendCard.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
		}

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	}

	return true;
}

//̯���¼�
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE bOx)
{
	//״̬Ч��
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING) return true;
	if (m_bOxCard[wChairID]!=0xff) return true;

	//Ч������
	ASSERT(bOx<=TRUE);
	if(bOx>TRUE)return false;

	//Ч������
	if(bOx)
	{
		ASSERT(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0);
		if(!(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0))return false;
	}
	else if(m_cbPlayStatus[wChairID]==USEX_PLAYING)
	{
		if(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>=OX_THREE_SAME)bOx=TRUE;
	}

	//ţţ����
	m_bOxCard[wChairID] = bOx;

	//̯������
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_bOxCard[i]<2 && m_cbPlayStatus[i]==USEX_PLAYING)bUserCount++;
		else if(m_cbPlayStatus[i]!=USEX_PLAYING)bUserCount++;
	}

	 //�������
	CMD_S_Open_Card OpenCard;
	ZeroMemory(&OpenCard,sizeof(OpenCard));

	//���ñ���
	OpenCard.bOpen=bOx;
	OpenCard.wPlayerID=wChairID;

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	

	//������Ϸ
	if(bUserCount == m_wPlayerCount)
	{
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//�˿˷���
void CTableFrameSink::AnalyseCard()
{
	//��������
	bool bIsAiBanker = false;
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
		//��ȡ�û�
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

	//ȫ������
	if(wPlayerCount == wAiCount || wAiCount==0)return;

	//�˿˱���
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//ţţ����
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]!=USEX_PLAYING)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	//�����˿�
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//��������
	LONGLONG lAndroidScore=0;

	//��������
	BYTE cbCardTimes[GAME_PLAYER];
	ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

	//���ұ���
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==USEX_PLAYING)
		{
			cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
		}
	}

	//����ׯ��
	if(bIsAiBanker)
	{
		//�Ա��˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//�û�����
			if ((i==m_wBankerUser)||(m_cbPlayStatus[i]!=USEX_PLAYING)) continue;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//��������
			if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser())) continue;

			//�Ա��˿�
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
	else//�û�ׯ��
	{
		//�Ա��˿�
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//�û�����
			if ((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser())) continue;

			//�Ա��˿�
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

	//����ж�
	if ((m_lStockScore+lAndroidScore)<0)
	{
		//��������
		WORD wWinUser=INVALID_CHAIR;

		//�ж����
		bool bCardMax = (bIsAiBanker)?true:false;

		//��������
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//�û�����
			if (m_cbPlayStatus[i]!=USEX_PLAYING) continue;

			//�����û�
			if (wWinUser==INVALID_CHAIR)
			{
				wWinUser=i;
				continue;
			}

			//�Ա��˿�
			if (m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wWinUser],MAX_COUNT,bUserOxData[i],bUserOxData[wWinUser])==bCardMax)
			{
				wWinUser=i;
			}
		}

		//��������
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);
		CopyMemory(m_cbHandCardData[m_wBankerUser],m_cbHandCardData[wWinUser],MAX_COUNT);
		CopyMemory(m_cbHandCardData[wWinUser],cbTempData,MAX_COUNT);
	}

	return;
}

//��Ϸ״̬
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==USEX_PLAYING);
	if(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==USEX_PLAYING)return true;
	return false;
}


//////////////////////////////////////////////////////////////////////////
