#include "StdAfx.h"
#include "TableFrameSink.h"
//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//游戏人数


#define	IDI_TIME_GAME				1				//游戏定时器

#define IDI_GAME_COMPAREEND			2				//结束定时器

#define	IDI_TIME_GameCard			3				//发牌延迟
#define IDI_GAME_NORMALEND			4				//正常结束定时器
#define IDI_TIME_GAMEOVER           5				//游戏结束
#define	IDT_CHIPTIMER			    17000			//游戏定时器时间
#define IDI_GAME_START              6               //加注延迟
#define	IDI_TIME_ADDBUFFER			7				//中心牌延迟
#define	IDI_TIME_SENDCARD			8				//发牌阶段

#define IDI_GAME_NO_PLAYER			9				//放弃结束

#define IDI_AGREE_BASESCORE         10
#define IDT_AGREE_BASESCORE         (25*1000 + 500) //同意底注

//////////////////////////////////////////////////////////////////////////

void TraceMessage(LPCTSTR pszMessage);

LONG CTableFrameSink::m_ISetDJPK[9];

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//组件变量
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;

	//玩家变量
	m_wDUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	//m_wWinner = INVALID_CHAIR;

	//玩家状态
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));

	//扑克变量
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData, sizeof(m_cbCenterCardData));
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	//ZeroMemory(m_nDaShangCount, sizeof(m_nDaShangCount));

	//加注变量
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

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);

	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);
	if (m_pITableFrame == NULL) return false;
	m_pHostChairID = 0xff;
	m_lBaseScore = 0;
	m_bGameStart = false;
	ZeroMemory(m_bAgreeBase, sizeof(m_bAgreeBase));

	//获取参数
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption != NULL);
	SetDzpkInit();

	return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
	//玩家变量
	m_wCurrentUser = INVALID_CHAIR;
	//玩家状态
	ZeroMemory(m_cbPlayStatus, sizeof(m_cbPlayStatus));
	//扑克变量
	m_cbSendCardCount = 0;
	ZeroMemory(m_cbCenterCardData, sizeof(m_cbCenterCardData));
	//ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	//ZeroMemory(m_nDaShangCount,sizeof(m_nDaShangCount));

	//加注变量
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

//用户坐下 
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (!bLookonUser)
	{
		m_lUserTakeGold[wChairID] = 0;  //金币带入
		m_lUserMaxScore[wChairID] = 0;

		//if (IsGenreGold())
		//{
		//	if (find(m_listPlayers.begin(), m_listPlayers.end(), wChairID) == m_listPlayers.end())
		//	{
		//		m_listPlayers.push_back(wChairID);
		//		TCHAR szString[256];
		//		_sntprintf_s(szString, CountArray(szString), TEXT("%d桌%d位 玩家坐下 m_lBaseScore = %d m_pHostChairID = %d\n"), m_pITableFrame->GetTableID(), wChairID, m_lBaseScore, m_pHostChairID);
		//		//WriteLog(buf);
		//		m_bAgreeBase[wChairID] = 0;  //玩家坐下 把当前座位设为未同意底注设置

		//		int userCount = 0;
		//		for (int i = 0; i < m_wPlayerCount; ++i)
		//		{
		//			IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

		//			if (NULL != pIServerUserItem)
		//			{
		//				++userCount;
		//			}
		//		}

		//		if (0xff == m_pHostChairID && 0 == m_lBaseScore && 1 == userCount)  //保存桌主信息 让桌主设置底注
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

		//		if (!m_bGameStart && pIServerUserItem->IsAndroidUser()) //机器人设置底注
		//		{
		//			DoUserSitDown(pIServerUserItem);  //发送设置 确认 底注 消息
		//		}
		//	}
		//}
		//else
		{
			//自身拥有的金币量
			SCORE lScore = m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore();

			m_lUserTakeGold[wChairID] = lScore;  //金币带入
			m_lUserMaxScore[wChairID] = lScore;
		}
	}
	return true;
}

// 玩家准备
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	// 忘记不为旁观状态
	if (pIServerUserItem->GetUserStatus() != US_LOOKON)
	{

	}

	return true;
}

//用户起来
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

		//		if (0 == m_lBaseScore && wChairID == m_pHostChairID) //桌主离开 并且没有设置底注
		//		{
		//			if (m_listPlayers.size() > 0)
		//			{
		//				m_pHostChairID = m_listPlayers.front();
		//				SendSetBaseScore(m_pITableFrame->GetTableUserItem(m_pHostChairID), SETBASE_SET);
		//				m_pITableFrame->KillGameTimer(IDI_AGREE_BASESCORE);
		//				m_pITableFrame->SetGameTimer(IDI_AGREE_BASESCORE, IDT_AGREE_BASESCORE, 1, m_wCurrentUser); //启动定时器
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

		//		m_bAgreeBase[wChairID] = 0;  //玩家离开 把当前座位设为未同意底注设置

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

//读取配置文件
BOOL CTableFrameSink::SetDzpkInit(BYTE ID)
{
	//设置文件名
	//小盲注5为单元注的0.5倍,10为1.0倍
	m_ISetDJPK[0] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom0"), 10, CGameLogic::m_strFileName);
	//大盲注5为单元注的0.5倍,10为1.0倍
	m_ISetDJPK[1] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom1"), 20, CGameLogic::m_strFileName);
	//第二轮的最低下注
	m_ISetDJPK[2] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom2"), 20, CGameLogic::m_strFileName);
	//首轮所有玩家的最低投注
	m_ISetDJPK[3] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom3"), 0, CGameLogic::m_strFileName);
	m_ISetDJPK[4] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom4"), 300, CGameLogic::m_strFileName);

	m_ISetDJPK[5] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom5"), 600, CGameLogic::m_strFileName);
	m_ISetDJPK[6] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom6"), 200, CGameLogic::m_strFileName);
	m_ISetDJPK[7] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom7"), 150, CGameLogic::m_strFileName);
	m_ISetDJPK[8] = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("WuXianRoom8"), 50, CGameLogic::m_strFileName);

	m_UserAllScore = GetPrivateProfileInt(TEXT("DZPKSET"), TEXT("UserWin"), 0, CGameLogic::m_strFileName2);	//获取玩家的总输赢值
	m_UserWinOk = GetPrivateProfileInt(TEXT("DZPKSET"), TEXT("UserWinOk"), 0, CGameLogic::m_strFileName2);	//是否启动换牌机制

	//m_lAddTakeMinTimes = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("AddTakeGoldMin"), 100, CGameLogic::m_strFileName);
	//m_lAddTakeMaxTimes = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("AddTakeGoldMax"), 1000, CGameLogic::m_strFileName);
	int TableVipSet = GetPrivateProfileInt(TEXT("DZPKTYPE"), TEXT("TableVipLimit"), 95, CGameLogic::m_strFileName);
	m_fTableVipLimit = (float)TableVipSet / 10;  //9.5倍

	return TRUE;
}

//读取配置文件
bool CTableFrameSink::CardExchange(BYTE ID)
{
	if (m_UserAllScore <= 0) return false;
	bool AndroidUser = false, Buser = false;
	WORD AndroidUserid = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		//无效用户
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
	BYTE bLastCenterALLCardData[GAME_PLAYER][MAX_CENTERCOUNT] = { 0 };//最后完整扑克	
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

//计算玩家最大可下注
void CTableFrameSink::CalUserMaxChipGold(WORD wChairID)
{
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	SCORE lScore = pIServerUserItem->GetUserScore();

	m_lUserTakeGold[wChairID] = lScore;		// 同步新带入金币
	m_lUserMaxScore[wChairID] = lScore;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	m_bGameStart = true;

	//设置状态
	m_pITableFrame->SetGameStatus(GAME_STATUS_PLAY);
	RepositionSink();

	// 游戏开始的时候清除该数据，针对玩家在游戏结束后亮牌
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_lUserTakeGold, sizeof(m_lUserTakeGold));
	srand(GetTickCount() + m_pITableFrame->GetTableID());
	SetDzpkInit();
	//游戏变量	
	BYTE wUserCount = 0;
	//下注变量
	if (m_lCellScore <= 0)
	{
		m_lCellScore = m_pITableFrame->GetCellScore();
	}

	memset(m_bFristPlayStatus, 0, sizeof(m_bFristPlayStatus));
	memset(m_cbShowHandBuffer, 0, sizeof(m_cbShowHandBuffer));
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		//无效用户
		if (pIServerUserItem == NULL) continue;

		CalUserMaxChipGold(i);
		//设置状态
		m_cbPlayStatus[i] = TRUE;
		m_bFristPlayStatus[i] = true;

		wUserCount++;
	}

	//混乱扑克	
	BYTE cbRandCard[FULL_COUNT];
	ZeroMemory(cbRandCard, sizeof(cbRandCard));
	m_GameLogic.RandCardList(cbRandCard, wUserCount*MAX_COUNT + MAX_CENTERCOUNT);

	//用户扑克
	WORD wCardCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			CopyMemory(&m_cbHandCardData[i], &cbRandCard[(wCardCount++)*MAX_COUNT], sizeof(BYTE)*MAX_COUNT);
		}
	}

	//中心扑克
	CopyMemory(m_cbCenterCardData, &cbRandCard[wUserCount*MAX_COUNT], CountArray(m_cbCenterCardData));
	CardExchange(0);

	wCardCount = 0;
	//扑克数目
	m_cbSendCardCount = 0;
	m_cbBalanceCount = 0;
	//首家判断
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

	//盲注玩家
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

	//当前下注
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

	//首轮最小下注
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
	if (m_cbShowHand[wPlayer[2]])//如果
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

	//构造变量
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

	//作弊/漏洞数据	
	//发送数据
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
					iIndex = 0;//普通
				}
				else if (m_ISetDJPK[5] <= Ai && Ai <= m_ISetDJPK[5] + m_ISetDJPK[6])
				{
					iIndex = 1;//变态
				}
				else if (m_ISetDJPK[5] + m_ISetDJPK[6] <= Ai && Ai <= m_ISetDJPK[5] + m_ISetDJPK[6] + m_ISetDJPK[7])
				{
					iIndex = 2;//平庸
				}
				else
				{
					iIndex = 3;//特殊
				}
				CopyMemory(m_GameRobot.lRobotAdd, CGameLogic::m_lAdd[iIndex], sizeof(m_GameRobot.lRobotAdd));
				CopyMemory(m_GameRobot.lRobotQuanYa, CGameLogic::m_lQuanYa[iIndex], sizeof(m_GameRobot.lRobotQuanYa));
				CopyMemory(m_GameRobot.lRobotGiveUp, CGameLogic::m_lGiveUp[iIndex], sizeof(m_GameRobot.lRobotGiveUp));
				m_pITableFrame->SendTableData(i, SUB_S_USER_ROBOT, &m_GameRobot, sizeof(m_GameRobot));
			}
			//发送数据		
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

	// 不发送玩家手里牌到旁观玩家手中
	ZeroMemory(GameStart.cbCardData, MAX_COUNT);
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_START, &GameStart, sizeof(GameStart));
	m_pITableFrame->SetGameTimer(IDI_TIME_SENDCARD, wUserCount * 1000 + 2000, 1, 0);

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
		case GER_DISMISS:		//游戏解散
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
		case GER_NORMAL:		//常规结束
		{
			if (m_GameOver)
			{
				return true;
			}

			m_bGameStart = false;	//游戏结束标识

			try
			{
				m_GameOver = true;
				KillTimerAll();

				//定义变量
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd, sizeof(GameEnd));
				GameEnd.cbTotalEnd = 1;

				//扑克数据
				BYTE cbEndCardData[GAME_PLAYER][MAX_CENTERCOUNT];
				BYTE cbCardTypeData[MAX_CENTERCOUNT];
				ZeroMemory(cbEndCardData, sizeof(cbEndCardData));
	
				BYTE cbUserCount = 0;

				try
				{
					//获取扑克
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						if (m_cbPlayStatus[i] == FALSE) continue;

						//统计最后参与游戏的玩家数量，不包括放弃玩家
						cbUserCount++;

						ZeroMemory(cbCardTypeData, sizeof(cbCardTypeData));

						//获取自己的最大牌型
						BYTE cbEndCardKind = m_GameLogic.FiveFromSeven(m_cbHandCardData[i], MAX_COUNT, m_cbCenterCardData, MAX_CENTERCOUNT, cbEndCardData[i], MAX_CENTERCOUNT);

						//最后玩家牌型
						m_GameLogic.FinishCard(cbEndCardData[i], cbCardTypeData, MAX_CENTERCOUNT, m_cbHandCardData[i], cbEndCardKind);
						CopyMemory(GameEnd.cbLastCenterCardData[i], cbEndCardData[i], sizeof(cbEndCardData[i]));

						//结束牌型
						GameEnd.cbCardType[i] = cbEndCardKind;

						//结束时，累计下注
						GameEnd.lTotalScore[i] = m_lTotalScore[i];

						ASSERT(cbEndCardKind != FALSE);
					}
				}
				catch (...)
				{
					m_pITableFrame->ExportInformation(TEXT("用户过滤v最大牌型"), true);
					ASSERT(FALSE);
				}

				//总下注备份
				LONG lTotalScore[GAME_PLAYER];
				ZeroMemory(lTotalScore, sizeof(lTotalScore));
				CopyMemory(lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));

				//胜利列表
				UserWinList WinnerList[GAME_PLAYER];
				ZeroMemory(WinnerList, sizeof(WinnerList));

				//临时数据
				BYTE bTempData[GAME_PLAYER][MAX_CENTERCOUNT];
				CopyMemory(bTempData, cbEndCardData, sizeof(bTempData));

				WORD wWinCount = 0;
				try
				{
					//用户得分顺序
					for (WORD i = 0; i < m_wPlayerCount; i++)
					{
						//查找最大用户
						if (!m_GameLogic.SelectMaxUser(bTempData, WinnerList[i], m_cbHandCardData, lTotalScore))
						{
							wWinCount = i;
							break;
						}

						//删除胜利数据
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
					m_pITableFrame->ExportInformation(TEXT("用户得分顺序"), true);
					ASSERT(FALSE);
				}

				//放弃用户
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i] == FALSE && lTotalScore[i]>0l)
					{
						WinnerList[wWinCount].wWinerList[WinnerList[wWinCount].bSameCount++] = i;
					}
					//if(m_cbPlayStatus[i] ==FALSE)continue;				
				}

				//得分变量
				LONG lUserScore[GAME_PLAYER];
				ZeroMemory(lUserScore, sizeof(lUserScore));

				//得分情况
				for (int i = 0; i < m_wPlayerCount - 1; i++)
				{
					//胜利人数
					int iWinCount = (int)WinnerList[i].bSameCount;

					// 当玩家数量为0时，已经分配结束
					if (0 == iWinCount)break;

					//胜利用户得分情况
					for (int j = 0; j < iWinCount; j++)
					{
						//胜利玩家
						WORD wWinChairID = WinnerList[i].wWinerList[j];

						if (0 == lTotalScore[wWinChairID])continue;

						// 出现牌型相同玩家，并且两人下注金额相同
						if (j > 0 && lTotalScore[wWinChairID] - lTotalScore[WinnerList[i].wWinerList[j - 1]] == 0)
						{
							continue;
						}

						//比wWinChairID牌型小的玩家
						for (int k = i + 1; k < m_wPlayerCount; k++)
						{
							//失败人数
							if (0 == WinnerList[k].bSameCount)break;

							// 多个人输的时候，一个个处理
							for (int l = 0; l < WinnerList[k].bSameCount; l++)
							{
								//失败玩家
								WORD wLoserChairID = WinnerList[k].wWinerList[l];

								//输家下注已经被分配完毕
								if (0 == lTotalScore[wLoserChairID])continue;

								LONG lMinScore = 0;		// 玩家应该赢取多少积分

								//上家得分数目
								LONG lLastScore = 0;
								if (j > 0)
								{
									lLastScore = lTotalScore[WinnerList[i].wWinerList[j - 1]];
									if (lLastScore <= 0) continue;
								}

								lMinScore = min(lTotalScore[wWinChairID] - lLastScore, lTotalScore[wLoserChairID]);
								for (int m = j; m < iWinCount; m++)
								{
									//得分数目
									lUserScore[WinnerList[i].wWinerList[m]] += lMinScore / (iWinCount - j);
								}

								//赔偿数目
								lUserScore[wLoserChairID] -= lMinScore;
								lTotalScore[wLoserChairID] -= lMinScore;
							}
						}
					}
				}

				//服务费变量
				LONG lWinChipBuffer[GAME_PLAYER];			//赢得的筹码

				//统计用户分数(服务费)
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

				// 主池和每个边池的具体分配
				LONG lChipScores[GAME_PLAYER][GAME_PLAYER];
				ZeroMemory(lChipScores, sizeof(lChipScores));

				int iChipPoolCount = 0;		// 奖池数量
				for (int ic = 0; ic < GAME_PLAYER; ic++)
				{
					if (m_aryChipPool[ic] > 0)
					{
						iChipPoolCount++;
					}
				}

				if (iChipPoolCount > 0)
				{
					//边池所属列表
					UserWinList BianChiList[GAME_PLAYER];
					ZeroMemory(BianChiList, sizeof(BianChiList));

					//每个边池输赢计算
					for (int i = 0; i < iChipPoolCount; i++)
					{
						//临时数据
						BYTE bTempData[GAME_PLAYER][MAX_CENTERCOUNT] = { 0 };
						ZeroMemory(bTempData, sizeof(bTempData));

						//每个边池中梭哈的玩家
						for (int j = 0; j < m_cbHandChairCount[i]; j++)
						{
							BYTE Bi = m_cbHandChair[i][j];
							if (m_cbPlayStatus[Bi] == TRUE)
							{
								CopyMemory(bTempData[Bi], cbEndCardData[Bi], sizeof(BYTE)*MAX_CENTERCOUNT);
							}
						}

						//计算每个边池胜利玩家
						m_GameLogic.SelectMaxUser(bTempData, BianChiList[i], m_cbHandCardData, NULL);
					}

					// 从主池到每个边池开始分配
					for (int a = 0; a < iChipPoolCount; a++)
					{
						// 存在赢家，有可能存在多个赢家
						if (BianChiList[a].bSameCount > 0)
						{
							for (int i = 0; i < BianChiList[a].bSameCount; i++)
							{
								if (m_cbPlayStatus[BianChiList[a].wWinerList[i]] == FALSE)continue;
								GameEnd.cbWinUserId[a][GameEnd.cbWinCount[a]++] = BianChiList[a].wWinerList[i];
							}

							// 参与奖池并且在游戏结束时，没放弃的玩家数量
							BYTE cbPlayerCount = GameEnd.cbWinCount[a];
							ASSERT(cbPlayerCount > 0);

							// 该奖池获得玩家每人获得多少
							SCORE lWinChipScore = m_aryChipPool[a] / cbPlayerCount;

							// 分配筹码
							for (int iW = 0; iW < cbPlayerCount; iW++)
							{
								lChipScores[a][iW] = lWinChipScore;
							}
						}
						else
						{
							// 该奖池没有赢家时，参与的所有玩家平分
							for (int b = 0; b < m_cbHandChairCount[a]; b++)
							{
								if (m_cbPlayStatus[m_cbHandChair[a][b]] == TRUE)
								{
									// 分配玩家的椅子号
									GameEnd.cbWinUserId[a][GameEnd.cbWinCount[a]++] = m_cbHandChair[a][b];
								}
							}

							// 参与奖池并且在游戏结束时，没放弃的玩家数量
							BYTE cbPlayerCount = GameEnd.cbWinCount[a];
							ASSERT(cbPlayerCount > 0);

							// 该奖池获得玩家每人获得多少
							SCORE lWinChipScore = m_aryChipPool[a] / cbPlayerCount;

							// 分配筹码
							for (int iW = 0; iW < cbPlayerCount; iW++)
							{
								lChipScores[a][iW] = lWinChipScore;
							}

							// 注释：每一波筹码分配时，GameEnd.cbWinUserId和lChipScores直接，是按照数据下标一一对应的
						}

						//如果没有找到所属玩家
						if (GameEnd.cbWinCount[a] <= 0)
						{
							for (int b = 0; b < GAME_PLAYER; b++)
							{
								if (m_cbPlayStatus[b] == TRUE)
								{
									GameEnd.cbWinUserId[a][GameEnd.cbWinCount[a]++] = b;
								}
							}

							// 参与奖池并且在游戏结束时，没放弃的玩家数量
							BYTE cbPlayerCount = GameEnd.cbWinCount[a];
							ASSERT(cbPlayerCount > 0);

							// 该奖池获得玩家每人获得多少
							SCORE lWinChipScore = m_aryChipPool[a] / cbPlayerCount;

							// 分配筹码
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
						if (m_pGameServiceOption->wRevenueRatio > 0)//服务费
						{
							//处理服务费
							GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i, GameEnd.lGameScore[i]);
							GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
						}
					}
				}

				GameEnd.lAllScore = m_lAllScore;
				GameEnd.cbChipPoolCount = iChipPoolCount;

				//统计用户分数		
				CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
				CopyMemory(GameEnd.lChipPool, m_aryChipPool, sizeof(m_aryChipPool));
				CopyMemory(GameEnd.cbPlayStatus, m_cbPlayStatus, sizeof(GameEnd.cbPlayStatus));
				CopyMemory(GameEnd.cbPlayGiveUp, m_cbGiveUpStatus, sizeof(GameEnd.cbPlayGiveUp));
				CopyMemory(GameEnd.cbCenterCardData, m_cbCenterCardData, sizeof(GameEnd.cbCenterCardData));
				CopyMemory(GameEnd.lChipPoolScore, lChipScores, sizeof(GameEnd.lChipPoolScore));

				// 统计该桌子玩家和机器人的输赢
				WriteAllScore(GameEnd.lGameScore, GameEnd.lGameTax);

				//修改用户分数
				tagScoreInfo ScoreInfo;// [m_wPlayerCount];


				//积分变量
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

						//写入积分
						m_aryEndScore[i] = GameEnd.lGameScore[i];

						memset(&ScoreInfo, 0, sizeof(ScoreInfo));
						ScoreInfo.lScore = GameEnd.lGameScore[i];// -m_nDaShangCount[i] * DASHANG_COST;
						//m_nDaShangCount[i] = 0;

						ScoreInfo.cbType = nScoreKind;
						ScoreInfo.lRevenue = GameEnd.lGameTax[i];
						m_pITableFrame->WriteUserScore(i, ScoreInfo);

						//输赢的金币 + 自己下注的金币 是现在自身拥有的金币带入量
						m_lUserTakeGold[i] = m_lUserTakeGold[i] + m_lTotalScore[i] + ScoreInfo.lScore;
						m_lUserMaxScore[i] = 0;

						SCORE lScore = pUserItem->GetUserScore();

						// 积分计算异常
						if (m_lUserTakeGold[i] > lScore)
						{
							ASSERT(0);
							CString str;
							str.Format(TEXT("OnEventGameEnd : GER_NORMAL m_lUserTakeGold[i] %d lScore %d"), m_lUserTakeGold[i], lScore);
							m_pITableFrame->ExportInformation(str, true);
						}

						//获取历史最大牌型
						BYTE aryCardData[MAX_CENTERCOUNT] = {0};
						BYTE cbCardType = pUserItem->GetMaxCardData(aryCardData, MAX_CENTERCOUNT);
						//当前牌型大于历史最大牌型则保存
						bool bSave = false;
						if (GameEnd.cbCardType[i]>cbCardType)
						{
							bSave = true;
						}
						//如果牌型相同则比较牌值
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

				//发送信息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

				m_GameOver = false;
				//结束游戏	
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			}
			catch (...)
			{
				ASSERT(false);
			}

			return true;
		}
		case GER_NO_PLAYER:		//没有玩家
		{
			m_bGameStart = false;
			try
			{
				m_GameOver = true;
				KillTimerAll();

				//定义变量
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd, sizeof(GameEnd));
				GameEnd.cbTotalEnd = 0;

				//效验结果
				BYTE cbUserCount = 0;
				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					if (m_cbPlayStatus[i] != FALSE) cbUserCount++;
				}
				if (cbUserCount != 1)
				{
					ASSERT(FALSE);
					m_pITableFrame->ExportInformation(TEXT("没有玩家//效验结果出错"), true);
				}

				//统计分数
				LONG lScore = 0, lRevenue = 0;
				BYTE nScoreKind = 0;
				WORD wWinner = INVALID_CHAIR;

				//结束玩家牌
				CopyMemory(GameEnd.cbCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
				CopyMemory(GameEnd.lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));

				for (WORD i = 0; i < m_wPlayerCount; i++)
				{
					//构造扑克			
					if (m_cbPlayStatus[i] == FALSE)
					{
						// 有下注，但游戏状态为FALSE的玩家是放弃玩家
						if (m_lTotalScore[i] > 0L)
						{
							GameEnd.lGameScore[i] -= m_lTotalScore[i];
						}

						// 游戏结束，玩家剩余筹码
						GameEnd.lChipScore[i] = m_lUserMaxScore[i] + GameEnd.lGameScore[i];

						continue;
					}

					wWinner = i;	// 最后一个没放弃的玩家是赢家

					//总下注数目
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

				// 只有一个赢家，将所有金币全给胜利玩家
				GameEnd.lChipPoolScore[0][0] = m_lAllScore;

				CopyMemory(GameEnd.cbPlayStatus, m_cbPlayStatus, sizeof(GameEnd.cbPlayStatus));
				CopyMemory(GameEnd.cbPlayGiveUp, m_cbGiveUpStatus, sizeof(GameEnd.cbPlayGiveUp));

				//写入积分
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

							//写入积分
							ScoreInfo[i].lScore = GameEnd.lGameScore[i];// -m_nDaShangCount[i] * DASHANG_COST;
							//m_nDaShangCount[i] = 0;
							ScoreInfo[i].cbType = nScoreKind;
							ScoreInfo[i].lRevenue = GameEnd.lGameTax[i];
							m_pITableFrame->WriteUserScore(i, ScoreInfo[i]);

							//输赢的金币 + 自己下注的金币 是现在自身拥有的金币带入量
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
					m_pITableFrame->ExportInformation(TEXT("赢家座位号不是有效值"), true);
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
			//效验参数
			ASSERT(pIServerUserItem != NULL);
			ASSERT(wChairID < m_wPlayerCount);

			//强退处理
			OnUserGiveUp(wChairID);
			return true;
		}
	}
	return false;
}

//可设置带入金币范围
void CTableFrameSink::CalRangeTakeGold(WORD wChairID, LONG & lMin, LONG & lMax)
{
	//初始化
	//lMin = m_lAddTakeMinTimes;
	//lMax = m_lAddTakeMaxTimes;

	//IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	//if (pIServerUserItem == NULL)
	//{
	//	ASSERT(0);
	//	return;
	//}

	//专属桌子
	//lMin = m_lAddTakeMinTimes*m_lCellScore;
	//lMax = m_lAddTakeMaxTimes*m_lCellScore;

	lMin = m_pGameServiceOption->lLowPutScore;
	//lMax = m_pGameServiceOption->lDefPutScore * m_pGameServiceOption->iExchangeRatio * 2;
	lMax = m_pGameServiceOption->lDefPutScore * 2;

}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
	{
		//构造数据
		CMD_S_StatusFree StatusFree;
		memset(&StatusFree, 0, sizeof(StatusFree));

		//设置变量
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
		//发送场景
		return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
	}
	case GAME_STATUS_PLAY:	//游戏状态
	{
		//构造数据
		CMD_S_StatusPlay StatusPlay;
		ZeroMemory(&StatusPlay, sizeof(StatusPlay));

		//标志变量			
		if (m_cbBalanceCount > 0)
		{
			StatusPlay.bGameId = 1;
		}
		//总下注数目
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

		//加注信息
		StatusPlay.lCellScore = m_lCellScore;
		if (m_GameOver)
		{
			StatusPlay.bGameId = 2;
		}
		StatusPlay.wDMUser[0] = m_wMUser[0];
		StatusPlay.wDMUser[1] = m_wMUser[1];

		//设置变量
		StatusPlay.wDUser = m_wDUser;
		StatusPlay.wCurrentUser = m_wCurrentUser;//当前玩家
		StatusPlay.cbChipPoolCount = m_ChipPoolCount;
		StatusPlay.cbBalanceCount = m_cbBalanceCount;
		CopyMemory(StatusPlay.aryChipPool, m_aryChipPool, sizeof(m_aryChipPool));
		CopyMemory(StatusPlay.lUserMaxScore, m_lUserMaxScore, sizeof(m_lUserMaxScore));//最大下注
		CopyMemory(StatusPlay.lTableScore, m_lTableScore, sizeof(m_lTableScore));//下注数目
		CopyMemory(StatusPlay.lTotalScore, m_lTotalScore, sizeof(m_lTotalScore));//下注数目
		CopyMemory(StatusPlay.cbGiveUpStatus, m_cbGiveUpStatus, sizeof(m_cbGiveUpStatus));//下注数目
		CopyMemory(StatusPlay.lTableBuffer, m_TabelScoreBuffer, sizeof(m_TabelScoreBuffer));//用户游戏状态
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

		//发送场景
		m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		if (pIServerUserItem->IsAndroidUser() && pIServerUserItem->GetUserStatus() == US_PLAYING)
		{
			m_pITableFrame->SendTableData(wChiarID, SUB_S_USER_ROBOT, &m_GameRobot, sizeof(m_GameRobot));
		}
		return true;
	}
	}
	//效验结果
	ASSERT(FALSE);
	return false;
}

//定时器事件
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
			if (0 == m_lTurnLessScore)  //让牌
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
	case IDI_TIME_SENDCARD://发牌结束
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
	case IDI_GAME_NORMALEND://正常结束
	{
		m_pITableFrame->KillGameTimer(IDI_GAME_NORMALEND);
		OutputDebugString(TEXT("IDI_GAME_NORMALEND\n"));
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
		return true;
	}
	case IDI_GAME_NO_PLAYER://放弃结束
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
	//结束判断
	if (m_cbBalanceCount == 4)
	{
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
		return true;
	}

	//梭哈用户统计
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

	//只剩一玩家没梭或者全梭
	if ((wShowHandCount >= wPlayerCount - 1) && m_cbBalanceCount < 4)
	{
		//构造数据
		CMD_S_SendCard SendCard;
		ZeroMemory(&SendCard, sizeof(SendCard));
		SendCard.cbPublic = m_cbBalanceCount;
		SendCard.wCurrentUser = INVALID_CHAIR;
		SendCard.cbSendCardCount = MAX_CENTERCOUNT;
		CopyMemory(SendCard.cbCenterCardData, m_cbCenterCardData, sizeof(BYTE)*MAX_CENTERCOUNT);
		SendCard.LAllScore = m_lAllScore;
		CopyMemory(SendCard.lTableScore, m_lTableScore, sizeof(SendCard.lTableScore));

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

		//结束游戏
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

	//盲注玩家
	for (WORD i = 1; i < m_wPlayerCount; i++)
	{
		//临时变量
		BYTE cbNextUser = (m_wDUser + i) % GAME_PLAYER;

		//获取用户
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(cbNextUser);

		//无效用户 梭哈用户过滤
		if (pIServerUserItem == NULL || m_cbPlayStatus[cbNextUser] == FALSE || m_cbShowHand[cbNextUser] == TRUE)
			continue;
		m_wCurrentUser = cbNextUser;
		break;
	}

	//构造数据
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

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));

	if (3 == SendCard.cbSendCardCount)//中心牌发牌延迟
	{
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER + 3000, 1, 0);
	}
	else
	{
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER + 2000, 1, 0);
	}

	return true;

}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_GF_TAKE_GOLD:
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_Add_TakeGold));
		if (wDataSize != sizeof(CMD_C_Add_TakeGold)) return false;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus == US_PLAYING) return true;

		//变量定义
		CMD_C_Add_TakeGold * pAddTakeGold = (CMD_C_Add_TakeGold *)pDataBuffer;
		return OnUserAddTakeGold(pIServerUserItem->GetChairID(), pAddTakeGold->lAddTakeGold);
	}
	case SUB_C_GIVE_UP:		//用户放弃
	{
		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();

		if (pUserData->cbUserStatus != US_PLAYING) return true;
		//状态判断
		ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
		if (m_cbPlayStatus[pUserData->wChairID] == FALSE) return false;
		//消息处理
		return OnUserGiveUp(pUserData->wChairID);
	}
	case SUB_C_LOOK_CARD:		//用户亮牌
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
	case SUB_C_LOOKNO_CARD:		//用户不亮牌
	{
		m_pITableFrame->KillGameTimer(IDI_GAME_NO_PLAYER);
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NO_PLAYER);
		return true;
	}
	case SUB_C_ADD_SCORE:	//用户加注	
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_AddScore));
		if (wDataSize != sizeof(CMD_C_AddScore)) return false;

		//变量定义
		CMD_C_AddScore * pAddScore = (CMD_C_AddScore *)pDataBuffer;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
		if (m_cbPlayStatus[pUserData->wChairID] == FALSE) return false;
		m_cbChipTypeID = pAddScore->ID;
		//消息处理
		return OnUserAddScore(pUserData->wChairID, pAddScore->lScore, false);
	}
	case SUB_C_PLAY_END://动画结束
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_PlatEnd));
		if (wDataSize != sizeof(CMD_C_PlatEnd)) return false;

		//变量定义
		CMD_C_PlatEnd * PlatEnd = (CMD_C_PlatEnd *)pDataBuffer;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;
		//状态判断
		ASSERT(m_cbPlayStatus[pUserData->wChairID] == TRUE);
		if (m_cbPlayStatus[pUserData->wChairID] == FALSE) return false;

		//消息处理
		return OnPlayEnd(pUserData->wChairID, PlatEnd->ID);

	}
	case SUB_C_GAMEOVER:	//游戏结束
	{
		if (!m_GameOver)
			return true;

		//用户效验
		m_GameOver = false;
		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		return true;
	}
	case SUB_C_SET_BASESCORE:  //玩家设置底注
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_SetBaseScore));
		if (wDataSize != sizeof(CMD_C_SetBaseScore)) return false;

		CMD_C_SetBaseScore * pBaseScore = (CMD_C_SetBaseScore *)pDataBuffer;

		return OnUserSetBase(pBaseScore, pIServerUserItem);
	}
	case SUB_C_DASHANG:
	{
		//效验数据
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

	//加注不能大于自身拥有的金币量
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
	//带入最大最小限制
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

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//放弃事件
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

	//重置状态
	m_cbPlayStatus[wChairID] = FALSE;
	m_cbShowHand[wChairID] = FALSE;
	m_cbGiveUpStatus[wChairID] = 1;

	//人数统计
	WORD wPlayerCount = 0;
	for (WORD i = 0; i < m_wPlayerCount; i++)
	{
		if (m_cbPlayStatus[i] == TRUE)
		{
			wPlayerCount++;
		}
	}

	//发送消息
	CMD_S_GiveUp GiveUp;
	GiveUp.wGiveUpUser = wChairID;
	GiveUp.lLost = -m_lTotalScore[wChairID];
	GiveUp.wID = m_cbGiveUpStatus[wChairID];
	GiveUp.wLookUser = INVALID_CHAIR;
	GiveUp.LAllScore = m_lAllScore;

	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GIVE_UP, &GiveUp, sizeof(GiveUp));

	//清空下注
	//m_aryChipPool[m_ChipPoolCount] += m_lTableScore[wChairID];

	//判断结束
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
		m_cbChipTypeID = CHIP_TYPE_OVER;//放弃结束比赛
		if (wChairID == m_wCurrentUser)
		{
			OnUserAddScore(wChairID, 0L, true);
		}
		m_GameOver = true;
		m_pITableFrame->SetGameTimer(IDI_GAME_NO_PLAYER, 2000, 1, 0);
	}

	return true;
}

//动画结束
bool CTableFrameSink::OnPlayEnd(WORD wChairID, BYTE PlayId)
{
	//校验用户
	ASSERT(!m_GameOver);
	if (m_GameOver) return true;
	if (m_SendCardEnd[wChairID]) return true;
	BOOL bKill = true;
	if (0 == PlayId)//发牌完毕
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

		if (bKill)//所有玩家动画完成
		{
			m_pITableFrame->KillGameTimer(IDI_TIME_SENDCARD);
			m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER, 1, 0);
		}

	}
	return true;
}

//加注事件 
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONG lScore, bool bGiveUp)
{
	//校验用户
	ASSERT(m_wCurrentUser == wChairID);
	if (m_wCurrentUser != wChairID || m_GameOver)
	{
		CString str;
		str.Format(TEXT(" OnUserAddScore : wChairID %d m_wCurrentUser %d lAddScore %d"), wChairID, m_wCurrentUser, lScore);
		m_pITableFrame->ExportInformation(str, true);
		ASSERT(0);
		return true;
	}

	// 跟注，全下容错处理
	if (m_cbChipTypeID == CHIP_TYPE_ALLIN)
	{
		lScore = m_lTurnMaxScore[wChairID];
	}

	if (m_cbChipTypeID == CHIP_TYPE_GEN)
	{
		lScore = m_lTurnLessScore;
	}

	//校验金币	
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

	//累计金币
	m_lTableScore[wChairID] += lScore;
	m_lTotalScore[wChairID] += lScore;
	m_TabelScoreBuffer[wChairID] = lScore;
	m_lTurnMaxScore[wChairID] -= lScore;
	m_lUserTakeGold[wChairID] -= lScore;

	//平衡下注
	if (m_lTableScore[wChairID] > m_lBalanceScore)
	{
		m_lBalanceScore = m_lTableScore[wChairID];
	}

	//梭哈判断
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

	//用户切换
	WORD wNextPlayer = INVALID_CHAIR;

	for (WORD i = 1; i < m_wPlayerCount; i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer = (m_wCurrentUser + i) % m_wPlayerCount;

		//继续判断
		if ((m_cbPlayStatus[wNextPlayer] == TRUE) && (m_cbShowHand[wNextPlayer] == FALSE))
		{
			break;
		}
	}
	ASSERT(wNextPlayer < m_wPlayerCount);

	//完成判断
	bool bFinishTurn = false;

	//判断非让牌情况下的一局下注完成
	if (m_wOperaCount >= m_wPlayerCount && m_lBalanceScore > 0)
	{
		WORD i = 0;
		//CString str = TEXT("梭哈");
		for (; i < m_wPlayerCount; i++)
		{
			//str.AppendFormat(TEXT("i %d m_cbPlayStatus %d m_lTableScore %d m_cbShowHand %d m_lBalanceScore %d \r\n"), i, m_cbPlayStatus[i], m_lTableScore[i], m_cbShowHand[i], m_lBalanceScore);
			//过滤未平衡 和未梭哈用户
			if ((m_cbPlayStatus[i] == TRUE) && (m_lTableScore[i] < m_lBalanceScore) && (m_cbShowHand[i] == FALSE))
				break;
		}
		if (i >= m_wPlayerCount)
		{
			bFinishTurn = true;
			//::OutputDebugString(str);
			//::OutputDebugString(TEXT("非让牌情况下 一局完成\n"));  //有BUG
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

	// 容错：当出现下注金额为0的跟注或加注消息时，当做PASS处理
	if ((m_cbChipTypeID == CHIP_TYPE_ADD || m_cbChipTypeID == CHIP_TYPE_GEN) && lScore == 0)
	{
		m_cbChipTypeID = CHIP_TYPE_PASS;
	}

	//当前是让牌流程
	if (m_cbChipTypeID == CHIP_TYPE_PASS || m_cbChipTypeID == CHIP_TYPE_GIVEUP)
	{
		if (m_cbChipTypeID == CHIP_TYPE_PASS)
		{
			m_nPassCardCount++;
		}

		CString str;
		str.Format(TEXT("CHIP_TYPE_PASS wPlayCount %d wShowCount %d m_nPassCardCount %d \n"), wPlayCount, wShowCount, m_nPassCardCount);
		::OutputDebugString(str);

		//判断完成一圈下注 让牌完成
		if (m_nPassCardCount >= (wPlayCount - wShowCount))
		{
			bFinishTurn = true;
			::OutputDebugString(TEXT("让牌 一局完成\n"));
		}
	}
	else
	{
		m_nPassCardCount = 0;
	}

	//下家没有梭哈 但是下注已经达到平衡
	//if ((wPlayCount - 1 == wShowCount) && m_lTableScore[wNextPlayer] >= m_lBalanceScore && m_lBalanceScore > 0)
	if ((wPlayCount - 1 == wShowCount) && m_lTableScore[wNextPlayer] >= m_lBalanceScore)
	{
		bFinishTurn = true;
		::OutputDebugString(TEXT("下家没有梭哈 但是下注已经达到平衡 一局完成\n"));
	}

	if (m_cbChipTypeID == CHIP_TYPE_OVER)
	{
		bFinishTurn = true;
		::OutputDebugString(TEXT("只剩一个人 一局完成\n"));
	}

	//继续加注
	if (!bFinishTurn)
	{
		//当前用户
		m_wCurrentUser = wNextPlayer;
		ASSERT(VALIDE_CHAIRID(m_wCurrentUser));
		ASSERT(m_cbPlayStatus[m_wCurrentUser] == TRUE);
		//最小值为平衡下注 -桌面下注  和 剩余金币中取小 可能梭哈
		m_lTurnLessScore = min(m_lBalanceScore - m_lTableScore[m_wCurrentUser], m_lUserMaxScore[m_wCurrentUser] - m_lTotalScore[m_wCurrentUser]);

		m_lTurnMaxScore[m_wCurrentUser] = m_lUserMaxScore[m_wCurrentUser] - m_lTotalScore[m_wCurrentUser];
		ASSERT(m_lTurnMaxScore[m_wCurrentUser] == m_lUserTakeGold[m_wCurrentUser]);
		m_lAddLessScore = (0 == m_lBalanceScore) ? (m_lCellScore*m_ISetDJPK[2] / 10) : m_lTurnLessScore + m_lCellScore;

		//构造数据
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

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_ADD_SCORE, &AddScore, sizeof(AddScore));
		m_pITableFrame->SetGameTimer(IDI_TIME_GAME, IDT_CHIPTIMER, 1, 0);
		return true;
	}

	ZeroMemory(m_TabelScoreBuffer, sizeof(m_TabelScoreBuffer));

	//平衡次数
	m_cbBalanceCount++;
	m_wOperaCount = 0;
	m_nPassCardCount = 0;

	//D家下注
	WORD wDUser = m_wDUser;
	for (BYTE i = 0; i < m_wPlayerCount; i++)
	{
		wDUser = (m_wDUser + i) % GAME_PLAYER;
		if (m_cbPlayStatus[wDUser] == TRUE && m_cbShowHand[wDUser] == FALSE) break;
	}

	//重值变量
	m_lBalanceScore = 0L;
	m_lTurnLessScore = 0L;
	m_lAddLessScore = m_lCellScore*m_ISetDJPK[2] / 10;	//最小加注

	//LONG TotalBuffer[GAME_PLAYER];	// 梭哈玩家下注
	//BYTE TotalChair[GAME_PLAYER];	// 梭哈玩家椅子号
	//memset(TotalBuffer, 0, sizeof(TotalBuffer));
	//memset(TotalChair, INVALID_CHAIR, sizeof(TotalChair));

	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (m_cbPlayStatus[i] == FALSE) continue;

		//下一轮可下注最大注码
		m_lTurnMaxScore[i] = m_lUserMaxScore[i] - m_lTotalScore[i];
		ASSERT(m_lTurnMaxScore[i] == m_lUserTakeGold[i]);

		////本轮梭哈玩家
		//if (m_cbShowHandBuffer[i] == TRUE && m_lTableScore[i]>0)
		//{
		//	TotalBuffer[i] = m_lTableScore[i];
		//	TotalChair[i] = i;
		//}
	}

	//构造数据
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

	/////////////////////////用玩家下注总注码来分奖池/////////////////////////////////////////////////
	int iSuoHaCountBuffer = 0;	// 已经梭哈玩家
	for (int iSuo = 0; iSuo < GAME_PLAYER; iSuo++)
	{
		//梭哈判断
		if (m_cbPlayStatus[iSuo] == TRUE && (m_lTotalScore[iSuo] >= m_lUserMaxScore[iSuo]))
		{
			iSuoHaCountBuffer++;
		}
	}

	m_ChipPoolCount = 0;				// 按总下注分配，每次分配都要重置
	
	//奖池重新初始化
	ZeroMemory(m_aryChipPool, sizeof(m_aryChipPool));
	ZeroMemory(m_cbHandChairCount, sizeof(m_cbHandChairCount));

	if (iSuoHaCountBuffer > 0)				// 有梭哈玩家
	{
		::OutputDebugString(TEXT("一轮加注结束 有玩家梭哈 开始进行边池分配\n"));

		LONG lSuoTotalScore[GAME_PLAYER] = {0};	// 累计下注
		CopyMemory(lSuoTotalScore, m_lTotalScore, sizeof(m_lTotalScore));

		LONG TotalBuffer[GAME_PLAYER] = {0};	// 梭哈玩家下注
		BYTE TotalChair[GAME_PLAYER] = {0};		// 梭哈玩家椅子号
		memset(TotalBuffer, 0, sizeof(TotalBuffer));
		memset(TotalChair, 0xff, sizeof(TotalChair));

		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_cbPlayStatus[i] == FALSE) continue;

			//梭哈玩家
			if (m_lTotalScore[i]>0 && m_lTotalScore[i] >= m_lUserMaxScore[i])
			{
				TotalBuffer[i] = m_lTotalScore[i];
				TotalChair[i] = i;
			}
		}

		// 注码从小到大排序
		m_GameLogic.SortValueList(TotalBuffer, TotalChair, GAME_PLAYER);

		LONG lPoolBuffer = 0;				// 记录上一个玩家下注金额

		//分边池
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			//没有梭哈或本轮没下注
			if (TotalBuffer[i] == 0 || TotalChair[i] == INVALID_CHAIR || lSuoTotalScore[TotalChair[i]] == 0)  continue;

			//如果没有下注或者梭哈的筹码跟上一家梭哈的筹码相同
			if (lPoolBuffer == TotalBuffer[i])	continue;

			// 和上个玩家下注差值
			LONG TotalBuffer2 = TotalBuffer[i] - lPoolBuffer;
			lPoolBuffer = TotalBuffer[i];

			for (int j = 0; j < GAME_PLAYER; j++)
			{
				LONG Score = lSuoTotalScore[j];

				// 本轮无下注
				if (Score <= 0) continue;

				// 玩家放弃，但有下注
				if (FALSE == m_cbPlayStatus[TotalChair[i]]) Score = 0;

				// 记录这波筹码下注的玩家
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

		//最后一个边池
		ASSERT(m_ChipPoolCount <= GAME_PLAYER);
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (lSuoTotalScore[i] > 0)
			{
				m_aryChipPool[m_ChipPoolCount] += lSuoTotalScore[i];

				// 不在游戏在状态不参与奖池分配
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

		::OutputDebugString(TEXT("一轮加注结束 有玩家梭哈 结束边池分配\n"));
	}
	else
	{
		::OutputDebugString(TEXT("一轮加注结束 无玩家梭哈 开始奖池分配\n"));

		// 没人梭哈，将筹码放入最后一个奖池
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (m_lTotalScore[i] > 0)
			{
				m_aryChipPool[0] += m_lTotalScore[i];

				// 不在游戏在状态不参与奖池分配
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

		::OutputDebugString(TEXT("一轮加注结束 无玩家梭哈 结束奖池分配\n"));
	}
		
	/////////////////////////用玩家下注总注码来分奖池/////////////////////////////////////////////////

	CopyMemory(TurnOver.aryChipPool, m_aryChipPool, sizeof(m_aryChipPool));

	int iChipPoolCount = 0;		// 奖池数量
	for (int ic = 0; ic < GAME_PLAYER; ic++)
	{
		if (m_aryChipPool[ic] > 0)
		{
			iChipPoolCount++;
		}
	}
	m_ChipPoolCount = iChipPoolCount;
	ASSERT(m_cbHandChairCount[m_ChipPoolCount - 1]>0);	//奖池中必须有玩家

	TurnOver.cbChipPoolCount = iChipPoolCount;
	TurnOver.cbBalanceCount = m_cbBalanceCount;

	// 重置每轮变量
	m_lBalanceScore = 0L;
	ZeroMemory(m_lTableScore, sizeof(m_lTableScore));
	ZeroMemory(m_cbShowHandBuffer, sizeof(m_cbShowHandBuffer));

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TURN_OVER, &TurnOver, sizeof(CMD_S_TurnOver));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TURN_OVER, &TurnOver, sizeof(CMD_S_TurnOver));

	//如果只剩一个玩家不发牌
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

//输出信息
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

// 统计该桌子上玩家的输赢
bool CTableFrameSink::WriteAllScore(LONG UserScore[], LONG Tax[])
{
	LONG ScoreBuffer = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (0 == UserScore)continue;

		// 没参与游戏玩家
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
		m_UserAllScore = GetPrivateProfileInt(TEXT("DZPKSET"), TEXT("UserWin"), 0, CGameLogic::m_strFileName2);	//获取玩家的总输赢值	
		m_UserAllScore += ScoreBuffer;

		CString BB1;
		BB1.Format(TEXT("%d"), m_UserAllScore);
		WritePrivateProfileString(TEXT("DZPKSET"), TEXT("UserWin"), BB1, CGameLogic::m_strFileName2);
	}

	return true;
}

//底注相关
bool CTableFrameSink::IsGenreGold(void)
{
	//if (IsGoldServer(m_pGameServiceOption->wServerType))   //金币场设置底注 
	//{
	//	LONG lLessCell = m_pGameServiceOption->lCellScore;
	//	LONG lMaxCell = m_pGameServiceOption->lMaxCellScore;
	//	if (lLessCell < lMaxCell)  //并且不是固定底注
	//	{
	//		return true;
	//	}
	//}

	return false;
}

void CTableFrameSink::DoUserSitDown(IServerUserItem * pIServerUserItem)
{
	WORD nChairID = pIServerUserItem->GetChairID();
	if (0xff != m_pHostChairID && m_pHostChairID == nChairID && 0 == m_lBaseScore)  //让桌主设置底注
	{
		SendSetBaseScore(pIServerUserItem, SETBASE_SET);
		m_pITableFrame->SetGameTimer(IDI_AGREE_BASESCORE, IDT_AGREE_BASESCORE, 1, m_wCurrentUser); //启动定时器
	}
	else
	{
		SendSetBaseScore(pIServerUserItem, SETBASE_WAIT); //等待桌主设置
		// 		CMD_S_StatusFree StatusFree;
		// 		ZeroMemory(&StatusFree,sizeof(StatusFree));
		// 		
		// 		StatusFree.lCellScore=0L;   //设置变量
		// 		StatusFree.lBaseScore = m_lBaseScore;
		// 		m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree)); //发送场景
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

		if (result == SETBASE_WAIT) //底注还没设定 等待
			return true;
	}

	return true;
}

bool CTableFrameSink::CheckAgreeBase()
{
	bool bFind = false;

	for (IteListPlayer ite = m_listConfirmBase.begin(); ite != m_listConfirmBase.end(); ++ite)  //让其他玩家确认同意桌主的设置
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

	if (pBaseScore->bSetOrConfirm == SETBASE_NOTAGREE)  //不同意底注
	{
		SendKickOutMsg(pIServerUserItem->GetChairID());
		return true;
	}

	m_bAgreeBase[pIServerUserItem->GetChairID()] = 1;

	if (pBaseScore->bSetOrConfirm == SETBASE_SET) //设置底注
	{
		m_lBaseScore = pBaseScore->lBaseScore;
		SCORE lMinSitScore = m_lBaseScore*GOLD_RATE;

		ASSERT(pBaseScore->bUserChairID == m_pHostChairID);
		ASSERT(m_lBaseScore != 0);
		ASSERT(lMinSitScore <= pIServerUserItem->GetUserScore());    //设置底注的玩家身上的钱要大于底注的1200倍

		CheckMoneyLimit(lMinSitScore);  //检查其他玩家的钱是否在限制内 不够的踢出

		if (CheckAgreeBase())
		{
			m_pITableFrame->SetGameTimer(IDI_AGREE_BASESCORE, IDT_AGREE_BASESCORE, 1, m_wCurrentUser); //启动定时器
		}

	}

	return true;
}

bool CTableFrameSink::OnTimerKickOut()
{
	m_pITableFrame->KillGameTimer(IDI_AGREE_BASESCORE);

	for (int i = 0; i < m_wPlayerCount; ++i)
	{
		//获取用户
		IServerUserItem *pIServerUser = m_pITableFrame->GetTableUserItem(i);
		if (NULL != pIServerUser)
		{
			if (0 == m_bAgreeBase[i])
			{
				SendKickOutMsg(i);
			}
		}
	}

	//等到玩家离开后会判断游戏是否可以开始

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

		// 身上积分
		LONG lTrusteeScore = pIServerUserItem->GetTrusteeScore();

		if (lTrusteeScore < m_pGameServiceOption->lGratuityScore)
		{
			CString str;
			str.Format(TEXT("玩家 %s 身上金币 %d 游戏开始时带入 %d 现有带入金币 %d"), pIServerUserItem->GetNickName(),
				lTrusteeScore, m_lUserMaxScore[wChairID], m_lUserTakeGold[wChairID]);
			m_pITableFrame->ExportInformation(str, false);
			return true;
		}
		
		//积分消费
		if (pIServerUserItem->WriteOtherScore(m_pGameServiceOption->lGratuityScore, 0) == false)
		{
			//SendGameMessage(pIServerUserItem, TEXT("抱歉，您的金币不足，无法发送魔法表情！"), SMT_EJECT | SMT_CHAT);
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
			SCORE lLockScore = m_lUserMaxScore[wChairID];      //下注金额

			return lLockScore;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////

