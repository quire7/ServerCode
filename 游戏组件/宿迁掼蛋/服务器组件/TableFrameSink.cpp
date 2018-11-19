#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////
//��̬����;
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//��Ϸ����;

//////////////////////////////////////////////////////////////////////////
//�궨��
#define IDI_GAEM_START					1				//��Ϸ��ʼ��ʱ��;
#define IDI_TRIBUTE						2				//������ʱ��;
#define	IDI_BACK_TRIBUTE				3				//�ع���ʱ��;
#define IDI_OUT_CARD					4				//����,pass��ʱ��;
#define IDI_RESPONSE_QUIT				5				//�ظ������˳�;


//////////////////////////////////////////////////////////////////////////
#define FLEE							5				//���ܿ۳�5����ע;
//////////////////////////////////////////////////////////////////////////

//���캯��;
CTableFrameSink::CTableFrameSink()
{
	//������ʼ;
	m_wFriendFirst = 0;
	ZeroMemory(m_cbUserTrustee, sizeof(m_cbUserTrustee));
	ZeroMemory(m_bUserOffline, sizeof(m_bUserOffline));
	ZeroMemory(&m_cbTributeCard, sizeof(m_cbTributeCard));

	//��Ϸ����;
	m_wBankerUser =	INVALID_CHAIR;
	m_wLastOutUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;

	//������Ϣ;
	m_cbTurnCardCount = 0;
	m_wTurnWiner = INVALID_CHAIR;
	m_cbTurnCardType = CT_ERROR;
	m_stSeries.cbCurrentSeries = 2;
	m_stSeries.cbOtherSeries = 2;
	m_stSeries.cbOurSeries = 2;

    m_cbMark = 0;
	m_cbGameStatus = 0;
	m_bGameEnd = true;
	m_wTributeBigID = INVALID_CHAIR;
	ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData, sizeof(m_cbMagicCardData));
	ZeroMemory(m_cbRanking, sizeof(m_cbRanking));
	ZeroMemory(m_cbUserCurRank, sizeof(m_cbUserCurRank));
	ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
	ZeroMemory(m_bTributeStatus, sizeof(m_bTributeStatus));
	ZeroMemory(m_cbRequestTimes, sizeof(m_cbRequestTimes));
	m_lstRequestUserID.clear();
	ZeroMemory(m_cbResponseStatus, sizeof(m_cbResponseStatus));
	m_bHaveSendRequestQuit = false;


	//�˿���Ϣ;
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

	//�������;
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;

	m_lBaseScore = 0L;

	srand(GetTickCount()+reinterpret_cast<DWORD_PTR>(this));
	return;
}

//��������;
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ;
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��;
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�;
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

	//�����ж�;
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ ITableFrame �ӿ�ʧ��"), TraceLevel_Exception);
		return false;
	}

	//��ʼģʽ;
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//��ȡ����;
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//�Զ�����;
	ASSERT(m_pITableFrame->GetCustomRule() != NULL);
	m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();

	return true;
}

//��λ����;
VOID CTableFrameSink::RepositionSink()
{	
	//��Ϸ����;
	m_wLastOutUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	
	//������Ϣ;
	m_cbTurnCardCount = 0;
	m_wTurnWiner = INVALID_CHAIR;
	m_cbTurnCardType = CT_ERROR;
	m_cbMark = 0;
	ZeroMemory(m_cbRequestTimes, sizeof(m_cbRequestTimes));
	m_lstRequestUserID.clear();
	ZeroMemory(m_cbResponseStatus, sizeof(m_cbResponseStatus));
	m_bHaveSendRequestQuit = false;

	//�˿���Ϣ;
	ZeroMemory(m_cbTurnCardData,	sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData, sizeof(m_cbMagicCardData));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbUserCurRank, sizeof(m_cbUserCurRank));

}

//��Ϸ��ʼ;
bool CTableFrameSink::OnEventGameStart()
{
	//��������;
	m_wLastOutUser = INVALID_CHAIR;
	m_wTributeBigID = INVALID_CHAIR;
	ZeroMemory(m_cbTributeCard, sizeof(m_cbTributeCard));
	ZeroMemory(m_cbUserTrustee, sizeof(m_cbUserTrustee));
	ZeroMemory(m_bTributeStatus, sizeof(m_bTributeStatus));
	ZeroMemory(m_cbUserCurRank, sizeof(m_cbUserCurRank));
	ZeroMemory(m_cbRequestTimes, sizeof(m_cbRequestTimes));
	m_lstRequestUserID.clear();
	ZeroMemory(m_cbResponseStatus, sizeof(m_cbResponseStatus));
	m_bHaveSendRequestQuit = false;
	m_cbPayTributeStatus = 0;

	//������Ϸ״̬;
	m_pITableFrame->SetGameStatus(GS_WK_SEND_CARD);
	
	m_bGameEnd = false;
	m_cbMark	= 0;


	BYTE cbRandCard[FULL_COUNT] = { 0 };
	m_GameLogic.RandCardList(cbRandCard, CountArray(cbRandCard));

	//�ַ��˿�;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		// ���ûع�����;
		m_aryRetTributeTarget[i] = 255;
		// ������������;
		m_cbHandCardCount[i] = 27;
		CopyMemory(&m_cbHandCardData[i], &cbRandCard[i*27], sizeof(BYTE)*27);
	}

	//���ü���;
	if (m_stSeries.cbCurrentSeries < 14)
	{
		m_GameLogic.SetMainValue(m_stSeries.cbCurrentSeries);
	}
	else
	{
		m_GameLogic.SetMainValue(0x01);
	}

	CMD_S_Send_Card  SendCard;
	SendCard.stSeries = m_stSeries;

	//�����˿���Ϣ;
	for (WORD i=0; i<GAME_PLAYER; ++i)
	{
		CopyMemory(SendCard.cbCardData, m_cbHandCardData[i], 27);
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	//���ÿ�ʼ��ʱ��;
	m_pITableFrame->SetGameTimer(IDI_GAEM_START, m_pGameCustomRule->cbTimeStartGame * 1000, 1, 0);

	return true;
}

//��Ϸ����;
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//ɾ����ʱ��;
	m_pITableFrame->KillGameTimer(IDI_GAEM_START);
	m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
	m_pITableFrame->KillGameTimer(IDI_BACK_TRIBUTE);
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
	m_pITableFrame->KillGameTimer(IDI_RESPONSE_QUIT);

	switch(cbReason)
	{
		//������Ϸ;
	case GER_NORMAL:
	{
		//��������;
		m_bGameEnd = true;

		//�������;
		CMD_S_GameEnd stGameEnd;
		ZeroMemory(&stGameEnd, sizeof(stGameEnd));
		stGameEnd.bTongGuan = false;
		stGameEnd.cbGameEndType = GER_NORMAL;
		//ʣ���˿�;
		BYTE bCardPos = 0;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			stGameEnd.cbCardCount[i] = m_cbHandCardCount[i];
			CopyMemory(&stGameEnd.cbCardData[bCardPos], m_cbHandCardData[i], m_cbHandCardCount[i] * sizeof(BYTE));
			bCardPos += m_cbHandCardCount[i];
		}
		//��������;
		BYTE cbUpgrading = 0;
		bool bHavePlayerMoneyEnough = false;
		BYTE cbFirst = m_cbRanking[0], cbSecond = (m_cbRanking[0] + 1) % GAME_PLAYER;
		//״̬�޸�;
		m_cbPayTribute[cbFirst] = 1;
		//ͳ�ƻ���;
		LONG lScore = 0L;
		//˫�´���;
		if (m_cbRanking[0] == ((m_cbRanking[1] + 2) % GAME_PLAYER))
		{
			m_cbGameStatus = SHUANG_XIA;
			m_cbPayTribute[(cbFirst + 2) % GAME_PLAYER] = 1;
			m_cbPayTribute[cbSecond] = 2;
			m_cbPayTribute[(cbSecond + 2) % GAME_PLAYER] = 2;
			m_cbRanking[2] = (cbFirst + 1) % GAME_PLAYER;
			m_cbRanking[3] = (cbFirst + 3) % GAME_PLAYER;
			lScore = 3;
			cbUpgrading = 3;
		}
		//���´���;
		else if (m_cbRanking[0] == ((m_cbRanking[2] + 2) % GAME_PLAYER))
		{
			m_cbGameStatus = DAN_XIA;
			m_cbPayTribute[m_cbRanking[2]] = 0;
			m_cbPayTribute[m_cbRanking[1]] = 0;
			m_cbPayTribute[(m_cbRanking[1] + 2) % GAME_PLAYER] = 2;
			m_cbRanking[3] = (m_cbRanking[1] + 2) % GAME_PLAYER;
			lScore = 2;
			cbUpgrading = 2;
		}
		else
		{
			m_cbGameStatus = OTHER;
			m_cbPayTribute[m_cbRanking[1]] = 0;
			m_cbPayTribute[m_cbRanking[2]] = 0;
			m_cbPayTribute[(m_cbRanking[0] + 2) % m_wPlayerCount] = 2;
			m_cbRanking[3] = (m_cbRanking[0] + 2) % GAME_PLAYER;
			lScore = 1;
			cbUpgrading = 1;
		}

		if (m_stSeries.cbCurrentSeries == 14 && m_cbGameStatus != OTHER)
		{
			if ((m_stSeries.cbOurSeries == 14 && cbFirst % 2 == 0) || (m_stSeries.cbOtherSeries == 14 && cbFirst % 2 == 1))
			{
				m_cbGameStatus = 0;
				stGameEnd.bTongGuan = true;
				ZeroMemory(m_cbRanking, sizeof(m_cbRanking));
				ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			}
		}

		if (stGameEnd.bTongGuan)
		{
			m_stSeries.cbOtherSeries = 2;
			m_stSeries.cbCurrentSeries = 2;
			m_stSeries.cbOurSeries = 2;
			m_wBankerUser = INVALID_CHAIR;
		}
		else
		{
			if ((cbFirst % 2) == 0)
			{
				m_stSeries.cbOurSeries += cbUpgrading;
				if (m_stSeries.cbOurSeries >= 14)
				{
					m_stSeries.cbOurSeries = 14;
				}
				m_stSeries.cbCurrentSeries = m_stSeries.cbOurSeries;
			}
			else
			{
				m_stSeries.cbOtherSeries += cbUpgrading;
				if (m_stSeries.cbOtherSeries >= 14)
				{
					m_stSeries.cbOtherSeries = 14;
				}
				m_stSeries.cbCurrentSeries = m_stSeries.cbOtherSeries;
			}
			m_wBankerUser = m_cbRanking[0];
		}

		//�����ȼ�;
		CopyMemory(&stGameEnd.stSeries, &m_stSeries, sizeof(m_stSeries));
		CopyMemory(stGameEnd.cbRanking, m_cbRanking, sizeof(m_cbRanking));

		//����׷�;
		LONG lCellScore = m_pITableFrame->GetCellScore();
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(cbSecond);
		SCORE lUserScore = pServerUserItem->GetUserScore();
		stGameEnd.lGameScore[cbSecond] = 0L - min(lScore*lCellScore, lUserScore);
		pServerUserItem = m_pITableFrame->GetTableUserItem((cbSecond + 2) % GAME_PLAYER);
		lUserScore = pServerUserItem->GetUserScore();
		stGameEnd.lGameScore[(cbSecond + 2) % GAME_PLAYER] = 0L - min(lScore*lCellScore, lUserScore);
		stGameEnd.lGameScore[cbFirst] = abs(stGameEnd.lGameScore[cbSecond] + stGameEnd.lGameScore[(cbSecond + 2) % GAME_PLAYER]) / 2;
		stGameEnd.lGameScore[(cbFirst + 2) % GAME_PLAYER] = stGameEnd.lGameScore[cbFirst];

		//д��;
		tagScoreInfo aryScoreInfo[GAME_PLAYER];
		ZeroMemory(aryScoreInfo, sizeof(aryScoreInfo));
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			LONG lRevenue = 0;
			if (stGameEnd.lGameScore[i] > 0)
			{
				aryScoreInfo[i].cbType = SCORE_TYPE_WIN;
				aryScoreInfo[i].lRevenue = m_pITableFrame->CalculateRevenue(i, stGameEnd.lGameScore[i]);
				stGameEnd.lGameScore[i] -= lRevenue;
			}
			else if (stGameEnd.lGameScore[i] == 0)
			{
				aryScoreInfo[i].cbType = SCORE_TYPE_DRAW;
			}
			else
			{
				aryScoreInfo[i].cbType = SCORE_TYPE_LOSE;
			}
			aryScoreInfo[i].lScore = stGameEnd.lGameScore[i];
		}
		m_pITableFrame->WriteTableScore(aryScoreInfo, CountArray(aryScoreInfo));

		//�ж���ҽ���Ƿ�������õĽ�ҵ�ע;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			SCORE lUserScore = pServerUserItem->GetUserScore();
			//���������ϵĽ�ҿ۳��󣬲�����;
			if (lUserScore < m_lBaseScore*MAX_RATE)
			{
				bHavePlayerMoneyEnough = true;
				break;
			}
		}

		if (bHavePlayerMoneyEnough)
		{
			ZeroMemory(m_cbRanking, sizeof(m_cbRanking));
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			m_stSeries.cbOtherSeries = 2;
			m_stSeries.cbCurrentSeries = 2;
			m_stSeries.cbOurSeries = 2;
			m_wBankerUser = INVALID_CHAIR;
		}

		//������Ϣ;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &stGameEnd, sizeof(stGameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &stGameEnd, sizeof(stGameEnd));

		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		return true;
	}
	//��Ϸ��ɢ;
	case GER_DISMISS:
		{
			m_bGameEnd = true;

			//��������;
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.cbGameEndType = GER_DISMISS;

			//ʣ���˿�;
			BYTE bCardPos = 0;
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				GameEnd.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[bCardPos], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));
				bCardPos += m_cbHandCardCount[i];
			}
			//������Ϣ;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			//������Ϸ;
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			//������Ϸ״̬;
			m_pITableFrame->SetGameStatus(GS_WK_FREE);
			//��������;
			m_wBankerUser = INVALID_CHAIR;
			m_stSeries.cbCurrentSeries = 2;
			m_stSeries.cbOtherSeries = 2;
			m_stSeries.cbOurSeries = 2;
			m_cbGameStatus = 0;
			ZeroMemory(m_cbRanking, sizeof(m_cbRanking));
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			return true;
		}
	//�û�ǿ��;
	case GER_USER_LEAVE:
	case GER_NETWORK_ERROR:
		{
			//�޸ı���;
			m_bGameEnd = true;

			//�������;
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.cbGameEndType = GER_USER_LEAVE;

			CopyMemory(&GameEnd.stSeries, &m_stSeries, sizeof(m_stSeries));
			CopyMemory(&GameEnd.cbRanking, &m_cbRanking, sizeof(m_cbRanking));
			//ʣ���˿�;
			BYTE bCardPos = 0;
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				GameEnd.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[bCardPos], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));
				bCardPos += m_cbHandCardCount[i];
			}
			//����׷�;
			LONG lCellScore = m_pITableFrame->GetCellScore();
			//��ҵ÷�;
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );
			SCORE lUserScore = pServerUserItem->GetUserScore();	
			GameEnd.lGameScore[wChairID] = 0L - abs(__min(FLEE*lCellScore, lUserScore));

			//������Ϣ;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo, sizeof(tagScoreInfo));
			ScoreInfo.cbType = SCORE_TYPE_FLEE;
			ScoreInfo.lScore = GameEnd.lGameScore[wChairID];

			//�޸Ļ���;
			m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

			//��Ϸ����;
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			//��������;
			m_wBankerUser = INVALID_CHAIR;
			m_stSeries.cbCurrentSeries = 2;
			m_stSeries.cbOtherSeries = 2;
			m_stSeries.cbOurSeries = 2;
			m_cbGameStatus = 0;
			ZeroMemory(m_cbRanking, sizeof(m_cbRanking));
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			return true;
		}
	}
	return false;
}

//���ͳ���;
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	//Ч����;
	//ASSERT(FALSE);
    switch(cbGameStatus)
	{
	//����״̬;
	case GS_WK_FREE:
		{
			//�������;
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			StatusFree.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			StatusFree.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			StatusFree.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusFree.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			m_lBaseScore = m_pITableFrame->GetCellScore();

			StatusFree.lBaseScore = m_lBaseScore;

			//���ͳ���;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	//����״̬;
	case GS_WK_SEND_CARD:
		{
			CDM_S_StatusSendCard SendCard;
			SendCard.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			SendCard.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			SendCard.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			SendCard.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			SendCard.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			SendCard.lBaseScore = m_lBaseScore;
			SendCard.stSeries = m_stSeries;
			SendCard.bWin = m_cbRanking[0]%2 == 0;

			for(WORD i=0; i<m_wPlayerCount; i++)
			{
				SendCard.cbHandCardCount[i] = 27;
			}
			CopyMemory(SendCard.cbHandCardData, m_cbHandCardData[wChairID], sizeof(BYTE) * 27);
			//���ͳ���;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &SendCard, sizeof(SendCard));
		}
	//����״̬;
	case GS_WK_TRIBUTE:
		{
			//�������;
			CMD_S_StatusTribute Tribute;
			ZeroMemory(&Tribute, sizeof(Tribute));

			Tribute.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			Tribute.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			Tribute.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			Tribute.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			Tribute.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			//���ñ���;
			Tribute.lBaseScore = m_lBaseScore;
			Tribute.stSeries = m_stSeries;
			Tribute.bLastRoundWin = m_cbRanking[0]%2 == 0;

			CopyMemory(Tribute.cbCardData, m_cbHandCardData[wChairID], sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(Tribute.cbCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
			CopyMemory(Tribute.cbUserTrustee, m_cbUserTrustee, sizeof(m_cbUserTrustee));
			//���ͳ���;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &Tribute, sizeof(Tribute));
		}	
	//��Ϸ״̬;
	case GS_WK_PLAYING:		
		{
			//��������;
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			StatusPlay.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			StatusPlay.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			StatusPlay.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusPlay.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			//���ñ���;
			StatusPlay.lBaseScore = m_lBaseScore;
			StatusPlay.wLastOutUser = m_wTurnWiner;
			StatusPlay.wCurrentUser = m_wCurrentUser;
			StatusPlay.cbTurnCardCount = m_cbTurnCardCount;
			StatusPlay.stSeries = m_stSeries;
			StatusPlay.bWin	= m_cbRanking[0]%2 == 0;

			CopyMemory(StatusPlay.cbCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
			CopyMemory(StatusPlay.cbTurnCardData, m_cbTurnCardData, sizeof(BYTE)*m_cbTurnCardCount);
			CopyMemory(StatusPlay.cbCardData, m_cbHandCardData[wChairID], sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(StatusPlay.cbRanking, m_cbUserCurRank, sizeof(m_cbUserCurRank));
			CopyMemory(StatusPlay.cbUserTrustee, m_cbUserTrustee, sizeof(m_cbUserTrustee));
			//���ͳ���;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		}
	}
	
	return false;
}


//��ʱ���¼�;
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{	
	//��Ϸ��ʼ��Ϣ;
	if (dwTimerID == IDI_GAEM_START)
	{
		m_pITableFrame->KillGameTimer(IDI_GAEM_START);
		bool bSuccess = false;
		try
		{
			bSuccess = OnGameStart();
		}
		catch (...)
		{
			CTraceService::TraceString(TEXT("CTableFrameSink OnGameStart"), TraceLevel_Exception);
			bSuccess = false;
			ASSERT(FALSE);
		}
		return bSuccess;
	}
	else if (dwTimerID == IDI_TRIBUTE)
	{
		m_pITableFrame->KillGameTimer(IDI_TRIBUTE);

		bool bSuccess = false;
		try
		{
			bSuccess = OnTributeTimeOut(0);
		}
		catch (...)
		{
			CTraceService::TraceString(TEXT("CTableFrameSink OnTributeTimeOut(0)"), TraceLevel_Exception);
			bSuccess = false;
			ASSERT(FALSE);
		}
		return bSuccess;
	}
	else if (dwTimerID == IDI_BACK_TRIBUTE)
	{
		m_pITableFrame->KillGameTimer(IDI_BACK_TRIBUTE);

		bool bSuccess = false;
		try
		{
			bSuccess = OnTributeTimeOut(1);
		}
		catch (...)
		{
			CTraceService::TraceString(TEXT("CTableFrameSink OnTributeTimeOut(1)"), TraceLevel_Exception);
			bSuccess = false;
			ASSERT(FALSE);
		}
		return bSuccess;
	}
	else if (dwTimerID == IDI_OUT_CARD)
	{
		m_pITableFrame->KillGameTimer(IDI_OUT_CARD);

		bool bSuccess = false;
		try
		{
			bSuccess = OnOutCardTimeOut();
		}
		catch (...)
		{
			CTraceService::TraceString(TEXT("CTableFrameSink OnOutCardTimeOut"), TraceLevel_Exception);
			bSuccess = false;
			ASSERT(FALSE);
		}
		return bSuccess;
	}
	else if (dwTimerID == IDI_RESPONSE_QUIT)
	{
		m_pITableFrame->KillGameTimer(IDI_RESPONSE_QUIT);
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (i!=(WORD)wBindParam && m_cbResponseStatus[i]==0)
			{
				//���Ͳ�ͬ��;
				OnDoResponseQuit(i, (WORD)wBindParam, 2);
			}
		}
		return true;
	}

	return false;
}

//��Ϸ��Ϣ;
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch(wSubCmdID)
	{
		//�û�����;
	case SUB_C_OUT_CARD:		
		{
			//��������;
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//Ч������;
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;

	
			//�û�Ч��;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubOutCard(pUserData->wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubOutCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
		//�û�����;
	case SUB_C_PASS_CARD:
		{
			//�û�Ч��;
			tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubPassCard(pUserData->wChairID);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubPassCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
	case SUB_C_PAY_TRIBUTE:
		{
		    //Ч������;
			ASSERT(wDataSize==sizeof(CMD_C_PayTribute));
			if (wDataSize!=sizeof(CMD_C_PayTribute)) return false;
			
			//�û�Ч��;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;
		
			//��������;
			CMD_C_PayTribute * pPayTribute=(CMD_C_PayTribute *)pDataBuffer;
         
			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubPayTribute(pUserData->wChairID, pDataBuffer);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubPayTribute"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
	case SUB_C_RETURN_TRIBUTE:
		{
			//Ч������;
			ASSERT(wDataSize == sizeof(CMD_C_PayTribute));
			if (wDataSize != sizeof(CMD_C_PayTribute)) return false;

			//�û�Ч��;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			//��������;
			CMD_C_PayTribute * pPayTribute = (CMD_C_PayTribute *)pDataBuffer;

			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubReturnTribute(pUserData->wChairID, pDataBuffer);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubReturnTribute"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
		//�û��й�;
	case SUB_C_USER_TRUST:
		{
			//У������;
			ASSERT(wDataSize==sizeof(CMD_C_UserTrust));
			if (wDataSize!=sizeof(CMD_C_UserTrust)) return false;
			
			//�û�У��;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubTrustee(pUserData->wChairID, pDataBuffer);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubTrustee"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
	case SUB_C_REQUEST_QUIT:
		{
			//У������;
			ASSERT(wDataSize==sizeof(CMD_C_RequestQuit));
			if (wDataSize!=sizeof(CMD_C_RequestQuit))	return false;
			
			//�û�У��;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;
			
			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubRequestQuit(pDataBuffer);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubRequestQuit"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
	case SUB_C_RESPONSE_QUIT:
		{
			//У������;
			ASSERT(wDataSize==sizeof(CMD_C_ResposeQuit));
			if (wDataSize!=sizeof(CMD_C_ResposeQuit))	return false;

			//�û�У��;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//��Ϣ����;
				bSuccess = OnSubResponseQuit(pIServerUserItem->GetChairID(), pDataBuffer);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CTableFrameSink OnSubResponseQuit"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}
			return bSuccess;
		}
	}

	return false;
}

//�����Ϣ����;
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����;
bool CTableFrameSink::OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//Ч��״̬;
	BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
	ASSERT(cbGameStatus == GS_WK_PLAYING);
	if (cbGameStatus != GS_WK_PLAYING)
	{
		return true;
	}

	ASSERT(wChairID == m_wCurrentUser);
	if (wChairID != m_wCurrentUser)
	{
		return true;
	}
	
	//����˿�
	BYTE cbMagicCardData[MAX_COUNT] = {0};
	m_GameLogic.MagicCardData(cbCardData, cbCardCount, cbMagicCardData);

	//��ȡ����
	BYTE cbCardType = m_GameLogic.GetCardType(cbMagicCardData, cbCardCount);

	//�����ж�
	if (cbCardType == CT_ERROR)
	{
		ASSERT(FALSE);
		return false;
	}

	//�����ж�
	if (m_cbTurnCardCount != 0)
	{
		//�Ա��˿�
		if (m_GameLogic.CompareCard(m_cbMagicCardData, cbMagicCardData, m_cbTurnCardCount, cbCardCount) == false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//ɾ���˿�
	if (m_GameLogic.RemoveCard(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
	{
		ASSERT(FALSE);
		return false;
	}
	m_cbHandCardCount[wChairID] -= cbCardCount;

	//ɾ����ʱ��;
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);

	//���Ƽ�¼;
	m_cbTurnCardType = cbCardType;
	m_cbTurnCardCount = cbCardCount;
	CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);
	CopyMemory(m_cbMagicCardData, cbMagicCardData, sizeof(BYTE)*cbCardCount);

	//ɾ���ѷ�����;
	if (m_wFriendFirst != 0)
	{
		m_wFriendFirst = 0;
	}

	//�л��û�;
	if(m_cbHandCardCount[wChairID]==0 && (m_cbHandCardCount[(wChairID+2)%m_wPlayerCount]==0))
	{
		 m_wCurrentUser = INVALID_CHAIR;
	}
	else
	{
		m_wCurrentUser = (m_wCurrentUser+1)%m_wPlayerCount;
		while(0==m_cbHandCardCount[m_wCurrentUser])
		{
			m_wCurrentUser = (m_wCurrentUser+1)%m_wPlayerCount;		
		}
	}
   
	if(m_cbHandCardCount[wChairID] == 0)
	{
 		m_cbRanking[m_cbMark] = (BYTE)wChairID;
		m_cbMark++;		
		m_cbUserCurRank[wChairID] = m_cbMark;
 		m_wTurnWiner = m_wCurrentUser;
 
 		//�ѷ�����;
 		if (m_wCurrentUser != INVALID_CHAIR)
 		{
 			m_wFriendFirst = (wChairID+2)%GAME_PLAYER << 8 | (GAME_PLAYER - m_cbMark);
 		}
 
 		//�����������Ϣ;
 		CMD_S_User_Out_Over UserOutOver;
 		UserOutOver.wChairID = wChairID;
 		UserOutOver.cbRanking = m_cbMark;
 		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_OUT_OVER, &UserOutOver, sizeof(UserOutOver));
 		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_USER_OUT_OVER, &UserOutOver, sizeof(UserOutOver));
	}
	else
	{
		m_wTurnWiner = wChairID;
	}

	//��������;
	CMD_S_OutCard OutCard;
	OutCard.cbCardCount = cbCardCount;
	OutCard.wOutCardUser = wChairID;
	OutCard.wCurrentUser = m_wCurrentUser;
	OutCard.wLastOutUser = m_wLastOutUser;
	OutCard.cbCardType = cbCardType;
	CopyMemory(OutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount*sizeof(BYTE));

	WORD wSendSize = sizeof(OutCard) - sizeof(OutCard.cbCardData) + OutCard.cbCardCount*sizeof(BYTE);
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, wSendSize);
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OUT_CARD, &OutCard, wSendSize);
	//��¼�ϴγ������ID;
	m_wLastOutUser = wChairID;

	//���ͶԼ��˿�;
	if(m_cbHandCardCount[wChairID]==0 && m_wCurrentUser!=INVALID_CHAIR)
	{
		CMD_S_Opposite_Card SCard;
		SCard.wFromUser = (wChairID+2)%m_wPlayerCount;
		SCard.cbCardCount = m_cbHandCardCount[SCard.wFromUser];
		CopyMemory(SCard.cbCardData, m_cbHandCardData[SCard.wFromUser], SCard.cbCardCount);
		m_pITableFrame->SendTableData(wChairID, SUB_S_OPPOSITE_CARD, &SCard, sizeof(SCard));
	}
	
	if(m_cbHandCardCount[(wChairID+2)%m_wPlayerCount]==0 && m_wCurrentUser!=INVALID_CHAIR)
	{
		CMD_S_Opposite_Card SCard;
		SCard.wFromUser = wChairID;
		SCard.cbCardCount = m_cbHandCardCount[SCard.wFromUser];
		CopyMemory(SCard.cbCardData, m_cbHandCardData[wChairID], SCard.cbCardCount);
		m_pITableFrame->SendTableData((wChairID+2)%m_wPlayerCount, SUB_S_OPPOSITE_CARD, &SCard,sizeof(SCard));
	}

	//�����ж�;
	if (m_wCurrentUser==INVALID_CHAIR) 
	{
		OnEventGameConclude(wChairID, NULL, GER_NORMAL);
	}
	else
	{
		if(m_cbUserTrustee[m_wCurrentUser] > 0)
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeTrustee * 1000, 1, 0);
		}
		else
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeOutCard * 1000, 1, 0);
		}
	}

	return true;
}

//�û�����;
bool CTableFrameSink::OnSubPassCard(WORD wChairID)
{
	//Ч��״̬;
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return true;

	//ɾ����ʱ��;
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);

	//���ñ���;
	m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	while(m_cbHandCardCount[m_wCurrentUser]==0)
	{
		m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
	}

	if (m_wCurrentUser==m_wTurnWiner)
	{
		m_cbTurnCardCount = 0;
		m_wLastOutUser = INVALID_CHAIR;
	}

	CMD_S_PassCard PassCard;
	PassCard.wJiefengUser = INVALID_CHAIR;
	//�ѷ�����;
	if ((m_wFriendFirst&0x00FF) != 0)
	{
		if(((--m_wFriendFirst) & 0x00FF) == 0)
		{	
			m_wCurrentUser = m_wFriendFirst>>8;
			m_wTurnWiner = m_wCurrentUser;
			m_cbTurnCardCount = 0;
			m_wFriendFirst = 0;
			PassCard.wJiefengUser = (m_wCurrentUser+2)%GAME_PLAYER;
		}
	}

	//��������;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_cbTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	//���ö�ʱ��;
	if (m_cbUserTrustee[m_wCurrentUser] > 0)
	{
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeTrustee * 1000, 1, 0);
	}
	else
	{
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeOutCard * 1000, 1, 0);
	}

	return true;
}

//�û�����;
bool CTableFrameSink::OnSubPayTribute(WORD wChairID, const void * pDataBuffer)
{
	ASSERT(1 == m_cbPayTributeStatus);

	//��ȡ����;
	CMD_C_PayTribute *pPayTribute = (CMD_C_PayTribute *)pDataBuffer;
	BYTE cbTributeCard = pPayTribute->cbCard;

	if (!m_bTributeStatus[wChairID])
	{
		WORD wFirst = m_cbRanking[0];
		WORD wSecond = m_cbRanking[1];

		ASSERT(m_cbHandCardCount[wChairID]==27);
		m_wCurrentUser = INVALID_CHAIR;	
		m_bTributeStatus[wChairID] = true;
		m_cbTributeCard[wChairID] = cbTributeCard;
		m_GameLogic.RemoveCard(&cbTributeCard, 1, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]);
		m_cbHandCardCount[wChairID]--;

		if (m_cbGameStatus == SHUANG_XIA)
		{
			WORD wJinGongID1 = (wFirst + 1) % GAME_PLAYER;
			WORD wJinGongID2 = (wFirst + 3) % GAME_PLAYER;
			ASSERT(wChairID==wJinGongID1 || wChairID==wJinGongID2);

			//ȫ���������;
			if (m_bTributeStatus[wJinGongID1] && m_bTributeStatus[wJinGongID2])
			{
				CMD_S_PayTributeResult PayTribute;

				BYTE cbJinGongCard1 = m_cbTributeCard[wJinGongID1];
				BYTE cbJinGongCard2 = m_cbTributeCard[wJinGongID2];
				ASSERT(cbJinGongCard1 != 0 && cbJinGongCard2 != 0);

				if (m_GameLogic.GetCardLogicValue(cbJinGongCard1) > m_GameLogic.GetCardLogicValue(cbJinGongCard2))
				{
					m_GameLogic.Add(cbJinGongCard1, m_cbHandCardData[wFirst], m_cbHandCardCount[wFirst]);
					m_GameLogic.Add(cbJinGongCard2, m_cbHandCardData[wSecond], m_cbHandCardCount[wSecond]);
					m_cbHandCardCount[wFirst]++;
					m_cbHandCardCount[wSecond]++;

					PayTribute.stTribute[0].wFrom = wJinGongID1;
					PayTribute.stTribute[0].wTo = wFirst;
					PayTribute.stTribute[0].cbCard = cbJinGongCard1;
					PayTribute.stTribute[1].wFrom = wJinGongID2;
					PayTribute.stTribute[1].wTo = wSecond;
					PayTribute.stTribute[1].cbCard = cbJinGongCard2;
					m_wTributeBigID = wJinGongID1;

					// ����ع�����;
					m_aryRetTributeTarget[wFirst] = wJinGongID1;
					m_aryRetTributeTarget[wSecond] = wJinGongID2;
				}
				else
				{
					m_GameLogic.Add(cbJinGongCard1, m_cbHandCardData[wSecond], m_cbHandCardCount[wSecond]);
					m_GameLogic.Add(cbJinGongCard2, m_cbHandCardData[wFirst], m_cbHandCardCount[wFirst]);
					m_cbHandCardCount[wFirst]++;
					m_cbHandCardCount[wSecond]++;

					PayTribute.stTribute[0].wFrom = wJinGongID2;
					PayTribute.stTribute[0].wTo = wFirst;
					PayTribute.stTribute[0].cbCard = cbJinGongCard2;
					PayTribute.stTribute[1].wFrom = wJinGongID1;
					PayTribute.stTribute[1].wTo = wSecond;
					PayTribute.stTribute[1].cbCard = cbJinGongCard1;
					m_wTributeBigID = wJinGongID2;

					// ����ع�����;
					m_aryRetTributeTarget[wFirst] = wJinGongID2;
					m_aryRetTributeTarget[wSecond] = wJinGongID1;
				}
				m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
				m_pITableFrame->SetGameTimer(IDI_BACK_TRIBUTE, m_pGameCustomRule->cbTimeRetTribute * 1000, 1, 0);
				m_cbPayTributeStatus = 2;


				//��������;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
			}
			else
			{
				CMD_S_PayTributeCard TributeCard;
				TributeCard.wChairID = wChairID;
				TributeCard.cbCardData = cbTributeCard;
				//��������;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_CARD, &TributeCard, sizeof(TributeCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_CARD, &TributeCard, sizeof(TributeCard));
			}
		}
		else if (m_cbGameStatus == DAN_XIA)
		{
			CMD_S_PayTributeResult PayTribute;

			ASSERT(wChairID==(wSecond+2)%GAME_PLAYER);
			m_GameLogic.Add(m_cbTributeCard[wChairID], m_cbHandCardData[wFirst], m_cbHandCardCount[wFirst]);
			m_cbHandCardCount[wFirst]++;
			PayTribute.stTribute[0].wFrom = wChairID;
			PayTribute.stTribute[0].wTo = wFirst;
			PayTribute.stTribute[0].cbCard = m_cbTributeCard[wChairID];
			m_wTributeBigID = wChairID;

			// ����ع�����;
			m_aryRetTributeTarget[wFirst] = wChairID;

			//��������;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));

			m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
			m_pITableFrame->SetGameTimer(IDI_BACK_TRIBUTE, m_pGameCustomRule->cbTimeRetTribute * 1000, 1, 0);
			m_cbPayTributeStatus = 2;
		}
		else if (m_cbGameStatus == OTHER)
		{
			CMD_S_PayTributeResult PayTribute;

			ASSERT(wChairID==(wFirst+2)%GAME_PLAYER);
			m_GameLogic.Add(m_cbTributeCard[wChairID], m_cbHandCardData[wFirst], m_cbHandCardCount[wFirst]);
			m_cbHandCardCount[wFirst]++;
			PayTribute.stTribute[0].wFrom = wChairID;
			PayTribute.stTribute[0].wTo = wFirst;
			PayTribute.stTribute[0].cbCard = m_cbTributeCard[wChairID];
			m_wTributeBigID = wChairID;

			// ����ع�����;
			m_aryRetTributeTarget[wFirst] = wChairID;

			//��������;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));

			m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
			m_pITableFrame->SetGameTimer(IDI_BACK_TRIBUTE, m_pGameCustomRule->cbTimeRetTribute * 1000, 1, 0);
			m_cbPayTributeStatus = 2;
		}
	}

	return true;
}

//�û��ع�;
bool CTableFrameSink::OnSubReturnTribute(WORD wChairID, const void * pDataBuffer)
{
	ASSERT(2 == m_cbPayTributeStatus);

	//��ȡ����;
	CMD_C_PayTribute *pPayTribute = (CMD_C_PayTribute *)pDataBuffer;
	BYTE bTributeCard = pPayTribute->cbCard;

	WORD wToChairID = m_aryRetTributeTarget[wChairID];

	if (!m_bTributeStatus[wChairID])
	{
		WORD wFirst = m_cbRanking[0];
		WORD wSecond = m_cbRanking[1];

		ASSERT(m_cbHandCardCount[wChairID] == 28);
		m_wCurrentUser = INVALID_CHAIR;
		m_bTributeStatus[wChairID] = true;
		m_cbTributeCard[wChairID] = bTributeCard;
		m_GameLogic.RemoveCard(&bTributeCard, 1, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]);
		m_cbHandCardCount[wChairID]--;

		TributeInfo stTribute;

		if (m_cbGameStatus == SHUANG_XIA)
		{
			stTribute.cbCard = bTributeCard;
			stTribute.wFrom = wChairID;
			stTribute.wTo = wToChairID;
			m_GameLogic.Add(bTributeCard, m_cbHandCardData[wToChairID], m_cbHandCardCount[wToChairID]);
			m_cbHandCardCount[wToChairID]++;
			ASSERT(m_cbHandCardCount[wToChairID] == 27);
			if (m_bTributeStatus[wFirst] && m_bTributeStatus[wSecond])
			{
				m_wCurrentUser = m_wTributeBigID;
			}
			//��������;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
		}
		else if (m_cbGameStatus == DAN_XIA)
		{
			stTribute.cbCard = bTributeCard;
			stTribute.wFrom = wChairID;
			stTribute.wTo = wToChairID;
			m_GameLogic.Add(bTributeCard, m_cbHandCardData[wToChairID], m_cbHandCardCount[wToChairID]);
			m_cbHandCardCount[wToChairID]++;
			ASSERT(m_cbHandCardCount[wToChairID] == 27);
			m_wCurrentUser = wToChairID;
			//��������;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
		}
		else if (m_cbGameStatus == OTHER)
		{
			stTribute.cbCard = bTributeCard;
			stTribute.wFrom = wChairID;
			stTribute.wTo = wToChairID;
			m_GameLogic.Add(bTributeCard, m_cbHandCardData[wToChairID], m_cbHandCardCount[wToChairID]);
			m_cbHandCardCount[wToChairID]++;
			ASSERT(m_cbHandCardCount[wToChairID] == 27);
			m_wCurrentUser = wToChairID;
			//��������;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
		}
	}

	if (m_wCurrentUser != INVALID_CHAIR)
	{
		m_pITableFrame->KillGameTimer(IDI_BACK_TRIBUTE);
		//������Ϸ״̬;
		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

		//֪ͨ��ҿ�ʼ����;
		CMD_S_StartOut StartOut;
		StartOut.wCurrentUser = m_wCurrentUser;
		StartOut.wWinSideUser = INVALID_CHAIR;
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			StartOut.cbHandCardCount = m_cbHandCardCount[i];
			CopyMemory(StartOut.cbHandCardData, &m_cbHandCardData[i], sizeof(StartOut.cbHandCardData));
			m_pITableFrame->SendTableData(i, SUB_S_START_OUT, &StartOut, sizeof(StartOut));
			m_pITableFrame->SendLookonData(i, SUB_S_START_OUT, &StartOut, sizeof(StartOut));
		}

		//������������;
		CMD_S_AllCard ALL_CARD;
		ZeroMemory(&ALL_CARD, sizeof(ALL_CARD));
		CopyMemory(ALL_CARD.cbHandCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
		CopyMemory(ALL_CARD.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
		//�ǻ����˷���������;
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
			{
				m_pITableFrame->SendTableData(i, SUB_S_ALL_CARD, &ALL_CARD, sizeof(ALL_CARD));
			}
		}

		if (m_cbUserTrustee[m_wCurrentUser] > 0)
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeTrustee * 1000, 1, 0);
		}
		else
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeHeadOutCard * 1000, 1, 0);
		}
	}

	return true;
}

//�û��й�;
bool CTableFrameSink::OnSubTrustee(WORD wChairID, const void* pDataBuffer)
{
	//��������;
	CMD_C_UserTrust *pUserTrust = (CMD_C_UserTrust*) pDataBuffer;
	VERIFY_RETURN_FALSE(wChairID<m_wPlayerCount);
	//״̬У��;
	if (m_cbHandCardCount[wChairID] == 0) return true;

	//������ֵ;
	m_cbUserTrustee[wChairID] = pUserTrust->cbUserTrustee;
	if (wChairID == m_wCurrentUser && pUserTrust->cbUserTrustee>0)
	{
		OnOutCardTimeOut();
	}

	//������Ϣ;
	CMD_S_User_Trust  UserTrust;
	UserTrust.cbUserTrustee = pUserTrust->cbUserTrustee;
	UserTrust.wChairID = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_TRUST, &UserTrust, sizeof(UserTrust));
	return true;
}

//�û�����;
bool CTableFrameSink::OnActionUserOffLine( WORD wChairID, IServerUserItem * pIServerUserItem )
{
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//����ʱ��ת����ϵͳ�й�;
	if (m_cbUserTrustee[wChairID]!=2 && !m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = true;
	}
	return true;
}

//�û�����;
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//�����������;
	if (m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = false;
	}

	return true;
}

//�û�����;
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//�û�����;
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//��Ϸ��ʼ;
bool CTableFrameSink::OnGameStart()
{
	//������ʼ;
	m_wFriendFirst = 0;

	//�����û�;
	if(m_wBankerUser == INVALID_CHAIR)
	{
		m_wCurrentUser = rand() % m_wPlayerCount;
		m_wBankerUser = m_wCurrentUser;
	}
	else
	{
		m_wCurrentUser = m_wBankerUser;
	}

	//�����˿�;
	for(WORD i=0; i<m_wPlayerCount; i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i] ,m_cbHandCardCount[i], ST_ORDER);
	}

	//�����ж�;
	if(m_cbGameStatus ==	SHUANG_XIA)
	{
		m_wCurrentUser = INVALID_CHAIR;
		BYTE  bFirst = m_cbRanking[0];
		//ӵ��˫����������;
		if(m_cbHandCardData[bFirst][0]!=0x4F && m_cbHandCardData[(bFirst+2)%m_wPlayerCount][0]!=0x4F)
		{	
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			m_wCurrentUser = bFirst;
		}
	}
	else if(m_cbGameStatus == DAN_XIA)
	{
		m_wCurrentUser = INVALID_CHAIR;
		BYTE bFourth = (m_cbRanking[1]+2)%m_wPlayerCount;
		//ӵ��˫����������;
		if(m_cbHandCardData[bFourth][0]==0x4F && m_cbHandCardData[bFourth][1]==0x4F)
		{	
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			m_wCurrentUser = m_cbRanking[0];
		}
	}
	else if(m_cbGameStatus == OTHER)
	{	
		m_wCurrentUser = INVALID_CHAIR;
		BYTE bFourth = (m_cbRanking[0]+2) % GAME_PLAYER;
		//ӵ��˫����������;
		if(m_cbHandCardData[bFourth][0]==0x4F&&m_cbHandCardData[bFourth][1]==0x4F)
		{	
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			m_wCurrentUser = m_cbRanking[0];
		}
	}

	CMD_S_PayTributeNotify PayTribute;
	//�������;
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbPayTribute[i] == 2)
			{
				PayTribute.cbPayStatus = enJin_Gong;
			}
			else
			{
				PayTribute.cbPayStatus = enNone;
			}
			m_pITableFrame->SendTableData(i, SUB_S_PAY_TRIBUTE_NOTIFY, &PayTribute, sizeof(PayTribute));
		}
	}
	//������;
	else if(m_cbGameStatus != 0)
	{
		PayTribute.cbPayStatus = enKang_Gong;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_NOTIFY, &PayTribute, sizeof(PayTribute));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_NOTIFY, &PayTribute, sizeof(PayTribute));
	}

	//������Ϸ״̬;
	if (m_wCurrentUser == INVALID_CHAIR)	
	{
		m_cbPayTributeStatus = 1;
		m_pITableFrame->SetGameStatus(GS_WK_TRIBUTE);
		m_pITableFrame->SetGameTimer(IDI_TRIBUTE, m_pGameCustomRule->cbTimePayTribute * 1000, 1, 0);
	}
	else
	{	
		CMD_S_StartOut StartOut;
		StartOut.wCurrentUser = m_wCurrentUser;
		StartOut.wWinSideUser = m_wCurrentUser;
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			StartOut.cbHandCardCount = m_cbHandCardCount[i];
			CopyMemory(StartOut.cbHandCardData, &m_cbHandCardData[i], sizeof(StartOut.cbHandCardData));
			m_pITableFrame->SendTableData(i, SUB_S_START_OUT, &StartOut, sizeof(StartOut));
			m_pITableFrame->SendLookonData(i, SUB_S_START_OUT, &StartOut, sizeof(StartOut));
		}

		///////////////////////////////////////////////////////////////////////////////////////
		//��������������Ϣ;
		CMD_S_AllCard ALL_CARD;
		ZeroMemory(&ALL_CARD, sizeof(ALL_CARD));
		//����������;
		CopyMemory(ALL_CARD.cbHandCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
		CopyMemory(ALL_CARD.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
		//�ǻ����˷���������;
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem* pIUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIUserItem != NULL &&pIUserItem->IsAndroidUser())
			{
				m_pITableFrame->SendTableData(i, SUB_S_ALL_CARD, &ALL_CARD, sizeof(ALL_CARD));
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////

		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD, m_pGameCustomRule->cbTimeHeadOutCard * 1000, 1, 0);
	}

	return true;
}

//��(��)����ʱ;
bool CTableFrameSink::OnTributeTimeOut( BYTE cbTributeType )
{
	//��ʱ�û�;
	WORD wOverTimeUser[2] = {INVALID_CHAIR, INVALID_CHAIR};
	if (m_pITableFrame->GetGameStatus() == GS_WK_TRIBUTE)
	{	
		WORD wFirst = m_cbRanking[0], wSecond=m_cbRanking[1];
		//����;
		if (cbTributeType == 0)
		{
			if (m_cbGameStatus == SHUANG_XIA)
			{
				WORD wThree=(wFirst+1)%GAME_PLAYER, wFour=(wFirst+3)%GAME_PLAYER;
				//����;
				if (!m_bTributeStatus[wThree])
				{	
					ASSERT(m_cbHandCardCount[wThree]==27);
					wOverTimeUser[0] = wThree;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_GameLogic.GetJingongCard(m_cbHandCardData[wThree]);
					OnSubPayTribute(wThree, &PayTribute);
				}

				if (!m_bTributeStatus[wFour])
				{
					ASSERT(m_cbHandCardCount[wFour]==27);
					wOverTimeUser[1] = wFour;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_GameLogic.GetJingongCard(m_cbHandCardData[wFour]);
					OnSubPayTribute(wFour, &PayTribute);
				}
			}
			else if (m_cbGameStatus == DAN_XIA)
			{
				WORD wFour=(wSecond+2)%GAME_PLAYER;
				//����;
				if (!m_bTributeStatus[wFour])
				{
					ASSERT(m_cbHandCardCount[wFour]==27);
					wOverTimeUser[0] = wFour;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_GameLogic.GetJingongCard(m_cbHandCardData[wFour]);
					OnSubPayTribute(wFour, &PayTribute);
				}
			}
			else if (m_cbGameStatus == OTHER)
			{
				WORD wFour=(wFirst+2)%GAME_PLAYER;
				//����;
				if (!m_bTributeStatus[wFour])
				{
					ASSERT(m_cbHandCardCount[wFour]==27);
					wOverTimeUser[0] = wFour;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_GameLogic.GetJingongCard(m_cbHandCardData[wFour]);
					OnSubPayTribute(wFour, &PayTribute);
				}
			}
		}
		//�ع�;
		else if (cbTributeType == 1)
		{
			if (m_cbGameStatus == SHUANG_XIA)
			{
				WORD wThree=(wFirst+1)%GS_WK_PLAYING, wFour=(wFirst+3)%GAME_PLAYER;
				//�ع�;
				if (!m_bTributeStatus[wFirst])
				{
					ASSERT(m_cbHandCardCount[wFirst]==28);
					wOverTimeUser[0] = wFirst;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_cbHandCardData[wFirst][27];
					OnSubReturnTribute(wFirst, &PayTribute);
				}
				if (!m_bTributeStatus[wSecond])
				{
					ASSERT(m_cbHandCardCount[wSecond]==28);
					wOverTimeUser[1] = wSecond;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_cbHandCardData[wSecond][27];
					OnSubReturnTribute(wSecond, &PayTribute);
				}
			}
			else if (m_cbGameStatus == DAN_XIA)
			{
				WORD wFour=(wSecond+2)%GAME_PLAYER;
				//�ع�;
				if (!m_bTributeStatus[wFirst])
				{
					ASSERT(m_cbHandCardCount[wFirst]==28);
					wOverTimeUser[0] = wFirst;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_cbHandCardData[wFirst][27];
					OnSubReturnTribute(wFirst, &PayTribute);
				}
			}
			else if (m_cbGameStatus == OTHER)
			{
				WORD wFour=(wFirst+2)%GAME_PLAYER;
				//�ع�;
				if (!m_bTributeStatus[wFirst])
				{
					ASSERT(m_cbHandCardCount[wFirst]==28);
					wOverTimeUser[0] = wFirst;
					CMD_C_PayTribute PayTribute;
					PayTribute.cbCard = m_cbHandCardData[wFirst][27];
					OnSubReturnTribute(wFirst, &PayTribute);
				}
			}
		}

		//״̬����,������;
		for (int i=0; i<2; ++i)
		{
			if (wOverTimeUser[i]!=INVALID_CHAIR && m_cbUserTrustee[wOverTimeUser[i]]==0 && !m_bGameEnd)
			{
				m_cbUserTrustee[wOverTimeUser[i]] = 2;
				CMD_S_User_Trust UserTrust;
				UserTrust.cbUserTrustee = m_cbUserTrustee[wOverTimeUser[i]];
				UserTrust.wChairID = wOverTimeUser[i];
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_TRUST, &UserTrust, sizeof(UserTrust));
			}
		}
	}
	return true;
}
//���ơ�PASS��ʱ;
bool CTableFrameSink::OnOutCardTimeOut()
{
	//��ʱ�û�;
	WORD wOverTimeUser = INVALID_CHAIR;
	if (m_pITableFrame->GetGameStatus() == GS_WK_PLAYING)
	{
		//�û�У��,�����˳�ʱ������������ʱ�Ҳ���ԭ��;
		//ASSERT(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsAndroidUser() == false);
		//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsAndroidUser()) return true;
		
		m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
		wOverTimeUser = m_wCurrentUser;
		
		//���ȳ���;
		if (m_cbTurnCardCount == 0)
		{
			ASSERT(m_cbHandCardCount[m_wCurrentUser] > 0);
			BYTE cbLastCardIndex = m_cbHandCardCount[m_wCurrentUser] - 1;
			ASSERT(cbLastCardIndex < 27);
			BYTE cbCardData = m_cbHandCardData[m_wCurrentUser][cbLastCardIndex];
			ASSERT(cbCardData != 0);
			OnSubOutCard(m_wCurrentUser, &cbCardData, 1);
		}
		//ѹ��;
		else
		{
			OnSubPassCard(m_wCurrentUser);
		}

		//״̬����,������;
		if (wOverTimeUser!=INVALID_CHAIR && m_wCurrentUser!=INVALID_CHAIR && m_cbUserTrustee[wOverTimeUser]==0 && !m_bGameEnd)
		{
			m_cbUserTrustee[wOverTimeUser] = 2;
			CMD_S_User_Trust UserTrust;
			UserTrust.cbUserTrustee = m_cbUserTrustee[wOverTimeUser];
			UserTrust.wChairID = wOverTimeUser;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_TRUST, &UserTrust, sizeof(UserTrust));
		}		
	}
	return true;
}


bool CTableFrameSink::OnSubRequestQuit( const void * pDataBuffer )
{
	CMD_C_RequestQuit *pRequest = (CMD_C_RequestQuit *)pDataBuffer;
	//ǿ���˳�;
	if (pRequest->cbRequestQuitType == 2)
	{
		//������Ϸ;
		OnEventGameConclude(pRequest->wChairID, NULL, GER_USER_LEAVE);

		//�رտͻ���;
		CMD_S_ForceQuit stQuitNot;
		ZeroMemory(&stQuitNot, sizeof(stQuitNot));
		stQuitNot.wChairID = pRequest->wChairID;
		m_pITableFrame->SendTableData(pRequest->wChairID, SUB_S_FORCE_QUIT, &stQuitNot, sizeof(stQuitNot));
		return true;
	}
	//����������ڵ���3�Σ�ֱ�ӷ���;
	if (m_cbRequestTimes[pRequest->wChairID] >= REQUEST_QUIT_COUNT)
	{
		return true;
	}
	//��������ID�������б���;
	m_lstRequestUserID.push_back(pRequest->wChairID);
	//��������;
	SendRequestQuitMsg();
	return true;
}

bool CTableFrameSink::OnSubResponseQuit( WORD wMeChiarID, const void * pDatabuffer )
{
	//��������;
	CMD_C_ResposeQuit *pResponseQuit = (CMD_C_ResposeQuit *)pDatabuffer;
	//����ظ�����;
	return	OnDoResponseQuit(wMeChiarID, pResponseQuit->wChairID, pResponseQuit->cbResponseQuitType);
}

//���������˳���Ϣ;
void CTableFrameSink::SendRequestQuitMsg()
{
	//�������������˳�����ǰ��û�з��͹�;
	if (!m_lstRequestUserID.empty() && !m_bHaveSendRequestQuit)
	{
		WORD wRequestQuitUser = m_lstRequestUserID.front();
		CMD_S_RequestQuit stRequestQuitNot;
		ZeroMemory(&stRequestQuitNot, sizeof(stRequestQuitNot));
		stRequestQuitNot.wChairID = wRequestQuitUser;
		//��������;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if ((i!=wRequestQuitUser)&&(pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()!=US_OFFLINE))
			{
				m_pITableFrame->SendTableData(i, SUB_S_REQUEST_QUIT, &stRequestQuitNot, sizeof(stRequestQuitNot));
			}
		}
		//��¼���������ûظ�״̬;
		m_bHaveSendRequestQuit = true;
		m_cbRequestTimes[wRequestQuitUser]++;
		ZeroMemory(m_cbResponseStatus, sizeof(m_cbResponseStatus));
		m_pITableFrame->SetGameTimer(IDI_RESPONSE_QUIT, 6000, 1, wRequestQuitUser);
		m_lstRequestUserID.pop_front();
	}
}

//�ظ�������;
bool CTableFrameSink::OnDoResponseQuit( WORD wMeChairID, WORD wRequestID, BYTE cbResponseQuitType )
{
	//�ظ�����ֱ�ӷ���;
	if (m_cbResponseStatus[wMeChairID] != 0) return true;

	m_cbResponseStatus[wMeChairID] = cbResponseQuitType;
	
	BYTE cbResponseCount=0, cbAgreeQuitCount=0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		//�ظ���;
		if (m_cbResponseStatus[i] != 0)
		{
			cbResponseCount++;
			//ͬ���˳�;
			if (m_cbResponseStatus[i] == 1)
			{
				cbAgreeQuitCount++;
			}
		}
	}

	//����ȫ���ظ�;
	if (cbResponseCount >= 3)
	{
		//ɾ����ʱ��;
		m_pITableFrame->KillGameTimer(IDI_RESPONSE_QUIT);
		m_bHaveSendRequestQuit = false;

		CMD_S_ResponseQuit stResponseQuitNot;
		ZeroMemory(&stResponseQuitNot, sizeof(stResponseQuitNot));
		stResponseQuitNot.wChairID = wRequestID;
		m_pITableFrame->SendTableData(stResponseQuitNot.wChairID, SUB_S_RESPONSE_QUIT, &stResponseQuitNot, sizeof(stResponseQuitNot));
		//ͬ���˳�;
		if (cbAgreeQuitCount >= 3)
		{
			OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);
			stResponseQuitNot.bResponse = true;
			m_pITableFrame->SendTableData(stResponseQuitNot.wChairID, SUB_S_RESPONSE_QUIT, &stResponseQuitNot, sizeof(stResponseQuitNot));
		}
		else
		{
			stResponseQuitNot.bResponse = false;
			m_pITableFrame->SendTableData(stResponseQuitNot.wChairID, SUB_S_RESPONSE_QUIT, &stResponseQuitNot, sizeof(stResponseQuitNot));
			//������������������;
			SendRequestQuitMsg();
		}
	}

	return true;
}

//����˽�˳�������Ϣ
void CTableFrameSink::ResetPrivateEndInfo()
{
	ZeroMemory(&m_PrivateEndInfo, sizeof(CMD_S_Private_End_Info));
}

//���˽�˳�������Ϣ��
void CTableFrameSink::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		// �ܻ���
		IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem)
		{
			m_PrivateEndInfo.lAllScore[i] = pServerUserItem->GetUserScore();
		}
	}

	m_PrivateEndInfo.StreamValue(kDataStream, bSend);
}

//�ж�˽�˳��Ƿ����
bool CTableFrameSink::IsPrivateEnd()
{
	ASSERT(m_pITableFrame != nullptr);
	if (m_pITableFrame == nullptr) return false;

	tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	tagPrivateFrameRecordInfo* pPrivateRecord = m_pITableFrame->GetPrivateFrameRecord();

	if ((pPrivateFrame == nullptr) || (pPrivateRecord == nullptr)) return false;

	if (pPrivateFrame->cbGameCout <= pPrivateRecord->cbFinishCout)
	{
		return true;
	}

	return false;
}