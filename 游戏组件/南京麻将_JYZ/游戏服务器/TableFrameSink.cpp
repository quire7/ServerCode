#include "StdAfx.h"
#include "GameLottery.h"
#include "TableFrameSink.h"
#include <strsafe.h>

#define BOOL_TO_STRING(val) ((val)?"true":"false")

#define IS_OPEN_PRINT_LOG	FALSE		// 必须用大写 TRUE FALSE

#define KEEP_LEFT_CARD_COUNT	0		// 流牌数量;

///////////////////////////////////////////////////////////////////////////////////////
//静态变量
BYTE				CTableFrameSink::m_cbOpenGameLottery = 0;				//游戏抽奖
LONG				CTableFrameSink::m_lMaxPlazaExp = 0;					//最大平台经验
CString				CTableFrameSink::m_strLucyDrawCfg;						//抽奖配置
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//游戏人数
const tagGameServiceOption *CTableFrameSink::m_pGameServiceOption = NULL;	//房间设置

///////////////////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrameSink::CTableFrameSink()
{
	m_lBaseScore = 0L;
	m_wHostChairID = INVALID_CHAIR;
	ZeroMemory(m_bAgreeBaseScore, sizeof(m_bAgreeBaseScore));
	m_listPlayers.clear();
	m_listAgreeBaseScore.clear();
	
	//重置变量(全部重置)
	ResetAllData(true);
	
	//组件变量
	m_pITableFrame = NULL;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID &Guid, DWORD dwQueryVer)
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

	//开始模式
	m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

	//游戏配置
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

	srand(GetTickCount() | (m_pITableFrame->GetTableID() << 8));

	return true;
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
	//重置变量(部分重置)
	ResetAllData(false);
}

//重置变量
void CTableFrameSink::ResetAllData(bool bAll)
{
	if (bAll)
	{
		m_wEastUser = INVALID_CHAIR;
		m_wBankerUser = INVALID_CHAIR;
		m_bLastRoundBiXiaHu = false;
	}

	m_bGameStart = false;

	//游戏变量
	m_wSiceCount = MAKEWORD(0, 0);
	ZeroMemory(m_lOutCardScore, sizeof(m_lOutCardScore));
	ZeroMemory(m_lGangScore, sizeof(m_lGangScore));
	ZeroMemory(m_cbCardIndex, sizeof(m_cbCardIndex));
	ZeroMemory(m_cbHuaPaiCount, sizeof(m_cbHuaPaiCount));
	ZeroMemory(m_cbHuaPaiData, sizeof(m_cbHuaPaiData));
	ZeroMemory(m_bUserTrust, sizeof(m_bUserTrust));
	ZeroMemory(m_bUserOffline, sizeof(m_bUserOffline));

	//出牌信息
	m_cbOutCardData = 0;
	m_cbOutCardCount = 0;
	m_wOutCardUser = INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard, sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount, sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbDiscardRecordCount, sizeof(m_cbDiscardRecordCount));
	ZeroMemory(m_cbDiscardRecordCard, sizeof(m_cbDiscardRecordCard));
	ZeroMemory(m_cbDiscardIndexCount, sizeof(m_cbDiscardIndexCount));

	//发牌信息
	m_bSendCardStatus = false;
	m_wSendCardUser = INVALID_CHAIR;
	m_wBuHuaUser = INVALID_CHAIR;
	m_cbSendCardData = 0;
	m_cbSendCardCount = 0;
	m_cbLeftCardCount = 0;
	ZeroMemory(m_cbRepertoryCard, sizeof(m_cbRepertoryCard));

	//运行变量
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

	//用户状态
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

	//组合牌
	ZeroMemory(m_WeaveItemArray, sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

	//结束信息
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

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_MJ_DICE);

	m_bGameStart = true;

	//获取房间底注
	m_lBaseScore = m_pITableFrame->GetCellScore();
	m_pITableFrame->KillGameTimer(IDI_BASESCORE);

	//设置变量
	if (m_wBankerUser == INVALID_CHAIR)
	{
		m_wBankerUser = rand() % GAME_PLAYER;
		m_wEastUser = m_wBankerUser;
	}
	m_wCurrentUser = m_wBankerUser;

	//发送掷骰子消息
	CMD_S_ShootDice stShootDice;
	ZeroMemory(&stShootDice, sizeof(stShootDice));
	stShootDice.bCurBiXiaHu = m_bLastRoundBiXiaHu;
	stShootDice.wBankerUser = m_wBankerUser;
	stShootDice.wEastUser = m_wEastUser;

	//发送掷骰子消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_SHOOT_DICE, &stShootDice, sizeof(stShootDice));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SHOOT_DICE, &stShootDice, sizeof(stShootDice));

	//启动定时器
	m_pITableFrame->SetGameTimer(IDI_SHOOT_DICE, IDT_SHOOT_DICE * 1000, 1, 0);
	return true;
}

/************************************************************************/
/* 我和客户确认了，举例啊！ (门清20花+（软花+硬花）*2+跑20花)÷2
   比下胡算法：门清20花+（软花+硬花）*2+跑20花               20161012        */
/************************************************************************/
//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{

#if IS_OPEN_PRINT_LOG
	MyLog("OnEventGameConclude(wChairID = %d, cbReason = %d) begin...", wChairID, cbReason);
#endif

	//删除定时器
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
	case GER_DISMISS:		//游戏解散
	{
		//变量定义
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));

		//设置变量
		GameEnd.wProvideUser = INVALID_CHAIR;
		GameEnd.cbGameEndType = GER_DISMISS;

		//拷贝牌
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

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		// 游戏录像
		addGameRecordAction(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		/**************结算积分统计判断********************************************/
#if IS_OPEN_PRINT_LOG
		LONG lEndScore = 0;

		//最终成绩
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

		//统计积分
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

		//写入积分
		DataStream kDataStream;
		m_kGameRecord.StreamValue(kDataStream, true);
		m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray), kDataStream);

		/********************************私人场统计****************************************/
		//统计是否是自摸
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			// 赢家
			if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
			{
				// 自摸
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbZiMoCout[i]++;
				}
				else // 捉炮
				{
					m_PrivateEndInfo.cbZhuoPaoCout[i]++;
				}

				// 胜利次数
				m_PrivateEndInfo.cbWinCout[i]++;
			}
			else // 输家
			{
				// 点炮
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbDianPaoCout[i]++;
				}

				// 失败次数
				m_PrivateEndInfo.cbLoseCout[i]++;
			}

			// 最高得分
			if (m_PrivateEndInfo.lMaxScore[i] <= GameEnd.lGameScore[i])
			{
				m_PrivateEndInfo.lMaxScore[i] = GameEnd.lGameScore[i];
			}

			// 总积分;
			m_PrivateEndInfo.lAllScore[i] += GameEnd.lGameScore[i];
			m_PrivateEndInfo.lBaoZiScore[i] += GameEnd.lBaoZiScore[i];
		}
		/********************************私人场统计****************************************/

		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
		return true;
	}
	case GER_BAOZI:				// 自摸胡包子;
	{
		m_bHaveBaozi = true;

		//变量定义
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));

		//设置变量
		GameEnd.cbGameEndType = 0;
		GameEnd.wBankerUser = m_wBankerUser;
		GameEnd.bHaveBiXiaHu = m_bLastRoundBiXiaHu;
		GameEnd.lCellScore = m_pITableFrame->GetCellScore();

		// 包子统计
		GameEnd.wProvideUser = GetBaoziProvideUser(wChairID);

		LONG lBiXiaHuMultiple = (m_bLastRoundBiXiaHu ? 1L : 2L);
		GameEnd.lBaoZiScore[wChairID] = BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// 胡包子人得分;
		GameEnd.lBaoZiScore[GameEnd.wProvideUser] = 0 - BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// 点包子人得分;

		//结束信息
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
			GameEnd.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
			CopyMemory(GameEnd.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(tagWeaveItem)*MAX_WEAVE);
			CopyMemory(&GameEnd.stHuPaiResult[i], &m_HuPaiResult[i], sizeof(tagHuPaiResult));
			GameEnd.cbHuaPaiCount[i][0] = m_cbHuaPaiCount[i][0];
			GameEnd.cbHuaPaiCount[i][1] = m_cbHuaPaiCount[i][1];

			// 杠分;
			GameEnd.lGameScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];//游戏积分
			GameEnd.lGangScore[i] = m_lGangScore[i];												//杠分;	
			GameEnd.lOutScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1];					//出牌分;
		}

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		// 游戏录像
		addGameRecordAction(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		m_bLastRoundBiXiaHu = IsBiXiaHu();

		//定义变量
		LONG lUserGameScore[GAME_PLAYER] = { 0L };
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			lUserGameScore[i] += (m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i]);
		}

		//金币场，玩家输钱判断
		bool bMoneyNotEnough = false;
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			LONG lUserScore = (LONG)pServerUserItem->GetUserScore();
			if (lUserGameScore[i] < 0)
			{
				//如果不够输
				if (abs(lUserGameScore[i]) > lUserScore)
				{
					lUserGameScore[i] = 0L - lUserScore;
					bMoneyNotEnough = true;
				}
			}
		}

		tagScoreInfo ScoreInfoArray[GAME_PLAYER];
		ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

		//统计积分
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (NULL == m_pITableFrame->GetTableUserItem(i)) continue;

			ScoreInfoArray[i].lScore = lUserGameScore[i];
			ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;
		}

		//写入积分
		DataStream kDataStream;
		m_kGameRecord.StreamValue(kDataStream, true);
		m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray), kDataStream);

		/********************************私人场统计****************************************/
		//统计是否是自摸
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			// 赢家
			if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
			{
				// 自摸
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbZiMoCout[i]++;
				}
				else // 捉炮
				{
					m_PrivateEndInfo.cbZhuoPaoCout[i]++;
				}

				// 胜利次数
				m_PrivateEndInfo.cbWinCout[i]++;
			}
			else // 输家
			{
				// 点炮
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbDianPaoCout[i]++;
				}

				// 失败次数
				m_PrivateEndInfo.cbLoseCout[i]++;
			}

			// 最高得分
			if (m_PrivateEndInfo.lMaxScore[i] <= GameEnd.lGameScore[i])
			{
				m_PrivateEndInfo.lMaxScore[i] = GameEnd.lGameScore[i];
			}

			// 总积分;
			m_PrivateEndInfo.lAllScore[i] += GameEnd.lGameScore[i];
			m_PrivateEndInfo.lBaoZiScore[i] += GameEnd.lBaoZiScore[i];
		}
		/********************************私人场统计****************************************/

		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		return true;
	}
	case GER_NORMAL:		//常规结束
	{
		//变量定义
		CMD_S_GameEnd GameEnd;
		ZeroMemory(&GameEnd, sizeof(GameEnd));
		
		//结束信息
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
			GameEnd.cbWeaveItemCount[i] = m_cbWeaveItemCount[i];
			CopyMemory(GameEnd.WeaveItemArray[i], m_WeaveItemArray[i], sizeof(tagWeaveItem)*MAX_WEAVE);
			CopyMemory(&GameEnd.stHuPaiResult[i], &m_HuPaiResult[i], sizeof(tagHuPaiResult));
			GameEnd.cbHuaPaiCount[i][0] = m_cbHuaPaiCount[i][0];
			GameEnd.cbHuaPaiCount[i][1] = m_cbHuaPaiCount[i][1];

			// 杠分，出牌分;
			GameEnd.lGangScore[i] = m_lGangScore[i];
			GameEnd.lOutScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1];
		}

		//结束信息;
		GameEnd.wBankerUser = m_wBankerUser;
		GameEnd.wProvideUser = m_wProvideUser;
		GameEnd.cbProvideCard = m_cbHuPaiCard;
		GameEnd.bHaveBiXiaHu = m_bLastRoundBiXiaHu;
		GameEnd.lCellScore = m_pITableFrame->GetCellScore();
		GameEnd.cbHuCardData = m_cbHuPaiCard;		// 吃胡的牌;

		bool bUserBigHu = false;
		bool bBankerHuPai = false;
		WORD wZiMoHuPaiUser = INVALID_CHAIR;
		LONG lBiXiaHuMultiple = (m_bLastRoundBiXiaHu ? 1L : 2L);
		LONG lTiaoZhanScore[GAME_PLAYER] = { 0L };

		// 大胡标识;
		DWORD dwBigHu = HU_WUHUAGUO | HU_YAJUE | HU_DGKAIHUA | HU_XGKAIHUA | HU_HUNYISE |
			HU_QINGYISE | HU_DUIDUIHU | HU_QQDUDIAO | HU_QIDUI | HU_SQIDUI | HU_TIANHU | HU_DIHU | HU_BAOZI;

		//统计积分
		if (m_wProvideUser != INVALID_CHAIR)
		{
			GameEnd.cbGameEndType = 0;

			BYTE cbHuPaiCount = 0;	// 胡牌人数;

			//统计是否是自摸
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

			//自摸胡牌(一家赢钱三家付)
			if (wZiMoHuPaiUser != INVALID_CHAIR)
			{
				cbHuPaiCount++;

				//大胡不换庄判断
				//if ((m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiKind&HU_PINGHU) == 0 && (m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiKind&HU_MENQING) == 0)
				if ((m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiKind&dwBigHu) != 0)
				{
					bUserBigHu = true;
				}

				//是否庄家胡牌
				if (wZiMoHuPaiUser == m_wBankerUser)
				{
					bBankerHuPai = true;
				}

				//计算报听分数
				GameEnd.lTingPaiScore[wZiMoHuPaiUser] = (m_bUserTing[wZiMoHuPaiUser] ? 10 : 0);

				// 每个玩家胡牌输分;
				LONG lScore = (m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiFanShu + (m_cbHuaPaiCount[wZiMoHuPaiUser][0] + m_cbHuaPaiCount[wZiMoHuPaiUser][1]) * 2 + 20) / lBiXiaHuMultiple;

#if IS_OPEN_PRINT_LOG
				MyLog("自摸玩家[%d] 理论赢钱[%d] dwHuPaiFanShu[%d] 硬花[%d] 软花[%d] 比下胡[%d]",
					wZiMoHuPaiUser, lScore, m_HuPaiResult[wZiMoHuPaiUser].dwHuPaiFanShu, m_cbHuaPaiCount[wZiMoHuPaiUser][0], m_cbHuaPaiCount[wZiMoHuPaiUser][1], lBiXiaHuMultiple);
#endif
				
				//积分平衡;
				LONG lAllWinScore = 0L;
				if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) && */(m_wGangPaiProvideUser != wZiMoHuPaiUser) && (m_wGangPaiProvideUser != INVALID_CHAIR))
				{
					IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wGangPaiProvideUser);
					ASSERT(pServerUserItem != NULL);

					// 游戏开始身上积分;
					LONG lUserScore = (LONG)pServerUserItem->GetUserScore();

					// 结算时实际身上积分;
					LONG lUserGameScore = lUserScore + (m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser]);
#if IS_OPEN_PRINT_LOG
					MyLog("扣钱人[%d]实际积分  ===================== 自摸包杠 UserScore = %ld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
						m_wGangPaiProvideUser, lUserScore, m_lOutCardScore[m_wGangPaiProvideUser][0], m_lOutCardScore[m_wGangPaiProvideUser][1], m_lGangScore[m_wGangPaiProvideUser]);
#endif
					LONG lShouldDelScore = (3 * lScore);
					// 身上实际积分小于输积分，平衡积分;
					if (lUserGameScore < lShouldDelScore)
					{
						lAllWinScore += lUserGameScore;
						GameEnd.lGameScore[m_wGangPaiProvideUser] = 0 - lUserScore;	// 实际输赢;

						GameEnd.lHuScore[m_wGangPaiProvideUser] = 0 - lUserGameScore;
					}
					else
					{
						lAllWinScore += lShouldDelScore;
						GameEnd.lGameScore[m_wGangPaiProvideUser] = m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser] - lShouldDelScore;	// 结算后实际输赢包括杠和罚分;

						GameEnd.lHuScore[m_wGangPaiProvideUser] = 0 - lAllWinScore;
					}

					// 计算其他玩家积分;
					for (WORD j = 0; j < GAME_PLAYER; j++)
					{
						// 过滤包杠玩家;
						if (j == m_wGangPaiProvideUser) continue;

						// 过滤胡牌玩家;
						if (j == wZiMoHuPaiUser) continue;

						// 结算后实际输赢包括杠和罚分;
						GameEnd.lGameScore[j] = (m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j]);
					}
				}
				else
				{ 
					for (WORD j = 0; j < GAME_PLAYER; j++)
					{
						// 过滤自摸玩家;
						if (j == wZiMoHuPaiUser) continue;

						IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(j);
						ASSERT(pServerUserItem != NULL);

						// 游戏开始身上积分;
						LONG lUserScore = (LONG)pServerUserItem->GetUserScore();

						// 结算时实际身上积分;
						LONG lUserGameScore = lUserScore + (m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j]);
#if IS_OPEN_PRINT_LOG
						MyLog("扣钱人[%d]实际积分  ===================== 自摸 UserScore = %ld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
							j, lUserScore, m_lOutCardScore[j][0], m_lOutCardScore[j][1], m_lGangScore[j]);
#endif
						// 身上实际积分小于输积分，平衡积分;
						if (lUserGameScore < lScore)
						{
							lAllWinScore += lUserGameScore;
							GameEnd.lGameScore[j] = 0 - lUserScore;	// 实际输赢;

							GameEnd.lHuScore[j] = 0 - lUserGameScore;
						}
						else
						{
							lAllWinScore += lScore;
							GameEnd.lGameScore[j] = (m_lOutCardScore[j][0] + m_lOutCardScore[j][1] + m_lGangScore[j]) - lScore;	// 结算后实际输赢包括杠和罚分;

							GameEnd.lHuScore[j] = 0 - lScore;
						}
					}
				}

				// 给赢家积分(包括杠和罚分);
				GameEnd.lGameScore[wZiMoHuPaiUser] = lAllWinScore + (m_lOutCardScore[wZiMoHuPaiUser][0] + m_lOutCardScore[wZiMoHuPaiUser][1] + m_lGangScore[wZiMoHuPaiUser]);;
				GameEnd.lHuScore[wZiMoHuPaiUser] = lAllWinScore;
			}
			//放铳胡牌(一炮多响一家输钱)
			else
			{
				//循环累计
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
					{
						//庄家胡牌判断
						if (i == m_wBankerUser)
						{
							bBankerHuPai = true;
						}

						//大胡不换庄判断
						if ((m_HuPaiResult[i].dwHuPaiKind&dwBigHu) != 0)
						{
							bUserBigHu = true;
						}

						// 是否有包子;
						if ((m_HuPaiResult[i].dwHuPaiKind&HU_BAOZI) != 0)
						{
							m_bHaveBaozi = true;
						}

						//计算报听分数
						GameEnd.lTingPaiScore[i] = (m_bUserTing[i] ? 10 : 0);

						cbHuPaiCount++;
					}
				}

				LONG lProvideLoseScore = 0L;

				// 一炮多响，从点炮者下家开始付钱，如果不够付所有人，则按照顺序一家家给，直到点炮的人身上钱全部输完;
				// 点炮者
				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wProvideUser);
				ASSERT(pServerUserItem != NULL);

				// 游戏开始身上积分;
				LONG lUserScore = (LONG)pServerUserItem->GetUserScore();

				// 结算时实际身上积分;
				LONG lUserGameScore = lUserScore + (m_lOutCardScore[m_wProvideUser][0] + m_lOutCardScore[m_wProvideUser][1] + m_lGangScore[m_wProvideUser]);
#if IS_OPEN_PRINT_LOG
				MyLog("点跑人[%d]实际积分  ===================== UserScore = %ld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
					m_wProvideUser, lUserScore, m_lOutCardScore[m_wProvideUser][0], m_lOutCardScore[m_wProvideUser][1], m_lGangScore[m_wProvideUser]);
#endif
				// 点炮者输积分
				LONG lAllLoseScore = 0;

				for (WORD i = 1; i < GAME_PLAYER; i++)
				{
					WORD wNext = (m_wProvideUser + GAME_PLAYER - i) % GAME_PLAYER;

					// 此人是否胡牌并且不是胡包子;
					if ((m_HuPaiResult[wNext].dwHuPaiKind != HU_NULL) && ((m_HuPaiResult[wNext].dwHuPaiKind&HU_BAOZI) == 0))
					{
						// 此人胡牌分数;
						LONG lScore = (m_HuPaiResult[wNext].dwHuPaiFanShu + (m_cbHuaPaiCount[wNext][0] + m_cbHuaPaiCount[wNext][1]) * 2 + 20) / lBiXiaHuMultiple;

						//统计抢杠胡胡牌
						if (IsQiangGangHu(wNext))
						{
#if IS_OPEN_PRINT_LOG
							MyLog("抢杠胡玩家[%d] 初始扣分[%ld]", lScore);
#endif
							GameEnd.stHuPaiResult[wNext].dwHuPaiKind |= HU_QIANGGANGHU;
							lScore = lScore * 3L;		// 抢杠胡三倍扣分;
							bUserBigHu = true;
						}
#if IS_OPEN_PRINT_LOG
						MyLog("胡牌玩家[%d] 理论赢钱[%d] dwHuPaiFanShu[%d] 硬花[%d] 软花[%d] 比下胡[%d]",
							wNext, lScore, m_HuPaiResult[wNext].dwHuPaiFanShu, m_cbHuaPaiCount[wNext][0], m_cbHuaPaiCount[wNext][1], lBiXiaHuMultiple);
#endif
						// 点炮者积分不足;
						if (lUserGameScore < lScore)
						{
							// 胜利者的实际积分;
							GameEnd.lGameScore[wNext] = lUserGameScore + (m_lOutCardScore[wNext][0] + m_lOutCardScore[wNext][1] + m_lGangScore[wNext]);
							
							// 胡牌分;
							GameEnd.lHuScore[wNext] = lUserGameScore;

							if (lUserGameScore > 0)
							{
								lAllLoseScore -= lUserGameScore;
							}

							lUserGameScore = 0;
						}
						else
						{
							// 胜利者的实际积分;
							GameEnd.lGameScore[wNext] = lScore + (m_lOutCardScore[wNext][0] + m_lOutCardScore[wNext][1] + m_lGangScore[wNext]);

							// 胡牌分;
							GameEnd.lHuScore[wNext] = lScore;

							// 剩余积分;
							lUserGameScore -= lScore;
							lAllLoseScore -= lScore;
						}
					}
					else
					{
						// 平局者，杠和罚分统计;
						GameEnd.lGameScore[wNext] = (m_lOutCardScore[wNext][0] + m_lOutCardScore[wNext][1] + m_lGangScore[wNext]);

						// 胡的是包子，记录外包分;
						if ((m_HuPaiResult[wNext].dwHuPaiKind&HU_BAOZI) == HU_BAOZI)
						{
							GameEnd.lBaoZiScore[wNext] = BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// 胡包子人得分;
							
							// 获取包子提供者，三碰同一家的提供者;
							WORD wBaoziProvideUser = GetBaoziProvideUser(wNext);
							
							// 不是三碰同一家;
							if (INVALID_CHAIR == wBaoziProvideUser)
							{
								wBaoziProvideUser = m_wProvideUser;
							}

							GameEnd.lBaoZiScore[wBaoziProvideUser] -= BASE_BAO_ZI_SCORE / lBiXiaHuMultiple;	// 点包子人得分;
						}
					}
				}

				// 点炮者最终积分;
				GameEnd.lGameScore[m_wProvideUser] = (m_lOutCardScore[m_wProvideUser][0] + m_lOutCardScore[m_wProvideUser][1] + m_lGangScore[m_wProvideUser]) + lAllLoseScore;

				GameEnd.lHuScore[m_wProvideUser] = lAllLoseScore;
			}

			//下庄：庄家只要没胡牌的情况下都会下庄（除了包子和留局）;
			if (!bBankerHuPai && !m_bHaveBaozi)
			{
				//m_wBankerUser = (m_wBankerUser + GAME_PLAYER - 1) % GAME_PLAYER;
				m_bChangeBanker = true;

				// 局数统计必须在写数据库之前;
				m_pITableFrame->addPrivatePlayCout(1);
			}

			//保存大胡状态
			m_bHaveBigHu = bUserBigHu;

			// 一炮多响算大胡，用了统计比下胡;
			if (cbHuPaiCount > 1)
			{
				m_bHaveBigHu = true;
			}
		}
		else
		{
			//荒庄
			GameEnd.cbGameEndType = 1;

			// 算杠分;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				GameEnd.lGameScore[i] = m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
			}
		}

		//房间底分
		LONG lCellScore = m_pITableFrame->GetCellScore();

		//定义变量
		LONG lUserGameScore[GAME_PLAYER] = { 0L };
		BYTE cbScoreKind[GAME_PLAYER] = { SCORE_TYPE_NULL };
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			GameEnd.lGameScore[i] *= lCellScore;
			lUserGameScore[i] = GameEnd.lGameScore[i];	// 最终得分
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

		//发送信息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		// 游戏录像
		addGameRecordAction(SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

		/**************结算积分统计判断********************************************/
#if IS_OPEN_PRINT_LOG
		LONG lEndScore = 0;

		//最终成绩
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

		//统计积分
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (NULL == m_pITableFrame->GetTableUserItem(i)) continue;

			ScoreInfoArray[i].lScore = lUserGameScore[i];
			ScoreInfoArray[i].cbType = cbScoreKind[i];
		}

		//写入积分
		DataStream kDataStream;
		m_kGameRecord.StreamValue(kDataStream, true);
		m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray), kDataStream);

		m_bLastRoundBiXiaHu = IsBiXiaHu();

		// 最后一局胡牌比下胡,庄家连庄继续游戏;
		if (m_bLastRoundBiXiaHu)
		{
			tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
			tagPrivateFrameRecordInfo* pPrivateRecord = m_pITableFrame->GetPrivateFrameRecord();

			if (pPrivateFrame->cbGameCout <= pPrivateRecord->cbFinishCout)
			{
				// 统计完成局数;
				m_pITableFrame->addPrivatePlayCout(-1);
				m_bChangeBanker = false;
			}
		}

		/********************************私人场统计****************************************/
		//统计是否是自摸
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			// 赢家
			if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
			{
				// 自摸
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbZiMoCout[i]++;
				}
				else // 捉炮
				{
					m_PrivateEndInfo.cbZhuoPaoCout[i]++;
				}

				// 胜利次数
				m_PrivateEndInfo.cbWinCout[i]++;
			}
			else // 输家
			{
				// 点炮
				if (i == m_wProvideUser)
				{
					m_PrivateEndInfo.cbDianPaoCout[i]++;
				}

				// 失败次数
				m_PrivateEndInfo.cbLoseCout[i]++;
			}

			// 最高得分
			if (m_PrivateEndInfo.lMaxScore[i] <= GameEnd.lGameScore[i])
			{
				m_PrivateEndInfo.lMaxScore[i] = GameEnd.lGameScore[i];
			}

			// 总积分;
			m_PrivateEndInfo.lAllScore[i] += GameEnd.lGameScore[i];
			m_PrivateEndInfo.lBaoZiScore[i] += GameEnd.lBaoZiScore[i];
		}
		/********************************私人场统计****************************************/

		// 是否换庄;
		if (m_bChangeBanker)
		{
			m_wBankerUser = (m_wBankerUser + GAME_PLAYER - 1) % GAME_PLAYER;
		}

		//结束游戏
		m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

		return true;
	}
	default:
		break;
	}

	//错误断言
	ASSERT(FALSE);

#if IS_OPEN_PRINT_LOG
	MyLog("OnEventGameConclude error wChairID = %d, cbReason = %d end", wChairID, cbReason);
#endif

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
	{
		//构造数据
		CMD_S_StatusFree stStatusFree;
		ZeroMemory(&stStatusFree, sizeof(stStatusFree));
		stStatusFree.bCurBiXiaHu = m_bLastRoundBiXiaHu;

		//获取房间底注
		m_lBaseScore = m_pITableFrame->GetCellScore();
		stStatusFree.lBaseScore = m_lBaseScore;
		stStatusFree.lCellScore = m_pGameServiceOption->lCellScore;
		stStatusFree.lMaxCellScore = (LONG)m_pGameServiceOption->lMaxEnterScore;
		stStatusFree.cbSBSStatus = STATUS_SBS_ERROR;

		//发送场景
		return m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusFree, sizeof(stStatusFree));
	}
	case GS_MJ_DICE:
	{
		//获取房间底注
		m_lBaseScore = m_pITableFrame->GetCellScore();

		//变量定义
		CMD_S_StatusDice stStatusDice;
		ZeroMemory(&stStatusDice, sizeof(stStatusDice));
		//构造数据
		stStatusDice.lBaseScore = m_pITableFrame->GetCellScore();
		stStatusDice.bCurBiXiaHu = m_bLastRoundBiXiaHu;
		stStatusDice.wBankerUser = m_wBankerUser;
		stStatusDice.wEastUser = m_wEastUser;
		//发送场景
		return m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusDice, sizeof(stStatusDice));
	}
	case GS_MJ_PLAY:	//游戏状态
	{
		//获取房间底注
		m_lBaseScore = m_pITableFrame->GetCellScore();

		//变量定义
		CMD_S_StatusPlay stStatusPlay;
		ZeroMemory(&stStatusPlay, sizeof(stStatusPlay));
		//构造数据
		stStatusPlay.lBaseScore = m_pITableFrame->GetCellScore();
		stStatusPlay.bCurBiXiaHu = m_bLastRoundBiXiaHu;
		stStatusPlay.wSiceCount = m_wSiceCount;
		stStatusPlay.wBankerUser = m_wBankerUser;
		stStatusPlay.wCurrentUser = m_wCurrentUser;
		stStatusPlay.wEastUser = m_wEastUser;
		CopyMemory(stStatusPlay.bUserTrust, m_bUserTrust, sizeof(m_bUserTrust));
		CopyMemory(stStatusPlay.bUserTing, m_bUserTing, sizeof(m_bUserTing));
		stStatusPlay.lTiaoZhanScore = 0;// m_lTiaoZhanScore[wChiarID];

		//状态变量
		stStatusPlay.cbLeftCardCount = m_cbLeftCardCount;
		//stStatusPlay.cbActionCard = m_cbProvideCard;
		//stStatusPlay.dwActionMask = !m_bResponse[wChiarID] ? m_dwUserAction[wChiarID] : WIK_NULL;

		//历史记录
		stStatusPlay.wOutCardUser = m_wOutCardUser;
		stStatusPlay.cbOutCardData = m_cbOutCardData;
		CopyMemory(stStatusPlay.cbDiscardCount, m_cbDiscardCount, sizeof(stStatusPlay.cbDiscardCount));
		CopyMemory(stStatusPlay.cbDiscardCard, m_cbDiscardCard, sizeof(stStatusPlay.cbDiscardCard));

		//组合牌
		CopyMemory(stStatusPlay.cbWeaveCount, m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));
		CopyMemory(stStatusPlay.WeaveItemArray, m_WeaveItemArray, sizeof(m_WeaveItemArray));
		CopyMemory(stStatusPlay.lOutCardScore, m_lOutCardScore, sizeof(m_lOutCardScore));
		CopyMemory(stStatusPlay.lGangScore, m_lGangScore, sizeof(m_lGangScore));

		//牌数据
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

		////杠牌数据
		//if ((m_dwUserAction[wChiarID] & WIK_GANG) != 0 && !m_bResponse[wChiarID])
		//{
		//	CopyMemory(&stStatusPlay.GangPaiResult, &m_GangPaiResult[wChiarID], sizeof(tagGangPaiResult));
		//}
		////听牌数据
		//if ((m_dwUserAction[wChiarID] & WIK_TING) != 0 && !m_bResponse[wChiarID])
		//{
		//	CopyMemory(&stStatusPlay.TingPaiResult, &m_TingPaiResult[wChiarID], sizeof(tagTingPaiResult));
		//}
		//CopyMemory(stStatusPlay.SpecialLights, m_SpecialLight, sizeof(m_SpecialLight));

		// 场景恢复;
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

		//发送场景
		bool bSendGameScene = m_pITableFrame->SendGameScene(pIServerUserItem, &stStatusPlay, sizeof(stStatusPlay));
		
		// 发送高级操作;
		//给玩家发送吃碰杠听胡消息
		CMD_S_OperateNotify_Send stOperateNotify;
		ZeroMemory(&stOperateNotify, sizeof(stOperateNotify));

		stOperateNotify.wOperateUser = wChiarID;
		stOperateNotify.dwActionMask = !m_bResponse[wChiarID] ? m_dwUserAction[wChiarID] : WIK_NULL;
		stOperateNotify.wCurrentUser = m_wCurrentUser;
		stOperateNotify.cbActionCard = m_cbProvideCard;

		CopyMemory(&stOperateNotify.GangPaiResult, &m_GangPaiResult[wChiarID], sizeof(tagGangPaiResult));

		DataStream databuff;
		databuff.pushValue(&stOperateNotify, sizeof(CMD_S_OperateNotify_Send));

		//听牌数据
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

//定时器事件
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
		//出牌定时器到
		WORD wOutChairID = static_cast<WORD>(wBindParam);
		OnSubOutCard(wOutChairID, GetOperateCard(wOutChairID));
		//定时器类型
		BYTE cbTimerStyle = static_cast<BYTE>((wBindParam >> 16) & 0xff);
		if (cbTimerStyle == IDS_TIMER_NORMAL && !m_bUserTrust[wOutChairID])
		{
			OnSubUserTrust(wOutChairID, true);
		}
	}return true;
	case IDI_USER_OPERATE:
	{
		//操作定时器到
		//定时器类型
		BYTE cbTimerStyle = static_cast<BYTE>((wBindParam >> 16) & 0xff);
		//被动操作
		if (m_wCurrentUser == INVALID_CHAIR)
		{
			bool bHaveOperate = false;
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i == (WORD)wBindParam || m_bResponse[i] || m_dwUserAction[i] == WIK_NULL || bHaveOperate) continue;

				OnSubOperateCard(i, WIK_NULL, m_cbProvideCard, bHaveOperate);
			}
			//超时托管
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
		//主动操作
		else
		{
			WORD wOperateChairID = (WORD)wBindParam;
			OnSubOutCard(wOperateChairID, GetOperateCard(wOperateChairID));
			//超时托管
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
		//非托管状态并且是掉线状态
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//当前操作玩家是自己
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//自己有操作，则托管
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
		//非托管状态并且是掉线状态
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//当前操作玩家是自己
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//自己有操作，则托管
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
		//非托管状态并且是掉线状态
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//当前操作玩家是自己
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//自己有操作，则托管
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
		//非托管状态并且是掉线状态
		if (!m_bUserTrust[wChairID] && m_bUserOffline[wChairID])
		{
			if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY)
			{
				//当前操作玩家是自己
				if (m_wCurrentUser == wChairID)
				{
					OnSubOutCard(wChairID, GetOperateCard(wChairID));
					OnSubUserTrust(wChairID, true);
				}
				else if (m_wCurrentUser == INVALID_CHAIR)
				{
					//自己有操作，则托管
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

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_SET_BASESCORE:	//玩家设置底注
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_C_SetBaseScore));
		if (wDataSize != sizeof(CMD_C_SetBaseScore)) return false;
		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_LOOKON) return true;

		CMD_C_SetBaseScore *pSetBaseScore = (CMD_C_SetBaseScore *)pDataBuffer;
		ASSERT(pSetBaseScore != NULL);
		return OnSubSetBaseScore(pUserData->wChairID, pSetBaseScore->lBaseScore, pSetBaseScore->cbSBSAction);
	}
	case SUB_C_SHOOT_DICE:		//掷骰子消息
	{
		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubShootDice(pUserData->wChairID);
	}
	case SUB_C_DISPATCH_OVER:	//发牌完成消息
	{
		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubDispatchOver(pUserData->wChairID);
	}
	case SUB_C_SENDCARD_OVER:	//摸牌完成消息
	{
		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubSendCardOver(pUserData->wChairID);
	}
	case SUB_C_BUHUA_OVER:		//补花完成消息
	{
		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		ASSERT(pUserData != NULL && pUserData->cbUserStatus == US_PLAYING);
		if (pUserData == NULL || pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		return OnSubBuHuaOver(pUserData->wChairID);
	}
	case SUB_C_OUT_CARD:		//出牌消息
	{
		//效验消息
		ASSERT(wDataSize == sizeof(CMD_C_OutCard));
		if (wDataSize != sizeof(CMD_C_OutCard)) return false;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		//消息处理
		CMD_C_OutCard *pOutCard = (CMD_C_OutCard *)pDataBuffer;
		ASSERT(pOutCard != NULL);
		return OnSubOutCard(pUserData->wChairID, pOutCard->cbCardData);
	}
	case SUB_C_OPERATE_CARD:	//操作消息
	{
		//效验消息
		ASSERT(wDataSize == sizeof(CMD_C_OperateCard));
		if (wDataSize != sizeof(CMD_C_OperateCard)) return false;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
		ASSERT(IsUserPlaying(pUserData->wChairID));
		if (!IsUserPlaying(pUserData->wChairID)) return false;

		//消息处理
		CMD_C_OperateCard *pOperateCard = (CMD_C_OperateCard *)pDataBuffer;
		ASSERT(pOperateCard != NULL);

		bool bHaveOperate = false;
		OnSubOperateCard(pUserData->wChairID, pOperateCard->dwOperateCode, pOperateCard->cbOperateCard, bHaveOperate);
		return true;
	}
	case SUB_C_USER_TRUST:
	{
		//效验消息
		ASSERT(wDataSize == sizeof(CMD_C_UserTrust));
		if (wDataSize != sizeof(CMD_C_UserTrust)) return false;

		//用户效验
		tagUserInfo * pUserData = pIServerUserItem->GetUserInfo();
		if (pUserData->cbUserStatus != US_PLAYING) return true;

		//状态判断
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

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//任务是否完成
bool CTableFrameSink::IsTaskCompleted(WORD wChairID, DWORD dwTaskID, IN OUT DWORD &dwRecord)
{
	return false;
}

//用户坐下
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

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (!bLookonUser)
	{
		//重置变量(全部重置)
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
						m_pITableFrame->SetGameTimer(IDI_BASESCORE, IDT_BASESCORE * 1000 + 500, 1, 0); //启动定时器
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

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(wChairID < GAME_PLAYER);
	if (wChairID >= GAME_PLAYER) return false;

	//掉线时，转换成系统托管
	if (!m_bUserTrust[wChairID] && !m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = true;

		////启动延时定时器
		//if (m_pGameServiceOption->wServerType != GAME_GENRE_EDUCATE)
		//{
		//	m_pITableFrame->SetGameTimer(IDI_USER_OFFLINE + wChairID, IDT_USER_OFFLINE * 1500, 1, wChairID);
		//}
	}
	return true;
}

//用户重入
bool CTableFrameSink::OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(wChairID < GAME_PLAYER);
	if (wChairID >= GAME_PLAYER) return false;

	//玩家重连进来
	if (m_bUserOffline[wChairID])
	{
		m_bUserOffline[wChairID] = false;
		m_pITableFrame->KillGameTimer(IDI_USER_OFFLINE + wChairID);
	}

	return true;
}

//设置底注
bool CTableFrameSink::OnSubSetBaseScore(WORD wChairID, LONG lBaseScore, BYTE cbActionType)
{
	//不同意底注
	if (cbActionType == ACTION_SBS_NOSET || cbActionType == ACTION_SBS_DISAGREE)
	{
		SendKickOutMsg(wChairID);
		return true;
	}

	m_bAgreeBaseScore[wChairID] = true;

	if (cbActionType == ACTION_SBS_SET) //设置底注
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
			m_pITableFrame->SetGameTimer(IDI_BASESCORE, IDT_BASESCORE * 1000 + 500, 1, 0); //启动定时器
		}
	}

	return true;
}

//开始发牌
bool CTableFrameSink::OnSubShootDice(WORD wChairID)
{
	//游戏状态校验
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_DICE);
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	VERIFY_RETURN_FALSE(wChairID == m_wCurrentUser);

	//结束掷骰子定时器
	m_pITableFrame->KillGameTimer(IDI_SHOOT_DICE);

	//设置游戏状态
	m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

	//随机骰子数
	m_wSiceCount = MAKEWORD(rand() % 6 + 1, rand() % 6 + 1);

	//混乱牌
	m_cbLeftCardCount = MAX_REPERTORY;
	m_GameLogic.RandCardData(m_cbRepertoryCard, MAX_REPERTORY);

	UseCardLibrary();

	//分发牌
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_cbLeftCardCount -= (MAX_COUNT - 1);
		m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount], MAX_COUNT - 1, m_cbCardIndex[i]);
		VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[i]) <= MAX_COUNT);
	}

	//发送发牌消息
	CMD_S_GameStart stGameStart;
	ZeroMemory(&stGameStart, sizeof(stGameStart));
	stGameStart.wSiceCount = m_wSiceCount;
	//发送数据
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//设置变量
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

		//发送数据
		m_pITableFrame->SendTableData(i, SUB_S_GAME_START, &stGameStart, sizeof(stGameStart));
		m_pITableFrame->SendLookonData(i, SUB_S_GAME_START, &stGameStart, sizeof(stGameStart));
	}

	// 录像开始
	starGameRecord();

	//启动定时器
	m_pITableFrame->SetGameTimer(IDI_DISPATCH_CARD, (IDT_DISPATCH_CARD + 1) * 1000, 1, 0);
	return true;
}

//发牌完成
bool CTableFrameSink::OnSubDispatchOver(WORD wChairID)
{
	//游戏状态校验
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//用户状态校验
	VERIFY_RETURN_TRUE(!m_bDispatchCardOver[wChairID]);

	//变量设置
	m_bDispatchCardOver[wChairID] = true;

	//检测是否全部发牌完成
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bDispatchCardOver[i])
		{
			return true;
		}
	}
	//删除发牌定时器
	m_pITableFrame->KillGameTimer(IDI_DISPATCH_CARD);
	//给庄家发第14张牌
	SendCardData(m_wBankerUser, SEND_CARD_FRONT, true);

	return true;
}

//摸牌完成
bool CTableFrameSink::OnSubSendCardOver(WORD wChairID)
{
	//游戏状态校验
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//用户状态校验
	VERIFY_RETURN_TRUE(!m_bSendCardOver[wChairID]);

	//变量设置
	m_bSendCardOver[wChairID] = true;

	//检测是否全部摸牌动画完成
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bSendCardOver[i])
		{
			return true;
		}
	}
	//删除摸牌定时器
	m_pITableFrame->KillGameTimer(IDI_SEND_CARD);
	//补花检测
	if (m_GameLogic.IsValidUser(m_wSendCardUser))
	{
		EstimatUserBuHua(m_wSendCardUser);
	}
	return true;
}

// 获取要牌
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
		//CTraceService::TraceString(TEXT("你选择的牌已经不存在"), TraceLevel_Exception);
#if IS_OPEN_PRINT_LOG
		MyLog(_T("玩家ID: %d, 选择的牌已经不存在!"), dwSendUser);
		m_pITableFrame->SendGameMessage(m_pITableFrame->GetTableUserItem(dwSendUser), "你选择的牌已经不存在！", SMT_CHAT);
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

//补花要替换的牌
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

//补花完成
bool CTableFrameSink::OnSubBuHuaOver(WORD wChairID)
{
	//游戏状态校验
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	//用户状态校验
	VERIFY_RETURN_TRUE(!m_bBuHuaOver[wChairID]);

	//变量设置
	m_bBuHuaOver[wChairID] = true;

	//检测是否全部补花动画完成
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (!m_bBuHuaOver[i])
		{
			return true;
		}
	}
	//删除补花定时器
	m_pITableFrame->KillGameTimer(IDI_SEND_BUHUA);
	//补花检测
	if (m_GameLogic.IsValidUser(m_wBuHuaUser))
	{
		EstimatUserBuHua(m_wBuHuaUser);
	}
	return true;
}

//用户出牌
bool CTableFrameSink::OnSubOutCard(WORD wChairID, BYTE cbCardData)
{
	//游戏状态校验
	VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);

	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	VERIFY_RETURN_TRUE(wChairID == m_wCurrentUser);
	
	//出牌数据校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidCard(cbCardData));

	// 必须首轮补花完成才可以出牌;
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

	//删除牌
	if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbCardData))
	{
		ASSERT(FALSE);
		return false;
	}
	//手牌数目校验
	BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbHandCardCount > 0 && cbHandCardCount < MAX_COUNT);

	//结束出牌定时器
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD);
	m_pITableFrame->KillGameTimer(IDI_USER_OPERATE);

	//重置操作变量
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
	ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

	//重置杠用户;
	m_wGangPaiProvideUser = INVALID_CHAIR;

	m_bInvalidHuStates[wChairID] = false;
	m_cbInvalidOperteStates[wChairID] = 0x0;

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser = wChairID;
	m_cbOutCardData = cbCardData;

	//加入到玩家出牌记录中
	ASSERT(m_cbDiscardCount[wChairID] + 1 < CountArray(m_cbDiscardCard[wChairID]));
	m_cbDiscardCard[wChairID][m_cbDiscardCount[wChairID]++] = cbCardData;
	m_cbDiscardRecordCard[wChairID][m_cbDiscardRecordCount[wChairID]++] = cbCardData;
	m_cbDiscardIndexCount[wChairID][m_GameLogic.SwitchToCardIndex(cbCardData)]++;

	// 天听不听牌直接出牌，取消天听;
	if (m_bUserTing[wChairID] == false)
	{
		if (m_bUserTianTing[wChairID])
		{
			m_bUserTianTing[wChairID] = false;
		}
	}
	
	//记录一圈出牌的记录
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

	//发送出牌消息
	CMD_S_OutCard stOutCard;
	ZeroMemory(&stOutCard, sizeof(stOutCard));
	stOutCard.wOutCardUser = wChairID;
	stOutCard.cbOutCardData = cbCardData;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OUT_CARD, &stOutCard, sizeof(stOutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OUT_CARD, &stOutCard, sizeof(stOutCard));

	// 游戏录像
	addGameRecordAction(SUB_S_OUT_CARD, &stOutCard, sizeof(stOutCard));

	//计算出牌得分
	CheckOutCardScore(wChairID);

	//用户切换
	m_wProvideUser = wChairID;
	m_cbProvideCard = cbCardData;
	m_wCurrentUser = (wChairID + GAME_PLAYER - 1) % GAME_PLAYER;
	m_bSendCardStatus = false;
	m_cbGangStatus = GS_NULL;

	//响应判断
	bool bAroseAction = EstimateUserRespond(wChairID, cbCardData, EstimatKind_OutCard);

	//下家摸牌
	if (!bAroseAction)
	{
		SendCardData(m_wCurrentUser, SEND_CARD_FRONT);
	}

	return true;
}

//用户操作
bool CTableFrameSink::OnSubOperateCard(WORD wChairID, DWORD dwOperateCode, BYTE cbOperateCard, bool &bHaveOperate)
{
#if IS_OPEN_PRINT_LOG
	MyLog("OnSubOperateCard(%d, %d, %d, %s) begin", wChairID, dwOperateCode, cbOperateCard, BOOL_TO_STRING(bHaveOperate));
#endif

	//游戏效验状态
	//VERIFY_RETURN_TRUE(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY);
	if (!(m_pITableFrame->GetGameStatus() == GS_MJ_PLAY))
	{ 
		ASSERT(0); 
#if IS_OPEN_PRINT_LOG
		MyLog("OnSubOperateCard error wChairID = %d, GetGameStatus = %d, dwOperateCode = %d, cbOperateCard = 0x%x", wChairID, m_pITableFrame->GetGameStatus(), dwOperateCode, cbOperateCard);
#endif
		return true; 
	}

	//用户效验
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

	//被动动作
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		//效验状态
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

		// 跟打不能胡，不能碰
		if (dwOperateCode == WIK_NULL)
		{
			//捉炮放弃的情况下,玩家这一轮不可以胡牌;
			if ((m_dwUserAction[wChairID] & WIK_ZHUOPAO) == WIK_ZHUOPAO)
			{
				m_bInvalidHuStates[wChairID] = true;
			}
			else if ((m_dwUserAction[wChairID] & WIK_PENG) != 0)
			{
				m_cbInvalidOperteStates[wChairID] = cbOperateCard;
			}
		}

		//变量定义
		WORD wTargetUser = wChairID;
		DWORD dwTargetAction = dwOperateCode;

		//设置变量
		m_bResponse[wChairID] = true;
		m_dwPerformAction[wChairID] = dwOperateCode;
		m_cbOperateCard[wChairID] = (cbOperateCard == 0) ? m_cbProvideCard : cbOperateCard;

		//执行判断
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//获取动作
			DWORD dwUserAction = (!m_bResponse[i]) ? m_dwUserAction[i] : m_dwPerformAction[i];

			//优先级别
			BYTE cbUserActionRank = m_GameLogic.GetUserActionRank(dwUserAction);
			BYTE cbTargetActionRank = m_GameLogic.GetUserActionRank(dwTargetAction);

			//动作判断
			if (cbUserActionRank > cbTargetActionRank)
			{
				wTargetUser = i;
				dwTargetAction = dwUserAction;
			}
		}

		//优先级高的玩家还没操作，等待
		if (!m_bResponse[wTargetUser])
		{
#if IS_OPEN_PRINT_LOG
			MyLog("OnSubOperateCard info m_bResponse[%d] = %s", wTargetUser, (m_bResponse[wTargetUser] ? "true" : "false"));
#endif
			return true;
		}
		
		//还有胡牌玩家没操作，等待
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

		//删除定时器
		m_pITableFrame->KillGameTimer(IDI_USER_OPERATE);

		//变量定义
		BYTE cbTargetCard = m_cbOperateCard[wTargetUser];
		//if (dwTargetAction != WIK_HU && dwTargetAction != WIK_NULL)
		//{
		//	//出牌变量
		//	m_cbOutCardData = 0;
		//	m_wOutCardUser = INVALID_CHAIR;
		//	m_cbGangStatus = GS_NULL;

		//	//清空状态
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
	//主动动作
	else if (m_wCurrentUser == wChairID)
	{
		//效验状态
		ASSERT(!m_bResponse[wChairID]);
		if (m_bResponse[wChairID]) return false;

		m_bResponse[wChairID] = true;

		//数据效验
		if (dwOperateCode == WIK_NULL || (m_dwUserAction[wChairID] & dwOperateCode) == 0)
		{
			ASSERT(FALSE);

			//出牌变量
			m_cbOutCardData = 0;
			m_wOutCardUser = INVALID_CHAIR;
			
			//清空状态
			ZeroMemory(m_bResponse, sizeof(m_bResponse));
			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
			ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
			ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));
			return false;
		}

		//if (dwOperateCode != WIK_HU && dwOperateCode != WIK_NULL)
		//{
		//	//出牌变量
		//	m_cbOutCardData = 0;
		//	m_wOutCardUser = INVALID_CHAIR;
		//	m_cbGangStatus = GS_NULL;

		//	//清空状态
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

//用户托管
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

//碰牌操作
bool CTableFrameSink::PerformPengPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformPengPai(%d, %d, %d) begin", wChairID, dwOperateAction, cbOperateCard);
#endif

	//用户校验
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

	//手牌数目校验
	BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbHandCardCount > 0 && cbHandCardCount < MAX_COUNT);

	//删除自己手中牌
	BYTE cbRemoveCard[2] = { cbOperateCard, cbOperateCard };
	bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbRemoveCard, CountArray(cbRemoveCard));
	ASSERT(bRemoveCard);
	if (!bRemoveCard)
	{
#if IS_OPEN_PRINT_LOG
		MyLog("PerformPengPai error wChairID = %d, cbRemoveCard[0] = 0x%x, cbRemoveCard[1] = 0x%x", wChairID, cbRemoveCard[0], cbRemoveCard[1]);

		// 显示手牌;
		for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
		{
			MyLog("PerformPengPai Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
		}
#endif
		return false;
	}

	VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
	m_bSendCardStatus = false;

	//取消此玩家天听状态
	m_bUserTianTing[wChairID] = false;

	//删除出牌玩家上次出牌数据
	BYTE cbDiscardCount = m_cbDiscardCount[m_wProvideUser]--;
	m_cbDiscardCard[m_wProvideUser][cbDiscardCount] = 0;

	//置当前操作玩家为自己
	m_wCurrentUser = wChairID;

	//加入到自己的组合牌中
	WORD wIndex = m_cbWeaveItemCount[wChairID]++;
	m_WeaveItemArray[wChairID][wIndex].cbPublicCard = TRUE;
	m_WeaveItemArray[wChairID][wIndex].cbCenterCard = cbOperateCard;
	m_WeaveItemArray[wChairID][wIndex].dwWeaveKind = dwOperateAction;
	m_WeaveItemArray[wChairID][wIndex].wProvideUser = m_wProvideUser;

	if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
	{
		m_cbHuaPaiCount[wChairID][1]++;
	}

	//发送玩家操作消息
	CMD_S_OperateResult stOperateResult;
	ZeroMemory(&stOperateResult, sizeof(stOperateResult));
	stOperateResult.wOperateUser = wChairID;
	stOperateResult.cbOperateCard = cbOperateCard;
	stOperateResult.dwOperateCode = dwOperateAction;
	stOperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wChairID : m_wProvideUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	// 游戏录像
	addGameRecordAction(SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	//重置听牌和杠牌数据
	ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
	ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));
	m_cbProvideCard = 0;

	//出牌变量
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;
	m_cbGangStatus = GS_NULL;

	//清空状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

	//检测后续操作
	EstimateUserRespond(wChairID, 0, EstimatKind_ChiPengCard);
#if IS_OPEN_PRINT_LOG
	MyLog("PerformPengPai end");
#endif
	return true;
}

//杠牌操作
bool CTableFrameSink::PerformGangPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bCheckQiangGangHu/*=true*/)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformGangPai(%d, %d, %d, %s) begin", wChairID, dwOperateAction, cbOperateCard, BOOL_TO_STRING(bCheckQiangGangHu));
#endif
	//数据校验
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
		//数据校验
		if (m_wCurrentUser != INVALID_CHAIR || !m_GameLogic.IsValidUser(m_wProvideUser))
		{
			ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error m_wCurrentUser = %d, m_wProvideUser = %d", m_wCurrentUser, m_wProvideUser);
#endif
			return false;
		}

		//删除自己手中牌
		BYTE cbRemoveCard[3] = { cbOperateCard, cbOperateCard, cbOperateCard };
		bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbRemoveCard, CountArray(cbRemoveCard));
		ASSERT(bRemoveCard); 
		if (!bRemoveCard)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error wChairID = %d, cbRemoveCard[0] = 0x%x, cbRemoveCard[1] = 0x%x, cbRemoveCard[2] = 0x%x", 
				wChairID, cbRemoveCard[0], cbRemoveCard[1], cbRemoveCard[2]);

			// 显示手牌;
			for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
			{
				MyLog("PerformGangPai WIK_MGANG Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
			}
#endif
			return false;
		}

		VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
		m_bSendCardStatus = false;

		//取消此玩家天听状态
		m_bUserTianTing[wChairID] = false;

		//重置听牌和杠牌数据
		ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
		ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

		//设置杠牌状态
		m_cbGangStatus = GS_DAGANGKAIHUA;

		//设置杠牌提供用户
		m_wGangPaiProvideUser = m_wProvideUser;

		//删除出牌玩家上次出牌数据
		BYTE cbDiscardCount = m_cbDiscardCount[m_wProvideUser]--;
		m_cbDiscardCard[m_wProvideUser][cbDiscardCount] = 0;

		//加入到自己的组合牌中
		WORD wIndex = m_cbWeaveItemCount[wChairID]++;
		m_WeaveItemArray[wChairID][wIndex].cbPublicCard = TRUE;
		m_WeaveItemArray[wChairID][wIndex].cbCenterCard = cbOperateCard;
		m_WeaveItemArray[wChairID][wIndex].dwWeaveKind = dwOperateAction;
		m_WeaveItemArray[wChairID][wIndex].wProvideUser = m_wProvideUser;

		//总倍数
		LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

		// 中间解散如果有玩家积分为0就不扣分
		// 获取出牌玩家积分
		WORD wProvideUser = m_wProvideUser;
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wProvideUser);
		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wProvideUser][0] + m_lOutCardScore[wProvideUser][1] + m_lGangScore[wProvideUser];
		
#if IS_OPEN_PRINT_LOG
		MyLog("扣钱人[%d]实际积分  ===================== 明杠 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
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

		//计算明杠分数
		m_lGangScore[wChairID] += lGangScore;
		m_lGangScore[m_wProvideUser] -= lGangScore;

		//发送杠牌小结消息
		CMD_S_PayMoney stPayMoney;
		ZeroMemory(&stPayMoney, sizeof(stPayMoney));
		stPayMoney.cbPayReason = PAY_MONEY_MINGGANG;
		stPayMoney.wPayToUser = wChairID;
		stPayMoney.lPayToNumber += lGangScore;
		stPayMoney.wPayFromUser[0] = m_wProvideUser;
		stPayMoney.lPayFromNumber[0] -= lGangScore;

		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

		// 记录日志
		PayMoneyLog(stPayMoney);

		// 统计次数
		m_PrivateEndInfo.cbMingGang[wChairID]++;

		// 风杠2花
		if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
		{
			m_cbHuaPaiCount[wChairID][1] += 2;
		}
		else
		{// 普通1花
			m_cbHuaPaiCount[wChairID][1] ++;
		}

	}break;
	case WIK_BGANG:
	{
		//检测抢杠胡
		if (bCheckQiangGangHu && EstimateUserRespond(wChairID, cbOperateCard, EstimatKind_BuGangCard))
		{
			//设置抢杠胡状态
			m_QiangGangHuInfo.wGangPaiUser = wChairID;
			m_QiangGangHuInfo.cbGangPaiCard = cbOperateCard;

#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai QiangGangHu wChairID = %d, cbOperateCard = %d, bCheckQiangGangHu=%s.", wChairID, cbOperateCard, BOOL_TO_STRING(bCheckQiangGangHu));
#endif
			return true;
		}
		else
		{
			//没有玩家抢杠胡
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
			//查找失败
			ASSERT(cbWeaveIndex != 0xFF);	
			if (cbWeaveIndex == 0xFF)
			{
#if IS_OPEN_PRINT_LOG
				MyLog("PerformGangPai WIK_BGANG error not find wProvideUser wChairID = %d, cbRemoveCard[0] = 0x%x", wChairID, cbOperateCard);
#endif
				return false;
			}

			//删除自己手中牌
			bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbOperateCard);
			ASSERT(bRemoveCard); //if (!bRemoveCard) return false;
			if (!bRemoveCard)
			{
#if IS_OPEN_PRINT_LOG
				MyLog("PerformGangPai WIK_BGANG error wChairID = %d, cbRemoveCard[0] = 0x%x", wChairID, cbOperateCard);

				// 显示手牌;
				for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
				{
					MyLog("PerformGangPai WIK_BGANG Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
				}
#endif
				return false;
			}

			VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
			m_bSendCardStatus = false;

			//取消此玩家天听状态
			m_bUserTianTing[wChairID] = false;

			//重置听牌和杠牌数据
			ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
			ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

			//重置自己组合牌
			m_WeaveItemArray[wChairID][cbWeaveIndex].dwWeaveKind = dwOperateAction;

			//总倍数
			LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

			// 扣钱玩家;
			WORD wProvideUser = INVALID_CHAIR;

			//连续杠牌的规则：玩家A明杠（包含先碰后杠）玩家B的的牌，后面摸牌，这张牌可以暗杠，暗杠后玩家B付三家的钱，在摸牌胡时，也全部算玩家B的牌;
			if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) && */(m_wGangPaiProvideUser != INVALID_CHAIR) && (m_wGangPaiProvideUser != wChairID))
			{
				// 提供一个杠的玩家;
				wProvideUser = m_wGangPaiProvideUser;
			}
			else
			{
				//设置杠牌状态
				m_cbGangStatus = GS_DAGANGKAIHUA;	// 补杠赋值;

				//设置杠牌提供用户
				if (m_wGangPaiProvideUser == INVALID_CHAIR)
				{
					m_wGangPaiProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
				}

				// 碰的提供者;
				wProvideUser = m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser;
			}

			LONG lGangScore = 0L;

			// 中间解散如果有玩家积分为0就不扣分
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wProvideUser);
			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wProvideUser][0] + m_lOutCardScore[wProvideUser][1] + m_lGangScore[wProvideUser];
#if IS_OPEN_PRINT_LOG
			MyLog("扣钱人[%d]实际积分  ===================== 补杠 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
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


			//计算补杠杠分数
			m_lGangScore[wChairID] += lGangScore;
			m_lGangScore[wProvideUser] -= lGangScore;

			//发送杠牌小结消息
			CMD_S_PayMoney stPayMoney;
			ZeroMemory(&stPayMoney, sizeof(stPayMoney));
			stPayMoney.cbPayReason = PAY_MONEY_MINGGANG;
			stPayMoney.wPayToUser = wChairID;
			stPayMoney.lPayToNumber += lGangScore;
			stPayMoney.wPayFromUser[0] = wProvideUser;
			stPayMoney.lPayFromNumber[0] -= lGangScore;

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

			// 记录日志
			PayMoneyLog(stPayMoney);

			// 统计次数
			m_PrivateEndInfo.cbMingGang[wChairID]++;

			// 风杠2花(碰的时候已经加了一朵花;);
			if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
			{
				m_cbHuaPaiCount[wChairID][1] += 1;
			}
			else
			{// 普通1花
				m_cbHuaPaiCount[wChairID][1] ++;
			}
		}
	}break;
	case WIK_AGANG:
	{
		//数据校验
		ASSERT(wChairID == m_wProvideUser);
		if (wChairID != m_wProvideUser)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error WIK_AGANG wChairID = %d, m_wProvideUser = %d", wChairID, m_wProvideUser);
#endif
			return false;
		}

		//删除自己手中牌
		BYTE cbRemoveCard[4] = { cbOperateCard, cbOperateCard, cbOperateCard, cbOperateCard };
		bool bRemoveCard = m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbRemoveCard, CountArray(cbRemoveCard));
		ASSERT(bRemoveCard); //if (!bRemoveCard) return false;

		if (!bRemoveCard)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGangPai error WIK_AGANG wChairID = %d, cbRemoveCard[0] = 0x%x, cbRemoveCard[1] = 0x%x, cbRemoveCard[2] = 0x%x, cbRemoveCard[3] = 0x%x",
				wChairID, cbRemoveCard[0], cbRemoveCard[1], cbRemoveCard[2], cbRemoveCard[3]);

			// 显示手牌;
			for (BYTE __ind = 0; __ind < MAX_INDEX; __ind)
			{
				MyLog("PerformGangPai WIK_AGANG Hand Card_%d = 0x%x", __ind, m_GameLogic.SwitchToCardData(m_cbCardIndex[wChairID][__ind]));
			}
#endif

			return false;
		}

		VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
		m_bSendCardStatus = false;

		//取消此玩家天听状态
		m_bUserTianTing[wChairID] = false;

		//重置听牌和杠牌数据
		ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
		ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

		//加入到自己的组合牌中
		WORD wIndex = m_cbWeaveItemCount[wChairID]++;
		m_WeaveItemArray[wChairID][wIndex].cbPublicCard = FALSE;
		m_WeaveItemArray[wChairID][wIndex].wProvideUser = wChairID;
		m_WeaveItemArray[wChairID][wIndex].dwWeaveKind = dwOperateAction;
		m_WeaveItemArray[wChairID][wIndex].cbCenterCard = cbOperateCard;
		
		//总倍数
		LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

		//发送杠牌小结消息
		CMD_S_PayMoney stPayMoney;
		ZeroMemory(&stPayMoney, sizeof(stPayMoney));
		stPayMoney.cbPayReason = PAY_MONEY_ANGANG;
		stPayMoney.wPayToUser = wChairID;
		
		LONG lAllGangScore = 0;
		BYTE cbFromChairID = 0;

		LONG lGangScore = BASE_HUA_COUNT_AGANG*lPreMultipleCellScore;

		//连续杠牌的规则：玩家A明杠（包含先碰后杠）玩家B的的牌，后面摸牌，这张牌可以暗杠，暗杠后玩家B付三家的钱，在摸牌胡时，也全部算玩家B的牌;
		if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) &&*/ (m_wGangPaiProvideUser != INVALID_CHAIR) && (m_wGangPaiProvideUser != wChairID))
		{
			// 三家钱一家付;
			lGangScore = lGangScore * 3L;

			//计算暗杠分数
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wGangPaiProvideUser);
			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[m_wGangPaiProvideUser][0] + m_lOutCardScore[m_wGangPaiProvideUser][1] + m_lGangScore[m_wGangPaiProvideUser];
			
#if IS_OPEN_PRINT_LOG
			MyLog("扣钱人[%d]实际积分 三家钱一家付 ===================== 暗杠 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
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

			//计算明杠分数
			lAllGangScore += lTmpGangScore;
			m_lGangScore[m_wGangPaiProvideUser] -= lTmpGangScore;

			stPayMoney.wPayFromUser[cbFromChairID] = m_wGangPaiProvideUser;
			stPayMoney.lPayFromNumber[cbFromChairID++] -= lTmpGangScore;
		}
		else
		{
			//设置杠牌状态
			m_cbGangStatus = GS_DAGANGKAIHUA;

			//计算暗杠分数
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i == wChairID) continue;

				IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
				LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];
#if IS_OPEN_PRINT_LOG
				MyLog("扣钱人[%d]实际积分  ===================== 暗杠 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]", 
					i, pServerUserItem->GetUserScore(), m_lOutCardScore[i][0], m_lOutCardScore[i][1], m_lGangScore[i]);
#endif
				LONG lTmpGangScore = 0;
				if (lProvideUserScore >= lGangScore)
				{
					//计算明杠分数
					lTmpGangScore = lGangScore;
				}
				else
				{
					lTmpGangScore = lProvideUserScore;
				}

				//计算明杠分数
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

		// 记录日志
		PayMoneyLog(stPayMoney);

		m_PrivateEndInfo.cbAnGang[wChairID]++;

		// 风杠2花
		if (cbOperateCard >= 0x31 && cbOperateCard <= 0x34)
		{
			m_cbHuaPaiCount[wChairID][1] += 3;
		}
		else
		{// 普通1花
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

	//发送玩家操作消息
	stOperateResult.wOperateUser = wChairID;
	stOperateResult.cbOperateCard = cbOperateCard;
	stOperateResult.dwOperateCode = dwOperateAction;
	stOperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wChairID : m_wProvideUser;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	// 游戏录像
	stOperateResult.cbOperateCard = cbOperateCard;
	addGameRecordAction(SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

	//出牌变量
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;
	//m_cbGangStatus = GS_NULL;

	//清空状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

	//重置抢杠胡状态
	m_QiangGangHuInfo.ReInit();

	m_cbProvideCard = 0;
	m_wCurrentUser = wChairID;

	//两家无积分牌局结束;
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
	
	////自己后面摸牌
	//SendCardData(wChairID, SEND_CARD_BACK);
#if IS_OPEN_PRINT_LOG
	MyLog("PerformGangPai(%d, %d, %d, %s) end", wChairID, dwOperateAction, cbOperateCard, BOOL_TO_STRING(bCheckQiangGangHu));
#endif

	return true;
}

//听牌操作
bool CTableFrameSink::PerformTingPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformTingPai(%d, %d, %d) begin", wChairID, dwOperateAction, cbOperateCard);
#endif

	//数据校验
	if (!m_GameLogic.IsValidUser(wChairID) || (dwOperateAction&WIK_TING) == 0)
	{
		ASSERT(FALSE);
#if IS_OPEN_PRINT_LOG
		MyLog("PerformTingPai error wChairID = %d, dwOperateAction = 0x%x", wChairID, dwOperateAction);
#endif
		return false;
	}

	//听牌判断
	ASSERT(m_TingPaiResult[wChairID].cbTingPaiItemCount > 0);
	if (m_TingPaiResult[wChairID].cbTingPaiItemCount == 0)
	{
#if IS_OPEN_PRINT_LOG
		MyLog("PerformTingPai error wChairID = %d, cbTingPaiItemCount = %d", wChairID, m_TingPaiResult[wChairID].cbTingPaiItemCount);
#endif
		return false;
	}

	//听牌状态判断
	ASSERT(!m_bUserTing[wChairID]);
	if (m_bUserTing[wChairID])
	{
#if IS_OPEN_PRINT_LOG
		MyLog("PerformTingPai m_bUserTing[%d]=%s", wChairID, BOOL_TO_STRING(m_bUserTing[wChairID]));
#endif
		return true;
	}

	//重置杠牌提供者
	m_wGangPaiProvideUser = INVALID_CHAIR;

	//记录玩家已听牌数据
	for (BYTE i = 0; i < m_TingPaiResult[wChairID].cbTingPaiItemCount; i++)
	{
		//如果打出去的牌是检测可出的牌
		if (m_TingPaiResult[wChairID].TingPaiItemArrary[i].cbOutCardData == cbOperateCard)
		{
			//重置听牌和杠牌数据
			ZeroMemory(m_TingPaiResult, sizeof(m_TingPaiResult));
			ZeroMemory(m_GangPaiResult, sizeof(m_GangPaiResult));

			//设置听牌状态
			m_bUserTing[wChairID] = true;

			//发送玩家操作消息
			CMD_S_OperateResult stOperateResult;
			ZeroMemory(&stOperateResult, sizeof(stOperateResult));
			stOperateResult.wOperateUser = wChairID;
			stOperateResult.cbOperateCard = cbOperateCard;
			stOperateResult.dwOperateCode = dwOperateAction;
			stOperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wChairID : m_wProvideUser;
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

			// 游戏录像
			addGameRecordAction(SUB_S_OPERATE_RESULT, &stOperateResult, sizeof(stOperateResult));

			//玩家出牌
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

//胡牌操作
bool CTableFrameSink::PerformHuPai(WORD wChairID, DWORD dwOperateAction, BYTE cbOperateCard, bool bZiMo)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformHuPai(%d, %d, %d, %s) begin", wChairID, dwOperateAction, cbOperateCard, BOOL_TO_STRING(bZiMo));
#endif

	//结束信息
	m_cbHuPaiCard = cbOperateCard;

	BYTE cbHuUserCout = 0;	// 胡牌人数;

	// 自摸
	if (bZiMo)
	{
		//用户校验
		VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
//		if (!m_GameLogic.IsValidUser(wChairID))
//		{
//			ASSERT(0);
//#if IS_OPEN_PRINT_LOG
//			MyLog("PerformHuPai error wChairID = %d", wChairID);
//#endif
//			return false;
//		}

		//操作校验
		VERIFY_RETURN_FALSE((dwOperateAction&WIK_HU) != 0);
		//if (!((dwOperateAction&WIK_HU) != 0))
		//{
		//	ASSERT(0);
		//	MyLog("PerformHuPai error dwOperateAction = %d", dwOperateAction);
		//	return false;
		//}

		//验证胡牌
		DWORD dwPreHuPaiKind = GetPreHuPaiKind(wChairID, m_cbHuPaiCard);
		DWORD dwHuPaiKind = m_GameLogic.AnalyseHuPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], m_cbHuPaiCard, m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, m_HuPaiResult[wChairID], bZiMo);

		// 真胡;
		if (dwHuPaiKind != WIK_NULL)
		{
			if ((m_HuPaiResult[wChairID].dwHuPaiKind&HU_ONLYONE) != 0 && (m_HuPaiResult[wChairID].dwHuPaiKind&HU_YAJUE) == 0)
			{
				m_cbHuaPaiCount[wChairID][1]++;
			}

			//检测是否缺一门
			if (IsQueYiMen(wChairID, m_cbCardIndex[wChairID]))
			{
				m_cbHuaPaiCount[wChairID][1]++;
			}

			// 胡牌;
			if (dwHuPaiKind != WIK_NULL)
			{
				cbHuUserCout++;
			}

			// 自摸对对胡检测包子;自摸清一色检测包子;
			if ((m_HuPaiResult[wChairID].dwHuPaiKind&HU_DUIDUIHU) != 0 || (m_HuPaiResult[wChairID].dwHuPaiKind&HU_QINGYISE) != 0)
			{
				if (IsSanPeng(wChairID))
				{
					m_HuPaiResult[wChairID].dwHuPaiKind |= HU_BAOZI;

					return OnEventGameConclude(wChairID, NULL, GER_BAOZI);
				}
			}
		}
		else// 假胡检测成包子(包子和普通胡牌同时出现，优先包子);
		{
			// 三碰，暗杠胡包子;
			if (IsGameRuleBaoZi())
			{
				DWORD dwUserAction = WIK_NULL;
				tagGangPaiResult GangPaiResult;
				dwUserAction |= m_GameLogic.AnalysePengPai(m_cbCardIndex[wChairID], cbOperateCard);
				dwUserAction |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], cbOperateCard, GangPaiResult, true);

				// 增加包子胡牌检测(如果可碰杠，判断碰杠后是否成包子，如果可以直接提示玩家胡牌);
				if ((dwUserAction & WIK_AGANG) != 0)
				{
					// 已经是对了同一个玩家三嘴了，在对任意一家一嘴，就算胡包子;
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

		//统计抢杠胡胡牌
		if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR && m_QiangGangHuInfo.cbGangPaiCard != 0)
		{
			//ASSERT(m_QiangGangHuInfo.cbGangPaiCard == m_cbHuPaiCard);
			if (m_QiangGangHuInfo.cbGangPaiCard != m_cbHuPaiCard)
			{
				m_cbHuPaiCard = m_QiangGangHuInfo.cbGangPaiCard;
			}
		}

		//胡牌判断
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			//过虑判断
			if ((m_dwPerformAction[i] & WIK_HU) == 0)
			{
				continue;
			}

			// 假胡检测成包子(包子和普通胡牌同时出现，优先包子);
			if (AnalyseBaoZi(i, m_wProvideUser, m_cbHuPaiCard))
			{
				m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbHuPaiCard)]++;

				m_HuPaiResult[i].dwHuPaiKind |= HU_BAOZI;
			}
			else
			{
				//验证胡牌
				DWORD dwPreHuPaiKind = GetPreHuPaiKind(i, m_cbHuPaiCard);

				//胡牌检测
				DWORD dwHuPaiKind = m_GameLogic.AnalyseHuPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], m_cbHuPaiCard, m_cbHuaPaiCount[i][0], dwPreHuPaiKind, m_HuPaiResult[i], bZiMo);
				ASSERT(dwHuPaiKind != WIK_NULL);

				//胡牌，将最后一张牌插入
				if (m_HuPaiResult[i].dwHuPaiKind != HU_NULL)
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbHuPaiCard)]++;

					//计算软花
					if ((m_HuPaiResult[i].dwHuPaiKind&HU_ONLYONE) != 0 && (m_HuPaiResult[i].dwHuPaiKind&HU_YAJUE) == 0)
					{
						m_cbHuaPaiCount[i][1]++;
					}

					//检测是否缺一门
					if (IsQueYiMen(i, m_cbCardIndex[i]))
					{
						m_cbHuaPaiCount[i][1]++;
					}

					// 对对胡包子判断三碰同一家对对胡算包子;
					if ((m_HuPaiResult[i].dwHuPaiKind&HU_DUIDUIHU) != 0)
					{
						if (IsSanPeng(i))
						{
							m_HuPaiResult[i].dwHuPaiKind |= HU_BAOZI;
						}
					}
					
					// 清一色包子判断;
					if (IsGameRuleBaoZi() && (m_HuPaiResult[i].dwHuPaiKind&HU_QINGYISE) != 0)
					{
						if (m_cbWeaveItemCount[i] >= MAX_WEAVE - 1)
						{
							m_HuPaiResult[i].dwHuPaiKind |= HU_BAOZI;
						}
					}

					cbHuUserCout++;		// 胡牌人数统计;

				}
			}
			
			ASSERT(m_HuPaiResult[i].dwHuPaiKind != HU_NULL);
		}
	}

#if IS_OPEN_PRINT_LOG
	// 未找到胡牌玩家;
	if (cbHuUserCout <= 0)
	{
		MyLog("HU wChairID = %d, m_cbHuPaiCard = 0x%x, cbOperateCard = 0x%x, bZiMo = %s", wChairID, m_cbHuPaiCard, cbOperateCard, BOOL_TO_STRING(bZiMo));

		//统计抢杠胡胡牌
		if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR && m_QiangGangHuInfo.cbGangPaiCard != 0)
		{
			MyLog("抢杠胡 wGangPaiUser= %d, cbGangPaiCard = %d", m_QiangGangHuInfo.wGangPaiUser, m_QiangGangHuInfo.cbGangPaiCard);
		}
	}

	MyLog("PerformHuPai end");
#endif

	//结束游戏
	return OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
}

//放弃操作
bool CTableFrameSink::PerformGiveUp(WORD wChairID, DWORD dwOperateCode, BYTE cbOperateCard)
{
#if IS_OPEN_PRINT_LOG
	MyLog("PerformGiveUp(%d, %d, %d) begin.", wChairID, dwOperateCode, cbOperateCard);
#endif

	//数据校验
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

	//捉炮放弃的情况下,玩家这一轮不可以胡牌;
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
		//有抢杠胡玩家放弃胡牌，杠牌玩家继续杠牌
		if (m_QiangGangHuInfo.wGangPaiUser != INVALID_CHAIR)
		{
#if IS_OPEN_PRINT_LOG
			MyLog("PerformGiveUp wGangPaiUser=%d, cbGangPaiCard=0x%x.", m_QiangGangHuInfo.wGangPaiUser, m_QiangGangHuInfo.cbGangPaiCard);
#endif
			PerformGangPai(m_QiangGangHuInfo.wGangPaiUser, WIK_BGANG, m_QiangGangHuInfo.cbGangPaiCard, false);
		}
		else
		{
			//出牌变量
			m_cbOutCardData = 0;
			m_wOutCardUser = INVALID_CHAIR;
			m_cbGangStatus = GS_NULL;

			//清空状态
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
		//出牌变量
		m_cbOutCardData = 0;
		m_wOutCardUser = INVALID_CHAIR;
		m_cbGangStatus = GS_NULL;

		//清空状态
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

//玩家摸牌
bool CTableFrameSink::SendCardData(WORD wChairID, BYTE cbDirection, bool bBanker14/*=false*/)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	
	//荒庄检测
	if (m_cbLeftCardCount <= KEEP_LEFT_CARD_COUNT)
	{
		m_cbHuPaiCard = 0;
		m_wProvideUser = INVALID_CHAIR;
		return OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
	}

	//重置出牌变量
	m_cbOutCardData = 0;
	m_wOutCardUser = INVALID_CHAIR;

	m_bSendCardStatus = true;

	//牌压到玩家手牌中
	m_cbSendCardCount++;

//#ifdef DEBUG
	m_cbSendCardData = getSendCardData(wChairID);
//#else
//	m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
//#endif // DEBUG
	
	m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//玩家手牌数目校验
	BYTE cbCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbCardCount > 0 && cbCardCount <= MAX_COUNT && cbCardCount % 3 == 2);

	//设置当前数值
	m_wCurrentUser = wChairID;
	m_wProvideUser = wChairID;
	m_cbProvideCard = m_cbSendCardData;
	m_wSendCardUser = wChairID;
	ZeroMemory(m_bSendCardOver, sizeof(m_bSendCardOver));

	//发送发牌消息
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

	stSendCard.cbCardData = 0x0;	// 旁观发
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_SEND_CARD, &stSendCard, sizeof(stSendCard));

#if IS_OPEN_PRINT_LOG
	MyLog("wChairID = %d, cbSendCardData = %d", wChairID, m_cbSendCardData);
#endif

	// 游戏录像
	stSendCard.cbCardData = m_cbSendCardData;
	addGameRecordAction(SUB_S_SEND_CARD, &stSendCard, sizeof(stSendCard));

	//启动摸牌动画定时器
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

//发送操作
bool CTableFrameSink::SendOperateNotify(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	bool bUserHaveOperate = false;
	//发送提示
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_dwUserAction[i] != WIK_NULL)
		{
			//给玩家发送吃碰杠听胡消息
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

			//听牌数据
			if ((stOperateNotify.dwActionMask & WIK_TING) != 0)
			{
				//m_TingPaiResult[i].StreamValue(dataStream, true);
				tagTingPaiDataStream TingPaiResult(m_TingPaiResult[i]);
				TingPaiResult.StreamValue(databuff, true);
			}

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());

			//发送数据
			//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &stOperateNotify, sizeof(stOperateNotify));
			//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_NOTIFY, &stOperateNotify, sizeof(stOperateNotify));
			bUserHaveOperate = IsHaveOperateCard(m_dwUserAction[i]);

#if IS_OPEN_PRINT_LOG
			MyLog("SendOperateNotify info wOperateUser=%d, dwActionMask=%d, wCurrentUser=%d", stOperateNotify.wOperateUser, stOperateNotify.dwActionMask, stOperateNotify.wCurrentUser);
#endif
			// 游戏录像
			addGameRecordAction(SUB_S_OPERATE_NOTIFY, &databuff[0], databuff.size());
		}
	}

	if (bUserHaveOperate)
	{
		BYTE cbTimerStyle = IDS_TIMER_NORMAL; //正常状态
		BYTE cbTimerLength = IDT_USER_OPERATE;
		if (m_GameLogic.IsValidUser(m_wCurrentUser))
		{
			ASSERT(m_wCurrentUser == wChairID);
			//托管状态
			if (m_bUserTrust[m_wCurrentUser])
			{
				cbTimerStyle = IDS_TIMER_TRUST;
				cbTimerLength = IDT_USER_TRUST;
			}

			//不是私人场;
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
		//判定当前用户合法
		ASSERT(m_GameLogic.IsValidUser(m_wCurrentUser) && m_wCurrentUser == wChairID);
		BYTE cbTimerStyle = IDS_TIMER_NORMAL;
		BYTE cbTimerLength = IDT_OUT_CARD;
		if (m_GameLogic.IsValidUser(m_wCurrentUser))
		{
			//托管状态
			if (m_bUserTrust[m_wCurrentUser])
			{
				cbTimerStyle = IDS_TIMER_TRUST;
				cbTimerLength = IDT_USER_TRUST;
			}
			//听牌状态
			else if (m_bUserTing[m_wCurrentUser])
			{
				cbTimerStyle = IDS_TIMER_TING;
				cbTimerLength = IDT_USER_TING;
			}
		}

		// 不是私人场;
		if (m_pGameServiceOption->wServerType != GAME_GENRE_EDUCATE && m_pGameServiceOption->wServerType != GAME_GENRE_PRIVATE)
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD, cbTimerLength * 1000, 1, (cbTimerStyle << 16) | wChairID);
		}
	}

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wChairID, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//用户状态
	ZeroMemory(m_bResponse, sizeof(m_bResponse));
	ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
	ZeroMemory(m_dwUserAction, sizeof(m_dwUserAction));
	ZeroMemory(m_dwPerformAction, sizeof(m_dwPerformAction));

	//变量定义
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

	//结果处理
	if (bAroseAction)
	{
		// 出牌，杠牌
		if (EstimatKind == EstimatKind_OutCard || EstimatKind == EstimatKind_BuGangCard)
		{
			m_wResumeUser = m_wCurrentUser;
			m_wCurrentUser = INVALID_CHAIR;
		}

		// 不是天听不报听
		if ((m_bUserTianTing[wChairID] == FALSE) && ((m_dwUserAction[wChairID] & WIK_TING) != 0))
		{
			m_dwUserAction[wChairID] &= ~WIK_TING;
		}

		//发送提示
		SendOperateNotify(wChairID, cbCenterCard, EstimatKind);

		return true;
	}

	return false;
}

//检查自己的杠、听、胡操作
bool CTableFrameSink::EstimatSendCard(WORD wChairID, BYTE cbSendCard)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//玩家手牌数目校验
	BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
	ASSERT(cbHandCardCount > 0 && cbHandCardCount <= MAX_COUNT && cbHandCardCount % 3 == 2);

	//胡牌检测
	DWORD dwPreHuPaiKind = GetPreHuPaiKind(wChairID, cbSendCard);

	// 检测背杠胡(提供者积分为0，不胡);
	if (/*(m_cbGangStatus == GS_DAGANGKAIHUA) &&*/ (m_wGangPaiProvideUser != INVALID_CHAIR) && (m_wGangPaiProvideUser != wChairID))
	{
		//计算暗杠分数
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

	//非听牌状态下，检测听牌和杠牌
	if (!m_bUserTing[wChairID])
	{
		dwPreHuPaiKind &= HU_XGKAIHUA;
		dwPreHuPaiKind &= HU_DGKAIHUA;

		//听牌检测
		m_dwUserAction[wChairID] |= m_GameLogic.AnalyseTingPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], \
			m_cbHuaPaiCount[wChairID][0], dwPreHuPaiKind, m_TingPaiResult[wChairID]);

		if ((m_dwUserAction[wChairID] & WIK_TING) != 0)
		{
			//加入剩余牌数量
			GetTingCardLastCount(m_TingPaiResult[wChairID], wChairID);

			//判断是否只听无花果
			if (IsOnlyHuWuHuaGuo(dwPreHuPaiKind))
			{
				m_dwUserAction[wChairID] &= ~WIK_TING;
			}
		}

		//杠牌检测
		if (m_cbLeftCardCount > KEEP_LEFT_CARD_COUNT)
		{
			m_dwUserAction[wChairID] |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], \
				cbSendCard, m_GangPaiResult[wChairID], true);

			// 增加包子胡牌检测(如果可碰杠，判断碰杠后是否成包子，如果可以直接提示玩家胡牌)自摸只有暗杠;
			if ((m_dwUserAction[wChairID] & WIK_AGANG) != 0)
			{
				// 已经是对了同一个玩家三嘴了，在对任意一家一嘴，就算胡包子;
				if (IsSanPeng(wChairID))
				{
					m_dwUserAction[wChairID] = WIK_HU;
				}
			}
		}
	}
	
	//加上出牌
	m_dwUserAction[wChairID] |= WIK_CHU;

	return true;
}

//检查其他玩家的吃、碰、杠、胡操作
bool CTableFrameSink::EstimatOutCard(WORD wChairID, BYTE cbCenterCard)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	BYTE cbLastCardData = 0x0;

	bool bAroseAction = false;

	// 获取出牌玩家积分
	IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONG lOutScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wChairID][0] + m_lOutCardScore[wChairID][1] + m_lGangScore[wChairID];

	//动作判断
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//用户过滤,自己不检测
		if (wChairID == i) continue;

		//玩家手牌数目校验
		BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
		ASSERT(cbHandCardCount > 0 && cbHandCardCount <= MAX_COUNT);

		// 胡牌检测
		if (!m_bInvalidHuStates[i])
		{
			DWORD dwPreHuPaiKind = GetPreHuPaiKind(i, cbCenterCard);
			tagHuPaiResult HuPaiResult;
			m_dwUserAction[i] |= m_GameLogic.AnalyseHuPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], cbCenterCard, m_cbHuaPaiCount[i][0], dwPreHuPaiKind, HuPaiResult, false);

			// 点炮玩家积分为0;
			if (lOutScore <= 0 && ((m_dwUserAction[i] & WIK_HU) == WIK_HU))
			{
				m_dwUserAction[i] &= ~WIK_HU;
				m_bInvalidHuStates[i] = true;
			}
		}

		//玩家非听牌状态下，检测碰牌和杠牌
		if (!m_bUserTing[i])
		{
			//碰牌检测
			if (cbCenterCard != m_cbInvalidOperteStates[i])
			{
				m_dwUserAction[i] |= m_GameLogic.AnalysePengPai(m_cbCardIndex[i], cbCenterCard);
			}

			//杠牌检测
			if (m_cbLeftCardCount > KEEP_LEFT_CARD_COUNT)
			{
				m_dwUserAction[i] |= m_GameLogic.AnalyseGangPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], cbCenterCard, m_GangPaiResult[i], false);
			}

			// 增加包子胡牌检测(如果可碰杠，判断碰杠后是否成包子，如果可以直接提示玩家胡牌)出牌无暗杠;
			if ((m_dwUserAction[i] & (WIK_PENG | WIK_MGANG)) != 0)
			{
				// 已经是对了同一个玩家三嘴了，在对任意一家一嘴，就算胡包子;
				if (IsSanPeng(i))
				{
					m_dwUserAction[i] = WIK_HU;
				}

				// 四碰清一色，算包子;
				if (IsSiPengQingyise(i, cbCenterCard, m_dwUserAction[i]))
				{
					m_dwUserAction[i] = WIK_HU;
				}
			}
		}

		//结果判断
		if (m_dwUserAction[i] != WIK_NULL)
		{
			bAroseAction = true;
		}
	}

	return bAroseAction;
}

//检查碰后的听操作(碰后检测胡包子)
bool CTableFrameSink::EstimatChiPengCard(WORD wChairID, BYTE cbCenterCard)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//加上出牌
	m_dwUserAction[wChairID] |= WIK_CHU;

	return true;
}

// 杠检测包子(杠后检测胡包子);
bool CTableFrameSink::EstimatGangCard(WORD wChairID, BYTE cbCenterCard)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	return false;
}

//检查补杠后其他玩家可否胡（抢杠胡）
bool CTableFrameSink::EstimatBuGangCard(WORD wChairID, BYTE cbCenterCard)
{
	//重置抢杠胡状态
	m_QiangGangHuInfo.ReInit();

	IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	LONG lPracticeScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wChairID][0] + m_lOutCardScore[wChairID][1] + m_lGangScore[wChairID];

	bool bAroseAction = false;
	BYTE cbCount = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//用户过滤
		if (wChairID == i) continue;

		//胡牌判断
		DWORD dwPreHuPaiKind = GetPreHuPaiKind(i, cbCenterCard);
		tagHuPaiResult HuPaiResult;
		m_dwUserAction[i] |= m_GameLogic.AnalyseHuPai(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], cbCenterCard, m_cbHuaPaiCount[i][0], dwPreHuPaiKind, HuPaiResult, false);

		// 点炮玩家积分为0;
		if (lPracticeScore <= 0 && ((m_dwUserAction[i] & WIK_HU) == WIK_HU))
		{
			m_dwUserAction[i] &= ~WIK_HU;
			m_bInvalidHuStates[i] = true;
		}

		//结果判断
		if ((m_dwUserAction[i] & WIK_HU) != 0)
		{
			bAroseAction = true;
			ASSERT(cbCount < CountArray(m_QiangGangHuInfo.wQiangGangHuUser));
			m_QiangGangHuInfo.wQiangGangHuUser[cbCount++] = i;
		}
	}

	return bAroseAction;
}

//检查玩家补花
bool CTableFrameSink::EstimatUserBuHua(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	m_wBuHuaUser = INVALID_CHAIR;
	ZeroMemory(m_bBuHuaOver, sizeof(m_bBuHuaOver));

	//起手补花完成
	if (IsFirstBuHuaOver())
	{
		BYTE cbHuaCardCount = 0;
		BYTE cbHuaCardData[MAX_HUA_COUNT] = { 0 };
		cbHuaCardCount = m_GameLogic.DistillHardHuaCard(m_cbCardIndex[wChairID], cbHuaCardData);
		ASSERT(cbHuaCardCount <= 1);

		//需要补花
		if (cbHuaCardCount > 0)
		{
			//直接流局
			if (m_cbLeftCardCount - cbHuaCardCount < KEEP_LEFT_CARD_COUNT)
			{
				m_cbHuPaiCard = 0;
				m_wProvideUser = INVALID_CHAIR;
				return OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);
			}

			//删除花牌
			if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbHuaCardData, cbHuaCardCount))
			{
				ASSERT(FALSE);
				return false;
			}
			VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);

			//发送补花消息
			CMD_S_SendBuHua stSendBuHua;
			ZeroMemory(&stSendBuHua, sizeof(stSendBuHua));
			stSendBuHua.wBuHuaUser = wChairID;
			stSendBuHua.bFirstBuHua = false;
			stSendBuHua.cbBuHuaCount = cbHuaCardCount;

			CopyMemory(stSendBuHua.cbBuHuaData, cbHuaCardData, sizeof(BYTE)*cbHuaCardCount);

			BYTE	cbReplaceCardData[MAX_COUNT];				//替换的牌录像存储
			ZeroMemory(cbReplaceCardData, CountArray(cbReplaceCardData));

			for (int i = 0; i < GAME_PLAYER; i++)
			{
				// 只有需要补花的用户可活动具体的补花数据
				if (i == wChairID)
				{
					// 花牌数据替换
					for (BYTE j = 0; j < cbHuaCardCount; j++)
					{
						m_cbSendCardCount++;
						m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
						m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
						VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
						stSendBuHua.cbReplaceCardData[j] = m_cbSendCardData;

						//记录花牌数据
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

			// 游戏录像
			CopyMemory(stSendBuHua.cbReplaceCardData, cbReplaceCardData, CountArray(stSendBuHua.cbReplaceCardData));
			addGameRecordAction(SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));

			//检测花杠得分
			CheckHuaGangScore(wChairID);

			//设置当前用户、提供用户和提供牌
			m_cbProvideCard = m_cbSendCardData;
			m_wProvideUser = wChairID;
			m_wCurrentUser = wChairID;

			//设置花杠状态
			m_cbGangStatus = GS_XIAOGANGKAIHUA;

			//启动补花延时定时器
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
			//检测手牌数据
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
			//手牌数目校验
			BYTE cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
			ASSERT(cbHandCardCount > 0 && cbHandCardCount <= MAX_COUNT);
			//删除花牌
			if (!m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], cbHuaCardData, cbHuaCardCount))
			{
				ASSERT(FALSE);
				return false;
			}
			//手牌数目校验
			cbHandCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
			ASSERT(cbHandCardCount >= 0 && cbHandCardCount <= MAX_COUNT);

			//发送补花消息
			CMD_S_SendBuHua stSendBuHua;
			ZeroMemory(&stSendBuHua, sizeof(stSendBuHua));
			stSendBuHua.wBuHuaUser = wChairID;
			stSendBuHua.bFirstBuHua = true;
			stSendBuHua.cbBuHuaCount = cbHuaCardCount;

			CopyMemory(stSendBuHua.cbBuHuaData, cbHuaCardData, sizeof(BYTE)*cbHuaCardCount);

			BYTE	cbReplaceCardData[MAX_COUNT];				//替换的牌录像存储
			ZeroMemory(cbReplaceCardData, CountArray(cbReplaceCardData));

			BYTE cbSendCardData = 0x0;
				
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				// 只有需要补花的用户可活动具体的补花数据
				if (i == wChairID)
				{
					// 花牌数据替换
					for (BYTE j = 0; j < cbHuaCardCount; j++)
					{
						m_cbSendCardCount++;
						//m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];
						cbSendCardData = getBuHuaCardData(wChairID);
						m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(cbSendCardData)]++;
						VERIFY(m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]) <= MAX_COUNT);
						stSendBuHua.cbReplaceCardData[j] = cbSendCardData;

						// 记录当前玩家的手牌;
						if (i == m_wCurrentUser)
						{
							m_cbSendCardData = cbSendCardData;
						}

						//记录花牌数据
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

			// 游戏录像
			CopyMemory(stSendBuHua.cbReplaceCardData, cbReplaceCardData, CountArray(stSendBuHua.cbReplaceCardData));
			addGameRecordAction(SUB_S_SEND_BUHUA, &stSendBuHua, sizeof(stSendBuHua));

			//庄家首轮补花时，设置花杠状态
			if (wChairID == m_wBankerUser)
			{
				m_wCurrentUser = wChairID;
				m_wProvideUser = wChairID;
				m_cbProvideCard = m_cbSendCardData;
				m_cbGangStatus = GS_XIAOGANGKAIHUA;
			}

			//检测花杠得分
			CheckHuaGangScore(wChairID);

			//启动补花延时定时器
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
			//该玩家不需要补花
			m_bFirstBuHuaOver[wChairID] = true;

			//检测闲家天听
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

			//下家继续补花
			WORD wNextBuHuaUser = (wChairID + GAME_PLAYER - 1) % GAME_PLAYER;
			if (wNextBuHuaUser != m_wBankerUser)
			{
				//检测下家补花
				EstimatUserBuHua(wNextBuHuaUser);
				return true;
			}
			else
			{
				//检测手牌数据
				EstimateUserRespond(m_wBankerUser, m_cbProvideCard, EstimatKind_SendCard);
				return true;
			}
		}
	}

	return false;
}

//检查玩家能否换听
bool CTableFrameSink::CheckChangeTingPai(WORD wChairID, tagTingPaiResult TingPaiResult)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//先前没听过
	if (m_HaveTingPaiResult[wChairID].cbTingCardCount == 0)
	{
		return true;
	}
	
	//获取新的听牌数据
	for (BYTE i = 0; i < TingPaiResult.cbTingPaiItemCount; i++)
	{
		for (BYTE j = 0; j < TingPaiResult.TingPaiItemArrary[i].cbTingCardCount; j++)
		{
			//如果新检测出的牌不在已听牌中，可以换听
			if (!m_GameLogic.IsCardInArrary(m_HaveTingPaiResult[wChairID].cbTingCardData, m_HaveTingPaiResult[wChairID].cbTingCardCount, TingPaiResult.TingPaiItemArrary[i].cbTingCardData[j]))
			{
				return true;
			}
		}
	}

	return false;
}

//检查花杠得分
void CTableFrameSink::CheckHuaGangScore(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	//统计花牌数量
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

	//总倍数
	LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

	//发送小结消息
	CMD_S_PayMoney stPayMoney;
	ZeroMemory(&stPayMoney, sizeof(stPayMoney));
	stPayMoney.wPayToUser = wChairID;

	//判断花杠状态
	for (BYTE i = 0; i < 5; i++)
	{
		//四个中、发、白 花杠 算分
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
			//一组春、夏、秋、冬花杠 算分
			if (cbHuaPaiCount[3] > 0 && cbHuaPaiCount[4] > 0 && cbHuaPaiCount[5] > 0 && cbHuaPaiCount[6] > 0 && !m_bHuaGangStatus[wChairID][i])
			{
				CountPayMoneyScore(stPayMoney, PAY_MONEY_HUAGANG, BASE_HUA_COUNT_HGANG);
				m_bHuaGangStatus[wChairID][i] = true;
			}
		}
		else if (i == 4)
		{
			//一组梅、兰、竹、菊花杠 算分
			if (cbHuaPaiCount[7] > 0 && cbHuaPaiCount[8] > 0 && cbHuaPaiCount[9] > 0 && cbHuaPaiCount[10] > 0 && !m_bHuaGangStatus[wChairID][i])
			{
				CountPayMoneyScore(stPayMoney, PAY_MONEY_HUAGANG, BASE_HUA_COUNT_HGANG);
				m_bHuaGangStatus[wChairID][i] = true;
			}
		}
	}

	ASSERT(stPayMoney.lPayToNumber + stPayMoney.lPayFromNumber[0] + stPayMoney.lPayFromNumber[1] + stPayMoney.lPayFromNumber[2] == 0);

	//发送花杠小结
	if (stPayMoney.lPayToNumber > 0)
	{
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

		// 记录日志
		PayMoneyLog(stPayMoney);

		//两家无积分牌局结束;
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

//计算花杠得分
void CTableFrameSink::CountPayMoneyScore(CMD_S_PayMoney &stPayMoney, BYTE cbPayType, LONG lBaseHuaCount)
{
	//总倍数
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
		MyLog("扣钱人[%d]实际积分  ===================== 花杠 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
			j, pServerUserItem->GetUserScore(), m_lOutCardScore[j][0], m_lOutCardScore[j][1], m_lGangScore[j]);
#endif
		
		if (lProvideUserScore >= lGangScore)
		{
			//计算明杠分数
			lAllGangScore += lGangScore;
			m_lGangScore[j] -= lGangScore;

			stPayMoney.wPayFromUser[cbFromChairID] = j;
			stPayMoney.lPayFromNumber[cbFromChairID++] -= lGangScore;
		}
		else
		{
			//计算明杠分数
			lAllGangScore += lProvideUserScore;
			m_lGangScore[j] -= lProvideUserScore;

			stPayMoney.wPayFromUser[cbFromChairID] = j;
			stPayMoney.lPayFromNumber[cbFromChairID++] -= lProvideUserScore;
		}
	}

	m_lGangScore[stPayMoney.wPayToUser] += lAllGangScore;	// 获得积分统计;
	stPayMoney.lPayToNumber += lAllGangScore;				// 获得积分;

	m_bHuaGang = true;

	return;
}

//检查出牌得分
void CTableFrameSink::CheckOutCardScore(WORD wChairID)
{
	// 首四风;
	if (!IsGameRuleBaoZi())
	{
		CheckShouSiFeng(wChairID);
	}

	// 梅四张;
	CheckMeiSiZhang(wChairID);

	// 四连发;
	CheckSiLianFa(wChairID);

	//两家无积分牌局结束;
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

// 首四风;
void CTableFrameSink::CheckShouSiFeng(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	//检测奖分
	if (m_cbDiscardRecordCount[wChairID] == 4)
	{
		bool bHaveDNXB = (m_cbDiscardRecordCard[wChairID][0] == 0x31) && (m_cbDiscardRecordCard[wChairID][1] == 0x32) && (m_cbDiscardRecordCard[wChairID][2] == 0x33) && (m_cbDiscardRecordCard[wChairID][3] == 0x34);
		bool bHaveBXND = (m_cbDiscardRecordCard[wChairID][0] == 0x34) && (m_cbDiscardRecordCard[wChairID][1] == 0x33) && (m_cbDiscardRecordCard[wChairID][2] == 0x32) && (m_cbDiscardRecordCard[wChairID][3] == 0x31);

		//前四张是东、南、西、北
		if ((bHaveDNXB || bHaveBXND) && !m_bAwardStatus[wChairID])
		{
			LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

			//设置状态
			m_bHaveJiangFen = true;
			m_bAwardStatus[wChairID] = true;

			CMD_S_PayMoney stPayMoney;
			ZeroMemory(&stPayMoney, sizeof(stPayMoney));
			stPayMoney.cbPayReason = PAY_MONEY_SHOUSIFENG;
			stPayMoney.wPayToUser = wChairID;

			BYTE cbFromChairID = 0;
			LONG lAllScore = 0;
			LONG lFengScore = BASE_HUA_COUNT_JIANGFEN_BX*lPreMultipleCellScore;		// 单个人扣分;

			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				if (i != wChairID)
				{
					IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
					LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[i][0] + m_lOutCardScore[i][1] + m_lGangScore[i];

#if IS_OPEN_PRINT_LOG
					MyLog("扣钱人[%d]实际积分  ===================== 首四风 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
						i, pServerUserItem->GetUserScore(), m_lOutCardScore[i][0], m_lOutCardScore[i][1], m_lGangScore[i]);
#endif

					if (lProvideUserScore >= lFengScore)
					{
						//出牌罚分
						lAllScore += lFengScore;
						m_lOutCardScore[i][0] -= lFengScore;

						stPayMoney.wPayFromUser[cbFromChairID] = i;
						stPayMoney.lPayFromNumber[cbFromChairID++] -= lFengScore;
					}
					else
					{
						//出牌罚分
						lAllScore += lProvideUserScore;
						m_lOutCardScore[i][0] -= lProvideUserScore;

						stPayMoney.wPayFromUser[cbFromChairID] = i;
						stPayMoney.lPayFromNumber[cbFromChairID++] -= lProvideUserScore;
					}
				}
			}

			m_lOutCardScore[wChairID][0] += lAllScore;	// 得分累计
			stPayMoney.lPayToNumber += lAllScore;		// 得分

			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_PAY_MONEY, &stPayMoney, sizeof(stPayMoney));

			// 记录日志
			PayMoneyLog(stPayMoney);

		}
	}
}

// 梅四张;
void CTableFrameSink::CheckMeiSiZhang(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	//检测罚分(出四张一样的牌)1个人出4张同样的牌（不一定要连续），罚给其他三人每人10支花。
	for (BYTE i = 0; i < MAX_NORMAL; i++)
	{
		//如果出了四张一样的牌罚分
		if (m_cbDiscardIndexCount[wChairID][i] == 4 && !m_bPenaltyStatus[wChairID][i])
		{
			LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

			//总倍数
			IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			// 扣分人实际积分;
			LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[wChairID][0] + m_lOutCardScore[wChairID][1] + m_lGangScore[wChairID];

#if IS_OPEN_PRINT_LOG
			MyLog("扣钱人[%d]实际积分  ===================== 梅四张 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
				wChairID, pServerUserItem->GetUserScore(), m_lOutCardScore[wChairID][0], m_lOutCardScore[wChairID][1], m_lGangScore[wChairID]);
#endif

			// 得分人每个人得分;
			LONG lScore = BASE_HUA_COUNT_JIANGFEN_BX * lPreMultipleCellScore;

			// 扣分人实际积分大于应扣积分，才进行扣分处理;
			if (lProvideUserScore >= lScore * 3L)
			{
				//设置罚分状态
				m_bPenaltyStatus[wChairID][i] = true;

				// 扣分人扣分;
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

				// 记录日志
				PayMoneyLog(stPayMoney);
			}

			m_bHaveFaFen = true;

			break;
		}
	}
}

// 四连发;
void CTableFrameSink::CheckSiLianFa(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));
	
	//检测罚分(4人连续出一样的牌)第一个出牌的人罚给其他三人每人10支花。
	if (m_cbOutSameCardCount == 4)
	{
		LONG lPreMultipleCellScore = m_pITableFrame->GetCellScore()*(m_bLastRoundBiXiaHu ? 2L : 1L);

		//用户校验
		VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(m_wFirstOutUserForFaFen));

		//总倍数
		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(m_wFirstOutUserForFaFen);
		LONG lProvideUserScore = (LONG)pServerUserItem->GetUserScore() + m_lOutCardScore[m_wFirstOutUserForFaFen][0]
			+ m_lOutCardScore[m_wFirstOutUserForFaFen][1] + m_lGangScore[m_wFirstOutUserForFaFen];

#if IS_OPEN_PRINT_LOG
		MyLog("扣钱人[%d]实际积分  ===================== 四连发 UserScore = %lld, m_lOutCardScore[0]:[%ld], m_lOutCardScore[0]:[%ld], m_lGangScore:[%ld]",
			m_wFirstOutUserForFaFen, pServerUserItem->GetUserScore(), m_lOutCardScore[m_wFirstOutUserForFaFen][0], m_lOutCardScore[m_wFirstOutUserForFaFen][1], m_lGangScore[m_wFirstOutUserForFaFen]);
#endif
		LONG lScore = BASE_HUA_COUNT_JIANGFEN_BX * lPreMultipleCellScore;

		if (lProvideUserScore >= lScore * 3L)
		{
			//设置罚分状态
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

			// 记录日志
			PayMoneyLog(stPayMoney);
		}

		m_bHaveFaFen = true;
	}
}

//检测胡牌是否有包牌
void CTableFrameSink::CheckHaveBaoPaiUser(WORD wChairID)
{
	////用户校验
	//VERIFY_RETURN_VOID(m_GameLogic.IsValidUser(wChairID));

	////去掉胡牌者的包牌状态
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

//获取玩家可操作的牌
BYTE CTableFrameSink::GetOperateCard(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	BYTE cbProvideCard = 0;

	//有提供牌
	if (m_cbProvideCard != 0)
	{
		cbProvideCard = m_cbProvideCard;
	}
	else
	{
		//选手牌中最后一张
		for (int i = MAX_INDEX - 1; i >= 0; --i)
		{
			if (m_cbCardIndex[wChairID][i] > 0)
			{
				cbProvideCard = m_GameLogic.SwitchToCardData(i);
				break;
			}
		}
	}

	// 不能出花牌
	ASSERT((cbProvideCard > 0x0) && (cbProvideCard < 0x35));

	return cbProvideCard;
}

//获取玩家先前胡牌类型
DWORD CTableFrameSink::GetPreHuPaiKind(WORD wChairID, BYTE cbHuPaiCard)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	DWORD dwPreHuPaiKind = 0;
	//检测天胡
	if (IsTianHu(wChairID))
	{
		dwPreHuPaiKind |= HU_TIANHU;
	}
	//检测地胡
	if (IsDiHu(wChairID))
	{
		dwPreHuPaiKind |= HU_DIHU;
	}
	//检测小杠开花
	if (IsXiaoGangKaiHua(wChairID))
	{
		dwPreHuPaiKind |= HU_XGKAIHUA;
	}
	//检测大杠开花
	if (IsDaGangKaiHua(wChairID))
	{
		dwPreHuPaiKind |= HU_DGKAIHUA;
	}
	//检测压绝
	if (IsYaJue(wChairID, cbHuPaiCard))
	{
		dwPreHuPaiKind |= HU_YAJUE;
	}
	//检测无花果
	if (IsWuHuaGuo(wChairID))
	{
		dwPreHuPaiKind |= HU_WUHUAGUO;
	}

	return dwPreHuPaiKind;
}

//获取听牌时剩余牌数量
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

//获取牌数目(包括打出去的牌，吃、碰、明杠的牌)
int CTableFrameSink::GetCardVisualCount(const BYTE cbCardData, const WORD wChairID)
{
	BYTE cbCardCount = 0;

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//打出去的牌
		for (BYTE j = 0; j < m_cbDiscardCount[i]; j++)
		{
			if (m_cbDiscardCard[i][j] != 0 && m_cbDiscardCard[i][j] == cbCardData)
			{
				cbCardCount++;
			}
		}
		//吃、碰、明杠的牌
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
	//去掉自己手中的牌
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

//自摸
bool CTableFrameSink::IsZiMo(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_wCurrentUser != INVALID_CHAIR && wChairID == m_wProvideUser)
	{
		return true;
	}
	return false;
}

//天胡
bool CTableFrameSink::IsTianHu(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbSendCardCount == 1 && m_cbOutCardCount == 0 && IsZiMo(wChairID) && wChairID == m_wBankerUser)
	{
		return true;
	}
	return false;
}

//地胡
bool CTableFrameSink::IsDiHu(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_bUserTianTing[wChairID])
	{
		return true;
	}
	return false;
}

//无花果
bool CTableFrameSink::IsWuHuaGuo(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbHuaPaiCount[wChairID][0] == 0)
	{
		return true;
	}
	return false;
}

//压绝
bool CTableFrameSink::IsYaJue(WORD wChairID, BYTE cbHuPaiCard)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (cbHuPaiCard == 0) return false;
	bool bYaJue = false;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		for (BYTE j = 0; j < m_cbWeaveItemCount[i]; j++)
		{
			//有玩家碰过此张牌
			if ((m_WeaveItemArray[i][j].dwWeaveKind == WIK_PENG) && (m_WeaveItemArray[i][j].cbCenterCard == cbHuPaiCard))
			{
				bYaJue = true;
				break;
			}
		}
	}
	return bYaJue;
}

//海底捞月
bool CTableFrameSink::IsHaiDiLaoYue(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));
	if (IsZiMo(wChairID) && m_cbLeftCardCount <= KEEP_LEFT_CARD_COUNT)
	{
		return true;
	}
	return false;
}

//小杠开花
bool CTableFrameSink::IsXiaoGangKaiHua(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbGangStatus == GS_XIAOGANGKAIHUA && IsZiMo(wChairID))
	{
		return true;
	}
	return false;
}

//大杠开花
bool CTableFrameSink::IsDaGangKaiHua(WORD wChairID)
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	if (m_cbGangStatus == GS_DAGANGKAIHUA && IsZiMo(wChairID))
	{
		return true;
	}
	return false;
}

//抢杠胡
bool CTableFrameSink::IsQiangGangHu(WORD wChairID)
{
	//用户校验
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

//比下胡
bool CTableFrameSink::IsBiXiaHu()
{
	if (!m_bChangeBanker || m_bHaveJiangFen || m_bHaveFaFen || m_bHaveBigHu || m_bHuaGang || m_bHaveBaozi)
	{
		return true;
	}

	return false;
}

//是否有高级操作
bool CTableFrameSink::IsHaveOperateCard(DWORD dwOperateMask)
{
	if ((dwOperateMask&WIK_CHI) != 0 || (dwOperateMask&WIK_PENG) != 0 || (dwOperateMask&WIK_GANG) != 0 || \
		(dwOperateMask&WIK_TING) != 0 || (dwOperateMask&WIK_TIAOZHAN) != 0 || (dwOperateMask&WIK_HU) != 0)
	{
		return true;
	}
	return false;
}

//是否是缺一门
bool CTableFrameSink::IsQueYiMen(WORD wChairID, BYTE cbCardIndex[MAX_INDEX])
{
	//用户校验
	VERIFY_RETURN_FALSE(m_GameLogic.IsValidUser(wChairID));

	//定义变量
	bool bCardColor[4] = { false, false, false, false };

	//手牌
	for (BYTE i = 0; i < MAX_NORMAL; i++)
	{
		if (cbCardIndex[i] > 0)
		{
			BYTE cbCardData = m_GameLogic.SwitchToCardData(i);
			bCardColor[m_GameLogic.GetCardColor(cbCardData)] = true;
		}
	}
	//组合牌
	for (BYTE i = 0; i < m_cbWeaveItemCount[wChairID]; i++)
	{
		BYTE cbCardData = m_WeaveItemArray[wChairID][i].cbCenterCard;
		bCardColor[m_GameLogic.GetCardColor(cbCardData)] = true;
	}
	//统计
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

//是否只胡无花果
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

//检测挑战分数
void CTableFrameSink::CheckTiaoZhanScore(WORD wChairID, LONG lTiaoZhanScore[GAME_PLAYER])
{
	//lTiaoZhanScore[wChairID] += m_TiaoZhanResult[wChairID].lTiaoZhanScore;
	//for (WORD i = 0; i < m_TiaoZhanResult[wChairID].cbCurUserIndex; i++)
	//{
	//	lTiaoZhanScore[m_TiaoZhanResult[wChairID].stTiaoZhanItem[i].wChairID] += m_TiaoZhanResult[wChairID].stTiaoZhanItem[i].lTiaoZhanScore;
	//}
}

//三碰;
bool CTableFrameSink::IsSanPeng(WORD wChairID)
{
	if (IsGameRuleBaoZi() == false)
	{
		return false;
	}

	tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
	if (pWeaveItem == nullptr) return false;

	BYTE cbItemCount = m_cbWeaveItemCount[wChairID];

	// 检测三碰;
	if (cbItemCount == MAX_WEAVE - 1)
	{
		// 是否有暗杠;
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
				return true;	// 三碰
			}
		}
	}

	return false;
}

//四碰清一色;
bool CTableFrameSink::IsSiPengQingyise(WORD wChairID, BYTE cbCenterCard, DWORD dwUserAction)
{
	if (IsGameRuleBaoZi() == false)
	{
		return false;
	}

	// 增加包子胡牌检测(如果可碰杠，判断碰杠后是否成包子，如果可以直接提示玩家胡牌);
	if ((dwUserAction & (WIK_PENG | WIK_MGANG | WIK_AGANG)) != 0)
	{
		tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
		if (pWeaveItem == nullptr) return false;

		BYTE cbItemCount = m_cbWeaveItemCount[wChairID];

		// 手中必须有三嘴;
		if (m_cbWeaveItemCount[wChairID] == MAX_WEAVE - 1)
		{
			bool bCardColor[3] = { false, false, false };
			BYTE cbCardColor[3] = { 0 };

			//组合牌
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

			// 预计算放入统计;
			if (m_GameLogic.IsZiCard(cbCenterCard))
			{
				return false;
			}
			else
			{
				cbCardColor[m_GameLogic.GetCardColor(cbCenterCard)]++;
			}

			//统计
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

//包子提供者;
WORD CTableFrameSink::GetBaoziProvideUser(WORD wChairID)
{
	tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
	if (pWeaveItem == nullptr) return false;

	BYTE cbItemCount = m_cbWeaveItemCount[wChairID];

	// 检测三碰;
	if (cbItemCount >= MAX_WEAVE - 1)
	{
		// 是否有暗杠;
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
				return j;	// 三碰
			}
		}
	}
	
	return INVALID_CHAIR;
}

////是否有菜包子
//bool CTableFrameSink::IsBaoZi(WORD wChairID)
//{
//	tagWeaveItem* pWeaveItem = m_WeaveItemArray[wChairID];
//	if (pWeaveItem == nullptr) return false;
//
//	BYTE cbItemCount = m_cbWeaveItemCount[wChairID];
//
//	// 检测菜包子
//	if (cbItemCount >= MAX_WEAVE - 1)
//	{
//		return IsSanPeng(wChairID);
//	}
//
//	return false;
//}

//预判断是否可以成包子;
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

	// 增加包子胡牌检测(如果可碰杠，判断碰杠后是否成包子，如果可以直接提示玩家胡牌);
	if ((dwUserAction & (WIK_PENG | WIK_MGANG | WIK_AGANG)) != 0)
	{
		// 已经是对了同一个玩家三嘴了，在对任意一家一嘴，就算胡包子;
		if (IsSanPeng(wChairID))
		{
			return true;
		}

		// 是不是四碰清一色;
		if (IsSiPengQingyise(wChairID, cbCenterCard, dwUserAction))
		{
			return true;
		}
	}

	return false;
}

//使用牌库
void CTableFrameSink::UseCardLibrary()
{
	WORD wChairID = INVALID_CHAIR;

	//找到作弊玩家的位置
	for (BYTE i = 0; i < GAME_PLAYER; i++)
	{
		//获取玩家信息
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

	//获取牌库
	tagGameCardLibrary* pCardLib = m_pITableFrame->GetGameCardLibrary(0);
	if (pCardLib == NULL)
	{
		return;
	}

	m_wCheatChairID = wChairID;

	//牌库内所有牌
	BYTE cbCardCount = pCardLib->cbCount;
	BYTE aryCardData[LEN_CARD_LIB] = { 0 };
	CopyMemory(aryCardData, pCardLib->cbData, cbCardCount);

	//花牌数据
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

		//删掉牌库中的花牌
		RemoveCards(aryCardData, LEN_CARD_LIB, aryHuaData, cbHuaCount);

		//重新组装牌
		CopyMemory(m_cbCheatCard, aryCardData, cbHuaCount);
		CopyMemory(aryCardData, aryHuaData, cbHuaCount);
	}

	//先把牌库中的牌删除
	BYTE cbMoveCount = (MAX_COUNT - 1);
	bool bRemoveSuccess = RemoveCards(m_cbRepertoryCard, CountArray(m_cbRepertoryCard), pCardLib->cbData, pCardLib->cbCount);
	if (bRemoveSuccess )
	{
		if (cbHuaCount>0)
		{
			memmove(m_cbRepertoryCard + cbHuaCount, m_cbRepertoryCard, CountArray(m_cbRepertoryCard) - pCardLib->cbCount);
			CopyMemory(m_cbRepertoryCard, m_cbCheatCard, cbHuaCount);
		}

		//发牌从后面开始，作弊玩家为0时正好是最后13张牌，这时直接把作弊牌拷贝过去即可，如果不为0先把对应位置牌移动到最后，再把作弊牌拷贝过去
		WORD wDesPos = CountArray(m_cbRepertoryCard) - ((wChairID + 1)*cbMoveCount);
		if (wChairID > 0)
		{
			CopyMemory(&m_cbRepertoryCard[CountArray(m_cbRepertoryCard) - cbMoveCount], &m_cbRepertoryCard[wDesPos], cbMoveCount);
		}
		CopyMemory(&m_cbRepertoryCard[wDesPos], aryCardData, cbMoveCount);
	}
}

//验证删除
bool CTableFrameSink::RemoveCards(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount)
{
	//检验数据
	ASSERT(cbRemoveCount <= cbCardCount);

	BYTE cbDeleteCount = 0;
	BYTE cbTempCardData[MAX_REPERTORY] = { 0 };

	CopyMemory(cbTempCardData, cbCardData, cbCardCount*sizeof(cbCardData[0]));
	//置零扑克
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
	//成功判断
	if (cbDeleteCount != cbRemoveCount)
	{
		ASSERT(FALSE);
		return false;
	}
	//清理扑克
	BYTE cbCardPos = 0;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (cbTempCardData[i] != 0)
			cbCardData[cbCardPos++] = cbTempCardData[i];
	}

	return true;
}

//设置游戏配置
void CTableFrameSink::SetGameServerOpertion(tagGameServiceOption *pGso)
{
	ASSERT(pGso != NULL);
	m_pGameServiceOption = pGso;
}

//读取服务器配置
bool CTableFrameSink::ReadServerConfig()
{
	//读取房间最大封顶倍数
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
		//是否开启抽奖
		m_cbOpenGameLottery = (BYTE)GetPrivateProfileInt(TEXT("ServerConfig"), TEXT("OpenGameLottery"), 0, szFileName);

		CString strKey;
		//读取奖励配置
		strKey.Format(_T("Prize%u"), m_pGameServiceOption->wServerID);
		GetPrivateProfileString(_T("LuckyDraw"), strKey, NULL, m_strLucyDrawCfg.GetBuffer(512), 512, szFileName);
		m_strLucyDrawCfg.ReleaseBuffer();
		//读取最大经验值
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

//发送设置底注消息
bool CTableFrameSink::SendSetBaseScore(WORD wChairID, BYTE cbBaseScoreType)
{
	CMD_S_SetBaseScore stSetBaseScore;
	ZeroMemory(&stSetBaseScore, sizeof(stSetBaseScore));
	stSetBaseScore.lBaseScore = m_lBaseScore;
	stSetBaseScore.lCellScore = m_pGameServiceOption->lCellScore;
	stSetBaseScore.lMaxCellScore = (LONG)m_pGameServiceOption->lMaxEnterScore;
	stSetBaseScore.cbSBSStatus = cbBaseScoreType;
	m_pITableFrame->SendTableData(wChairID, SUB_S_SET_BASESCORE, &stSetBaseScore, sizeof(stSetBaseScore));

	//有人进来重新计时
	if (cbBaseScoreType == STATUS_SBS_NOTIFY)
	{
		m_pITableFrame->KillGameTimer(IDI_BASESCORE);
		m_pITableFrame->SetGameTimer(IDI_BASESCORE, IDT_BASESCORE * 1000 + 500, 1, 0);
	}
	return true;
}

//检测通知底注
bool CTableFrameSink::CheckNotifyBaseScore()
{
	bool bFind = false;

	//让其他玩家确认同意桌主的设置
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

//检测玩家金币
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

//踢出玩家
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

//进程目录
bool CTableFrameSink::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//分析文件
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		if (szModulePath[i] == TEXT('\\'))
		{
			szModulePath[i] = 0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0] != 0);
	lstrcpyn(szWorkDirectory, szModulePath, wBufferCount);

	return true;
}

/////////////////////////////// 游戏录像 ///////////////////////////////////////////

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

////////////////////////////// 私人场 ////////////////////////////////////////////
// 重置统计数字
void CTableFrameSink::ResetPrivateEndInfo()
{
	ZeroMemory(&m_PrivateEndInfo, sizeof(CMD_S_Private_End_Info));
}

// 获得私人场结束信息流
void CTableFrameSink::GetPrivateEndInfo(DataStream &kDataStream, bool bSend)
{
	//for (WORD i = 0; i < GAME_PLAYER; i++)
	//{
	//	// 总积分
	//	IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(i);
	//	if (pServerUserItem)
	//	{
	//		SCORE lScore = pServerUserItem->GetUserScore();
	//		m_PrivateEndInfo.lAllScore[i] = lScore;
	//	}
	//}

	m_PrivateEndInfo.StreamValue(kDataStream, bSend);
}

//判断私人场是否结束
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

	//判断是否有玩家积分为0
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

	// 两人积分为0就解散游戏
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

//是否有包子;
bool CTableFrameSink::IsGameRuleBaoZi()
{
	if (m_pITableFrame == NULL) return false;

	const tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	if (NULL == pPrivateFrame)	return false;
	return ((pPrivateFrame->dwGameRule & eNJMJRuleEnum_BAO) == 0);
}

//是否是进园子;
bool CTableFrameSink::IsGameRuleJYZ()
{
	if (m_pITableFrame == NULL) return false;

	const tagPrivateFrameParameter* pPrivateFrame = m_pITableFrame->GetPrivateFrameInfo();
	if (NULL == pPrivateFrame)	return false;
	return ((pPrivateFrame->dwGameRule & eNJMJRuleEnum_JYZ) == 0);
}

// 扣分记录
void CTableFrameSink::PayMoneyLog(CMD_S_PayMoney & stPayMoney)
{
	// 录像记录;
	addGameRecordAction(SUB_S_PAY_MONEY, &stPayMoney, sizeof(CMD_S_PayMoney));

#if IS_OPEN_PRINT_LOG
	// 记录日志
	MyLog("扣分日志 ===================== cbPayReason = %d ", stPayMoney.cbPayReason);
	MyLog("赢钱人   ===================== wPayToUser = %d,  lPayToNumber = %ld", stPayMoney.wPayToUser, stPayMoney.lPayToNumber);

	LONG lAllPayMoney = stPayMoney.lPayToNumber;

	for (WORD i = 0; i < GAME_PLAYER -1 ; i++)
	{
		if (stPayMoney.lPayFromNumber == 0) continue;

		MyLog("扣钱人  ===================== wPayFromUser=%d, lPayFromNumber=%ld", stPayMoney.wPayFromUser[i], stPayMoney.lPayFromNumber[i]);

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