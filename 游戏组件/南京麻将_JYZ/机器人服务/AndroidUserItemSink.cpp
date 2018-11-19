#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include <strsafe.h>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
//游戏时间
#define	IDI_ANDROID_ITEM_SINK		100
#define IDI_START_GAME_A			(IDI_ANDROID_ITEM_SINK+0)			//开始时间
#define IDI_SHOOT_DICE_A			(IDI_ANDROID_ITEM_SINK+1)			//掷骰子时间
#define IDI_DISPATCH_CARD_A			(IDI_ANDROID_ITEM_SINK+2)			//发牌动画时间
#define IDI_OPERATE_CARD_A			(IDI_ANDROID_ITEM_SINK+3)			//操作时间
#define IDI_SET_BASESCORE_A			(IDI_ANDROID_ITEM_SINK+4)			//设置底注时间
////////////////////////////////////////////////////////////////////////////////////////////
//静态变量
BYTE CAndroidUserItemSink::m_cbMinReadyTime = 2;						//最小准备时间
BYTE CAndroidUserItemSink::m_cbMaxReadyTime = 5;						//最大准备时间
BYTE CAndroidUserItemSink::m_cbMinOperateCardTime = 2;					//最小操作时间
BYTE CAndroidUserItemSink::m_cbMaxOperateCardTime = 5;					//最大操作时间
BYTE CAndroidUserItemSink::m_cbMinBaseScoreTime = 2;					//最小设置底注时间
BYTE CAndroidUserItemSink::m_cbMaxBaseScoreTime = 5;					//最大设置底注时间

////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lBaseScore = 0L;
	m_cbSetBaseScoreStatus = STATUS_SBS_ERROR;
	ReInit();
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
	ReInit();

	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME_A:		//开始游戏
		{
			if(m_pIAndroidUserItem->GetMeUserItem()->GetUserStatus()==US_SIT)
			{
				m_pIAndroidUserItem->SendUserReady(NULL, 0);
			}
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME_A);
			return true;
		}
	case IDI_SHOOT_DICE_A:
		{
			if (m_pIAndroidUserItem->GetGameStatus()==GS_MJ_DICE && m_pIAndroidUserItem->GetChairID()==m_wBankerUser)
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_SHOOT_DICE);
			}
			m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
			return true;
		}
	case IDI_DISPATCH_CARD_A:
		{
			if (m_pIAndroidUserItem->GetGameStatus()==GS_MJ_PLAY && !m_bHaveGameEnd)
			{
				m_pIAndroidUserItem->SendSocketData(SUB_C_DISPATCH_OVER);
			}
			m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCH_CARD_A);
			return true;
		}
	case IDI_OPERATE_CARD_A:		//操作定时器
		{
			ASSERT(m_pIAndroidUserItem->GetGameStatus()==GS_MJ_PLAY && m_wCurrentUser==m_pIAndroidUserItem->GetChairID());
			if (m_pIAndroidUserItem->GetGameStatus()==GS_MJ_PLAY && m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				DWORD dwAction = m_dwActionMask & (~WIK_CHU);
				if( dwAction != WIK_NULL )       //有吃碰杠听胡操作
				{
					OnOperateCard();
				}
				else if( (m_dwActionMask & WIK_CHU) > 0 )   //出牌操作
				{
					OnOutCard();
				}
				else
				{
					ASSERT(0);
				}
			}
			return true;
		}
	case IDI_SET_BASESCORE_A:		//设置底注定时器
		{
			//发送消息
			CMD_C_SetBaseScore stSetBaseScore;
			ZeroMemory(&stSetBaseScore, sizeof(stSetBaseScore));
			if (m_cbSetBaseScoreStatus == STATUS_SBS_SET)
			{
				//m_lBaseScore = m_pIAndroidUserItem->GetAndroidSetTableCell();
				stSetBaseScore.lBaseScore = m_lBaseScore;
				stSetBaseScore.cbSBSAction = ACTION_SBS_SET;
			}
			else if (m_cbSetBaseScoreStatus == STATUS_SBS_NOTIFY)
			{
				m_lBaseScore = __max(1, m_lBaseScore);
				stSetBaseScore.lBaseScore = m_lBaseScore;
				stSetBaseScore.cbSBSAction = ACTION_SBS_AGREE;
			}
			m_pIAndroidUserItem->SendSocketData(SUB_C_SET_BASESCORE, &stSetBaseScore, sizeof(stSetBaseScore));

			//机器人会有不准备的情况
			tagUserInfo* UserData = m_pIAndroidUserItem->GetMeUserItem()->GetUserInfo();
			if (m_cbSetBaseScoreStatus!=STATUS_SBS_ERROR && m_cbSetBaseScoreStatus!=STATUS_SBS_WAIT && UserData->cbUserStatus==US_SIT) 
			{
				m_pIAndroidUserItem->SendUserReady(NULL, 0);
			}
			return true;
		}
	}
	ASSERT(FALSE);
	return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_SET_BASESCORE:	//设置底注
		{
			return OnSubSetBaseScore(pData,wDataSize);
		}
	case SUB_S_SHOOT_DICE:		//掷骰子消息
		{
			return OnSubShootDice(pData,wDataSize);
		}
	case SUB_S_GAME_START:		//游戏开始
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//发牌消息
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_SEND_BUHUA:		//补花消息
		{
            return OnSubUserBuHua(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//用户出牌
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//操作提示
		{
			return OnSubOperateNotify(pData,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//操作结果
		{
			return OnSubOperateResult(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//游戏结束
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_KICK_OUT:		//踢出玩家
		{
			m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE);
			return true;
		}
	case SUB_S_PAY_MONEY:		//小结消息
		{
			return true;
		}
	case SUB_S_USER_TRUST:		//用户托管
		{
			return true;
		}
	}

	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//非设置底注房间
			if (pStatusFree->cbSBSStatus == STATUS_SBS_ERROR)
			{
				if (pIServerUserItem->GetUserStatus() == US_SIT)
				{
					UINT nElapse = GetAndroidOpTime(IDI_START_GAME_A);
					m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME_A, nElapse);
				}
			}
			else
			{
				m_cbSetBaseScoreStatus = pStatusFree->cbSBSStatus;
				if (STATUS_SBS_SET == m_cbSetBaseScoreStatus)
				{
					m_lBaseScore = pStatusFree->lCellScore;
					UINT nElapse = GetAndroidOpTime(IDI_SET_BASESCORE_A);
					m_pIAndroidUserItem->SetGameTimer(IDI_SET_BASESCORE_A, nElapse);
				}
				if (STATUS_SBS_NOTIFY == m_cbSetBaseScoreStatus)
				{
					m_lBaseScore = pStatusFree->lBaseScore;
					UINT nElapse = GetAndroidOpTime(IDI_SET_BASESCORE_A);
					m_pIAndroidUserItem->SetGameTimer(IDI_SET_BASESCORE_A, nElapse);
				}
			}
			return true;
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//设置变量
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;

			//听牌状态
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
//			m_bTingStatus=(pStatusPlay->cbHearStatus[wMeChairID]==TRUE)?true:false;

			//历史变量
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));

			//牌
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount[wMeChairID],m_cbCardIndex[wMeChairID]);

			////出牌设置
			//if ((pStatusPlay->dwActionMask!=WIK_NULL) && m_wCurrentUser==wMeChairID)
			//{
			//	UINT nElapse = GetAndroidOpTime(IDI_OPERATE_CARD_A);
			//	m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD_A, nElapse);
			//}
			return true;
		}
	}

	return false;
}

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
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

//处理设置底注消息
bool CAndroidUserItemSink::OnSubSetBaseScore( void *pData, WORD wDataSize )
{
	if (wDataSize!=sizeof(CMD_S_SetBaseScore)) return false;
	CMD_S_SetBaseScore * pSetBaseScore = (CMD_S_SetBaseScore *)pData;

	m_cbSetBaseScoreStatus = pSetBaseScore->cbSBSStatus;

	if (STATUS_SBS_SET == m_cbSetBaseScoreStatus)
	{
		m_lBaseScore = pSetBaseScore->lCellScore;
		UINT nElapse = GetAndroidOpTime(IDI_SET_BASESCORE_A);
		m_pIAndroidUserItem->SetGameTimer(IDI_SET_BASESCORE_A, nElapse);
	}
	if (STATUS_SBS_NOTIFY == m_cbSetBaseScoreStatus)
	{
		m_lBaseScore = pSetBaseScore->lBaseScore;
		UINT nElapse = GetAndroidOpTime(IDI_SET_BASESCORE_A);
		m_pIAndroidUserItem->SetGameTimer(IDI_SET_BASESCORE_A, nElapse);
	}
	return true;
}

//掷骰子消息
bool CAndroidUserItemSink::OnSubShootDice( const void *pBuffer, WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_ShootDice));
	if (wDataSize!=sizeof(CMD_S_ShootDice)) return false;
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_DICE);

	CMD_S_ShootDice *pGameShootDice = (CMD_S_ShootDice *)pBuffer;

	m_wBankerUser = pGameShootDice->wBankerUser;
// 	//庄家是自己
// 	if (m_pIAndroidUserItem->GetChairID() == m_wBankerUser)
// 	{
// 		//设置
// 		UINT nElapse = rand()%TIME_SHOOT_DICE+TIME_LESS;
// 		m_pIAndroidUserItem->SetGameTimer(IDI_SHOOT_DICE_A, nElapse);
// 	}

	return true;
}

//庄家信息
bool CAndroidUserItemSink::OnSubGameStart(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCH_CARD_A);

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	ReInit();
	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_PLAY);

	m_bHaveGameEnd = false;
	m_cbLeftCardCount = MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1);
	m_dwActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//设置牌
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardCount = (wMeChairId==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,(MAX_COUNT-1),m_cbCardIndex[wMeChairId]);

	//设置
	UINT nElapse = GetAndroidOpTime(IDI_DISPATCH_CARD_A);
	m_pIAndroidUserItem->SetGameTimer(IDI_DISPATCH_CARD_A, nElapse);
	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//消息处理
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	m_bHaveGameEnd = true;
	//设置状态
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_FREE);

	//删除定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCH_CARD_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	//设置
	UINT nElapse = GetAndroidOpTime(IDI_START_GAME_A);
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME_A, nElapse);
	return true;
}

//操作提示
bool CAndroidUserItemSink::OnSubOperateNotify( const void *pBuffer,WORD wDataSize )
{
	//效验数据
	ASSERT(wDataSize >= sizeof(CMD_S_OperateNotify_Send));
	if (wDataSize < sizeof(CMD_S_OperateNotify_Send)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	DataStream kDataStream((void*)pBuffer, wDataSize);

	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	CMD_S_OperateNotify_Send kOperateNotify;
	kOperateNotify.StreamValue(kDataStream, false);
	m_wCurrentUser = kOperateNotify.wOperateUser;

	//用户界面
	if ((kOperateNotify.wOperateUser == wMeChairID) && (kOperateNotify.dwActionMask != WIK_NULL))
	{
		//获取变量
		m_dwActionMask = kOperateNotify.dwActionMask;
		m_cbActionCard = kOperateNotify.cbActionCard;

		CopyMemory(&m_GangResult, &kOperateNotify.GangPaiResult, sizeof(tagGangPaiResult));

		// 有听牌信息;
		if ((kOperateNotify.dwActionMask&WIK_TING) != 0 && kDataStream.size() > 0)
		{
			tagTingPaiDataStream kTingPaiStream;
			kTingPaiStream.StreamValue(kDataStream, false);
			
			tagTingPaiResult TingPaiResult(kTingPaiStream);
			CopyMemory(&m_TingResult, &TingPaiResult, sizeof(tagTingPaiResult));
		}

		//设置时间
		UINT nElapse = GetAndroidOpTime(IDI_OPERATE_CARD_A);
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD_A, nElapse);
	}
	
	return true;
}

//
bool CAndroidUserItemSink::OnSubOperateResult( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//删除之前的定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);
	//消息处理
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	BYTE cbPublicCard=TRUE;
	WORD wOperateUser = pOperateResult->wOperateUser;
	WORD wProvideUser = pOperateResult->wProvideUser;
	BYTE cbOperateCard = pOperateResult->cbOperateCard;
	DWORD dwOperateCode = pOperateResult->dwOperateCode;

	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	//出牌变量
	if (dwOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//设置数据
	m_wCurrentUser = INVALID_CHAIR;
	m_dwActionMask = WIK_NULL;
	m_cbActionCard =0;
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	//设置组合
	if ( dwOperateCode & WIK_GANG )
	{
		if (dwOperateCode == WIK_BGANG)
		{
			for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
			{
				DWORD dwWeaveKind=m_WeaveItemArray[wOperateUser][i].dwWeaveKind;
				BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
				if ( (cbCenterCard==cbOperateCard) && (dwWeaveKind==WIK_PENG) )
				{
					m_WeaveItemArray[wOperateUser][i].cbPublicCard=TRUE;
					m_WeaveItemArray[wOperateUser][i].dwWeaveKind = dwOperateCode;
					m_WeaveItemArray[wOperateUser][i].wProvideUser = wProvideUser;
					break;
				}
			}
		}
		else
		{
			if(dwOperateCode == WIK_AGANG)
			{
				cbPublicCard = FALSE;
				ASSERT(wProvideUser == wOperateUser);
			}
			if (cbPublicCard == TRUE)
			{
				ASSERT(wProvideUser >= 0 && wProvideUser<GAME_PLAYER);
				//明杠 清除上家打出的牌
				BYTE cbDiscardCount = --m_cbDiscardCount[wProvideUser];
				m_cbDiscardCard[wProvideUser][cbDiscardCount] = 0;
			}
			//设置扑克
			BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard = cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard = cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].dwWeaveKind  = dwOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = wProvideUser;
		}
		//扑克设置
		if( wOperateUser == wMeChairID )
		{
			BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard);
			m_cbCardIndex[wMeChairID][cbCardIndex] = 0;
		}
	}
	else if ( (dwOperateCode & WIK_PENG)>0 || (dwOperateCode & WIK_CHI)>0 )
	{
		m_wCurrentUser = wOperateUser;
		//清除上家打出的牌
		BYTE cbDiscardCount = --m_cbDiscardCount[wProvideUser];
		m_cbDiscardCard[wProvideUser][cbDiscardCount] = 0;

		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].dwWeaveKind=dwOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=wProvideUser;

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=dwOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);

		//删除扑克
		if( wOperateUser == wMeChairID)
		{
			m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
			m_GameLogic.RemoveCard(m_cbCardIndex[wOperateUser],cbWeaveCard,cbWeaveCardCount-1);
		}
	}
	else if (dwOperateCode == WIK_TING)
	{
		if( wOperateUser == wMeChairID)
		{
		}
	}
	return true;
}

//
bool CAndroidUserItemSink::OnSubOutCard( const void *pBuffer,WORD wDataSize )
{
	//效验消息
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//删除操作定时器
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	//消息处理
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	m_wCurrentUser = INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	ASSERT( pOutCard->cbOutCardData != 0 );
	m_cbOutCardData=pOutCard->cbOutCardData;

	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	if ( wMeChairId == pOutCard->wOutCardUser )
	{
		m_GameLogic.RemoveCard(m_cbCardIndex[wMeChairId],pOutCard->cbOutCardData);
	}
	
	BYTE cbDiscardCount = ++m_cbDiscardCount[m_wOutCardUser];
	m_cbDiscardCard[m_wOutCardUser][cbDiscardCount-1] = m_cbOutCardData;
	return true;
}

//
bool CAndroidUserItemSink::OnSubSendCard( const void *pBuffer,WORD wDataSize )
{
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCH_CARD_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	m_wCurrentUser=pSendCard->wSendCardUser;

	//出牌数据重置
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;

	//发牌处理
	if ( (pSendCard->cbCardData!=0) && (m_wCurrentUser==wMeChairID) )
	{
		BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData);
		m_cbCardIndex[m_wCurrentUser][cbCardIndex]++;	

		m_cbSendCardData = pSendCard->cbCardData;
	}
	//扣除扑克
	m_cbLeftCardCount--;
	return true;
}

bool CAndroidUserItemSink::OnSubUserBuHua( const void *pBuffer, WORD wDataSize )
{
	//数据校验
	ASSERT(wDataSize==sizeof(CMD_S_SendBuHua));
	if (wDataSize!=sizeof(CMD_S_SendBuHua)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	CMD_S_SendBuHua *pSendBuHua = (CMD_S_SendBuHua *)pBuffer;
	ASSERT(pSendBuHua->cbBuHuaCount>0);

	//设置变量
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	if ( wMeChairID == pSendBuHua->wBuHuaUser )   //自己补花
	{
		m_GameLogic.RemoveCard(m_cbCardIndex[wMeChairID],pSendBuHua->cbBuHuaData,pSendBuHua->cbBuHuaCount);
		for (int i = 0; i < pSendBuHua->cbBuHuaCount; ++i)
		{
			BYTE cbSendIndex = m_GameLogic.SwitchToCardIndex(pSendBuHua->cbReplaceCardData[i]);
			m_cbCardIndex[wMeChairID][cbSendIndex]++;
		}
	}
	//扣除扑克
	m_cbLeftCardCount -= pSendBuHua->cbBuHuaCount;
	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnOutCard()
{
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	ASSERT(m_wCurrentUser == wMeChairID /*&& !m_bTingStatus*/);
	
	if ( wMeChairID !=m_wCurrentUser)
	{
		return false;
	}
	//打字牌
	const BYTE MaxOutCount = 14;    //最多可出牌张数
	
	BYTE cbIsolateCard[MaxOutCount];
	memset(cbIsolateCard,0,sizeof(cbIsolateCard));

	BYTE cbIsolateCount = CalIsolatedCard(m_cbCardIndex[wMeChairID],cbIsolateCard);
	BYTE cbCardData = ChooseIsolatedCard(cbIsolateCard,cbIsolateCount);

	if( m_GameLogic.IsValidCard(cbCardData) && m_GameLogic.IsZiCard(cbCardData))
	{
		DoOutCard(cbCardData);
		return true;
	}

	//打听牌  根据服务器发来数据处理更准确
	tagTingPaiResult TingPaiResult;
	DWORD dwPreTingPaiKind = m_GameLogic.AnalyseTingPai(m_cbCardIndex[wMeChairID], m_WeaveItemArray[wMeChairID], m_cbWeaveCount[wMeChairID], 
														0, 0, TingPaiResult);
	if (dwPreTingPaiKind & WIK_TING)
	{
		BYTE cbOutCard = GetBestTingPutCard(TingPaiResult);
		if( m_GameLogic.IsValidCard(cbOutCard) )
		{
			DoOutCard(cbOutCard);
			return true;
		}
	}
	//打缺门
	if( OutCardSameColor() )
		return true;
	
	//打七对
	if ( OutCardQiDui(cbIsolateCard,cbIsolateCount) )
	{
		return true;
	}
	//打孤牌
	if( m_GameLogic.IsValidCard(cbCardData) )
	{
		DoOutCard(cbCardData);
		return true;
	}
	
	//没有孤牌
	BYTE cbOneCard[MaxOutCount] = {0};
	BYTE cbOneCount = GetCardFromCount(cbOneCard,1);
	BYTE cbTwoCard[MaxOutCount] = {0};
	BYTE cbTwoCount = GetCardFromCount(cbTwoCard,2);
	BYTE cbThreeCard[MaxOutCount] = {0};
	BYTE cbThreeCount = GetCardFromCount(cbThreeCard,3);
	
	//根据牌数量选择
	if (cbOneCount > 0)  //优先打单张
	{
		const BYTE TypeCount = 4;
		BYTE cbCount[TypeCount];    //个数
		BYTE cbOutCard[TypeCount][MaxOutCount]; //牌值
		ZeroMemory(cbCount,sizeof(cbCount));
		ZeroMemory(cbOutCard,sizeof(cbOutCard));
		for (int i = 0; i < cbOneCount; ++i)
		{
			BYTE cbCard = cbOneCard[i];
			DWORD dwChiType = GetCardShunType(cbCard);

			if (dwChiType == WIK_NULL)    //不成顺
				cbOutCard[0][cbCount[0]++] = cbCard;
			else if( dwChiType == WIK_OXX || dwChiType == WIK_XXO )  //
				cbOutCard[1][cbCount[1]++] = cbCard;
			else if(dwChiType == WIK_XOX)            //顺 打中间一张牌
				cbOutCard[2][cbCount[2]++] = cbCard;
			else
				cbOutCard[3][cbCount[3]++] = cbCard;
		}
		for (int j = 0; j < TypeCount; ++j)
		{
			BYTE cbTypeCount = cbCount[j];
			if (cbTypeCount > 0)
			{
				DoOutCard(cbOutCard[j][rand()%cbTypeCount]);
				return true;
			}
		}
	}
	if (cbTwoCount > 0)
	{
		//优先非字牌
		BYTE cbCount[2] = {0};
		BYTE cbOutCard[2][MaxOutCount];
		ZeroMemory(cbCount,sizeof(cbCount));
		ZeroMemory(cbOutCard,sizeof(cbOutCard));
		for (int i = 0; i < cbTwoCount;++i)
		{
			BYTE cbCard = cbTwoCard[i];
			if( !m_GameLogic.IsZiCard(cbCard))
				cbOutCard[0][cbCount[0]++] = cbCard;
			else
				cbOutCard[1][cbCount[1]++] = cbCard;
		}
		for (int j = 0; j < 2; ++j)
		{
			BYTE cbTypeCount = cbCount[j];
			if (cbTypeCount > 0)
			{
				DoOutCard(cbOutCard[j][rand()%cbTypeCount]);
				return true;
			}
		}
	}
	if (cbThreeCount > 0)
	{
		BYTE cbCount[2] = {0};
		BYTE cbOutCard[2][MaxOutCount];
		ZeroMemory(cbCount,sizeof(cbCount));
		ZeroMemory(cbOutCard,sizeof(cbOutCard));
		for (int i = 0; i < cbThreeCount;++i)
		{
			BYTE cbCard = cbThreeCard[i];
			if( !m_GameLogic.IsZiCard(cbCard))
				cbOutCard[0][cbCount[0]++] = cbCard;
			else
				cbOutCard[1][cbCount[1]++] = cbCard;
		}
		for (int j = 0; j < 2; ++j)
		{
			BYTE cbTypeCount = cbCount[j];
			if (cbTypeCount > 0)
			{
				DoOutCard(cbOutCard[j][rand()%cbTypeCount]);
				return true;
			}
		}
	}
	BYTE cbAllCount = 0;
	BYTE cbAllCard[MAX_NORMAL] = {0};
	for (BYTE k=0;k < MAX_NORMAL;k++)
	{
		if ( m_cbCardIndex[wMeChairID][k] == 0 ) continue;

		cbCardData = m_GameLogic.SwitchToCardData(k);
		cbAllCard[cbAllCount++] = cbCardData;
	}

	ASSERT(cbAllCount > 0);
	if (cbAllCount == 0)
		return false;
	
	DoOutCard(cbAllCard[rand()%cbAllCount]);
	return true;
}

bool CAndroidUserItemSink::OutCardSameColor()
{
	float fGameProgress = (float)m_cbLeftCardCount / (MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1);  //游戏进度
	if (fGameProgress <= 0.5f)
	{
		return false;
	}

	BYTE cbLimitJudge = 2;
	if(fGameProgress >= 0.8f)
		cbLimitJudge = 5;
	else if(fGameProgress >= 0.7f)
		cbLimitJudge = 4;
	else if(fGameProgress >= 0.6f)
		cbLimitJudge = 3;
	else if(fGameProgress >= 0.5f)
		cbLimitJudge = 2;
	else if(fGameProgress >= 0.3f)
		cbLimitJudge = 1;

	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	const BYTE MaxOutCount = 14;    //最多张数

	BYTE cbWanCards[MaxOutCount] = {0};  //万
	BYTE cbWanCount = GetSameColorCard(cbWanCards,0);
	BYTE cbTiaoCards[MaxOutCount] = {0};  //条
	BYTE cbTiaoCount = GetSameColorCard(cbTiaoCards,1);
	BYTE cbTongCards[MaxOutCount] = {0};  //筒
	BYTE cbTongCount = GetSameColorCard(cbTongCards,2);

	BYTE cbBigColorCount = 0;
	BYTE cbBigColorCards[MaxOutCount] = {0};

	BYTE cbOutCards[MaxOutCount];
	BYTE cbOutCount = 0;
	ZeroMemory(cbOutCards,MaxOutCount);

	if (cbTiaoCount + cbTongCount <= cbLimitJudge)
	{
		cbBigColorCount = cbWanCount;
		CopyMemory(cbBigColorCards,cbWanCards,cbWanCount);
		CopyMemory(cbOutCards,cbTiaoCards,cbTiaoCount);
		CopyMemory(cbOutCards+cbTiaoCount,cbTongCards,cbTongCount);
		cbOutCount = cbTiaoCount+cbTongCount;
	}
	if (cbWanCount + cbTongCount <= cbLimitJudge)
	{
		cbBigColorCount = cbTiaoCount;
		CopyMemory(cbBigColorCards,cbTiaoCards,cbTiaoCount);
		CopyMemory(cbOutCards,cbWanCards,cbWanCount);
		CopyMemory(cbOutCards+cbWanCount,cbTongCards,cbTongCount);
		cbOutCount = cbWanCount+cbTongCount;
	}
	if (cbWanCount + cbTiaoCount <= cbLimitJudge)
	{
		cbBigColorCount = cbTongCount;
		CopyMemory(cbBigColorCards,cbTongCards,cbTongCount);
		CopyMemory(cbOutCards,cbWanCards,cbWanCount);
		CopyMemory(cbOutCards+cbWanCount,cbTiaoCards,cbTiaoCount);
		cbOutCount = cbWanCount+cbTiaoCount;
	}
	//CString str;
	//str.Format(_T("CAndroidUserItemSink::OutCardSameColor() 3 cbBigColorCount %d cbOutCount %d \n"),cbBigColorCount,cbOutCount);
	//::OutputDebugString(str);

	if (cbBigColorCount > 0)
	{
		BYTE cbColor = m_GameLogic.GetCardColor(cbBigColorCards[0]);
		//左手牌花色
		for (int i = 0; i < m_cbWeaveCount[wMeChairID]; ++i)
		{
			const tagWeaveItem & weaveitem = m_WeaveItemArray[wMeChairID][i];
			BYTE cbLeftColor = m_GameLogic.GetCardColor(weaveitem.cbCenterCard);
			if (cbLeftColor != cbColor && cbLeftColor < 3)
			{
				return false;
			}
		}
		if (cbOutCount > 0)  //从另外两种花色牌中选择最优出牌
		{
			BYTE cbSelectCardsIndex[MAX_INDEX];
			ZeroMemory(cbSelectCardsIndex,sizeof(cbSelectCardsIndex));
			m_GameLogic.SwitchToCardIndex(cbOutCards,cbOutCount,cbSelectCardsIndex);

			BYTE cbIsolateCard[MaxOutCount] = {0};
			BYTE cbIsolateCount = CalIsolatedCard(cbSelectCardsIndex,cbIsolateCard);
			if (cbIsolateCount > 0)
			{
				BYTE cbCardData = ChooseIsolatedCard(cbIsolateCard,cbIsolateCount);
				if( m_GameLogic.IsValidCard(cbCardData) )
				{
					DoOutCard(cbCardData);
					return true;
				}
			}
			//没有孤牌就选一张打出最多的牌
			SortCardHeap(cbOutCards,cbOutCount,&CAndroidUserItemSink::CompareCard);

			BYTE cbOutChoose = cbOutCards[0/*rand()%cbOutCount*/];
			VERIFY( m_GameLogic.IsValidCard(cbOutChoose));
			if ( !m_GameLogic.IsValidCard(cbOutChoose) )
			{
				return false;
			}

			DoOutCard(cbOutChoose);
			return true;
		}
	}
	ASSERT(cbBigColorCount == 0 || cbOutCount == 0);
	return false;
}

bool CAndroidUserItemSink::OutCardQiDui( BYTE cbCanOutCard[], BYTE cbCanOutCount)
{
	//七对判断
	BYTE cbDuiZiNum = GetDuiZiNum();
	float fGameProgress = (float)m_cbLeftCardCount / (MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1);  //游戏进度

	bool bJudgeQiDui = false;  //判断是否满足打七对条件

	if (cbDuiZiNum == 4 && fGameProgress >= 0.6f)
		bJudgeQiDui = true;
	else if (cbDuiZiNum == 5 && fGameProgress >= 0.4f)
		bJudgeQiDui = true;
	else if (cbDuiZiNum >= 6 && fGameProgress >= 0.2f)
		bJudgeQiDui = true;

	if ( bJudgeQiDui )  //打出手中单牌已经出现最多的那张
	{
		BYTE cbMaxPutCount = 0xff;
		BYTE cbOutCard = 0xff;
		
		for (int i = 0; i < cbCanOutCount;++i)  
		{
			BYTE cbCard = cbCanOutCard[i];
			if (m_GameLogic.IsValidCard(cbCard))
			{
				BYTE cbCardCount = CalCardCountInTable(cbCard)-CalCardCountInHand(cbCard);

				if( cbMaxPutCount == 0xff || (cbMaxPutCount != 0xff && cbCardCount > cbMaxPutCount) )
				{
					cbMaxPutCount = cbCardCount;
					cbOutCard = cbCard;
				}
			}
		}
		if ( m_GameLogic.IsValidCard(cbOutCard))  
		{
			DoOutCard(cbOutCard);
			return true;
		}
	}
	return false;
}

//
bool CAndroidUserItemSink::OnOperateCard()
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);
	if ((m_dwActionMask&WIK_HU) > 0)
	{
		DoOperateCard(WIK_HU, m_cbActionCard);
		return true;
	}
	if ((m_dwActionMask&WIK_TING) > 0)
	{
		DoOperateCard(WIK_TING, 0);
		return true;
	}
	if ((m_dwActionMask&WIK_GANG) > 0)
	{
		DoOperateCard(m_dwActionMask,0);
		return true;
	}
	if (m_dwActionMask != WIK_NULL)
	{
		DWORD dwOperateCode = GetTheBestOperate(m_dwActionMask,m_cbActionCard);

		ASSERT( m_cbActionCard!=0xFF && m_cbActionCard!=0);
		
		if(m_cbActionCard == 0xff || m_cbActionCard == 0)
			return false;

		DoOperateCard(dwOperateCode,m_cbActionCard);
		return true;
	}
	return false;
}

//出牌判断
bool CAndroidUserItemSink::VerdictOutCard(BYTE cbCardData)
{
	//听牌判断
	if (m_bTingStatus)
	{
		//变量定义
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		BYTE cbWeaveCount=m_cbWeaveCount[wMeChairID];

		//构造牌
		BYTE cbCardIndexTemp[MAX_INDEX];
		CopyMemory(cbCardIndexTemp,m_cbCardIndex[wMeChairID],sizeof(cbCardIndexTemp));

		//删除牌
		m_GameLogic.RemoveCard(cbCardIndexTemp,cbCardData);

		//听牌判断
		BYTE i = 0;
		for (;i<MAX_INDEX;i++)
		{
			//胡牌分析
			BYTE wChiHuRight=0;
			BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(i);
			DWORD dwHuCardKind=0;//m_GameLogic.AnalyseHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairID],cbWeaveCount,cbCurrentCard,ChiHuResult);

			//结果判断
			if (dwHuCardKind!=HU_NULL) 
				break;
		}
		//听牌判断
		return (i!=MAX_INDEX);
	}
	return true;
}

void CAndroidUserItemSink::ReInit()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	m_dwActionMask = WIK_NULL;
	m_cbActionCard = 0;
    
	m_bHaveGameEnd = true;
	m_bTingStatus = false;
	//出牌信息
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbSendCardData = 0;

	//组合扑克
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//扑克变量
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));
	ZeroMemory(&m_TingResult,sizeof(m_TingResult));
	ZeroMemory(&m_GangResult,sizeof(m_GangResult));
}

bool CAndroidUserItemSink::IsYaoCard( BYTE cbCardData )
{
	if ( m_GameLogic.IsZiCard(cbCardData) )
	{
		return true;
	}
	else
	{
		BYTE cbValue = m_GameLogic.GetCardValue(cbCardData);
		if(cbValue == 1 || cbValue == 9)
			return true;
	}
	return false;
}

void CAndroidUserItemSink::DoOutCard( BYTE cbOutCard )
{
	m_wCurrentUser = INVALID_CHAIR;
	//设置界面
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);
	m_cbSendCardData=0;

	//重置变量
	m_dwActionMask = WIK_NULL;

	VERIFY(m_GameLogic.IsValidCard(cbOutCard));
	if( !m_GameLogic.IsValidCard(cbOutCard)) return;

	//发送数据
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
	{
		ASSERT( FALSE );
		return ;
	}
}

void CAndroidUserItemSink::DoOperateCard( DWORD dwOperateCode, BYTE cbOperateCard )
{
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	if ((dwOperateCode&WIK_HU) > 0)				//胡牌
	{
		dwOperateCode = WIK_HU;
	}
	else if ((dwOperateCode&WIK_GANG) > 0)   //有杠则杠
	{
		//随机选择其中的杠牌
		ASSERT(m_GangResult.cbCardCount > 0);
		int nChooseGang = rand()%m_GangResult.cbCardCount;

		dwOperateCode = m_GangResult.dwGangType[nChooseGang];
		cbOperateCard = m_GangResult.cbCardData[nChooseGang];

		VERIFY(m_GameLogic.IsValidCard(cbOperateCard));
		if (!m_GameLogic.IsValidCard(cbOperateCard))
		{
			return;
		}
		CString str;
		str.Format(_T("DoOperateCard WIK_GANG dwOperateCode %d dwOperateCode %d cbOperateCard %d"),dwOperateCode,dwOperateCode,cbOperateCard);
		::OutputDebugString(str);
	}
	else if ((dwOperateCode&WIK_TING) > 0)
	{
		cbOperateCard = GetBestTingPutCard(m_TingResult);
		if (!m_GameLogic.IsValidCard(cbOperateCard))
		{
			OnOutCard();
			return;
		}
		else
		{
			m_dwActionMask = WIK_NULL;
			CMD_C_OutCard OutCard;
			OutCard.cbCardData = cbOperateCard;

			if (!m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCard, sizeof(OutCard)))
			{
				ASSERT(FALSE);
			}
		}
		return;
	}

	//重置数据
	m_dwActionMask = WIK_NULL;    

	//发送命令
	CMD_C_OperateCard OperateCard;
	OperateCard.dwOperateCode = dwOperateCode;
	OperateCard.cbOperateCard = cbOperateCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}
}

BYTE CAndroidUserItemSink::CalIsolatedCard(BYTE cbCalCards[], BYTE cbIsolateCard[],bool bGetAllIsolate /*= true*/ )
{
	BYTE cbIsolateCount = 0;
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	//临时手牌
	BYTE cbTempCardIndex[MAX_INDEX];
	ZeroMemory(cbTempCardIndex,sizeof(cbTempCardIndex));
	CopyMemory(cbTempCardIndex,cbCalCards,sizeof(cbTempCardIndex));

	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(cbTempCardIndex,cbCardData);
	ASSERT(cbCardCount <= 14);

	BYTE cbRandCount = 0;
	for (int i = 0; i < cbCardCount; ++i)    //有字孤牌
	{
		BYTE cbCard = cbCardData[i];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue;   //单张时处理孤牌
		if (cbIsolateCount > 0 && m_GameLogic.IsCardInArrary(cbIsolateCard,cbIsolateCount,cbCard))  continue;

		if ( m_GameLogic.IsZiCard(cbCard) )
		{
			if( !bGetAllIsolate )  return cbIsolateCount;

			cbIsolateCard[cbIsolateCount++] = cbCard;
			++cbRandCount;
		}
	}
	RandCardData(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount);
	SortCardHeap(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount,&CAndroidUserItemSink::CompareCard);

	cbRandCount = 0;
	for (int j = 0; j < cbCardCount; ++j)   //19孤牌
	{
		BYTE cbCard = cbCardData[j];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue;   
		if (cbIsolateCount > 0 && m_GameLogic.IsCardInArrary(cbIsolateCard,cbIsolateCount,cbCard))  continue;

		if ( IsYaoCard(cbCard) )
		{
			BYTE cbNearCount = CalNearCardNum(cbCard);
			if( cbNearCount == 0 )
			{
				if( !bGetAllIsolate )  return cbIsolateCount;

				cbIsolateCard[cbIsolateCount++] = cbCard;
				++cbRandCount;
			}
		}
	}
	RandCardData(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount);
	SortCardHeap(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount,&CAndroidUserItemSink::CompareCard);

	cbRandCount = 0;
	for (int k = 0; k < cbCardCount; ++k)  //打普通孤牌 左右2张都没有
	{
		BYTE cbCard = cbCardData[k];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue; 
		if (cbIsolateCount > 0 && m_GameLogic.IsCardInArrary(cbIsolateCard,cbIsolateCount,cbCard))  continue;

		if ( !IsYaoCard(cbCard) )
		{
			BYTE cbExistCount = CalNearCardNum(cbCard);
			if (cbExistCount == 0)
			{
				if( !bGetAllIsolate )  return cbIsolateCount;

				cbIsolateCard[cbIsolateCount++] = cbCard;
				++cbRandCount;
			}
		} 
	}
	RandCardData(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount);
	SortCardHeap(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount,&CAndroidUserItemSink::CompareCard);

	cbRandCount = 0;
	for (int m = 0; m < cbCardCount; ++m)   //19卡牌 组不成顺子
	{
		BYTE cbCard = cbCardData[m];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue;   
		if (cbIsolateCount > 0 && m_GameLogic.IsCardInArrary(cbIsolateCard,cbIsolateCount,cbCard))  continue;

		if ( IsYaoCard(cbCard) )
		{
			BYTE cbNearCount = CalNearCardNum(cbCard);
			if( cbNearCount == 1 )
			{
				if( !bGetAllIsolate )  return cbIsolateCount;

				cbIsolateCard[cbIsolateCount++] = cbCard;
				++cbRandCount;
			}
		}
	}
	RandCardData(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount);
	SortCardHeap(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount,&CAndroidUserItemSink::CompareCard);

	cbRandCount = 0;
	for (int n = 0; n < cbCardCount; ++n)  //打普通卡牌  组不成顺子  左右都没有牌
	{
		BYTE cbCard = cbCardData[n];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue;   
		if (cbIsolateCount > 0 && m_GameLogic.IsCardInArrary(cbIsolateCard,cbIsolateCount,cbCard))  continue;

		if ( !IsYaoCard(cbCard) )
		{
			BYTE cbLeftIndex = m_GameLogic.SwitchToCardIndex(cbCard - 1);
			BYTE cbRightIndex = m_GameLogic.SwitchToCardIndex(cbCard + 1);
			if (cbTempCardIndex[cbLeftIndex] == 0 && 
				cbTempCardIndex[cbRightIndex] == 0)
			{
				if( !bGetAllIsolate )  return cbIsolateCount;

				cbIsolateCard[cbIsolateCount++] = cbCard;
				++cbRandCount;
			}
		} 
	}
	RandCardData(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount);
	SortCardHeap(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount,&CAndroidUserItemSink::CompareCard);

	cbRandCount = 0;
	for (int n = 0; n < cbCardCount; ++n)  //打普通卡牌  组不成顺子  普通牌的左右只有一张
	{
		BYTE cbCard = cbCardData[n];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue;   
		if (cbIsolateCount > 0 && m_GameLogic.IsCardInArrary(cbIsolateCard,cbIsolateCount,cbCard))  continue;

		if ( !IsYaoCard(cbCard) )
		{
			DWORD dwCanChiType = m_GameLogic.AnalyseChiPai(cbTempCardIndex,cbCard);
			if ( dwCanChiType == WIK_NULL )
			{
				if( !bGetAllIsolate )  return cbIsolateCount;

				cbIsolateCard[cbIsolateCount++] = cbCard;
				++cbRandCount;
			}
		} 
	}
	RandCardData(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount);
	SortCardHeap(cbIsolateCard+cbIsolateCount-cbRandCount,cbRandCount,&CAndroidUserItemSink::CompareCard);

	ASSERT(cbIsolateCount <= 14);
	return cbIsolateCount;
}

BYTE CAndroidUserItemSink::CalNearCardNum( BYTE cbCard )
{
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

	BYTE cbColor = m_GameLogic.GetCardColor(cbCard);
	BYTE cbValue = m_GameLogic.GetCardValue(cbCard);

	BYTE cbNearCard[4];  //左右4张相邻的牌是否存在
	memset(cbNearCard,0,sizeof(cbNearCard));
	if( cbValue > 2 )   cbNearCard[0] = cbCard - 2;
	if( cbValue > 1)    cbNearCard[1] = cbCard - 1;
	if( cbValue < 9)    cbNearCard[2] = cbCard + 1;  
	if( cbValue < 8)    cbNearCard[3] = cbCard + 2; 

	BYTE cbExistCount = 0;
	for (int i = 0; i < 4; ++i)
	{
		BYTE cbNear = cbNearCard[i];
		if (m_GameLogic.IsValidCard(cbNear))
		{
			BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbNear);
			if(m_cbCardIndex[wMeChairID][cbCardIndex] > 0) cbExistCount++;
		}
	}
	return cbExistCount;
}

DWORD CAndroidUserItemSink::GetTheBestOperate( DWORD dwOperateCode,BYTE cbOperateCard )
{
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	const BYTE INVALID_COUNT = 0xff;
	BYTE cbIsolateCard[14];
	BYTE cbPreIsolateCount= CalIsolatedCard(m_cbCardIndex[wMeChairID],cbIsolateCard,true);
	BYTE cbPengIsolateCount = INVALID_COUNT;
	BYTE cbLeftIsolateCount = INVALID_COUNT;
	BYTE cbCenterIsolateCount = INVALID_COUNT;
	BYTE cbRightIsolateCount = INVALID_COUNT;

	//碰牌操作
	if(dwOperateCode & WIK_PENG)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_PENG;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbPengIsolateCount= CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}
	//左吃操作
	if(dwOperateCode & WIK_OXX)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_OXX;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbLeftIsolateCount=CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}
	//中吃操作
	if(dwOperateCode & WIK_XOX)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_XOX;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbCenterIsolateCount=CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}
	//右吃操作
	if(dwOperateCode & WIK_XXO)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//组合界面
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_XXO;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbRightIsolateCount=CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}

	//最优权位
	if( (dwOperateCode & WIK_PENG) > 0 )
	{
		//七对判断
		BYTE cbDuiZiNum = GetDuiZiNum();
		float fGameProgress = (float)m_cbLeftCardCount / (MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1);  //游戏进度

		if (cbDuiZiNum == 4 && fGameProgress >= 0.6f)
			return WIK_NULL;
		else if (cbDuiZiNum == 5 && fGameProgress >= 0.4f)
			return WIK_NULL;
		else if (cbDuiZiNum >= 6 && fGameProgress >= 0.2f)
			return WIK_NULL;

		if( cbPreIsolateCount-1==cbPengIsolateCount )
		{
			return WIK_PENG;
		}
	}
	if( (dwOperateCode & WIK_OXX) > 0)
	{
		if ( cbPreIsolateCount-1 == cbLeftIsolateCount)
		{
			BYTE cbChance = rand()%3;
			if (cbChance == 0)
			{
				return WIK_OXX;
			}
		}
	}
	if( (dwOperateCode & WIK_XOX) > 0)
	{
		if ( cbPreIsolateCount-1 == cbCenterIsolateCount)
		{
			BYTE cbChance = rand()%5;
			if (cbChance == 0)
			{
				return WIK_XOX;
			}
		}
	}
	if( (dwOperateCode & WIK_XXO) > 0)
	{
		if ( cbPreIsolateCount-1 == cbRightIsolateCount)
		{
			BYTE cbChance = rand()%3;
			if (cbChance == 0)
			{
				return WIK_XXO;
			}
		}
	}

	//剩下最少孤牌的操作
	BYTE cbMin=INVALID_COUNT;
	if( cbMin > cbPengIsolateCount)
		cbMin=cbPengIsolateCount;
	if(cbMin>cbLeftIsolateCount)
		cbMin=cbLeftIsolateCount;
	if(cbMin>cbCenterIsolateCount)
		cbMin=cbCenterIsolateCount;
	if(cbMin>cbRightIsolateCount)
		cbMin=cbRightIsolateCount;

	ASSERT( cbMin != INVALID_COUNT );
	if( cbMin>cbPreIsolateCount)
		return WIK_NULL;
	if( cbMin==cbPengIsolateCount)
		return WIK_PENG;
	if( cbMin==cbLeftIsolateCount)
		return WIK_OXX;
	if( cbMin==cbCenterIsolateCount)
		return WIK_XOX;
	if( cbMin==cbRightIsolateCount)
		return WIK_XXO;

	return WIK_NULL;
}

BYTE CAndroidUserItemSink::CalCardCountInTable( BYTE cbCardData )
{
	BYTE cbCount = 0;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		//打出的牌
		for (int j = 0; j < m_cbDiscardCount[i];++j)
		{
			if(m_cbDiscardCard[i][j] == cbCardData)
				++cbCount;
		}
		//吃碰杠的牌
		for (int k = 0; k < m_cbWeaveCount[i]; ++k)
		{
			const tagWeaveItem & weaveitem = m_WeaveItemArray[i][k];
			if( weaveitem.cbCenterCard == cbCardData)
			{
				if( (WIK_GANG & weaveitem.dwWeaveKind) > 0 )  cbCount += 4;
				else if(WIK_PENG == weaveitem.dwWeaveKind)  cbCount += 3;
			}
			BYTE cbCardBuffer[3] = {0};
			if (WIK_OXX == weaveitem.dwWeaveKind)
			{
				cbCardBuffer[0] = weaveitem.cbCenterCard;cbCardBuffer[1] = weaveitem.cbCenterCard + 1;cbCardBuffer[2] = weaveitem.cbCenterCard + 2;
			}
			else if (WIK_XOX == weaveitem.dwWeaveKind)
			{
				cbCardBuffer[0] = weaveitem.cbCenterCard - 1;cbCardBuffer[1] = weaveitem.cbCenterCard;cbCardBuffer[2] = weaveitem.cbCenterCard + 1;
			}
			else if (WIK_XXO == weaveitem.dwWeaveKind)
			{
				cbCardBuffer[0] = weaveitem.cbCenterCard - 2;cbCardBuffer[1] = weaveitem.cbCenterCard - 1;cbCardBuffer[2] = weaveitem.cbCenterCard;
			}
			//吃牌
			if ( (weaveitem.dwWeaveKind & WIK_CHI) > 0 ) {
				for (int m = 0; m < 3; ++m) {
					if(cbCardBuffer[m] == cbCardData) {
						cbCount++;
						break;
					}
				}
			}
		}
	}
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	for (int n = 0; n < MAX_INDEX; ++n) 
	{
		if(m_cbCardIndex[wMeChairID][n] > 0) 
		{
			BYTE cbIndexCard = m_GameLogic.SwitchToCardData(n);
			if(cbIndexCard == cbCardData) 
			{
				cbCount += m_cbCardIndex[wMeChairID][n];
				break;
			}
		}
	}

	ASSERT(cbCount <= 4);
	return cbCount;
}

BYTE CAndroidUserItemSink::CalCardCountInHand( BYTE cbCardData )
{
    if (m_GameLogic.IsValidCard(cbCardData))
    {
		WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

		BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbCardData);

		BYTE cbCount = m_cbCardIndex[wMeChairID][cbCardIndex];
		ASSERT(cbCount <= 4);
		return  cbCount;
    }

	return 0;
}

BYTE CAndroidUserItemSink::ChooseIsolatedCard( BYTE cbCanOutCard[],BYTE cbCanOutCount )
{
	//打出已经出的牌
	BYTE cbMaxPutCount = 0xff;
	BYTE cbOutCard = 0xff;
	//孤字牌优先打
	for (int i = 0; i < cbCanOutCount;++i)  
	{
		BYTE cbCard = cbCanOutCard[i];

		if (m_GameLogic.IsZiCard(cbCard))
		{
			BYTE cbCardCount = CalCardCountInTable(cbCard)-CalCardCountInHand(cbCard);

			if( cbMaxPutCount == 0xff || (cbMaxPutCount != 0xff && cbCardCount > cbMaxPutCount) )
			{
				cbMaxPutCount = cbCardCount;
				cbOutCard = cbCard;
			}
		}
	}
	if ( m_GameLogic.IsValidCard(cbOutCard))  
	{
		return cbOutCard;
	}

	//幺九牌
	cbMaxPutCount = 0xff;
	cbOutCard = 0xff;
	for (int i = 0; i < cbCanOutCount;++i)  
	{
		BYTE cbCard = cbCanOutCard[i];
		
		if ( IsYaoCard(cbCard) )
		{
			BYTE cbCardCount = CalCardCountInTable(cbCard)-CalCardCountInHand(cbCard);

			if( cbMaxPutCount == 0xff || (cbMaxPutCount != 0xff && cbCardCount > cbMaxPutCount) )
			{
				cbMaxPutCount = cbCardCount;
				cbOutCard = cbCard;
			}
		}
	}
	if ( cbMaxPutCount >= 3 && m_GameLogic.IsValidCard(cbOutCard))  //优先出已经被打出多张的牌
	{
		return cbOutCard;
	}

	//非字牌 非幺九牌
	cbMaxPutCount = 0xff;
	cbOutCard = 0xff;
	for (int i = 0; i < cbCanOutCount;++i)  
	{
		BYTE cbCard = cbCanOutCard[i];

		if ( m_GameLogic.IsValidCard(cbCard) )
		{
			BYTE cbCardCount = CalCardCountInTable(cbCard)-CalCardCountInHand(cbCard);

			if( cbMaxPutCount == 0xff || (cbMaxPutCount != 0xff && cbCardCount > cbMaxPutCount) )
			{
				cbMaxPutCount = cbCardCount;
				cbOutCard = cbCard;
			}
		}
	}
	
	if ( cbMaxPutCount >= 3 && m_GameLogic.IsValidCard(cbOutCard))  //优先出已经被打出多张的牌
	{
		return cbOutCard;
	}

	return cbCanOutCard[0];
}

BYTE CAndroidUserItemSink::GetCardFromCount( OUT BYTE cbAllCard[],IN const int nCardCount )
{
	const BYTE MaxOutCount = 14;    //最多张数
	WORD wMeChair=m_pIAndroidUserItem->GetChairID();

	BYTE cbAllCount = 0;
	ZeroMemory(cbAllCard,sizeof(BYTE)*MaxOutCount);
	for (INT j= 0;j < MAX_NORMAL; j++)
	{
		BYTE cbCount = m_cbCardIndex[wMeChair][j];
		if( cbCount == nCardCount)
		{
			BYTE cbCardData=m_GameLogic.SwitchToCardData(j);
			cbAllCard[cbAllCount++] = cbCardData;
		}
	}
	return cbAllCount;
}

DWORD CAndroidUserItemSink::GetCardShunType( BYTE cbCard )
{
	//是否万饼条
	BYTE cbColor = m_GameLogic.GetCardColor(cbCard);
	if (cbColor >= 3)
	{
		return WIK_NULL;
	}
	WORD wMeChair=m_pIAndroidUserItem->GetChairID();
	BYTE cbNearCount = CalNearCardNum(cbCard);
	if (cbNearCount  > 1)
	{
		BYTE cbValue = m_GameLogic.GetCardValue(cbCard);
		if(cbValue == 1 )
			return WIK_OXX;
		else if(cbValue == 9)
			return WIK_XXO;

		DWORD dwChiType = WIK_NULL;
		BYTE cbLeft2Count = 0,cbLeft1Count = 0,cbRight1Count = 0,cbRight2Count = 0;
		if (cbValue > 2)
		{
			BYTE cbLeft2Card = cbCard -2;
			cbLeft2Count = m_cbCardIndex[wMeChair][m_GameLogic.SwitchToCardIndex(cbLeft2Card)];
		}
		if (cbValue > 1)
		{
			BYTE cbLeft1Card = cbCard -1;
			cbLeft1Count = m_cbCardIndex[wMeChair][m_GameLogic.SwitchToCardIndex(cbLeft1Card)];
		}
		if (cbValue < 9)
		{
			BYTE cbRight1Card = cbCard +1;
			cbRight1Count = m_cbCardIndex[wMeChair][m_GameLogic.SwitchToCardIndex(cbRight1Card)];
		}
		if (cbValue < 8)
		{
			BYTE cbRight2Card = cbCard +2;
			cbRight2Count = m_cbCardIndex[wMeChair][m_GameLogic.SwitchToCardIndex(cbRight2Card)];
		}
		if (cbLeft2Count > 0 && cbLeft1Count > 0)
		{
			dwChiType |= WIK_XXO;
		}
		if (cbLeft1Count > 0 && cbRight1Count > 0)
		{
			dwChiType |= WIK_XOX;
		}
		if (cbRight1Count > 0 && cbRight2Count > 0)
		{
			dwChiType |= WIK_OXX;
		}
		return dwChiType;
	}
	return WIK_NULL;
}

BYTE CAndroidUserItemSink::GetDuiZiNum()
{ 
	BYTE cbDuiZiCount = 0;
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

	for (int i = 0; i < MAX_INDEX; ++i)    
	{
		if (m_cbCardIndex[wMeChairID][i] == 2) 
			cbDuiZiCount++;
		else if (m_cbCardIndex[wMeChairID][i] == 4)
			cbDuiZiCount += 2;
	}

	return cbDuiZiCount;
}

BYTE CAndroidUserItemSink::GetSameColorCard( OUT BYTE cbSameColorCard[],IN BYTE cbColor )
{
	BYTE cbSameColorCount = 0;
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	for (int i = 0; i < MAX_INDEX;++i)
	{
		BYTE cbCardCount = m_cbCardIndex[wMeChairID][i];
		if (cbCardCount > 0)
		{
			BYTE cbCard = m_GameLogic.SwitchToCardData(i);
			BYTE cbCardColor = m_GameLogic.GetCardColor(cbCard);

			if (cbCardColor == cbColor)
			{
				for(int j = 0; j < cbCardCount; ++j)  
					cbSameColorCard[cbSameColorCount++] = cbCard;
			}
		}
	}

	return cbSameColorCount;
}

BYTE CAndroidUserItemSink::GetBestTingPutCard( const tagTingPaiResult & TingPaiData )
{
	BYTE cbPutCard = 0;
	BYTE cbMaxLeft = 0;

	for (int i = 0; i < TingPaiData.cbTingPaiItemCount; ++i)
	{
		const tagTingPaiItem & TingPaiItem = TingPaiData.TingPaiItemArrary[i];
		if ( m_GameLogic.IsValidCard(TingPaiItem.cbOutCardData) )
		{
			BYTE cbHuLeft = 0;
			for (int j = 0; j < TingPaiItem.cbTingCardCount; ++j)
			{
				const BYTE cbHuCard = TingPaiItem.cbTingCardData[j]; 
				if (m_GameLogic.IsValidCard(cbHuCard))       //判断胡的牌还有
				{
					BYTE cbLeft = 4 - CalCardCountInTable(cbHuCard);
					ASSERT(cbLeft <= 4);
					if(cbLeft == 0)  continue;

					cbHuLeft += cbLeft;
				}
			}
			if ( cbHuLeft > cbMaxLeft)
			{
				cbPutCard = TingPaiItem.cbOutCardData;   //要打出的牌 胡牌剩牌最多的
				cbMaxLeft  = cbHuLeft;
			}
		}
	}

	return cbPutCard;
}

void CAndroidUserItemSink::RandCardData( BYTE cbCardData[], BYTE cbMaxCount )
{
	if(cbMaxCount <= 1)  return;

	const BYTE MAX_DEAL_COUNT = 14;
	//混乱准备
	BYTE cbCardDataTemp[MAX_DEAL_COUNT] = {0};  //拷贝原数据
	CopyMemory(cbCardDataTemp, cbCardData, sizeof(BYTE)*cbMaxCount);
	BYTE cbCardPutIn[MAX_DEAL_COUNT] = {0};  //接收混乱结果

	//初始化种子
	srand(GetTickCount()|(rand()<<8));

	//混乱牌
	BYTE cbRandCount=0, cbPosition=0;
	do
	{
		cbPosition = rand()%(cbMaxCount-cbRandCount);
		cbCardPutIn[cbRandCount++] = cbCardDataTemp[cbPosition];
		cbCardDataTemp[cbPosition] = cbCardDataTemp[cbMaxCount-cbRandCount];
	} while (cbRandCount<cbMaxCount);

	CopyMemory(cbCardData, cbCardPutIn, sizeof(BYTE)*cbMaxCount);
}

void CAndroidUserItemSink::SortCardHeap( BYTE cbCardData[],BYTE cbMaxCount, pSortFun pFun )
{
    if (cbMaxCount <= 1)  return;
    
	//std::sort(cbCardData,cbCardData+cbMaxCount,pFun);
	for (int i = 0; i < cbMaxCount-1;++i)
	{
		for (int j = i+1; j < cbMaxCount;++j)
		{
			if ( (this->*pFun)(cbCardData[i],cbCardData[j]))
			{
				std::swap(cbCardData[i],cbCardData[j]);
			}
		}
	}
}

bool CAndroidUserItemSink::CompareCard( BYTE cbCard1, BYTE cbCard2 )
{
	BYTE cbLeftCount = CalCardCountInTable(cbCard1)-CalCardCountInHand(cbCard1);
	BYTE cbRightCount = CalCardCountInTable(cbCard2)-CalCardCountInHand(cbCard2);

	ASSERT(cbLeftCount <= 4 && cbRightCount <= 4);

	BYTE cbMargin = cbLeftCount > cbRightCount ? (cbLeftCount - cbRightCount):(cbRightCount - cbLeftCount);
	
    if (  cbLeftCount < cbRightCount && 
		 (cbRightCount > 2 || cbMargin >= 2) )
    {
		return true;
    }
	else if (cbLeftCount < cbRightCount && IsYaoCard(cbCard1) && IsYaoCard(cbCard2))
	{
		return true;
	}
	else if (cbLeftCount < cbRightCount && !IsYaoCard(cbCard1) && !IsYaoCard(cbCard2))
	{
		return true;
	}
	else 
	{
		if ( IsYaoCard(cbCard2) && !IsYaoCard(cbCard1))
		{
			return true;
		}
	}

	return false;
}

//读取机器人配置
bool CAndroidUserItemSink::ReadAndroidConfig()
{
	//设置文件名
	TCHAR szFileName[MAX_PATH] = TEXT("");
	GetWorkDirectory(szFileName,CountString(szFileName));	
	StringCchCat(szFileName,CountString(szFileName),TEXT("\\NJMJServer.ini"));
	if (!::PathFileExists(szFileName))
	{
		ASSERT(FALSE);
		return false;
	}
	try
	{
		//最小准备时间
		m_cbMinReadyTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinReadyTime"), 2, szFileName); 
		//最大准备时间
		m_cbMaxReadyTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxReadyTime"), 5, szFileName);
		ASSERT(m_cbMaxReadyTime >= m_cbMinReadyTime);

		//最小操作时间
		m_cbMinOperateCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinOperateCardTime"), 2, szFileName); 
		//最小操作时间
		m_cbMaxOperateCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxOperateCardTime"), 5, szFileName); 
		ASSERT(m_cbMaxOperateCardTime >= m_cbMinOperateCardTime);

		//最小设置底注时间
		m_cbMinBaseScoreTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinBaseScoreTime"), 2, szFileName); 
		//最大设置底注时间
		m_cbMaxBaseScoreTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxBaseScoreTime"), 5, szFileName); 
		ASSERT(m_cbMaxBaseScoreTime >= m_cbMinBaseScoreTime);

		return true;
	}
	catch (...)
	{
		ASSERT(false);
		return false;
	}
}

//获取机器人操作时间
UINT CAndroidUserItemSink::GetAndroidOpTime( UINT uOpType )
{
	UINT uElapse = 2;
	switch (uOpType)
	{
	case IDI_START_GAME_A:
		uElapse = m_cbMinReadyTime + ((m_cbMaxReadyTime-m_cbMinReadyTime)>0 ? rand()%(m_cbMaxReadyTime-m_cbMinReadyTime) : 0);
		break;
	case IDI_DISPATCH_CARD_A:
		uElapse = 1+rand()%3;
		break;
	case IDI_OPERATE_CARD_A:
		uElapse = m_cbMinOperateCardTime + ((m_cbMaxOperateCardTime-m_cbMinOperateCardTime)>0 ? rand()%(m_cbMaxOperateCardTime-m_cbMinOperateCardTime) : 0);
		break;
	case IDI_SET_BASESCORE_A:
		uElapse = m_cbMinBaseScoreTime + ((m_cbMaxBaseScoreTime-m_cbMinBaseScoreTime)>0 ? rand()%(m_cbMaxBaseScoreTime-m_cbMinBaseScoreTime) : 0);
		break;
	default:
		break;
	}
	return uElapse;
}

//进程目录
bool CAndroidUserItemSink::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
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

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);
