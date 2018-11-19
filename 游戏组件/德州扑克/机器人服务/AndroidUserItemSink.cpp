#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{	
	//接口变量
	m_pIAndroidUserItem=NULL;	
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));	
	ZeroMemory(m_cbCenterCardData,sizeof(m_cbCenterCardData));	
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));
	m_RobottLogic.SetRobotAlgorithm(this);
	OnSubClear();
	m_lAddTakeGoldMin = 0;
	m_lAddTakeGoldMax = 0;
	m_lSelfTakeGold = 0;

	return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	m_pIAndroidUserItem->KillGameTimer(0);
	OnSubClear();
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_ADDTAKEGOLD:
		{
			try
			{
				m_pIAndroidUserItem->KillGameTimer(IDI_ADDTAKEGOLD);	
				WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
				CMD_C_Add_TakeGold AddTakeGold;
				AddTakeGold.lAddTakeGold = m_lAddTakeGoldMin + rand()%(m_lAddTakeGoldMax - m_lAddTakeGoldMin);
				//SCORE lScore = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();
				SCORE lScore = m_pIAndroidUserItem->GetMeUserItem()->GetTrusteeScore();

				if (lScore < m_lAddTakeGoldMin)  //身上的钱不能低于下限
				{
					//ASSERT(0);
					return false;
				}

				LONG lMaxLimit = AddTakeGold.lAddTakeGold + m_lUserMaxScore[wMeChairID];
				if (lMaxLimit > lScore)  //游戏开始 或游戏中 限制
				{
					AddTakeGold.lAddTakeGold = lScore - m_lUserMaxScore[wMeChairID];
				}

				lMaxLimit = AddTakeGold.lAddTakeGold + m_lSelfTakeGold;
				if (lMaxLimit > lScore)  //总带入不能超自身拥有金币
				{
					AddTakeGold.lAddTakeGold = lScore - m_lSelfTakeGold;
				}

				if (lMaxLimit > m_lAddTakeGoldMax)  //总带入不能超上限
				{
					AddTakeGold.lAddTakeGold = m_lAddTakeGoldMax - m_lSelfTakeGold;
				}

				ASSERT(AddTakeGold.lAddTakeGold > 0);
				ASSERT(lMaxLimit  >= m_lAddTakeGoldMin);
				//m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_TAKEGOLD,&AddTakeGold,sizeof(AddTakeGold));
				m_pIAndroidUserItem->SendSocketData(SUB_GF_TAKE_GOLD, &AddTakeGold, sizeof(AddTakeGold));
			}
			catch (...)
			{
				ASSERT(0);
			}

			return true;
		}
	case IDI_START_GAME:		//开始定时器
		{		
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);			
			//发送准备
			m_pIAndroidUserItem->SendUserReady(NULL,0);		
			
			//CString str;
			//str.Format(_T("机器人 %s 定时器 %d 执行完成\n"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), IDI_START_GAME);
			//::OutputDebugString(str);

			return true;
		}
	case IDI_USER_ROBOTDELAY:	//机器人延迟操作
		{	
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ROBOTDELAY);
			m_RobottLogic.Robots(m_pIAndroidUserItem->GetChairID(), 0, 0);
			return true;
		}
	case IDI_USER_ADD_SCORE:
		{	
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);		
			if(m_cbBalanceCount>0)
			{
				if(m_RobottLogic.m_lRobotsScore>m_lCellScore && m_RobottLogic.m_lRobotsScore<m_RobottLogic.m_lBei[2]*m_lCellScore)
					m_RobottLogic.m_lRobotsScore = m_RobottLogic.m_lBei[2]*m_lCellScore;
			}		
			try
			{
				RobotGameMessage(m_wCurrentUser,m_RobottLogic.m_lRobotsScore,m_RobottLogic.m_cbRobotsType);	
			}
			catch(...)
			{
				ASSERT(false);
			}

			return true;
		}
	case  IDI_REQUEST://响应请求
		{
			//发送回复			
			return true;
		}
	case IDI_SET_BASESCORE://设置底注定时
		{
			//发送消息
			CMD_C_SetBaseScore BaseScore;
			ZeroMemory(&BaseScore, sizeof(BaseScore));
			BaseScore.bSetOrConfirm = m_bSetBaseScore;
			BaseScore.bUserChairID = m_pIAndroidUserItem->GetChairID();
			if ( SETBASE_SET == BaseScore.bSetOrConfirm )   //按最小设置
			{
				BaseScore.lBaseScore = m_lCellScore;
				BaseScore.lBaseScore = __max(1,BaseScore.lBaseScore);
			}
			m_pIAndroidUserItem->SendSocketData(SUB_C_SET_BASESCORE,&BaseScore,sizeof(BaseScore));

			IServerUserItem * pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if (m_bSetBaseScore > 0 && pUserItem->GetUserStatus() == US_SIT)  //机器人会有不准备的情况
			{
				m_pIAndroidUserItem->SendUserReady(NULL,0);
			}
			return true;	

		}
	case IDI_USER_PEICARD:
		{			
			return true;
		}
	case IDI_USER_ENDPEICARD:	//机器人延迟操作
		{				
			return true;
		}
		break;
	case IDI_USER_FAN_END://翻牌结束
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_FAN_END);			
			return true;
		}
	case IDI_USER_SCORE_END:
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_USER_SCORE_END);			
			return true;
		}
	}

	return false;
}
//机器人参数
bool CAndroidUserItemSink::OnGameRobot(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_Robot)) return false;
	m_RobottLogic.OnClear();
	CMD_S_Robot * pPlayerRobot=(CMD_S_Robot *)pBuffer;
	CopyMemory(m_cbHandCardData, pPlayerRobot->cbCardData, sizeof(pPlayerRobot->cbCardData));
	CopyMemory(m_cbCenterCardData, pPlayerRobot->cbCenterCardData, sizeof(pPlayerRobot->cbCenterCardData));
	CopyMemory(m_RobottLogic.m_lBei, pPlayerRobot->clLbei, sizeof(pPlayerRobot->clLbei));
	CopyMemory(m_RobottLogic.m_lRobotAdd, pPlayerRobot->lRobotAdd, sizeof(pPlayerRobot->lRobotAdd));
	CopyMemory(m_RobottLogic.m_lRobotQuAnYa, pPlayerRobot->lRobotQuanYa, sizeof(pPlayerRobot->lRobotQuanYa));
	CopyMemory(m_RobottLogic.m_lRobotGiveUp, pPlayerRobot->lRobotGiveUp, sizeof(pPlayerRobot->lRobotGiveUp));
	return true;
}
//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAN_READY:	//允许准备
		{	
			// 			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
			// 			//m_pIAndroidUserItem->SetGameStatus(US_SIT);
			// 			UINT nElapse = rand()%TIME_START_GAME + 6; 
			// 			if(m_ChipPoolCount>1)
			// 			{
			// 				nElapse += m_ChipPoolCount*3; 
			// 				nElapse = __min(nElapse,20);
			// 			}
			// 			
			// 			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
			return true;
		}
	case SUB_S_GAME_START://游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_TAKEGOLD:
	{
		return OnUserAddTakeGold(pData, wDataSize);
	}
	case SUB_S_USER_ROBOT://机器人配置
		{
			return OnGameRobot(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//用户放弃
		{
			//消息处理
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			//消息处理
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:		//用户加注
		{
			return OnSubAddScore(pData,wDataSize);
		}	
	case SUB_S_LOOK_CARD://亮牌
		{
			return true;
		}
	case SUB_S_TURN_OVER:	//一轮结束
		{
			return OnSubTurnOver(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_SET_BASESCORE:	//设置底
		{
			//消息处理
			return OnSubSetBaseScore(pData,wDataSize);
		}
	case SUB_S_KICK_OUT:
		{
			//m_pIAndroidUserItem->SendLeaveRequest();
			return true;
		}
	case SUB_S_DASHANG:
		{
			return OnSubDaShang(pData,wDataSize);
		}
	}
	//错误断言
	ASSERT(FALSE);
	return true;
}

bool CAndroidUserItemSink::OnUserAddTakeGold( const void * pBuffer, WORD wDataSize )
{
	if (wDataSize!=sizeof(CMD_S_Add_TakeGold)) return false;
	CMD_S_Add_TakeGold * pAddGold = (CMD_S_Add_TakeGold *) pBuffer;

	if (pAddGold->wChairID == m_pIAndroidUserItem->GetChairID())
	{
		m_lSelfTakeGold = pAddGold->lAllTakeGold;

		IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();
		//玩家设置
		if (pIServerUserItem->GetUserStatus()==US_SIT)
		{	
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,1);
		}
	}
	return true;
}

bool CAndroidUserItemSink::OnSubSetBaseScore( const void * pBuffer, WORD wDataSize )
{
	if (wDataSize!=sizeof(CMD_S_Set_BaseScore)) return false;
	CMD_S_Set_BaseScore * pSetBase = (CMD_S_Set_BaseScore *) pBuffer;

	m_bSetBaseScore = pSetBase->bSetOrConfirm;

	if ( SETBASE_SET == m_bSetBaseScore || SETBASE_CONFIRM == m_bSetBaseScore || SETBASE_OVER == m_bSetBaseScore)
	{
		UINT nElapse = rand()%4 + 2;
		m_lCellScore = pSetBase->lCellScore;
		m_pIAndroidUserItem->SetGameTimer(IDI_SET_BASESCORE,nElapse);
	}
	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pBuffer, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//效验数据
			VERIFY_RETURN_FALSE(wDataSize==sizeof(CMD_S_StatusFree));

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;
			m_lCellScore = pStatusFree->lBaseScore;

			m_lAddTakeGoldMin = pStatusFree->lAddTakeGoldMin;
			m_lAddTakeGoldMax = pStatusFree->lAddTakeGoldMax;
			//m_lSelfTakeGold = 0;
			m_lSelfTakeGold = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

			UINT nElapse=rand()%2 + 1;
			m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);

			return true;
		}
	case GAME_STATUS_PLAY:
		{
			//效验数据
			VERIFY_RETURN_FALSE(wDataSize==sizeof(CMD_S_StatusPlay));
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			m_pIAndroidUserItem->SetGameStatus(GAME_STATUS_PLAY);
			//设置变量
			m_wCurrentUser=pStatusPlay->wCurrentUser;//当前玩家
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore));//下注数目
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(m_cbPlayStatus));//用户游戏状态
			CopyMemory(m_lTurnMaxScore,pStatusPlay->lTurnMaxScore,sizeof(m_lTurnMaxScore));//用户游戏状态
			CopyMemory(m_cbCenterCardData,pStatusPlay->cbCenterCardData,sizeof(m_cbCenterCardData));//用户放弃状态

			m_iChipPoolCount = pStatusPlay->cbChipPoolCount;
			//加注信息
			m_lAddLessScore = pStatusPlay->lAddLessScore;
			m_lCellScore	= pStatusPlay->lCellScore;	
			m_lTurnLessScore= pStatusPlay->lTurnLessScore;

			m_lAddTakeGoldMin = pStatusPlay->lAddTakeGoldMin;
			m_lAddTakeGoldMax = pStatusPlay->lAddTakeGoldMax;
			return true;
		}
	}
	ASSERT(FALSE);
	return false;
}

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	//m_lSelfTakeGold = 0;
	m_lSelfTakeGold = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

	return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//用户段位
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}
void CAndroidUserItemSink::OnSubClear()
{
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));	
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));	
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));	
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));	

	m_wCurrentUser = INVALID_CHAIR;
	m_lCellScore	= 0;							//单元下注	
	m_bSetBaseScore  = 0;
	m_lTurnLessScore = 0;
	m_lAddLessScore  = 0;
	m_cbBalanceCount = 0;
	m_iChipPoolCount = 0;

}
//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//效验数据
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;	
	OnSubClear();
	//数据信息
	m_wCurrentUser = pGameStart->wCurrentUser;//当前玩家
	m_lCellScore = pGameStart->lCellScore;
	m_lTurnLessScore = pGameStart->lTurnLessScore;
	m_lAddLessScore =  pGameStart->lAddLessScore;
	CopyMemory(m_lTableScore, pGameStart->lTableScore, sizeof(pGameStart->lTableScore));	
	CopyMemory(m_lUserMaxScore, pGameStart->lUserMaxScore, sizeof(pGameStart->lUserMaxScore));	
	CopyMemory(m_lTurnMaxScore, pGameStart->lTurnMaxScore, sizeof(pGameStart->lTurnMaxScore));	
	CopyMemory(m_cbPlayStatus, pGameStart->cbPlayStatus, sizeof(pGameStart->cbPlayStatus));
	ASSERT(VALIDE_CHAIRID(m_wCurrentUser));
	ASSERT(m_cbPlayStatus[m_wCurrentUser] == TRUE);

	m_RobottLogic.m_lCellScore = m_lCellScore;
	m_RobottLogic.m_lTurnLessScore = m_lTurnLessScore;
	m_RobottLogic.m_lAddLessScore = m_lAddLessScore;
	m_cbBalanceCount =0;
	m_RobottLogic.m_iBalanceCount = 0;		

	//设置时间
	if( m_wCurrentUser == m_pIAndroidUserItem->GetChairID() )
	{
		//下注时间		
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ROBOTDELAY,4);		
	}
	return true;
}

bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;
	CMD_S_SendCard * p_SendCard = (CMD_S_SendCard *)pBuffer;
	//CopyMemory(m_RobottLogic.m_cbCenterCardData, p_SendCard->cbCenterCardData, sizeof(BYTE)*(p_SendCard->cbSendCardCount));
	m_wCurrentUser = p_SendCard->wCurrentUser;

	CopyMemory(m_lTableScore, p_SendCard->lTableScore, sizeof(m_lTableScore));
	//控制界面
	if (m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{	
		ASSERT(m_cbPlayStatus[m_wCurrentUser] == TRUE);
		m_RobottLogic.m_lTurnLessScore = m_lTurnLessScore;
		m_RobottLogic.m_lAddLessScore = m_lAddLessScore;		
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ROBOTDELAY,1);
	}
	return true;
}
bool CAndroidUserItemSink::OnSubAddScore(const void * pBuffer, WORD wDataSize)
{
	VERIFY_RETURN_FALSE (wDataSize==sizeof(CMD_S_AddScore));
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	CMD_S_AddScore * pAddScore=(CMD_S_AddScore *)pBuffer;
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	if( pAddScore->cbBalanceCount!=m_RobottLogic.m_iBalanceCount)
	{
		m_cbBalanceCount = pAddScore->cbBalanceCount;
		m_RobottLogic.m_iBalanceCount = pAddScore->cbBalanceCount;		
	}
	//加注处理
	if (pAddScore->wAddScoreUser != wMeChairID)
	{
		//加注界面
		if(pAddScore->lAddScoreCount>0)
		{			
			m_lTableScore[pAddScore->wAddScoreUser] +=pAddScore->lAddScoreCount;			
		}
	}

	//m_lSelfTakeGold = pAddScore->lUserTakeGold[wMeChairID];

	//设置变量
	m_wCurrentUser = pAddScore->wCurrentUser;

	m_lTurnLessScore = pAddScore->lTurnLessScore;
	m_lAddLessScore = pAddScore->lAddLessScore;
	if( VALIDE_CHAIRID(m_wCurrentUser) )
	{
		ASSERT(m_cbPlayStatus[m_wCurrentUser] == TRUE);
		m_lTurnMaxScore[m_wCurrentUser] = pAddScore->lTurnMaxScore[m_wCurrentUser];
	}
	//控制界面
	if (m_wCurrentUser == wMeChairID)
	{			
		m_RobottLogic.m_lTurnLessScore = m_lTurnLessScore;
		m_RobottLogic.m_lAddLessScore = m_lAddLessScore;		
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ROBOTDELAY,1);		
	}

	return true;
}

//一轮结束
bool CAndroidUserItemSink::OnSubTurnOver(const void * pBuffer, WORD wDataSize)
{
	VERIFY_RETURN_FALSE(wDataSize == sizeof(CMD_S_TurnOver));
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
	CMD_S_TurnOver * pTurnOver = (CMD_S_TurnOver *)pBuffer;
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	if (pTurnOver->cbBalanceCount != m_RobottLogic.m_iBalanceCount)
	{
		m_cbBalanceCount = pTurnOver->cbBalanceCount;
		m_RobottLogic.m_iBalanceCount = pTurnOver->cbBalanceCount;
	}
	//加注处理
	if (pTurnOver->wAddScoreUser != wMeChairID)
	{
		//加注界面
		if (pTurnOver->lAddScoreCount > 0)
		{
			m_lTableScore[pTurnOver->wAddScoreUser] += pTurnOver->lAddScoreCount;
		}
	}

	//设置变量
	m_wCurrentUser = INVALID_CHAIR;

	m_lTurnLessScore = pTurnOver->lTurnLessScore;
	m_lAddLessScore = pTurnOver->lAddLessScore;

	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//::OutputDebugString(TEXT("CAndroidUserItemSink::OnSubGiveUp()"));

	//效验数据
	VERIFY_RETURN_FALSE (wDataSize==sizeof(CMD_S_GiveUp));
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);		
	//设置变量
	ASSERT(VALIDE_CHAIRID(pGiveUp->wGiveUpUser));
	m_cbPlayStatus[pGiveUp->wGiveUpUser] = FALSE;
	//CString str;
	//str.Format(TEXT("nTableID %d wChairID %d 放弃下注 wGiveUpUser %d \n"),m_pIAndroidUserItem->GetTableID(),m_pIAndroidUserItem->GetChairID(),pGiveUp->wGiveUpUser);
	//::OutputDebugString(str);
	return true;
}

bool CAndroidUserItemSink::RobotGameMessage(WORD wChairID, LONG lScore,BYTE ID) 
{
	//::OutputDebugString(TEXT("CAndroidUserItemSink::RobotGameMessage()"));

	switch(ID)
	{
	case CHIP_TYPE_GIVEUP://机器人放弃
		{
			//状态判断
			ASSERT(m_cbPlayStatus[wChairID]==TRUE);
			if (m_cbPlayStatus[wChairID]==FALSE) return false;		
			//CString str;
			//str.Format(TEXT("nTableID %d wChairID %d 主动放弃下注 \n"),m_pIAndroidUserItem->GetTableID(),m_pIAndroidUserItem->GetChairID());
			//::OutputDebugString(str);
			//消息处理
			m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP);
			break;
		}
	case CHIP_TYPE_PASS://让
	case CHIP_TYPE_GEN://跟
	case CHIP_TYPE_ADD://加注
	case CHIP_TYPE_ALLIN://全压
		{
			ASSERT(m_cbPlayStatus[wChairID]==TRUE);
			if (m_cbPlayStatus[wChairID]==FALSE) return false;
			CMD_C_AddScore AddScore;
			AddScore.ID  = ID;	
			if( ID == CHIP_TYPE_PASS)//过
			{
				m_RobottLogic.m_lRobotsScore = 0;			
			}
			else if( ID == CHIP_TYPE_GEN )//跟
			{
			}		
			else if ( ID == CHIP_TYPE_ALLIN ) //全押
			{
			}
			else if( ID == CHIP_TYPE_ADD )//加注
			{
				if(m_RobottLogic.m_lRobotsScore > m_lTurnMaxScore[wChairID]) //超过最低剩余筹码，全压
				{
					m_RobottLogic.m_lRobotsScore = m_lTurnMaxScore[wChairID];
					AddScore.ID  = CHIP_TYPE_ALLIN;		
				}
			}
			if (m_RobottLogic.m_lRobotsScore >= m_lSelfTakeGold)
			{
				m_RobottLogic.m_lRobotsScore = m_lSelfTakeGold;
				AddScore.ID  = CHIP_TYPE_ALLIN;	
			}
			AddScore.lScore = m_RobottLogic.m_lRobotsScore;
			m_lSelfTakeGold -= AddScore.lScore;
			//消息处理
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
			break;
		}
	}
	return false;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//效验参数
	VERIFY_RETURN_FALSE(wDataSize==sizeof(CMD_S_GameEnd));
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;	
	m_iChipPoolCount = pGameEnd->cbChipPoolCount;
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);	
	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ROBOTDELAY);
	m_pIAndroidUserItem->KillGameTimer(IDI_SET_BASESCORE);

	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));	

	WORD wChairID = m_pIAndroidUserItem->GetChairID();
	m_lSelfTakeGold = pGameEnd->lChipScore[wChairID];

#ifdef DEBUG
	CString str;
	str.Format(TEXT("游戏结束 机器人 lUserTakeGold[0] %d 1. %d 2. %d 3. %d 4. %d 5. %d 6. %d 7. %d 8. %d wChairID %d m_lSelfTakeGold %d \n"), pGameEnd->lChipScore[0], pGameEnd->lChipScore[1],
		pGameEnd->lChipScore[2], pGameEnd->lChipScore[3], pGameEnd->lChipScore[4], pGameEnd->lChipScore[5], pGameEnd->lChipScore[6],
		pGameEnd->lChipScore[7], pGameEnd->lChipScore[8], wChairID, m_lSelfTakeGold);
	::OutputDebugString(str);
#endif

	if (m_lSelfTakeGold < m_lAddTakeGoldMin)
	{
		//UINT nElapse = rand() % TIME_START_GAME + 2;
		//m_pIAndroidUserItem->SetGameTimer(IDI_ADDTAKEGOLD, nElapse);
		CString str;
		str.Format(_T("机器人 %s 持有金币 %d 小于最小带入金币 %d \n"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), m_lSelfTakeGold, m_lAddTakeGoldMin);
		::OutputDebugString(str);
	}
	//else
	{
		UINT nElapse = rand()%TIME_START_GAME + 2;
		if(m_iChipPoolCount>1)
		{
			nElapse += m_iChipPoolCount*3; 
			nElapse = __min(nElapse,20);
		}

		m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, nElapse);
		
		//CString str;
		//str.Format(_T("机器人 %s 设置定时器 %d 定时时间 %d \n"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), IDI_START_GAME, nElapse);
		//::OutputDebugString(str);
		
	}

	return true;
}

bool CAndroidUserItemSink::OnSubDaShang( void * pData, WORD wDataSize )
{
	//::OutputDebugString(TEXT("CAndroidUserItemSink::OnSubDaShang()"));

	//效验参数
	if (wDataSize!=sizeof(CMD_S_DaShang)) return false;	
	CMD_S_DaShang * pDaShang=(CMD_S_DaShang *)pData;
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	if (pDaShang->wChairID == wMeChairID)
	{
		//m_lSelfTakeGold = pDaShang->lUserTakeGold[wMeChairID];
		m_lSelfTakeGold = pDaShang->lUserScore;
	}
	return true;
}

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);
//////////////////////////////////////////////////////////////////////////
