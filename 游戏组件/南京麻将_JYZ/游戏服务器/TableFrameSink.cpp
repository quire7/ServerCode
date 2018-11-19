#include "StdAfx.h"
#include "GameLottery.h"
#include "TableFrameSink.h"
#include <strsafe.h>

#define BOOL_TO_STRING(val) ((val)?"true":"false")

#define IS_OPEN_PRINT_LOG	FALSE		// �����ô�д TRUE FALSE

#define KEEP_LEFT_CARD_COUNT	0		// ��������;

///////////////////////////////////////////////////////////////////////////////////////
//��̬����
BYTE				CTableFrameSink::m_cbOpenGameLottery = 0;				//��Ϸ�齱
LONG				CTableFrameSink::m_lMaxPlazaExp = 0;					//���ƽ̨����
CString				CTableFrameSink::m_strLucyDrawCfg;						//�齱����
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//��Ϸ����
const tagGameServiceOption *CTableFrameSink::m_pGameServiceOption = NULL;	//��������

///////////////////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrameSink::CTableFrameSink()
{
	m_lBaseScore = 0L;
	m_wHostChairID = INVALID_CHAIR;
	ZeroMemory(m_bAgreeBaseScore, sizeof(m_bAgreeBaseScore));
	m_listPlayers.clear();
	m_listAgreeBaseScore.clear();
	
	//���ñ���(ȫ������)
	ResetAllData(true);
	
	//�������
	m_pITableFrame = NULL;
}

//��������
CTableFrameSink::~CTableFrameSink()
{
}

//�ӿڲ�ѯ
void * CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//��ʼ��
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL) return false;

	//��ʼģʽ
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//��Ϸ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

	srand(GetTickCount() | (m_pITableFrame->GetTableID() << 8));

	return true;
}

//��Ϸ״̬
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
	//���ñ���(��������)
	ResetAllData(false);
}

//���ñ���
void CTableFrameSink::ResetAllData(bool bAll)
{
	if (bAll)
	{
		m_wEastUser = INVALID_CHAIR;
		m_wBankerUser = INVALID_CHAIR;
		m_bLastRoundBiXiaHu = false;
	}

	m_bGameStart = false;

	//��Ϸ����
	m_wSiceCount = MAKEWORD(0, 0);
	ZeroMemory(m_lOutCardScore, sizeof(m_lOutCardScore));
	ZeroMemory(m_lGangScore, sizeof(m_lGangScore));
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHuaPaiCount, sizeof(m_cbHuaPaiCount));
	ZeroMemory(m_cbHuaPaiData, sizeof(m_cbHuaPaiData));
	ZeroMemory(m_bUserTrust, sizeof(m_bUserTrust));
	ZeroMemory(m_bUserOffline, sizeof(m_bUserOffline));

	//������Ϣ
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardRecordCount, sizeof(m_cbDiscardRecordCount));
	ZeroMemory(m_cbDiscardRecordCard, sizeof(m_cbDiscardRecordCard));
	ZeroMemory(m_cbDiscardIndexCount, sizeof(m_cbDiscardIndexCount));

	//������Ϣ
	m_bSendCardStatus = false;
	m_wSendCardUser = INVALID_CHAIR;
	m_wBuHuaUser = INVALID_CHAIR;
	m_cbSendCardData = 0;
	m_cbSendCardCount = 0;
	m_cbLeftCardCount = 0;
	ZeroMemory(m_cbRepertoryCard, sizeof(m_cbRepertoryCard));

	//���б���
	m_cbProvideCard = 0;
	m_wResumeUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_wProvideUser = INVALID_CHAIR;
	m_wGangPaiProvideUser = INVALID_CHAIR;

	//
	m_wFirstOutUserForFaFen = INVALID_CHAIR;
	m_cbLastOutCardData = 0;
	m_cbOutSameCardCount = 0;
	m_cbGangStatus = GS_NULL;
	ZeroMemory(m_bAwardStatus, sizeof(m_bAwardStatus));
	ZeroMemory(m_bHuaGangStatus, sizeof(m_bHuaGangStatus));
	ZeroMemory(m_bPenaltyStatus, sizeof(m_bPenaltyStatus));
	m_QiangGangHuInfo.ReInit();
	m_bChangeBanker = false;
	m_bHaveJiangFen = false;
	m_bHaveFaFen = false;
	m_bHaveBigHu = false;
	m_bHuaGang = false;
	m_bHaveBaozi = false;

	//�û�״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
	ZeroMemory(m_bDispatchCardOver, sizeof(m_bDispatchCardOver));
	ZeroMemory(m_bSendCardOver, sizeof(m_bSendCardOver));
	ZeroMemory(m_bBuHuaOver, sizeof(m_bBuHuaOver));
	ZeroMemory(m_bFirstBuHuaOver, sizeof(m_bFirstBuHuaOver));
	ZeroMemory(m_bUserTianTing, sizeof(m_bUserTianTing));
	ZeroMemory(m_bUserTing, sizeof(m_bUserTing));

	//�����
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//������Ϣ
	m_cbHuPaiCard = 0;
	ZeroMemory(m_HuPaiResult, sizeof(m_HuPaiResult));
	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
	ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

	ZeroMemory(m_cbMasterCheckCard, sizeof(m_cbMasterCheckCard));

	m_wCheatChairID = INVALID_CHAIR;
	m_cbLeftCheatCardCount = 0;

	ZeroMemory(m_cbCheatCard, sizeof(m_cbCheatCard));

	ZeroMemory(m_bInvalidHuStates, sizeof(m_bInvalidHuStates));
	ZeroMemory(m_cbInvalidOperteStates, sizeof(m_cbInvalidOperteStates));

}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	//����״̬
	m_pITableFrame->SetGameStatus(GS_MJ_DICE);

	m_bGameStart = true;

	//��ȡ�����ע
	m_lBaseScore = m_pITableFrame->GetCellScore();
	m_pITableFrame->KillGameTimer(IDI_BASESCORE);

	//���ñ���
	if (m_wBankerUser == INVALID_CHAIR)
	{
		m_wBankerUser = rand() % GAME_PLAYER;
		m_wEastUser = m_wBankerUser;
	}
	m_wCurrentUser = m_wBankerUser;

	//������������Ϣ
	CMD_S_ShootDice stShootDice;
	ZeroMemory(&stShootDice, sizeof(stShootDice));
	stShootDice.bCurBiXiaHu = m_bLastRoundBiXiaHu;
	stShootDice.wBankerUser = m_wBankerUser;
	stShootDice.wEastUser = m_wEastUser;

	//������������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SHOOT_DICE, &stShootDice, sizeof(stShootDice));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SHOOT_DICE, &stShootDice, sizeof(stShootDice));

	//������ʱ��
	m_pITableFrame->SetGameTimer(IDI_SHOOT_DICE, IDT_SHOOT_DICE * 1000, 1, 0);
	return true;
}

/************************************************************************/
/* �ҺͿͻ�ȷ���ˣ��������� (����20��+����+Ӳ����*2+��20��)��2
   ���º��㷨������20��+����+Ӳ����*2+��20��               20161012        */
/************************************************************************/
//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{

#if IS_OPEN_PRINT_LOG
	MyLog("OnEventGameConclude(wChairID = %d, cbReason = %d) begin...", wChairID, cbReason);
#endif

	//ɾ����ʱ��
	m_pITableFrame->KillGameTimer(IDI_SHOOT_DICE);
	m_pITableFrame->KillGameTimer(IDI_DISPATCH_CARD);
	m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
	m_pITableFrame->KillGameTimer(IDI_SEND_BUHUA);
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
	m_pITableFrame->KillGameTimer(IDI_USER_OPERATE);
	m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 0);
	m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 1);
	m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 2);
	m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 3);
	switch (cbReason)
	{
	case GER_DISMISS:		//��Ϸ��ɢ
	{
		//��������
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));

		//���ñ���
		GameEnd.wProvideUser = INVALID_CHAIR;
		GameEnd.cbGameEndType = GER_DISMISS;

		//������
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
			GameEnd.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
			CopyMemory(GameEnd.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(tagWeaveItem)*MAX_WEAVE);
			CopyMemory(&GameEnd.stHuPaiResult[i], &m_HuPaiResult[i], sizeof(tagHuPaiResult));
			GameEnd.cbHuaPaiCount[i][0] = m_cbHuaPaiCount[i][0];
			GameEnd.cbHuaPaiCount[i][1] = m_cbHuaPaiCount[i][1];

			GameEnd.lGameScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
			GameEnd.lGangScore[i] = m_lGangScore[i];
			GameEnd.lOutScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1];
		}

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		// ��Ϸ¼��
		addGameRecordAction(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		/**************�������ͳ���ж�********************************************/
#if IS_OPEN_PRINT_LOG
		LONG lEndScore = 0;

		//���ճɼ�
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			ASSERT((pIServerUserItem->GetUserScore() + GameEnd.lGameScore[i]) >= 0);

			lEndScore += GameEnd.lGameScore[i];

			MyLog("name:[%s], ChairID:[%d], lGameScore:[%ld],m_lGangScore[%ld],m_lOutCardScore0:[%ld],m_lOutCardScore1:[%ld],m_cbHuaPaiCount0:[%d],m_cbHuaPaiCount1:[%d]\n",
				pIServerUserItem->GetNickName(), i, GameEnd.lGameScore[i], m_lGangScore[i], m_lOutCardScore[i][0], m_lOutCardScore[i][1], m_cbHuaPaiCount[i][0], m_cbHuaPaiCount[i][1]);
		}

		ASSERT(lEndScore == 0);
#endif
		/************************************************************************/
		tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

		//ͳ�ƻ���
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (NULL == m_pITableFrame->GetTableUserItem(i)) continue;

			ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
			if (GameEnd.lGameScore[i] > 0L)
			{
				ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
			}
			else if (GameEnd.lGameScore[i] == 0L)
			{
				ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;
			}
			else
			{
				ScoreInfoArray[i].cbType = SCORE_TYPE_LOSE;
			}
		}

		//д�����
		DataStream kDataStream;
		m_kGameRecord.StreamValue(kDataStream, true);
		m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray), kDataStream);

		/********************************˽�˳�ͳ��****************************************/
		//ͳ���Ƿ�������
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			// Ӯ��
			if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
			{
				// ����
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbZiMoCout[i]++;
				}
				else // ׽��
				{
					m_PrivateEndInfo.cbZhuoPaoCout[i]++;
				}

				// ʤ������
				m_PrivateEndInfo.cbWinCout[i]++;
			}
			else // ���
			{
				// ����
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbDianPaoCout[i]++;
				}

				// ʧ�ܴ���
				m_PrivateEndInfo.cbLoseCout[i]++;
			}

			// ��ߵ÷�
			if (m_PrivateEndInfo.lMaxScore[i] <= GameEnd.lGameScore[i])
			{
				m_PrivateEndInfo.lMaxScore[i] = GameEnd.lGameScore[i];
			}

			// �ܻ���;
			m_PrivateEndInfo.lAllScore[i] += GameEnd.lGameScore[i];
			m_PrivateEndInfo.lBaoZiScore[i] += GameEnd.lBaoZiScore[i];
		}
		/********************************˽�˳�ͳ��****************************************/

		//������Ϸ
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		return true;
	}
	case GER_BAOZI:				// ����������;
	{
		m_bHaveBaozi = true;

		//��������
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));

		//���ñ���
		GameEnd.cbGameEndType = 0;
		GameEnd.wBankerUser = m_wBankerUser;
		GameEnd.bHaveBiXiaHu = m_bLastRoundBiXiaHu;
		GameEnd.lCellScore = m_pITableFrame->GetCellScore();

		// ����ͳ��
		GameEnd.wProvideUser = GetBaoziProvideUser(wChairID);

		LONG lBiXiaHuMultiple = (m_bLastRoundBiXiaHu ? 1L : 2L);
		GameEnd.lBaoZiScore[wChairID] = BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// �������˵÷�;
		GameEnd.lBaoZiScore[GameEnd.wProvideUser] = 0 - BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// ������˵÷�;

		//������Ϣ
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
			GameEnd.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
			CopyMemory(GameEnd.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(tagWeaveItem)*MAX_WEAVE);
			CopyMemory(&GameEnd.stHuPaiResult[i], &m_HuPaiResult[i], sizeof(tagHuPaiResult));
			GameEnd.cbHuaPaiCount[i][0] = m_cbHuaPaiCount[i][0];
			GameEnd.cbHuaPaiCount[i][1] = m_cbHuaPaiCount[i][1];

			// �ܷ�;
			GameEnd.lGameScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];//��Ϸ����
			GameEnd.lGangScore[i] = m_lGangScore[i];												//�ܷ�;	
			GameEnd.lOutScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1];					//���Ʒ�;
		}

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		// ��Ϸ¼��
		addGameRecordAction(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		m_bLastRoundBiXiaHu = IsBiXiaHu();

		//�������
		LONG lUserGameScore[GAME_PLAYER] = { 0L };
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			lUserGameScore[i] += (m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i]);
		}

		//��ҳ��������Ǯ�ж�
		bool bMoneyNotEnough = false;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			LONG lUserScore = (LONG)pServerUserItem->GetUserScore();
			if (lUserGameScore[i] < 0)
			{
				//���������
				if (abs(lUserGameScore[i]) > lUserScore)
				{
					lUserGameScore[i] = 0L - lUserScore;
					bMoneyNotEnough = true;
				}
			}
		}

		tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

		//ͳ�ƻ���
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (NULL == m_pITableFrame->GetTableUserItem(i)) continue;

			ScoreInfoArray[i].lScore = lUserGameScore[i];
			ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;
		}

		//д�����
		DataStream kDataStream;
		m_kGameRecord.StreamValue(kDataStream, true);
		m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray), kDataStream);

		/********************************˽�˳�ͳ��****************************************/
		//ͳ���Ƿ�������
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			// Ӯ��
			if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
			{
				// ����
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbZiMoCout[i]++;
				}
				else // ׽��
				{
					m_PrivateEndInfo.cbZhuoPaoCout[i]++;
				}

				// ʤ������
				m_PrivateEndInfo.cbWinCout[i]++;
			}
			else // ���
			{
				// ����
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbDianPaoCout[i]++;
				}

				// ʧ�ܴ���
				m_PrivateEndInfo.cbLoseCout[i]++;
			}

			// ��ߵ÷�
			if (m_PrivateEndInfo.lMaxScore[i] <= GameEnd.lGameScore[i])
			{
				m_PrivateEndInfo.lMaxScore[i] = GameEnd.lGameScore[i];
			}

			// �ܻ���;
			m_PrivateEndInfo.lAllScore[i] += GameEnd.lGameScore[i];
			m_PrivateEndInfo.lBaoZiScore[i] += GameEnd.lBaoZiScore[i];
		}
		/********************************˽�˳�ͳ��****************************************/

		//������Ϸ
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		return true;
	}
	case GER_NORMAL:		//�������
	{
		//��������
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));
		
		//������Ϣ
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
			GameEnd.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
			CopyMemory(GameEnd.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(tagWeaveItem)*MAX_WEAVE);
			CopyMemory(&GameEnd.stHuPaiResult[i], &m_HuPaiResult[i], sizeof(tagHuPaiResult));
			GameEnd.cbHuaPaiCount[i][0] = m_cbHuaPaiCount[i][0];
			GameEnd.cbHuaPaiCount[i][1] = m_cbHuaPaiCount[i][1];

			// �ܷ֣����Ʒ�;
			GameEnd.lGangScore[i] = m_lGangScore[i];
			GameEnd.lOutScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1];
		}

		//������Ϣ;
		GameEnd.wBankerUser = m_wBankerUser;
		GameEnd.wProvideUser = m_wProvideUser;
		GameEnd.cbProvideCard = m_cbHuPaiCard;
		GameEnd.bHaveBiXiaHu = m_bLastRoundBiXiaHu;
		GameEnd.lCellScore = m_pITableFrame->GetCellScore();
		GameEnd.cbHuCardData = m_cbHuPaiCard;		// �Ժ�����;

		bool bUserBigHu = false;
		bool bBankerHuPai = false;
		WORD wZiMoHuPaiUser = INVALID_CHAIR;
		LONG lBiXiaHuMultiple = (m_bLastRoundBiXiaHu ? 1L : 2L);
		LONG lTiaoZhanScore[GAME_PLAYER] = { 0L };

		// �����ʶ;
		DWORD dwBigHu = HU_WUHUAGUO | HU_YAJUE | HU_DGKAIHUA | HU_XGKAIHUA | HU_HUNYISE |
			HU_QINGYISE | HU_DUIDUIHU | HU_QQDUDIAO | HU_QIDUI | HU_SQIDUI | HU_TIANHU | HU_DIHU | HU_BAOZI;

		//ͳ�ƻ���
		if (m_wProvideUser != INVALID_CHAIR)
		{
			GameEnd.cbGameEndType = 0;

			BYTE cbHuPaiCount = 0;	// ��������;

			//ͳ���Ƿ�������
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
				{
					if (i == m_wProvideUser)
					{
						wZiMoHuPaiUser = i;
						break;
					}
				}
			}

			//��������(һ��ӮǮ���Ҹ�)
			if (wZiMoHuPaiUser != INVALID_CHAIR)
			{
				cbHuPaiCount++;

				//�������ׯ�ж�
				//if ((m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiKind&HU_PINGHU) == 0 && (m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiKind&HU_MENQING) == 0)
				if ((m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiKind&dwBigHu) != 0)
				{
					bUserBigHu = true;
				}

				//�Ƿ�ׯ�Һ���
				if (wZiMoHuPaiUser == m_wBankerUser)
				{
					bBankerHuPai = true;
				}

				//���㱨������
				GameEnd.lTingPaiScore[wZiMoHuPaiUser] = (m_bUserTing[wZiMoHuPaiUser] ? 10 : 0);

				// ÿ����Һ������;
				LONG lScore = (m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiFanShu + (m_cbHuaPaiCount[wZiMoHuPaiUser][0] + m_cbHuaPaiCount[wZiMoHuPaiUser][1]) * 2 + 20) / lBiXiaHuMultiple;

#if IS_OPEN_PRINT_LOG
				MyLog("�������[%d] ����ӮǮ[%d] dwHuPaiFanShu[%d] Ӳ��[%d] ��[%d] ���º�[%d]",
					wZiMoHuPaiUser, lScore, m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiFanShu, m_cbHuaPaiCount[wZiMoHuPaiUser][0], m_cbHuaPaiCount[wZiMoHuPaiUser][1], lBiXiaHuMultiple);
#endif
				
				//����ƽ��;
				LONG lAllWinScore = 0L;
				if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) && */(m_wGangPaiProvideUser != wZiMoHuPaiUser) && (m_wGangPaiProvideUser != INVALID_CHAIR))
				{
					IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wGangPaiProvideUser);
					ASSERT(pServerUserItem != NULL);

					// ��Ϸ��ʼ���ϻ���;
					LONG lUserScore = (LONG)pServerUserItem->GetUserScore();

					// ����ʱʵ�����ϻ���;
					LONG lUserGameScore = lUserScore + (m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser]);
#if IS_OPEN_PRINT_LOG
					MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== �������� UserScore = %ld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
						m_wGangPaiProvideUser, lUserScore, m_lOutCardScore[m_wGangPaiProvideUser][0], m_lOutCardScore[m_wGangPaiProvideUser][1], m_lGangScore[m_wGangPaiProvideUser]);
#endif
					LONG lShouldDelScore = (3 * lScore);
					// ����ʵ�ʻ���С������֣�ƽ�����;
					if (lUserGameScore < lShouldDelScore)
					{
						lAllWinScore += lUserGameScore;
						GameEnd.lGameScore[m_wGangPaiProvideUser] = 0 - lUserScore;	// ʵ����Ӯ;

						GameEnd.lHuScore[m_wGangPaiProvideUser] = 0 - lUserGameScore;
					}
					else
					{
						lAllWinScore += lShouldDelScore;
						GameEnd.lGameScore[m_wGangPaiProvideUser] = m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser] - lShouldDelScore;	// �����ʵ����Ӯ�����ܺͷ���;

						GameEnd.lHuScore[m_wGangPaiProvideUser] = 0 - lAllWinScore;
					}

					// ����������һ���;
					for (WORD j = 0; j < GAME_PLAYER; j++)
					{
						// ���˰������;
						if (j == m_wGangPaiProvideUser) continue;

						// ���˺������;
						if (j == wZiMoHuPaiUser) continue;

						// �����ʵ����Ӯ�����ܺͷ���;
						GameEnd.lGameScore[j] = (m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j]);
					}
				}
				else
				{ 
					for (WORD j = 0; j < GAME_PLAYER; j++)
					{
						// �����������;
						if (j == wZiMoHuPaiUser) continue;

						IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(j);
						ASSERT(pServerUserItem != NULL);

						// ��Ϸ��ʼ���ϻ���;
						LONG lUserScore = (LONG)pServerUserItem->GetUserScore();

						// ����ʱʵ�����ϻ���;
						LONG lUserGameScore = lUserScore + (m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j]);
#if IS_OPEN_PRINT_LOG
						MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ���� UserScore = %ld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
							j, lUserScore, m_lOutCardScore[j][0], m_lOutCardScore[j][1], m_lGangScore[j]);
#endif
						// ����ʵ�ʻ���С������֣�ƽ�����;
						if (lUserGameScore < lScore)
						{
							lAllWinScore += lUserGameScore;
							GameEnd.lGameScore[j] = 0 - lUserScore;	// ʵ����Ӯ;

							GameEnd.lHuScore[j] = 0 - lUserGameScore;
						}
						else
						{
							lAllWinScore += lScore;
							GameEnd.lGameScore[j] = (m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j]) - lScore;	// �����ʵ����Ӯ�����ܺͷ���;

							GameEnd.lHuScore[j] = 0 - lScore;
						}
					}
				}

				// ��Ӯ�һ���(�����ܺͷ���);
				GameEnd.lGameScore[wZiMoHuPaiUser] = lAllWinScore + (m_lOutCardScore[wZiMoHuPaiUser][0] + m_lOutCardScore[wZiMoHuPaiUser][1] + m_lGangScore[wZiMoHuPaiUser]);;
				GameEnd.lHuScore[wZiMoHuPaiUser] = lAllWinScore;
			}
			//��梁���(һ�ڶ���һ����Ǯ)
			else
			{
				//ѭ���ۼ�
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
					{
						//ׯ�Һ����ж�
						if (i == m_wBankerUser)
						{
							bBankerHuPai = true;
						}

						//�������ׯ�ж�
						if ((m_HuPaiResult[i].dwHuPaiKind&dwBigHu) != 0)
						{
							bUserBigHu = true;
						}

						// �Ƿ��а���;
						if ((m_HuPaiResult[i].dwHuPaiKind&HU_BAOZI) != 0)
						{
							m_bHaveBaozi = true;
						}

						//���㱨������
						GameEnd.lTingPaiScore[i] = (m_bUserTing[i] ? 10 : 0);

						cbHuPaiCount++;
					}
				}

				LONG lProvideLoseScore = 0L;

				// һ�ڶ��죬�ӵ������¼ҿ�ʼ��Ǯ����������������ˣ�����˳��һ�ҼҸ���ֱ�����ڵ�������Ǯȫ������;
				// ������
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wProvideUser);
				ASSERT(pServerUserItem != NULL);

				// ��Ϸ��ʼ���ϻ���;
				LONG lUserScore = (LONG)pServerUserItem->GetUserScore();

				// ����ʱʵ�����ϻ���;
				LONG lUserGameScore = lUserScore + (m_lOutCardScore[m_wProvideUser][0] + m_lOutCardScore[m_wProvideUser][1] + m_lGangScore[m_wProvideUser]);
#if IS_OPEN_PRINT_LOG
				MyLog("������[%d]ʵ�ʻ���  ===================== UserScore = %ld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
					m_wProvideUser, lUserScore, m_lOutCardScore[m_wProvideUser][0], m_lOutCardScore[m_wProvideUser][1], m_lGangScore[m_wProvideUser]);
#endif
				// �����������
				LONG lAllLoseScore = 0;

				for (WORD i = 1; i < GAME_PLAYER; i++)
				{
					WORD wNext = (m_wProvideUser + GAME_PLAYER - i) % GAME_PLAYER;

					// �����Ƿ���Ʋ��Ҳ��Ǻ�����;
					if ((m_HuPaiResult[wNext].dwHuPaiKind != HU_NULL) && ((m_HuPaiResult[wNext].dwHuPaiKind&HU_BAOZI) == 0))
					{
						// ���˺��Ʒ���;
						LONG lScore = (m_HuPaiResult[wNext].dwHuPaiFanShu + (m_cbHuaPaiCount[wNext][0] + m_cbHuaPaiCount[wNext][1]) * 2 + 20) / lBiXiaHuMultiple;

						//ͳ�����ܺ�����
						if (IsQiangGangHu(wNext))
						{
#if IS_OPEN_PRINT_LOG
							MyLog("���ܺ����[%d] ��ʼ�۷�[%ld]", lScore);
#endif
							GameEnd.stHuPaiResult[wNext].dwHuPaiKind |= HU_QIANGGANGHU;
							lScore = lScore * 3L;		// ���ܺ������۷�;
							bUserBigHu = true;
						}
#if IS_OPEN_PRINT_LOG
						MyLog("�������[%d] ����ӮǮ[%d] dwHuPaiFanShu[%d] Ӳ��[%d] ��[%d] ���º�[%d]",
							wNext, lScore, m_HuPaiResult[wNext].dwHuPaiFanShu, m_cbHuaPaiCount[wNext][0], m_cbHuaPaiCount[wNext][1], lBiXiaHuMultiple);
#endif
						// �����߻��ֲ���;
						if (lUserGameScore < lScore)
						{
							// ʤ���ߵ�ʵ�ʻ���;
							GameEnd.lGameScore[wNext] = lUserGameScore + (m_lOutCardScore[wNext][0] + m_lOutCardScore[wNext][1] + m_lGangScore[wNext]);
							
							// ���Ʒ�;
							GameEnd.lHuScore[wNext] = lUserGameScore;

							if (lUserGameScore > 0)
							{
								lAllLoseScore -= lUserGameScore;
							}

							lUserGameScore = 0;
						}
						else
						{
							// ʤ���ߵ�ʵ�ʻ���;
							GameEnd.lGameScore[wNext] = lScore + (m_lOutCardScore[wNext][0] + m_lOutCardScore[wNext][1] + m_lGangScore[wNext]);

							// ���Ʒ�;
							GameEnd.lHuScore[wNext] = lScore;

							// ʣ�����;
							lUserGameScore -= lScore;
							lAllLoseScore -= lScore;
						}
					}
					else
					{
						// ƽ���ߣ��ܺͷ���ͳ��;
						GameEnd.lGameScore[wNext] = (m_lOutCardScore[wNext][0] + m_lOutCardScore[wNext][1] + m_lGangScore[wNext]);

						// �����ǰ��ӣ���¼�����;
						if ((m_HuPaiResult[wNext].dwHuPaiKind&HU_BAOZI) == HU_BAOZI)
						{
							GameEnd.lBaoZiScore[wNext] = BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// �������˵÷�;
							
							// ��ȡ�����ṩ�ߣ�����ͬһ�ҵ��ṩ��;
							WORD wBaoziProvideUser = GetBaoziProvideUser(wNext);
							
							// ��������ͬһ��;
							if (INVALID_CHAIR == wBaoziProvideUser)
							{
								wBaoziProvideUser = m_wProvideUser;
							}

							GameEnd.lBaoZiScore[wBaoziProvideUser] -= BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// ������˵÷�;
						}
					}
				}

				// ���������ջ���;
				GameEnd.lGameScore[m_wProvideUser] = (m_lOutCardScore[m_wProvideUser][0] + m_lOutCardScore[m_wProvideUser][1] + m_lGangScore[m_wProvideUser]) + lAllLoseScore;

				GameEnd.lHuScore[m_wProvideUser] = lAllLoseScore;
			}

			//��ׯ��ׯ��ֻҪû���Ƶ�����¶�����ׯ�����˰��Ӻ����֣�;
			if (!bBankerHuPai && !m_bHaveBaozi)
			{
				//m_wBankerUser = (m_wBankerUser + GAME_PLAYER - 1) % GAME_PLAYER;
				m_bChangeBanker = true;

				// ����ͳ�Ʊ�����д���ݿ�֮ǰ;
				m_pITableFrame->addPrivatePlayCout(1);
			}

			//������״̬
			m_bHaveBigHu = bUserBigHu;

			// һ�ڶ�������������ͳ�Ʊ��º�;
			if (cbHuPaiCount > 1)
			{
				m_bHaveBigHu = true;
			}
		}
		else
		{
			//��ׯ
			GameEnd.cbGameEndType = 1;

			// ��ܷ�;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				GameEnd.lGameScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
			}
		}

		//����׷�
		LONG lCellScore = m_pITableFrame->GetCellScore();

		//�������
		LONG lUserGameScore[GAME_PLAYER] = { 0L };
		BYTE cbScoreKind[GAME_PLAYER] = { SCORE_TYPE_NULL };
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.lGameScore[i] *= lCellScore;
			lUserGameScore[i] = GameEnd.lGameScore[i];	// ���յ÷�
			if (GameEnd.lGameScore[i] > 0L)
			{
				cbScoreKind[i] = SCORE_TYPE_WIN;
			}
			else if (GameEnd.lGameScore[i] == 0L)
			{
				cbScoreKind[i] = SCORE_TYPE_DRAW;
			}
			else
			{
				cbScoreKind[i] = SCORE_TYPE_LOSE;
			}
		}

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		// ��Ϸ¼��
		addGameRecordAction(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		/**************�������ͳ���ж�********************************************/
#if IS_OPEN_PRINT_LOG
		LONG lEndScore = 0;

		//���ճɼ�
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			ASSERT((pIServerUserItem->GetUserScore() + GameEnd.lGameScore[i]) >= 0);

			lEndScore += GameEnd.lGameScore[i];

			MyLog("name:[%s], ChairID:[%d], dwHuPaiFanShu:[%d],lGameScore:[%ld],m_lGangScore[%ld],m_lOutCardScore0:[%ld],m_lOutCardScore1:[%ld],m_cbHuaPaiCount0:[%d],m_cbHuaPaiCount1:[%d], m_bLastRoundBiXiaHu:[%ld]\n",
				pIServerUserItem->GetNickName(), i, m_HuPaiResult[i].dwHuPaiFanShu, GameEnd.lGameScore[i], m_lGangScore[i], m_lOutCardScore[i][0], m_lOutCardScore[i][1], m_cbHuaPaiCount[i][0], m_cbHuaPaiCount[i][1], lBiXiaHuMultiple);
		}

		ASSERT(lEndScore == 0);
#endif
		/************************************************************************/
		tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

		//ͳ�ƻ���
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (NULL == m_pITableFrame->GetTableUserItem(i)) continue;

			ScoreInfoArray[i].lScore = lUserGameScore[i];
			ScoreInfoArray[i].cbType = cbScoreKind[i];
		}

		//д�����
		DataStream kDataStream;
		m_kGameRecord.StreamValue(kDataStream, true);
		m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray), kDataStream);

		m_bLastRoundBiXiaHu = IsBiXiaHu();

		// ���һ�ֺ��Ʊ��º�,ׯ����ׯ������Ϸ;
		if (m_bLastRoundBiXiaHu)
		{
			tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
			tagPrivateFrameRecordInfo* pPrivateRecord = m_pITableFrame->GetPrivateFrameRecord();

			if (pPrivateFrame->cbGameCout <= pPrivateRecord->cbFinishCout)
			{
				// ͳ����ɾ���;
				m_pITableFrame->addPrivatePlayCout(-1);
				m_bChangeBanker = false;
			}
		}

		/********************************˽�˳�ͳ��****************************************/
		//ͳ���Ƿ�������
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			// Ӯ��
			if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
			{
				// ����
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbZiMoCout[i]++;
				}
				else // ׽��
				{
					m_PrivateEndInfo.cbZhuoPaoCout[i]++;
				}

				// ʤ������
				m_PrivateEndInfo.cbWinCout[i]++;
			}
			else // ���
			{
				// ����
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbDianPaoCout[i]++;
				}

				// ʧ�ܴ���
				m_PrivateEndInfo.cbLoseCout[i]++;
			}

			// ��ߵ÷�
			if (m_PrivateEndInfo.lMaxScore[i] <= GameEnd.lGameScore[i])
			{
				m_PrivateEndInfo.lMaxScore[i] = GameEnd.lGameScore[i];
			}

			// �ܻ���;
			m_PrivateEndInfo.lAllScore[i] += GameEnd.lGameScore[i];
			m_PrivateEndInfo.lBaoZiScore[i] += GameEnd.lBaoZiScore[i];
		}
		/********************************˽�˳�ͳ��****************************************/

		// �Ƿ�ׯ;
		if (m_bChangeBanker)
		{
			m_wBankerUser = (m_wBankerUser + GAME_PLAYER - 1) % GAME_PLAYER;
		}

		//������Ϸ
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		return true;
	}
	default:
		break;
	}

	//�������
	ASSERT(FALSE);

#if IS_OPEN_PRINT_LOG
	MyLog("OnEventGameConclude error wChairID = %d, cbReason = %d end", wChairID, cbReason);
#endif

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
		CMD_S_StatusFree stStatusFree;
		ZeroMemory(&stStatusFree, sizeof(stStatusFree));
		stStatusFree.bCurBiXiaHu = m_bLastRoundBiXiaHu;

		//��ȡ�����ע
		m_lBaseScore = m_pITableFrame->GetCellScore();
		stStatusFree.lBaseScore = m_lBaseScore;
		stStatusFree.lCellScore = m_pGameServiceOption->lCellScore;
		stStatusFree.lMaxCellScore = (LONG)m_pGameServiceOption->lMaxEnterScore;
		stStatusFree.cbSBSStatus = STATUS_SBS_ERROR;

		//���ͳ���
		return m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusFree, sizeof(stStatusFree));
	}
	case GS_MJ_DICE:
	{
		//��ȡ�����ע
		m_lBaseScore = m_pITableFrame->GetCellScore();

		//��������
		CMD_S_StatusDice stStatusDice;
		ZeroMemory(&stStatusDice, sizeof(stStatusDice));
		//��������
		stStatusDice.lBaseScore = m_pITableFrame->GetCellScore();
		stStatusDice.bCurBiXiaHu = m_bLastRoundBiXiaHu;
		stStatusDice.wBankerUser = m_wBankerUser;
		stStatusDice.wEastUser = m_wEastUser;
		//���ͳ���
		return m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusDice, sizeof(stStatusDice));
	}
	case GS_MJ_PLAY:	//��Ϸ״̬
	{
		//��ȡ�����ע
		m_lBaseScore = m_pITableFrame->GetCellScore();

		//��������
		CMD_S_StatusPlay stStatusPlay;
		ZeroMemory(&stStatusPlay, sizeof(stStatusPlay));
		//��������
		stStatusPlay.lBaseScore = m_pITableFrame->GetCellScore();
		stStatusPlay.bCurBiXiaHu = m_bLastRoundBiXiaHu;
		stStatusPlay.wSiceCount = m_wSiceCount;
		stStatusPlay.wBankerUser = m_wBankerUser;
		stStatusPlay.wCurrentUser = m_wCurrentUser;
		stStatusPlay.wEastUser = m_wEastUser;
		CopyMemory(stStatusPlay.bUserTrust, m_bUserTrust, sizeof(m_bUserTrust));
		CopyMemory(stStatusPlay.bUserTing, m_bUserTing, sizeof(m_bUserTing));
		stStatusPlay.lTiaoZhanScore = 0;// m_lTiaoZhanScore[wChiarID];

		//״̬����
		stStatusPlay.cbLeftCardCount = m_cbLeftCardCount;
		//stStatusPlay.cbActionCard = m_cbProvideCard;
		//stStatusPlay.dwActionMask = !m_bResponse[wChiarID] ? m_dwUserAction[wChiarID] : WIK_NULL;

		//��ʷ��¼
		stStatusPlay.wOutCardUser = m_wOutCardUser;
		stStatusPlay.cbOutCardData = m_cbOutCardData;
		CopyMemory(stStatusPlay.cbDiscardCount, m_cbDiscardCount, sizeof(stStatusPlay.cbDiscardCount));
		CopyMemory(stStatusPlay.cbDiscardCard, m_cbDiscardCard, sizeof(stStatusPlay.cbDiscardCard));

		//�����
		CopyMemory(stStatusPlay.cbWeaveCount, m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));
		CopyMemory(stStatusPlay.WeaveItemArray, m_WeaveItemArray, sizeof(m_WeaveItemArray));
		CopyMemory(stStatusPlay.lOutCardScore, m_lOutCardScore, sizeof(m_lOutCardScore));
		CopyMemory(stStatusPlay.lGangScore, m_lGangScore, sizeof(m_lGangScore));

		//������
		stStatusPlay.cbSendCardData = m_bSendCardStatus ? m_cbSendCardData : 0x00;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (i == wChiarID)
			{
				stStatusPlay.cbCardCount[wChiarID] = m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID], stStatusPlay.cbCardData);
			}
			else
			{
				stStatusPlay.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}

			stStatusPlay.cbHuaPaiCount[i] = m_cbHuaPaiCount[i][0];
		}
		CopyMemory(stStatusPlay.cbHuaPaiData, m_cbHuaPaiData, sizeof(m_cbHuaPaiData));

		////��������
		//if ((m_dwUserAction[wChiarID] & WIK_GANG) != 0 && !m_bResponse[wChiarID])
		//{
		//	CopyMemory(&stStatusPlay.GangPaiResult, &m_GangPaiResult[wChiarID], sizeof(tagGangPaiResult));
		//}
		////��������
		//if ((m_dwUserAction[wChiarID] & WIK_TING) != 0 && !m_bResponse[wChiarID])
		//{
		//	CopyMemory(&stStatusPlay.TingPaiResult, &m_TingPaiResult[wChiarID], sizeof(tagTingPaiResult));
		//}
		//CopyMemory(stStatusPlay.SpecialLights, m_SpecialLight, sizeof(m_SpecialLight));

		// �����ָ�;
		/************************************************************************/
#if IS_OPEN_PRINT_LOG
		MyLog("wChiarID = %d, wBankerUser:%d,%d, wCurrentUser:%d,%d, wEastUser:%d,%d.", wChiarID,
			stStatusPlay.wBankerUser, m_wBankerUser,
			stStatusPlay.wCurrentUser, m_wCurrentUser,
			stStatusPlay.wEastUser, m_wEastUser);

		for (BYTE i = 0; i < stStatusPlay.cbCardCount; i++)
		{
			MyLog("CARD_%d = 0x%x", i, stStatusPlay.cbCardData[i]);
		}
		MyLog("cbSendCardData = 0x%x", stStatusPlay.cbSendCardData);
#endif
		/************************************************************************/

		//���ͳ���
		bool bSendGameScene = m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusPlay, sizeof(stStatusPlay));
		
		// ���͸߼�����;
		//����ҷ��ͳ�����������Ϣ
		CMD_S_OperateNotify_Send stOperateNotify;
		ZeroMemory(&stOperateNotify, sizeof(stOperateNotify));

		stOperateNotify.wOperateUser = wChiarID;
		stOperateNotify.dwActionMask = !m_bResponse[wChiarID] ? m_dwUserAction[wChiarID] : WIK_NULL;
		stOperateNotify.wCurrentUser = m_wCurrentUser;
		stOperateNotify.cbActionCard = m_cbProvideCard;

		CopyMemory(&stOperateNotify.GangPaiResult, &m_GangPaiResult[wChiarID], sizeof(tagGangPaiResult));

		DataStream databuff;
		databuff.pushValue(&stOperateNotify, sizeof(CMD_S_OperateNotify_Send));

		//��������
		if ((stOperateNotify.dwActionMask & WIK_TING) != 0)
		{
			tagTingPaiDataStream TingPaiResult(m_TingPaiResult[wChiarID]);
			TingPaiResult.StreamValue(databuff, true);
		}

		m_pITableFrame->SendUserItemData(pIServerUserItem, SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());

		return bSendGameScene;
		//return m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusPlay, sizeof(stStatusPlay));
	}
	}

	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_BASESCORE:
	{
		m_pITableFrame->KillGameTimer(IDI_BASESCORE);
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser)
			{
				if (!m_bAgreeBaseScore[i])
				{
					SendKickOutMsg(i);
				}
			}
		}
	}return true;
	case IDI_SHOOT_DICE:
	{
		OnSubShootDice(m_wCurrentUser);
	}return true;
	case IDI_DISPATCH_CARD:
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (!m_bDispatchCardOver[i])
			{
				OnSubDispatchOver(i);
			}
		}
	}return true;
	case IDI_SEND_CARD:
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (!m_bSendCardOver[i])
			{
				OnSubSendCardOver(i);
			}
		}
	}return true;
	case IDI_SEND_BUHUA:
	{
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (!m_bBuHuaOver[i])
			{
				OnSubBuHuaOver(i);
			}
		}
	}return true;
	case IDI_OUT_CARD:
	{
		//���ƶ�ʱ����
		WORD wOutChairID = static_cast<WORD>(wBindParam);
		OnSubOutCard(wOutChairID, GetOperateCard(wOutChairID));
		//��ʱ������
		BYTE cbTimerStyle = static_cast<BYTE>((wBindParam >> 16) & 0xff);
		if (cbTimerStyle == IDS_TIMER_NORMAL && !m_bUserTrust[wOutChairID])
		{
			OnSubUserTrust(wOutChairID, true);
		}
	}return true;
	case IDI_USER_OPERATE:
	{
		//������ʱ����
		//��ʱ������
		BYTE cbTimerStyle = static_cast<BYTE>((wBindParam >> 16) & 0xff);
		//��������
		if (m_wCurrentUser == INVALID_CHAIR)
		{
			bool bHaveOperate = false;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i == (WORD)wBindParam || m_bResponse[i] || m_dwUserAction[i] == WIK_NULL || bHaveOperate) continue;

				OnSubOperateCard(i, WIK_NULL, m_cbProvideCard, bHaveOperate);
			}
			//��ʱ�й�
			if (cbTimerStyle == IDS_TIMER_NORMAL)
			{
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (IsHaveOperateCard(m_dwUserAction[i]) && !m_bResponse[i] && !m_bUserTrust[i])
					{
						OnSubUserTrust(i, true);
					}
				}
			}
		}
		//��������
		else
		{
			WORD wOperateChairID = (WORD)wBindParam;
			OnSubOutCard(wOperateChairID, GetOperateCard(wOperateChairID));
			//��ʱ�й�
			if (cbTimerStyle == IDS_TIMER_NORMAL && !m_bUserTrust[wOperateChairID])
			{
				OnSubUserTrust(wOperateChairID, true);
			}
		}
	}return true;
	case IDI_USER_OFFLINE + 0:
	{
		WORD wChairID = WORD(wBindParam);
		ASSERT(wChairID == 0);
		//���й�״̬�����ǵ���״̬
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//��ǰ����������Լ�
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//�Լ��в��������й�
					if (IsHaveOperateCard(m_dwUserAction[wChairID]) && !m_bResponse[wChairID] && wChairID != m_wProvideUser)
					{
						OnSubUserTrust(wChairID, true);
						bool bHaveOperate = false;
						OnSubOperateCard(wChairID, WIK_NULL, m_cbProvideCard, bHaveOperate);
					}
				}
			}
		}
		m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 0);
	}return true;
	case IDI_USER_OFFLINE + 1:
	{
		WORD wChairID = WORD(wBindParam);
		ASSERT(wChairID == 1);
		//���й�״̬�����ǵ���״̬
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//��ǰ����������Լ�
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//�Լ��в��������й�
					if (IsHaveOperateCard(m_dwUserAction[wChairID]) && !m_bResponse[wChairID] && wChairID != m_wProvideUser)
					{
						OnSubUserTrust(wChairID, true);
						bool bHaveOperate = false;
						OnSubOperateCard(wChairID, WIK_NULL, m_cbProvideCard, bHaveOperate);
					}
				}
			}
		}
		m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 1);
	}return true;
	case IDI_USER_OFFLINE + 2:
	{
		WORD wChairID = WORD(wBindParam);
		ASSERT(wChairID == 2);
		//���й�״̬�����ǵ���״̬
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//��ǰ����������Լ�
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//�Լ��в��������й�
					if (IsHaveOperateCard(m_dwUserAction[wChairID]) && !m_bResponse[wChairID] && wChairID != m_wProvideUser)
					{
						OnSubUserTrust(wChairID, true);
						bool bHaveOperate = false;
						OnSubOperateCard(wChairID, WIK_NULL, m_cbProvideCard, bHaveOperate);
					}
				}
			}
		}
		m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 2);
	}return true;
	case IDI_USER_OFFLINE + 3:
	{
		WORD wChairID = WORD(wBindParam);
		ASSERT(wChairID == 3);
		//���й�״̬�����ǵ���״̬
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//��ǰ����������Լ�
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//�Լ��в��������й�
					if (IsHaveOperateCard(m_dwUserAction[wChairID]) && !m_bResponse[wChairID] && wChairID != m_wProvideUser)
					{
						OnSubUserTrust(wChairID, true);
						bool bHaveOperate = false;
						OnSubOperateCard(wChairID, WIK_NULL, m_cbProvideCard, bHaveOperate);
					}
				}
			}
		}
		m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + 3);
	}return true;
	}
	ASSERT(FALSE);
	return false;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_SET_BASESCORE:	//������õ�ע
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_SetBaseScore));
		if (wDataSize != sizeof(CMD_C_SetBaseScore)) return false;
		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_LOOKON) return true;

		CMD_C_SetBaseScore *pSetBaseScore = (CMD_C_SetBaseScore *)pDataBuffer;
		ASSERT(pSetBaseScore != NULL);
		return OnSubSetBaseScore(pUserData->wChairID, pSetBaseScore->lBaseScore, pSetBaseScore->cbSBSAction);
	}
	case SUB_C_SHOOT_DICE:		//��������Ϣ
	{
		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubShootDice(pUserData->wChairID);
	}
	case SUB_C_DISPATCH_OVER:	//���������Ϣ
	{
		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubDispatchOver(pUserData->wChairID);
	}
	case SUB_C_SENDCARD_OVER:	//���������Ϣ
	{
		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubSendCardOver(pUserData->wChairID);
	}
	case SUB_C_BUHUA_OVER:		//���������Ϣ
	{
		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		ASSERT(pUserData != NULL && pUserData->cbUserStatus == US_PLAYING);
		if (pUserData == NULL || pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubBuHuaOver(pUserData->wChairID);
	}
	case SUB_C_OUT_CARD:		//������Ϣ
	{
		//Ч����Ϣ
		ASSERT(wDataSize == sizeof(CMD_C_OutCard));
		if (wDataSize != sizeof(CMD_C_OutCard)) return false;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		//��Ϣ����
		CMD_C_OutCard *pOutCard = (CMD_C_OutCard *)pDataBuffer;
		ASSERT(pOutCard != NULL);
		return OnSubOutCard(pUserData->wChairID, pOutCard->cbCardData);
	}
	case SUB_C_OPERATE_CARD:	//������Ϣ
	{
		//Ч����Ϣ
		ASSERT(wDataSize == sizeof(CMD_C_OperateCard));
		if (wDataSize != sizeof(CMD_C_OperateCard)) return false;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		//��Ϣ����
		CMD_C_OperateCard *pOperateCard = (CMD_C_OperateCard *)pDataBuffer;
		ASSERT(pOperateCard != NULL);

		bool bHaveOperate = false;
		OnSubOperateCard(pUserData->wChairID, pOperateCard->dwOperateCode, pOperateCard->cbOperateCard, bHaveOperate);
		return true;
	}
	case SUB_C_USER_TRUST:
	{
		//Ч����Ϣ
		ASSERT(wDataSize == sizeof(CMD_C_UserTrust));
		if (wDataSize != sizeof(CMD_C_UserTrust)) return false;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		CMD_C_UserTrust *pUserTrust = (CMD_C_UserTrust *)pDataBuffer;
		ASSERT(pUserTrust != NULL);
		return OnSubUserTrust(pUserData->wChairID, pUserTrust->bTrust);
	}
#ifdef DEBUG
	case SUB_C_MASTER_CHEAKCARD:
	{
		if (CUserRight::CanGetCard(pIServerUserItem->GetUserRight()))
		{
			CMD_C_MaterCheckCard *pMaterCheckCard = (CMD_C_MaterCheckCard *)pDataBuffer;
			if (wDataSize != sizeof(CMD_C_MaterCheckCard)) return false;

			WORD wChairID = pIServerUserItem->GetChairID();
			m_cbMasterCheckCard[wChairID] = pMaterCheckCard->cbCheakCard;
			return true;
		}

		return false;
	}
	case SUB_C_MASTER_LEFTCARD:
	{
		if (CUserRight::CanGetCard(pIServerUserItem->GetUserRight()))
		{
			WORD wChairID = pIServerUserItem->GetChairID();
			sendMasterLeftCard(wChairID);
			return true;
		}

		return false;
	}
#endif // DEBUG

	default:
		break;
	}

	return false;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�����Ƿ����
bool CTableFrameSink::IsTaskCompleted(WORD wChairID, DWORD dwTaskID, IN OUT DWORD &dwRecord)
{
	return false;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (!bLookonUser)
	{
		//if (IsGenreGold())
		{
			if (find(m_listPlayers.begin(), m_listPlayers.end(), wChairID) == m_listPlayers.end())
			{
				m_listPlayers.push_back(wChairID);
				m_bAgreeBaseScore[wChairID] = false;

				int nUserCount = 0;
				for (int i = 0; i < GAME_PLAYER; ++i)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem)
					{
						++nUserCount;
					}
				}
				if (INVALID_CHAIR == m_wHostChairID && 0 == m_lBaseScore && 1 == nUserCount)
				{
					m_wHostChairID = wChairID;
				}
				else
				{
					if (find(m_listAgreeBaseScore.begin(), m_listAgreeBaseScore.end(), wChairID) == m_listAgreeBaseScore.end())
					{
						m_listAgreeBaseScore.push_back(wChairID);
					}
				}
			}
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (!bLookonUser)
	{
		//���ñ���(ȫ������)
		ResetAllData(true);

		//if (IsGenreGold())
		{
			m_bAgreeBaseScore[wChairID] = false;

			IteListPlayer ite = find(m_listPlayers.begin(), m_listPlayers.end(), wChairID);
			if (ite != m_listPlayers.end())
			{
				m_listPlayers.erase(ite);
				if (0 == m_lBaseScore && wChairID == m_wHostChairID)
				{
					if (m_listPlayers.size() > 0)
					{
						m_wHostChairID = m_listPlayers.front();
						SendSetBaseScore(m_wHostChairID, STATUS_SBS_SET);
						m_pITableFrame->KillGameTimer(IDI_BASESCORE);
						m_pITableFrame->SetGameTimer(IDI_BASESCORE, IDT_BASESCORE * 1000 + 500, 1, 0); //������ʱ��
					}
					else
					{
						m_wHostChairID = INVALID_CHAIR;
					}
				}
				else if (m_listPlayers.empty())
				{
					m_lBaseScore = 0L;
					m_wHostChairID = INVALID_CHAIR;
					ZeroMemory(m_bAgreeBaseScore, sizeof(m_bAgreeBaseScore));
					//m_pITableFrame->SetTableScore(m_pGameServiceOption->lCellScore, m_pGameServiceOption->lLessScore, 0);
				}

				IteListPlayer iteAgree = find(m_listAgreeBaseScore.begin(), m_listAgreeBaseScore.end(), wChairID);
				if (iteAgree != m_listAgreeBaseScore.end())
				{
					m_listAgreeBaseScore.erase(iteAgree);
				}
			}
		}
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(wChairID < GAME_PLAYER);
	if (wChairID >= GAME_PLAYER) return false;

	//����ʱ��ת����ϵͳ�й�
	if (!m_bUserTrust[wChairID] && !m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = true;

		////������ʱ��ʱ��
		//if (m_pGameServiceOption->wServerType != GAME_GENRE_EDUCATE)
		//{
		//	m_pITableFrame->SetGameTimer(IDI_USER_OFFLINE + wChairID, IDT_USER_OFFLINE * 1500, 1, wChairID);
		//}
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(wChairID < GAME_PLAYER);
	if (wChairID >= GAME_PLAYER) return false;

	//�����������
	if (m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = false;
		m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + wChairID);
	}

	return true;
}

//���õ�ע
bool CTableFrameSink::OnSubSetBaseScore(WORD wChairID, LONG lBaseScore, BYTE cbActionType)
{
	//��ͬ���ע
	if (cbActionType == ACTION_SBS_NOSET || cbActionType == ACTION_SBS_DISAGREE)
	{
		SendKickOutMsg(wChairID);
		return true;
	}

	m_bAgreeBaseScore[wChairID] = true;

	if (cbActionType == ACTION_SBS_SET) //���õ�ע
	{
		m_pITableFrame->KillGameTimer(IDI_BASESCORE);
		m_lBaseScore = lBaseScore;
		LONG lMinSitScore = m_lBaseScore*GOLD_RATE;

		ASSERT(wChairID == m_wHostChairID);
		ASSERT(m_lBaseScore != 0);
		ASSERT(lMinSitScore <= m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore());

		//m_pITableFrame->SetTableScore(m_lBaseScore, lMinSitScore, 0);

		CheckMoneyLimit(lMinSitScore);

		if (CheckNotifyBaseScore())
		{
			m_pITableFrame->SetGameTimer(IDI_BASESCORE, IDT_BASESCORE * 1000 + 500, 1, 0); //������ʱ��
		}
	}

	return true;
}

//��ʼ����
bool CTableFrameSink::OnSubShootDice(WORD wChairID)
{
	//��Ϸ״̬У��
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_DICE);
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	VERIFY_RETURN_FALSE(wChairID == m_wCurrentUser);

	//���������Ӷ�ʱ��
	m_pITableFrame->KillGameTimer(IDI_SHOOT_DICE);

	//������Ϸ״̬
	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

	//���������
	m_wSiceCount = MAKEWORD(rand() % 6 + 1, rand() % 6 + 1);

	//������
	m_cbLeftCardCount = MAX_REPERTORY;
	m_GameLogic.RandCardData(m_cbRepertoryCard, MAX_REPERTORY);

	UseCardLibrary();

	//�ַ���
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_cbLeftCardCount -= (MAX_COUNT - 1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount], MAX_COUNT - 1, m_cbCardIndex[i]);
		VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[i]) <= MAX_COUNT);
	}

	//���ͷ�����Ϣ
	CMD_S_GameStart stGameStart;
	ZeroMemory(&stGameStart, sizeof(stGameStart));
	stGameStart.wSiceCount = m_wSiceCount;
	//��������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//���ñ���
		BYTE cbCardCount = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], stGameStart.cbCardData);
		ASSERT(cbCardCount == (MAX_COUNT - 1));

		/************************************************************************/
#if IS_OPEN_PRINT_LOG
		MyLog("wChairID = %d, cbCardCount = %d", i, cbCardCount);
		for (BYTE j = 0; j < cbCardCount; j++)
		{
			MyLog("CARD_%d = 0x%x", j, stGameStart.cbCardData[j]);
		}
#endif
		/************************************************************************/

		//��������
		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &stGameStart, sizeof(stGameStart));
		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &stGameStart, sizeof(stGameStart));
	}

	// ¼��ʼ
	starGameRecord();

	//������ʱ��
	m_pITableFrame->SetGameTimer(IDI_DISPATCH_CARD, (IDT_DISPATCH_CARD + 1) * 1000, 1, 0);
	return true;
}

//�������
bool CTableFrameSink::OnSubDispatchOver(WORD wChairID)
{
	//��Ϸ״̬У��
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//�û�״̬У��
	VERIFY_RETURN_TRUE(!m_bDispatchCardOver[wChairID]);

	//��������
	m_bDispatchCardOver[wChairID] = true;

	//����Ƿ�ȫ���������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bDispatchCardOver[i])
		{
			return true;
		}
	}
	//ɾ�����ƶ�ʱ��
	m_pITableFrame->KillGameTimer(IDI_DISPATCH_CARD);
	//��ׯ�ҷ���14����
	SendCardData(m_wBankerUser, SEND_CARD_FRONT, true);

	return true;
}

//�������
bool CTableFrameSink::OnSubSendCardOver(WORD wChairID)
{
	//��Ϸ״̬У��
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//�û�״̬У��
	VERIFY_RETURN_TRUE(!m_bSendCardOver[wChairID]);

	//��������
	m_bSendCardOver[wChairID] = true;

	//����Ƿ�ȫ�����ƶ������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bSendCardOver[i])
		{
			return true;
		}
	}
	//ɾ�����ƶ�ʱ��
	m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
	//�������
	if (m_GameLogic.IsValidUser(m_wSendCardUser))
	{
		EstimatUserBuHua(m_wSendCardUser);
	}
	return true;
}

// ��ȡҪ��
BYTE CTableFrameSink::getSendCardData(WORD dwSendUser)
{
#define  swapCard(a,b)\
	BYTE nTemp = a;\
	a = b;\
	b = nTemp;\

	BYTE MasterCard = m_cbMasterCheckCard[dwSendUser];
	if (MasterCard == 0)
	{
		return m_cbRepertoryCard[--m_cbLeftCardCount];
	}

	bool bValue = false;
	for (int i = m_cbLeftCardCount - 1; i >= 0; i--)
	{
		if (m_cbRepertoryCard[i] == MasterCard)
		{
			bValue = true;
			swapCard(m_cbRepertoryCard[i], m_cbRepertoryCard[m_cbLeftCardCount - 1]);
			break;
		}
	}

	m_cbMasterCheckCard[dwSendUser] = 0;
	BYTE nCard = m_cbRepertoryCard[--m_cbLeftCardCount];
	if (!bValue)
	{
		//CTraceService::TraceString(TEXT("��ѡ������Ѿ�������"), TraceLevel_Exception);
#if IS_OPEN_PRINT_LOG
		MyLog(_T("���ID: %d, ѡ������Ѿ�������!"), dwSendUser);
		m_pITableFrame->SendGameMessage(m_pITableFrame->GetTableUserItem(dwSendUser), "��ѡ������Ѿ������ڣ�", SMT_CHAT);
#endif
	}

	return nCard;
}

void CTableFrameSink::sendMasterLeftCard(int nChairdID)
{
	MasterLeftCard kMasterLeftCard;
	ZeroMemory(&kMasterLeftCard, sizeof(kMasterLeftCard));
	kMasterLeftCard.kMasterCheakCard = m_cbMasterCheckCard[nChairdID];

	for (int i = 0; i < m_cbLeftCardCount; i++)
	{
		BYTE __ind = m_GameLogic.SwitchToCardIndex(m_cbRepertoryCard[i]);
		kMasterLeftCard.kMasterLeftIndex[__ind]++;
	}

	m_pITableFrame->SendTableData(nChairdID, SUB_S_MASTER_LEFTCARD, &kMasterLeftCard, sizeof(kMasterLeftCard));
}

//����Ҫ�滻����
BYTE CTableFrameSink::getBuHuaCardData(WORD wChairID)
{
	if (wChairID == m_wCheatChairID&&m_cbLeftCheatCardCount>0)
	{
		for (int i = m_cbLeftCardCount - 1; i >= 0; i--)
		{
			if (m_cbRepertoryCard[i] == m_cbCheatCard[m_cbLeftCheatCardCount-1])
			{
				m_cbLeftCheatCardCount--;
				swapCard(m_cbRepertoryCard[i], m_cbRepertoryCard[m_cbLeftCardCount - 1]);
				break;
			}
		}
	}

	return m_cbRepertoryCard[--m_cbLeftCardCount];
}

//�������
bool CTableFrameSink::OnSubBuHuaOver(WORD wChairID)
{
	//��Ϸ״̬У��
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//�û�״̬У��
	VERIFY_RETURN_TRUE(!m_bBuHuaOver[wChairID]);

	//��������
	m_bBuHuaOver[wChairID] = true;

	//����Ƿ�ȫ�������������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bBuHuaOver[i])
		{
			return true;
		}
	}
	//ɾ��������ʱ��
	m_pITableFrame->KillGameTimer(IDI_SEND_BUHUA);
	//�������
	if (m_GameLogic.IsValidUser(m_wBuHuaUser))
	{
		EstimatUserBuHua(m_wBuHuaUser);
	}
	return true;
}

//�û�����
bool CTableFrameSink::OnSubOutCard(WORD wChairID, BYTE cbCardData)
{
	//��Ϸ״̬У��
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);

	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	VERIFY_RETURN_TRUE(wChairID == m_wCurrentUser);
	
	//��������У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidCard(cbCardData));

	// �������ֲ�����ɲſ��Գ���;
	if (!IsFirstBuHuaOver())
	{
#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOutCard error first bu hua no over wChairID = %d cbCardData=0x%x", wChairID, cbCardData);
#endif
		return true;
	}

	if (m_GameLogic.IsHuaCard(cbCardData))
	{
#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOutCard err wChairID = %d cbCardData=0x%x", wChairID, cbCardData);
#endif
		return false;
	}

	//ɾ����
	if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData))
	{
		ASSERT(FALSE);
		return false;
	}
	//������ĿУ��
	BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbHandCardCount > 0 && cbHandCardCount < MAX_COUNT);

	//�������ƶ�ʱ��
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
	m_pITableFrame->KillGameTimer(IDI_USER_OPERATE);

	//���ò�������
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
	ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

	//���ø��û�;
	m_wGangPaiProvideUser = INVALID_CHAIR;

	m_bInvalidHuStates[wChairID] = false;
	m_cbInvalidOperteStates[wChairID] = 0x0;

	//���Ƽ�¼
	m_cbOutCardCount++;
	m_wOutCardUser = wChairID;
	m_cbOutCardData = cbCardData;

	//���뵽��ҳ��Ƽ�¼��
	ASSERT(m_cbDiscardCount[wChairID] + 1 < CountArray(m_cbDiscardCard[wChairID]));
	m_cbDiscardCard[wChairID][m_cbDiscardCount[wChairID]++] = cbCardData;
	m_cbDiscardRecordCard[wChairID][m_cbDiscardRecordCount[wChairID]++] = cbCardData;
	m_cbDiscardIndexCount[wChairID][m_GameLogic.SwitchToCardIndex(cbCardData)]++;

	// ����������ֱ�ӳ��ƣ�ȡ������;
	if (m_bUserTing[wChairID] == false)
	{
		if (m_bUserTianTing[wChairID])
		{
			m_bUserTianTing[wChairID] = false;
		}
	}
	
	//��¼һȦ���Ƶļ�¼
	if (m_cbLastOutCardData != cbCardData)
	{
		m_cbLastOutCardData = cbCardData;
		m_wFirstOutUserForFaFen = wChairID;
		m_cbOutSameCardCount = 1;
	}
	else
	{
		m_cbOutSameCardCount++;
	}

	//���ͳ�����Ϣ
	CMD_S_OutCard stOutCard;
	ZeroMemory(&stOutCard, sizeof(stOutCard));
	stOutCard.wOutCardUser = wChairID;
	stOutCard.cbOutCardData = cbCardData;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OUT_CARD, &stOutCard, sizeof(stOutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OUT_CARD, &stOutCard, sizeof(stOutCard));

	// ��Ϸ¼��
	addGameRecordAction(SUB_S_OUT_CARD, &stOutCard, sizeof(stOutCard));

	//������Ƶ÷�
	CheckOutCardScore(wChairID);

	//�û��л�
	m_wProvideUser = wChairID;
	m_cbProvideCard = cbCardData;
	m_wCurrentUser = (wChairID + GAME_PLAYER - 1) % GAME_PLAYER;
	m_bSendCardStatus = false;
	m_cbGangStatus = GS_NULL;

	//��Ӧ�ж�
	bool bAroseAction = EstimateUserRespond(wChairID, cbCardData, EstimatKind_OutCard);

	//�¼�����
	if (!bAroseAction)
	{
		SendCardData(m_wCurrentUser, SEND_CARD_FRONT);
	}

	return true;
}

//�û�����
bool CTableFrameSink::OnSubOperateCard(WORD wChairID, DWORD dwOperateCode, BYTE cbOperateCard, bool &bHaveOperate)
{
#if IS_OPEN_PRINT_LOG
	MyLog("OnSubOperateCard(%d, %d, %d, %s) begin", wChairID, dwOperateCode, cbOperateCard, BOOL_TO_STRING(bHaveOperate));
#endif

	//��ϷЧ��״̬
	//VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	if (!(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY))
	{ 
		ASSERT(0); 
#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOperateCard error wChairID = %d, GetGameStatus = %d, dwOperateCode = %d, cbOperateCard = 0x%x", wChairID, m_pITableFrame->GetGameStatus(), dwOperateCode, cbOperateCard);
#endif
		return true; 
	}

	//�û�Ч��
	ASSERT((wChairID == m_wCurrentUser) || (m_wCurrentUser == INVALID_CHAIR));
	if ((wChairID != m_wCurrentUser) && (m_wCurrentUser != INVALID_CHAIR))
	{
#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOperateCard error wChairID = %d, m_wCurrentUser = %d", wChairID, m_wCurrentUser);
#endif
		return false;
	}

#if IS_OPEN_PRINT_LOG
	MyLog("OnSubOperateCard m_wCurrentUser=%d", m_wCurrentUser);
#endif

	//��������
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		//Ч��״̬
		ASSERT(!m_bResponse[wChairID]);
		if (m_bResponse[wChairID])
		{
#if IS_OPEN_PRINT_LOG
			MyLog("OnSubOperateCard error m_bResponse[%d] = %s", wChairID, (m_bResponse[wChairID]? "true":"false"));
#endif
			return false;
		}

		ASSERT((dwOperateCode == WIK_NULL) || ((m_dwUserAction[wChairID] & dwOperateCode) != 0));
		if ((dwOperateCode != WIK_NULL) && ((m_dwUserAction[wChairID] & dwOperateCode) == 0))
		{
#if IS_OPEN_PRINT_LOG
			MyLog("OnSubOperateCard error m_dwUserAction[%d] = %d, dwOperateCode = %d", wChairID, m_dwUserAction[wChairID], dwOperateCode);
#endif
			return false;
		}

		// �����ܺ���������
		if (dwOperateCode == WIK_NULL)
		{
			//׽�ڷ����������,�����һ�ֲ����Ժ���;
			if ((m_dwUserAction[wChairID] & WIK_ZHUOPAO) == WIK_ZHUOPAO)
			{
				m_bInvalidHuStates[wChairID] = true;
			}
			else if ((m_dwUserAction[wChairID] & WIK_PENG) != 0)
			{
				m_cbInvalidOperteStates[wChairID] = cbOperateCard;
			}
		}

		//��������
		WORD wTargetUser = wChairID;
		DWORD dwTargetAction = dwOperateCode;

		//���ñ���
		m_bResponse[wChairID] = true;
		m_dwPerformAction[wChairID] = dwOperateCode;
		m_cbOperateCard[wChairID] = (cbOperateCard == 0) ? m_cbProvideCard : cbOperateCard;

		//ִ���ж�
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//��ȡ����
			DWORD dwUserAction = (!m_bResponse[i]) ? m_dwUserAction[i] : m_dwPerformAction[i];

			//���ȼ���
			BYTE cbUserActionRank = m_GameLogic.GetUserActionRank(dwUserAction);
			BYTE cbTargetActionRank = m_GameLogic.GetUserActionRank(dwTargetAction);

			//�����ж�
			if (cbUserActionRank > cbTargetActionRank)
			{
				wTargetUser = i;
				dwTargetAction = dwUserAction;
			}
		}

		//���ȼ��ߵ���һ�û�������ȴ�
		if (!m_bResponse[wTargetUser])
		{
#if IS_OPEN_PRINT_LOG
			MyLog("OnSubOperateCard info m_bResponse[%d] = %s", wTargetUser, (m_bResponse[wTargetUser] ? "true" : "false"));
#endif
			return true;
		}
		
		//���к������û�������ȴ�
		if ((dwTargetAction&WIK_HU)!=0)
		{
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if ((!m_bResponse[i]) && (m_dwUserAction[i] & WIK_HU) != 0)
				{
#if IS_OPEN_PRINT_LOG
					MyLog("OnSubOperateCard info m_bResponse[%d] = %s, m_dwUserAction[%d]=%d", i, (m_bResponse[i] ? "true" : "false"), i, m_dwUserAction[i]);
#endif
					return true;
				}
			}
		}

		bHaveOperate = true;

		//ɾ����ʱ��
		m_pITableFrame->KillGameTimer(IDI_USER_OPERATE);

		//��������
		BYTE cbTargetCard = m_cbOperateCard[wTargetUser];
		//if (dwTargetAction != WIK_HU && dwTargetAction != WIK_NULL)
		//{
		//	//���Ʊ���
		//	m_cbOutCardData = 0;
		//	m_wOutCardUser = INVALID_CHAIR;
		//	m_cbGangStatus = GS_NULL;

		//	//���״̬
		//	ZeroMemory(m_bResponse, sizeof(m_bResponse));
		//	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
		//	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
		//	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
		//}

#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOperateCard wTargetUser=%d dwTargetAction=%d, cbTargetCard=%d", wTargetUser, dwTargetAction, cbTargetCard);
#endif
		switch (dwTargetAction)
		{
			case WIK_PENG:
			{
				return PerformPengPai(wTargetUser, dwTargetAction, cbTargetCard);
			}
			case WIK_MGANG:
			{
				return PerformGangPai(wTargetUser, dwTargetAction, cbTargetCard);
			}
			case WIK_HU:
			{
				return PerformHuPai(wTargetUser, dwTargetAction, cbTargetCard, false);
			}
			case WIK_NULL:
			{
				return PerformGiveUp(wTargetUser, dwTargetAction, cbTargetCard);
			}
			default:
			{
				ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
				MyLog("OnSubOperateCard error m_wCurrentUser=%d dwTargetAction=%d", m_wCurrentUser, dwTargetAction);
#endif
				return false;
			}
		}
	}
	//��������
	else if (m_wCurrentUser == wChairID)
	{
		//Ч��״̬
		ASSERT(!m_bResponse[wChairID]);
		if (m_bResponse[wChairID]) return false;

		m_bResponse[wChairID] = true;

		//����Ч��
		if (dwOperateCode == WIK_NULL || (m_dwUserAction[wChairID] & dwOperateCode) == 0)
		{
			ASSERT(FALSE);

			//���Ʊ���
			m_cbOutCardData = 0;
			m_wOutCardUser = INVALID_CHAIR;
			
			//���״̬
			ZeroMemory(m_bResponse, sizeof(m_bResponse));
			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
			ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
			ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
			return false;
		}

		//if (dwOperateCode != WIK_HU && dwOperateCode != WIK_NULL)
		//{
		//	//���Ʊ���
		//	m_cbOutCardData = 0;
		//	m_wOutCardUser = INVALID_CHAIR;
		//	m_cbGangStatus = GS_NULL;

		//	//���״̬
		//	ZeroMemory(m_bResponse, sizeof(m_bResponse));
		//	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
		//	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
		//	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
		//}
#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOperateCard wChairID=%d dwOperateCode=%d, cbOperateCard=%d", wChairID, dwOperateCode, cbOperateCard);
#endif
		switch (dwOperateCode)
		{
			case WIK_BGANG:
			{
				return PerformGangPai(wChairID, dwOperateCode, cbOperateCard);
			}
			case WIK_AGANG:
			{
				return PerformGangPai(wChairID, dwOperateCode, cbOperateCard);
			}
			case WIK_TING:
			{
				return PerformTingPai(wChairID, dwOperateCode, cbOperateCard);
			}
			case WIK_HU:
			{
				return PerformHuPai(wChairID, dwOperateCode, cbOperateCard, true);
			}
			case WIK_NULL:
			{
				return PerformGiveUp(wChairID, dwOperateCode, cbOperateCard);
			}
			default:
			{
				ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
				MyLog("OnSubOperateCard error m_wCurrentUser=%d dwTargetAction=%d", m_wCurrentUser, dwOperateCode);
#endif
				return false;
			}
		}
	}
	ASSERT(FALSE);

#if IS_OPEN_PRINT_LOG
	MyLog("OnSubOperateCard error end");
#endif

	return false;
}

//�û��й�
bool CTableFrameSink::OnSubUserTrust(WORD wChairID, bool bTrust)
{
	m_bUserTrust[wChairID] = bTrust;
	CMD_S_UserTrust stUserTrust;
	ZeroMemory(&stUserTrust, sizeof(stUserTrust));
	stUserTrust.wChairID = wChairID;
	stUserTrust.bTrust = bTrust;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_TRUST, &stUserTrust, sizeof(stUserTrust));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_USER_TRUST, &stUserTrust, sizeof(stUserTrust));
	return true;
}

//���Ʋ���
bool CTableFrameSink::PerformPengPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformPengPai(%d, %d, %d) begin", wChairID, dwOperateAction, cbOperateCard);
#endif

	//�û�У��
	//VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(m_wProvideUser));
	//VERIFY_RETURN_FALSE(m_wCurrentUser == INVALID_CHAIR);
	//VERIFY_RETURN_FALSE(wChairID != m_wProvideUser);
	if (!m_GameLogic.IsValidUser(wChairID))
	{ 
		ASSERT(0); 
#if IS_OPEN_PRINT_LOG
		MyLog("PerformPengPai error wChairID = %d", wChairID);
#endif
		return false; 
	}

	if (!m_GameLogic.IsValidUser(m_wProvideUser))
	{
		ASSERT(0);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformPengPai error m_wProvideUser = %d", m_wProvideUser);
#endif
		return false;
	}

	if (!(m_wCurrentUser == INVALID_CHAIR))
	{
		ASSERT(0);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformPengPai error m_wCurrentUser = %d", m_wCurrentUser);
#endif
		return false;
	}

	if (!(wChairID != m_wProvideUser))
	{
		ASSERT(0);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformPengPai error wChairID = %d, m_wProvideUser = %d", wChairID, m_wProvideUser);
#endif
		return false;
	}

	//������ĿУ��
	BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbHandCardCount > 0 && cbHandCardCount < MAX_COUNT);

	//ɾ���Լ�������
	BYTE cbRemoveCard[2] = { cbOperateCard, cbOperateCard };
	bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbRemoveCard, CountArray(cbRemoveCard));
	ASSERT(bRemoveCard);
	if (!bRemoveCard)
	{
#if IS_OPEN_PRINT_LOG
		MyLog("PerformPengPai error wChairID = %d, cbRemoveCard[0] = 0x%x, cbRemoveCard[1] = 0x%x", wChairID, cbRemoveCard[0], cbRemoveCard[1]);

		// ��ʾ����;
		for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
		{
			MyLog("PerformPengPai Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
		}
#endif
		return false;
	}

	VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
	m_bSendCardStatus = false;

	//ȡ�����������״̬
	m_bUserTianTing[wChairID] = false;

	//ɾ����������ϴγ�������
	BYTE cbDiscardCount = m_cbDiscardCount[m_wProvideUser]--;
	m_cbDiscardCard[m_wProvideUser][cbDiscardCount] = 0;

	//�õ�ǰ�������Ϊ�Լ�
	m_wCurrentUser = wChairID;

	//���뵽�Լ����������
	WORD wIndex = m_cbWeaveItemCount[wChairID]++;
	m_WeaveItemArray[wChairID][wIndex].cbPublicCard = TRUE;
	m_WeaveItemArray[wChairID][wIndex].cbCenterCard = cbOperateCard;
	m_WeaveItemArray[wChairID][wIndex].dwWeaveKind = dwOperateAction;
	m_WeaveItemArray[wChairID][wIndex].wProvideUser = m_wProvideUser;

	if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
	{
		m_cbHuaPaiCount[wChairID][1]++;
	}

	//������Ҳ�����Ϣ
	CMD_S_OperateResult stOperateResult;
	ZeroMemory(&stOperateResult, sizeof(stOperateResult));
	stOperateResult.wOperateUser = wChairID;
	stOperateResult.cbOperateCard = cbOperateCard;
	stOperateResult.dwOperateCode = dwOperateAction;
	stOperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wChairID : m_wProvideUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	// ��Ϸ¼��
	addGameRecordAction(SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	//�������ƺ͸�������
	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
	ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));
	m_cbProvideCard = 0;

	//���Ʊ���
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;
	m_cbGangStatus = GS_NULL;

	//���״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

	//����������
	EstimateUserRespond(wChairID, 0, EstimatKind_ChiPengCard);
#if IS_OPEN_PRINT_LOG
	MyLog("PerformPengPai end");
#endif
	return true;
}

//���Ʋ���
bool CTableFrameSink::PerformGangPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bCheckQiangGangHu/*=true*/)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformGangPai(%d, %d, %d, %s) begin", wChairID, dwOperateAction, cbOperateCard, BOOL_TO_STRING(bCheckQiangGangHu));
#endif
	//����У��
	if (!m_GameLogic.IsValidUser(wChairID) || (dwOperateAction&WIK_GANG) == 0)
	{
		ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformGangPai error wChairID = %d, dwOperateAction = 0x%x", wChairID, dwOperateAction);
#endif
		return false;
	}

	switch (dwOperateAction)
	{
	case WIK_MGANG:
	{
		//����У��
		if (m_wCurrentUser != INVALID_CHAIR || !m_GameLogic.IsValidUser(m_wProvideUser))
		{
			ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error m_wCurrentUser = %d, m_wProvideUser = %d", m_wCurrentUser, m_wProvideUser);
#endif
			return false;
		}

		//ɾ���Լ�������
		BYTE cbRemoveCard[3] = { cbOperateCard, cbOperateCard, cbOperateCard };
		bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbRemoveCard, CountArray(cbRemoveCard));
		ASSERT(bRemoveCard); 
		if (!bRemoveCard)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error wChairID = %d, cbRemoveCard[0] = 0x%x, cbRemoveCard[1] = 0x%x, cbRemoveCard[2] = 0x%x", 
				wChairID, cbRemoveCard[0], cbRemoveCard[1], cbRemoveCard[2]);

			// ��ʾ����;
			for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
			{
				MyLog("PerformGangPai WIK_MGANG Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
			}
#endif
			return false;
		}

		VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
		m_bSendCardStatus = false;

		//ȡ�����������״̬
		m_bUserTianTing[wChairID] = false;

		//�������ƺ͸�������
		ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
		ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

		//���ø���״̬
		m_cbGangStatus = GS_DAGANGKAIHUA;

		//���ø����ṩ�û�
		m_wGangPaiProvideUser = m_wProvideUser;

		//ɾ����������ϴγ�������
		BYTE cbDiscardCount = m_cbDiscardCount[m_wProvideUser]--;
		m_cbDiscardCard[m_wProvideUser][cbDiscardCount] = 0;

		//���뵽�Լ����������
		WORD wIndex = m_cbWeaveItemCount[wChairID]++;
		m_WeaveItemArray[wChairID][wIndex].cbPublicCard = TRUE;
		m_WeaveItemArray[wChairID][wIndex].cbCenterCard = cbOperateCard;
		m_WeaveItemArray[wChairID][wIndex].dwWeaveKind = dwOperateAction;
		m_WeaveItemArray[wChairID][wIndex].wProvideUser = m_wProvideUser;

		//�ܱ���
		LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

		// �м��ɢ�������һ���Ϊ0�Ͳ��۷�
		// ��ȡ������һ���
		WORD wProvideUser = m_wProvideUser;
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wProvideUser);
		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wProvideUser][0] + m_lOutCardScore[wProvideUser][1] + m_lGangScore[wProvideUser];
		
#if IS_OPEN_PRINT_LOG
		MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ���� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
			wProvideUser, pServerUserItem->GetUserScore(), m_lOutCardScore[wProvideUser][0], m_lOutCardScore[wProvideUser][1], m_lGangScore[wProvideUser]);
#endif

		LONG lGangScore = 0L;
		if (lProvideUserScore >= BASE_HUA_COUNT_MGANG*lPreMultipleCellScore )
		{
			lGangScore = BASE_HUA_COUNT_MGANG*lPreMultipleCellScore;
		}
		else
		{
			lGangScore = lProvideUserScore;
		}

		//�������ܷ���
		m_lGangScore[wChairID] += lGangScore;
		m_lGangScore[m_wProvideUser] -= lGangScore;

		//���͸���С����Ϣ
		CMD_S_PayMoney stPayMoney;
		ZeroMemory(&stPayMoney, sizeof(stPayMoney));
		stPayMoney.cbPayReason = PAY_MONEY_MINGGANG;
		stPayMoney.wPayToUser = wChairID;
		stPayMoney.lPayToNumber += lGangScore;
		stPayMoney.wPayFromUser[0] = m_wProvideUser;
		stPayMoney.lPayFromNumber[0] -= lGangScore;

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

		// ��¼��־
		PayMoneyLog(stPayMoney);

		// ͳ�ƴ���
		m_PrivateEndInfo.cbMingGang[wChairID]++;

		// ���2��
		if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
		{
			m_cbHuaPaiCount[wChairID][1] += 2;
		}
		else
		{// ��ͨ1��
			m_cbHuaPaiCount[wChairID][1] ++;
		}

	}break;
	case WIK_BGANG:
	{
		//������ܺ�
		if (bCheckQiangGangHu && EstimateUserRespond(wChairID, cbOperateCard, EstimatKind_BuGangCard))
		{
			//�������ܺ�״̬
			m_QiangGangHuInfo.wGangPaiUser = wChairID;
			m_QiangGangHuInfo.cbGangPaiCard = cbOperateCard;

#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai QiangGangHu wChairID = %d, cbOperateCard = %d, bCheckQiangGangHu=%s.", wChairID, cbOperateCard, BOOL_TO_STRING(bCheckQiangGangHu));
#endif
			return true;
		}
		else
		{
			//û��������ܺ�
			BYTE cbWeaveIndex = 0xFF;
			for (BYTE i = 0; i < m_cbWeaveItemCount[wChairID]; i++)
			{
				DWORD dwWeaveKind = m_WeaveItemArray[wChairID][i].dwWeaveKind;
				BYTE cbCenterCard = m_WeaveItemArray[wChairID][i].cbCenterCard;
				if ((cbCenterCard == cbOperateCard) && (dwWeaveKind == WIK_PENG))
				{
					cbWeaveIndex = i;
					break;
				}
			}
			//����ʧ��
			ASSERT(cbWeaveIndex != 0xFF);	
			if (cbWeaveIndex == 0xFF)
			{
#if IS_OPEN_PRINT_LOG
				MyLog("PerformGangPai WIK_BGANG error not find wProvideUser wChairID = %d, cbRemoveCard[0] = 0x%x", wChairID, cbOperateCard);
#endif
				return false;
			}

			//ɾ���Լ�������
			bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbOperateCard);
			ASSERT(bRemoveCard); //if (!bRemoveCard) return false;
			if (!bRemoveCard)
			{
#if IS_OPEN_PRINT_LOG
				MyLog("PerformGangPai WIK_BGANG error wChairID = %d, cbRemoveCard[0] = 0x%x", wChairID, cbOperateCard);

				// ��ʾ����;
				for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
				{
					MyLog("PerformGangPai WIK_BGANG Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
				}
#endif
				return false;
			}

			VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
			m_bSendCardStatus = false;

			//ȡ�����������״̬
			m_bUserTianTing[wChairID] = false;

			//�������ƺ͸�������
			ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
			ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

			//�����Լ������
			m_WeaveItemArray[wChairID][cbWeaveIndex].dwWeaveKind = dwOperateAction;

			//�ܱ���
			LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

			// ��Ǯ���;
			WORD wProvideUser = INVALID_CHAIR;

			//�������ƵĹ������A���ܣ�����������ܣ����B�ĵ��ƣ��������ƣ������ƿ��԰��ܣ����ܺ����B�����ҵ�Ǯ�������ƺ�ʱ��Ҳȫ�������B����;
			if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) && */(m_wGangPaiProvideUser != INVALID_CHAIR) && (m_wGangPaiProvideUser != wChairID))
			{
				// �ṩһ���ܵ����;
				wProvideUser = m_wGangPaiProvideUser;
			}
			else
			{
				//���ø���״̬
				m_cbGangStatus = GS_DAGANGKAIHUA;	// ���ܸ�ֵ;

				//���ø����ṩ�û�
				if (m_wGangPaiProvideUser == INVALID_CHAIR)
				{
					m_wGangPaiProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
				}

				// �����ṩ��;
				wProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
			}

			LONG lGangScore = 0L;

			// �м��ɢ�������һ���Ϊ0�Ͳ��۷�
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wProvideUser);
			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wProvideUser][0] + m_lOutCardScore[wProvideUser][1] + m_lGangScore[wProvideUser];
#if IS_OPEN_PRINT_LOG
			MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ���� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
				wProvideUser, pServerUserItem->GetUserScore(), m_lOutCardScore[wProvideUser][0], m_lOutCardScore[wProvideUser][1], m_lGangScore[wProvideUser]);
#endif
			if (lProvideUserScore >= BASE_HUA_COUNT_BGANG*lPreMultipleCellScore)
			{
				lGangScore = BASE_HUA_COUNT_BGANG*lPreMultipleCellScore;
			}
			else
			{
				lGangScore = lProvideUserScore;
			}


			//���㲹�ܸܷ���
			m_lGangScore[wChairID] += lGangScore;
			m_lGangScore[wProvideUser] -= lGangScore;

			//���͸���С����Ϣ
			CMD_S_PayMoney stPayMoney;
			ZeroMemory(&stPayMoney, sizeof(stPayMoney));
			stPayMoney.cbPayReason = PAY_MONEY_MINGGANG;
			stPayMoney.wPayToUser = wChairID;
			stPayMoney.lPayToNumber += lGangScore;
			stPayMoney.wPayFromUser[0] = wProvideUser;
			stPayMoney.lPayFromNumber[0] -= lGangScore;

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

			// ��¼��־
			PayMoneyLog(stPayMoney);

			// ͳ�ƴ���
			m_PrivateEndInfo.cbMingGang[wChairID]++;

			// ���2��(����ʱ���Ѿ�����һ�仨;);
			if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
			{
				m_cbHuaPaiCount[wChairID][1] += 1;
			}
			else
			{// ��ͨ1��
				m_cbHuaPaiCount[wChairID][1] ++;
			}
		}
	}break;
	case WIK_AGANG:
	{
		//����У��
		ASSERT(wChairID == m_wProvideUser);
		if (wChairID != m_wProvideUser)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error WIK_AGANG wChairID = %d, m_wProvideUser = %d", wChairID, m_wProvideUser);
#endif
			return false;
		}

		//ɾ���Լ�������
		BYTE cbRemoveCard[4] = { cbOperateCard, cbOperateCard, cbOperateCard, cbOperateCard };
		bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbRemoveCard, CountArray(cbRemoveCard));
		ASSERT(bRemoveCard); //if (!bRemoveCard) return false;

		if (!bRemoveCard)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error WIK_AGANG wChairID = %d, cbRemoveCard[0] = 0x%x, cbRemoveCard[1] = 0x%x, cbRemoveCard[2] = 0x%x, cbRemoveCard[3] = 0x%x",
				wChairID, cbRemoveCard[0], cbRemoveCard[1], cbRemoveCard[2], cbRemoveCard[3]);

			// ��ʾ����;
			for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
			{
				MyLog("PerformGangPai WIK_AGANG Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
			}
#endif

			return false;
		}

		VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
		m_bSendCardStatus = false;

		//ȡ�����������״̬
		m_bUserTianTing[wChairID] = false;

		//�������ƺ͸�������
		ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
		ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

		//���뵽�Լ����������
		WORD wIndex = m_cbWeaveItemCount[wChairID]++;
		m_WeaveItemArray[wChairID][wIndex].cbPublicCard = FALSE;
		m_WeaveItemArray[wChairID][wIndex].wProvideUser = wChairID;
		m_WeaveItemArray[wChairID][wIndex].dwWeaveKind = dwOperateAction;
		m_WeaveItemArray[wChairID][wIndex].cbCenterCard = cbOperateCard;
		
		//�ܱ���
		LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

		//���͸���С����Ϣ
		CMD_S_PayMoney stPayMoney;
		ZeroMemory(&stPayMoney, sizeof(stPayMoney));
		stPayMoney.cbPayReason = PAY_MONEY_ANGANG;
		stPayMoney.wPayToUser = wChairID;
		
		LONG lAllGangScore = 0;
		BYTE cbFromChairID = 0;

		LONG lGangScore = BASE_HUA_COUNT_AGANG*lPreMultipleCellScore;

		//�������ƵĹ������A���ܣ�����������ܣ����B�ĵ��ƣ��������ƣ������ƿ��԰��ܣ����ܺ����B�����ҵ�Ǯ�������ƺ�ʱ��Ҳȫ�������B����;
		if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) &&*/ (m_wGangPaiProvideUser != INVALID_CHAIR) && (m_wGangPaiProvideUser != wChairID))
		{
			// ����Ǯһ�Ҹ�;
			lGangScore = lGangScore * 3L;

			//���㰵�ܷ���
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wGangPaiProvideUser);
			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser];
			
#if IS_OPEN_PRINT_LOG
			MyLog("��Ǯ��[%d]ʵ�ʻ��� ����Ǯһ�Ҹ� ===================== ���� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
				m_wGangPaiProvideUser, pServerUserItem->GetUserScore(), m_lOutCardScore[m_wGangPaiProvideUser][0], m_lOutCardScore[m_wGangPaiProvideUser][1], m_lGangScore[m_wGangPaiProvideUser]);
#endif

			LONG lTmpGangScore = 0;
			if (lProvideUserScore >= lGangScore)
			{
				lTmpGangScore = lGangScore;
			}
			else
			{
				lTmpGangScore = lProvideUserScore;
			}

			//�������ܷ���
			lAllGangScore += lTmpGangScore;
			m_lGangScore[m_wGangPaiProvideUser] -= lTmpGangScore;

			stPayMoney.wPayFromUser[cbFromChairID] = m_wGangPaiProvideUser;
			stPayMoney.lPayFromNumber[cbFromChairID++] -= lTmpGangScore;
		}
		else
		{
			//���ø���״̬
			m_cbGangStatus = GS_DAGANGKAIHUA;

			//���㰵�ܷ���
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i == wChairID) continue;

				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
#if IS_OPEN_PRINT_LOG
				MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ���� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]", 
					i, pServerUserItem->GetUserScore(), m_lOutCardScore[i][0], m_lOutCardScore[i][1], m_lGangScore[i]);
#endif
				LONG lTmpGangScore = 0;
				if (lProvideUserScore >= lGangScore)
				{
					//�������ܷ���
					lTmpGangScore = lGangScore;
				}
				else
				{
					lTmpGangScore = lProvideUserScore;
				}

				//�������ܷ���
				lAllGangScore += lTmpGangScore;
				m_lGangScore[i] -= lTmpGangScore;

				stPayMoney.wPayFromUser[cbFromChairID] = i;
				stPayMoney.lPayFromNumber[cbFromChairID++] -= lTmpGangScore;
			}
		}

		m_lGangScore[wChairID] += lAllGangScore;
		stPayMoney.lPayToNumber += lAllGangScore;

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

		// ��¼��־
		PayMoneyLog(stPayMoney);

		m_PrivateEndInfo.cbAnGang[wChairID]++;

		// ���2��
		if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
		{
			m_cbHuaPaiCount[wChairID][1] += 3;
		}
		else
		{// ��ͨ1��
			m_cbHuaPaiCount[wChairID][1] ++;
		}

	}break;
	default:
	{
		ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformGangPai error wChairID = %d dwOperateAction = %d", wChairID, dwOperateAction);
#endif
		return false;
	}
	}

	CMD_S_OperateResult stOperateResult;
	ZeroMemory(&stOperateResult, sizeof(stOperateResult));

	//������Ҳ�����Ϣ
	stOperateResult.wOperateUser = wChairID;
	stOperateResult.cbOperateCard = cbOperateCard;
	stOperateResult.dwOperateCode = dwOperateAction;
	stOperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wChairID : m_wProvideUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	// ��Ϸ¼��
	stOperateResult.cbOperateCard = cbOperateCard;
	addGameRecordAction(SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	//���Ʊ���
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;
	//m_cbGangStatus = GS_NULL;

	//���״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

	//�������ܺ�״̬
	m_QiangGangHuInfo.ReInit();

	m_cbProvideCard = 0;
	m_wCurrentUser = wChairID;

	//�����޻����ƾֽ���;
	BYTE cbNoScore = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == nullptr) continue;

		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];

#if IS_OPEN_PRINT_LOG
		MyLog("gang end check score:[%s]=%ld", pServerUserItem->GetNickName(), lProvideUserScore);
#endif
		if (lProvideUserScore <= 0)
		{
			cbNoScore++;
		}
	}

	if (cbNoScore >= 2)
	{
		return OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
	}
	else
	{
		SendCardData(wChairID, SEND_CARD_BACK);
	}
	
	////�Լ���������
	//SendCardData(wChairID, SEND_CARD_BACK);
#if IS_OPEN_PRINT_LOG
	MyLog("PerformGangPai(%d, %d, %d, %s) end", wChairID, dwOperateAction, cbOperateCard, BOOL_TO_STRING(bCheckQiangGangHu));
#endif

	return true;
}

//���Ʋ���
bool CTableFrameSink::PerformTingPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformTingPai(%d, %d, %d) begin", wChairID, dwOperateAction, cbOperateCard);
#endif

	//����У��
	if (!m_GameLogic.IsValidUser(wChairID) || (dwOperateAction&WIK_TING) == 0)
	{
		ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformTingPai error wChairID = %d, dwOperateAction = 0x%x", wChairID, dwOperateAction);
#endif
		return false;
	}

	//�����ж�
	ASSERT(m_TingPaiResult[wChairID].cbTingPaiItemCount > 0);
	if (m_TingPaiResult[wChairID].cbTingPaiItemCount == 0)
	{
#if IS_OPEN_PRINT_LOG
		MyLog("PerformTingPai error wChairID = %d, cbTingPaiItemCount = %d", wChairID, m_TingPaiResult[wChairID].cbTingPaiItemCount);
#endif
		return false;
	}

	//����״̬�ж�
	ASSERT(!m_bUserTing[wChairID]);
	if (m_bUserTing[wChairID])
	{
#if IS_OPEN_PRINT_LOG
		MyLog("PerformTingPai m_bUserTing[%d]=%s", wChairID, BOOL_TO_STRING(m_bUserTing[wChairID]));
#endif
		return true;
	}

	//���ø����ṩ��
	m_wGangPaiProvideUser = INVALID_CHAIR;

	//��¼�������������
	for (BYTE i = 0; i < m_TingPaiResult[wChairID].cbTingPaiItemCount; i++)
	{
		//������ȥ�����Ǽ��ɳ�����
		if (m_TingPaiResult[wChairID].TingPaiItemArrary[i].cbOutCardData == cbOperateCard)
		{
			//�������ƺ͸�������
			ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
			ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

			//��������״̬
			m_bUserTing[wChairID] = true;

			//������Ҳ�����Ϣ
			CMD_S_OperateResult stOperateResult;
			ZeroMemory(&stOperateResult, sizeof(stOperateResult));
			stOperateResult.wOperateUser = wChairID;
			stOperateResult.cbOperateCard = cbOperateCard;
			stOperateResult.dwOperateCode = dwOperateAction;
			stOperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wChairID : m_wProvideUser;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

			// ��Ϸ¼��
			addGameRecordAction(SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

			//��ҳ���
			OnSubOutCard(wChairID, cbOperateCard);

#if IS_OPEN_PRINT_LOG
			MyLog("PerformTingPai end.");
#endif
			return true;
		}
	}
	ASSERT(FALSE);
	
#if IS_OPEN_PRINT_LOG
	MyLog("PerformTingPai error cbOperateCard = 0x%x not find end.", cbOperateCard);
#endif

	return false;
}

//���Ʋ���
bool CTableFrameSink::PerformHuPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bZiMo)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformHuPai(%d, %d, %d, %s) begin", wChairID, dwOperateAction, cbOperateCard, BOOL_TO_STRING(bZiMo));
#endif

	//������Ϣ
	m_cbHuPaiCard = cbOperateCard;

	BYTE cbHuUserCout = 0;	// ��������;

	// ����
	if (bZiMo)
	{
		//�û�У��
		VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
//		if (!m_GameLogic.IsValidUser(wChairID))
//		{
//			ASSERT(0);
//#if IS_OPEN_PRINT_LOG
//			MyLog("PerformHuPai error wChairID = %d", wChairID);
//#endif
//			return false;
//		}

		//����У��
		VERIFY_RETURN_FALSE((dwOperateAction&WIK_HU) != 0);
		//if (!((dwOperateAction&WIK_HU) != 0))
		//{
		//	ASSERT(0);
		//	MyLog("PerformHuPai error dwOperateAction = %d", dwOperateAction);
		//	return false;
		//}

		//��֤����
		DWORD dwPreHuPaiKind = GetPreHuPaiKind(wChairID, m_cbHuPaiCard);
		DWORD dwHuPaiKind = m_GameLogic.AnalyseHuPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], m_cbHuPaiCard, m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, m_HuPaiResult[wChairID], bZiMo);

		// ���;
		if (dwHuPaiKind != WIK_NULL)
		{
			if ((m_HuPaiResult[wChairID].dwHuPaiKind&HU_ONLYONE) != 0 && (m_HuPaiResult[wChairID].dwHuPaiKind&HU_YAJUE) == 0)
			{
				m_cbHuaPaiCount[wChairID][1]++;
			}

			//����Ƿ�ȱһ��
			if (IsQueYiMen(wChairID, m_cbCardIndex[wChairID]))
			{
				m_cbHuaPaiCount[wChairID][1]++;
			}

			// ����;
			if (dwHuPaiKind != WIK_NULL)
			{
				cbHuUserCout++;
			}

			// �����ԶԺ�������;������һɫ������;
			if ((m_HuPaiResult[wChairID].dwHuPaiKind&HU_DUIDUIHU) != 0 || (m_HuPaiResult[wChairID].dwHuPaiKind&HU_QINGYISE) != 0)
			{
				if (IsSanPeng(wChairID))
				{
					m_HuPaiResult[wChairID].dwHuPaiKind |= HU_BAOZI;

					return OnEventGameConclude(wChairID, NULL, GER_BAOZI);
				}
			}
		}
		else// �ٺ����ɰ���(���Ӻ���ͨ����ͬʱ���֣����Ȱ���);
		{
			// ���������ܺ�����;
			if (IsGameRuleBaoZi())
			{
				DWORD dwUserAction = WIK_NULL;
				tagGangPaiResult GangPaiResult;
				dwUserAction |= m_GameLogic.AnalysePengPai(m_cbCardIndex[wChairID], cbOperateCard);
				dwUserAction |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], cbOperateCard, GangPaiResult, true);

				// ���Ӱ��Ӻ��Ƽ��(��������ܣ��ж����ܺ��Ƿ�ɰ��ӣ��������ֱ����ʾ��Һ���);
				if ((dwUserAction & WIK_AGANG) != 0)
				{
					// �Ѿ��Ƕ���ͬһ����������ˣ��ڶ�����һ��һ�죬���������;
					if (IsSanPeng(wChairID))
					{
						m_HuPaiResult[wChairID].dwHuPaiKind |= HU_BAOZI;
						return OnEventGameConclude(wChairID, NULL, GER_BAOZI);
					}
				}
			}
		}
	}
	else
	{
		ASSERT(m_cbHuaPaiCount != 0);

		//ͳ�����ܺ�����
		if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR && m_QiangGangHuInfo.cbGangPaiCard != 0)
		{
			//ASSERT(m_QiangGangHuInfo.cbGangPaiCard == m_cbHuPaiCard);
			if (m_QiangGangHuInfo.cbGangPaiCard != m_cbHuPaiCard)
			{
				m_cbHuPaiCard = m_QiangGangHuInfo.cbGangPaiCard;
			}
		}

		//�����ж�
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//�����ж�
			if ((m_dwPerformAction[i] & WIK_HU) == 0)
			{
				continue;
			}

			// �ٺ����ɰ���(���Ӻ���ͨ����ͬʱ���֣����Ȱ���);
			if (AnalyseBaoZi(i, m_wProvideUser, m_cbHuPaiCard))
			{
				m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbHuPaiCard)]++;

				m_HuPaiResult[i].dwHuPaiKind |= HU_BAOZI;
			}
			else
			{
				//��֤����
				DWORD dwPreHuPaiKind = GetPreHuPaiKind(i, m_cbHuPaiCard);

				//���Ƽ��
				DWORD dwHuPaiKind = m_GameLogic.AnalyseHuPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], m_cbHuPaiCard, m_cbHuaPaiCount[i][0], dwPreHuPaiKind, m_HuPaiResult[i], bZiMo);
				ASSERT(dwHuPaiKind != WIK_NULL);

				//���ƣ������һ���Ʋ���
				if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbHuPaiCard)]++;

					//������
					if ((m_HuPaiResult[i].dwHuPaiKind&HU_ONLYONE) != 0 && (m_HuPaiResult[i].dwHuPaiKind&HU_YAJUE) == 0)
					{
						m_cbHuaPaiCount[i][1]++;
					}

					//����Ƿ�ȱһ��
					if (IsQueYiMen(i, m_cbCardIndex[i]))
					{
						m_cbHuaPaiCount[i][1]++;
					}

					// �ԶԺ������ж�����ͬһ�ҶԶԺ������;
					if ((m_HuPaiResult[i].dwHuPaiKind&HU_DUIDUIHU) != 0)
					{
						if (IsSanPeng(i))
						{
							m_HuPaiResult[i].dwHuPaiKind |= HU_BAOZI;
						}
					}
					
					// ��һɫ�����ж�;
					if (IsGameRuleBaoZi() && (m_HuPaiResult[i].dwHuPaiKind&HU_QINGYISE) != 0)
					{
						if (m_cbWeaveItemCount[i] >= MAX_WEAVE - 1)
						{
							m_HuPaiResult[i].dwHuPaiKind |= HU_BAOZI;
						}
					}

					cbHuUserCout++;		// ��������ͳ��;

				}
			}
			
			ASSERT(m_HuPaiResult[i].dwHuPaiKind != HU_NULL);
		}
	}

#if IS_OPEN_PRINT_LOG
	// δ�ҵ��������;
	if (cbHuUserCout <= 0)
	{
		MyLog("HU wChairID = %d, m_cbHuPaiCard = 0x%x, cbOperateCard = 0x%x, bZiMo = %s", wChairID, m_cbHuPaiCard, cbOperateCard, BOOL_TO_STRING(bZiMo));

		//ͳ�����ܺ�����
		if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR && m_QiangGangHuInfo.cbGangPaiCard != 0)
		{
			MyLog("���ܺ� wGangPaiUser= %d, cbGangPaiCard = %d", m_QiangGangHuInfo.wGangPaiUser, m_QiangGangHuInfo.cbGangPaiCard);
		}
	}

	MyLog("PerformHuPai end");
#endif

	//������Ϸ
	return OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
}

//��������
bool CTableFrameSink::PerformGiveUp(WORD wChairID, DWORD dwOperateCode, BYTE cbOperateCard)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformGiveUp(%d, %d, %d) begin.", wChairID, dwOperateCode, cbOperateCard);
#endif

	//����У��
	if (!m_GameLogic.IsValidUser(wChairID) || dwOperateCode != WIK_NULL)
	{
		ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformGiveUp error wChairID[%d], dwOperateCode[0x%x]", wChairID, dwOperateCode);
#endif
		return false;
	}

#if IS_OPEN_PRINT_LOG
	MyLog("wChairID[%d], m_dwUserAction[%d]", wChairID, m_dwUserAction[wChairID]);
#endif

	//׽�ڷ����������,�����һ�ֲ����Ժ���;
	if ((m_dwUserAction[wChairID] & WIK_ZHUOPAO) == WIK_ZHUOPAO)
	{
		m_bInvalidHuStates[wChairID] = true;
	}
	else if ((m_dwUserAction[wChairID] & WIK_PENG) != 0)
	{
		m_cbInvalidOperteStates[wChairID] = cbOperateCard;
	}

#if IS_OPEN_PRINT_LOG
	MyLog("PerformGiveUp m_wCurrentUser = %d.", m_wCurrentUser);
#endif

	if (INVALID_CHAIR == m_wCurrentUser)
	{
		//�����ܺ���ҷ������ƣ�������Ҽ�������
		if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGiveUp wGangPaiUser=%d, cbGangPaiCard=0x%x.", m_QiangGangHuInfo.wGangPaiUser, m_QiangGangHuInfo.cbGangPaiCard);
#endif
			PerformGangPai(m_QiangGangHuInfo.wGangPaiUser, WIK_BGANG, m_QiangGangHuInfo.cbGangPaiCard, false);
		}
		else
		{
			//���Ʊ���
			m_cbOutCardData = 0;
			m_wOutCardUser = INVALID_CHAIR;
			m_cbGangStatus = GS_NULL;

			//���״̬
			ZeroMemory(m_bResponse, sizeof(m_bResponse));
			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
			ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
			ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

#if IS_OPEN_PRINT_LOG
			MyLog("PerformGiveUp SendCardData");
#endif
			SendCardData(m_wResumeUser, SEND_CARD_FRONT);
		}
	}
	else
	{
		//���Ʊ���
		m_cbOutCardData = 0;
		m_wOutCardUser = INVALID_CHAIR;
		m_cbGangStatus = GS_NULL;

		//���״̬
		ZeroMemory(m_bResponse, sizeof(m_bResponse));
		ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
		ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
		ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
	}

#if IS_OPEN_PRINT_LOG
	MyLog("PerformGiveUp m_wCurrentUser = %d end.", m_wCurrentUser);
#endif

	return true;
}

//�������
bool CTableFrameSink::SendCardData(WORD wChairID, BYTE cbDirection, bool bBanker14/*=false*/)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	
	//��ׯ���
	if (m_cbLeftCardCount <= KEEP_LEFT_CARD_COUNT)
	{
		m_cbHuPaiCard = 0;
		m_wProvideUser = INVALID_CHAIR;
		return OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
	}

	//���ó��Ʊ���
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;

	m_bSendCardStatus = true;

	//��ѹ�����������
	m_cbSendCardCount++;

//#ifdef DEBUG
	m_cbSendCardData = getSendCardData(wChairID);
//#else
//	m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
//#endif // DEBUG
	
	m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//���������ĿУ��
	BYTE cbCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbCardCount > 0 && cbCardCount <= MAX_COUNT && cbCardCount % 3 == 2);

	//���õ�ǰ��ֵ
	m_wCurrentUser = wChairID;
	m_wProvideUser = wChairID;
	m_cbProvideCard = m_cbSendCardData;
	m_wSendCardUser = wChairID;
	ZeroMemory(m_bSendCardOver, sizeof(m_bSendCardOver));

	//���ͷ�����Ϣ
	CMD_S_SendCard stSendCard;
	ZeroMemory(&stSendCard, sizeof(stSendCard));
	stSendCard.wSendCardUser = wChairID;

	stSendCard.cbDirection = cbDirection;

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (i == wChairID)
		{
			stSendCard.cbCardData = m_cbSendCardData;
		}
		else
		{
			stSendCard.cbCardData = 0x0;
		}
		
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &stSendCard, sizeof(stSendCard));
	}

	stSendCard.cbCardData = 0x0;	// �Թ۷�
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &stSendCard, sizeof(stSendCard));

#if IS_OPEN_PRINT_LOG
	MyLog("wChairID = %d, cbSendCardData = %d", wChairID, m_cbSendCardData);
#endif

	// ��Ϸ¼��
	stSendCard.cbCardData = m_cbSendCardData;
	addGameRecordAction(SUB_S_SEND_CARD, &stSendCard, sizeof(stSendCard));

	//�������ƶ�����ʱ��
	m_pITableFrame->SetGameTimer(IDI_SEND_CARD, IDT_SEND_CARD * 1000, 1, wChairID);
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem != NULL && pServerUserItem->IsAndroidUser())
		{
			OnSubSendCardOver(i);
		}
	}
	return true;
}

//���Ͳ���
bool CTableFrameSink::SendOperateNotify(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	bool bUserHaveOperate = false;
	//������ʾ
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_dwUserAction[i] != WIK_NULL)
		{
			//����ҷ��ͳ�����������Ϣ
			CMD_S_OperateNotify_Send stOperateNotify;
			ZeroMemory(&stOperateNotify, sizeof(stOperateNotify));
			stOperateNotify.wOperateUser = i;
			stOperateNotify.dwActionMask = m_dwUserAction[i];
			stOperateNotify.wCurrentUser = m_wCurrentUser;
			//stOperateNotify.lTiaoZhanScore = 0;// m_lTiaoZhanScore[i];
			switch (EstimatKind)
			{
			case EstimatKind_SendCard:
			case EstimatKind_OutCard:
			case EstimatKind_GangCard:
			{
				stOperateNotify.cbActionCard = cbCenterCard;
			}break;
			case EstimatKind_ChiPengCard:
			{
				stOperateNotify.cbActionCard = 0;
			}break;
			}
			CopyMemory(&stOperateNotify.GangPaiResult, &m_GangPaiResult[i], sizeof(tagGangPaiResult));
			//CopyMemory(&stOperateNotify.TingPaiResult, &m_TingPaiResult[i], sizeof(tagTingPaiResult));

			DataStream databuff;
			databuff.pushValue(&stOperateNotify, sizeof(CMD_S_OperateNotify_Send));

			//��������
			if ((stOperateNotify.dwActionMask & WIK_TING) != 0)
			{
				//m_TingPaiResult[i].StreamValue(dataStream, true);
				tagTingPaiDataStream TingPaiResult(m_TingPaiResult[i]);
				TingPaiResult.StreamValue(databuff, true);
			}

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());

			//��������
			//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &stOperateNotify, sizeof(stOperateNotify));
			//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &stOperateNotify, sizeof(stOperateNotify));
			bUserHaveOperate = IsHaveOperateCard(m_dwUserAction[i]);

#if IS_OPEN_PRINT_LOG
			MyLog("SendOperateNotify info wOperateUser=%d, dwActionMask=%d, wCurrentUser=%d", stOperateNotify.wOperateUser, stOperateNotify.dwActionMask, stOperateNotify.wCurrentUser);
#endif
			// ��Ϸ¼��
			addGameRecordAction(SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());
		}
	}

	if (bUserHaveOperate)
	{
		BYTE cbTimerStyle = IDS_TIMER_NORMAL; //����״̬
		BYTE cbTimerLength = IDT_USER_OPERATE;
		if (m_GameLogic.IsValidUser(m_wCurrentUser))
		{
			ASSERT(m_wCurrentUser == wChairID);
			//�й�״̬
			if (m_bUserTrust[m_wCurrentUser])
			{
				cbTimerStyle = IDS_TIMER_TRUST;
				cbTimerLength = IDT_USER_TRUST;
			}

			//����˽�˳�;
			if (m_pGameServiceOption->wServerType != GAME_GENRE_EDUCATE && m_pGameServiceOption->wServerType != GAME_GENRE_PRIVATE)
			{
				m_pITableFrame->SetGameTimer(IDI_OUT_CARD, cbTimerLength * 1000, 1, (cbTimerStyle << 16) | m_wCurrentUser);
			}
		}
		else
		{
			cbTimerStyle = IDS_TIMER_TRUST;
			cbTimerLength = IDT_USER_TRUST;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (IsHaveOperateCard(m_dwUserAction[i]) && !m_bUserTrust[i])
				{
					cbTimerStyle = IDS_TIMER_NORMAL;
					cbTimerLength = IDT_USER_OPERATE;
					break;
				}
			}
			if (m_pGameServiceOption->wServerType != GAME_GENRE_EDUCATE && m_pGameServiceOption->wServerType != GAME_GENRE_PRIVATE)
			{
				m_pITableFrame->SetGameTimer(IDI_USER_OPERATE, cbTimerLength * 1000, 1, (cbTimerStyle << 16) | m_wProvideUser);
			}
		}
	}
	else
	{
		//�ж���ǰ�û��Ϸ�
		ASSERT(m_GameLogic.IsValidUser(m_wCurrentUser) && m_wCurrentUser == wChairID);
		BYTE cbTimerStyle = IDS_TIMER_NORMAL;
		BYTE cbTimerLength = IDT_OUT_CARD;
		if (m_GameLogic.IsValidUser(m_wCurrentUser))
		{
			//�й�״̬
			if (m_bUserTrust[m_wCurrentUser])
			{
				cbTimerStyle = IDS_TIMER_TRUST;
				cbTimerLength = IDT_USER_TRUST;
			}
			//����״̬
			else if (m_bUserTing[m_wCurrentUser])
			{
				cbTimerStyle = IDS_TIMER_TING;
				cbTimerLength = IDT_USER_TING;
			}
		}

		// ����˽�˳�;
		if (m_pGameServiceOption->wServerType != GAME_GENRE_EDUCATE && m_pGameServiceOption->wServerType != GAME_GENRE_PRIVATE)
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, cbTimerLength * 1000, 1, (cbTimerStyle << 16) | wChairID);
		}
	}

	return true;
}

//��Ӧ�ж�
bool CTableFrameSink::EstimateUserRespond(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//�û�״̬
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

	//��������
	bool bAroseAction = false;
	switch (EstimatKind)
	{
		case EstimatKind_SendCard:
		{
			bAroseAction = EstimatSendCard(wChairID, cbCenterCard);
		}break;
		case EstimatKind_OutCard:
		{
			bAroseAction = EstimatOutCard(wChairID, cbCenterCard);
		}break;
		case EstimatKind_ChiPengCard:
		{
			bAroseAction = EstimatChiPengCard(wChairID, cbCenterCard);
		}break;
		case EstimatKind_BuGangCard:
		{
			bAroseAction = EstimatBuGangCard(wChairID, cbCenterCard);
		}break;
		default:
		{
			ASSERT(FALSE);
		}break;
	}

	//�������
	if (bAroseAction)
	{
		// ���ƣ�����
		if (EstimatKind == EstimatKind_OutCard || EstimatKind == EstimatKind_BuGangCard)
		{
			m_wResumeUser = m_wCurrentUser;
			m_wCurrentUser = INVALID_CHAIR;
		}

		// ��������������
		if ((m_bUserTianTing[wChairID] == FALSE) && ((m_dwUserAction[wChairID] & WIK_TING) != 0))
		{
			m_dwUserAction[wChairID] &= ~WIK_TING;
		}

		//������ʾ
		SendOperateNotify(wChairID, cbCenterCard, EstimatKind);

		return true;
	}

	return false;
}

//����Լ��ĸܡ�����������
bool CTableFrameSink::EstimatSendCard(WORD wChairID, BYTE cbSendCard)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//���������ĿУ��
	BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbHandCardCount > 0 && cbHandCardCount <= MAX_COUNT && cbHandCardCount % 3 == 2);

	//���Ƽ��
	DWORD dwPreHuPaiKind = GetPreHuPaiKind(wChairID, cbSendCard);

	// ��ⱳ�ܺ�(�ṩ�߻���Ϊ0������);
	if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) &&*/ (m_wGangPaiProvideUser != INVALID_CHAIR) && (m_wGangPaiProvideUser != wChairID))
	{
		//���㰵�ܷ���
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wGangPaiProvideUser);
		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser];

		if (lProvideUserScore > 0)
		{
			tagHuPaiResult HuPaiResult;
			m_dwUserAction[wChairID] |= m_GameLogic.AnalyseHuPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], \
				cbSendCard, m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, HuPaiResult, true);
		}
	}
	else
	{
		tagHuPaiResult HuPaiResult;
		m_dwUserAction[wChairID] |= m_GameLogic.AnalyseHuPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], \
			cbSendCard, m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, HuPaiResult, true);
	}

	//������״̬�£�������ƺ͸���
	if (!m_bUserTing[wChairID])
	{
		dwPreHuPaiKind &= HU_XGKAIHUA;
		dwPreHuPaiKind &= HU_DGKAIHUA;

		//���Ƽ��
		m_dwUserAction[wChairID] |= m_GameLogic.AnalyseTingPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], \
			m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, m_TingPaiResult[wChairID]);

		if ((m_dwUserAction[wChairID] & WIK_TING) != 0)
		{
			//����ʣ��������
			GetTingCardLastCount(m_TingPaiResult[wChairID], wChairID);

			//�ж��Ƿ�ֻ���޻���
			if (IsOnlyHuWuHuaGuo(dwPreHuPaiKind))
			{
				m_dwUserAction[wChairID] &= ~WIK_TING;
			}
		}

		//���Ƽ��
		if (m_cbLeftCardCount > KEEP_LEFT_CARD_COUNT)
		{
			m_dwUserAction[wChairID] |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], \
				cbSendCard, m_GangPaiResult[wChairID], true);

			// ���Ӱ��Ӻ��Ƽ��(��������ܣ��ж����ܺ��Ƿ�ɰ��ӣ��������ֱ����ʾ��Һ���)����ֻ�а���;
			if ((m_dwUserAction[wChairID] & WIK_AGANG) != 0)
			{
				// �Ѿ��Ƕ���ͬһ����������ˣ��ڶ�����һ��һ�죬���������;
				if (IsSanPeng(wChairID))
				{
					m_dwUserAction[wChairID] = WIK_HU;
				}
			}
		}
	}
	
	//���ϳ���
	m_dwUserAction[wChairID] |= WIK_CHU;

	return true;
}

//���������ҵĳԡ������ܡ�������
bool CTableFrameSink::EstimatOutCard(WORD wChairID, BYTE cbCenterCard)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	BYTE cbLastCardData = 0x0;

	bool bAroseAction = false;

	// ��ȡ������һ���
	IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONG lOutScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wChairID][0] + m_lOutCardScore[wChairID][1] + m_lGangScore[wChairID];

	//�����ж�
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//�û�����,�Լ������
		if (wChairID == i) continue;

		//���������ĿУ��
		BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
		ASSERT(cbHandCardCount > 0 && cbHandCardCount <= MAX_COUNT);

		// ���Ƽ��
		if (!m_bInvalidHuStates[i])
		{
			DWORD dwPreHuPaiKind = GetPreHuPaiKind(i, cbCenterCard);
			tagHuPaiResult HuPaiResult;
			m_dwUserAction[i] |= m_GameLogic.AnalyseHuPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], cbCenterCard, m_cbHuaPaiCount[i][0], dwPreHuPaiKind, HuPaiResult, false);

			// ������һ���Ϊ0;
			if (lOutScore <= 0 && ((m_dwUserAction[i] & WIK_HU) == WIK_HU))
			{
				m_dwUserAction[i] &= ~WIK_HU;
				m_bInvalidHuStates[i] = true;
			}
		}

		//��ҷ�����״̬�£�������ƺ͸���
		if (!m_bUserTing[i])
		{
			//���Ƽ��
			if (cbCenterCard != m_cbInvalidOperteStates[i])
			{
				m_dwUserAction[i] |= m_GameLogic.AnalysePengPai(m_cbCardIndex[i], cbCenterCard);
			}

			//���Ƽ��
			if (m_cbLeftCardCount > KEEP_LEFT_CARD_COUNT)
			{
				m_dwUserAction[i] |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], cbCenterCard, m_GangPaiResult[i], false);
			}

			// ���Ӱ��Ӻ��Ƽ��(��������ܣ��ж����ܺ��Ƿ�ɰ��ӣ��������ֱ����ʾ��Һ���)�����ް���;
			if ((m_dwUserAction[i] & (WIK_PENG | WIK_MGANG)) != 0)
			{
				// �Ѿ��Ƕ���ͬһ����������ˣ��ڶ�����һ��һ�죬���������;
				if (IsSanPeng(i))
				{
					m_dwUserAction[i] = WIK_HU;
				}

				// ������һɫ�������;
				if (IsSiPengQingyise(i, cbCenterCard, m_dwUserAction[i]))
				{
					m_dwUserAction[i] = WIK_HU;
				}
			}
		}

		//����ж�
		if (m_dwUserAction[i] != WIK_NULL)
		{
			bAroseAction = true;
		}
	}

	return bAroseAction;
}

//��������������(�����������)
bool CTableFrameSink::EstimatChiPengCard(WORD wChairID, BYTE cbCenterCard)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//���ϳ���
	m_dwUserAction[wChairID] |= WIK_CHU;

	return true;
}

// �ܼ�����(�ܺ��������);
bool CTableFrameSink::EstimatGangCard(WORD wChairID, BYTE cbCenterCard)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	return false;
}

//��鲹�ܺ�������ҿɷ�������ܺ���
bool CTableFrameSink::EstimatBuGangCard(WORD wChairID, BYTE cbCenterCard)
{
	//�������ܺ�״̬
	m_QiangGangHuInfo.ReInit();

	IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONG lPracticeScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wChairID][0] + m_lOutCardScore[wChairID][1] + m_lGangScore[wChairID];

	bool bAroseAction = false;
	BYTE cbCount = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//�û�����
		if (wChairID == i) continue;

		//�����ж�
		DWORD dwPreHuPaiKind = GetPreHuPaiKind(i, cbCenterCard);
		tagHuPaiResult HuPaiResult;
		m_dwUserAction[i] |= m_GameLogic.AnalyseHuPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], cbCenterCard, m_cbHuaPaiCount[i][0], dwPreHuPaiKind, HuPaiResult, false);

		// ������һ���Ϊ0;
		if (lPracticeScore <= 0 && ((m_dwUserAction[i] & WIK_HU) == WIK_HU))
		{
			m_dwUserAction[i] &= ~WIK_HU;
			m_bInvalidHuStates[i] = true;
		}

		//����ж�
		if ((m_dwUserAction[i] & WIK_HU) != 0)
		{
			bAroseAction = true;
			ASSERT(cbCount < CountArray(m_QiangGangHuInfo.wQiangGangHuUser));
			m_QiangGangHuInfo.wQiangGangHuUser[cbCount++] = i;
		}
	}

	return bAroseAction;
}

//�����Ҳ���
bool CTableFrameSink::EstimatUserBuHua(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	m_wBuHuaUser = INVALID_CHAIR;
	ZeroMemory(m_bBuHuaOver, sizeof(m_bBuHuaOver));

	//���ֲ������
	if (IsFirstBuHuaOver())
	{
		BYTE cbHuaCardCount = 0;
		BYTE cbHuaCardData[MAX_HUA_COUNT] = { 0 };
		cbHuaCardCount = m_GameLogic.DistillHardHuaCard(m_cbCardIndex[wChairID], cbHuaCardData);
		ASSERT(cbHuaCardCount <= 1);

		//��Ҫ����
		if (cbHuaCardCount > 0)
		{
			//ֱ������
			if (m_cbLeftCardCount - cbHuaCardCount < KEEP_LEFT_CARD_COUNT)
			{
				m_cbHuPaiCard = 0;
				m_wProvideUser = INVALID_CHAIR;
				return OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
			}

			//ɾ������
			if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbHuaCardData, cbHuaCardCount))
			{
				ASSERT(FALSE);
				return false;
			}
			VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);

			//���Ͳ�����Ϣ
			CMD_S_SendBuHua stSendBuHua;
			ZeroMemory(&stSendBuHua, sizeof(stSendBuHua));
			stSendBuHua.wBuHuaUser = wChairID;
			stSendBuHua.bFirstBuHua = false;
			stSendBuHua.cbBuHuaCount = cbHuaCardCount;

			CopyMemory(stSendBuHua.cbBuHuaData, cbHuaCardData, sizeof(BYTE)*cbHuaCardCount);

			BYTE	cbReplaceCardData[MAX_COUNT];				//�滻����¼��洢
			ZeroMemory(cbReplaceCardData, CountArray(cbReplaceCardData));

			for (int i = 0; i < GAME_PLAYER; i++)
			{
				// ֻ����Ҫ�������û��ɻ����Ĳ�������
				if (i == wChairID)
				{
					// ���������滻
					for (BYTE j = 0; j < cbHuaCardCount; j++)
					{
						m_cbSendCardCount++;
						m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
						m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
						VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
						stSendBuHua.cbReplaceCardData[j] = m_cbSendCardData;

						//��¼��������
						m_cbHuaPaiData[wChairID][m_cbHuaPaiCount[wChairID][0]++] = cbHuaCardData[j];
					}

					CopyMemory(cbReplaceCardData, stSendBuHua.cbReplaceCardData, CountArray(stSendBuHua.cbReplaceCardData));
				}
				else
				{
					ZeroMemory(stSendBuHua.cbReplaceCardData, sizeof(stSendBuHua.cbReplaceCardData));
				}

				m_pITableFrame->SendTableData(i, SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));
			}

			ZeroMemory(stSendBuHua.cbReplaceCardData, sizeof(stSendBuHua.cbReplaceCardData));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));

			// ��Ϸ¼��
			CopyMemory(stSendBuHua.cbReplaceCardData, cbReplaceCardData, CountArray(stSendBuHua.cbReplaceCardData));
			addGameRecordAction(SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));

			//��⻨�ܵ÷�
			CheckHuaGangScore(wChairID);

			//���õ�ǰ�û����ṩ�û����ṩ��
			m_cbProvideCard = m_cbSendCardData;
			m_wProvideUser = wChairID;
			m_wCurrentUser = wChairID;

			//���û���״̬
			m_cbGangStatus = GS_XIAOGANGKAIHUA;

			//����������ʱ��ʱ��
			m_wBuHuaUser = wChairID;
			m_pITableFrame->SetGameTimer(IDI_SEND_BUHUA, IDT_SEND_BUHUA * 1000, 1, wChairID);
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem != NULL && pServerUserItem->IsAndroidUser())
				{
					OnSubBuHuaOver(i);
				}
			}

			return true;
		}
		else
		{
			//�����������
			EstimateUserRespond(wChairID, m_cbProvideCard, EstimatKind_SendCard);
		}
	}
	else
	{
		BYTE cbHuaCardCount = 0;
		BYTE cbHuaCardData[MAX_HUA_COUNT] = { 0 };
		cbHuaCardCount = m_GameLogic.DistillHardHuaCard(m_cbCardIndex[wChairID], cbHuaCardData);
		ASSERT(!m_bFirstBuHuaOver[wChairID] && cbHuaCardCount <= MAX_COUNT);
		if (cbHuaCardCount > 0 && !m_bFirstBuHuaOver[wChairID])
		{
			//������ĿУ��
			BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
			ASSERT(cbHandCardCount > 0 && cbHandCardCount <= MAX_COUNT);
			//ɾ������
			if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbHuaCardData, cbHuaCardCount))
			{
				ASSERT(FALSE);
				return false;
			}
			//������ĿУ��
			cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
			ASSERT(cbHandCardCount >= 0 && cbHandCardCount <= MAX_COUNT);

			//���Ͳ�����Ϣ
			CMD_S_SendBuHua stSendBuHua;
			ZeroMemory(&stSendBuHua, sizeof(stSendBuHua));
			stSendBuHua.wBuHuaUser = wChairID;
			stSendBuHua.bFirstBuHua = true;
			stSendBuHua.cbBuHuaCount = cbHuaCardCount;

			CopyMemory(stSendBuHua.cbBuHuaData, cbHuaCardData, sizeof(BYTE)*cbHuaCardCount);

			BYTE	cbReplaceCardData[MAX_COUNT];				//�滻����¼��洢
			ZeroMemory(cbReplaceCardData, CountArray(cbReplaceCardData));

			BYTE cbSendCardData = 0x0;
				
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				// ֻ����Ҫ�������û��ɻ����Ĳ�������
				if (i == wChairID)
				{
					// ���������滻
					for (BYTE j = 0; j < cbHuaCardCount; j++)
					{
						m_cbSendCardCount++;
						//m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
						cbSendCardData = getBuHuaCardData(wChairID);
						m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(cbSendCardData)]++;
						VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
						stSendBuHua.cbReplaceCardData[j] = cbSendCardData;

						// ��¼��ǰ��ҵ�����;
						if (i == m_wCurrentUser)
						{
							m_cbSendCardData = cbSendCardData;
						}

						//��¼��������
						m_cbHuaPaiData[wChairID][m_cbHuaPaiCount[wChairID][0]++] = cbHuaCardData[j];
					}

					CopyMemory(cbReplaceCardData, stSendBuHua.cbReplaceCardData, CountArray(stSendBuHua.cbReplaceCardData));
				}
				else
				{
					ZeroMemory(stSendBuHua.cbReplaceCardData, sizeof(stSendBuHua.cbReplaceCardData));
				}

				m_pITableFrame->SendTableData(i, SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));
			}

			ZeroMemory(stSendBuHua.cbReplaceCardData, sizeof(stSendBuHua.cbReplaceCardData));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));

			// ��Ϸ¼��
			CopyMemory(stSendBuHua.cbReplaceCardData, cbReplaceCardData, CountArray(stSendBuHua.cbReplaceCardData));
			addGameRecordAction(SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));

			//ׯ�����ֲ���ʱ�����û���״̬
			if (wChairID == m_wBankerUser)
			{
				m_wCurrentUser = wChairID;
				m_wProvideUser = wChairID;
				m_cbProvideCard = m_cbSendCardData;
				m_cbGangStatus = GS_XIAOGANGKAIHUA;
			}

			//��⻨�ܵ÷�
			CheckHuaGangScore(wChairID);

			//����������ʱ��ʱ��
			m_wBuHuaUser = wChairID;
			m_pITableFrame->SetGameTimer(IDI_SEND_BUHUA, IDT_SEND_BUHUA * 1000, 1, wChairID);
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				if (pServerUserItem != NULL && pServerUserItem->IsAndroidUser())
				{
					OnSubBuHuaOver(i);
				}
			}
			return true;
		}
		else
		{
			//����Ҳ���Ҫ����
			m_bFirstBuHuaOver[wChairID] = true;

			//����м�����
			if (wChairID != m_wBankerUser)
			{
				DWORD dwPreHuPaiKind = HU_DIHU;
				tagTingPaiResult TingpaiResult;
				DWORD dwTingAction = m_GameLogic.AnalyseTingPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, TingpaiResult, true);
				if (dwTingAction != WIK_NULL)
				{
					m_bUserTianTing[wChairID] = true;
				}
			}

			//�¼Ҽ�������
			WORD wNextBuHuaUser = (wChairID + GAME_PLAYER - 1) % GAME_PLAYER;
			if (wNextBuHuaUser != m_wBankerUser)
			{
				//����¼Ҳ���
				EstimatUserBuHua(wNextBuHuaUser);
				return true;
			}
			else
			{
				//�����������
				EstimateUserRespond(m_wBankerUser, m_cbProvideCard, EstimatKind_SendCard);
				return true;
			}
		}
	}

	return false;
}

//�������ܷ���
bool CTableFrameSink::CheckChangeTingPai(WORD wChairID, tagTingPaiResult TingPaiResult)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//��ǰû����
	if (m_HaveTingPaiResult[wChairID].cbTingCardCount == 0)
	{
		return true;
	}
	
	//��ȡ�µ���������
	for (BYTE i = 0; i < TingPaiResult.cbTingPaiItemCount; i++)
	{
		for (BYTE j = 0; j < TingPaiResult.TingPaiItemArrary[i].cbTingCardCount; j++)
		{
			//����¼������Ʋ����������У����Ի���
			if (!m_GameLogic.IsCardInArrary(m_HaveTingPaiResult[wChairID].cbTingCardData, m_HaveTingPaiResult[wChairID].cbTingCardCount, TingPaiResult.TingPaiItemArrary[i].cbTingCardData[j]))
			{
				return true;
			}
		}
	}

	return false;
}

//��黨�ܵ÷�
void CTableFrameSink::CheckHuaGangScore(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	//ͳ�ƻ�������
	BYTE cbHuaPaiCount[11] = { 0 };
	for (BYTE i = 0; i < m_cbHuaPaiCount[wChairID][0]; i++)
	{
		if (m_cbHuaPaiData[wChairID][i] >= 0x35 && m_cbHuaPaiData[wChairID][i] <= 0x37)
		{
			ASSERT((m_cbHuaPaiData[wChairID][i] - 0x35) >= 0 && (m_cbHuaPaiData[wChairID][i] - 0x35) < 11);
			cbHuaPaiCount[m_cbHuaPaiData[wChairID][i] - 0x35]++;
		}
		if (m_cbHuaPaiData[wChairID][i] >= 0x41 && m_cbHuaPaiData[wChairID][i] <= 0x48)
		{
			ASSERT((m_cbHuaPaiData[wChairID][i] - 0x41) >= 0 && (m_cbHuaPaiData[wChairID][i] - 0x41) < 8);
			cbHuaPaiCount[3 + m_cbHuaPaiData[wChairID][i] - 0x41]++;
		}
	}

	//�ܱ���
	LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

	//����С����Ϣ
	CMD_S_PayMoney stPayMoney;
	ZeroMemory(&stPayMoney, sizeof(stPayMoney));
	stPayMoney.wPayToUser = wChairID;

	//�жϻ���״̬
	for (BYTE i = 0; i < 5; i++)
	{
		//�ĸ��С������� ���� ���
		if (i < 3)
		{
			if (cbHuaPaiCount[i] == 4 && !m_bHuaGangStatus[wChairID][i])
			{
				CountPayMoneyScore(stPayMoney, PAY_MONEY_HUAGANG, BASE_HUA_COUNT_HGANG);
				m_bHuaGangStatus[wChairID][i] = true;
			}
		}
		else if (i == 3)
		{
			//һ�鴺���ġ�������� ���
			if (cbHuaPaiCount[3] > 0 && cbHuaPaiCount[4] > 0 && cbHuaPaiCount[5] > 0 && cbHuaPaiCount[6] > 0 && !m_bHuaGangStatus[wChairID][i])
			{
				CountPayMoneyScore(stPayMoney, PAY_MONEY_HUAGANG, BASE_HUA_COUNT_HGANG);
				m_bHuaGangStatus[wChairID][i] = true;
			}
		}
		else if (i == 4)
		{
			//һ��÷�������񡢾ջ��� ���
			if (cbHuaPaiCount[7] > 0 && cbHuaPaiCount[8] > 0 && cbHuaPaiCount[9] > 0 && cbHuaPaiCount[10] > 0 && !m_bHuaGangStatus[wChairID][i])
			{
				CountPayMoneyScore(stPayMoney, PAY_MONEY_HUAGANG, BASE_HUA_COUNT_HGANG);
				m_bHuaGangStatus[wChairID][i] = true;
			}
		}
	}

	ASSERT(stPayMoney.lPayToNumber + stPayMoney.lPayFromNumber[0] + stPayMoney.lPayFromNumber[1] + stPayMoney.lPayFromNumber[2] == 0);

	//���ͻ���С��
	if (stPayMoney.lPayToNumber > 0)
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

		// ��¼��־
		PayMoneyLog(stPayMoney);

		//�����޻����ƾֽ���;
		BYTE cbNoScore = 0;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == nullptr) continue;

			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
			if (lProvideUserScore <= 0)
			{
				cbNoScore++;
			}
		}

		if (cbNoScore >= 2)
		{
			OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
		}
	}
}

//���㻨�ܵ÷�
void CTableFrameSink::CountPayMoneyScore(CMD_S_PayMoney &stPayMoney, BYTE cbPayType, LONG lBaseHuaCount)
{
	//�ܱ���
	LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

	stPayMoney.cbPayReason = cbPayType;

	BYTE cbFromChairID = 0;
	LONG lAllGangScore = 0;
	LONG lGangScore = lBaseHuaCount*lPreMultipleCellScore;
	for (WORD j = 0; j < GAME_PLAYER; j++)
	{
		if (j == stPayMoney.wPayToUser) continue;

		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(j);
		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j];

#if IS_OPEN_PRINT_LOG
		MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ���� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
			j, pServerUserItem->GetUserScore(), m_lOutCardScore[j][0], m_lOutCardScore[j][1], m_lGangScore[j]);
#endif
		
		if (lProvideUserScore >= lGangScore)
		{
			//�������ܷ���
			lAllGangScore += lGangScore;
			m_lGangScore[j] -= lGangScore;

			stPayMoney.wPayFromUser[cbFromChairID] = j;
			stPayMoney.lPayFromNumber[cbFromChairID++] -= lGangScore;
		}
		else
		{
			//�������ܷ���
			lAllGangScore += lProvideUserScore;
			m_lGangScore[j] -= lProvideUserScore;

			stPayMoney.wPayFromUser[cbFromChairID] = j;
			stPayMoney.lPayFromNumber[cbFromChairID++] -= lProvideUserScore;
		}
	}

	m_lGangScore[stPayMoney.wPayToUser] += lAllGangScore;	// ��û���ͳ��;
	stPayMoney.lPayToNumber += lAllGangScore;				// ��û���;

	m_bHuaGang = true;

	return;
}

//�����Ƶ÷�
void CTableFrameSink::CheckOutCardScore(WORD wChairID)
{
	// ���ķ�;
	if (!IsGameRuleBaoZi())
	{
		CheckShouSiFeng(wChairID);
	}

	// ÷����;
	CheckMeiSiZhang(wChairID);

	// ������;
	CheckSiLianFa(wChairID);

	//�����޻����ƾֽ���;
	BYTE cbNoScore = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == nullptr) continue;

		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
		if (lProvideUserScore <= 0)
		{
			cbNoScore++;
		}
	}

	if (cbNoScore >= 2)
	{
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
	}
}

// ���ķ�;
void CTableFrameSink::CheckShouSiFeng(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	//��⽱��
	if (m_cbDiscardRecordCount[wChairID] == 4)
	{
		bool bHaveDNXB = (m_cbDiscardRecordCard[wChairID][0] == 0x31) && (m_cbDiscardRecordCard[wChairID][1] == 0x32) && (m_cbDiscardRecordCard[wChairID][2] == 0x33) && (m_cbDiscardRecordCard[wChairID][3] == 0x34);
		bool bHaveBXND = (m_cbDiscardRecordCard[wChairID][0] == 0x34) && (m_cbDiscardRecordCard[wChairID][1] == 0x33) && (m_cbDiscardRecordCard[wChairID][2] == 0x32) && (m_cbDiscardRecordCard[wChairID][3] == 0x31);

		//ǰ�����Ƕ����ϡ�������
		if ((bHaveDNXB || bHaveBXND) && !m_bAwardStatus[wChairID])
		{
			LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

			//����״̬
			m_bHaveJiangFen = true;
			m_bAwardStatus[wChairID] = true;

			CMD_S_PayMoney stPayMoney;
			ZeroMemory(&stPayMoney, sizeof(stPayMoney));
			stPayMoney.cbPayReason = PAY_MONEY_SHOUSIFENG;
			stPayMoney.wPayToUser = wChairID;

			BYTE cbFromChairID = 0;
			LONG lAllScore = 0;
			LONG lFengScore = BASE_HUA_COUNT_JIANGFEN_BX*lPreMultipleCellScore;		// �����˿۷�;

			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i != wChairID)
				{
					IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
					LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];

#if IS_OPEN_PRINT_LOG
					MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ���ķ� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
						i, pServerUserItem->GetUserScore(), m_lOutCardScore[i][0], m_lOutCardScore[i][1], m_lGangScore[i]);
#endif

					if (lProvideUserScore >= lFengScore)
					{
						//���Ʒ���
						lAllScore += lFengScore;
						m_lOutCardScore[i][0] -= lFengScore;

						stPayMoney.wPayFromUser[cbFromChairID] = i;
						stPayMoney.lPayFromNumber[cbFromChairID++] -= lFengScore;
					}
					else
					{
						//���Ʒ���
						lAllScore += lProvideUserScore;
						m_lOutCardScore[i][0] -= lProvideUserScore;

						stPayMoney.wPayFromUser[cbFromChairID] = i;
						stPayMoney.lPayFromNumber[cbFromChairID++] -= lProvideUserScore;
					}
				}
			}

			m_lOutCardScore[wChairID][0] += lAllScore;	// �÷��ۼ�
			stPayMoney.lPayToNumber += lAllScore;		// �÷�

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

			// ��¼��־
			PayMoneyLog(stPayMoney);

		}
	}
}

// ÷����;
void CTableFrameSink::CheckMeiSiZhang(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	//��ⷣ��(������һ������)1���˳�4��ͬ�����ƣ���һ��Ҫ��������������������ÿ��10֧����
	for (BYTE i = 0; i < MAX_NORMAL; i++)
	{
		//�����������һ�����Ʒ���
		if (m_cbDiscardIndexCount[wChairID][i] == 4 && !m_bPenaltyStatus[wChairID][i])
		{
			LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

			//�ܱ���
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			// �۷���ʵ�ʻ���;
			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wChairID][0] + m_lOutCardScore[wChairID][1] + m_lGangScore[wChairID];

#if IS_OPEN_PRINT_LOG
			MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ÷���� UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
				wChairID, pServerUserItem->GetUserScore(), m_lOutCardScore[wChairID][0], m_lOutCardScore[wChairID][1], m_lGangScore[wChairID]);
#endif

			// �÷���ÿ���˵÷�;
			LONG lScore = BASE_HUA_COUNT_JIANGFEN_BX * lPreMultipleCellScore;

			// �۷���ʵ�ʻ��ִ���Ӧ�ۻ��֣��Ž��п۷ִ���;
			if (lProvideUserScore >= lScore * 3L)
			{
				//���÷���״̬
				m_bPenaltyStatus[wChairID][i] = true;

				// �۷��˿۷�;
				m_lOutCardScore[wChairID][1] -= lScore * 3L;

				CMD_S_PayMoney stPayMoney;
				ZeroMemory(&stPayMoney, sizeof(stPayMoney));
				stPayMoney.cbPayReason = PAY_MONEY_MEISIZHANG;
				stPayMoney.wPayToUser = wChairID;

				stPayMoney.lPayToNumber -= lScore * 3L;

				BYTE cbFromChairID = 0;
				for (WORD j = 0; j < GAME_PLAYER; j++)
				{
					if (j != wChairID)
					{
						stPayMoney.wPayFromUser[cbFromChairID] = j;
						stPayMoney.lPayFromNumber[cbFromChairID++] += lScore;
						m_lOutCardScore[j][1] += lScore;
					}
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

				// ��¼��־
				PayMoneyLog(stPayMoney);
			}

			m_bHaveFaFen = true;

			break;
		}
	}
}

// ������;
void CTableFrameSink::CheckSiLianFa(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));
	
	//��ⷣ��(4��������һ������)��һ�����Ƶ��˷�����������ÿ��10֧����
	if (m_cbOutSameCardCount == 4)
	{
		LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

		//�û�У��
		VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(m_wFirstOutUserForFaFen));

		//�ܱ���
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wFirstOutUserForFaFen);
		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[m_wFirstOutUserForFaFen][0]
			+ m_lOutCardScore[m_wFirstOutUserForFaFen][1] + m_lGangScore[m_wFirstOutUserForFaFen];

#if IS_OPEN_PRINT_LOG
		MyLog("��Ǯ��[%d]ʵ�ʻ���  ===================== ������ UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
			m_wFirstOutUserForFaFen, pServerUserItem->GetUserScore(), m_lOutCardScore[m_wFirstOutUserForFaFen][0], m_lOutCardScore[m_wFirstOutUserForFaFen][1], m_lGangScore[m_wFirstOutUserForFaFen]);
#endif
		LONG lScore = BASE_HUA_COUNT_JIANGFEN_BX * lPreMultipleCellScore;

		if (lProvideUserScore >= lScore * 3L)
		{
			//���÷���״̬
			VERIFY(m_GameLogic.IsValidUser(m_wFirstOutUserForFaFen));
			m_lOutCardScore[m_wFirstOutUserForFaFen][1] -= lScore * 3L;

			CMD_S_PayMoney stPayMoney;
			ZeroMemory(&stPayMoney, sizeof(stPayMoney));
			stPayMoney.cbPayReason = PAY_MONEY_SILIANFA;
			stPayMoney.wPayToUser = m_wFirstOutUserForFaFen;
			stPayMoney.lPayToNumber -= lScore * 3L;

			BYTE cbFromChairID = 0;
			for (WORD j = 0; j < GAME_PLAYER; j++)
			{
				if (j != m_wFirstOutUserForFaFen)
				{
					stPayMoney.wPayFromUser[cbFromChairID] = j;
					stPayMoney.lPayFromNumber[cbFromChairID++] += lScore;
					m_lOutCardScore[j][1] += lScore;
				}
			}

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

			// ��¼��־
			PayMoneyLog(stPayMoney);
		}

		m_bHaveFaFen = true;
	}
}

//�������Ƿ��а���
void CTableFrameSink::CheckHaveBaoPaiUser(WORD wChairID)
{
	////�û�У��
	//VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	////ȥ�������ߵİ���״̬
	//m_BaoPaiResult[wChairID].bBaoPaiStatus = false;
	//for (WORD i = 0; i < GAME_PLAYER; i++)
	//{
	//	if (i == wChairID) continue;
	//	if (m_BaoPaiResult[i].bBaoPaiStatus)
	//	{
	//		for (BYTE j = 0; j < 3; j++)
	//		{
	//			if (m_BaoPaiResult[i].wTargetUser[j] == wChairID)
	//			{
	//				m_bHaveBaoPai = true;
	//				break;
	//			}
	//		}
	//	}
	//}
}

//��ȡ��ҿɲ�������
BYTE CTableFrameSink::GetOperateCard(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	BYTE cbProvideCard = 0;

	//���ṩ��
	if (m_cbProvideCard != 0)
	{
		cbProvideCard = m_cbProvideCard;
	}
	else
	{
		//ѡ���������һ��
		for (int i = MAX_INDEX - 1; i >= 0; --i)
		{
			if (m_cbCardIndex[wChairID][i] > 0)
			{
				cbProvideCard = m_GameLogic.SwitchToCardData(i);
				break;
			}
		}
	}

	// ���ܳ�����
	ASSERT((cbProvideCard > 0x0) && (cbProvideCard < 0x35));

	return cbProvideCard;
}

//��ȡ�����ǰ��������
DWORD CTableFrameSink::GetPreHuPaiKind(WORD wChairID, BYTE cbHuPaiCard)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	DWORD dwPreHuPaiKind = 0;
	//������
	if (IsTianHu(wChairID))
	{
		dwPreHuPaiKind |= HU_TIANHU;
	}
	//���غ�
	if (IsDiHu(wChairID))
	{
		dwPreHuPaiKind |= HU_DIHU;
	}
	//���С�ܿ���
	if (IsXiaoGangKaiHua(wChairID))
	{
		dwPreHuPaiKind |= HU_XGKAIHUA;
	}
	//����ܿ���
	if (IsDaGangKaiHua(wChairID))
	{
		dwPreHuPaiKind |= HU_DGKAIHUA;
	}
	//���ѹ��
	if (IsYaJue(wChairID, cbHuPaiCard))
	{
		dwPreHuPaiKind |= HU_YAJUE;
	}
	//����޻���
	if (IsWuHuaGuo(wChairID))
	{
		dwPreHuPaiKind |= HU_WUHUAGUO;
	}

	return dwPreHuPaiKind;
}

//��ȡ����ʱʣ��������
void CTableFrameSink::GetTingCardLastCount(tagTingPaiResult &TingPaiResult, const WORD wChairID)
{
	if (TingPaiResult.cbTingPaiItemCount == 0) return;
	for (BYTE i = 0; i < TingPaiResult.cbTingPaiItemCount; i++)
	{
		for (BYTE j = 0; j < TingPaiResult.TingPaiItemArrary[i].cbTingCardCount; j++)
		{
			BYTE cbTingCard = TingPaiResult.TingPaiItemArrary[i].cbTingCardData[j];
			TingPaiResult.TingPaiItemArrary[i].cbLeftCardCount[j] = GetCardVisualCount(cbTingCard, wChairID);
		}
	}
}

//��ȡ����Ŀ(�������ȥ���ƣ��ԡ��������ܵ���)
int CTableFrameSink::GetCardVisualCount(const BYTE cbCardData, const WORD wChairID)
{
	BYTE cbCardCount = 0;

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//���ȥ����
		for (BYTE j = 0; j < m_cbDiscardCount[i]; j++)
		{
			if (m_cbDiscardCard[i][j] != 0 && m_cbDiscardCard[i][j] == cbCardData)
			{
				cbCardCount++;
			}
		}
		//�ԡ��������ܵ���
		for (BYTE k = 0; k < m_cbWeaveItemCount[i]; k++)
		{
			if (m_WeaveItemArray[i][k].cbCenterCard == cbCardData)
			{
				if ((m_WeaveItemArray[i][k].dwWeaveKind&WIK_PENG) != 0)
				{
					cbCardCount += 3;
				}
				else if ((m_WeaveItemArray[i][k].dwWeaveKind&WIK_BGANG) != 0)
				{
					cbCardCount += 4;
				}
				else if ((m_WeaveItemArray[i][k].dwWeaveKind&WIK_MGANG) != 0)
				{
					cbCardCount += 4;
				}
			}
		}
	}
	//ȥ���Լ����е���
	for (BYTE i = 0; i < MAX_INDEX; i++)
	{
		if (m_cbCardIndex[wChairID][i]>0 && m_GameLogic.SwitchToCardData(i) == cbCardData)
		{
			cbCardCount += m_cbCardIndex[wChairID][i];
		}
	}
	ASSERT(cbCardCount <= 4);
	return 4 - cbCardCount;
}

//����
bool CTableFrameSink::IsZiMo(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_wCurrentUser != INVALID_CHAIR && wChairID == m_wProvideUser)
	{
		return true;
	}
	return false;
}

//���
bool CTableFrameSink::IsTianHu(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbSendCardCount == 1 && m_cbOutCardCount == 0 && IsZiMo(wChairID) && wChairID == m_wBankerUser)
	{
		return true;
	}
	return false;
}

//�غ�
bool CTableFrameSink::IsDiHu(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_bUserTianTing[wChairID])
	{
		return true;
	}
	return false;
}

//�޻���
bool CTableFrameSink::IsWuHuaGuo(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbHuaPaiCount[wChairID][0] == 0)
	{
		return true;
	}
	return false;
}

//ѹ��
bool CTableFrameSink::IsYaJue(WORD wChairID, BYTE cbHuPaiCard)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (cbHuPaiCard == 0) return false;
	bool bYaJue = false;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		for (BYTE j = 0; j < m_cbWeaveItemCount[i]; j++)
		{
			//���������������
			if ((m_WeaveItemArray[i][j].dwWeaveKind == WIK_PENG) && (m_WeaveItemArray[i][j].cbCenterCard == cbHuPaiCard))
			{
				bYaJue = true;
				break;
			}
		}
	}
	return bYaJue;
}

//��������
bool CTableFrameSink::IsHaiDiLaoYue(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	if (IsZiMo(wChairID) && m_cbLeftCardCount <= KEEP_LEFT_CARD_COUNT)
	{
		return true;
	}
	return false;
}

//С�ܿ���
bool CTableFrameSink::IsXiaoGangKaiHua(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbGangStatus == GS_XIAOGANGKAIHUA && IsZiMo(wChairID))
	{
		return true;
	}
	return false;
}

//��ܿ���
bool CTableFrameSink::IsDaGangKaiHua(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbGangStatus == GS_DAGANGKAIHUA && IsZiMo(wChairID))
	{
		return true;
	}
	return false;
}

//���ܺ�
bool CTableFrameSink::IsQiangGangHu(WORD wChairID)
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR && !IsZiMo(wChairID))
	{
		for (int i = 0; i < CountArray(m_QiangGangHuInfo.wQiangGangHuUser); i++)
		{
			if (wChairID == m_QiangGangHuInfo.wQiangGangHuUser[i])
			{
				return true;
			}
		}
	}
	return false;
}

//���º�
bool CTableFrameSink::IsBiXiaHu()
{
	if (!m_bChangeBanker || m_bHaveJiangFen || m_bHaveFaFen || m_bHaveBigHu || m_bHuaGang || m_bHaveBaozi)
	{
		return true;
	}

	return false;
}

//�Ƿ��и߼�����
bool CTableFrameSink::IsHaveOperateCard(DWORD dwOperateMask)
{
	if ((dwOperateMask&WIK_CHI) != 0 || (dwOperateMask&WIK_PENG) != 0 || (dwOperateMask&WIK_GANG) != 0 || \
		(dwOperateMask&WIK_TING) != 0 || (dwOperateMask&WIK_TIAOZHAN) != 0 || (dwOperateMask&WIK_HU) != 0)
	{
		return true;
	}
	return false;
}

//�Ƿ���ȱһ��
bool CTableFrameSink::IsQueYiMen(WORD wChairID, BYTE cbCardIndex[MAX_INDEX])
{
	//�û�У��
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//�������
	bool bCardColor[4] = { false, false, false, false };

	//����
	for (BYTE i = 0; i < MAX_NORMAL; i++)
	{
		if (cbCardIndex[i] > 0)
		{
			BYTE cbCardData = m_GameLogic.SwitchToCardData(i);
			bCardColor[m_GameLogic.GetCardColor(cbCardData)] = true;
		}
	}
	//�����
	for (BYTE i = 0; i < m_cbWeaveItemCount[wChairID]; i++)
	{
		BYTE cbCardData = m_WeaveItemArray[wChairID][i].cbCenterCard;
		bCardColor[m_GameLogic.GetCardColor(cbCardData)] = true;
	}
	//ͳ��
	BYTE cbColorCount = 0;
	for (BYTE i = 0; i < 3; i++)
	{
		if (bCardColor[i])
		{
			cbColorCount++;
		}
	}

	return (cbColorCount == 2) ? true : false;
}

//�Ƿ�ֻ���޻���
bool CTableFrameSink::IsOnlyHuWuHuaGuo(DWORD dwHuPaiKind)
{
	if ((dwHuPaiKind&HU_PINGHU) != 0)			return false;
	else if ((dwHuPaiKind&HU_MENQING) != 0)		return false;
	else if ((dwHuPaiKind&HU_HUNYISE) != 0)		return false;
	else if ((dwHuPaiKind&HU_QINGYISE) != 0)	return false;
	else if ((dwHuPaiKind&HU_DUIDUIHU) != 0)	return false;
	else if ((dwHuPaiKind&HU_QQDUDIAO) != 0)	return false;
	else if ((dwHuPaiKind&HU_QIDUI) != 0)		return false;
	else if ((dwHuPaiKind&HU_SQIDUI) != 0)		return false;
	else if ((dwHuPaiKind&HU_YAJUE) != 0)		return false;
	else if ((dwHuPaiKind&HU_XGKAIHUA) != 0)	return false;
	else if ((dwHuPaiKind&HU_DGKAIHUA) != 0)	return false;
	else if ((dwHuPaiKind&HU_TIANHU) != 0)		return false;
	else if ((dwHuPaiKind&HU_DIHU) != 0)		return false;
	else if ((dwHuPaiKind&HU_WUHUAGUO) != 0)	return true;
	return false;
}

//�����ս����
void CTableFrameSink::CheckTiaoZhanScore(WORD wChairID, LONG lTiaoZhanScore[GAME_PLAYER])
{
	//lTiaoZhanScore[wChairID] += m_TiaoZhanResult[wChairID].lTiaoZhanScore;
	//for (WORD i = 0; i < m_TiaoZhanResult[wChairID].cbCurUserIndex; i++)
	//{
	//	lTiaoZhanScore[m_TiaoZhanResult[wChairID].stTiaoZhanItem[i].wChairID] += m_TiaoZhanResult[wChairID].stTiaoZhanItem[i].lTiaoZhanScore;
	//}
}

//����;
bool CTableFrameSink::IsSanPeng(WORD wChairID)
{
	if (IsGameRuleBaoZi() == false)
	{
		return false;
	}

	tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
	if (pWeaveItem == nullptr) return false;

	BYTE cbItemCount = m_cbWeaveItemCount[wChairID];

	// �������;
	if (cbItemCount == MAX_WEAVE - 1)
	{
		// �Ƿ��а���;
		BYTE cbAnGang = 0;

		BYTE cbSameItemCount[GAME_PLAYER] = { 0 };
		for (int i = 0; i < MAX_WEAVE - 1; i++)
		{
			if (pWeaveItem[i].wProvideUser != wChairID)
			{
				cbSameItemCount[pWeaveItem[i].wProvideUser]++;
			}

			if ((pWeaveItem[i].dwWeaveKind&WIK_AGANG) != 0)
			{
				cbAnGang++;
			}
		}

		for (int j = 0; j < GAME_PLAYER; j++)
		{
			if (cbSameItemCount[j] + cbAnGang >= 3)
			{
				return true;	// ����
			}
		}
	}

	return false;
}

//������һɫ;
bool CTableFrameSink::IsSiPengQingyise(WORD wChairID, BYTE cbCenterCard, DWORD dwUserAction)
{
	if (IsGameRuleBaoZi() == false)
	{
		return false;
	}

	// ���Ӱ��Ӻ��Ƽ��(��������ܣ��ж����ܺ��Ƿ�ɰ��ӣ��������ֱ����ʾ��Һ���);
	if ((dwUserAction & (WIK_PENG | WIK_MGANG | WIK_AGANG)) != 0)
	{
		tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
		if (pWeaveItem == nullptr) return false;

		BYTE cbItemCount = m_cbWeaveItemCount[wChairID];

		// ���б���������;
		if (m_cbWeaveItemCount[wChairID] == MAX_WEAVE - 1)
		{
			bool bCardColor[3] = { false, false, false };
			BYTE cbCardColor[3] = { 0 };

			//�����
			for (BYTE i = 0; i < MAX_WEAVE - 1; i++)
			{
				BYTE cbCardData = m_WeaveItemArray[wChairID][i].cbCenterCard;
				if (m_GameLogic.IsZiCard(cbCardData))
				{
					return false;
				}
				else
				{
					cbCardColor[m_GameLogic.GetCardColor(cbCardData)]++;
				}
			}

			// Ԥ�������ͳ��;
			if (m_GameLogic.IsZiCard(cbCenterCard))
			{
				return false;
			}
			else
			{
				cbCardColor[m_GameLogic.GetCardColor(cbCenterCard)]++;
			}

			//ͳ��
			BYTE cbColorCount = 0;
			for (BYTE i = 0; i < 3; i++)
			{
				if (MAX_WEAVE == cbCardColor[i])
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

//�����ṩ��;
WORD CTableFrameSink::GetBaoziProvideUser(WORD wChairID)
{
	tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
	if (pWeaveItem == nullptr) return false;

	BYTE cbItemCount = m_cbWeaveItemCount[wChairID];

	// �������;
	if (cbItemCount >= MAX_WEAVE - 1)
	{
		// �Ƿ��а���;
		BYTE cbAnGang = 0;

		BYTE cbSameItemCount[GAME_PLAYER] = { 0 };
		for (int i = 0; i < MAX_WEAVE - 1; i++)
		{
			if (pWeaveItem[i].wProvideUser != wChairID)
			{
				cbSameItemCount[pWeaveItem[i].wProvideUser]++;
			}

			if ((pWeaveItem[i].dwWeaveKind&WIK_AGANG) != 0)
			{
				cbAnGang++;
			}
		}

		for (int j = 0; j < GAME_PLAYER; j++)
		{
			if (cbSameItemCount[j] + cbAnGang >= 3)
			{
				return j;	// ����
			}
		}
	}
	
	return INVALID_CHAIR;
}

////�Ƿ��в˰���
//bool CTableFrameSink::IsBaoZi(WORD wChairID)
//{
//	tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
//	if (pWeaveItem == nullptr) return false;
//
//	BYTE cbItemCount = m_cbWeaveItemCount[wChairID];
//
//	// ���˰���
//	if (cbItemCount >= MAX_WEAVE - 1)
//	{
//		return IsSanPeng(wChairID);
//	}
//
//	return false;
//}

//Ԥ�ж��Ƿ���Գɰ���;
bool CTableFrameSink::AnalyseBaoZi(WORD wChairID, WORD wProvideUser, BYTE cbCenterCard)
{
	if (IsGameRuleBaoZi() == false)
	{
		return false;
	}

	tagGangPaiResult GangPaiResult;

	DWORD dwUserAction = WIK_NULL;

	dwUserAction |= m_GameLogic.AnalysePengPai(m_cbCardIndex[wChairID], cbCenterCard);
	dwUserAction |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], cbCenterCard, GangPaiResult, false);

	// ���Ӱ��Ӻ��Ƽ��(��������ܣ��ж����ܺ��Ƿ�ɰ��ӣ��������ֱ����ʾ��Һ���);
	if ((dwUserAction & (WIK_PENG | WIK_MGANG | WIK_AGANG)) != 0)
	{
		// �Ѿ��Ƕ���ͬһ����������ˣ��ڶ�����һ��һ�죬���������;
		if (IsSanPeng(wChairID))
		{
			return true;
		}

		// �ǲ���������һɫ;
		if (IsSiPengQingyise(wChairID, cbCenterCard, dwUserAction))
		{
			return true;
		}
	}

	return false;
}

//ʹ���ƿ�
void CTableFrameSink::UseCardLibrary()
{
	WORD wChairID = INVALID_CHAIR;

	//�ҵ�������ҵ�λ��
	for (BYTE i = 0; i < GAME_PLAYER; i++)
	{
		//��ȡ�����Ϣ
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem != NULL && pServerUserItem->IsCheatUser())
		//if (pServerUserItem != NULL && pServerUserItem->GetGameID() == 100967)
		{
			wChairID = i;
			break;
		}
	}

	if (wChairID == INVALID_CHAIR)
	{
		return;
	}

	//��ȡ�ƿ�
	tagGameCardLibrary* pCardLib = m_pITableFrame->GetGameCardLibrary(0);
	if (pCardLib == NULL)
	{
		return;
	}

	m_wCheatChairID = wChairID;

	//�ƿ���������
	BYTE cbCardCount = pCardLib->cbCount;
	BYTE aryCardData[LEN_CARD_LIB] = { 0 };
	CopyMemory(aryCardData, pCardLib->cbData, cbCardCount);

	//��������
	BYTE cbHuaCount = 0;
	BYTE aryHuaData[MAX_COUNT] = {0};
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (m_GameLogic.IsHuaCard(aryCardData[i]))
		{
			aryHuaData[cbHuaCount++] = aryCardData[i];
		}
	}

	if (cbHuaCount > 0)
	{
		m_cbLeftCheatCardCount = cbHuaCount;

		//ɾ���ƿ��еĻ���
		RemoveCards(aryCardData, LEN_CARD_LIB, aryHuaData, cbHuaCount);

		//������װ��
		CopyMemory(m_cbCheatCard, aryCardData, cbHuaCount);
		CopyMemory(aryCardData, aryHuaData, cbHuaCount);
	}

	//�Ȱ��ƿ��е���ɾ��
	BYTE cbMoveCount = (MAX_COUNT - 1);
	bool bRemoveSuccess = RemoveCards(m_cbRepertoryCard, CountArray(m_cbRepertoryCard), pCardLib->cbData, pCardLib->cbCount);
	if (bRemoveSuccess )
	{
		if (cbHuaCount>0)
		{
			memmove(m_cbRepertoryCard + cbHuaCount, m_cbRepertoryCard, CountArray(m_cbRepertoryCard) - pCardLib->cbCount);
			CopyMemory(m_cbRepertoryCard, m_cbCheatCard, cbHuaCount);
		}

		//���ƴӺ��濪ʼ���������Ϊ0ʱ���������13���ƣ���ʱֱ�Ӱ������ƿ�����ȥ���ɣ������Ϊ0�ȰѶ�Ӧλ�����ƶ�������ٰ������ƿ�����ȥ
		WORD wDesPos = CountArray(m_cbRepertoryCard) - ((wChairID + 1)*cbMoveCount);
		if (wChairID > 0)
		{
			CopyMemory(&m_cbRepertoryCard[CountArray(m_cbRepertoryCard) - cbMoveCount], &m_cbRepertoryCard[wDesPos], cbMoveCount);
		}
		CopyMemory(&m_cbRepertoryCard[wDesPos], aryCardData, cbMoveCount);
	}
}

//��֤ɾ��
bool CTableFrameSink::RemoveCards(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//��������
	ASSERT(cbRemoveCount <= cbCardCount);

	BYTE cbDeleteCount = 0;
	BYTE cbTempCardData[MAX_REPERTORY] = { 0 };

	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));
	//�����˿�
	for (BYTE i = 0; i < cbRemoveCount; i++)
	{
		for (BYTE j = 0; j < cbCardCount; j++)
		{
			if (cbRemoveCard[i] == cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j] = 0;
				break;
			}
		}
	}
	//�ɹ��ж�
	if (cbDeleteCount != cbRemoveCount)
	{
		ASSERT(FALSE);
		return false;
	}
	//�����˿�
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0)
			cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	return true;
}

//������Ϸ����
void CTableFrameSink::SetGameServerOpertion(tagGameServiceOption *pGso)
{
	ASSERT(pGso != NULL);
	m_pGameServiceOption = pGso;
}

//��ȡ����������
bool CTableFrameSink::ReadServerConfig()
{
	//��ȡ�������ⶥ����
	TCHAR szFileName[MAX_PATH] = TEXT("");
	GetWorkDirectory(szFileName, CountString(szFileName));
	StringCchCat(szFileName, CountString(szFileName), TEXT("\\NJMJServer.ini"));
	if (!::PathFileExists(szFileName))
	{
		ASSERT(FALSE);
		return false;
	}
	try
	{
		//�Ƿ����齱
		m_cbOpenGameLottery = (BYTE)GetPrivateProfileInt(TEXT("ServerConfig"), TEXT("OpenGameLottery"), 0, szFileName);

		CString strKey;
		//��ȡ��������
		strKey.Format(_T("Prize%u"), m_pGameServiceOption->wServerID);
		GetPrivateProfileString(_T("LuckyDraw"), strKey, NULL, m_strLucyDrawCfg.GetBuffer(512), 512, szFileName);
		m_strLucyDrawCfg.ReleaseBuffer();
		//��ȡ�����ֵ
		strKey.Format(_T("MaxExp%u"), m_pGameServiceOption->wServerID);
		m_lMaxPlazaExp = static_cast<LONG>(GetPrivateProfileInt(_T("LuckyDraw"), strKey, 0, szFileName));
		return true;
	}
	catch (...)
	{
		ASSERT(false);
		return false;
	}
}

//�������õ�ע��Ϣ
bool CTableFrameSink::SendSetBaseScore(WORD wChairID, BYTE cbBaseScoreType)
{
	CMD_S_SetBaseScore stSetBaseScore;
	ZeroMemory(&stSetBaseScore, sizeof(stSetBaseScore));
	stSetBaseScore.lBaseScore = m_lBaseScore;
	stSetBaseScore.lCellScore = m_pGameServiceOption->lCellScore;
	stSetBaseScore.lMaxCellScore = (LONG)m_pGameServiceOption->lMaxEnterScore;
	stSetBaseScore.cbSBSStatus = cbBaseScoreType;
	m_pITableFrame->SendTableData(wChairID, SUB_S_SET_BASESCORE, &stSetBaseScore, sizeof(stSetBaseScore));

	//���˽������¼�ʱ
	if (cbBaseScoreType == STATUS_SBS_NOTIFY)
	{
		m_pITableFrame->KillGameTimer(IDI_BASESCORE);
		m_pITableFrame->SetGameTimer(IDI_BASESCORE, IDT_BASESCORE * 1000 + 500, 1, 0);
	}
	return true;
}

//���֪ͨ��ע
bool CTableFrameSink::CheckNotifyBaseScore()
{
	bool bFind = false;

	//���������ȷ��ͬ������������
	for (IteListPlayer ite = m_listAgreeBaseScore.begin(); ite != m_listAgreeBaseScore.end(); ++ite)
	{
		WORD wChairID = *ite;
		if (wChairID >= 0 && wChairID < GAME_PLAYER)
		{
			if (!m_bAgreeBaseScore[wChairID])
			{
				SendSetBaseScore(wChairID, STATUS_SBS_NOTIFY);
				bFind = true;
			}
		}
	}
	return bFind;
}

//�����ҽ��
bool CTableFrameSink::CheckMoneyLimit(LONG lScoreLimit)
{
	bool bKick = false;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pUserItem != NULL)
		{
			LONG lScore = (LONG)pUserItem->GetUserScore();
			if (lScore < lScoreLimit)
			{
				SendKickOutMsg(i);
				bKick = true;
			}
		}
	}
	return bKick;
}

//�߳����
void CTableFrameSink::SendKickOutMsg(WORD ChairID)
{
	IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(ChairID);
	if (pIServerUser)
	{
		CMD_S_KickOut KickOut;
		KickOut.cbReasion = 0;
		KickOut.wChairID = ChairID;
		m_pITableFrame->SendTableData(ChairID, SUB_S_KICK_OUT, &KickOut, sizeof(KickOut));
	}
}

//����Ŀ¼
bool CTableFrameSink::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//�����ļ�
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == TEXT('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//���ý��
	ASSERT(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

/////////////////////////////// ��Ϸ¼�� ///////////////////////////////////////////

void CTableFrameSink::addGameRecordAction(WORD wSubCmdID, void* pSubMessage, int nMessageSize)
{
	tagGameRecordOperateResult kOperateRecord;
	kOperateRecord.wSubCmdID = wSubCmdID;
	kOperateRecord.subMessageData.pushValue(pSubMessage, nMessageSize);
	m_kGameRecord.actionVec.push_back(kOperateRecord);
}

void CTableFrameSink::starGameRecord()
{
	m_kGameRecord = tagGameRecord();

	m_kGameRecord.wBankerUser	= m_wBankerUser;
	m_kGameRecord.wEastUser		= m_wEastUser;

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);
		tagUserInfo* pUserInfo = pUserItem->GetUserInfo();
		if (pUserItem == NULL || NULL == pUserInfo)
		{
			continue;
		}

		tagGameRecordPlayer kReocrdPlayer;
		kReocrdPlayer.dwUserID		= pUserItem->GetUserID();
		//kReocrdPlayer.dwGameID		= pUserItem->GetGameID();
		kReocrdPlayer.lScore		= pUserItem->GetUserScore();
		//kReocrdPlayer.strHead		= pUserInfo->szHeadHttp;
		kReocrdPlayer.strNickName	= pUserItem->GetNickName();

		BYTE cbCardData[MAX_COUNT] = {0};
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i], cbCardData);
		for (int i = 0; i < MAX_COUNT; i++)
		{
			kReocrdPlayer.cbCardData.push_back(cbCardData[i]);
		}
		m_kGameRecord.playersVec.push_back(kReocrdPlayer);
	}
}

////////////////////////////// ˽�˳� ////////////////////////////////////////////
// ����ͳ������
void CTableFrameSink::ResetPrivateEndInfo()
{
	ZeroMemory(&m_PrivateEndInfo, sizeof(CMD_S_Private_End_Info));
}

// ���˽�˳�������Ϣ��
void CTableFrameSink::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{
	//for (WORD i = 0; i < GAME_PLAYER; i++)
	//{
	//	// �ܻ���
	//	IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
	//	if (pServerUserItem)
	//	{
	//		SCORE lScore = pServerUserItem->GetUserScore();
	//		m_PrivateEndInfo.lAllScore[i] = lScore;
	//	}
	//}

	m_PrivateEndInfo.StreamValue(kDataStream, bSend);
}

//�ж�˽�˳��Ƿ����
bool CTableFrameSink::IsPrivateEnd()
{
	if (m_pITableFrame == nullptr) return false;

	tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	tagPrivateFrameRecordInfo* pPrivateRecord = m_pITableFrame->GetPrivateFrameRecord();

	if ((pPrivateFrame == nullptr) || (pPrivateRecord == nullptr)) return false;

	if (pPrivateFrame->cbGameCout <= pPrivateRecord->cbFinishCout)
	{
		return true;
	}

	//�ж��Ƿ�����һ���Ϊ0
	BYTE cbHasZeroScore = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem* pIUserItem = m_pITableFrame->GetTableUserItem(i);
		ASSERT(pIUserItem != NULL);
		if (pIUserItem == NULL) continue;

		if (pIUserItem->GetUserScore() <= 0)
		{
			cbHasZeroScore++;
		}
	}

	// ���˻���Ϊ0�ͽ�ɢ��Ϸ
	if (cbHasZeroScore >= 2)
	{
		return true;
	}

	return false;
}

bool CTableFrameSink::IsHasGameRule(eNJMJRuleEnum eType)
{
	if (m_pITableFrame == NULL) return false;

	const tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	if (NULL == pPrivateFrame)	return false;
	return ((pPrivateFrame->dwGameRule & eType)!= 0);
}

//�Ƿ��а���;
bool CTableFrameSink::IsGameRuleBaoZi()
{
	if (m_pITableFrame == NULL) return false;

	const tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	if (NULL == pPrivateFrame)	return false;
	return ((pPrivateFrame->dwGameRule & eNJMJRuleEnum_BAO) == 0);
}

//�Ƿ��ǽ�԰��;
bool CTableFrameSink::IsGameRuleJYZ()
{
	if (m_pITableFrame == NULL) return false;

	const tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	if (NULL == pPrivateFrame)	return false;
	return ((pPrivateFrame->dwGameRule & eNJMJRuleEnum_JYZ) == 0);
}

// �۷ּ�¼
void CTableFrameSink::PayMoneyLog(CMD_S_PayMoney & stPayMoney)
{
	// ¼���¼;
	addGameRecordAction(SUB_S_PAY_MONEY, &stPayMoney, sizeof(CMD_S_PayMoney));

#if IS_OPEN_PRINT_LOG
	// ��¼��־
	MyLog("�۷���־ ===================== cbPayReason = %d ", stPayMoney.cbPayReason);
	MyLog("ӮǮ��   ===================== wPayToUser = %d,  lPayToNumber = %ld", stPayMoney.wPayToUser, stPayMoney.lPayToNumber);

	LONG lAllPayMoney = stPayMoney.lPayToNumber;

	for (WORD i = 0; i < GAME_PLAYER -1 ; i++)
	{
		if (stPayMoney.lPayFromNumber == 0) continue;

		MyLog("��Ǯ��  ===================== wPayFromUser=%d, lPayFromNumber=%ld", stPayMoney.wPayFromUser[i], stPayMoney.lPayFromNumber[i]);

		lAllPayMoney += stPayMoney.lPayFromNumber[i];
	}

	ASSERT(lAllPayMoney == 0);
#endif
}

void CTableFrameSink::MyLog(TCHAR *szLog, ...)
{
	CString strLog = "";
	va_list ap;
	va_start(ap, szLog);
	strLog.FormatV(szLog, ap);
	va_end(ap);
	
	CString strTmpLog = "";
	strTmpLog.Format("TableID = %d, %s.\n", m_pITableFrame->GetTableID(), strLog);

	::OutputDebugString(strTmpLog.GetBuffer());
}
//////////////////////////////////////////////////////////////////////////