#include "StdAfx.h"
#include "TableFrameSink.h"
//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//��Ϸ����


#define	IDI_TIME_GAME				1				//��Ϸ��ʱ��

#define IDI_GAME_COMPAREEND			2				//������ʱ��

#define	IDI_TIME_GameCard			3				//�����ӳ�
#define IDI_GAME_NORMALEND			4				//����������ʱ��
#define IDI_TIME_GAMEOVER           5				//��Ϸ����
#define	IDT_CHIPTIMER			    17000			//��Ϸ��ʱ��ʱ��
#define IDI_GAME_START              6               //��ע�ӳ�
#define	IDI_TIME_ADDBUFFER			7				//�������ӳ�
#define	IDI_TIME_SENDCARD			8				//���ƽ׶�

#define IDI_GAME_NO_PLAYER			9				//��������

#define IDI_AGREE_BASESCORE         10
#define IDT_AGREE_BASESCORE         (25*1000 + 500) //ͬ���ע

//////////////////////////////////////////////////////////////////////////

void TraceMessage(LPCTSTR pszMessage);

LONG CTableFrameSink::m_ISetDJPK[9];

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//�������
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;

	//��ұ���
	m_wDUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	//m_wWinner = INVALID_CHAIR;

	//���״̬
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));

	//�˿˱���
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData, sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	//ZeroMemory(m_nDaShangCount, sizeof(m_nDaShangCount));

	//��ע����
	m_lCellScore = 0L;
	m_lTurnLessScore = 0L;
	m_lAddLessScore = 0L;
	m_wOperaCount = 0;
	m_cbBalanceCount = 0;
	m_lBalanceScore = 0L;
	m_ChipPoolCount = 0;
	//m_OnPoolBuffer  = 0;
	m_lAllScore = 0L;
	m_cbChipTypeID = 0;
	m_GameOver = false;
	m_pHostChairID = 0xff;
	m_lBaseScore = 0;
	m_bGameStart = false;
	m_nPassCardCount = 0;
	ZeroMemory(m_bAgreeBase, sizeof(m_bAgreeBase));
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore, sizeof(m_lTotalScore));
	ZeroMemory(m_cbShowHand, sizeof(m_cbShowHand));
	ZeroMemory(m_cbHandChair, sizeof(m_cbHandChair));
	memset(m_cbHandChairCount, 0, sizeof(m_cbHandChairCount));
	memset(m_aryChipPool, 0, sizeof(m_aryChipPool));
	memset(m_wMUser, 0, sizeof(m_wMUser));
	memset(m_lTurnMaxScore, 0, sizeof(m_lTurnMaxScore));
	memset(m_aryEndScore, 0, sizeof(m_aryEndScore));
	memset(m_SendCardEnd, 0, sizeof(m_SendCardEnd));
	memset(m_cbGiveUpStatus, 0, sizeof(m_cbGiveUpStatus));

	memset(m_SuoHaCountBuffer, 0, sizeof(m_SuoHaCountBuffer));
	memset(m_lUserTakeGold, 0, sizeof(m_lUserTakeGold));
	memset(m_lUserMaxScore, 0, sizeof(m_lUserMaxScore));
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
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
	m_pHostChairID = 0xff;
	m_lBaseScore = 0;
	m_bGameStart = false;
	ZeroMemory(m_bAgreeBase, sizeof(m_bAgreeBase));

	//��ȡ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption != NULL);
	SetDzpkInit();

	return true;
}

//��λ����
void CTableFrameSink::RepositionSink()
{
	//��ұ���
	m_wCurrentUser = INVALID_CHAIR;
	//���״̬
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	//�˿˱���
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData, sizeof(m_cbCenterCardData));
	//ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	//ZeroMemory(m_nDaShangCount,sizeof(m_nDaShangCount));

	//��ע����
	m_lTurnLessScore = 0L;
	m_lAddLessScore = 0L;
	m_wOperaCount = 0;
	m_cbBalanceCount = 0;
	m_lBalanceScore = 0L;

	m_ChipPoolCount = 0;
	//m_OnPoolBuffer  = 0;
	m_lAllScore = 0;
	m_cbChipTypeID = 0;
	m_nPassCardCount = 0;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore, sizeof(m_lTotalScore));
	ZeroMemory(m_cbShowHand, sizeof(m_cbShowHand));
	ZeroMemory(m_cbHandChair, sizeof(m_cbHandChair));
	memset(m_lTurnMaxScore, 0, sizeof(m_lTurnMaxScore));
	memset(m_cbHandChairCount, 0, sizeof(m_cbHandChairCount));
	memset(m_aryChipPool, 0, sizeof(m_aryChipPool));
	memset(m_aryEndScore, 0, sizeof(m_aryEndScore));
	memset(m_SendCardEnd, 0, sizeof(m_SendCardEnd));
	memset(m_cbGiveUpStatus, 0, sizeof(m_cbGiveUpStatus));
	memset(m_SuoHaCountBuffer, 0, sizeof(m_SuoHaCountBuffer));
	memset(m_lTotalScore, 0, sizeof(m_lTotalScore));
	memset(m_lUserMaxScore, 0, sizeof(m_lUserMaxScore));
	ZeroMemory(m_TabelScoreBuffer, sizeof(m_TabelScoreBuffer));

	return;
}

//
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID < m_wPlayerCount);
	return (m_bFristPlayStatus[wChairID]);
}

//�û����� 
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (!bLookonUser)
	{
		m_lUserTakeGold[wChairID] = 0;  //��Ҵ���
		m_lUserMaxScore[wChairID] = 0;

		//if (IsGenreGold())
		//{
		//	if (find(m_listPlayers.begin(), m_listPlayers.end(), wChairID) == m_listPlayers.end())
		//	{
		//		m_listPlayers.push_back(wChairID);
		//		TCHAR szString[256];
		//		_sntprintf_s(szString, CountArray(szString), TEXT("%d��%dλ ������� m_lBaseScore = %d m_pHostChairID = %d\n"), m_pITableFrame->GetTableID(), wChairID, m_lBaseScore, m_pHostChairID);
		//		//WriteLog(buf);
		//		m_bAgreeBase[wChairID] = 0;  //������� �ѵ�ǰ��λ��Ϊδͬ���ע����

		//		int userCount = 0;
		//		for (int i = 0; i < m_wPlayerCount; ++i)
		//		{
		//			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

		//			if (NULL != pIServerUserItem)
		//			{
		//				++userCount;
		//			}
		//		}

		//		if (0xff == m_pHostChairID && 0 == m_lBaseScore && 1 == userCount)  //����������Ϣ ���������õ�ע
		//		{
		//			m_pHostChairID = wChairID;
		//		}
		//		else
		//		{
		//			if (find(m_listConfirmBase.begin(), m_listConfirmBase.end(), wChairID) == m_listConfirmBase.end())
		//			{
		//				m_listConfirmBase.push_back(wChairID);
		//			}
		//		}

		//		if (!m_bGameStart && pIServerUserItem->IsAndroidUser()) //���������õ�ע
		//		{
		//			DoUserSitDown(pIServerUserItem);  //�������� ȷ�� ��ע ��Ϣ
		//		}
		//	}
		//}
		//else
		{
			//����ӵ�еĽ����
			SCORE lScore = m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore();

			m_lUserTakeGold[wChairID] = lScore;  //��Ҵ���
			m_lUserMaxScore[wChairID] = lScore;
		}
	}
	return true;
}

// ���׼��
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	// ���ǲ�Ϊ�Թ�״̬
	if (pIServerUserItem->GetUserStatus() != US_LOOKON)
	{

	}

	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (!bLookonUser)
	{
		//if (IsGenreGold())
		//{
		//	IteListPlayer ite = find(m_listPlayers.begin(), m_listPlayers.end(), wChairID);
		//	if (ite != m_listPlayers.end())
		//	{
		//		m_listPlayers.erase(ite);

		//		if (0 == m_lBaseScore && wChairID == m_pHostChairID) //�����뿪 ����û�����õ�ע
		//		{
		//			if (m_listPlayers.size() > 0)
		//			{
		//				m_pHostChairID = m_listPlayers.front();
		//				SendSetBaseScore(m_pITableFrame->GetTableUserItem(m_pHostChairID), SETBASE_SET);
		//				m_pITableFrame->KillGameTimer(IDI_AGREE_BASESCORE);
		//				m_pITableFrame->SetGameTimer(IDI_AGREE_BASESCORE, IDT_AGREE_BASESCORE, 1, m_wCurrentUser); //������ʱ��
		//			}
		//			else
		//			{
		//				m_pHostChairID = 0xff;
		//			}
		//		}
		//		else if (m_listPlayers.empty())
		//		{
		//			m_pHostChairID = 0xff;
		//			m_lBaseScore = 0;
		//		}

		//		m_bAgreeBase[wChairID] = 0;  //����뿪 �ѵ�ǰ��λ��Ϊδͬ���ע����

		//		IteListPlayer iteConfirm = find(m_listConfirmBase.begin(), m_listConfirmBase.end(), wChairID);
		//		if (iteConfirm != m_listConfirmBase.end())
		//			m_listConfirmBase.erase(iteConfirm);
		//	}
		//}
		//else
		//{

		//}
	}

	return true;
}

//��ȡ�����ļ�
BOOL CTableFrameSink::SetDzpkInit(BYTE ID)
{
	//�����ļ���
	//Сäע5Ϊ��Ԫע��0.5��,10Ϊ1.0��
	m_ISetDJPK[0] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom0"), 10, CGameLogic::m_strFileName);
	//��äע5Ϊ��Ԫע��0.5��,10Ϊ1.0��
	m_ISetDJPK[1] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom1"), 20, CGameLogic::m_strFileName);
	//�ڶ��ֵ������ע
	m_ISetDJPK[2] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom2"), 20, CGameLogic::m_strFileName);
	//����������ҵ����Ͷע
	m_ISetDJPK[3] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom3"), 0, CGameLogic::m_strFileName);
	m_ISetDJPK[4] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom4"), 300, CGameLogic::m_strFileName);

	m_ISetDJPK[5] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom5"), 600, CGameLogic::m_strFileName);
	m_ISetDJPK[6] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom6"), 200, CGameLogic::m_strFileName);
	m_ISetDJPK[7] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom7"), 150, CGameLogic::m_strFileName);
	m_ISetDJPK[8] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom8"), 50, CGameLogic::m_strFileName);

	m_UserAllScore = GetPrivateProfileInt(TEXT("DZPKSET"), TEXT("UserWin"), 0, CGameLogic::m_strFileName2);	//��ȡ��ҵ�����Ӯֵ
	m_UserWinOk = GetPrivateProfileInt(TEXT("DZPKSET"), TEXT("UserWinOk"), 0, CGameLogic::m_strFileName2);	//�Ƿ��������ƻ���

	//m_lAddTakeMinTimes = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("AddTakeGoldMin"), 100, CGameLogic::m_strFileName);
	//m_lAddTakeMaxTimes = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("AddTakeGoldMax"), 1000, CGameLogic::m_strFileName);
	int TableVipSet = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("TableVipLimit"), 95, CGameLogic::m_strFileName);
	m_fTableVipLimit = (float)TableVipSet / 10;  //9.5��

	return TRUE;
}

//��ȡ�����ļ�
bool CTableFrameSink::CardExchange(BYTE ID)
{
	if (m_UserAllScore <= 0) return false;
	bool AndroidUser = false, Buser = false;
	WORD AndroidUserid = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		//��Ч�û�
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->IsAndroidUser())
		{
			AndroidUserid = i;
			AndroidUser = true;
		}
		else
		{
			Buser = true;
		}
	}
	BYTE bLastCenterALLCardData[GAME_PLAYER][MAX_CENTERCOUNT] = { 0 };//��������˿�	
	if (!AndroidUser || !Buser) return false;
	BYTE MaxId = 10;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (10 == MaxId)
		{
			MaxId = i;
		}
		m_GameLogic.FiveFromSeven(m_cbHandCardData[i], 2, m_cbCenterCardData, 5, bLastCenterALLCardData[i], 5);
	}
	if (10 == MaxId || MaxId == GAME_PLAYER - 1) return false;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (MaxId == i)continue;
		if (pIServerUserItem == NULL) continue;

		if (m_GameLogic.CompareCard(bLastCenterALLCardData[i], bLastCenterALLCardData[MaxId], 5)>1)
		{
			MaxId = i;
		}
	}
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(MaxId);
	if (pIServerUserItem == NULL) return false;
	if (pIServerUserItem->IsAndroidUser())
	{
		return false;
	}

	if (rand() % 1000 > m_UserWinOk) return false;
	BYTE cbTempCardData[MAX_COUNT] = { 0 };
	CopyMemory(cbTempCardData, m_cbHandCardData[MaxId], MAX_COUNT*sizeof(BYTE));
	CopyMemory(m_cbHandCardData[MaxId], m_cbHandCardData[AndroidUserid], MAX_COUNT*sizeof(BYTE));
	CopyMemory(m_cbHandCardData[AndroidUserid], cbTempCardData, MAX_COUNT*sizeof(BYTE));
	return true;
}

//�������������ע
void CTableFrameSink::CalUserMaxChipGold(WORD wChairID)
{
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	SCORE lScore = pIServerUserItem->GetUserScore();

	m_lUserTakeGold[wChairID] = lScore;		// ͬ���´�����
	m_lUserMaxScore[wChairID] = lScore;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{
	m_bGameStart = true;

	//����״̬
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);
	RepositionSink();

	// ��Ϸ��ʼ��ʱ����������ݣ�����������Ϸ����������
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_lUserTakeGold, sizeof(m_lUserTakeGold));
	srand(GetTickCount() + m_pITableFrame->GetTableID());
	SetDzpkInit();
	//��Ϸ����	
	BYTE wUserCount = 0;
	//��ע����
	if (m_lCellScore <= 0)
	{
		m_lCellScore = m_pITableFrame->GetCellScore();
	}

	memset(m_bFristPlayStatus, 0, sizeof(m_bFristPlayStatus));
	memset(m_cbShowHandBuffer, 0, sizeof(m_cbShowHandBuffer));
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		//��Ч�û�
		if (pIServerUserItem == NULL) continue;

		CalUserMaxChipGold(i);
		//����״̬
		m_cbPlayStatus[i] = TRUE;
		m_bFristPlayStatus[i] = true;

		wUserCount++;
	}

	//�����˿�	
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard, sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard, wUserCount*MAX_COUNT + MAX_CENTERCOUNT);

	//�û��˿�
	WORD wCardCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			CopyMemory(&m_cbHandCardData[i], &cbRandCard[(wCardCount++)*MAX_COUNT], sizeof(BYTE)*MAX_COUNT);
		}
	}

	//�����˿�
	CopyMemory(m_cbCenterCardData, &cbRandCard[wUserCount*MAX_COUNT], CountArray(m_cbCenterCardData));
	CardExchange(0);

	wCardCount = 0;
	//�˿���Ŀ
	m_cbSendCardCount = 0;
	m_cbBalanceCount = 0;
	//�׼��ж�
	if (m_wDUser == INVALID_CHAIR)
	{
		m_wDUser = 0;
	}
	else
	{
		m_wDUser = (m_wDUser + 1) % GAME_PLAYER;
	}

	while (m_cbPlayStatus[m_wDUser] == FALSE)
	{
		m_wDUser = (m_wDUser + 1) % m_wPlayerCount;
	}

	//äע���
	WORD wPlayer[] = { INVALID_CHAIR, INVALID_CHAIR, INVALID_CHAIR };
	WORD wPlayerCount = 0;
	WORD wNextUser = m_wDUser;
	do
	{
		wNextUser = (wNextUser + 1) % GAME_PLAYER;
		if (m_cbPlayStatus[wNextUser] == TRUE && m_cbShowHand[wNextUser] == 0)
		{
			wPlayer[wPlayerCount++] = wNextUser;
		}
	} while (wPlayerCount < 3);

	//��ǰ��ע
	m_lTableScore[wPlayer[0]] = m_lCellScore*m_ISetDJPK[0] / 10;
	m_lTableScore[wPlayer[1]] = m_lCellScore*m_ISetDJPK[1] / 10;
	if (m_lTableScore[wPlayer[0]] <= 0)
	{
		m_lTableScore[wPlayer[0]] = m_lCellScore;
	}
	m_lTotalScore[wPlayer[0]] = m_lTableScore[wPlayer[0]];
	m_lTotalScore[wPlayer[1]] = m_lTableScore[wPlayer[1]];
	m_TabelScoreBuffer[wPlayer[0]] = m_lTableScore[wPlayer[0]];
	m_TabelScoreBuffer[wPlayer[1]] = m_lTableScore[wPlayer[1]];
	m_lUserTakeGold[wPlayer[0]] -= m_lTableScore[wPlayer[0]];
	m_lUserTakeGold[wPlayer[1]] -= m_lTableScore[wPlayer[1]];

	//������С��ע
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_cbPlayStatus[i] == FALSE)
			continue;

		m_lTableScore[i] += m_lCellScore*m_ISetDJPK[3] / 10;
		m_lTotalScore[i] += m_lCellScore*m_ISetDJPK[3] / 10;
		if (m_lTableScore[i] >= m_lUserMaxScore[i])
		{
			m_lTableScore[i] = m_lUserMaxScore[i];
			m_lTotalScore[i] = m_lUserMaxScore[i];
			m_lUserTakeGold[i] = 0;
			m_cbShowHand[i] = TRUE;
			m_cbShowHandBuffer[i] = TRUE;
			m_SuoHaCountBuffer[m_cbBalanceCount]++;
		}

		m_lTurnMaxScore[i] = m_lUserMaxScore[i] - m_lTotalScore[i];
	}

	wPlayerCount = 0;
	if (m_cbShowHand[wPlayer[2]])//���
	{
		for (int i = 1; i < GAME_PLAYER; ++i)
		{
			wPlayerCount++;
			wPlayer[2] = (wPlayer[2] + i) % GAME_PLAYER;
			if (m_cbPlayStatus[wPlayer[2]] && !m_cbShowHand[wPlayer[2]])
			{
				wPlayerCount = 0;
				break;
			}
		}
	}

	if (wPlayerCount != 0)
	{
		m_wCurrentUser = INVALID_CHAIR;
	}
	else
	{
		m_wCurrentUser = wPlayer[2];
	}

	ASSERT(m_wCurrentUser != INVALID_CHAIR);

	m_lBalanceScore = m_lTableScore[wPlayer[1]];
	m_lAddLessScore = m_lCellScore + m_lTurnLessScore;
	m_wMUser[0] = wPlayer[0];
	m_wMUser[1] = wPlayer[1];
	if (VALIDE_CHAIRID(m_wCurrentUser))
	{
		m_lTurnLessScore = m_lBalanceScore - m_lTotalScore[m_wCurrentUser];
	}
	else
	{
		m_lTurnLessScore = m_lBalanceScore;
	}

	//�������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));
	GameStart.wDUser = m_wDUser;
	GameStart.wSmallChipInUser = wPlayer[0];
	GameStart.wMaxChipInUser = wPlayer[1];
	GameStart.wCurrentUser = m_wCurrentUser;
	GameStart.lCellScore = m_lCellScore;
	GameStart.lAddLessScore = m_lAddLessScore;
	GameStart.lTurnLessScore = m_lTurnLessScore;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		GameStart.cbPlayStatus[i] = m_cbPlayStatus[i];
		if (m_cbPlayStatus[i] == FALSE)
			continue;
		GameStart.lTurnMaxScore[i] = m_lTurnMaxScore[i];
		GameStart.lUserMaxScore[i] = m_lUserMaxScore[i];
		GameStart.cbShowHand[i] = m_cbShowHand[i];
		GameStart.lTableScore[i] = m_lTableScore[i];
	}
	GameStart.UserCount = wUserCount;
	GameStart.lAddSmall = m_lCellScore*m_ISetDJPK[0] / 10;
	GameStart.lAddMax = m_lCellScore*m_ISetDJPK[1] / 10;

	ZeroMemory(&m_GameRobot, sizeof(m_GameRobot));
	CopyMemory(m_GameRobot.clLbei, m_ISetDJPK, sizeof(m_ISetDJPK));
	CopyMemory(m_GameRobot.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
	CopyMemory(m_GameRobot.cbCenterCardData, m_cbCenterCardData, sizeof(m_cbCenterCardData));

	//����/©������	
	//��������
	int count1 = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem != NULL && pServerUserItem->IsAndroidUser())
			{
				int Ai = rand() % 1000;
				int iIndex = 0;
				if (Ai < m_ISetDJPK[5])
				{
					iIndex = 0;//��ͨ
				}
				else if (m_ISetDJPK[5] <= Ai && Ai <= m_ISetDJPK[5] + m_ISetDJPK[6])
				{
					iIndex = 1;//��̬
				}
				else if (m_ISetDJPK[5] + m_ISetDJPK[6] <= Ai && Ai <= m_ISetDJPK[5] + m_ISetDJPK[6] + m_ISetDJPK[7])
				{
					iIndex = 2;//ƽӹ
				}
				else
				{
					iIndex = 3;//����
				}
				CopyMemory(m_GameRobot.lRobotAdd, CGameLogic::m_lAdd[iIndex], sizeof(m_GameRobot.lRobotAdd));
				CopyMemory(m_GameRobot.lRobotQuanYa, CGameLogic::m_lQuanYa[iIndex], sizeof(m_GameRobot.lRobotQuanYa));
				CopyMemory(m_GameRobot.lRobotGiveUp, CGameLogic::m_lGiveUp[iIndex], sizeof(m_GameRobot.lRobotGiveUp));
				m_pITableFrame->SendTableData(i, SUB_S_USER_ROBOT, &m_GameRobot, sizeof(m_GameRobot));
			}
			//��������		
			count1++;
			//CopyMemory(GameStart.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
			CopyMemory(GameStart.cbCardData, m_cbHandCardData[i], MAX_COUNT);
			m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &GameStart, sizeof(GameStart));

		}
		else
		{
			GameStart.cbPlayStatus[i] = FALSE;
		}
	}

	// ��������������Ƶ��Թ��������
	ZeroMemory(GameStart.cbCardData, MAX_COUNT);
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	m_pITableFrame->SetGameTimer(IDI_TIME_SENDCARD, wUserCount * 1000 + 2000, 1, 0);

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
		case GER_DISMISS:		//��Ϸ��ɢ
		{
			if (m_GameOver)
			{
				return true;
			}
			KillTimerAll();
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			m_bGameStart = false;
			return true;
		}
		case GER_NORMAL:		//�������
		{
			if (m_GameOver)
			{
				return true;
			}

			m_bGameStart = false;	//��Ϸ������ʶ

			try
			{
				m_GameOver = true;
				KillTimerAll();

				//�������
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd, sizeof(GameEnd));
				GameEnd.cbTotalEnd = 1;

				//�˿�����
				BYTE cbEndCardData[GAME_PLAYER][MAX_CENTERCOUNT];
				BYTE cbCardTypeData[MAX_CENTERCOUNT];
				ZeroMemory(cbEndCardData, sizeof(cbEndCardData));
	
				BYTE cbUserCount = 0;

				try
				{
					//��ȡ�˿�
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						if (m_cbPlayStatus[i] == FALSE) continue;

						//ͳ����������Ϸ������������������������
						cbUserCount++;

						ZeroMemory(cbCardTypeData, sizeof(cbCardTypeData));

						//��ȡ�Լ����������
						BYTE cbEndCardKind = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], MAX_COUNT, m_cbCenterCardData, MAX_CENTERCOUNT, cbEndCardData[i], MAX_CENTERCOUNT);

						//����������
						m_GameLogic.FinishCard(cbEndCardData[i], cbCardTypeData, MAX_CENTERCOUNT, m_cbHandCardData[i], cbEndCardKind);
						CopyMemory(GameEnd.cbLastCenterCardData[i], cbEndCardData[i], sizeof(cbEndCardData[i]));

						//��������
						GameEnd.cbCardType[i] = cbEndCardKind;

						//����ʱ���ۼ���ע
						GameEnd.lTotalScore[i] = m_lTotalScore[i];

						ASSERT(cbEndCardKind != FALSE);
					}
				}
				catch (...)
				{
					m_pITableFrame->ExportInformation(TEXT("�û�����v�������"), true);
					ASSERT(FALSE);
				}

				//����ע����
				LONG lTotalScore[GAME_PLAYER];
				ZeroMemory(lTotalScore, sizeof(lTotalScore));
				CopyMemory(lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));

				//ʤ���б�
				UserWinList WinnerList[GAME_PLAYER];
				ZeroMemory(WinnerList, sizeof(WinnerList));

				//��ʱ����
				BYTE bTempData[GAME_PLAYER][MAX_CENTERCOUNT];
				CopyMemory(bTempData, cbEndCardData, sizeof(bTempData));

				WORD wWinCount = 0;
				try
				{
					//�û��÷�˳��
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						//��������û�
						if (!m_GameLogic.SelectMaxUser(bTempData, WinnerList[i], m_cbHandCardData, lTotalScore))
						{
							wWinCount = i;
							break;
						}

						//ɾ��ʤ������
						for (WORD j = 0; j < WinnerList[i].bSameCount; j++)
						{
							WORD wRemoveId = WinnerList[i].wWinerList[j];
							ASSERT(bTempData[wRemoveId][0] != 0);
							ZeroMemory(bTempData[wRemoveId], sizeof(BYTE)*MAX_CENTERCOUNT);
						}
					}
				}
				catch (...)
				{
					m_pITableFrame->ExportInformation(TEXT("�û��÷�˳��"), true);
					ASSERT(FALSE);
				}

				//�����û�
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i] == FALSE && lTotalScore[i]>0l)
					{
						WinnerList[wWinCount].wWinerList[WinnerList[wWinCount].bSameCount++] = i;
					}
					//if(m_cbPlayStatus[i] ==FALSE)continue;				
				}

				//�÷ֱ���
				LONG lUserScore[GAME_PLAYER];
				ZeroMemory(lUserScore, sizeof(lUserScore));

				//�÷����
				for (int i = 0; i < m_wPlayerCount - 1; i++)
				{
					//ʤ������
					int iWinCount = (int)WinnerList[i].bSameCount;

					// ���������Ϊ0ʱ���Ѿ��������
					if (0 == iWinCount)break;

					//ʤ���û��÷����
					for (int j = 0; j < iWinCount; j++)
					{
						//ʤ�����
						WORD wWinChairID = WinnerList[i].wWinerList[j];

						if (0 == lTotalScore[wWinChairID])continue;

						// ����������ͬ��ң�����������ע�����ͬ
						if (j > 0 && lTotalScore[wWinChairID] - lTotalScore[WinnerList[i].wWinerList[j - 1]] == 0)
						{
							continue;
						}

						//��wWinChairID����С�����
						for (int k = i + 1; k < m_wPlayerCount; k++)
						{
							//ʧ������
							if (0 == WinnerList[k].bSameCount)break;

							// ��������ʱ��һ��������
							for (int l = 0; l < WinnerList[k].bSameCount; l++)
							{
								//ʧ�����
								WORD wLoserChairID = WinnerList[k].wWinerList[l];

								//�����ע�Ѿ����������
								if (0 == lTotalScore[wLoserChairID])continue;

								LONG lMinScore = 0;		// ���Ӧ��Ӯȡ���ٻ���

								//�ϼҵ÷���Ŀ
								LONG lLastScore = 0;
								if (j > 0)
								{
									lLastScore = lTotalScore[WinnerList[i].wWinerList[j - 1]];
									if (lLastScore <= 0) continue;
								}

								lMinScore = min(lTotalScore[wWinChairID] - lLastScore, lTotalScore[wLoserChairID]);
								for (int m = j; m < iWinCount; m++)
								{
									//�÷���Ŀ
									lUserScore[WinnerList[i].wWinerList[m]] += lMinScore / (iWinCount - j);
								}

								//�⳥��Ŀ
								lUserScore[wLoserChairID] -= lMinScore;
								lTotalScore[wLoserChairID] -= lMinScore;
							}
						}
					}
				}

				//����ѱ���
				LONG lWinChipBuffer[GAME_PLAYER];			//Ӯ�õĳ���

				//ͳ���û�����(�����)
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					GameEnd.lGameScore[i] = lUserScore[i];
					GameEnd.lWinChipScore[i] = lUserScore[i] + m_lTotalScore[i];
					GameEnd.lChipScore[i] = m_lUserMaxScore[i] - m_lTotalScore[i] + GameEnd.lWinChipScore[i];
					ASSERT(lUserScore[i] + m_lTotalScore[i] >= 0L);
				}

				BOOL m_cbCountStatus[GAME_PLAYER] = { 0 };
				memset(m_cbCountStatus, 0, sizeof(m_cbCountStatus));
				ZeroMemory(GameEnd.cbWinUserId, sizeof(GameEnd.cbWinUserId));

				// ���غ�ÿ���߳صľ������
				LONG lChipScores[GAME_PLAYER][GAME_PLAYER];
				ZeroMemory(lChipScores, sizeof(lChipScores));

				int iChipPoolCount = 0;		// ��������
				for (int ic = 0; ic < GAME_PLAYER; ic++)
				{
					if (m_aryChipPool[ic] > 0)
					{
						iChipPoolCount++;
					}
				}

				if (iChipPoolCount > 0)
				{
					//�߳������б�
					UserWinList BianChiList[GAME_PLAYER];
					ZeroMemory(BianChiList, sizeof(BianChiList));

					//ÿ���߳���Ӯ����
					for (int i = 0; i < iChipPoolCount; i++)
					{
						//��ʱ����
						BYTE bTempData[GAME_PLAYER][MAX_CENTERCOUNT] = { 0 };
						ZeroMemory(bTempData, sizeof(bTempData));

						//ÿ���߳�����������
						for (int j = 0; j < m_cbHandChairCount[i]; j++)
						{
							BYTE Bi = m_cbHandChair[i][j];
							if (m_cbPlayStatus[Bi] == TRUE)
							{
								CopyMemory(bTempData[Bi], cbEndCardData[Bi], sizeof(BYTE)*MAX_CENTERCOUNT);
							}
						}

						//����ÿ���߳�ʤ�����
						m_GameLogic.SelectMaxUser(bTempData, BianChiList[i], m_cbHandCardData, NULL);
					}

					// �����ص�ÿ���߳ؿ�ʼ����
					for (int a = 0; a < iChipPoolCount; a++)
					{
						// ����Ӯ�ң��п��ܴ��ڶ��Ӯ��
						if (BianChiList[a].bSameCount > 0)
						{
							for (int i = 0; i < BianChiList[a].bSameCount; i++)
							{
								if (m_cbPlayStatus[BianChiList[a].wWinerList[i]] == FALSE)continue;
								GameEnd.cbWinUserId[a][GameEnd.cbWinCount[a]++] = BianChiList[a].wWinerList[i];
							}

							// ���뽱�ز�������Ϸ����ʱ��û�������������
							BYTE cbPlayerCount = GameEnd.cbWinCount[a];
							ASSERT(cbPlayerCount > 0);

							// �ý��ػ�����ÿ�˻�ö���
							SCORE lWinChipScore = m_aryChipPool[a] / cbPlayerCount;

							// �������
							for (int iW = 0; iW < cbPlayerCount; iW++)
							{
								lChipScores[a][iW] = lWinChipScore;
							}
						}
						else
						{
							// �ý���û��Ӯ��ʱ��������������ƽ��
							for (int b = 0; b < m_cbHandChairCount[a]; b++)
							{
								if (m_cbPlayStatus[m_cbHandChair[a][b]] == TRUE)
								{
									// ������ҵ����Ӻ�
									GameEnd.cbWinUserId[a][GameEnd.cbWinCount[a]++] = m_cbHandChair[a][b];
								}
							}

							// ���뽱�ز�������Ϸ����ʱ��û�������������
							BYTE cbPlayerCount = GameEnd.cbWinCount[a];
							ASSERT(cbPlayerCount > 0);

							// �ý��ػ�����ÿ�˻�ö���
							SCORE lWinChipScore = m_aryChipPool[a] / cbPlayerCount;

							// �������
							for (int iW = 0; iW < cbPlayerCount; iW++)
							{
								lChipScores[a][iW] = lWinChipScore;
							}

							// ע�ͣ�ÿһ���������ʱ��GameEnd.cbWinUserId��lChipScoresֱ�ӣ��ǰ��������±�һһ��Ӧ��
						}

						//���û���ҵ��������
						if (GameEnd.cbWinCount[a] <= 0)
						{
							for (int b = 0; b < GAME_PLAYER; b++)
							{
								if (m_cbPlayStatus[b] == TRUE)
								{
									GameEnd.cbWinUserId[a][GameEnd.cbWinCount[a]++] = b;
								}
							}

							// ���뽱�ز�������Ϸ����ʱ��û�������������
							BYTE cbPlayerCount = GameEnd.cbWinCount[a];
							ASSERT(cbPlayerCount > 0);

							// �ý��ػ�����ÿ�˻�ö���
							SCORE lWinChipScore = m_aryChipPool[a] / cbPlayerCount;

							// �������
							for (int iW = 0; iW < cbPlayerCount; iW++)
							{
								lChipScores[a][iW] = lWinChipScore;
							}
						}
					}
				}
				
				CopyMemory(lWinChipBuffer, GameEnd.lWinChipScore, sizeof(GameEnd.lWinChipScore));
				m_lAllScore = 0;

				for (int i = 0; i < m_wPlayerCount; i++)
				{
					m_lAllScore += m_lTotalScore[i];
					if (GameEnd.lGameScore[i] > 0)
					{
						if (m_pGameServiceOption->wRevenueRatio > 0)//�����
						{
							//��������
							GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
							GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
						}
					}
				}

				GameEnd.lAllScore = m_lAllScore;
				GameEnd.cbChipPoolCount = iChipPoolCount;

				//ͳ���û�����		
				CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
				CopyMemory(GameEnd.lChipPool, m_aryChipPool, sizeof(m_aryChipPool));
				CopyMemory(GameEnd.cbPlayStatus, m_cbPlayStatus, sizeof(GameEnd.cbPlayStatus));
				CopyMemory(GameEnd.cbPlayGiveUp, m_cbGiveUpStatus, sizeof(GameEnd.cbPlayGiveUp));
				CopyMemory(GameEnd.cbCenterCardData, m_cbCenterCardData, sizeof(GameEnd.cbCenterCardData));
				CopyMemory(GameEnd.lChipPoolScore, lChipScores, sizeof(GameEnd.lChipPoolScore));

				// ͳ�Ƹ�������Һͻ����˵���Ӯ
				WriteAllScore(GameEnd.lGameScore, GameEnd.lGameTax);

				//�޸��û�����
				tagScoreInfo ScoreInfo;// [m_wPlayerCount];


				//���ֱ���
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);

					if (m_bFristPlayStatus[i] && pUserItem != NULL)
					{
						BYTE nScoreKind;
						if (GameEnd.lGameScore[i] == 0L)
						{
							nScoreKind = SCORE_TYPE_DRAW;
						}
						else
						{
							nScoreKind = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
						}

						//д�����
						m_aryEndScore[i] = GameEnd.lGameScore[i];

						memset(&ScoreInfo, 0, sizeof(ScoreInfo));
						ScoreInfo.lScore = GameEnd.lGameScore[i];// -m_nDaShangCount[i] * DASHANG_COST;
						//m_nDaShangCount[i] = 0;

						ScoreInfo.cbType = nScoreKind;
						ScoreInfo.lRevenue = GameEnd.lGameTax[i];
						m_pITableFrame->WriteUserScore(i, ScoreInfo);

						//��Ӯ�Ľ�� + �Լ���ע�Ľ�� ����������ӵ�еĽ�Ҵ�����
						m_lUserTakeGold[i] = m_lUserTakeGold[i] + m_lTotalScore[i] + ScoreInfo.lScore;
						m_lUserMaxScore[i] = 0;

						SCORE lScore = pUserItem->GetUserScore();

						// ���ּ����쳣
						if (m_lUserTakeGold[i] > lScore)
						{
							ASSERT(0);
							CString str;
							str.Format(TEXT("OnEventGameEnd : GER_NORMAL m_lUserTakeGold[i] %d lScore %d"), m_lUserTakeGold[i], lScore);
							m_pITableFrame->ExportInformation(str, true);
						}

						//��ȡ��ʷ�������
						BYTE aryCardData[MAX_CENTERCOUNT] = {0};
						BYTE cbCardType = pUserItem->GetMaxCardData(aryCardData, MAX_CENTERCOUNT);
						//��ǰ���ʹ�����ʷ��������򱣴�
						bool bSave = false;
						if (GameEnd.cbCardType[i]>cbCardType)
						{
							bSave = true;
						}
						//���������ͬ��Ƚ���ֵ
						else if ((GameEnd.cbCardType[i] == cbCardType) && (2==m_GameLogic.CompareCard(GameEnd.cbLastCenterCardData[i], aryCardData, MAX_CENTERCOUNT)))
						{
							bSave = true;
						}

						if (bSave)
						{
							pUserItem->ModifyMaxCardType(GameEnd.cbCardType[i], GameEnd.cbLastCenterCardData[i], MAX_CENTERCOUNT);
						}
					}
				}

				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

				m_GameOver = false;
				//������Ϸ	
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			catch (...)
			{
				ASSERT(false);
			}

			return true;
		}
		case GER_NO_PLAYER:		//û�����
		{
			m_bGameStart = false;
			try
			{
				m_GameOver = true;
				KillTimerAll();

				//�������
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd, sizeof(GameEnd));
				GameEnd.cbTotalEnd = 0;

				//Ч����
				BYTE cbUserCount = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i] != FALSE) cbUserCount++;
				}
				if (cbUserCount != 1)
				{
					ASSERT(FALSE);
					m_pITableFrame->ExportInformation(TEXT("û�����//Ч��������"), true);
				}

				//ͳ�Ʒ���
				LONG lScore = 0, lRevenue = 0;
				BYTE nScoreKind = 0;
				WORD wWinner = INVALID_CHAIR;

				//���������
				CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
				CopyMemory(GameEnd.lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));

				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					//�����˿�			
					if (m_cbPlayStatus[i] == FALSE)
					{
						// ����ע������Ϸ״̬ΪFALSE������Ƿ������
						if (m_lTotalScore[i] > 0L)
						{
							GameEnd.lGameScore[i] -= m_lTotalScore[i];
						}

						// ��Ϸ���������ʣ�����
						GameEnd.lChipScore[i] = m_lUserMaxScore[i] + GameEnd.lGameScore[i];

						continue;
					}

					wWinner = i;	// ���һ��û�����������Ӯ��

					//����ע��Ŀ
					LONG lAllScore = 0L;
					for (WORD j = 0; j < m_wPlayerCount; j++)
					{
						if (wWinner == j)
						{
							continue;
						}
						lAllScore += m_lTotalScore[j];
					}

					ASSERT(lAllScore >= 0);
					GameEnd.lGameScore[i] = lAllScore;
					lScore = GameEnd.lGameScore[i];
					//lRevenue= GameEnd.lGameTax[i];
					GameEnd.lWinChipScore[i] = lScore + m_lTotalScore[i];
					GameEnd.lChipScore[i] = m_lUserMaxScore[i] + GameEnd.lGameScore[i];
					nScoreKind = (GameEnd.lGameScore[i]>0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
				}

				m_lAllScore = 0;
				for (int i = 0; i < m_wPlayerCount; i++)
				{
					m_lAllScore += m_lTotalScore[i];
					if (GameEnd.lGameScore[i] > 0)
					{
						GameEnd.cbWinUserId[0][GameEnd.cbWinCount[0]++] = i;
						GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
						GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
					}
				}
				GameEnd.lAllScore = m_lAllScore;

				// ֻ��һ��Ӯ�ң������н��ȫ��ʤ�����
				GameEnd.lChipPoolScore[0][0] = m_lAllScore;

				CopyMemory(GameEnd.cbPlayStatus, m_cbPlayStatus, sizeof(GameEnd.cbPlayStatus));
				CopyMemory(GameEnd.cbPlayGiveUp, m_cbGiveUpStatus, sizeof(GameEnd.cbPlayGiveUp));

				//д�����
				if (wWinner < GAME_PLAYER)
				{
					WriteAllScore(GameEnd.lGameScore, GameEnd.lGameTax);
					tagScoreInfo ScoreInfo[m_wPlayerCount];
					memset(&ScoreInfo, 0, sizeof(ScoreInfo));
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);
						//if(m_cbGiveUpStatus[i]==2) 
						//{
						//	GameEnd.lGameScore[i]-=20*m_lCellScore;
						//	continue;
						//}

						if (m_bFristPlayStatus[i])
						{
							BYTE nScoreKind = 0;
							if (GameEnd.lGameScore[i] == 0L)
							{
								nScoreKind = SCORE_TYPE_DRAW;
							}
							else
							{
								nScoreKind = (GameEnd.lGameScore[i] > 0L) ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE;
							}

							//д�����
							ScoreInfo[i].lScore = GameEnd.lGameScore[i];// -m_nDaShangCount[i] * DASHANG_COST;
							//m_nDaShangCount[i] = 0;
							ScoreInfo[i].cbType = nScoreKind;
							ScoreInfo[i].lRevenue = GameEnd.lGameTax[i];
							m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);

							//��Ӯ�Ľ�� + �Լ���ע�Ľ�� ����������ӵ�еĽ�Ҵ�����
							m_lUserTakeGold[i] = m_lUserTakeGold[i] + m_lTotalScore[i] + ScoreInfo[i].lScore;
							m_lUserMaxScore[i] = 0;
							if (pUserItem)
							{
								SCORE lScore = pUserItem->GetUserScore();
								if (m_lUserTakeGold[i] > lScore)
								{
									ASSERT(0);
									CString str;
									str.Format(TEXT("OnEventGameEnd : GER_NO_PLAYER m_lUserTakeGold[i] %d lScore %d"), m_lUserTakeGold[i], lScore);
									m_pITableFrame->ExportInformation(str, true);
								}
							}
						}
					}
				}
				else
				{
					m_pITableFrame->ExportInformation(TEXT("Ӯ����λ�Ų�����Чֵ"), true);
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

				m_GameOver = false;
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			catch (...)
			{
				ASSERT(FALSE);
			}

			return true;
		}
		case GER_USER_LEAVE:
		case GER_NETWORK_ERROR:
		{
			//Ч�����
			ASSERT(pIServerUserItem != NULL);
			ASSERT(wChairID < m_wPlayerCount);

			//ǿ�˴���
			OnUserGiveUp(wChairID);
			return true;
		}
	}
	return false;
}

//�����ô����ҷ�Χ
void CTableFrameSink::CalRangeTakeGold(WORD wChairID, LONG & lMin, LONG & lMax)
{
	//��ʼ��
	//lMin = m_lAddTakeMinTimes;
	//lMax = m_lAddTakeMaxTimes;

	//IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	//if (pIServerUserItem == NULL)
	//{
	//	ASSERT(0);
	//	return;
	//}

	//ר������
	//lMin = m_lAddTakeMinTimes*m_lCellScore;
	//lMax = m_lAddTakeMaxTimes*m_lCellScore;

	lMin = m_pGameServiceOption->lLowPutScore;
	//lMax = m_pGameServiceOption->lDefPutScore * m_pGameServiceOption->iExchangeRatio * 2;
	lMax = m_pGameServiceOption->lDefPutScore * 2;

}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
	{
		//��������
		CMD_S_StatusFree StatusFree;
		memset(&StatusFree, 0, sizeof(StatusFree));

		//���ñ���
		m_lCellScore = m_pITableFrame->GetCellScore();
		StatusFree.lBaseScore = m_lCellScore;

		LONG lMinTakeGold = 0;
		LONG lMaxTakeGold = 0;
		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			CalRangeTakeGold(wChiarID, lMinTakeGold, lMaxTakeGold);
		}
		StatusFree.lAddTakeGoldMin = lMinTakeGold;
		StatusFree.lAddTakeGoldMax = lMaxTakeGold;

		CopyMemory(StatusFree.lUserTakeGold, m_lUserTakeGold, sizeof(StatusFree.lUserTakeGold));
		//���ͳ���
		return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
	}
	case GAME_STATUS_PLAY:	//��Ϸ״̬
	{
		//��������
		CMD_S_StatusPlay StatusPlay;
		ZeroMemory(&StatusPlay, sizeof(StatusPlay));

		//��־����			
		if (m_cbBalanceCount > 0)
		{
			StatusPlay.bGameId = 1;
		}
		//����ע��Ŀ
		LONG lAllScore = 0L;
		for (WORD j = 0; j < m_wPlayerCount; j++)
		{
			lAllScore += m_lTotalScore[j];
		}
		ASSERT(lAllScore >= 0);
		StatusPlay.lAddLessScore = m_lAddLessScore;
		StatusPlay.lTurnLessScore = m_lTurnLessScore;
		StatusPlay.lAllChipScore = m_lAllScore;
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (!m_bFristPlayStatus[i])
				continue;
			StatusPlay.lTurnMaxScore[i] = m_lUserMaxScore[i] - m_lTotalScore[i];
		}

		//��ע��Ϣ
		StatusPlay.lCellScore = m_lCellScore;
		if (m_GameOver)
		{
			StatusPlay.bGameId = 2;
		}
		StatusPlay.wDMUser[0] = m_wMUser[0];
		StatusPlay.wDMUser[1] = m_wMUser[1];

		//���ñ���
		StatusPlay.wDUser = m_wDUser;
		StatusPlay.wCurrentUser = m_wCurrentUser;//��ǰ���
		StatusPlay.cbChipPoolCount = m_ChipPoolCount;
		StatusPlay.cbBalanceCount = m_cbBalanceCount;
		CopyMemory(StatusPlay.aryChipPool, m_aryChipPool, sizeof(m_aryChipPool));
		CopyMemory(StatusPlay.lUserMaxScore, m_lUserMaxScore, sizeof(m_lUserMaxScore));//�����ע
		CopyMemory(StatusPlay.lTableScore, m_lTableScore, sizeof(m_lTableScore));//��ע��Ŀ
		CopyMemory(StatusPlay.lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));//��ע��Ŀ
		CopyMemory(StatusPlay.cbGiveUpStatus, m_cbGiveUpStatus, sizeof(m_cbGiveUpStatus));//��ע��Ŀ
		CopyMemory(StatusPlay.lTableBuffer, m_TabelScoreBuffer, sizeof(m_TabelScoreBuffer));//�û���Ϸ״̬
		CopyMemory(StatusPlay.lUserTakeGold, m_lUserTakeGold, sizeof(StatusPlay.lUserTakeGold));

		if (wChiarID < GAME_PLAYER)
		{
			StatusPlay.cbHandCardData[0] = m_cbHandCardData[wChiarID][0];
			StatusPlay.cbHandCardData[1] = m_cbHandCardData[wChiarID][1];
		}

		if (m_cbBalanceCount > 0)
		{
			StatusPlay.cbCenterCount = 3 + (m_cbBalanceCount - 1);
			CopyMemory(StatusPlay.cbCenterCardData, m_cbCenterCardData, StatusPlay.cbCenterCount);
		}
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			//StatusPlay.cbFristPlayStatus[i] = m_bFristPlayStatus[i] ? TRUE : FALSE;	
			StatusPlay.cbPlayStatus[i] = m_cbPlayStatus[i];
			if (m_GameOver)
			{
				StatusPlay.aryEndScore[i] = m_aryEndScore[i];
			}
		}

		LONG lMinTakeGold = 0;
		LONG lMaxTakeGold = 0;
		//CalRangeTakeGold(wChiarID,lMinTakeGold,lMaxTakeGold);
		if (pIServerUserItem->GetUserStatus() != US_LOOKON)
		{
			CalRangeTakeGold(wChiarID, lMinTakeGold, lMaxTakeGold);
		}
		StatusPlay.lAddTakeGoldMin = lMinTakeGold;
		StatusPlay.lAddTakeGoldMax = lMaxTakeGold;

		//���ͳ���
		m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		if (pIServerUserItem->IsAndroidUser() && pIServerUserItem->GetUserStatus() == US_PLAYING)
		{
			m_pITableFrame->SendTableData(wChiarID, SUB_S_USER_ROBOT, &m_GameRobot, sizeof(m_GameRobot));
		}
		return true;
	}
	}
	//Ч����
	ASSERT(FALSE);
	return false;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_TIME_GAME:
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_GAME);
		if (m_wCurrentUser == INVALID_CHAIR)
		{
			ASSERT(false);
			return true;
		}

		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentUser);
		ASSERT(pServerUserItem != NULL);
		if (pServerUserItem == NULL) return true;
		if (!m_GameOver)
		{
			if (0 == m_lTurnLessScore)  //����
			{
				m_cbChipTypeID = CHIP_TYPE_PASS;
				OnUserAddScore(m_wCurrentUser, 0, false);
			}
			else
			{
				OnUserGiveUp(m_wCurrentUser);
			}
		}

		return true;
	}
	case IDI_TIME_SENDCARD://���ƽ���
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_SENDCARD);
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			m_SendCardEnd[i] = true;
		}
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER, 1, 0);
		return true;
	}
	case IDI_TIME_ADDBUFFER:
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_ADDBUFFER);
		OnChipRendEnd();
		return true;
	}
	case IDI_TIME_GameCard:
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_GameCard);

		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NO_PLAYER);
		return true;
	}
	case IDI_GAME_NORMALEND://��������
	{
		m_pITableFrame->KillGameTimer(IDI_GAME_NORMALEND);
		OutputDebugString(TEXT("IDI_GAME_NORMALEND\n"));
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
		return true;
	}
	case IDI_GAME_NO_PLAYER://��������
	{
		//if(m_GameOver)return true;

		m_pITableFrame->KillGameTimer(IDI_GAME_NO_PLAYER);
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NO_PLAYER);
		return true;
	}
	case IDI_GAME_START:
	{
		m_pITableFrame->KillGameTimer(IDI_GAME_START);
		OnUserAddScore(m_wDUser, 0, false);
		return true;
	}
	}
	return false;
}

void CTableFrameSink::KillTimerAll()
{
	m_pITableFrame->KillGameTimer(IDI_TIME_GAME);
	m_pITableFrame->KillGameTimer(IDI_TIME_GameCard);
	m_pITableFrame->KillGameTimer(IDI_TIME_SENDCARD);
	m_pITableFrame->KillGameTimer(IDI_GAME_START);
	m_pITableFrame->KillGameTimer(IDI_TIME_ADDBUFFER);
}

bool CTableFrameSink::OnChipRendEnd()
{
	//�����ж�
	if (m_cbBalanceCount == 4)
	{
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
		return true;
	}

	//����û�ͳ��
	WORD wShowHandCount = 0, wPlayerCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbShowHand[i] == TRUE)
		{
			wShowHandCount++;
		}

		if (m_cbPlayStatus[i] == TRUE)
		{
			wPlayerCount++;
		}
	}

	//ֻʣһ���û�����ȫ��
	if ((wShowHandCount >= wPlayerCount - 1) && m_cbBalanceCount < 4)
	{
		//��������
		CMD_S_SendCard SendCard;
		ZeroMemory(&SendCard, sizeof(SendCard));
		SendCard.cbPublic = m_cbBalanceCount;
		SendCard.wCurrentUser = INVALID_CHAIR;
		SendCard.cbSendCardCount = MAX_CENTERCOUNT;
		CopyMemory(SendCard.cbCenterCardData, m_cbCenterCardData, sizeof(BYTE)*MAX_CENTERCOUNT);
		SendCard.LAllScore = m_lAllScore;
		CopyMemory(SendCard.lTableScore, m_lTableScore, sizeof(SendCard.lTableScore));

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

		//������Ϸ
		DWORD dwTimer = 2000;
		if (0 == m_cbBalanceCount)
		{
			dwTimer = 5000;
		}
		else if (m_cbBalanceCount <= 3)
		{
			dwTimer = 4000;
		}
		else if (m_cbBalanceCount <= 4)
		{
			dwTimer = 3000;
		}
		m_pITableFrame->SetGameTimer(IDI_GAME_NORMALEND, dwTimer, 1, 0);
		ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
		return true;
	}

	//äע���
	for (WORD i = 1; i < m_wPlayerCount; i++)
	{
		//��ʱ����
		BYTE cbNextUser = (m_wDUser + i) % GAME_PLAYER;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(cbNextUser);

		//��Ч�û� ����û�����
		if (pIServerUserItem == NULL || m_cbPlayStatus[cbNextUser] == FALSE || m_cbShowHand[cbNextUser] == TRUE)
			continue;
		m_wCurrentUser = cbNextUser;
		break;
	}

	//��������
	ASSERT(m_wCurrentUser != INVALID_CHAIR);
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard, sizeof(SendCard));
	SendCard.cbPublic = 0;
	SendCard.wCurrentUser = m_wCurrentUser;
	SendCard.cbSendCardCount = 3 + (m_cbBalanceCount - 1);
	CopyMemory(SendCard.cbCenterCardData, m_cbCenterCardData, sizeof(BYTE)*(SendCard.cbSendCardCount));
	SendCard.LAllScore = m_lAllScore;
	CopyMemory(SendCard.lTableScore, m_lTableScore, sizeof(SendCard.lTableScore));
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));

	if (VALIDE_CHAIRID(m_wCurrentUser))
	{
		SendCard.lTurnMaxScore = m_lTurnMaxScore[m_wCurrentUser];
	}

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

	if (3 == SendCard.cbSendCardCount)//�����Ʒ����ӳ�
	{
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER + 3000, 1, 0);
	}
	else
	{
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER + 2000, 1, 0);
	}

	return true;

}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_GF_TAKE_GOLD:
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_Add_TakeGold));
		if (wDataSize != sizeof(CMD_C_Add_TakeGold)) return false;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus == US_PLAYING) return true;

		//��������
		CMD_C_Add_TakeGold * pAddTakeGold = (CMD_C_Add_TakeGold *)pDataBuffer;
		return OnUserAddTakeGold(pIServerUserItem->GetChairID(), pAddTakeGold->lAddTakeGold);
	}
	case SUB_C_GIVE_UP:		//�û�����
	{
		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

		if (pUserData->cbUserStatus != US_PLAYING) return true;
		//״̬�ж�
		ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
		if (m_cbPlayStatus[pUserData->wChairID] == FALSE) return false;
		//��Ϣ����
		return OnUserGiveUp(pUserData->wChairID);
	}
	case SUB_C_LOOK_CARD:		//�û�����
	{
		m_pITableFrame->KillGameTimer(IDI_GAME_NO_PLAYER);
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		CMD_S_LiangCard GiveUp;
		GiveUp.wLiangUser = pUserData->wChairID;
		GiveUp.BLiangId = 1;
		for (int i = 0; i < MAX_COUNT; ++i)
		{
			GiveUp.cbCardData[i] = m_cbHandCardData[pUserData->wChairID][i];
		}

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_LOOK_CARD, &GiveUp, sizeof(GiveUp));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_LOOK_CARD, &GiveUp, sizeof(GiveUp));
		//OnEventGameEnd(INVALID_CHAIR,NULL,GER_NO_PLAYER);
		return true;
	}
	case SUB_C_LOOKNO_CARD:		//�û�������
	{
		m_pITableFrame->KillGameTimer(IDI_GAME_NO_PLAYER);
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NO_PLAYER);
		return true;
	}
	case SUB_C_ADD_SCORE:	//�û���ע	
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_AddScore));
		if (wDataSize != sizeof(CMD_C_AddScore)) return false;

		//��������
		CMD_C_AddScore * pAddScore = (CMD_C_AddScore *)pDataBuffer;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//״̬�ж�
		ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
		if (m_cbPlayStatus[pUserData->wChairID] == FALSE) return false;
		m_cbChipTypeID = pAddScore->ID;
		//��Ϣ����
		return OnUserAddScore(pUserData->wChairID, pAddScore->lScore, false);
	}
	case SUB_C_PLAY_END://��������
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_PlatEnd));
		if (wDataSize != sizeof(CMD_C_PlatEnd)) return false;

		//��������
		CMD_C_PlatEnd * PlatEnd = (CMD_C_PlatEnd *)pDataBuffer;

		//�û�Ч��
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;
		//״̬�ж�
		ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
		if (m_cbPlayStatus[pUserData->wChairID] == FALSE) return false;

		//��Ϣ����
		return OnPlayEnd(pUserData->wChairID, PlatEnd->ID);

	}
	case SUB_C_GAMEOVER:	//��Ϸ����
	{
		if (!m_GameOver)
			return true;

		//�û�Ч��
		m_GameOver = false;
		//������Ϸ
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		return true;
	}
	case SUB_C_SET_BASESCORE:  //������õ�ע
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_SetBaseScore));
		if (wDataSize != sizeof(CMD_C_SetBaseScore)) return false;

		CMD_C_SetBaseScore * pBaseScore = (CMD_C_SetBaseScore *)pDataBuffer;

		return OnUserSetBase(pBaseScore, pIServerUserItem);
	}
	case SUB_C_DASHANG:
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_C_DaShang));
		if (wDataSize != sizeof(CMD_C_DaShang)) return false;
		CMD_C_DaShang * pDaShang = (CMD_C_DaShang *)pDataBuffer;
		return  OnUserDaShang(pDaShang->wChairID);
	}
	}
	return false;
}

bool CTableFrameSink::OnUserAddTakeGold(WORD wChairID, LONG lAddTake)
{
	ASSERT(VALIDE_CHAIRID(wChairID) && lAddTake > 0);
	if (!VALIDE_CHAIRID(wChairID) || lAddTake <= 0)
	{
		CString str;
		str.Format(TEXT(" OnUserAddTakeGold : wChairID %d lAddTake %d"), wChairID, lAddTake);
		m_pITableFrame->ExportInformation(str, true);
		ASSERT(0);
		return false;
	}

	//��ע���ܴ�������ӵ�еĽ����
	LONG lScore = m_pITableFrame->GetTableUserItem(wChairID)->GetTrusteeScore();

	LONG lLeftGold = lScore - m_lUserMaxScore[wChairID];
	ASSERT(lLeftGold >= lAddTake);
	if (lLeftGold < lAddTake)
	{
		CString str;
		str.Format(TEXT("OnUserAddTakeGold : lScore %d m_lUserMaxScore[wChairID] %d lAddTake %d"), lScore, m_lUserMaxScore[wChairID], lAddTake);
		m_pITableFrame->ExportInformation(str, true);
		ASSERT(0);
		return false;
	}
	//���������С����
	LONG lMinLimit = 0;
	LONG lMaxLimit = 0;
	CalRangeTakeGold(wChairID, lMinLimit, lMaxLimit);
	LONG lLimitTake = m_lUserTakeGold[wChairID] + lAddTake;
	ASSERT(lLimitTake <= lScore && lLimitTake >= lMinLimit);

	if (lLimitTake > lScore ||
		lLimitTake < lMinLimit || lLimitTake > lMaxLimit)
	{
		CString str;
		str.Format(TEXT("OnUserAddTakeGold : m_lUserTakeGold[wChairID] %d lAddTake %d lMinLimit %d nMaxTake %d lScore %d"), m_lUserTakeGold[wChairID], lAddTake, lMinLimit, lMaxLimit, lScore);
		m_pITableFrame->ExportInformation(str, true);
		ASSERT(0);
		return false;
	}
	m_lUserTakeGold[wChairID] += lAddTake;
	m_lUserMaxScore[wChairID] += lAddTake;

	CMD_S_Add_TakeGold AddTakeGold;
	AddTakeGold.wChairID = wChairID;
	AddTakeGold.lAddTakeGold = lAddTake;
	AddTakeGold.lAllTakeGold = m_lUserTakeGold[wChairID];
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ADD_TAKEGOLD, &AddTakeGold, sizeof(AddTakeGold));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_TAKEGOLD, &AddTakeGold, sizeof(AddTakeGold));
	return true;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�����¼�
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	ASSERT(VALIDE_CHAIRID(wChairID));
	if (m_wCurrentUser >= GAME_PLAYER)  return true;

	if (m_GameOver) return true;

	if (wChairID != m_wCurrentUser)  return true;

	if (wChairID == m_wCurrentUser)
	{
		m_pITableFrame->KillGameTimer(IDI_TIME_GAME);
	}

	//����״̬
	m_cbPlayStatus[wChairID] = FALSE;
	m_cbShowHand[wChairID] = FALSE;
	m_cbGiveUpStatus[wChairID] = 1;

	//����ͳ��
	WORD wPlayerCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			wPlayerCount++;
		}
	}

	//������Ϣ
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser = wChairID;
	GiveUp.lLost = -m_lTotalScore[wChairID];
	GiveUp.wID = m_cbGiveUpStatus[wChairID];
	GiveUp.wLookUser = INVALID_CHAIR;
	GiveUp.LAllScore = m_lAllScore;

	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));

	//�����ע
	//m_aryChipPool[m_ChipPoolCount] += m_lTableScore[wChairID];

	//�жϽ���
	if (wPlayerCount >= 2)
	{
		m_cbChipTypeID = CHIP_TYPE_GIVEUP;
		if (wChairID == m_wCurrentUser)
		{
			OnUserAddScore(wChairID, 0L, true);
		}
	}
	else
	{
		m_cbChipTypeID = CHIP_TYPE_OVER;//������������
		if (wChairID == m_wCurrentUser)
		{
			OnUserAddScore(wChairID, 0L, true);
		}
		m_GameOver = true;
		m_pITableFrame->SetGameTimer(IDI_GAME_NO_PLAYER, 2000, 1, 0);
	}

	return true;
}

//��������
bool CTableFrameSink::OnPlayEnd(WORD wChairID, BYTE PlayId)
{
	//У���û�
	ASSERT(!m_GameOver);
	if (m_GameOver) return true;
	if (m_SendCardEnd[wChairID]) return true;
	BOOL bKill = true;
	if (0 == PlayId)//�������
	{
		m_SendCardEnd[wChairID] = true;
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem != NULL && !pServerUserItem->IsAndroidUser())
			{
				if (!m_SendCardEnd[i])
				{
					bKill = false;
					break;
				}
			}
		}

		if (bKill)//������Ҷ������
		{
			m_pITableFrame->KillGameTimer(IDI_TIME_SENDCARD);
			m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER, 1, 0);
		}

	}
	return true;
}

//��ע�¼� 
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp)
{
	//У���û�
	ASSERT(m_wCurrentUser == wChairID);
	if (m_wCurrentUser != wChairID || m_GameOver)
	{
		CString str;
		str.Format(TEXT(" OnUserAddScore : wChairID %d m_wCurrentUser %d lAddScore %d"), wChairID, m_wCurrentUser, lScore);
		m_pITableFrame->ExportInformation(str, true);
		ASSERT(0);
		return true;
	}

	// ��ע��ȫ���ݴ���
	if (m_cbChipTypeID == CHIP_TYPE_ALLIN)
	{
		lScore = m_lTurnMaxScore[wChairID];
	}

	if (m_cbChipTypeID == CHIP_TYPE_GEN)
	{
		lScore = m_lTurnLessScore;
	}

	//У����	
	if ((lScore + m_lTotalScore[wChairID]) > m_lUserMaxScore[wChairID])
	{
		CString str;
		str.Format(TEXT(" OnUserAddScore : lScore %d m_lTotalScore[wChairID] %d m_lUserMaxScore[wChairID] %d"), lScore, m_lTotalScore[wChairID], m_lUserMaxScore[wChairID]);
		m_pITableFrame->ExportInformation(str, false);
		lScore = m_lUserMaxScore[wChairID] - m_lTotalScore[wChairID];
	}

	ASSERT(lScore >= 0L);
	if (lScore < 0)
	{
		m_pITableFrame->ExportInformation(TEXT("OnUserAddScore : lScore < 0"), true);
		ASSERT(0);
		return true;
	}

	ASSERT(lScore <= m_lUserTakeGold[wChairID]);
	if (lScore > m_lUserTakeGold[wChairID])
	{
		CString str;
		str.Format(TEXT(" OnUserAddScore : m_lTotalScore[wChairID] %d lAddScore %d m_lUserTakeGold[wChairID] %d"), m_lTotalScore[wChairID], lScore, m_lUserTakeGold[wChairID]);
		m_pITableFrame->ExportInformation(str, true);
		ASSERT(0);
		return true;
	}

	m_pITableFrame->KillGameTimer(IDI_TIME_GAME);

	//�ۼƽ��
	m_lTableScore[wChairID] += lScore;
	m_lTotalScore[wChairID] += lScore;
	m_TabelScoreBuffer[wChairID] = lScore;
	m_lTurnMaxScore[wChairID] -= lScore;
	m_lUserTakeGold[wChairID] -= lScore;

	//ƽ����ע
	if (m_lTableScore[wChairID] > m_lBalanceScore)
	{
		m_lBalanceScore = m_lTableScore[wChairID];
	}

	//����ж�
	if (m_lTotalScore[wChairID] >= m_lUserMaxScore[wChairID] || m_cbChipTypeID == CHIP_TYPE_ALLIN)
	{
		m_cbShowHand[wChairID] = TRUE;
		m_cbShowHandBuffer[wChairID] = TRUE;
		m_lTotalScore[wChairID] = m_lUserMaxScore[wChairID];
		m_lUserTakeGold[wChairID] = 0;
		m_cbChipTypeID = CHIP_TYPE_ALLIN;
		m_SuoHaCountBuffer[m_cbBalanceCount]++;
	}

	m_lAllScore = 0;
	for (int i = 0; i < m_wPlayerCount; i++)
	{
		m_lAllScore += m_lTotalScore[i];
	}

	//�û��л�
	WORD wNextPlayer = INVALID_CHAIR;

	for (WORD i = 1; i < m_wPlayerCount; i++)
	{
		//���ñ���
		m_wOperaCount++;
		wNextPlayer = (m_wCurrentUser + i) % m_wPlayerCount;

		//�����ж�
		if ((m_cbPlayStatus[wNextPlayer] == TRUE) && (m_cbShowHand[wNextPlayer] == FALSE))
		{
			break;
		}
	}
	ASSERT(wNextPlayer < m_wPlayerCount);

	//����ж�
	bool bFinishTurn = false;

	//�жϷ���������µ�һ����ע���
	if (m_wOperaCount >= m_wPlayerCount && m_lBalanceScore > 0)
	{
		WORD i = 0;
		//CString str = TEXT("���");
		for (; i < m_wPlayerCount; i++)
		{
			//str.AppendFormat(TEXT("i %d m_cbPlayStatus %d m_lTableScore %d m_cbShowHand %d m_lBalanceScore %d \r\n"), i, m_cbPlayStatus[i], m_lTableScore[i], m_cbShowHand[i], m_lBalanceScore);
			//����δƽ�� ��δ����û�
			if ((m_cbPlayStatus[i] == TRUE) && (m_lTableScore[i] < m_lBalanceScore) && (m_cbShowHand[i] == FALSE))
				break;
		}
		if (i >= m_wPlayerCount)
		{
			bFinishTurn = true;
			//::OutputDebugString(str);
			//::OutputDebugString(TEXT("����������� һ�����\n"));  //��BUG
		}
	}

	WORD wPlayCount = 0, wShowCount = 0;
	for (BYTE i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			if (m_cbShowHand[i] == TRUE)
			{
				wShowCount++;
			}
			wPlayCount++;
		}
	}

	// �ݴ���������ע���Ϊ0�ĸ�ע���ע��Ϣʱ������PASS����
	if ((m_cbChipTypeID == CHIP_TYPE_ADD || m_cbChipTypeID == CHIP_TYPE_GEN) && lScore == 0)
	{
		m_cbChipTypeID = CHIP_TYPE_PASS;
	}

	//��ǰ����������
	if (m_cbChipTypeID == CHIP_TYPE_PASS || m_cbChipTypeID == CHIP_TYPE_GIVEUP)
	{
		if (m_cbChipTypeID == CHIP_TYPE_PASS)
		{
			m_nPassCardCount++;
		}

		CString str;
		str.Format(TEXT("CHIP_TYPE_PASS wPlayCount %d wShowCount %d m_nPassCardCount %d \n"), wPlayCount, wShowCount, m_nPassCardCount);
		::OutputDebugString(str);

		//�ж����һȦ��ע �������
		if (m_nPassCardCount >= (wPlayCount - wShowCount))
		{
			bFinishTurn = true;
			::OutputDebugString(TEXT("���� һ�����\n"));
		}
	}
	else
	{
		m_nPassCardCount = 0;
	}

	//�¼�û����� ������ע�Ѿ��ﵽƽ��
	//if ((wPlayCount - 1 == wShowCount) && m_lTableScore[wNextPlayer] >= m_lBalanceScore && m_lBalanceScore > 0)
	if ((wPlayCount - 1 == wShowCount) && m_lTableScore[wNextPlayer] >= m_lBalanceScore)
	{
		bFinishTurn = true;
		::OutputDebugString(TEXT("�¼�û����� ������ע�Ѿ��ﵽƽ�� һ�����\n"));
	}

	if (m_cbChipTypeID == CHIP_TYPE_OVER)
	{
		bFinishTurn = true;
		::OutputDebugString(TEXT("ֻʣһ���� һ�����\n"));
	}

	//������ע
	if (!bFinishTurn)
	{
		//��ǰ�û�
		m_wCurrentUser = wNextPlayer;
		ASSERT(VALIDE_CHAIRID(m_wCurrentUser));
		ASSERT(m_cbPlayStatus[m_wCurrentUser] == TRUE);
		//��СֵΪƽ����ע -������ע  �� ʣ������ȡС �������
		m_lTurnLessScore = min(m_lBalanceScore - m_lTableScore[m_wCurrentUser], m_lUserMaxScore[m_wCurrentUser] - m_lTotalScore[m_wCurrentUser]);

		m_lTurnMaxScore[m_wCurrentUser] = m_lUserMaxScore[m_wCurrentUser] - m_lTotalScore[m_wCurrentUser];
		ASSERT(m_lTurnMaxScore[m_wCurrentUser] == m_lUserTakeGold[m_wCurrentUser]);
		m_lAddLessScore = (0 == m_lBalanceScore) ? (m_lCellScore*m_ISetDJPK[2] / 10) : m_lTurnLessScore + m_lCellScore;

		//��������
		CMD_S_AddScore AddScore;
		ZeroMemory(&AddScore, sizeof(AddScore));
		AddScore.lAddScoreCount = lScore;
		AddScore.wAddScoreUser = wChairID;
		AddScore.wCurrentUser = m_wCurrentUser;
		AddScore.lTurnLessScore = m_lTurnLessScore;
		AddScore.lAllScore = m_lAllScore;
		AddScore.cbBalanceCount = m_cbBalanceCount;
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_cbPlayStatus[i] == FALSE) continue;
			AddScore.lTurnMaxScore[i] = m_lTurnMaxScore[i];
		}

		AddScore.cbChipTypeID = m_cbChipTypeID;
		if (m_lAddLessScore < 0)
		{
			m_lAddLessScore *= -1;
		}

		AddScore.lAddLessScore = m_lAddLessScore;
		CopyMemory(AddScore.lTableScore, m_lTableScore, sizeof(AddScore.lTableScore));
		CopyMemory(AddScore.lTotalScore, m_lTotalScore, sizeof(AddScore.lTotalScore));

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER, 1, 0);
		return true;
	}

	ZeroMemory(m_TabelScoreBuffer, sizeof(m_TabelScoreBuffer));

	//ƽ�����
	m_cbBalanceCount++;
	m_wOperaCount = 0;
	m_nPassCardCount = 0;

	//D����ע
	WORD wDUser = m_wDUser;
	for (BYTE i = 0; i < m_wPlayerCount; i++)
	{
		wDUser = (m_wDUser + i) % GAME_PLAYER;
		if (m_cbPlayStatus[wDUser] == TRUE && m_cbShowHand[wDUser] == FALSE) break;
	}

	//��ֵ����
	m_lBalanceScore = 0L;
	m_lTurnLessScore = 0L;
	m_lAddLessScore = m_lCellScore*m_ISetDJPK[2] / 10;	//��С��ע

	//LONG TotalBuffer[GAME_PLAYER];	// ��������ע
	//BYTE TotalChair[GAME_PLAYER];	// ���������Ӻ�
	//memset(TotalBuffer, 0, sizeof(TotalBuffer));
	//memset(TotalChair, INVALID_CHAIR, sizeof(TotalChair));

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_cbPlayStatus[i] == FALSE) continue;

		//��һ�ֿ���ע���ע��
		m_lTurnMaxScore[i] = m_lUserMaxScore[i] - m_lTotalScore[i];
		ASSERT(m_lTurnMaxScore[i] == m_lUserTakeGold[i]);

		////����������
		//if (m_cbShowHandBuffer[i] == TRUE && m_lTableScore[i]>0)
		//{
		//	TotalBuffer[i] = m_lTableScore[i];
		//	TotalChair[i] = i;
		//}
	}

	//��������
	CMD_S_TurnOver TurnOver;
	ZeroMemory(&TurnOver, sizeof(CMD_S_TurnOver));
	TurnOver.wAddScoreUser = wChairID;
	TurnOver.lAddScoreCount = lScore;
	TurnOver.lTurnLessScore = m_lTurnLessScore;
	TurnOver.lTurnMaxScore = m_lTurnMaxScore[m_wCurrentUser];
	TurnOver.lAddLessScore = m_lAddLessScore;
	TurnOver.cbChipTypeID = m_cbChipTypeID;
	TurnOver.lAllScore = m_lAllScore;
	CopyMemory(TurnOver.lTableScore, m_lTableScore, sizeof(TurnOver.lTableScore));
	CopyMemory(TurnOver.lTotalScore, m_lTotalScore, sizeof(TurnOver.lTotalScore));

	/////////////////////////�������ע��ע�����ֽ���/////////////////////////////////////////////////
	int iSuoHaCountBuffer = 0;	// �Ѿ�������
	for (int iSuo = 0; iSuo < GAME_PLAYER; iSuo++)
	{
		//����ж�
		if (m_cbPlayStatus[iSuo] == TRUE && (m_lTotalScore[iSuo] >= m_lUserMaxScore[iSuo]))
		{
			iSuoHaCountBuffer++;
		}
	}

	m_ChipPoolCount = 0;				// ������ע���䣬ÿ�η��䶼Ҫ����
	
	//�������³�ʼ��
	ZeroMemory(m_aryChipPool, sizeof(m_aryChipPool));
	ZeroMemory(m_cbHandChairCount, sizeof(m_cbHandChairCount));

	if (iSuoHaCountBuffer > 0)				// ��������
	{
		::OutputDebugString(TEXT("һ�ּ�ע���� �������� ��ʼ���б߳ط���\n"));

		LONG lSuoTotalScore[GAME_PLAYER] = {0};	// �ۼ���ע
		CopyMemory(lSuoTotalScore, m_lTotalScore, sizeof(m_lTotalScore));

		LONG TotalBuffer[GAME_PLAYER] = {0};	// ��������ע
		BYTE TotalChair[GAME_PLAYER] = {0};		// ���������Ӻ�
		memset(TotalBuffer, 0, sizeof(TotalBuffer));
		memset(TotalChair, 0xff, sizeof(TotalChair));

		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_cbPlayStatus[i] == FALSE) continue;

			//������
			if (m_lTotalScore[i]>0 && m_lTotalScore[i] >= m_lUserMaxScore[i])
			{
				TotalBuffer[i] = m_lTotalScore[i];
				TotalChair[i] = i;
			}
		}

		// ע���С��������
		m_GameLogic.SortValueList(TotalBuffer, TotalChair, GAME_PLAYER);

		LONG lPoolBuffer = 0;				// ��¼��һ�������ע���

		//�ֱ߳�
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			//û���������û��ע
			if (TotalBuffer[i] == 0 || TotalChair[i] == INVALID_CHAIR || lSuoTotalScore[TotalChair[i]] == 0)  continue;

			//���û����ע��������ĳ������һ������ĳ�����ͬ
			if (lPoolBuffer == TotalBuffer[i])	continue;

			// ���ϸ������ע��ֵ
			LONG TotalBuffer2 = TotalBuffer[i] - lPoolBuffer;
			lPoolBuffer = TotalBuffer[i];

			for (int j = 0; j < GAME_PLAYER; j++)
			{
				LONG Score = lSuoTotalScore[j];

				// ��������ע
				if (Score <= 0) continue;

				// ��ҷ�����������ע
				if (FALSE == m_cbPlayStatus[TotalChair[i]]) Score = 0;

				// ��¼�Ⲩ������ע�����
				if (Score > 0 && (m_cbPlayStatus[j] == TRUE || m_cbShowHandBuffer[j] == TRUE))
				{
					BYTE cbHandChairCount = m_cbHandChairCount[m_ChipPoolCount];

					ASSERT((m_ChipPoolCount < 9) && (cbHandChairCount < 9));
					if ((m_ChipPoolCount < 9) && (cbHandChairCount < 9))
					{
						m_cbHandChair[m_ChipPoolCount][cbHandChairCount] = j;
						m_cbHandChairCount[m_ChipPoolCount]++;
					}
					else
					{
						CString strinfo;
						strinfo.Format(TEXT("m_ChipPoolCount = %d, m_cbHandChairCount[%d]=%d"), m_ChipPoolCount, m_ChipPoolCount, cbHandChairCount);
						::OutputDebugString(strinfo);
					}
				}

				if (lSuoTotalScore[j] > TotalBuffer2)
				{
					Score = TotalBuffer2;
					lSuoTotalScore[j] -= TotalBuffer2;
				}
				else
				{
					Score = lSuoTotalScore[j];
					lSuoTotalScore[j] = 0;
				}

				m_aryChipPool[m_ChipPoolCount] += Score;
			}

			m_ChipPoolCount++;
		}

		//���һ���߳�
		ASSERT(m_ChipPoolCount <= GAME_PLAYER);
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (lSuoTotalScore[i] > 0)
			{
				m_aryChipPool[m_ChipPoolCount] += lSuoTotalScore[i];

				// ������Ϸ��״̬�����뽱�ط���
				if (FALSE == m_cbPlayStatus[i]) continue;

				BYTE cbHandChairCount = m_cbHandChairCount[m_ChipPoolCount];
				ASSERT((m_ChipPoolCount < 9) && (cbHandChairCount < 9));
				if ((m_ChipPoolCount < 9) && (cbHandChairCount < 9))
				{
					m_cbHandChair[m_ChipPoolCount][cbHandChairCount] = i;
					m_cbHandChairCount[m_ChipPoolCount]++;
				}
				else
				{
					CString strinfo;
					strinfo.Format(TEXT("m_ChipPoolCount = %d, m_cbHandChairCount[%d]=%d"), m_ChipPoolCount, m_ChipPoolCount, cbHandChairCount);
					::OutputDebugString(strinfo);
				}
			}
		}

		::OutputDebugString(TEXT("һ�ּ�ע���� �������� �����߳ط���\n"));
	}
	else
	{
		::OutputDebugString(TEXT("һ�ּ�ע���� �������� ��ʼ���ط���\n"));

		// û�������������������һ������
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_lTotalScore[i] > 0)
			{
				m_aryChipPool[0] += m_lTotalScore[i];

				// ������Ϸ��״̬�����뽱�ط���
				if (FALSE == m_cbPlayStatus[i]) continue;

				BYTE cbHandChairCount = m_cbHandChairCount[0];
				ASSERT(cbHandChairCount < 9);
				if (cbHandChairCount < 9)
				{
					m_cbHandChair[0][cbHandChairCount] = i;
					m_cbHandChairCount[0]++;
				}
				else
				{
					CString strinfo;
					strinfo.Format(TEXT("m_ChipPoolCount = %d, m_cbHandChairCount[%d]=%d"), m_ChipPoolCount, m_ChipPoolCount, cbHandChairCount);
					::OutputDebugString(strinfo);
				}
			}
		}

		::OutputDebugString(TEXT("һ�ּ�ע���� �������� �������ط���\n"));
	}
		
	/////////////////////////�������ע��ע�����ֽ���/////////////////////////////////////////////////

	CopyMemory(TurnOver.aryChipPool, m_aryChipPool, sizeof(m_aryChipPool));

	int iChipPoolCount = 0;		// ��������
	for (int ic = 0; ic < GAME_PLAYER; ic++)
	{
		if (m_aryChipPool[ic] > 0)
		{
			iChipPoolCount++;
		}
	}
	m_ChipPoolCount = iChipPoolCount;
	ASSERT(m_cbHandChairCount[m_ChipPoolCount - 1]>0);	//�����б��������

	TurnOver.cbChipPoolCount = iChipPoolCount;
	TurnOver.cbBalanceCount = m_cbBalanceCount;

	// ����ÿ�ֱ���
	m_lBalanceScore = 0L;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_cbShowHandBuffer, sizeof(m_cbShowHandBuffer));

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TURN_OVER, &TurnOver, sizeof(CMD_S_TurnOver));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TURN_OVER, &TurnOver, sizeof(CMD_S_TurnOver));

	//���ֻʣһ����Ҳ�����
	if (m_cbChipTypeID == CHIP_TYPE_OVER)
	{
		return true;
	}

	if (m_cbBalanceCount != 4)
	{
		m_pITableFrame->SetGameTimer(IDI_TIME_ADDBUFFER, 1000, 1, 0);
	}
	else
	{
		m_pITableFrame->SetGameTimer(IDI_TIME_ADDBUFFER, 500, 1, 0);
	}

	return true;
}

//�����Ϣ
void TraceMessage(LPCTSTR pszMessage)
{
	CFile File;
	if ((File.Open(TEXT("TraceData.txt"), CFile::modeWrite) == FALSE) &&
		(File.Open(TEXT("TraceData.txt"), CFile::modeWrite | CFile::modeCreate) == FALSE))
	{
		ASSERT(FALSE);
		return;
	}

	File.SeekToEnd();
	File.Write(pszMessage, lstrlen(pszMessage));
	File.Write(TEXT("\r\n"), lstrlen(TEXT("\r\n")));

	File.Flush();
	File.Close();

	return;
}

// ͳ�Ƹ���������ҵ���Ӯ
bool CTableFrameSink::WriteAllScore(LONG UserScore[], LONG Tax[])
{
	LONG ScoreBuffer = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (0 == UserScore)continue;

		// û������Ϸ���
		if (m_cbPlayStatus[i] == FALSE && m_cbGiveUpStatus[i] <= 0) continue;

		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem == NULL) continue;

		if (!pServerUserItem->IsAndroidUser() && UserScore[i] != 0)
		{
			if (UserScore[i] > 0)
				ScoreBuffer += UserScore[i] + Tax[i];
			else
				ScoreBuffer += UserScore[i];
		}
	}

	if (ScoreBuffer != 0)
	{
		m_UserAllScore = GetPrivateProfileInt(TEXT("DZPKSET"), TEXT("UserWin"), 0, CGameLogic::m_strFileName2);	//��ȡ��ҵ�����Ӯֵ	
		m_UserAllScore += ScoreBuffer;

		CString BB1;
		BB1.Format(TEXT("%d"), m_UserAllScore);
		WritePrivateProfileString(TEXT("DZPKSET"), TEXT("UserWin"), BB1, CGameLogic::m_strFileName2);
	}

	return true;
}

//��ע���
bool CTableFrameSink::IsGenreGold(void)
{
	//if (IsGoldServer(m_pGameServiceOption->wServerType))   //��ҳ����õ�ע 
	//{
	//	LONG lLessCell = m_pGameServiceOption->lCellScore;
	//	LONG lMaxCell = m_pGameServiceOption->lMaxCellScore;
	//	if (lLessCell < lMaxCell)  //���Ҳ��ǹ̶���ע
	//	{
	//		return true;
	//	}
	//}

	return false;
}

void CTableFrameSink::DoUserSitDown(IServerUserItem * pIServerUserItem)
{
	WORD nChairID = pIServerUserItem->GetChairID();
	if (0xff != m_pHostChairID && m_pHostChairID == nChairID && 0 == m_lBaseScore)  //���������õ�ע
	{
		SendSetBaseScore(pIServerUserItem, SETBASE_SET);
		m_pITableFrame->SetGameTimer(IDI_AGREE_BASESCORE, IDT_AGREE_BASESCORE, 1, m_wCurrentUser); //������ʱ��
	}
	else
	{
		SendSetBaseScore(pIServerUserItem, SETBASE_WAIT); //�ȴ���������
		// 		CMD_S_StatusFree StatusFree;
		// 		ZeroMemory(&StatusFree,sizeof(StatusFree));
		// 		
		// 		StatusFree.lCellScore=0L;   //���ñ���
		// 		StatusFree.lBaseScore = m_lBaseScore;
		// 		m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree)); //���ͳ���
	}
}

bool CTableFrameSink::SendSetBaseScore(IServerUserItem * pIServerUser, BYTE bSetBaseScore /*= 1*/)
{
	if (pIServerUser)
	{
		CMD_S_Set_BaseScore SetBaseScore;
		SetBaseScore.wChairID = pIServerUser->GetChairID();
		SetBaseScore.lBaseScore = m_lBaseScore;
		SetBaseScore.lCellScore = m_pGameServiceOption->lCellScore;
		SetBaseScore.lMaxCellScore = m_pGameServiceOption->lMaxEnterScore;

		BYTE result = bSetBaseScore;
		if (0 == bSetBaseScore && 0 < m_lBaseScore)      result = SETBASE_CONFIRM;//result = SETBASE_OVER;
		else if (0 == bSetBaseScore && 0 == m_lBaseScore)
		{
			result = SETBASE_WAIT;
			//m_listConfirmBase.push_back(SetBaseScore.wChairID);
		}
		SetBaseScore.bSetOrConfirm = result;

		//LOGTOFINE(SendSetBaseScore,SetBaseScore.wChairID,SetBaseScore.bSetOrConfirm);
		m_pITableFrame->SendTableData(SetBaseScore.wChairID, SUB_S_SET_BASESCORE, &SetBaseScore, sizeof(SetBaseScore));

		if (result == SETBASE_WAIT) //��ע��û�趨 �ȴ�
			return true;
	}

	return true;
}

bool CTableFrameSink::CheckAgreeBase()
{
	bool bFind = false;

	for (IteListPlayer ite = m_listConfirmBase.begin(); ite != m_listConfirmBase.end(); ++ite)  //���������ȷ��ͬ������������
	{
		WORD wChairID = *ite;
		if (wChairID < m_wPlayerCount && wChairID >= 0)
		{
			IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if (pUserItem && 0 == m_bAgreeBase[(int)wChairID])
			{
				SendSetBaseScore(pUserItem, SETBASE_CONFIRM);
				bFind = true;
			}
		}
	}

	return bFind;
}

bool CTableFrameSink::CheckMoneyLimit(LONG lScoreLimit)
{
	bool bKick = false;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		IServerUserItem* pUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pUserItem != NULL)
		{
			LONG lScore = pUserItem->GetUserScore();
			if (lScore < lScoreLimit)
			{
				SendKickOutMsg(i);
				bKick = true;
			}
		}
	}

	return bKick;
}

void CTableFrameSink::SendKickOutMsg(WORD ChairID)
{
	IServerUserItem* pIServerUser = m_pITableFrame->GetTableUserItem(ChairID);
	if (pIServerUser)
	{
		CMD_S_KickOut KickOut;
		KickOut.bReasion = 0;
		KickOut.wChairID = ChairID;

		//pIServerUser->SetForceLeave();
		m_pITableFrame->SendTableData(ChairID, SUB_S_KICK_OUT, &KickOut, sizeof(KickOut));
	}
}

bool CTableFrameSink::OnUserSetBase(CMD_C_SetBaseScore* pBaseScore, IServerUserItem * pIServerUserItem)
{
	if ( /*pBaseScore->bSetOrConfirm  == SETBASE_OVER || */
		pBaseScore->bSetOrConfirm == SETBASE_WAIT)
		return true;

	if (pBaseScore->bSetOrConfirm == SETBASE_NOTAGREE)  //��ͬ���ע
	{
		SendKickOutMsg(pIServerUserItem->GetChairID());
		return true;
	}

	m_bAgreeBase[pIServerUserItem->GetChairID()] = 1;

	if (pBaseScore->bSetOrConfirm == SETBASE_SET) //���õ�ע
	{
		m_lBaseScore = pBaseScore->lBaseScore;
		SCORE lMinSitScore = m_lBaseScore*GOLD_RATE;

		ASSERT(pBaseScore->bUserChairID == m_pHostChairID);
		ASSERT(m_lBaseScore != 0);
		ASSERT(lMinSitScore <= pIServerUserItem->GetUserScore());    //���õ�ע��������ϵ�ǮҪ���ڵ�ע��1200��

		CheckMoneyLimit(lMinSitScore);  //���������ҵ�Ǯ�Ƿ��������� �������߳�

		if (CheckAgreeBase())
		{
			m_pITableFrame->SetGameTimer(IDI_AGREE_BASESCORE, IDT_AGREE_BASESCORE, 1, m_wCurrentUser); //������ʱ��
		}

	}

	return true;
}

bool CTableFrameSink::OnTimerKickOut()
{
	m_pITableFrame->KillGameTimer(IDI_AGREE_BASESCORE);

	for (int i = 0; i < m_wPlayerCount; ++i)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
		if (NULL != pIServerUser)
		{
			if (0 == m_bAgreeBase[i])
			{
				SendKickOutMsg(i);
			}
		}
	}

	//�ȵ�����뿪����ж���Ϸ�Ƿ���Կ�ʼ

	return true;
}

bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}

bool CTableFrameSink::OnUserDaShang(WORD wChairID)
{
	if (VALIDE_CHAIRID(wChairID))
	{
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem == nullptr) return true;

		// ���ϻ���
		LONG lTrusteeScore = pIServerUserItem->GetTrusteeScore();

		if (lTrusteeScore < m_pGameServiceOption->lGratuityScore)
		{
			CString str;
			str.Format(TEXT("��� %s ���Ͻ�� %d ��Ϸ��ʼʱ���� %d ���д����� %d"), pIServerUserItem->GetNickName(),
				lTrusteeScore, m_lUserMaxScore[wChairID], m_lUserTakeGold[wChairID]);
			m_pITableFrame->ExportInformation(str, false);
			return true;
		}
		
		//��������
		if (pIServerUserItem->WriteOtherScore(m_pGameServiceOption->lGratuityScore, 0) == false)
		{
			//SendGameMessage(pIServerUserItem, TEXT("��Ǹ�����Ľ�Ҳ��㣬�޷�����ħ�����飡"), SMT_EJECT | SMT_CHAT);
			return true;
		}

		//m_nDaShangCount[wChairID]++;
		//m_lUserTakeGold[wChairID] -= DASHANG_COST;
		//m_lUserMaxScore[wChairID] -= DASHANG_COST;

		CMD_S_DaShang  DaShang;
		DaShang.wChairID = wChairID;
		DaShang.lUserScore = m_lUserTakeGold[wChairID];
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_DASHANG, &DaShang, sizeof(DaShang));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_DASHANG, &DaShang, sizeof(DaShang));
		return true;
	}
	return false;
}

SCORE CTableFrameSink::GetGameLockGold(IServerUserItem * pIServerUserItem)
{
	if (pIServerUserItem)
	{
		WORD wChairID = pIServerUserItem->GetChairID();
		if (TRUE == m_cbPlayStatus[wChairID])
		{
			SCORE lLockScore = m_lUserMaxScore[wChairID];      //��ע���

			return lLockScore;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////

