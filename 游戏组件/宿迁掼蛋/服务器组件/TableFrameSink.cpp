#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////
//静态变量;
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数;

//////////////////////////////////////////////////////////////////////////
//宏定义
#define IDI_GAEM_START					1				//游戏开始定时器;
#define IDI_TRIBUTE						2				//进贡定时器;
#define	IDI_BACK_TRIBUTE				3				//回贡定时器;
#define IDI_OUT_CARD					4				//出牌,pass定时器;
#define IDI_RESPONSE_QUIT				5				//回复请求退出;


//////////////////////////////////////////////////////////////////////////
#define FLEE							5				//逃跑扣除5倍底注;
//////////////////////////////////////////////////////////////////////////

//构造函数;
CTableFrameSink::CTableFrameSink()
{
	//变量初始;
	m_wFriendFirst = 0;
	ZeroMemory(m_cbUserTrustee, sizeof(m_cbUserTrustee));
	ZeroMemory(m_bUserOffline, sizeof(m_bUserOffline));
	ZeroMemory(&m_cbTributeCard, sizeof(m_cbTributeCard));

	//游戏变量;
	m_wBankerUser =	INVALID_CHAIR;
	m_wLastOutUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;

	//运行信息;
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


	//扑克信息;
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));

	//组件变量;
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;

	m_lBaseScore = 0L;

	srand(GetTickCount()+reinterpret_cast<DWORD_PTR>(this));
	return;
}

//析构函数;
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询;
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化;
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口;
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrame);

	//错误判断;
	if (m_pITableFrame == NULL)
	{
		CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"), TraceLevel_Exception);
		return false;
	}

	//开始模式;
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//获取参数;
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//自定规则;
	ASSERT(m_pITableFrame->GetCustomRule() != NULL);
	m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();

	return true;
}

//复位桌子;
VOID CTableFrameSink::RepositionSink()
{	
	//游戏变量;
	m_wLastOutUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	
	//运行信息;
	m_cbTurnCardCount = 0;
	m_wTurnWiner = INVALID_CHAIR;
	m_cbTurnCardType = CT_ERROR;
	m_cbMark = 0;
	ZeroMemory(m_cbRequestTimes, sizeof(m_cbRequestTimes));
	m_lstRequestUserID.clear();
	ZeroMemory(m_cbResponseStatus, sizeof(m_cbResponseStatus));
	m_bHaveSendRequestQuit = false;

	//扑克信息;
	ZeroMemory(m_cbTurnCardData,	sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbMagicCardData, sizeof(m_cbMagicCardData));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbUserCurRank, sizeof(m_cbUserCurRank));

}

//游戏开始;
bool CTableFrameSink::OnEventGameStart()
{
	//数据重置;
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

	//设置游戏状态;
	m_pITableFrame->SetGameStatus(GS_WK_SEND_CARD);
	
	m_bGameEnd = false;
	m_cbMark	= 0;


	BYTE cbRandCard[FULL_COUNT] = { 0 };
	m_GameLogic.RandCardList(cbRandCard, CountArray(cbRandCard));

	//分发扑克;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		// 重置回贡对象;
		m_aryRetTributeTarget[i] = 255;
		// 重置手牌数量;
		m_cbHandCardCount[i] = 27;
		CopyMemory(&m_cbHandCardData[i], &cbRandCard[i*27], sizeof(BYTE)*27);
	}

	//设置级数;
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

	//发送扑克信息;
	for (WORD i=0; i<GAME_PLAYER; ++i)
	{
		CopyMemory(SendCard.cbCardData, m_cbHandCardData[i], 27);
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	//设置开始定时器;
	m_pITableFrame->SetGameTimer(IDI_GAEM_START, m_pGameCustomRule->cbTimeStartGame * 1000, 1, 0);

	return true;
}

//游戏结束;
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//删除定时器;
	m_pITableFrame->KillGameTimer(IDI_GAEM_START);
	m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
	m_pITableFrame->KillGameTimer(IDI_BACK_TRIBUTE);
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
	m_pITableFrame->KillGameTimer(IDI_RESPONSE_QUIT);

	switch(cbReason)
	{
		//正常游戏;
	case GER_NORMAL:
	{
		//变量重置;
		m_bGameEnd = true;

		//定义变量;
		CMD_S_GameEnd stGameEnd;
		ZeroMemory(&stGameEnd, sizeof(stGameEnd));
		stGameEnd.bTongGuan = false;
		stGameEnd.cbGameEndType = GER_NORMAL;
		//剩余扑克;
		BYTE bCardPos = 0;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			stGameEnd.cbCardCount[i] = m_cbHandCardCount[i];
			CopyMemory(&stGameEnd.cbCardData[bCardPos], m_cbHandCardData[i], m_cbHandCardCount[i] * sizeof(BYTE));
			bCardPos += m_cbHandCardCount[i];
		}
		//变量定义;
		BYTE cbUpgrading = 0;
		bool bHavePlayerMoneyEnough = false;
		BYTE cbFirst = m_cbRanking[0], cbSecond = (m_cbRanking[0] + 1) % GAME_PLAYER;
		//状态修改;
		m_cbPayTribute[cbFirst] = 1;
		//统计积分;
		LONG lScore = 0L;
		//双下处理;
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
		//单下处理;
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

		//拷贝等级;
		CopyMemory(&stGameEnd.stSeries, &m_stSeries, sizeof(m_stSeries));
		CopyMemory(stGameEnd.cbRanking, m_cbRanking, sizeof(m_cbRanking));

		//房间底分;
		LONG lCellScore = m_pITableFrame->GetCellScore();
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(cbSecond);
		SCORE lUserScore = pServerUserItem->GetUserScore();
		stGameEnd.lGameScore[cbSecond] = 0L - min(lScore*lCellScore, lUserScore);
		pServerUserItem = m_pITableFrame->GetTableUserItem((cbSecond + 2) % GAME_PLAYER);
		lUserScore = pServerUserItem->GetUserScore();
		stGameEnd.lGameScore[(cbSecond + 2) % GAME_PLAYER] = 0L - min(lScore*lCellScore, lUserScore);
		stGameEnd.lGameScore[cbFirst] = abs(stGameEnd.lGameScore[cbSecond] + stGameEnd.lGameScore[(cbSecond + 2) % GAME_PLAYER]) / 2;
		stGameEnd.lGameScore[(cbFirst + 2) % GAME_PLAYER] = stGameEnd.lGameScore[cbFirst];

		//写分;
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

		//判断玩家金币是否符合设置的金币底注;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			SCORE lUserScore = pServerUserItem->GetUserScore();
			//如果玩家身上的金币扣除后，不满足;
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

		//发送信息;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &stGameEnd, sizeof(stGameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &stGameEnd, sizeof(stGameEnd));

		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		return true;
	}
	//游戏解散;
	case GER_DISMISS:
		{
			m_bGameEnd = true;

			//构造数据;
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.cbGameEndType = GER_DISMISS;

			//剩余扑克;
			BYTE bCardPos = 0;
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				GameEnd.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[bCardPos], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));
				bCardPos += m_cbHandCardCount[i];
			}
			//发送信息;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			//结束游戏;
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			//设置游戏状态;
			m_pITableFrame->SetGameStatus(GS_WK_FREE);
			//重置数据;
			m_wBankerUser = INVALID_CHAIR;
			m_stSeries.cbCurrentSeries = 2;
			m_stSeries.cbOtherSeries = 2;
			m_stSeries.cbOurSeries = 2;
			m_cbGameStatus = 0;
			ZeroMemory(m_cbRanking, sizeof(m_cbRanking));
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			return true;
		}
	//用户强退;
	case GER_USER_LEAVE:
	case GER_NETWORK_ERROR:
		{
			//修改变量;
			m_bGameEnd = true;

			//定义变量;
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.cbGameEndType = GER_USER_LEAVE;

			CopyMemory(&GameEnd.stSeries, &m_stSeries, sizeof(m_stSeries));
			CopyMemory(&GameEnd.cbRanking, &m_cbRanking, sizeof(m_cbRanking));
			//剩余扑克;
			BYTE bCardPos = 0;
			for (WORD i=0; i<m_wPlayerCount; i++)
			{
				GameEnd.cbCardCount[i] = m_cbHandCardCount[i];
				CopyMemory(&GameEnd.cbCardData[bCardPos], m_cbHandCardData[i], m_cbHandCardCount[i]*sizeof(BYTE));
				bCardPos += m_cbHandCardCount[i];
			}
			//房间底分;
			LONG lCellScore = m_pITableFrame->GetCellScore();
			//玩家得分;
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem( wChairID );
			SCORE lUserScore = pServerUserItem->GetUserScore();	
			GameEnd.lGameScore[wChairID] = 0L - abs(__min(FLEE*lCellScore, lUserScore));

			//发送信息;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		
			tagScoreInfo ScoreInfo;
			ZeroMemory(&ScoreInfo, sizeof(tagScoreInfo));
			ScoreInfo.cbType = SCORE_TYPE_FLEE;
			ScoreInfo.lScore = GameEnd.lGameScore[wChairID];

			//修改积分;
			m_pITableFrame->WriteUserScore(wChairID, ScoreInfo);

			//游戏结束;
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
			//重置数据;
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

//发送场景;
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	//效验结果;
	//ASSERT(FALSE);
    switch(cbGameStatus)
	{
	//空闲状态;
	case GS_WK_FREE:
		{
			//构造参数;
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree, sizeof(StatusFree));

			StatusFree.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			StatusFree.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			StatusFree.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusFree.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			m_lBaseScore = m_pITableFrame->GetCellScore();

			StatusFree.lBaseScore = m_lBaseScore;

			//发送场景;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusFree, sizeof(StatusFree));
		}
	//发牌状态;
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
			//发送场景;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &SendCard, sizeof(SendCard));
		}
	//进贡状态;
	case GS_WK_TRIBUTE:
		{
			//构造参数;
			CMD_S_StatusTribute Tribute;
			ZeroMemory(&Tribute, sizeof(Tribute));

			Tribute.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			Tribute.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			Tribute.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			Tribute.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			Tribute.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			//设置变量;
			Tribute.lBaseScore = m_lBaseScore;
			Tribute.stSeries = m_stSeries;
			Tribute.bLastRoundWin = m_cbRanking[0]%2 == 0;

			CopyMemory(Tribute.cbCardData, m_cbHandCardData[wChairID], sizeof(BYTE)*m_cbHandCardCount[wChairID]);
			CopyMemory(Tribute.cbCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
			CopyMemory(Tribute.cbUserTrustee, m_cbUserTrustee, sizeof(m_cbUserTrustee));
			//发送场景;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &Tribute, sizeof(Tribute));
		}	
	//游戏状态;
	case GS_WK_PLAYING:		
		{
			//构造数据;
			CMD_S_StatusPlay StatusPlay;
			ZeroMemory(&StatusPlay, sizeof(StatusPlay));

			StatusPlay.cbTimePayTribute = m_pGameCustomRule->cbTimePayTribute;
			StatusPlay.cbTimeRetTribute = m_pGameCustomRule->cbTimeRetTribute;
			StatusPlay.cbTimeHeadOutCard = m_pGameCustomRule->cbTimeHeadOutCard;
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
			StatusPlay.cbTimeTrustee = m_pGameCustomRule->cbTimeTrustee;

			//设置变量;
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
			//发送场景;
			return m_pITableFrame->SendGameScene(pIServerUserItem, &StatusPlay, sizeof(StatusPlay));
		}
	}
	
	return false;
}


//定时器事件;
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{	
	//游戏开始消息;
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
				//发送不同意;
				OnDoResponseQuit(i, (WORD)wBindParam, 2);
			}
		}
		return true;
	}

	return false;
}

//游戏消息;
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch(wSubCmdID)
	{
		//用户出牌;
	case SUB_C_OUT_CARD:		
		{
			//变量定义;
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_C_OutCard)-sizeof(pOutCard->cbCardData);

			//效验数据;
			ASSERT(wDataSize>=wHeadSize);
			if (wDataSize<wHeadSize) return false;
			ASSERT(wDataSize==(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0])));
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;

	
			//用户效验;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//消息处理;
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
		//用户不出;
	case SUB_C_PASS_CARD:
		{
			//用户效验;
			tagUserInfo *pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//消息处理;
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
		    //效验数据;
			ASSERT(wDataSize==sizeof(CMD_C_PayTribute));
			if (wDataSize!=sizeof(CMD_C_PayTribute)) return false;
			
			//用户效验;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;
		
			//变量定义;
			CMD_C_PayTribute * pPayTribute=(CMD_C_PayTribute *)pDataBuffer;
         
			bool bSuccess = false;
			try
			{
				//消息处理;
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
			//效验数据;
			ASSERT(wDataSize == sizeof(CMD_C_PayTribute));
			if (wDataSize != sizeof(CMD_C_PayTribute)) return false;

			//用户效验;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			//变量定义;
			CMD_C_PayTribute * pPayTribute = (CMD_C_PayTribute *)pDataBuffer;

			bool bSuccess = false;
			try
			{
				//消息处理;
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
		//用户托管;
	case SUB_C_USER_TRUST:
		{
			//校验数据;
			ASSERT(wDataSize==sizeof(CMD_C_UserTrust));
			if (wDataSize!=sizeof(CMD_C_UserTrust)) return false;
			
			//用户校验;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//消息处理;
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
			//校验数据;
			ASSERT(wDataSize==sizeof(CMD_C_RequestQuit));
			if (wDataSize!=sizeof(CMD_C_RequestQuit))	return false;
			
			//用户校验;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;
			
			bool bSuccess = false;
			try
			{
				//消息处理;
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
			//校验数据;
			ASSERT(wDataSize==sizeof(CMD_C_ResposeQuit));
			if (wDataSize!=sizeof(CMD_C_ResposeQuit))	return false;

			//用户校验;
			tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus != US_PLAYING) return true;

			bool bSuccess = false;
			try
			{
				//消息处理;
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

//框架消息处理;
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户出牌;
bool CTableFrameSink::OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
	//效验状态;
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
	
	//变幻扑克
	BYTE cbMagicCardData[MAX_COUNT] = {0};
	m_GameLogic.MagicCardData(cbCardData, cbCardCount, cbMagicCardData);

	//获取类型
	BYTE cbCardType = m_GameLogic.GetCardType(cbMagicCardData, cbCardCount);

	//类型判断
	if (cbCardType == CT_ERROR)
	{
		ASSERT(FALSE);
		return false;
	}

	//出牌判断
	if (m_cbTurnCardCount != 0)
	{
		//对比扑克
		if (m_GameLogic.CompareCard(m_cbMagicCardData, cbMagicCardData, m_cbTurnCardCount, cbCardCount) == false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
	{
		ASSERT(FALSE);
		return false;
	}
	m_cbHandCardCount[wChairID] -= cbCardCount;

	//删除定时器;
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);

	//出牌记录;
	m_cbTurnCardType = cbCardType;
	m_cbTurnCardCount = cbCardCount;
	CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);
	CopyMemory(m_cbMagicCardData, cbMagicCardData, sizeof(BYTE)*cbCardCount);

	//删除友方优先;
	if (m_wFriendFirst != 0)
	{
		m_wFriendFirst = 0;
	}

	//切换用户;
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
 
 		//友方优先;
 		if (m_wCurrentUser != INVALID_CHAIR)
 		{
 			m_wFriendFirst = (wChairID+2)%GAME_PLAYER << 8 | (GAME_PLAYER - m_cbMark);
 		}
 
 		//发送完成牌消息;
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

	//构造数据;
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
	//记录上次出牌玩家ID;
	m_wLastOutUser = wChairID;

	//发送对家扑克;
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

	//结束判断;
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

//用户放弃;
bool CTableFrameSink::OnSubPassCard(WORD wChairID)
{
	//效验状态;
	if (m_pITableFrame->GetGameStatus()!=GS_WK_PLAYING) return true;
	if ((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0)) return true;

	//删除定时器;
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);

	//设置变量;
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
	//友方优先;
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

	//发送数据;
	PassCard.wPassUser=wChairID;
	PassCard.wCurrentUser=m_wCurrentUser;
	PassCard.bNewTurn=(m_cbTurnCardCount==0)?TRUE:FALSE;
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

	//设置定时器;
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

//用户进贡;
bool CTableFrameSink::OnSubPayTribute(WORD wChairID, const void * pDataBuffer)
{
	ASSERT(1 == m_cbPayTributeStatus);

	//获取数据;
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

			//全部进贡完成;
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

					// 保存回贡对象;
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

					// 保存回贡对象;
					m_aryRetTributeTarget[wFirst] = wJinGongID2;
					m_aryRetTributeTarget[wSecond] = wJinGongID1;
				}
				m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
				m_pITableFrame->SetGameTimer(IDI_BACK_TRIBUTE, m_pGameCustomRule->cbTimeRetTribute * 1000, 1, 0);
				m_cbPayTributeStatus = 2;


				//发送数据;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
			}
			else
			{
				CMD_S_PayTributeCard TributeCard;
				TributeCard.wChairID = wChairID;
				TributeCard.cbCardData = cbTributeCard;
				//发送数据;
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

			// 保存回贡对象;
			m_aryRetTributeTarget[wFirst] = wChairID;

			//发送数据;
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

			// 保存回贡对象;
			m_aryRetTributeTarget[wFirst] = wChairID;

			//发送数据;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_RESULT, &PayTribute, sizeof(PayTribute));

			m_pITableFrame->KillGameTimer(IDI_TRIBUTE);
			m_pITableFrame->SetGameTimer(IDI_BACK_TRIBUTE, m_pGameCustomRule->cbTimeRetTribute * 1000, 1, 0);
			m_cbPayTributeStatus = 2;
		}
	}

	return true;
}

//用户回贡;
bool CTableFrameSink::OnSubReturnTribute(WORD wChairID, const void * pDataBuffer)
{
	ASSERT(2 == m_cbPayTributeStatus);

	//获取数据;
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
			//发送数据;
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
			//发送数据;
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
			//发送数据;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_RETURN_TRIBUTE, &stTribute, sizeof(stTribute));
		}
	}

	if (m_wCurrentUser != INVALID_CHAIR)
	{
		m_pITableFrame->KillGameTimer(IDI_BACK_TRIBUTE);
		//设置游戏状态;
		m_pITableFrame->SetGameStatus(GS_WK_PLAYING);

		//通知玩家开始出牌;
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

		//发送所有手牌;
		CMD_S_AllCard ALL_CARD;
		ZeroMemory(&ALL_CARD, sizeof(ALL_CARD));
		CopyMemory(ALL_CARD.cbHandCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
		CopyMemory(ALL_CARD.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
		//是机器人发送所有牌;
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

//用户托管;
bool CTableFrameSink::OnSubTrustee(WORD wChairID, const void* pDataBuffer)
{
	//变量定义;
	CMD_C_UserTrust *pUserTrust = (CMD_C_UserTrust*) pDataBuffer;
	VERIFY_RETURN_FALSE(wChairID<m_wPlayerCount);
	//状态校验;
	if (m_cbHandCardCount[wChairID] == 0) return true;

	//参数赋值;
	m_cbUserTrustee[wChairID] = pUserTrust->cbUserTrustee;
	if (wChairID == m_wCurrentUser && pUserTrust->cbUserTrustee>0)
	{
		OnOutCardTimeOut();
	}

	//发送消息;
	CMD_S_User_Trust  UserTrust;
	UserTrust.cbUserTrustee = pUserTrust->cbUserTrustee;
	UserTrust.wChairID = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_TRUST, &UserTrust, sizeof(UserTrust));
	return true;
}

//用户断线;
bool CTableFrameSink::OnActionUserOffLine( WORD wChairID, IServerUserItem * pIServerUserItem )
{
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//掉线时，转换成系统托管;
	if (m_cbUserTrustee[wChairID]!=2 && !m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = true;
	}
	return true;
}

//用户重入;
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return false;

	//玩家重连进来;
	if (m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = false;
	}

	return true;
}

//用户坐下;
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//用户起立;
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	return true;
}

//游戏开始;
bool CTableFrameSink::OnGameStart()
{
	//变量初始;
	m_wFriendFirst = 0;

	//设置用户;
	if(m_wBankerUser == INVALID_CHAIR)
	{
		m_wCurrentUser = rand() % m_wPlayerCount;
		m_wBankerUser = m_wCurrentUser;
	}
	else
	{
		m_wCurrentUser = m_wBankerUser;
	}

	//排序扑克;
	for(WORD i=0; i<m_wPlayerCount; i++)
	{
		m_GameLogic.SortCardList(m_cbHandCardData[i] ,m_cbHandCardCount[i], ST_ORDER);
	}

	//抗贡判断;
	if(m_cbGameStatus ==	SHUANG_XIA)
	{
		m_wCurrentUser = INVALID_CHAIR;
		BYTE  bFirst = m_cbRanking[0];
		//拥有双大王，抗贡;
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
		//拥有双大王，抗贡;
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
		//拥有双大王，抗贡;
		if(m_cbHandCardData[bFourth][0]==0x4F&&m_cbHandCardData[bFourth][1]==0x4F)
		{	
			ZeroMemory(m_cbPayTribute, sizeof(m_cbPayTribute));
			m_wCurrentUser = m_cbRanking[0];
		}
	}

	CMD_S_PayTributeNotify PayTribute;
	//处理进贡;
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
	//处理抗贡;
	else if(m_cbGameStatus != 0)
	{
		PayTribute.cbPayStatus = enKang_Gong;
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_NOTIFY, &PayTribute, sizeof(PayTribute));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_TRIBUTE_NOTIFY, &PayTribute, sizeof(PayTribute));
	}

	//设置游戏状态;
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
		//构造所有手牌信息;
		CMD_S_AllCard ALL_CARD;
		ZeroMemory(&ALL_CARD, sizeof(ALL_CARD));
		//拷贝所有牌;
		CopyMemory(ALL_CARD.cbHandCardCount, m_cbHandCardCount, sizeof(m_cbHandCardCount));
		CopyMemory(ALL_CARD.cbHandCardData, m_cbHandCardData, sizeof(m_cbHandCardData));
		//是机器人发送所有牌;
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

//进(回)贡超时;
bool CTableFrameSink::OnTributeTimeOut( BYTE cbTributeType )
{
	//超时用户;
	WORD wOverTimeUser[2] = {INVALID_CHAIR, INVALID_CHAIR};
	if (m_pITableFrame->GetGameStatus() == GS_WK_TRIBUTE)
	{	
		WORD wFirst = m_cbRanking[0], wSecond=m_cbRanking[1];
		//进贡;
		if (cbTributeType == 0)
		{
			if (m_cbGameStatus == SHUANG_XIA)
			{
				WORD wThree=(wFirst+1)%GAME_PLAYER, wFour=(wFirst+3)%GAME_PLAYER;
				//进贡;
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
				//进贡;
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
				//进贡;
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
		//回贡;
		else if (cbTributeType == 1)
		{
			if (m_cbGameStatus == SHUANG_XIA)
			{
				WORD wThree=(wFirst+1)%GS_WK_PLAYING, wFour=(wFirst+3)%GAME_PLAYER;
				//回贡;
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
				//回贡;
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
				//回贡;
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

		//状态设置,并发送;
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
//出牌、PASS超时;
bool CTableFrameSink::OnOutCardTimeOut()
{
	//超时用户;
	WORD wOverTimeUser = INVALID_CHAIR;
	if (m_pITableFrame->GetGameStatus() == GS_WK_PLAYING)
	{
		//用户校验,机器人超时服务器代打，暂时找不出原因;
		//ASSERT(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsAndroidUser() == false);
		//if(m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsAndroidUser()) return true;
		
		m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
		wOverTimeUser = m_wCurrentUser;
		
		//首先出牌;
		if (m_cbTurnCardCount == 0)
		{
			ASSERT(m_cbHandCardCount[m_wCurrentUser] > 0);
			BYTE cbLastCardIndex = m_cbHandCardCount[m_wCurrentUser] - 1;
			ASSERT(cbLastCardIndex < 27);
			BYTE cbCardData = m_cbHandCardData[m_wCurrentUser][cbLastCardIndex];
			ASSERT(cbCardData != 0);
			OnSubOutCard(m_wCurrentUser, &cbCardData, 1);
		}
		//压牌;
		else
		{
			OnSubPassCard(m_wCurrentUser);
		}

		//状态设置,并发送;
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
	//强制退出;
	if (pRequest->cbRequestQuitType == 2)
	{
		//结束游戏;
		OnEventGameConclude(pRequest->wChairID, NULL, GER_USER_LEAVE);

		//关闭客户端;
		CMD_S_ForceQuit stQuitNot;
		ZeroMemory(&stQuitNot, sizeof(stQuitNot));
		stQuitNot.wChairID = pRequest->wChairID;
		m_pITableFrame->SendTableData(pRequest->wChairID, SUB_S_FORCE_QUIT, &stQuitNot, sizeof(stQuitNot));
		return true;
	}
	//请求次数大于等于3次，直接返回;
	if (m_cbRequestTimes[pRequest->wChairID] >= REQUEST_QUIT_COUNT)
	{
		return true;
	}
	//将申请人ID，加入列表中;
	m_lstRequestUserID.push_back(pRequest->wChairID);
	//发送请求;
	SendRequestQuitMsg();
	return true;
}

bool CTableFrameSink::OnSubResponseQuit( WORD wMeChiarID, const void * pDatabuffer )
{
	//变量定义;
	CMD_C_ResposeQuit *pResponseQuit = (CMD_C_ResposeQuit *)pDatabuffer;
	//处理回复请求;
	return	OnDoResponseQuit(wMeChiarID, pResponseQuit->wChairID, pResponseQuit->cbResponseQuitType);
}

//发送请求退出消息;
void CTableFrameSink::SendRequestQuitMsg()
{
	//如果有玩家申请退出并且前面没有发送过;
	if (!m_lstRequestUserID.empty() && !m_bHaveSendRequestQuit)
	{
		WORD wRequestQuitUser = m_lstRequestUserID.front();
		CMD_S_RequestQuit stRequestQuitNot;
		ZeroMemory(&stRequestQuitNot, sizeof(stRequestQuitNot));
		stRequestQuitNot.wChairID = wRequestQuitUser;
		//发送请求;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if ((i!=wRequestQuitUser)&&(pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()!=US_OFFLINE))
			{
				m_pITableFrame->SendTableData(i, SUB_S_REQUEST_QUIT, &stRequestQuitNot, sizeof(stRequestQuitNot));
			}
		}
		//记录次数，重置回复状态;
		m_bHaveSendRequestQuit = true;
		m_cbRequestTimes[wRequestQuitUser]++;
		ZeroMemory(m_cbResponseStatus, sizeof(m_cbResponseStatus));
		m_pITableFrame->SetGameTimer(IDI_RESPONSE_QUIT, 6000, 1, wRequestQuitUser);
		m_lstRequestUserID.pop_front();
	}
}

//回复请求处理;
bool CTableFrameSink::OnDoResponseQuit( WORD wMeChairID, WORD wRequestID, BYTE cbResponseQuitType )
{
	//回复过，直接返回;
	if (m_cbResponseStatus[wMeChairID] != 0) return true;

	m_cbResponseStatus[wMeChairID] = cbResponseQuitType;
	
	BYTE cbResponseCount=0, cbAgreeQuitCount=0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		//回复过;
		if (m_cbResponseStatus[i] != 0)
		{
			cbResponseCount++;
			//同意退出;
			if (m_cbResponseStatus[i] == 1)
			{
				cbAgreeQuitCount++;
			}
		}
	}

	//三人全部回复;
	if (cbResponseCount >= 3)
	{
		//删除定时器;
		m_pITableFrame->KillGameTimer(IDI_RESPONSE_QUIT);
		m_bHaveSendRequestQuit = false;

		CMD_S_ResponseQuit stResponseQuitNot;
		ZeroMemory(&stResponseQuitNot, sizeof(stResponseQuitNot));
		stResponseQuitNot.wChairID = wRequestID;
		m_pITableFrame->SendTableData(stResponseQuitNot.wChairID, SUB_S_RESPONSE_QUIT, &stResponseQuitNot, sizeof(stResponseQuitNot));
		//同意退出;
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
			//继续检测其他玩家请求;
			SendRequestQuitMsg();
		}
	}

	return true;
}

//重置私人场结束信息
void CTableFrameSink::ResetPrivateEndInfo()
{
	ZeroMemory(&m_PrivateEndInfo, sizeof(CMD_S_Private_End_Info));
}

//获得私人场结束信息流
void CTableFrameSink::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		// 总积分
		IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pServerUserItem)
		{
			m_PrivateEndInfo.lAllScore[i] = pServerUserItem->GetUserScore();
		}
	}

	m_PrivateEndInfo.StreamValue(kDataStream, bSend);
}

//判断私人场是否结束
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