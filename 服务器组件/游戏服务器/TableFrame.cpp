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

//���߶���
#define IDI_OFF_LINE				(TIME_TABLE_SINK_RANGE+1)			//���߱�ʶ
#define MAX_OFF_LINE				3									//���ߴ���
#define TIME_OFF_LINE				60000L								//����ʱ��

//��ʼ��ʱ
#define IDI_START_OVERTIME		(TIME_TABLE_SINK_RANGE+2)			    //��ʼ��ʱ
#define IDI_START_OVERTIME_END	(TIME_TABLE_SINK_RANGE+22)			    //��ʼ��ʱ
#ifndef _DEBUG
#define TIME_OVERTIME				30000L								//��ʱʱ��
#else
#define TIME_OVERTIME               5000L                               //��ʱʱ��
#endif

//////////////////////////////////////////////////////////////////////////////////

//��Ϸ��¼
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;
CGameCardLibraryArray				CTableFrame::m_GameCardLibraryBuffer;

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrame::CTableFrame()
{
	//��������
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;

	//��־����
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	m_bTableInitFinish=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//��Ϸ����
	m_lCellScore=0L;
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wDrawCount=0;

	//ʱ�����
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//��̬����
	m_dwTableOwnerID=0L;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//���߱���
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//������Ϣ
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//����ӿ�
	m_pITimerEngine=NULL;
	m_pITableFrameSink=NULL;
	m_pIMainServiceFrame=NULL;
	m_pIAndroidUserManager=NULL;

	//���Žӿ�
	m_pITableUserAction=NULL;
	m_pITableUserRequest=NULL;
	m_pIMatchTableAction=NULL;

	//���ݽӿ�
	m_pIKernelDataBaseEngine=NULL;
	m_pIRecordDataBaseEngine=NULL;

	//�����ӿ�
	m_pITableFrameHook=NULL;

	//�û�����
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	SafeRelease(m_pITableFrameSink);
	SafeRelease(m_pITableFrameHook);

	//�û�����
	for (INT_PTR i = 0; i < m_GameScoreRecordBuffer.GetCount(); i++)
	{
		//��ȡ����
		ASSERT(m_GameScoreRecordBuffer[i] != NULL);
		tagGameScoreRecord * pGameScoreRecord = m_GameScoreRecordBuffer[i];
		SafeDelete(pGameScoreRecord);
	}
	m_GameScoreRecordBuffer.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CTableFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrame,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��Ϸ
bool CTableFrame::StartGame()
{
	//��Ϸ״̬
	ASSERT(m_bDrawStarted==false);
	if (m_bDrawStarted==true) return false;

	//�������
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//����״̬
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//��ʼ����
	if (bGameStarted==false)
	{
		//״̬����
		ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
		ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem!=NULL)
			{
				//������Ϸ��
				if (m_pGameServiceOption->lServiceScore>0L)
				{
					m_lFrozenedScore[i]=m_pGameServiceOption->lServiceScore;
					pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
				}

				//����״̬
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus != US_OFFLINE) && (cbUserStatus != US_PLAYING))
				{
					pIServerUserItem->SetUserStatus(US_PLAYING, m_wTableID, i);
				}
			}
		}

		//����״̬
		if (bTableStarted != m_bTableStarted)
		{
			SendTableStatus();
		}
	}

	//����֪ͨ
	bool bStart=true;
	if (m_pITableFrameHook != NULL)
	{
		bStart = m_pITableFrameHook->OnEventGameStart(this, m_wChairCount);
	}

	//֪ͨ�¼�
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink != NULL&&bStart)
	{
		m_pITableFrameSink->OnEventGameStart();
	}

	return true;
}

//��ɢ��Ϸ
bool CTableFrame::DismissGame()
{
	//״̬�ж�
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//������Ϸ
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//����״̬
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		SendTableStatus();
	}

	return true;
}

//������Ϸ
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//Ч��״̬
	ASSERT(m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//�������
	bool bDrawStarted=m_bDrawStarted;

	//����״̬
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	//��Ϸ��¼
	RecordGameScore(bDrawStarted);
	
	//��������
	if (m_bGameStarted==false)
	{
		//��������
		bool bOffLineWait=false;

		//�����û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//tagTimeInfo* TimeInfo=pIServerUserItem->GetTimeInfo();
				////��Ϸʱ��
				//DWORD dwCurrentTime=(DWORD)time(NULL);
				//TimeInfo->dwEndGameTimer=dwCurrentTime;

				//������Ϸ��
				if (m_lFrozenedScore[i]!=0L)
				{
					pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[i]);
					m_lFrozenedScore[i]=0L;
				}

				//����״̬
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//���ߴ���;
					bOffLineWait=true;
					if(m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
					{
						PerformStandUpAction(pIServerUserItem);

						// ˽�˳����û�ԭ״̬;
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
					//����״̬
					pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);					
				}
			}
		}

		//ɾ��ʱ��
		if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);
	}

	//֪ͨ����
	if (m_pITableFrameHook != NULL)
	{
		m_pITableFrameHook->OnEventGameEnd(this, 0, NULL, cbGameStatus);
	}

	//��������
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink != NULL)
	{
		m_pITableFrameSink->RepositionSink();
	}

	//�߳����
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			if (m_TableUserItemArray[i]==NULL) continue;
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];

			bool isBenefitInfo = false;

			//�ȼý�
			//if (pIServerUserItem->IsAndroidUser() == false)
			//{
			//	SCORE lAllScore = pIServerUserItem->GetUserScore() + pIServerUserItem->GetTrusteeScore() + pIServerUserItem->GetUserInsure();

			//	// С�����Ʒ��;ȼý�, ��ȡ��������
			//	if ((lAllScore < m_pBenefitInfo->lGoldLimit) 
			//		&& (pIServerUserItem->GetBenefitTimes() < m_pBenefitInfo->cbReceiveTimes))
			//	{
			//		// ��ȡʱ��������
			//		SYSTEMTIME SystemTime;
			//		GetLocalTime(&SystemTime);
			//		
			//		COleDateTime dTimeF(SystemTime);
			//		COleDateTime dTimeS(pIServerUserItem->GetLastBenefitTime());
			//		
			//		double nSecnonSpan = 0;
			//		if (dTimeS.m_status == COleDateTime::valid)	//��Чʱ��ֵ
			//		{
			//			COleDateTimeSpan dTimeSpan = dTimeF - dTimeS;
			//			nSecnonSpan = dTimeSpan.GetTotalSeconds();
			//		}
			//		else
			//		{
			//			nSecnonSpan = m_pBenefitInfo->wTimeIntervals + 1;	//��Чʱ��ֵ�����Է��žȼý�
			//		}
			//		
			//		// ����ȡ�ȼý�
			//		if (nSecnonSpan > m_pBenefitInfo->wTimeIntervals)
			//		{
			//			//֪ͨ���ݿ���ȡ
			//			SendBenefitInfo(pIServerUserItem, SystemTime);

			//			//������Ϣ
			//			SendGameMessage(pIServerUserItem, m_pBenefitInfo->szPromptMsg, SMT_EJECT);

			//			isBenefitInfo = true;
			//		}
			//	}
			//}

			//��������
			//if ((m_pGameServiceOption->lMinTableScore!=0L)&&(pIServerUserItem->GetUserScore()<m_pGameServiceOption->lMinTableScore))
			//SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore * m_pGameServiceOption->iExchangeRatio;
			SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore;
			if ((lMinTableScore != 0L) && (pIServerUserItem->GetUserScore()<lMinTableScore))
			{
				// ������ȡ�ȼý�ʱ�򣬲���ʾ���ֲ���
				if (isBenefitInfo == false)
				{
					//������ʾ
					TCHAR szDescribe[128] = TEXT("");
					if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("�����ν������ ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), lMinTableScore);
					}
					else
					{
						_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������� ") SCORE_STRING TEXT("�����ܼ�����Ϸ��"), lMinTableScore);
					}

					//������Ϣ
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

				//�û�����
				PerformStandUpAction(pIServerUserItem);

				if (pIServerUserItem->IsAndroidUser() == false)
				{
					// ��Ҳ�������ֱ���Թۣ�����ʾ�����ʾ��ֵ����
					PerformLookonAction(wOldChairID, pIServerUserItem);
				}
				continue;
			}

			//�ر��ж�
			if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("����ϵͳά������ǰ��Ϸ���ӽ�ֹ�û�������Ϸ��");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CLOSE_GAME);

				//�û�����
				PerformStandUpAction(pIServerUserItem);
				continue;
			}

			//��������
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//��������
				if (pIServerUserItem->IsAndroidUser()==true)
				{
					//����Ϣ
					CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;
					tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIServerUserItem->GetBindIndex());

					//���һ���
					IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIServerUserItem->GetUserID(),pBindParameter->dwSocketID);
					if(pIAndroidUserItem==NULL) continue;

					//�뿪�ж�
					if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE))
					{
						//��������
						PerformStandUpAction(pIServerUserItem);

						continue;
					}

					//�����ж�
					if(pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITSTANDUP))
					{
						//��������
						PerformStandUpAction(pIServerUserItem);

						continue;
					}												
				}
			}

			////������ʱ
			//if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
			//{
			//	SetGameTimer(IDI_START_OVERTIME + i, TIME_OVERTIME, 1, i);
			//}
		}		
	}

	//��������
	ConcludeTable();

	//����״̬
	SendTableStatus();

	return true;
}

//��������
bool CTableFrame::ConcludeTable()
{
	//��������
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//�����ж�
		WORD wTableUserCount=GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//ģʽ�ж�
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
	}

	return true;
}

//д�����
bool CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo, DWORD dwGameMemal, DWORD dwPlayGameTime)
{
	//Ч�����
	ASSERT((wChairID<m_wChairCount)&&(ScoreInfo.cbType!=SCORE_TYPE_NULL));
	if ((wChairID>=m_wChairCount)&&(ScoreInfo.cbType==SCORE_TYPE_NULL)) return false;

	//��ȡ�û�
	ASSERT(GetTableUserItem(wChairID)!=NULL);
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
	TCHAR szMessage[128]=TEXT("");

	//д�����
	if (pIServerUserItem!=NULL)
	{
		//��������
		DWORD dwUserMemal=0L;
		SCORE lRevenueScore=__min(m_lFrozenedScore[wChairID],m_pGameServiceOption->lServiceScore);

		//�۷����
		if (m_pGameServiceOption->lServiceScore>0L 
			&& m_pGameServiceOption->wServerType == GAME_GENRE_GOLD
			&& m_pITableFrameSink->QueryBuckleServiceCharge(wChairID))
		{
			//�۷����
			ScoreInfo.lScore-=lRevenueScore;
			ScoreInfo.lRevenue+=lRevenueScore;

			//������Ϸ��
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//���Ƽ���
		if(dwGameMemal != INVALID_DWORD)
		{
			dwUserMemal = dwGameMemal;
		}
		else if (ScoreInfo.lRevenue>0L)
		{
			dwUserMemal=(DWORD)(ScoreInfo.lRevenue*0.01f/1000L);
		}

		//��Ϸʱ��
		DWORD dwCurrentTime=(DWORD)time(NULL);
		DWORD dwPlayTimeCount=((m_bDrawStarted==true)?(dwCurrentTime-m_dwDrawStartTime):0L);
		if (dwPlayGameTime != INVALID_DWORD)
		{
			dwPlayTimeCount = dwPlayGameTime;
		}

		//��������
		//tagUserProperty * pUserProperty=pIServerUserItem->GetUserProperty();
		//
		////�����ж�
		//if(m_pGameServiceOption->wServerType == GAME_GENRE_SCORE)
		//{
		//	if (ScoreInfo.lScore>0L)
		//	{
		//		//�ı�����
		//		if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_FOURE_SCORE)!=0)
		//		{
		//			//��������
		//			DWORD dwValidTime=pUserProperty->PropertyInfo[1].wPropertyCount*pUserProperty->PropertyInfo[1].dwValidNum;
		//			if(pUserProperty->PropertyInfo[1].dwEffectTime+dwValidTime>dwCurrentTime)
		//			{
		//				//���ַ���
		//				ScoreInfo.lScore *= 4;
		//				_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ �ı����ֿ� ]���÷ַ��ı���)"),pIServerUserItem->GetNickName());
		//			}
		//			else
		//			{
		//				pUserProperty->wPropertyUseMark&=~PT_USE_MARK_FOURE_SCORE;
		//			}
		//		} //˫������
		//		else if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_DOUBLE_SCORE)!=0)
		//		{
		//			//��������
		//			DWORD dwValidTime=pUserProperty->PropertyInfo[0].wPropertyCount*pUserProperty->PropertyInfo[0].dwValidNum;
		//			if (pUserProperty->PropertyInfo[0].dwEffectTime+dwValidTime>dwCurrentTime)
		//			{
		//				//���ַ���
		//				ScoreInfo.lScore*=2L;
		//				_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ˫�����ֿ� ]���÷ַ�����"), pIServerUserItem->GetNickName());
		//			}
		//			else
		//			{
		//				pUserProperty->wPropertyUseMark&=~PT_USE_MARK_DOUBLE_SCORE;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		//�����
		//		if ((pUserProperty->wPropertyUseMark&PT_USE_MARK_POSSESS)!=0)
		//		{
		//			//��������
		//			DWORD dwValidTime=pUserProperty->PropertyInfo[3].wPropertyCount*pUserProperty->PropertyInfo[3].dwValidNum;
		//			if(pUserProperty->PropertyInfo[3].dwEffectTime+dwValidTime>dwCurrentTime)
		//			{
		//				//���ַ���
		//				ScoreInfo.lScore = 0;
		//				_sntprintf_s(szMessage,CountArray(szMessage),TEXT("[ %s ] ʹ����[ ������� ]�����ֲ��䣡"),pIServerUserItem->GetNickName());
		//			}
		//			else
		//			{
		//				pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_POSSESS;
		//			}
		//		}
		//	}
		//}

		//д�����
		DWORD dwWinExperience=(ScoreInfo.cbType==SCORE_TYPE_WIN)?m_pGameServiceOption->dwWinExperience:m_pGameServiceOption->dwFailExperience;
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore,ScoreInfo.lGrade,ScoreInfo.lRevenue,dwUserMemal,ScoreInfo.cbType,dwPlayTimeCount,dwWinExperience);

		//��Ϸ��¼
		if (pIServerUserItem->IsAndroidUser()==false && CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//��������
			tagGameScoreRecord * pGameScoreRecord=NULL;

			//��ѯ���
			if (m_GameScoreRecordBuffer.GetCount()>0L)
			{
				//��ȡ����
				INT_PTR nCount=m_GameScoreRecordBuffer.GetCount();
				pGameScoreRecord=m_GameScoreRecordBuffer[nCount-1];

				//ɾ������
				m_GameScoreRecordBuffer.RemoveAt(nCount-1);
			}

			//��������
			if (pGameScoreRecord==NULL)
			{
				try
				{
					//��������
					pGameScoreRecord=new tagGameScoreRecord;
					if (pGameScoreRecord==NULL) throw TEXT("��Ϸ��¼���󴴽�ʧ��");
				}
				catch (...)
				{
					ASSERT(FALSE);
				}
			}

			//��¼����
			if (pGameScoreRecord!=NULL)
			{
				//�û���Ϣ
				pGameScoreRecord->wChairID=wChairID;
				pGameScoreRecord->dwUserID=pIServerUserItem->GetUserID();
				pGameScoreRecord->cbAndroid=(pIServerUserItem->IsAndroidUser()?TRUE:FALSE);

				//�û���Ϣ
				pGameScoreRecord->dwDBQuestID=pIServerUserItem->GetDBQuestID();
				pGameScoreRecord->dwInoutIndex=pIServerUserItem->GetInoutIndex();

				//�ɼ���Ϣ
				pGameScoreRecord->lScore=ScoreInfo.lScore;
				pGameScoreRecord->lGrade=ScoreInfo.lGrade;
				pGameScoreRecord->lRevenue=ScoreInfo.lRevenue;

				//������Ϣ
				pGameScoreRecord->dwUserMemal=dwUserMemal;
				pGameScoreRecord->dwPlayTimeCount=dwPlayTimeCount;

				//��������˰
				if(pIServerUserItem->IsAndroidUser())
				{
					pGameScoreRecord->lScore += pGameScoreRecord->lRevenue;
					pGameScoreRecord->lRevenue = 0L;
				}

				//��������
				m_GameScoreRecordActive.Add(pGameScoreRecord);
			}
		}

		//��Ϸ��¼
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
		//�뿪�û�
		CTraceService::TraceString(TEXT("ϵͳ��ʱδ֧���뿪�û��Ĳ��ֲ�������"),TraceLevel_Exception);

		return false;
	}

	//�㲥��Ϣ
	if (szMessage[0]!=0)
	{
		//��������
		IServerUserItem * pISendUserItem = NULL;
		WORD wEnumIndex=0;

		//��Ϸ���
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			pISendUserItem=GetTableUserItem(i);
			if(pISendUserItem==NULL) continue;

			//������Ϣ
			SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
		}

		//�Թ��û�
		do
		{
			pISendUserItem=EnumLookonUserItem(wEnumIndex++);
			if(pISendUserItem!=NULL) 
			{
				//������Ϣ
				SendGameMessage(pISendUserItem, szMessage, SMT_CHAT);
			}
		} while (pISendUserItem!=NULL);
	}

	return true;
}

//д�����
bool CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData)
{
	//Ч�����
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//д�����
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

//����˰��
SCORE CTableFrame::CalculateRevenue(WORD wChairID, SCORE lScore)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return 0L;

	//����˰��
	if ((m_pGameServiceOption->wRevenueRatio>0)&&(lScore>=REVENUE_BENCHMARK))
	{
		//��ȡ�û�
		ASSERT(GetTableUserItem(wChairID)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);

		//����˰��
		SCORE lRevenue=lScore*m_pGameServiceOption->wRevenueRatio/REVENUE_DENOMINATOR;

		return lRevenue;
	}

	return 0L;
}

//�����޶�
SCORE CTableFrame::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	//�û�Ч��
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//��ѯ���
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lMinEnterScore=m_pGameServiceOption->lMinTableScore;
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//Ч����
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)||(lUserConsumeQuota>pIServerUserItem->GetUserScore()-lMinEnterScore)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//Ѱ���û�
IServerUserItem * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem != NULL) && (pIServerUserItem->GetUserID() == dwUserID))
		{
			return pIServerUserItem;
		}
	}

	//�Թ��û�
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

//��Ϸ�û�
IServerUserItem * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//��ȡ�û�
	return m_TableUserItemArray[wChairID];
}

//�Թ��û�
IServerUserItem * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//����ʱ��
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine != NULL)
	{
		m_pITimerEngine->SetTimer(dwEngineTimerID + dwTimerID, dwElapse, dwRepeat, dwBindParameter);
	}

	return true;
}

//ɾ��ʱ��
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine != NULL)
	{
		m_pITimerEngine->KillTimer(dwEngineTimerID + dwTimerID);
	}

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//��������
			m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		//��������
		m_pIMainServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIMainServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//������Ϣ
		m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	m_pIMainServiceFrame->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//��Ϸ��Ϣ
bool CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	return m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//���ͳ���
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//���ͳ���
	ASSERT(m_pIMainServiceFrame!=NULL);
	m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//���ýӿ�
bool CTableFrame::SetTableFrameHook(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	if(pIUnknownEx==NULL) return false;

	//�����ж�
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH | GAME_GENRE_PRIVATE)) == 0) return false;

	//��ѯ�ӿ�
	m_pITableFrameHook=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameHook);
	m_pIMatchTableAction=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableUserAction);

	return true;
}

//�����¼�
bool CTableFrame::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//����Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	IServerUserItem * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//��Ϸ�û�
	if (cbUserStatus!=US_LOOKON)
	{
		//Ч���û�
		ASSERT(pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//˽������;
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
		{
			pIServerUserItem->SetReUserStatus(pIServerUserItem->GetUserStatus());
			pIServerUserItem->SetUserStatus(US_OFFLINE, m_wTableID, wChairID);
			//����֪ͨ;
			if (m_pIMatchTableAction != NULL) m_pIMatchTableAction->OnActionUserOffLine(wChairID, pIServerUserItem);

			return true;
		}

		//���ߴ���
		//if ((cbUserStatus==US_PLAYING)&&(m_wOffLineCount[wChairID]<MAX_OFF_LINE))
		if (cbUserStatus == US_PLAYING)
		{
			//�û�����
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//��������
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				if(pIServerUserItem->IsTrusteeUser()==false)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(true);

					//����֪ͨ
					if (m_pITableUserAction != NULL)
					{
						m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
					}
				}

				return true;
			}

			//����֪ͨ
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			}
			
			////���ߴ���
			//if (m_dwOffLineTime[wChairID]==0L)
			//{
			//	//���ñ���
			//	m_wOffLineCount[wChairID]++;
			//	m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

			//	//ʱ������
			//	WORD wOffLineCount=GetOffLineUserCount();
			//	if (wOffLineCount == 1)
			//	{
			//		SetGameTimer(IDI_OFF_LINE, TIME_OFF_LINE, 1, wChairID);
			//	}
			//}

			return true;
		}
	}

	//�û�����
	PerformStandUpAction(pIServerUserItem,true);
	
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		if (pIServerUserItem->IsTrusteeUser() == true)
		{
			return true;
		}
	}

	//ɾ���û�
	ASSERT(pIServerUserItem->GetUserStatus()==US_FREE);
	pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//�����¼�
bool CTableFrame::OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//֪ͨ��Ϸ
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
}

//ʱ���¼�
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//�¼�����
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:	//�����¼�
		{
			//Ч��״̬
			ASSERT(m_bGameStarted==true);
			if (m_bGameStarted==false) return false;

			//��������
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//Ѱ���û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//λ���ж�
			ASSERT(wOffLineChair!=INVALID_CHAIR);
			if (wOffLineChair==INVALID_CHAIR) return false;

			//�û��ж�
			ASSERT(dwBindParameter<m_wChairCount);
			if (wOffLineChair!=(WORD)dwBindParameter)
			{
				//ʱ�����
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//����ʱ��
				dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE-2000L);
				SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE-dwLapseTime,1,wOffLineChair);

				return true;
			}

			//��ȡ�û�
			ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
			IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

			//������Ϸ
			if (pIServerUserItem!=NULL)
			{
				//���ñ���
				m_dwOffLineTime[wOffLineChair]=0L;

				//�û�����
				PerformStandUpAction(pIServerUserItem);
			}

			//����ʱ��
			if (m_bGameStarted==true)
			{
				//��������
				DWORD dwOffLineTime=0L;
				WORD wOffLineChair=INVALID_CHAIR;

				//Ѱ���û�
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
					{
						wOffLineChair=i;
						dwOffLineTime=m_dwOffLineTime[i];
					}
				}

				//����ʱ��
				if (wOffLineChair!=INVALID_CHAIR)
				{
					//ʱ�����
					DWORD dwCurrentTime=(DWORD)time(NULL);
					DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

					//����ʱ��
					dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE-2000L);
					SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE-dwLapseTime,1,wOffLineChair);
				}
			}

			return true;
		}
	}

	//δ��ʼ��ʱ
	if(dwTimerID >= IDI_START_OVERTIME && dwTimerID <=IDI_START_OVERTIME_END)
	{
		//��������
		WORD wChair=(WORD)dwBindParameter;

		//
		if(wChair != (WORD)(dwTimerID-IDI_START_OVERTIME)) return false;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(wChair);

		//��ʱ����
		if(pIServerUserItem && pIServerUserItem->GetUserStatus()==US_SIT)
		{
			//�û�����
			PerformStandUpAction(pIServerUserItem);
		}

		return false;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//��Ϸ�¼�
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//����¼�
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//��Ϸ����
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//��Ϸ����
		{
			//Ч�����
 			ASSERT(wDataSize==sizeof(CMD_GF_GameOption));
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//��������
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//��������
			if ((cbUserStatus!=US_LOOKON)&&((m_dwOffLineTime[wChairID]!=0L)))
			{
				//���ñ���
				m_dwOffLineTime[wChairID]=0L;

				//ɾ��ʱ��
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==0) KillGameTimer(IDI_OFF_LINE);
			}

			//����״̬
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus != US_LOOKON)
			{
				m_bAllowLookon[wChairID] = pGameOption->cbAllowLookon ? true : false;
			}

			//����״̬
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;
			m_pIMainServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			////������Ϣ
			//TCHAR szMessage[128]=TEXT("");
			//_sntprintf_s(szMessage,CountArray(szMessage),TEXT("��ӭ�����롰%s����Ϸ��ף����Ϸ��죡"),m_pGameServiceAttrib->szGameName);
			//m_pIMainServiceFrame->SendGameMessage(pIServerUserItem,szMessage,SMT_CHAT);

			//���ͳ���
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);



			//ȡ���й�
			if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				if(pIServerUserItem->IsTrusteeUser()==true)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(false);
				}

				////�ع�֪ͨ;
				//if(m_pITableFrameHook) m_pITableFrameHook->OnUserBackMatch(this,pIServerUserItem);
			}

			//�ع�֪ͨ;
			if (m_pITableFrameHook != NULL) m_pITableFrameHook->OnEventUserReturnMatch(pIServerUserItem);

			//��ʼ�ж�;
			if ((cbUserStatus == US_READY) && (EfficacyStartGame(wChairID) == true))
			{
				StartGame();
			}

			return true;
		}
	case SUB_GF_USER_READY:		//�û�׼��
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//Ч��״̬
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//�����׷����ж�
			if((CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
				&& (m_wDrawCount >= m_pGameServiceOption->wDistributeDrawCount || CheckDistribute()))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("ϵͳ���·������ӣ����Ժ�");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT);

				//������Ϣ
				m_pIMainServiceFrame->InsertDistribute(pIServerUserItem);

				////�û�����
				//PerformStandUpAction(pIServerUserItem);

				return true;
			}

			//�¼�֪ͨ
			if(m_pIMatchTableAction!=NULL && !m_pIMatchTableAction->OnActionUserOnReady(wChairID,pIServerUserItem, pData,wDataSize))
			{
				return true;
			}

			//�¼�֪ͨ
			if (m_pITableUserAction!=NULL)
			{
				m_pITableUserAction->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);

				// ��Ҵ�������

			}

			//ɾ����ʱ
			if(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) KillGameTimer(IDI_START_OVERTIME+wChairID);

			//��ʼ�ж�
			if (EfficacyStartGame(wChairID)==false)
			{
				//����״̬
				pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);
			}
			else
			{
				StartGame(); 
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//�û�����
		{
			//��������
			CMD_GF_C_UserChat * pUserChat=(CMD_GF_C_UserChat *)pData;

			//Ч�����
			ASSERT(wDataSize<=sizeof(CMD_GF_C_UserChat));
			ASSERT(wDataSize>=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)));
			//ASSERT(wDataSize==(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));
			ASSERT(wDataSize == (sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString) + CountStringBuffer(pUserChat->szChatString)));

			//Ч�����
			if (wDataSize>sizeof(CMD_GF_C_UserChat)) return false;
			if (wDataSize<(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
			//if (wDataSize!=(sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;
			if (wDataSize != (sizeof(CMD_GF_C_UserChat)-sizeof(pUserChat->szChatString) + CountStringBuffer(pUserChat->szChatString)))
			{
				return false;
			}

			//Ŀ���û�
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//������Ϣ
			CMD_GF_S_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			//�ַ�����
			m_pIMainServiceFrame->SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));

			//��������
			UserChat.dwChatColor=pUserChat->dwChatColor;
			UserChat.wChatLength=pUserChat->wChatLength;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			UserChat.dwSendUserID=pIServerUserItem->GetUserID();
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

			//��������
			WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
			WORD wSendSize=wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]);

			SendTableData(INVALID_CHAIR, SUB_GF_USER_CHAT, &UserChat, wSendSize, MDM_GF_FRAME);
			SendLookonData(INVALID_CHAIR, SUB_GF_USER_CHAT, &UserChat, wSendSize, MDM_GF_FRAME);

			return true;
		}
	case SUB_GF_USER_EXPRESSION:	//�û�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_C_UserExpression));
			if (wDataSize!=sizeof(CMD_GF_C_UserExpression)) return false;

			//��������
			CMD_GF_C_UserExpression * pUserExpression=(CMD_GF_C_UserExpression *)pData;

			//Ŀ���û�
			if ((pUserExpression->dwTargetUserID!=0)&&(SearchUserItem(pUserExpression->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//״̬�ж�
			if ((CServerRule::IsForfendGameChat(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ��Ϸ���죡"),SMT_CHAT);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem,TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
				return true;
			}

			try
			{
				if (pUserExpression->wPropertyID > 0)
				{
					//���ҵ�����Ϣ
					tagPropertyInfo * pPropertyInfo = CGamePropertyManager::GetInstance()->SearchPropertyItem(pUserExpression->wPropertyID);

					//��ЧЧ��
					if (pPropertyInfo == NULL)
					{
						SendGameMessage(pIServerUserItem, TEXT("��Ǹ����Ϸ�в�����ħ����������޷�ʹ�ã�"), SMT_EJECT | SMT_CHAT);
						return true;
					}

					SYSTEMTIME SystemTime;
					GetLocalTime(&SystemTime);

					CMD_GR_C_Property Property;
					ZeroMemory(&Property, sizeof(Property));
					Property.wPropertyIndex = pUserExpression->wPropertyID;
					Property.wItemCount = 0;

					DWORD dwPropDBID = 0;
					//�Ƿ��е���
					EN_PROPERTY_ERROR error = pIServerUserItem->UsedProperty(&Property, SystemTime, dwPropDBID);

					//����ʹ��ʧ��
					if (error != EN_PROPERTY_ERROR::EN_PROP_SUCC)
					{
						CString str;
						switch (error)
						{
						case EN_PROP_NULL:
						case EN_PROP_NUMBER:
							str.Format(_T("��Ǹ������û��ħ�����������ǰ���̳ǹ���"));
							break;
						case EN_PROP_OVER:
							str.Format(_T("��Ǹ������ӵ�е�ħ��������ѹ��ڣ���ǰ���̳ǹ���"));
							break;
						default:
							break;
						}

						SendGameMessage(pIServerUserItem, str, SMT_EJECT | SMT_CHAT);
						return true;
					}
				}

				//������Ϣ
				CMD_GR_S_UserExpression UserExpression;
				ZeroMemory(&UserExpression, sizeof(UserExpression));

				//��������
				UserExpression.wItemIndex = pUserExpression->wItemIndex;
				UserExpression.dwSendUserID = pIServerUserItem->GetUserID();
				UserExpression.dwTargetUserID = pUserExpression->dwTargetUserID;

				SendTableData(INVALID_CHAIR, SUB_GF_USER_EXPRESSION, &UserExpression, sizeof(UserExpression), MDM_GF_FRAME);
				SendLookonData(INVALID_CHAIR, SUB_GF_USER_EXPRESSION, &UserExpression, sizeof(UserExpression), MDM_GF_FRAME);
			}
			catch (...)
			{
				//������Ϣ
				CTraceService::TraceString(TEXT("�û�������鷢���쳣"), TraceLevel_Exception);
			}

			return true;

		}
	case SUB_GF_MAGIC_EXPRESSION:	//ħ������
		{
			//Ч�����
			ASSERT(wDataSize == sizeof(CMD_GF_C_MagicExpression));
			if (wDataSize != sizeof(CMD_GF_C_MagicExpression)) return false;

			//��������
			CMD_GF_C_MagicExpression * pUserExpression = (CMD_GF_C_MagicExpression *)pData;

			//Ŀ���û�
			if ((pUserExpression->wTargetChairID != INVALID_CHAIR) && (GetTableUserItem(pUserExpression->wTargetChairID) == NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			////Ȩ���ж�
			//if (CUserRight::CanRoomChat(pIServerUserItem->GetUserRight()) == false)
			//{
			//	SendGameMessage(pIServerUserItem, TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"), SMT_EJECT | SMT_CHAT);
			//	return true;
			//}

			//��������������
			if ((pIServerUserItem->GetChairID() == INVALID_CHAIR)||(pIServerUserItem->GetUserStatus() == US_LOOKON))
			{
				SendGameMessage(pIServerUserItem, TEXT("��Ǹ����������Ϸ�У��޷�����ħ�����飡"), SMT_EJECT | SMT_CHAT);
				return true;
			}

			try
			{
				//VIPȨ���ж�

				//�����ж�
				//SCORE lTrusteeScore = pIServerUserItem->GetTrusteeScore() * m_pGameServiceOption->iExchangeRatio;
				SCORE lTrusteeScore = pIServerUserItem->GetTrusteeScore();
				if (lTrusteeScore < m_pGameServiceOption->lMagicExpScore)
				{
					SendGameMessage(pIServerUserItem, TEXT("��Ǹ�����Ļ��ֲ��㣬�޷�����ħ�����飡"), SMT_EJECT | SMT_CHAT);
					return true;
				}

				//��������
				if (pIServerUserItem->WriteOtherScore(m_pGameServiceOption->lMagicExpScore, TASK_TYPE_MAGIC_EXP) == false)
				{
					SendGameMessage(pIServerUserItem, TEXT("��Ǹ�����Ļ��ֲ��㣬�޷�����ħ�����飡"), SMT_EJECT | SMT_CHAT);
					return true;
				}

				//������Ϣ
				CMD_GF_S_MagicExpression UserExpression;
				ZeroMemory(&UserExpression, sizeof(UserExpression));

				//��������
				UserExpression.wItemIndex = pUserExpression->wItemIndex;
				UserExpression.wSendChairID = pIServerUserItem->GetChairID();
				UserExpression.wTargetChairID = pUserExpression->wTargetChairID;
				SendTableData(INVALID_CHAIR, SUB_GF_MAGIC_EXPRESSION, &UserExpression, sizeof(UserExpression), MDM_GF_FRAME);

			}
			catch (...)
			{
				//������Ϣ
				CTraceService::TraceString(TEXT("ħ�����鷢���쳣"), TraceLevel_Exception);
			}

			return true;
		}
	case SUB_GF_PROPERTY_SEND:		//����ʹ��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_GR_C_Property));
		if (wDataSize != sizeof(CMD_GR_C_Property)) return false;

		//��������
		CMD_GR_C_Property * pProperty = (CMD_GR_C_Property *)pData;

		//����Ч��
		ASSERT(pProperty->wItemCount>0);
		if (pProperty->wItemCount == 0) return false;
		
		IServerUserItem * pTargetUesrItem = GetTableUserItem(pProperty->wTargetChairID);

		//Ŀ���û�
		if ((pProperty->wTargetChairID != INVALID_CHAIR) && (pTargetUesrItem == NULL))
		{
			ASSERT(FALSE);
			return true;
		}

		//�Լ���������������
		if ((pIServerUserItem->GetChairID() == INVALID_CHAIR) || (pIServerUserItem->GetUserStatus() == US_LOOKON))
		{
			SendGameMessage(pIServerUserItem, TEXT("��Ǹ����������Ϸ�У��޷������������ߣ�"), SMT_EJECT | SMT_CHAT);
			return true;
		}

		try
		{
			//���ҵ�����Ϣ
			tagPropertyInfo * pPropertyInfo = CGamePropertyManager::GetInstance()->SearchPropertyItem(pProperty->wPropertyIndex);

			//��ЧЧ��
			if (pPropertyInfo == NULL)
			{
				SendGameMessage(pIServerUserItem, TEXT("��Ǹ����Ϸ�в����ڸ��������ߣ��޷����ͣ�"), SMT_EJECT | SMT_CHAT);
				return true;
			}

			SYSTEMTIME SystemTime;
			GetLocalTime(&SystemTime);

			//�Ƿ��е���
			DWORD dwPropDBID = 0;
			EN_PROPERTY_ERROR error = pIServerUserItem->UsedProperty(pProperty, SystemTime, dwPropDBID);

			//����ʹ��ʧ��
			if (error != EN_PROPERTY_ERROR::EN_PROP_SUCC)
			{
				CString str;
				switch (error)
				{
				case EN_PROP_NULL:
					str.Format(_T("��Ǹ�����޸��������ߣ���ǰ���̳ǹ���"));
					break;
				case EN_PROP_NUMBER:
					str.Format(_T("��Ǹ������ӵ�и����������������㣬��ǰ���̳ǹ���"));
					break;
				case EN_PROP_OVER:
					str.Format(_T("��Ǹ������ӵ�и����������ѹ��ڣ���ǰ���̳ǹ���"));
					break;
				default:
					break;
				}

				SendGameMessage(pIServerUserItem, str, SMT_EJECT | SMT_CHAT);
				return true;
			}

			//�޸Ľ������������������
			pTargetUesrItem->ModifyUserProperty(0, pPropertyInfo->lRecvLoveLiness);

			//֪ͨ���ݿ����
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

			//������Ϣ
			CMD_GF_S_Property sProperty;
			ZeroMemory(&sProperty, sizeof(sProperty));

			//��������
			sProperty.wPropertyIndex = pProperty->wPropertyIndex;
			sProperty.wItemCount = pProperty->wItemCount;
			sProperty.lRecvLoveLiness = pPropertyInfo->lRecvLoveLiness;
			sProperty.wSendChairID = pIServerUserItem->GetChairID();
			sProperty.wTargetChairID = pProperty->wTargetChairID;
			SendTableData(INVALID_CHAIR, SUB_GF_PROPERTY_SUCC, &sProperty, sizeof(CMD_GF_S_Property), MDM_GF_FRAME);
		}
		catch (...)
		{
			//������Ϣ
			CTraceService::TraceString(TEXT("��������ʹ���쳣"), TraceLevel_Exception);
		}

		return true;
	}
	case SUB_GF_LOOKON_CONFIG:		//�Թ�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//��������
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//Ŀ���û�
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//������Ϣ
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//�����ж�
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//���ñ���
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					ASSERT(m_pIMainServiceFrame!=NULL);
					m_pIMainServiceFrame->SendData(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_GF_TAKE_GOLD:		// ������
		{
			//Ч�����
			ASSERT(wDataSize == sizeof(CMD_GF_TakeGold));
			if (wDataSize < sizeof(CMD_GF_TakeGold)) return false;

			//��������
			CMD_GF_TakeGold * pTakeGold = (CMD_GF_TakeGold *)pData;

			if (pTakeGold->lScore <= 0) return false;
			

			//��ȡ����
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT(GetTableUserItem(wChairID) == pIServerUserItem);
			if (GetTableUserItem(wChairID) != pIServerUserItem) return false;

			//Ч��״̬
			switch (cbUserStatus)
			{
			case US_LOOKON:		// ���������
			{
				return true;
			}
			case US_PLAYING:	// ��¼�������Ϸ���������
			{
				//�������
				SCORE lScore = pIServerUserItem->TakeGold(pTakeGold->lScore, false);
				return true;
			}
			case US_READY:		// ֱ�Ӵ���
			case US_SIT:
			{
 			    //�������
				SCORE lScore = pIServerUserItem->TakeGold(pTakeGold->lScore);
				
				//ʹ���û����²��������·�������������Ҵ�������Ϣ
				m_pITableUserAction->OnActionUserSitDown(wChairID, pIServerUserItem, false);

				//��������
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

//��Ϸ����
WORD CTableFrame::GetDrawCount()
{
	return m_wDrawCount;
}

//��ȡ��λ
WORD CTableFrame::GetNullChairID()
{
	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�����λ
WORD CTableFrame::GetRandNullChairID()
{
	//��������
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

//�û���Ŀ
WORD CTableFrame::GetSitUserCount()
{
	//��������
	WORD wUserCount=0;

	//��Ŀͳ��
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//�Թ���Ŀ
WORD CTableFrame::GetLookonUserCount()
{
	//��ȡ��Ŀ
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//������Ŀ
WORD CTableFrame::GetOffLineUserCount()
{
	//��������
	WORD wOffLineCount=0;

	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L)
		{
			wOffLineCount++;
		}
	}

	return wOffLineCount;
}

//����״��
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//���ñ���
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//׼���ж�
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//������Ŀ
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//����׼��
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_PAIR_READY:		//��Կ�ʼ
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//ʱ�����
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//Ĭ��ģʽ
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//��������
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//���ñ���
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//���ò���
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//����ӿ�
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIMainServiceFrame=TableFrameParameter.pIMainServiceFrame;
	m_pIAndroidUserManager=TableFrameParameter.pIAndroidUserManager;
	m_pIKernelDataBaseEngine=TableFrameParameter.pIKernelDataBaseEngine;
	m_pIRecordDataBaseEngine=TableFrameParameter.pIRecordDataBaseEngine;
	m_pBenefitInfo = TableFrameParameter.pBenefitInfo;

	//��������
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//�����ж�
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}
	
	//���ñ���
	m_lCellScore=m_pGameServiceOption->lCellScore;

	//��������
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	//���ñ�ʶ
	m_bTableInitFinish=true;

	//��չ�ӿ�
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pITableUserRequest=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);

	return true;
}

//��������
bool CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative/*=false*/)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);

	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	// �Թ�������ȴ���
	if (cbUserStatus == US_LOOKON)
	{
		//��������
		for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
		{
			if (pIServerUserItem == m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

				return true;
			}
		}

		return true;
	}

	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//��Ϸ�û�
	if ((m_bGameStarted==true)&&((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)))
	{
		//��������
		if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
		{
			//�����й�
			pIServerUserItem->SetTrusteeUser(true);

			//����֪ͨ
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			}

			return true;
		}

		//������Ϸ
		BYTE cbConcludeReason=(cbUserStatus==US_OFFLINE)?GER_NETWORK_ERROR:GER_USER_LEAVE;
		m_pITableFrameSink->OnEventGameConclude(wChairID,pIServerUserItem,cbConcludeReason);

		//�뿪�ж�
		if (m_TableUserItemArray[wChairID]!=pIServerUserItem) return true;
	}

	if ((m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) && (bInitiative==true))
	{
		if (pIServerUserItem->IsTrusteeUser()==false)
		{
			pIServerUserItem->SetTrusteeUser(true);

			//����֪ͨ
			if (m_pITableUserAction != NULL)
			{
				m_pITableUserAction->OnActionUserOffLine(wChairID, pIServerUserItem);
			}
		}	
		return true;
	}

	//���ñ��� ��������Ϸ���վ��
	if (pIServerUserItem==pITableUserItem)
	{
		//ɾ����ʱ
		if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
		{
			KillGameTimer(IDI_START_OVERTIME + wChairID);
		}

		//���ñ���
		m_TableUserItemArray[wChairID]=NULL;

		//������Ϸ��
		if (m_lFrozenedScore[wChairID]!=0L)
		{
			pIServerUserItem->UnFrozenedUserScore(m_lFrozenedScore[wChairID]);
			m_lFrozenedScore[wChairID]=0L;
		}

		//�¼�֪ͨ
		if (m_pITableUserAction!=NULL)
		{
			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,false);
		}

		//�¼�֪ͨ
		if (m_pIMatchTableAction != NULL)
		{
			m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, false);
		}

#ifdef _DEBUG
		if (pIServerUserItem->IsAndroidUser())
		{
			CString strMsg;
			strMsg.Format("������ID:%d,�ǳ�:%s,վ������:%d,����:%d", pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
			//��ʾ��Ϣ
			CTraceService::TraceString(strMsg, TraceLevel_Normal);
		}
#endif

		//�û�״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//��������
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//���ñ���
		m_wUserCount=wTableUserCount;

		//������Ϣ
		if (wTableUserCount==0)
		{
			m_dwTableOwnerID=0L;
			m_szEnterPassword[0]=0;
		}

		//�����Թ�
		//if (wTableUserCount==0)
		//{
		//	for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		//	{
		//		SendGameMessage(m_LookonUserItemArray[i],TEXT("����Ϸ������������Ѿ��뿪�ˣ�"),SMT_CLOSE_GAME|SMT_EJECT);
		//	}
		//}

		//��������
		ConcludeTable();

		//��ʼ�ж�
		if (EfficacyStartGame(INVALID_CHAIR)==true)
		{
			StartGame();
		}

		//����״̬
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted()))
		{
			SendTableStatus();
		}

		//�����׷����ж�
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
		{
			//�����׷����ж�
			if(IsTableStarted()==false && CheckDistribute())
			{
				for(int i=0; i<m_wChairCount; i++)
				{
					if(m_TableUserItemArray[i] != NULL && m_TableUserItemArray[i]->GetUserStatus()==US_READY)
					{
						//������Ϣ
						LPCTSTR pszMessage=TEXT("������뿪��ϵͳ���·������ӣ����Ժ�");
						SendGameMessage(m_TableUserItemArray[i],pszMessage,SMT_CHAT);

						//������Ϣ
						m_pIMainServiceFrame->InsertDistribute(m_TableUserItemArray[i]);

						////�û�����
						//PerformStandUpAction(m_TableUserItemArray[i]);
					}
				}
			}
		}

		return true;
	}
	else
	{
		////��������
		//for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		//{
		//	if (pIServerUserItem==m_LookonUserItemArray[i])
		//	{
		//		//ɾ������
		//		m_LookonUserItemArray.RemoveAt(i);

		//		//�¼�֪ͨ
		//		if (m_pITableUserAction!=NULL)
		//		{
		//			m_pITableUserAction->OnActionUserStandUp(wChairID,pIServerUserItem,true);
		//		}

		//		//�¼�֪ͨ
		//		if (m_pIMatchTableAction != NULL)
		//		{
		//			m_pIMatchTableAction->OnActionUserStandUp(wChairID, pIServerUserItem, true);
		//		}

		//		//�û�״̬
		//		pIServerUserItem->SetClientReady(false);
		//		pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//		return true;
		//	}
		//}

		//�������
		ASSERT(FALSE);
	}

	return true;
}

bool CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	//ASSERT((pIServerUserItem != NULL) && (wChairID < m_wChairCount));
	ASSERT(pIServerUserItem != NULL);
	ASSERT((pIServerUserItem->GetTableID() == INVALID_TABLE) && (pIServerUserItem->GetChairID() == INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule = pIServerUserItem->GetUserRule();

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser() == false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink = (CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem = m_TableUserItemArray[i];
			if (pIUserItem == NULL) continue;
			if (pIUserItem->IsAndroidUser() == false)break;

			//��ȡ����
			tagBindParameter * pBindParameter = pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem = m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(), pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter = pIAndroidUserItem->GetAndroidParameter();

			//ģ���ж�
			if ((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE) != 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY) == 0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE) == 0)
			{
				//SendRequestFailure(pIServerUserItem, TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û��Թۣ�"), REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_TABLE);
				return false;
			}

			break;
		}
	}


	//�Թ��ж�
	if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule) == true
		&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) == false))
	{
		//if ((pITableUserItem != NULL) && (pITableUserItem->IsAndroidUser() == true))
		{
			//SendRequestFailure(pIServerUserItem, TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"), REQUEST_FAILURE_NORMAL);
			SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_ROOM);
			return false;
		}
	}

	//״̬�ж�
	if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule) == true) && (pIServerUserItem->GetMasterOrder() == 0))
	{
		//SendRequestFailure(pIServerUserItem, TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"), REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_ROOM);
		return false;
	}

	//����Ч��
	if ((IsTableLocked() == true) && (pIServerUserItem->GetMasterOrder() == 0L) && (lstrcmp(pUserRule->szPassword, m_szEnterPassword) != 0))
	{
		//SendRequestFailure(pIServerUserItem, TEXT("��Ϸ���������벻��ȷ�������Թ���Ϸ��"), REQUEST_FAILURE_PASSWORD);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_PASSW);
		return false;
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	//pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetClientReady(true);
	pIServerUserItem->SetUserStatus(US_LOOKON, m_wTableID, wChairID);

	//�¼�֪ͨ
	if (m_pITableUserAction != NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID, pIServerUserItem, true);
	}

	//�¼�֪ͨ
	if (m_pIMatchTableAction != NULL)
	{
		m_pIMatchTableAction->OnActionUserSitDown(wChairID, pIServerUserItem, true);
	}

	return true;
}

//���¶���
bool CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//���ֱ���
	//SCORE lUserScore=pIServerUserItem->GetUserScore();
	//SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	//SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	//״̬�ж�
	if ((CServerRule::IsForfendGameEnter(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	{
		//SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_ENTER);
		return false;
	}

	//ģ�⴦��
	if (m_pGameServiceAttrib->wChairCount < MAX_CHAIR && pIServerUserItem->IsAndroidUser()==false)
	{
		//�������
		CAttemperEngineSink * pAttemperEngineSink=(CAttemperEngineSink *)m_pIMainServiceFrame;

		//���һ���
		for (WORD i=0; i<m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem=m_TableUserItemArray[i];
			if(pIUserItem==NULL) continue;
			if(pIUserItem->IsAndroidUser()==false)break;

			//��ȡ����
			tagBindParameter * pBindParameter=pAttemperEngineSink->GetBindParameter(pIUserItem->GetBindIndex());
			IAndroidUserItem * pIAndroidUserItem=m_pIAndroidUserManager->SearchAndroidUserItem(pIUserItem->GetUserID(),pBindParameter->dwSocketID);
			tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

			//ģ���ж�
			if((pAndroidParameter->dwServiceMode&ANDROID_SIMULATE)!=0
				&& (pAndroidParameter->dwServiceMode&ANDROID_PASSIVITY)==0
				&& (pAndroidParameter->dwServiceMode&ANDROID_INITIATIVE)==0)
			{
				//SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ���ӽ�ֹ�û����룡"),REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_ENTER);
				return false;
			}

			break;
		}
	}

	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//��Ϸ״̬
	if ((m_bGameStarted==true)&&(bDynamicJoin==false))
	{
		//SendRequestFailure(pIServerUserItem,TEXT("��Ϸ�Ѿ���ʼ�ˣ����ڲ��ܽ�����Ϸ����"),REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_ENTER_START);
		return false;
	}

	//�����ж�
	if (pITableUserItem!=NULL)
	{
		//������
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return false;

		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"),pITableUserItem->GetNickName());

		//������Ϣ
		//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

		return false;
	}

	//����Ч��
	if(m_wChairCount <= MAX_CHAIR_GENERAL)
	{
		//������
		if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule) == false)
		{
			if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L))
			&&((lpszPassword==NULL)||(lstrcmp(lpszPassword,m_szEnterPassword)!=0)))
			{
				//SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�����ܼ�����Ϸ��"),REQUEST_FAILURE_PASSWORD);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LOOKON_PASSW);
				return false;
			}
		}
	}

	//����Ч��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyEnterTableScoreRule(wChairID,pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyIPAddress(pIServerUserItem)==false) return false;
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && EfficacyScoreRule(pIServerUserItem)==false) return false;

	//��չЧ��
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0 && m_pITableUserRequest!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableUserRequest->OnUserRequestSitDown(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//������Ϣ
			//SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);
			SendRequestFailure(pIServerUserItem, RequestResult.cbFailureCode, EN_MOBILE_CODE::EN_MC_NULL, RequestResult.szFailureReason);

			return false;
		}
	}

#ifdef _DEBUG
	if (pIServerUserItem->IsAndroidUser())
	{
		CString strMsg;
		strMsg.Format("������ID:%d,�ǳ�:%s,��������:%d,����:%d", pIServerUserItem->GetUserID(), pIServerUserItem->GetNickName(), pIServerUserItem->GetTableID(), pIServerUserItem->GetChairID());
		//��ʾ��Ϣ
		CTraceService::TraceString(strMsg, TraceLevel_Normal);
	}
#endif

	//���ñ���
	m_TableUserItemArray[wChairID]=pIServerUserItem;
	m_wDrawCount=0;

	//�û�״̬
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
		//���ñ���
		m_wOffLineCount[wChairID]=0L;
		m_dwOffLineTime[wChairID]=0L;

		//������Ϸ��
		if (m_pGameServiceOption->lServiceScore>0L)
		{
			m_lFrozenedScore[wChairID]=m_pGameServiceOption->lServiceScore;
			pIServerUserItem->FrozenedUserScore(m_pGameServiceOption->lServiceScore);
		}

		//����״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);
	}

	//���ñ���
	m_wUserCount=GetSitUserCount();

	//������Ϣ
	if (GetSitUserCount()==1)
	{
		//״̬����
		bool bTableLocked=IsTableLocked();

		//���ñ���
		m_dwTableOwnerID=pIServerUserItem->GetUserID();
		lstrcpyn(m_szEnterPassword,pUserRule->szPassword,CountArray(m_szEnterPassword));

		//����״̬
		if (bTableLocked != IsTableLocked())
		{
			SendTableStatus();
		}
	}

	//������ʱ
	//if (IsGameStarted() == false && pIServerUserItem->IsMobileUser() && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	//{
	//	SetGameTimer(IDI_START_OVERTIME + wChairID, TIME_OVERTIME, 1, wChairID);
	//}

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserSitDown(wChairID,pIServerUserItem,false);
	}

	//�¼�֪ͨ
	if (m_pIMatchTableAction != NULL)
	{
		m_pIMatchTableAction->OnActionUserSitDown(wChairID, pIServerUserItem, false);
	}

	// ������£�������ֻ���ң���������������ҷ��͸��Լ�
	if ( m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	{
		for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem = m_TableUserItemArray[i];
			if (pIUserItem == NULL) continue;
			if (pIUserItem->GetUserID() == pIServerUserItem->GetUserID()) continue;

			if (m_pIMainServiceFrame!=NULL)
			{
				m_pIMainServiceFrame->SendUserInfo(pIUserItem, pIServerUserItem->GetBindIndex());
			}
		}
	}

	// ������£����Լ����͸������ϵ������ֻ����
	for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem *pISerUserItem = m_TableUserItemArray[i];
		if (pISerUserItem == NULL) continue;
		if (pISerUserItem->GetUserID() == pIServerUserItem->GetUserID()) continue;

		if (m_pIMainServiceFrame != NULL)
		{
			m_pIMainServiceFrame->SendUserInfo(pIServerUserItem, pISerUserItem->GetBindIndex());
		}
	}

	// ������£����Լ����͸��ֻ��Թ����
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
	//	//��ʼ�ж�
	//	if (EfficacyStartGame(INVALID_CHAIR)==true)
	//	{
	//		StartGame(); 
	//	}
	//}

	return true;
}

//�ָ��Թ�
bool CTableFrame::PerformRecoerLookOn(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if (wChairID == INVALID_CHAIR || pIServerUserItem == nullptr)
	{
		return false;
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	pIServerUserItem->SetClientReady(true);
	pIServerUserItem->SetUserStatus(US_LOOKON, m_wTableID, wChairID);

	return true;
}

//��Ԫ����
VOID CTableFrame::SetCellScore(LONG lCellScore)
{
	m_lCellScore=lCellScore;
	if (m_bTableInitFinish==true)
	{
		SendTableStatus();
		m_pITableFrameSink->SetGameBaseScore(lCellScore);
	}
}

//����״̬
bool CTableFrame::SendTableStatus()
{
	//��������
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus,sizeof(TableStatus));

	//��������
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.lCellScore=m_lCellScore;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//��������
	m_pIMainServiceFrame->SendData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//�ֻ�����

	return true;
}

//�ֻ�����ʧ��
bool CTableFrame::SendRequestFailure(IServerUserItem * pIServerUserItem, BYTE cbErrType, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure, sizeof(RequestFailure));

	//��������
	RequestFailure.cbErrType = cbErrType;
	RequestFailure.lErrorCode = lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString, pszDescribe, CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize = CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize = sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	m_pIMainServiceFrame->SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_REQUEST_FAILURE, &RequestFailure, wHeadSize + wDataSize);

	return true;
}

//��ʼЧ��
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	//״̬�ж�
	if (m_bGameStarted==true) return false;

	//ģʽ����
	if (m_cbStartMode==START_MODE_TIME_CONTROL) return false;
	if (m_cbStartMode==START_MODE_MASTER_CONTROL) return false;

	//׼������
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			//״̬�ж�
			if (pITableUserItem->IsClientReady() == false)
			{
				return false;
			}
			if ((wReadyChairID != i) && (pITableUserItem->GetUserStatus() != US_READY))
			{
				return false;
			}

			//�û�����
			wReadyUserCount++;
		}
	}

	//��ʼ����
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//����׼��
		{
			//��Ŀ�ж�
			if (wReadyUserCount>=2L) return true;

			return false;
		}
	case START_MODE_FULL_READY:			//���˿�ʼ
		{
			//�����ж�
			if (wReadyUserCount==m_wChairCount) return true;

			return false;
		}
	case START_MODE_PAIR_READY:			//��Կ�ʼ
		{
			//��Ŀ�ж�
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//λ���ж�
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//��ȡ�û�
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//λ�ù���
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

//��ַЧ��
bool CTableFrame::EfficacyIPAddress(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//���˲�����
	if(m_wChairCount > MAX_CHAIR_GENERAL) return true;

	//��ַЧ��
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
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

	//��ַЧ��
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientAddr();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
			{
				if (!pUserRule->bLimitSameIP)
				{
					//������Ϣ
					//LPCTSTR pszDescribe=TEXT("����Ϸ����������˲�����ͬ IP ��ַ�������Ϸ���� IP ��ַ�����ҵ� IP ��ַ��ͬ�����ܼ�����Ϸ��");
					//SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_IP_SAME);
					return false;
				}
				else
				{
					//������Ϣ
					/*LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
					SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);*/
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_YOU_IP_SAME);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			//��ȡ�û�
			IServerUserItem * pITableUserItem=GetTableUserItem(i);
			if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					//��ȡ�û�
					IServerUserItem * pITableNextUserItem=GetTableUserItem(j);
					if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
					{
						//LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ������ IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
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

//����Ч��
bool CTableFrame::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)) return true;

	//���˲�����
	if(m_wChairCount > MAX_CHAIR_GENERAL) return true;

	//��������
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//���ַ�Χ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
			//��ȡ����
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("����������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				//������Ϣ
				//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

				return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
				//������Ϣ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("����ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

				//������Ϣ
				//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
				SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

				return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitGameScore==true)
			{
				//��߻���
				if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					//������Ϣ
					//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

					return false;
				}

				//��ͻ���
				if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
				{
					//������Ϣ
					TCHAR szDescribe[128]=TEXT("");
					_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

					//������Ϣ
					//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
					SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

					return false;
				}
			}
		}
	}

	return true;
}

//����Ч��
bool CTableFrame::EfficacyEnterTableScoreRule(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//���ֱ���
	//SCORE lUserScore = pIServerUserItem->GetUserScore() + pIServerUserItem->GetTrusteeScore()*m_pGameServiceOption->iExchangeRatio;
	//SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore*m_pGameServiceOption->iExchangeRatio;
	SCORE lUserScore = pIServerUserItem->GetUserScore() + pIServerUserItem->GetTrusteeScore();
	SCORE lMinTableScore = m_pGameServiceOption->lMinTableScore;
	SCORE lLessEnterScore=m_pITableFrameSink->QueryLessEnterScore(wChairID,pIServerUserItem);

	if (((lMinTableScore!=0L)&&(lUserScore<lMinTableScore))||((lLessEnterScore!=0L)&&(lUserScore<lLessEnterScore)))
	{
		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		if (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ�ң�������Ϸ�Ҳ��������ܼ��룡"), __max(lMinTableScore, lLessEnterScore));
		}
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" �ı����ң����ı����Ҳ��������ܼ��룡"), __max(lMinTableScore, lLessEnterScore));
		}
		else
		{
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������Ϸ������Ҫ ") SCORE_STRING TEXT(" ����Ϸ���֣����Ļ��ֲ��������ܼ��룡"), __max(lMinTableScore, lLessEnterScore));
		}

		//������Ϣ
		//SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NOSCORE);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NOSCORE, EN_MOBILE_CODE::EN_MC_NULL, szDescribe);

		return false;
	}

	return true;
}

//������
bool CTableFrame::CheckDistribute()
{
	//������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
	{
		//����״��
		tagTableUserInfo TableUserInfo;
		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

		//�û�����
		if(wUserSitCount < TableUserInfo.wMinUserCount)
		{
			return true;
		}
	}

	return false;
}

//��Ϸ��¼
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	if (bDrawStarted==true)
	{
		//д���¼
		if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//��������
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//���ñ���
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//��Ϸʱ��
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

			//�û�����
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//��ȡ����
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//�û���Ŀ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//����ͳ��
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//ͳ����Ϣ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//�ɼ���Ϣ
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//Ͷ������
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//�����¼
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

//���;ȼý�
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
//����˽�˳���Ϣ
void CTableFrame::SetPrivateInfo(tagPrivateFrameParameter &PriavateFrame)
{
	CopyMemory(&m_PrivateFramePrarameter, &PriavateFrame, sizeof(tagPrivateFrameParameter));
}

//��ȡ˽�˳�����
tagPrivateFrameParameter* CTableFrame::GetPrivateFrameInfo()
{
	return &m_PrivateFramePrarameter;
}

//��ȡ��ǰ�������
tagPrivateFrameRecordInfo* CTableFrame::GetPrivateFrameRecord()
{
	return &m_PrivateFrameRecord;
}

//ͳ��˽�˳���Ҿ���
VOID  CTableFrame::addPrivatePlayCout(WORD wCout)
{
	m_PrivateFrameRecord.cbFinishCout += wCout;

	if (m_pITableFrameHook)
	{
		m_pITableFrameHook->addPrivatePlayCout(this, wCout);
	}
}

//����˽�˳�������Ϣ;
void CTableFrame::ResetPrivateEndInfo()
{
	ZeroMemory(&m_PrivateFrameRecord, sizeof(m_PrivateFrameRecord));

	if (m_pITableFrameSink)
	{
		m_pITableFrameSink->ResetPrivateEndInfo();
	}
}

//���˽�˳�������Ϣ��
void CTableFrame::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{
	if (m_pITableFrameSink)
	{
		m_pITableFrameSink->GetPrivateEndInfo(kDataStream, bSend);
	}
}

//�ж�˽�˳��Ƿ����
bool CTableFrame::IsPrivateEnd()
{
	// �жϷ�����ά��;
	if (CServerRule::IsForfendGameStart(m_pGameServiceOption->dwServerRule) == true)
	{
		return true;
	}

	// ��Ϸ�Ƿ����;
	if (m_pITableFrameSink)
	{
		return m_pITableFrameSink->IsPrivateEnd();
	}

	return false;
}


//��ȡ�ƿ�
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

//�����ƿ�
void CTableFrame::AddGameCardLibrary(tagGameCardLibrary* pGameCard)
{
	m_GameCardLibraryBuffer.Add(pGameCard);
}

//�޸��ƿ�
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

//ɾ���ƿ�
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
