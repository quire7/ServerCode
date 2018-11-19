#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#define IDI_TIMER_XIAO_HU			1			//С��;
#define IDI_USER_OUT_CARD			2			//���Ƽ�ʱ��;
#define IDI_USER_OPERATE			3			//������ʱ��;


#define TIME_XIAO_HU  8
#define IDT_USER_OUT_CARD			15			//����ʱ��;
#define IDT_USER_OPERATE			15			//����ʱ��;

#define IDT_USER_TRUSTEE			2			//�й���Ӧʱ��;

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����
	m_cbOneShootScore = 0;
	m_cbNiaoCount = 0;
	m_cbNiaoPick = 0;
	m_wBankerUser=INVALID_CHAIR;
	m_wEastUser = INVALID_CHAIR;		//�����û�
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	m_bCanTrustee = false;

	//������Ϣ
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//������Ϣ
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbRepertoryCard_HZ,sizeof(m_cbRepertoryCard_HZ));
	
	//���б���
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;

	//״̬����
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory( m_dwChiHuKind,sizeof(m_dwChiHuKind) );
	memset( m_wProvider,INVALID_CHAIR,sizeof(m_wProvider) );

	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	m_cbGameTypeIdex = GAME_TYPE_ZZ;
	m_dwGameRuleIdex = GAME_RULE_ZZ_HONGZHONG | GAME_RULE_ZZ_ZHANIAO4 | GAME_RULE_ZZ_QIANGGANGHU;

	m_cbMagicCardData	= 0x35;
	m_cbMagicCardIndex	= m_GameLogic.SwitchToCardIndex(0x35);

	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));

	ZeroMemory( m_lStartHuScore,sizeof(m_lStartHuScore) );
	ZeroMemory( m_cbCardDataNiao,sizeof(m_cbCardDataNiao) );
	
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
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
	if (m_pITableFrame==NULL)
		return false;

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	return true;
}

//��λ����
VOID CTableFrameSink::RepositionSink()
{
	//��Ϸ����
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_wLostFanShu,sizeof(m_wLostFanShu) );

	//�����й�״̬;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_bTrustee[i] = false;
	}

	//������Ϣ
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//������Ϣ
	m_cbOneShootScore = 0;
	m_cbNiaoPick = 0;
	m_cbNiaoCount = 0;
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(m_cbRepertoryCard_HZ,sizeof(m_cbRepertoryCard_HZ));

	//���б���
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	//m_wBankerUser = INVALID_CHAIR;

	//״̬����
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//����˿�
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbChiHuCard=0;
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));
	
	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		m_ChiHuRight[i].SetEmpty();
		m_StartHuRight[i].SetEmpty();
	}

	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));

	ZeroMemory( m_lStartHuScore,sizeof(m_lStartHuScore) );
	ZeroMemory( m_cbCardDataNiao,sizeof(m_cbCardDataNiao) );

	ZeroMemory(m_cbMasterCheckCard, sizeof(m_cbMasterCheckCard));

	return;
}


//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	Shuffle(m_cbRepertoryCard_HZ, MAX_REPERTORY_HZ);

	//�����˿�
	LONG lSiceCount = MAKELONG(MAKEWORD(rand() % 6 + 1, rand() % 6 + 1), MAKEWORD(rand() % 6 + 1, rand() % 6 + 1));
	//m_wBankerUser = ((BYTE)(lSiceCount>>24)+(BYTE)(lSiceCount>>16)-1)%GAME_PLAYER;

	//���ñ���
	m_cbProvideCard = 0;
	m_wProvideUser = INVALID_CHAIR;
	m_wCurrentUser = m_wBankerUser;

	if (m_wEastUser == INVALID_CHAIR)
	{
		m_wEastUser = m_wBankerUser;
	}

	//��������
	CMD_S_AndroidGameStart ags;
	ags.GameStart.lSiceCount = lSiceCount;
	ags.GameStart.wBankerUser = m_wBankerUser;
	ags.GameStart.wCurrentUser = m_wCurrentUser;
	ags.GameStart.wEastUser = m_wEastUser;			//�����û�
	ags.GameStart.cbLeftCardCount = m_cbLeftCardCount;

	ags.GameStart.cbOutCardTime = IDT_USER_OUT_CARD;	//����ʱ��
	ags.GameStart.cbOperateTime = IDT_USER_OPERATE;		//����ʱ��

	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);
	ags.GameStart.cbXiaoHuTag = 0;

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//���������˵���;
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],&ags.cbCardData[MAX_COUNT*i]);
	}

	//��������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//���ñ���
		ags.GameStart.cbUserAction = WIK_NULL;//m_cbUserAction[i];

		IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pUserItem->IsAndroidUser())
		{
			//��������;
			m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &ags, sizeof(ags));
		}
		else
		{
			ZeroMemory(ags.GameStart.cbCardData, sizeof(ags.GameStart.cbCardData));
			m_GameLogic.SwitchToCardData(m_cbCardIndex[i], ags.GameStart.cbCardData);

			//��������;
			m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &ags.GameStart, sizeof(ags.GameStart));
			m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &ags.GameStart, sizeof(ags.GameStart));
		}
	}

	m_bSendStatus = true;
	DispatchCardData(m_wCurrentUser);

	//test
// 	BYTE byTest[] = {
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,
// 
// 		0x21,0x22,0x14,0x15,0x16,0x17,
// 		0x08,0x12,0x13,0x14,0x15,0x16,0x17,
// 		0x19,0x12,0x13,0x14,0x15,0x16,0x17,
// 
// 		0x21,0x22,0x23,0x24,0x25,0x26,0x27,
// 		0x21,0x22,0x23,0x11,0x24,0x25,0x26,0x27,
// 
// 		0x12,0x13,0x23,0x24,0x25,0x26,0x27,0x28,0x18,0x18,0x18,0x08,0x08,
// 
// 		0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x29,0x29,0x29,0x28,
// 
// 		0x11,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x19,0x19,0x28,
// 
// 		0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x09,0x09,0x09,0x11,
// 	};
//	CopyMemory( m_cbRepertoryCard,byTest,sizeof(byTest) );
//	m_wBankerUser = 0;
	//end test


	return true;
}

void CTableFrameSink::Shuffle(BYTE* pRepertoryCard,int nCardCount)
{
	m_cbLeftCardCount = nCardCount;
	m_GameLogic.RandCardData(pRepertoryCard,nCardCount);

	//�ַ��˿�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( m_pITableFrame->GetTableUserItem(i) != NULL )
		{
			m_cbLeftCardCount-=(MAX_COUNT-1);
			m_GameLogic.SwitchToCardIndex(&pRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
		}
	}
}
//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	if (m_bCanTrustee)
	{
		//�����ʱ��;
		m_pITableFrame->KillGameTimer(IDI_USER_OUT_CARD);
		for (BYTE i = 0; i < GAME_PLAYER; i++)
		{
			m_pITableFrame->KillGameTimer(IDI_USER_OPERATE + i);
		}
	}

	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			GameEnd.cbChiHuCard = m_cbChiHuCard;

			//������������
			ZeroMemory(GameEnd.cbCardDataNiao,sizeof(GameEnd.cbCardDataNiao));
			GameEnd.cbNiaoPick = m_cbNiaoPick;
			GameEnd.cbNiaoCount = m_cbNiaoCount;
			for (int i = 0;i<MAX_NIAO_CARD && i< m_cbNiaoCount;i++)
			{
				GameEnd.cbCardDataNiao[i] = m_cbCardDataNiao[i];
			}
			
			//������Ϣ
			WORD wWinner = INVALID_CHAIR;
			BYTE cbLeftUserCount = 0;			//�ж��Ƿ�����
			bool bUserStatus[GAME_PLAYER];		//
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
				m_ChiHuRight[i].GetRightData( &GameEnd.dwChiHuRight[i],MAX_RIGHT_COUNT );
				m_StartHuRight[i].GetRightData( &GameEnd.dwStartHuRight[i],MAX_RIGHT_COUNT );
				
				//���������
				if( m_ChiHuRight[i].IsEmpty() ) cbLeftUserCount++;
				//��ǰ���״̬
				if( NULL != m_pITableFrame->GetTableUserItem(i) ) bUserStatus[i] = true;
				else bUserStatus[i] = false;
			}
			
			LONGLONG lGangScore[GAME_PLAYER];
			ZeroMemory(&lGangScore,sizeof(lGangScore));
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				for( BYTE j = 0; j < m_GangScore[i].cbGangCount; j++ )
				{
					for( WORD k = 0; k < GAME_PLAYER; k++ )
						lGangScore[k] += m_GangScore[i].lScore[j][k];
				}
			}

			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				m_lGameScore[i] += lGangScore[i];
				m_lGameScore[i] += m_lStartHuScore[i];

				//��ֹ����
				IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
				if( GAME_GENRE_SCORE!=m_pGameServiceOption->wServerType &&
					m_lGameScore[i] < 0L && -m_lGameScore[i] > pUserItem->GetUserScore() )
					m_lGameScore[i] = -pUserItem->GetUserScore();
			}

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

			int	lGameTaxs[GAME_PLAYER];				//
			ZeroMemory(&lGameTaxs,sizeof(lGameTaxs));
			//ͳ�ƻ���
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if( NULL == m_pITableFrame->GetTableUserItem(i) ) continue;

				//���û���
				if (m_lGameScore[i]>0L)
				{
					lGameTaxs[i] = m_pITableFrame->CalculateRevenue(i,m_lGameScore[i]);
					m_lGameScore[i] -= lGameTaxs[i];
				}
				
				BYTE ScoreKind;
				if( m_lGameScore[i] > 0L ) ScoreKind = SCORE_TYPE_WIN;
				else if( m_lGameScore[i] < 0L ) ScoreKind = SCORE_TYPE_LOSE;
				else ScoreKind = SCORE_TYPE_DRAW;

				ScoreInfoArray[i].lScore   = m_lGameScore[i];
				ScoreInfoArray[i].lRevenue = lGameTaxs[i];
				ScoreInfoArray[i].cbType   = ScoreKind;

			}

			//д�����
			m_pITableFrame->WriteTableScore(ScoreInfoArray,CountArray(ScoreInfoArray));

			CopyMemory( GameEnd.lGameScore,m_lGameScore,sizeof(GameEnd.lGameScore) );
			CopyMemory( GameEnd.lGangScore,lGangScore,sizeof(GameEnd.lGangScore) );
			CopyMemory( GameEnd.wProvideUser,m_wProvider,sizeof(GameEnd.wProvideUser) );
			CopyMemory( GameEnd.lGameTax,lGameTaxs,sizeof(GameEnd.lGameTax));
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				CopyMemory( GameEnd.wLostFanShu[i],m_wLostFanShu[i],sizeof(m_wLostFanShu[i]) );
				CopyMemory(GameEnd.WeaveItemArray[i],m_WeaveItemArray[i],sizeof(m_WeaveItemArray[i]));
			}
			CopyMemory( GameEnd.lStartHuScore,m_lStartHuScore,sizeof(GameEnd.lStartHuScore) );

			CString strValue;
			strValue.Format(" [END] m_lGameScore[0]:%lld,m_lGameScore[1]:%lld,m_lGameScore[2]:%lld,m_lGameScore[3]:%lld,lGangScore[0]:%lld,lGangScore[1]:%lld,lGangScore[2]:%lld,lGangScore[3]:%lld.\n",
				m_lGameScore[0], m_lGameScore[1], m_lGameScore[2], m_lGameScore[3], lGangScore[0], lGangScore[1], lGangScore[2], lGangScore[3]);
			OutputDebugString(strValue);

			//����˿�
			CopyMemory(GameEnd.cbWeaveCount,m_cbWeaveItemCount,sizeof(GameEnd.cbWeaveCount));

			GameEnd.cbLeftNum = m_cbLeftCardCount;
			if (GameEnd.cbLeftNum > 0 )
			{
				CopyMemory(GameEnd.cbLeftCardData, m_cbRepertoryCard_HZ, sizeof(BYTE)*GameEnd.cbLeftNum);
			}

			//���ͽ�����Ϣ
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd) );

			if (m_wProvideUser != m_wBankerUser)
			{	//����ׯ��;
				m_wBankerUser = (m_wBankerUser + 1) % GAME_PLAYER;
			}

			//������Ϸ
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_DISMISS:		//��Ϸ��ɢ
		{
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//���ñ���
			memset( GameEnd.wProvideUser,INVALID_CHAIR,sizeof(GameEnd.wProvideUser) );

			//�����˿�
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				GameEnd.cbCardCount[i]=m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameEnd.cbCardData[i]);
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);

			return true;
		}
	case GER_NETWORK_ERROR:		//�������
	case GER_USER_LEAVE:		//�û�ǿ��
		{
			//m_bTrustee[wChairID] = true;

			return true;
		}
	}

	//�������
	ASSERT(FALSE);
	return false;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//��������
			StatusFree.wBankerUser=m_wBankerUser;
			if (m_wEastUser == INVALID_CHAIR)
			{
				StatusFree.wEastUser = m_wBankerUser;
			}
			else
			{
				StatusFree.wEastUser = m_wEastUser;
			}

			StatusFree.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_PLAY:	//��Ϸ״̬
		{
			//��������
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//��Ϸ����
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.wEastUser = m_wEastUser;
			StatusPlay.lCellScore=m_pGameServiceOption->lCellScore;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));

			//״̬����
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;

			//��ʷ��¼
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			for (int i = 0;i<GAME_PLAYER;i++)
			{
				CopyMemory(StatusPlay.cbDiscardCard[i],m_cbDiscardCard[i],sizeof(BYTE)*60);
			}
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//����˿�
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			CopyMemory(&StatusPlay.TingPaiResult, &m_TingPaiResult[wChiarID].TingPaiItemArrary[0], sizeof(StatusPlay.TingPaiResult));

			//�˿�����
			StatusPlay.cbCardCount=m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID],StatusPlay.cbCardData);
			StatusPlay.cbSendCardData=((m_cbSendCardData!=0)&&(m_wProvideUser==wChiarID))?m_cbSendCardData:0x00;

			StatusPlay.cbOutCardTime = IDT_USER_OUT_CARD;		//����ʱ��
			StatusPlay.cbOperateTime = IDT_USER_OPERATE;		//����ʱ��

			//���ͳ���
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_TIMER_XIAO_HU:  //���ƽ���
		{
			ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
			m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

			m_bSendStatus = true;
			DispatchCardData(m_wCurrentUser);
			return true;
		}
	case IDI_USER_OUT_CARD:
		{
			//���ƶ�ʱ����;
			WORD wOutChairID = static_cast<WORD>(wBindParam);
			ASSERT(wOutChairID != INVALID_CHAIR);
			if (wOutChairID == INVALID_CHAIR)
			{
				return true;
			}

			//��������й�״̬������Ϊ�й�;
			if (!m_bTrustee[wOutChairID])
			{
				OnUserTrustee(wOutChairID, true);
			}

			//����в���;
			if (m_cbUserAction[wOutChairID] > 0)
			{
				//�����С��;
				if ((m_cbUserAction[wOutChairID] & WIK_XIAO_HU)>0)
				{
					CMD_C_XiaoHu XiaoHu;
					ZeroMemory(&XiaoHu, sizeof(XiaoHu));
					XiaoHu.cbOperateCode = WIK_NULL;

					IServerUserItem* pIUserItem = m_pITableFrame->GetTableUserItem(wOutChairID);
					ASSERT(pIUserItem!=NULL);

					DisposeZZXiaohu(&XiaoHu, sizeof(XiaoHu), pIUserItem);
				}
				else
				{
					if (m_bGangStatus && (m_cbUserAction[wOutChairID] & WIK_CHI_HU))
					{	//ǿ�ܺ������;
						OnUserOperateCard(wOutChairID, WIK_NULL, 0);
						return true;
					}
					else
					{
						OnUserOperateCard(wOutChairID, WIK_NULL, 0);
					}
				}
			}

			////���������������;
			//OnUserOutCard(wOutChairID, m_cbSendCardData);

			//�ҵ�һ������;
			byte cbCardIndex = -1;
			for (BYTE i = 0; i < MAX_INDEX; i++)
			{
				if (m_cbCardIndex[wOutChairID][i] > 0)
				{
					cbCardIndex = i;
					break;
				}
			}

			ASSERT(cbCardIndex >= 0);
			if (cbCardIndex >= 0)
			{
				OnUserOutCard(wOutChairID, m_GameLogic.SwitchToCardData(cbCardIndex));
			}

			return true;
		}
	case IDI_USER_OPERATE:
	case IDI_USER_OPERATE+1:
	case IDI_USER_OPERATE+2:
	case IDI_USER_OPERATE+3:
		{
			//������ʱ����;
			WORD wChairID = static_cast<WORD>(wBindParam);
			ASSERT(wChairID != INVALID_CHAIR);
			if (wChairID == INVALID_CHAIR)
			{
				return true;
			}

			//��������й�״̬������Ϊ�й�;
			if (!m_bTrustee[wChairID])
			{
				OnUserTrustee(wChairID, true);
			}

			//����в���;
			ASSERT(m_cbUserAction[wChairID] > 0);
			if (m_cbUserAction[wChairID] > 0)
			{
				OnUserOperateCard(wChairID, WIK_NULL, 0);
			}

			return true;
		}
	}
	return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_OUT_CARD:		//������Ϣ
		{
			//Ч����Ϣ
			ASSERT(wDataSize==sizeof(CMD_C_OutCard));
			if (wDataSize!=sizeof(CMD_C_OutCard)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pIServerUserItem->GetChairID(),pOutCard->cbCardData);
		}
	case SUB_C_OPERATE_CARD:	//������Ϣ
		{
			//Ч����Ϣ
			ASSERT(wDataSize==sizeof(CMD_C_OperateCard));
			if (wDataSize!=sizeof(CMD_C_OperateCard)) return false;

			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_OperateCard * pOperateCard=(CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pIServerUserItem->GetChairID(),pOperateCard->cbOperateCode,pOperateCard->cbOperateCard);
		}
	case SUB_C_TRUSTEE:
		{
			CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pDataBuffer;
			if(wDataSize != sizeof(CMD_C_Trustee)) return false;

			return OnUserTrustee(pIServerUserItem->GetChairID(), pTrustee->bTrustee);
		}
	case SUB_C_XIAOHU:
		{
			return DisposeZZXiaohu(pDataBuffer, wDataSize, pIServerUserItem);
		}
#ifdef DEBUG
		case SUB_C_MASTER_CHEAKCARD:
		{
			CMD_C_MaterCheckCard *pMaterCheckCard = (CMD_C_MaterCheckCard *)pDataBuffer;
			if (wDataSize != sizeof(CMD_C_MaterCheckCard)) return false;

			WORD wChairID = pIServerUserItem->GetChairID();
			m_cbMasterCheckCard[wChairID] = pMaterCheckCard->cbCheakCard;
			return true;
		}
#endif // DEBUG
	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//ׯ������;
	if ((bLookonUser == false) && (m_wBankerUser == INVALID_CHAIR))
	{
		m_wBankerUser = pIServerUserItem->GetChairID();
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//ׯ������
	if (bLookonUser==false)
	{
		BYTE cbPlayerCount = 0;
		//ִ���ж�;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (NULL == m_pITableFrame->GetTableUserItem(i)) continue;
			cbPlayerCount++;
		}

		//��Ҷ��뿪����ׯ�ҺͶ���;
		if (0==cbPlayerCount)
		{
			m_wBankerUser = INVALID_CHAIR;
			m_wEastUser = INVALID_CHAIR;
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData)
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()==GS_MJ_PLAY);
	if (m_pITableFrame->GetGameStatus()!=GS_MJ_PLAY) return true;

	//�������
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//Ч�����
	if (wChairID!=m_wCurrentUser) return true;
	if (m_GameLogic.IsValidCard(cbCardData)==false) return true;

	//ɾ���˿�
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);
		return true;
	}

	if (m_bCanTrustee)
	{
		//ɾ�����ƶ�ʱ��;
		m_pITableFrame->KillGameTimer(IDI_USER_OUT_CARD);
	}

	//���ñ���
	m_bSendStatus=true;
	if( m_bGangStatus )
	{
		m_bGangStatus = false;
		m_bGangOutStatus = true;
	}
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//���Ƽ�¼
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//��������
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	if (IsTingPai(wChairID))
	{	//������������;
		m_pITableFrame->SendTableData(wChairID, SUB_S_TING_PAI, &m_TingPaiResult[wChairID].TingPaiItemArrary[0], sizeof(m_TingPaiResult[wChairID].TingPaiItemArrary[0]));
		m_pITableFrame->SendLookonData(wChairID, SUB_S_TING_PAI, &m_TingPaiResult[wChairID].TingPaiItemArrary[0], sizeof(m_TingPaiResult[wChairID].TingPaiItemArrary[0]));
	}

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;

	//�û��л�
	m_wCurrentUser=(wChairID+GAME_PLAYER-1)%GAME_PLAYER;

	//��Ӧ�ж�
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//�ɷ��˿�
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//�û�����
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard, bool bCheckQiangGangHu)
{
	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()!=GS_MJ_FREE);
	if (m_pITableFrame->GetGameStatus()==GS_MJ_FREE)
		return true;

	//Ч���û�	ע�⣺�������п��ܷ����˶���
	//ASSERT((wChairID==m_wCurrentUser)||(m_wCurrentUser==INVALID_CHAIR));
	if ((wChairID!=m_wCurrentUser)&&(m_wCurrentUser!=INVALID_CHAIR)) 
		return true;

	//��������
	if (m_wCurrentUser==INVALID_CHAIR)
	{
		//Ч��״̬
		if (m_bResponse[wChairID]==true) 
			return true;
		if ((cbOperateCode!=WIK_NULL)&&((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//��������
		WORD wTargetUser=wChairID;
		BYTE cbTargetAction=cbOperateCode;

		//���ñ���
		m_bResponse[wChairID]=true;
		m_cbPerformAction[wChairID]=cbOperateCode;
		m_cbOperateCard[wChairID]=m_cbProvideCard;

		//ִ���ж�
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//��ȡ����
			BYTE cbUserAction=(m_bResponse[i]==false)?m_cbUserAction[i]:m_cbPerformAction[i];

			//���ȼ���
			BYTE cbUserActionRank=m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank=m_GameLogic.GetUserActionRank(cbTargetAction);

			//�����ж�
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser=i;
				cbTargetAction=cbUserAction;
			}
		}
		if (m_bResponse[wTargetUser]==false) 
			return true;

		//�Ժ��ȴ�
		if (cbTargetAction==WIK_CHI_HU)
		{
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if ((m_bResponse[i]==false)&&(m_cbUserAction[i]&WIK_CHI_HU))
					return true;
			}
		}

		//��������
		if (cbTargetAction==WIK_NULL)
		{
			if (m_bCanTrustee)
			{
				//���������ʱ��
				m_pITableFrame->KillGameTimer(IDI_USER_OPERATE + wChairID);
			}

			if (m_bGangStatus && (m_cbUserAction[wChairID] & WIK_CHI_HU))
			{	//ǿ�ܺ�����£����Ƶ����ʱ�������ܲ���;
				m_wCurrentUser = m_wProvideUser;
				m_cbUserAction[m_wProvideUser] = WIK_GANG;

				OnUserOperateCard(m_wProvideUser, WIK_GANG, m_cbProvideCard, false);
			}
			else
			{
				//�û�״̬
				ZeroMemory(m_bResponse, sizeof(m_bResponse));
				ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
				ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
				ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

				//�����˿�
				DispatchCardData(m_wResumeUser);
			}

			return true;
		}

		//��������
		BYTE cbTargetCard=m_cbOperateCard[wTargetUser];

		//���Ʊ���
		m_cbOutCardData=0;
		m_bSendStatus=true;
		m_wOutCardUser=INVALID_CHAIR;

		//���Ʋ���
		if (cbTargetAction==WIK_CHI_HU)
		{
			setNiaoCard(wTargetUser);//��������������
			//������Ϣ
			m_cbChiHuCard=cbTargetCard;

			for (WORD i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER;i!=m_wProvideUser;i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//�����ж�
				if ((m_cbPerformAction[i]&WIK_CHI_HU)==0)
					continue;

				//�����ж�
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[i];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[i];
				m_dwChiHuKind[i] = AnalyseChiHuCard(m_cbCardIndex[i],pWeaveItem,cbWeaveItemCount,m_cbChiHuCard,m_ChiHuRight[i]);

				//�����˿�
				if (m_dwChiHuKind[i]!=WIK_NULL) 
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					ProcessChiHuUser( i,false );
				}
			}

			OnEventGameConclude( INVALID_CHAIR,NULL,GER_NORMAL );

			return true;
		}

		//�û�״̬
		ZeroMemory(m_bResponse,sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

		//����˿�
		ASSERT(m_cbWeaveItemCount[wTargetUser]<4);
		WORD wIndex=m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard=cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind=cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//ɾ���˿�
		switch (cbTargetAction)
		{
		case WIK_LEFT:		//���Ʋ���
			{
				//ɾ���˿�
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_LEFT,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_RIGHT:		//���Ʋ���
			{
				//ɾ���˿�
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_RIGHT,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_CENTER:	//���Ʋ���
			{
				//ɾ���˿�
				BYTE cbRemoveCard[3];
				m_GameLogic.GetWeaveCard(WIK_CENTER,cbTargetCard,cbRemoveCard);
				VERIFY( m_GameLogic.RemoveCard(cbRemoveCard,3,&cbTargetCard,1) );
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_PENG:		//���Ʋ���
			{
				//ɾ���˿�
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,2) );

				break;
			}
		case WIK_GANG:		//���Ʋ���
			{
				//ɾ���˿�,��������ֻ���ڷŸ�
				BYTE cbRemoveCard[]={cbTargetCard,cbTargetCard,cbTargetCard};
				VERIFY( m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser],cbRemoveCard,CountArray(cbRemoveCard)) );

				break;
			}
		default:
			ASSERT( FALSE );
			return false;
		}

		if (m_bCanTrustee)
		{
			//���������ʱ��;
			m_pITableFrame->KillGameTimer(IDI_USER_OPERATE + wChairID);
		}

		//������
		CMD_S_OperateResult OperateResult;
		OperateResult.wOperateUser=wTargetUser;
		OperateResult.cbOperateCard=cbTargetCard;
		OperateResult.cbOperateCode = ((cbTargetAction == WIK_GANG) ? WIK_MING_GANG : cbTargetAction);
		OperateResult.wProvideUser=(m_wProvideUser==INVALID_CHAIR)?wTargetUser:m_wProvideUser;

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));

		//�����û�
		m_wCurrentUser=wTargetUser;

		//���ƴ���
		if (cbTargetAction==WIK_GANG)
		{
			//������Ϣ
			CMD_S_GangScore gs;
			ZeroMemory( &gs,sizeof(gs) );
			gs.wChairId = wTargetUser;
			gs.cbXiaYu = FALSE;
			gs.lGangScore[wTargetUser] = m_pGameServiceOption->lCellScore*3L;
			gs.lGangScore[m_wProvideUser] = -m_pGameServiceOption->lCellScore*3L;
			m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
			m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

			//�ܵ÷�
			BYTE cbIndex = m_GangScore[wTargetUser].cbGangCount++;
			m_GangScore[wTargetUser].lScore[cbIndex][wTargetUser] = m_pGameServiceOption->lCellScore*3;
			m_GangScore[wTargetUser].lScore[cbIndex][m_wProvideUser] = -m_pGameServiceOption->lCellScore*3;
			m_bGangStatus = true;
			DispatchCardData(wTargetUser,true);
		}
		else
		{
			if (m_bCanTrustee)
			{
				//���ó���;
				if (m_bTrustee[m_wCurrentUser])
				{
					m_pITableFrame->SetGameTimer(IDI_USER_OUT_CARD, IDT_USER_TRUSTEE * 1000, 1, wChairID);
				}
				else
				{
					m_pITableFrame->SetGameTimer(IDI_USER_OUT_CARD, IDT_USER_OUT_CARD * 1000, 1, wChairID);
				}
			}
		}

		return true;
	}

	//��������
	if (m_wCurrentUser==wChairID)
	{
		//Ч�����
		if ((cbOperateCode==WIK_NULL)||((m_cbUserAction[wChairID]&cbOperateCode)==0))
			return true;

		//�˿�Ч��
		ASSERT((cbOperateCode==WIK_NULL)||(cbOperateCode==WIK_CHI_HU)||
			(m_GameLogic.IsValidCard(cbOperateCard)==true));
		if ((cbOperateCode!=WIK_NULL)&&(cbOperateCode!=WIK_CHI_HU)
			&&(m_GameLogic.IsValidCard(cbOperateCard)==false)) 
			return true;

		//���ñ���
		m_bSendStatus=true;
		m_cbUserAction[m_wCurrentUser]=WIK_NULL;
		m_cbPerformAction[m_wCurrentUser]=WIK_NULL;

		bool bPublic=false;

		//ִ�ж���
		switch (cbOperateCode)
		{
		case WIK_GANG:			//���Ʋ���
			{
				//������ܺ�;
				if (bCheckQiangGangHu && EstimateUserRespond(wChairID, cbOperateCard, EstimatKind_GangCard))
				{
					m_bGangStatus = true;

					m_pITableFrame->KillGameTimer(IDI_USER_OUT_CARD);
					return true;
				}
				//��������
				BYTE cbWeaveIndex=0xFF;
				BYTE cbCardIndex=m_GameLogic.SwitchToCardIndex(cbOperateCard);

				//���ƴ���
				if (m_cbCardIndex[wChairID][cbCardIndex]==1)
				{
					//Ѱ�����
					for (BYTE i=0;i<m_cbWeaveItemCount[wChairID];i++)
					{
						BYTE cbWeaveKind=m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard=m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
						{
							bPublic=true;
							cbWeaveIndex=i;
							break;
						}
					}

					//Ч�鶯��
					ASSERT(cbWeaveIndex!=0xFF);
					if (cbWeaveIndex==0xFF) return false;

					//����˿�
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}
				else
				{
					//�˿�Ч��
					ASSERT(m_cbCardIndex[wChairID][cbCardIndex]==4);
					if (m_cbCardIndex[wChairID][cbCardIndex]!=4) 
						return false;

					//���ñ���
					bPublic=false;
					cbWeaveIndex=m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard=FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser=wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind=cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard=cbOperateCard;
				}

				//ɾ���˿�
				m_cbCardIndex[wChairID][cbCardIndex]=0;

				m_bGangStatus = true;

				//���Ƶ÷�
				LONGLONG lScore = bPublic?m_pGameServiceOption->lCellScore:m_pGameServiceOption->lCellScore*2L;
				BYTE cbGangIndex = m_GangScore[wChairID].cbGangCount++;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if(  i == wChairID ) continue;

					m_GangScore[wChairID].lScore[cbGangIndex][i] = -lScore;
					m_GangScore[wChairID].lScore[cbGangIndex][wChairID] += lScore;
				}

				//������
				CMD_S_OperateResult OperateResult;
				OperateResult.wOperateUser=wChairID;
				OperateResult.wProvideUser=wChairID;
				//OperateResult.cbOperateCode=cbOperateCode;
				OperateResult.cbOperateCode = (bPublic?WIK_MING_GANG:WIK_AN_GANG);
				OperateResult.cbOperateCard=cbOperateCard;

				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPERATE_RESULT,&OperateResult,sizeof(OperateResult));


				//������Ϣ
				CMD_S_GangScore gs;
				gs.wChairId = wChairID;
				ZeroMemory( &gs,sizeof(gs) );
				gs.cbXiaYu = bPublic?FALSE:TRUE;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( i == wChairID ) continue;

					gs.lGangScore[i] = -lScore;
					gs.lGangScore[wChairID] += lScore;
				}
				m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );
				m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_GANG_SCORE,&gs,sizeof(gs) );

				DispatchCardData(wChairID, true);

				////Ч�鶯��
				//bool bAroseAction=false;
				//if (bPublic == true)
				//{
				//	bAroseAction = EstimateUserRespond(wChairID, cbOperateCard, EstimatKind_GangCard);
				//}

				////�����˿�
				//if (bAroseAction==false)
				//{
				//	DispatchCardData(wChairID,true);
				//}
				//else
				//{
				//	m_pITableFrame->KillGameTimer(IDI_USER_OUT_CARD);
				//}
				return true;
			}
		case WIK_CHI_HU:		//�Ժ�����
			{
				setNiaoCard(wChairID);//��������������
				//����Ȩλ
				if (m_cbOutCardCount==0)
				{
					m_wProvideUser = m_wCurrentUser;
					m_cbProvideCard = m_cbSendCardData;
				}

				//��ͨ����
				BYTE cbWeaveItemCount=m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem=m_WeaveItemArray[wChairID];
				m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],&m_cbProvideCard,1);
				m_dwChiHuKind[wChairID] = AnalyseChiHuCard(m_cbCardIndex[wChairID],pWeaveItem,cbWeaveItemCount,m_cbProvideCard,
					m_ChiHuRight[wChairID]);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

				//������Ϣ
				m_cbChiHuCard=m_cbProvideCard;

				ProcessChiHuUser( wChairID,false );

				
				OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);

				return true;
			}
		}

		return true;
	}

	return false;
}

//�йܴ���;
bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	m_bTrustee[wChairID] = bTrustee;

	CMD_S_Trustee Trustee;
	Trustee.bTrustee = bTrustee;
	Trustee.wChairID = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));

	return true;
}

//����˽�˳���Ϣ
void CTableFrameSink::SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex)
{
	//m_cbGameTypeIdex = bGameTypeIdex;
	//m_dwGameRuleIdex = bGameRuleIdex;
}

//����˽�˳�������Ϣ
void CTableFrameSink::ResetPrivateEndInfo()
{
}

//���˽�˳�������Ϣ��
void CTableFrameSink::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{

}
//�ж�˽�˳��Ƿ����
bool CTableFrameSink::IsPrivateEnd()
{
	return false;
}

//���Ͳ���
bool CTableFrameSink::SendOperateNotify()
{
	//������ʾ
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//��������
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//��������
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));

			if (m_bCanTrustee)
			{
				//���ò�����ʱ��;
				if (m_bTrustee[i])
				{
					m_pITableFrame->SetGameTimer(IDI_USER_OPERATE + i, IDT_USER_TRUSTEE * 1000, 1, i);
				}
				else
				{
					m_pITableFrame->SetGameTimer(IDI_USER_OPERATE + i, IDT_USER_OPERATE * 1000, 1, i);
				}
			}
		}
	}

	return true;
}

//�ɷ��˿�
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser,bool bTail)
{
	//״̬Ч��
	ASSERT(wCurrentUser!=INVALID_CHAIR);
	if (wCurrentUser==INVALID_CHAIR)
		return false;

	//�����˿�
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]-1]=m_cbOutCardData;
	}

	
	if ( m_cbGameTypeIdex == GAME_TYPE_ZZ )
	{
		static BYTE s_cbNiaoCount = GetNiaoCardNum(INVALID_CHAIR);
		if (m_cbLeftCardCount <= s_cbNiaoCount)
		{
			m_cbChiHuCard = 0;
			m_wProvideUser = INVALID_CHAIR;
			OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);

			return true;
		}
	}

	//��ׯ����
	if (m_cbLeftCardCount==0)
	{
		m_cbChiHuCard=0;
		m_wProvideUser=INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser,NULL,GER_NORMAL);

		return true;
	}

	//���ñ���
	m_cbOutCardData=0;
	m_wCurrentUser=wCurrentUser;
	m_wOutCardUser=INVALID_CHAIR;

	//�ܺ���
	if( m_bGangOutStatus )
	{
		m_bGangOutStatus = false;
	}

	bool bTingPai = false;
	//���ƴ���
	if (m_bSendStatus==true)
	{
		//�����˿�
		m_cbSendCardCount++;
		if (hasRule(GAME_RULE_ZZ_HONGZHONG))
		{
#ifdef DEBUG
			m_cbSendCardData = GetSendCardData(wCurrentUser);
#else
			m_cbSendCardData = m_cbRepertoryCard_HZ[--m_cbLeftCardCount];
#endif // DEBUG
		}
		else
		{
			m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
		}

		//�����ж�
		CChiHuRight chr;
		m_wProvideUser = wCurrentUser;
		m_cbUserAction[wCurrentUser]|= AnalyseChiHuCard(m_cbCardIndex[wCurrentUser],m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],m_cbSendCardData,chr);

		//����
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//���ñ���
		m_cbProvideCard = m_cbSendCardData;

		//�����ж�
		if (m_cbLeftCardCount>0)
		{
			tagGangCardResult GangCardResult;
			m_cbUserAction[wCurrentUser]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser],
				m_WeaveItemArray[wCurrentUser],m_cbWeaveItemCount[wCurrentUser],GangCardResult, m_cbSendCardData);
		}

		//����4�����е��ж�(59 == (MAX_REPERTORY_HZ - 13*4 - 1));
		if (m_cbGameTypeIdex == GAME_TYPE_ZZ && (m_cbLeftCardCount == MAX_LEFT_NUM_HZ) && m_wCurrentUser == m_wBankerUser)
		{
			CChiHuRight cChiHuRight;
			m_cbUserAction[wCurrentUser] |= AnalyseChiHuCardZZ_XIAOHU(m_cbCardIndex[wCurrentUser], cChiHuRight);
		}
	}


	//��������
	CMD_S_SendCard SendCard;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.bTail = bTail;
	//SendCard.cbActionMask=m_cbUserAction[wCurrentUser];
	//SendCard.cbCardData=(m_bSendStatus==true)?m_cbSendCardData:0x00;

	for (WORD i = 0; i < GAME_PLAYER; ++i)
	{
		if (i == wCurrentUser)
		{
			SendCard.cbActionMask	= m_cbUserAction[wCurrentUser];
			SendCard.cbCardData		= (m_bSendStatus == true) ? m_cbSendCardData : 0x00;
		}
		else
		{
			SendCard.cbActionMask = 0x00;
			SendCard.cbCardData	  = 0x00;
		}			

		//��������
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	if (m_bCanTrustee)
	{
		//���ó���;
		if (m_bTrustee[m_wCurrentUser])
		{
			m_pITableFrame->SetGameTimer(IDI_USER_OUT_CARD, IDT_USER_TRUSTEE * 1000, 1, m_wCurrentUser);
		}
		else
		{
			m_pITableFrame->SetGameTimer(IDI_USER_OUT_CARD, IDT_USER_OUT_CARD * 1000, 1, m_wCurrentUser);
		}
	}

	return true;
}

//��Ӧ�ж�
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//��������
	bool bAroseAction=false;

	//�û�״̬
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//�����ж�
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//�û�����
		if (wCenterUser==i) continue;

		//��������
		if (EstimatKind==EstimatKind_OutCard)
		{
			////�����ж�
			m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

			//�����ж�
			if (m_cbLeftCardCount>0) 
			{
				m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
			}
		}

		//�Ժ��ж�
		CChiHuRight chr;
		BYTE cbWeaveCount=m_cbWeaveItemCount[i];
		m_cbUserAction[i]|= AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr);
		

		//����ж�
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//��ɳ�齫�Բ���
	if (m_cbGameTypeIdex == GAME_TYPE_CS)
	{
		m_cbUserAction[m_wCurrentUser] = m_GameLogic.EstimateEatCard(m_cbCardIndex[m_wCurrentUser],cbCenterCard);

		//����ж�
		if (m_cbUserAction[m_wCurrentUser]!=WIK_NULL) 
			bAroseAction=true;
	}
	//�������;
	if (bAroseAction==true) 
	{
		//���ñ���
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//������ʾ
		SendOperateNotify();
		return true;
	}

	return false;
}
//
void CTableFrameSink::ProcessChiHuUser( WORD wChairId, bool bGiveUp )
{
	if( !bGiveUp )
	{
		//����Ȩλ
		CChiHuRight &chr = m_ChiHuRight[wChairId];

		FiltrateRight( wChairId,chr );
		WORD wFanShu = 0;
		if ( m_cbGameTypeIdex == GAME_TYPE_ZZ)
		{
			wFanShu = m_GameLogic.GetChiHuActionRank_ZZ(chr);
		}
		if ( m_cbGameTypeIdex == GAME_TYPE_CS)
		{
			wFanShu = m_GameLogic.GetChiHuActionRank_CS(chr);
		}
		
		LONGLONG lChiHuScore = wFanShu*m_pGameServiceOption->lCellScore;
		
		bool bGiangGangHu = false;

		//������,����ת��
		if( !(chr&CHR_GANG_SHANG_PAO).IsEmpty() )
		{
			BYTE cbGangIndex = m_GangScore[m_wProvideUser].cbGangCount-1;
			//һ�ڶ���������,������ƽ�ָܵ÷�
			BYTE cbChiHuCount = 0;
			for( WORD i = 0; i < GAME_PLAYER; i++ )
				if( m_cbPerformAction[i]==WIK_CHI_HU ) cbChiHuCount++;
			if( cbChiHuCount == 1 )
			{
				LONGLONG lScore = m_GangScore[m_wProvideUser].lScore[cbGangIndex][wChairId];
				m_GangScore[m_wProvideUser].lScore[cbGangIndex][wChairId] = m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser];
				m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] = lScore;
			}
			else
			{
				LONGLONG lGangScore = m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser]/cbChiHuCount;
				lGangScore = __max(lGangScore,m_pGameServiceOption->lCellScore);
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( m_cbPerformAction[i]==WIK_CHI_HU )
						m_GangScore[m_wProvideUser].lScore[cbGangIndex][i] = lGangScore;
				}
				m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] = 0;
				for( WORD i = 0; i < GAME_PLAYER; i++ )
				{
					if( i != m_wProvideUser)
						m_GangScore[m_wProvideUser].lScore[cbGangIndex][m_wProvideUser] += m_GangScore[m_wProvideUser].lScore[cbGangIndex][i];
				}
			}
		}
		//���ֲܸܷ���
		else if( !(chr&CHR_QIANG_GANG_HU).IsEmpty() )
		{
			bGiangGangHu = true;
			//m_GangScore[m_wProvideUser].cbGangCount--;
		}

		if( m_wProvideUser != wChairId )
			m_wLostFanShu[m_wProvideUser][wChairId] = (WORD)lChiHuScore;
		else
		{
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				m_wLostFanShu[i][wChairId] = (WORD)lChiHuScore;
			}
		}

		if( m_wProvideUser == wChairId )
		{
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				if( i == wChairId ) continue;

				//���Ʒ�
				m_lGameScore[i] -= lChiHuScore;
				m_lGameScore[wChairId] += lChiHuScore;

				//�����
				m_lGameScore[i] -= (2 * m_cbNiaoPick + m_cbOneShootScore) * m_pGameServiceOption->lCellScore;
				m_lGameScore[wChairId] += (2 * m_cbNiaoPick + m_cbOneShootScore) * m_pGameServiceOption->lCellScore;
			}
		}
		//����
		else
		{

			m_lGameScore[m_wProvideUser] -= lChiHuScore;
			m_lGameScore[wChairId] += lChiHuScore;

			if (bGiangGangHu)
			{	
				m_lGameScore[m_wProvideUser] -= (2 * m_cbNiaoPick + m_cbOneShootScore) * 3* m_pGameServiceOption->lCellScore;
				m_lGameScore[wChairId] += (2 * m_cbNiaoPick + m_cbOneShootScore) * 3 * m_pGameServiceOption->lCellScore;
			}
			else
			{
				//m_lGameScore[m_wProvideUser] -= (2 * m_cbNiaoPick + m_cbOneShootScore) * m_pGameServiceOption->lCellScore;
				//m_lGameScore[wChairId] += (2 * m_cbNiaoPick + m_cbOneShootScore) * m_pGameServiceOption->lCellScore;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (i == wChairId) continue;
					//�����;
					m_lGameScore[i] -= (2 * m_cbNiaoPick + m_cbOneShootScore) * m_pGameServiceOption->lCellScore;
					m_lGameScore[wChairId] += (2 * m_cbNiaoPick + m_cbOneShootScore) * m_pGameServiceOption->lCellScore;
				}
			}
		}

		CString strValue;
		strValue.Format(" [ProcessChiHuUser] m_lGameScore[0]:%lld,m_lGameScore[1]:%lld,m_lGameScore[2]:%lld,m_lGameScore[3]:%lld,lChiHuScore:%lld,m_cbOneShootScore:%d.\n",
			m_lGameScore[0], m_lGameScore[1], m_lGameScore[2], m_lGameScore[3], lChiHuScore, m_cbOneShootScore);
		OutputDebugString(strValue);

		//���ñ���
		m_wProvider[wChairId] = m_wProvideUser;
		m_bGangStatus = false;
		m_bGangOutStatus = false;

		//������Ϣ
		CMD_S_ChiHu ChiHu;
		ChiHu.wChiHuUser = wChairId;
		ChiHu.wProviderUser = m_wProvideUser;
		ChiHu.lGameScore = m_lGameScore[wChairId];
		ChiHu.cbCardCount = m_GameLogic.GetCardCount( m_cbCardIndex[wChairId] );
		ChiHu.cbChiHuCard = m_cbProvideCard;
		m_pITableFrame->SendTableData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );
		m_pITableFrame->SendLookonData( INVALID_CHAIR,SUB_S_CHI_HU,&ChiHu,sizeof(ChiHu) );

	}

	return;
}

BYTE CTableFrameSink::GetNiaoCardNum( WORD wHuChairID )
{
	BYTE nNum = ZZ_ZHANIAO0;
	if(hasRule(GAME_RULE_ZZ_ZHANIAO4))
	{
		nNum = ZZ_ZHANIAO4;
	}
	if(hasRule(GAME_RULE_ZZ_ZHANIAO6))
	{
		nNum = ZZ_ZHANIAO6;
	}
	if(hasRule(GAME_RULE_ZZ_ZHANIAO8))
	{
		nNum = ZZ_ZHANIAO8;
	}

	if ((nNum > 0) && (INVALID_CHAIR != wHuChairID))
	{	//�����������,�Ƿ��к���;
		if (!HasHongZhongCard(wHuChairID))
		{
			++nNum;
		}
	}

	if (nNum>m_cbLeftCardCount)
	{
		nNum = m_cbLeftCardCount;
	}
	return nNum;
}

void CTableFrameSink::setNiaoCard( WORD wHuChairID )
{
	ZeroMemory(m_cbCardDataNiao,sizeof(m_cbCardDataNiao));
	m_cbNiaoCount = GetNiaoCardNum( wHuChairID );

	if (m_cbNiaoCount>ZZ_ZHANIAO0)
	{
		BYTE cbLastCardData;
		BYTE cbCardIndexTemp[MAX_INDEX];
		m_cbLeftCardCount-= m_cbNiaoCount;
		if (hasRule(GAME_RULE_ZZ_HONGZHONG))
		{
			cbLastCardData = m_cbRepertoryCard_HZ[m_cbLeftCardCount];
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard_HZ[m_cbLeftCardCount],m_cbNiaoCount,cbCardIndexTemp);
		}
		else
		{
			cbLastCardData = m_cbRepertoryCard[m_cbLeftCardCount];
			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],m_cbNiaoCount,cbCardIndexTemp);
		}
		m_GameLogic.SwitchToCardData(cbCardIndexTemp,m_cbCardDataNiao);

		if (hasRule(GAME_RULE_ZZ_ONESHOOT))
		{
			m_cbOneShootScore = CalcOneShootScore(cbLastCardData);
		}
	}
	m_cbNiaoPick = m_GameLogic.GetPickNiaoCount(m_cbCardDataNiao,m_cbNiaoCount);

	CString strValue;
	strValue.Format(" [setNiaoCard] m_cbNiaoCount:[%d],m_cbNiaoPick:[%d],m_cbCardDataNiao[0]:[%d],m_cbCardDataNiao[1]:[%d],m_cbCardDataNiao[2]:[%d],m_cbCardDataNiao[3]:[%d],m_cbCardDataNiao[4]:[%d],m_cbCardDataNiao[5]:[%d],m_cbCardDataNiao[6]:[%d],m_cbCardDataNiao[7]:[%d].\n",
		m_cbNiaoCount, m_cbNiaoPick, m_cbCardDataNiao[0], m_cbCardDataNiao[1], m_cbCardDataNiao[2], m_cbCardDataNiao[3], m_cbCardDataNiao[4], m_cbCardDataNiao[5], m_cbCardDataNiao[6], m_cbCardDataNiao[7]);
	OutputDebugString(strValue);
}

BYTE CTableFrameSink::AnalyseChiHuCardZZ_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight)
{
	BYTE cbChiHuKind = WIK_NULL;

	//�������ֺ���4����;
	if (cbCardIndex[m_cbMagicCardIndex] == 4)
	{
		ChiHuRight |= CHR_XIAO_DA_SI_XI;
		cbChiHuKind = WIK_XIAO_HU;
	}

	return cbChiHuKind;
}

bool CTableFrameSink::DisposeZZXiaohu(const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч����Ϣ;
	ASSERT(wDataSize == sizeof(CMD_C_XiaoHu));
	if (wDataSize != sizeof(CMD_C_XiaoHu)) return false;

	//�û�Ч��;
	if (pIServerUserItem->GetUserStatus() != US_PLAYING) return true;

	//��Ϣ����
	CMD_C_XiaoHu* pXiaoHu = (CMD_C_XiaoHu *)pDataBuffer;
	if ( pXiaoHu->cbOperateCode != WIK_XIAO_HU && pXiaoHu->cbOperateCode != WIK_NULL && pXiaoHu->cbOperateCard != m_cbMagicCardData)
	{
		return true;
	}

	WORD wChairID = pIServerUserItem->GetChairID();
	if ( pXiaoHu->cbOperateCode == WIK_XIAO_HU )
	{
		m_dwChiHuKind[wChairID] = AnalyseChiHuCardCS_XIAOHU(m_cbCardIndex[wChairID], m_StartHuRight[wChairID]);

		int wFanShu = m_GameLogic.GetChiHuActionRank_CS(m_StartHuRight[wChairID]);
		LONGLONG lStartHuScore = wFanShu*m_pGameServiceOption->lCellScore;
		m_lStartHuScore[wChairID] += lStartHuScore * 3;

		for (int i = 0; i < GAME_PLAYER; i++)
		{
			if (i == wChairID)continue;
			m_lStartHuScore[i] -= lStartHuScore;
		}

		m_cbUserAction[wChairID] = WIK_NULL;

		setNiaoCard(wChairID);//��������������;
		//������Ϣ
		m_cbChiHuCard = m_cbMagicCardData;

		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
	}
	else
	{	//������;
		m_cbUserAction[wChairID] &= (~WIK_XIAO_HU);
	}

	return true;
}

BYTE CTableFrameSink::AnalyseChiHuCardCS_XIAOHU(const BYTE cbCardIndex[MAX_INDEX], CChiHuRight &ChiHuRight)
{
	BYTE cbReplaceCount = 0;
	BYTE cbChiHuKind = WIK_NULL;

	//��ʱ����
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	bool bDaSiXi = false;//����ϲ
	bool bBanBanHu = true;//����
	BYTE cbQueYiMenColor[3] = { 1, 1, 1 };//ȱһɫ
	BYTE cbLiuLiuShun = 0;//����˳

	//���㵥��
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		BYTE cbCardCount = cbCardIndexTemp[i];

		if (cbCardCount == 0)
		{
			continue;
		}

		if (cbCardCount == 4)
		{
			bDaSiXi = true;
		}

		if (cbCardCount == 3)
		{
			cbLiuLiuShun++;
		}

		BYTE cbValue = m_GameLogic.SwitchToCardData(i) & MASK_VALUE;
		if (cbValue == 2 || cbValue == 5 || cbValue == 8)
		{
			bBanBanHu = false;
		}

		BYTE cbCardColor = m_GameLogic.SwitchToCardData(i)&MASK_COLOR;
		cbQueYiMenColor[cbCardColor >> 4] = 0;
	}
	if (bDaSiXi)
	{
		ChiHuRight |= CHR_XIAO_DA_SI_XI;
		cbChiHuKind = WIK_XIAO_HU;
	}
	if (bBanBanHu)
	{
		ChiHuRight |= CHR_XIAO_BAN_BAN_HU;
		cbChiHuKind = WIK_XIAO_HU;
	}
	if (cbQueYiMenColor[0] || cbQueYiMenColor[1] || cbQueYiMenColor[2])
	{
		ChiHuRight |= CHR_XIAO_QUE_YI_SE;
		cbChiHuKind = WIK_XIAO_HU;
	}
	if (cbLiuLiuShun >= 2)
	{
		ChiHuRight |= CHR_XIAO_LIU_LIU_SHUN;
		cbChiHuKind = WIK_XIAO_HU;
	}
	return cbChiHuKind;
}

BYTE CTableFrameSink::CalcOneShootScore(BYTE cbCardData)
{	//������ϵĵ���Ϊ1��5��9��������1������������;
	if (cbCardData == 0x35)
	{	
		return 1;
	}
	return ( cbCardData & MASK_VALUE );
}

bool CTableFrameSink::HasHongZhongCard(WORD wChairID)
{
	if ( wChairID < 0 || wChairID >= GAME_PLAYER)
	{
		return true;
	}
		
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, m_cbCardIndex[wChairID], sizeof(cbCardIndexTemp));
	if ( cbCardIndexTemp[m_cbMagicCardIndex] > 0 )
	{
		return true;
	}

	//Ѱ�����;
	BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChairID];
	for (BYTE i = 0; i < cbWeaveItemCount; i++)
	{
		BYTE cbCenterCard = m_WeaveItemArray[wChairID][i].cbCenterCard;
		if ((cbCenterCard == 0x35))
		{
			return true;
		}
	}

	return false;
}

bool CTableFrameSink::IsTingPai(WORD wChairID)
{
	m_cbUserAction[wChairID] |= m_GameLogic.AnalyseTingCard( m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], m_TingPaiResult[wChairID], true );

	return ((m_cbUserAction[wChairID] & WIK_TING) == WIK_TING);
}

bool CTableFrameSink::hasRule(DWORD cbRule)
{
	//return FvMask::HasAny(m_dwGameRuleIdex,_MASK_(cbRule));
	return ((m_dwGameRuleIdex & cbRule) == cbRule);
}

BYTE CTableFrameSink::AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight)
{
	bool bSelfSendCard = (m_wProvideUser == m_wCurrentUser);

	//��������
	BYTE cbChiHuKind = WIK_NULL;
	CAnalyseItemArray AnalyseItemArray;

	if (hasRule(GAME_RULE_ZZ_ZIMOHU) && !bSelfSendCard)
	{
		return WIK_NULL;
	}

	//���ñ���
	AnalyseItemArray.RemoveAll();
	ChiHuRight.SetEmpty();
	ChiHuRight |= CHR_SHU_FAN;

	//���ܺ�
	if (m_wCurrentUser == INVALID_CHAIR && m_bGangStatus)
	{
		if (hasRule(GAME_RULE_ZZ_QIANGGANGHU))
		{
			ChiHuRight |= CHR_QIANG_GANG_HU;
		}
		else
		{
			ChiHuRight.SetEmpty();
			return WIK_NULL;
		}

	}
	//�����˿�
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp, cbCardIndex, sizeof(cbCardIndexTemp));

	//cbCurrentCardһ����Ϊ0			!!!!!!!!!
	ASSERT(cbCurrentCard != 0);
	if (cbCurrentCard == 0) return WIK_NULL;

	//�����˿�
	if (cbCurrentCard != 0)
		cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(cbCurrentCard)]++;

	//�����˿�
	bool bValue = m_GameLogic.AnalyseCard(cbCardIndexTemp, WeaveItem, cbWeaveCount, AnalyseItemArray);
	if (!bValue)
	{

		ChiHuRight.SetEmpty();
		return WIK_NULL;
	}

	//���Ʒ���
	//���ͷ���
	for (INT_PTR i = 0; i < AnalyseItemArray.GetCount(); i++)
	{
		//��������
		tagAnalyseItem * pAnalyseItem = &AnalyseItemArray[i];
		if (hasRule(GAME_RULE_ZZ_258))
		{
			BYTE cbCardValue = pAnalyseItem->cbCardEye&MASK_VALUE;
			if (cbCardValue != 2 && cbCardValue != 5 && cbCardValue != 8)
			{
				continue;
			}
		}
		cbChiHuKind = WIK_CHI_HU;
		break;
	}

	if (bSelfSendCard)
	{
		cbChiHuKind = WIK_CHI_HU;
		ChiHuRight = CHR_SHU_FAN;
		ChiHuRight |= CHR_ZI_MO;
	}
	else
	{
		if (hasRule(GAME_RULE_ZZ_QIANGGANGHU) && (ChiHuRight&CHR_QIANG_GANG_HU).IsEmpty())
		{
			cbChiHuKind = WIK_NULL;
		}
	}

	return cbChiHuKind;
}
//
void CTableFrameSink::FiltrateRight( WORD wChairId,CChiHuRight &chr )
{
	//Ȩλ����
	//����
	if( m_wCurrentUser == INVALID_CHAIR && m_bGangStatus )
	{
		chr |= CHR_QIANG_GANG_HU;
	}
	if (m_cbLeftCardCount==0)
	{
		chr |= CHR_HAI_DI_LAO;
	}
	//����Ȩλ
	//���ϻ�
	if( m_wCurrentUser==wChairId && m_bGangStatus )
	{
		chr |= CHR_GANG_KAI;
	}
	//������
	if( m_bGangOutStatus && !m_bGangStatus )
	{
		chr |= CHR_GANG_SHANG_PAO;
	}
}

// ��ȡҪ��
BYTE CTableFrameSink::GetSendCardData(WORD dwSendUser)
{
#define  swapCard(a,b)\
	BYTE nTemp = a;\
	a = b;\
	b = nTemp;\

	BYTE MasterCard = m_cbMasterCheckCard[dwSendUser];
	if (MasterCard == 0)
	{
		return m_cbRepertoryCard_HZ[--m_cbLeftCardCount];
	}

	bool bValue = false;
	for (int i = m_cbLeftCardCount - 1; i >= 0; i--)
	{
		if (m_cbRepertoryCard_HZ[i] == MasterCard)
		{
			bValue = true;
			swapCard(m_cbRepertoryCard_HZ[i], m_cbRepertoryCard_HZ[m_cbLeftCardCount - 1]);
			break;
		}
	}

	m_cbMasterCheckCard[dwSendUser] = 0;
	BYTE nCard = m_cbRepertoryCard_HZ[--m_cbLeftCardCount];
	if (!bValue)
	{
		m_pITableFrame->SendGameMessage(m_pITableFrame->GetTableUserItem(dwSendUser), _T("��ѡ������Ѿ������ڣ�"), SMT_CHAT);
	}

	return nCard;
}

//////////////////////////////////////////////////////////////////////////
