#include "Stdafx.h"
#include "AndroidUserItemSink.h"

////////////////////////////////////////////////////////////////////////////////////////
BYTE CAndroidUserItemSink::m_cbMinSetTableTime = 0;
BYTE CAndroidUserItemSink::m_cbMaxSetTableTime = 0;
BYTE CAndroidUserItemSink::m_cbMinStartTime = 0;
BYTE CAndroidUserItemSink::m_cbMaxStartTime = 0;
BYTE CAndroidUserItemSink::m_cbMinTributeTime = 0;
BYTE CAndroidUserItemSink::m_cbMaxTributeTime = 0;
BYTE CAndroidUserItemSink::m_cbMinBackTributeTime = 0;
BYTE CAndroidUserItemSink::m_cbMaxBackTributeTime = 0;
BYTE CAndroidUserItemSink::m_cbMinOutCardTime = 0;	
BYTE CAndroidUserItemSink::m_cbMaxOutCardTime = 0;
BYTE CAndroidUserItemSink::m_cbMinSetSeries = 0;
BYTE CAndroidUserItemSink::m_cbMaxSetSeries = 0;
bool CAndroidUserItemSink::m_bLoadConfig = false;
////////////////////////////////////////////////////////////////////////////////////////
//宏定义
#define IDI_START_GAME				(0)			//开始时间
#define IDI_OUT_CARD				(1)			//出牌时间
#define IDI_JINGONG					(2)			//进贡定时器
#define	IDI_HUIGONG					(3)			//回贡定时器
#define IDI_REQUEST					(4)			//响应请求
#define IDI_SET_TABLE				(5)			//设置桌子
////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//初始化变量
	m_cbTributeCard = 0;
	m_wFromUser = INVALID_CHAIR;
	m_cbHandCardCount = 0;												//扑克数目
	ZeroMemory(m_cbHandCardData, MAX_COUNT);							//手上扑克
	m_cbTurnCardCount = 0;												//出牌数目
	ZeroMemory(m_cbTurnCardData, MAX_COUNT);							//出牌列表
	ZeroMemory(m_cbAllCardCount, sizeof(m_cbAllCardCount));				//所有牌数目
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));				//所有牌

	m_wCurrentUser = INVALID_CHAIR;
	m_wOutCardUser = INVALID_CHAIR;										//出牌玩家
	m_wRequestQuitID = INVALID_CHAIR;									//请求退出玩家	

	m_cbCurSeries = 0;
	m_lBaseScore = 0L;

	//接口变量
	m_pIAndroidUserItem = NULL;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{

}

//接口查询
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
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

	if (!m_bLoadConfig)
	{
		ReadAndroidConfig();

		m_bLoadConfig = true;
	}

	return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//开始游戏
		{
			//开始判断
			m_pIAndroidUserItem->SendUserReady(NULL,0);
			return true;
		}
	case IDI_OUT_CARD:		//用户出牌
		{
			SearchOutCardData();
			return true;
		}
	case IDI_JINGONG:
		{
			//创建消息
 			CMD_C_PayTribute PayTribute;
 			PayTribute.cbCard = m_cbTributeCard;
 			m_pIAndroidUserItem->SendSocketData(SUB_C_PAY_TRIBUTE, &PayTribute, sizeof(PayTribute));
			return true;
		}
	case IDI_HUIGONG:
		{
			//创建消息
 			CMD_C_PayTribute PayTribute;
			PayTribute.cbCard = m_cbTributeCard;
			m_pIAndroidUserItem->SendSocketData(SUB_C_RETURN_TRIBUTE, &PayTribute, sizeof(PayTribute));
			return true;
		}
	case  IDI_REQUEST:		//响应请求
		{
			//发送回复
			CMD_C_ResposeQuit  answer;
			answer.wChairID = m_wRequestQuitID;
			answer.cbResponseQuitType = 1+rand()%2;
			m_pIAndroidUserItem->SendSocketData(SUB_C_RESPONSE_QUIT,&answer,sizeof(answer));
			return true;
		}
	}

	return false;
}

//游戏消息;
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_ALL_CARD:	//所有扑克;
		{
			//数据校验
			ASSERT(wDataSize == sizeof(CMD_S_AllCard));
			if (wDataSize != sizeof(CMD_S_AllCard)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubGetAllCard(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubGetAllCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_SEND_CARD:	//发牌消息;
		{
			//数据校验
			ASSERT(wDataSize == sizeof(CMD_S_Send_Card));
			if (wDataSize != sizeof(CMD_S_Send_Card)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubSendCard(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubSendCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_START_OUT:	//游戏开始;
		{
			//数据校验
			ASSERT(wDataSize == sizeof(CMD_S_StartOut));
			if (wDataSize != sizeof(CMD_S_StartOut)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubStartOutCard(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubStartOutCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_OUT_CARD:	//用户出牌;
		{
			CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);
			//效验数据;
			if (wDataSize<wHeadSize) return false;
			if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubOutCard(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubOutCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_PASS_CARD:	//用户放弃;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(CMD_S_PassCard));
			if (wDataSize != sizeof(CMD_S_PassCard)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubPassCard(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubPassCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}	
	case SUB_S_PAY_TRIBUTE_NOTIFY: //进贡开始;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(CMD_S_PayTributeNotify));
			if (wDataSize != sizeof(CMD_S_PayTributeNotify)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubTributeCardNotify(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubTributeCardNotify"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_PAY_TRIBUTE_CARD:	//进贡的牌;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(CMD_S_PayTributeCard));
			if (wDataSize != sizeof(CMD_S_PayTributeCard)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubTributeOneCard(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubTributeOneCard"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_PAY_TRIBUTE_RESULT: //进贡、回贡结果;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(CMD_S_PayTributeResult));
			if (wDataSize != sizeof(CMD_S_PayTributeResult)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubTributeCardResult(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubTributeCardResult"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_RETURN_TRIBUTE:		//回贡结果;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(TributeInfo));
			if (wDataSize != sizeof(TributeInfo)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubReturnTribute(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubReturnTribute"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_REQUEST_QUIT: //请求离开;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(CMD_S_RequestQuit));
			if (wDataSize != sizeof(CMD_S_RequestQuit)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubRequestQuit(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubRequestQuit"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_GAME_END:	//游戏结束;
		{
			//数据校验;
			ASSERT(wDataSize == sizeof(CMD_S_GameEnd));
			if (wDataSize != sizeof(CMD_S_GameEnd)) return false;

			bool bSuccess = false;
			try
			{
				bSuccess = OnSubGameEnd(pData, wDataSize);
			}
			catch (...)
			{
				CTraceService::TraceString(TEXT("CAndroidUserItemSink OnSubGameEnd"), TraceLevel_Exception);
				bSuccess = false;
				ASSERT(FALSE);
			}

			return bSuccess;
		}
	case SUB_S_USER_TRUST:	//用户托管;
		{
			return true;
		}
	case SUB_S_USER_OUT_OVER:
		{
			return true;
		}
	case  SUB_S_RESPONSE_QUIT:		
		{
			return true;
		}
	case SUB_S_OPPOSITE_CARD:
		{
			return true;
		}
	case SUB_S_FORCE_QUIT:
		{
			return true;
		}
	default:
		break;
	}

	//错误断言
	ASSERT(FALSE);
	return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch(cbGameStatus)
	{
		case GS_WK_FREE:			//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if (pIServerUserItem==NULL) return false;

			//变量定义
			CMD_S_StatusFree *pStatusFree=(CMD_S_StatusFree *)pData;

			m_cbCurSeries = pStatusFree->cbCurSeries;

			m_lBaseScore = pStatusFree->lBaseScore;
			if (pIServerUserItem->GetUserStatus() == US_SIT)
			{
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, GetAndroidOpTime(IDI_START_GAME));
			}

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);
	return false;
}

//进贡扑克
bool CAndroidUserItemSink::OnSubTributeCardNotify(void * pData, WORD wDataSize)
{
	//消息处理
	CMD_S_PayTributeNotify *pPayTribute = (CMD_S_PayTributeNotify *)pData;
	//自己椅子
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	if (pPayTribute->cbPayStatus == enNone || pPayTribute->cbPayStatus == enKang_Gong)
	{
		return true;
	}
	else if(pPayTribute->cbPayStatus==enJin_Gong)
	{
		ASSERT(m_cbHandCardCount == 27);

		//选择进贡牌
		BYTE cbTributeCard = m_GameLogic.GetTributeCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount);
		ASSERT(cbTributeCard >= 0x01 && cbTributeCard <= 0x4F);
		//删除进贡牌
		m_GameLogic.RemoveCard(&cbTributeCard, 1, m_cbHandCardData, m_cbHandCardCount);
		m_cbHandCardCount--;
		m_cbTributeCard = cbTributeCard;
		m_wFromUser = INVALID_CHAIR;
		m_pIAndroidUserItem->SetGameTimer(IDI_JINGONG, GetAndroidOpTime(IDI_JINGONG));

		return true;
	}

	return false;
}

//进贡扑克;
bool CAndroidUserItemSink::OnSubTributeOneCard(void * pData, WORD wDataSize)
{
	return true;
}

//进回贡结束
bool CAndroidUserItemSink::OnSubTributeCardResult(void * pData, WORD wDataSize)
{
	//消息处理
	CMD_S_PayTributeResult *pPayTribute = (CMD_S_PayTributeResult *)pData;

	//自己椅子
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	for (WORD i = 0; i < 2; i++)
	{
		if (pPayTribute->stTribute[i].wTo != INVALID_CHAIR)
		{
			if (pPayTribute->stTribute[i].wTo == wMeChairID)
			{
				ASSERT(m_cbHandCardCount == 27);
				//加入别人进贡的牌
				m_cbHandCardData[m_cbHandCardCount++] = pPayTribute->stTribute[i].cbCard;
				m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);
				ASSERT(pPayTribute->stTribute[i].cbCard >= 0x01 && pPayTribute->stTribute[i].cbCard <= 0x4F);
				m_wFromUser = pPayTribute->stTribute[i].wFrom;

				//选取回贡的牌
				bool bToFriend = (m_wFromUser == (wMeChairID + 2) % GAME_PLAYER);
				BYTE cbBackTributeCard = m_GameLogic.GetBackTributeCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount, 2, bToFriend);
				ASSERT(cbBackTributeCard >= 0x01 && cbBackTributeCard <= 0x4F);
				//删除回贡牌
				m_GameLogic.RemoveCard(&cbBackTributeCard, 1, m_cbHandCardData, m_cbHandCardCount);
				m_cbHandCardCount--;
				m_GameLogic.SetUserHandCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount);
				m_cbTributeCard = cbBackTributeCard;

				m_pIAndroidUserItem->SetGameTimer(IDI_HUIGONG, GetAndroidOpTime(IDI_HUIGONG));
				break;
			}
		}
	}
	return true;
}

//回贡
bool CAndroidUserItemSink::OnSubReturnTribute(void * pData, WORD wDataSize)
{
	//消息处理
	TributeInfo *pTributeInfo = (TributeInfo *)pData;

	//自己椅子
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	if (pTributeInfo->wTo != INVALID_CHAIR && pTributeInfo->wTo == wMeChairID)
	{
		ASSERT(m_cbHandCardCount == 26);
		//加入别人进贡的牌
		m_cbHandCardData[m_cbHandCardCount++] = pTributeInfo->cbCard;
		m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);
		m_GameLogic.SetUserHandCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount);
	}

	return true;
}

//游戏开始
bool CAndroidUserItemSink::OnSubSendCard(void * pData, WORD wDataSize)
{	
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_JINGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_HUIGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_REQUEST);

	//重置数据
	m_cbHandCardCount = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	m_cbTurnCardCount = 0;
	ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbAllCardCount, sizeof(m_cbAllCardCount));
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));
	m_cbTributeCard = 0;
	m_wFromUser = INVALID_CHAIR;
	m_wOutCardUser = INVALID_CHAIR;
	m_wRequestQuitID = INVALID_CHAIR;
    
	CMD_S_Send_Card *pSendCard = (CMD_S_Send_Card *)pData;
	BYTE cbMainValue = pSendCard->stSeries.cbCurrentSeries < 14 ? (pSendCard->stSeries.cbCurrentSeries & 0x0F) : 0x01;
	m_GameLogic.SetMainValue(cbMainValue);
	m_GameLogic.SetInitData();
	m_cbHandCardCount = 27;
	CopyMemory(m_cbHandCardData, pSendCard->cbCardData, sizeof(BYTE)*m_cbHandCardCount);
	m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);
	return true;
}

//庄家信息
bool CAndroidUserItemSink::OnSubStartOutCard(void * pData, WORD wDataSize)
{
	//校验数据
	ASSERT(wDataSize == sizeof(CMD_S_StartOut));
	if (wDataSize != sizeof(CMD_S_StartOut)) return false;

	//变量声明
	CMD_S_StartOut *pStartOut = (CMD_S_StartOut*) pData;

	m_wCurrentUser = pStartOut->wCurrentUser;

	//拷贝扑克
	CopyMemory(m_cbHandCardData, pStartOut->cbHandCardData, 27);
	m_cbHandCardCount = 27;
	m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);

	//设置定时器
	if (pStartOut->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, GetAndroidOpTime(IDI_OUT_CARD));
	}

	return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard(void * pData, WORD wDataSize)
{
	//变量定义
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//效验数据
	if (wDataSize<wHeadSize) return false;
	if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;


	//设置参数
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	m_wCurrentUser = pOutCard->wCurrentUser;
	m_wOutCardUser = pOutCard->wOutCardUser;

	CopyMemory(m_cbTurnCardData, pOutCard->cbCardData, pOutCard->cbCardCount);

	m_GameLogic.SetOutCardImfomation(wMeChairID,pOutCard->wOutCardUser,pOutCard->wCurrentUser,m_cbHandCardData,m_cbHandCardCount,pOutCard->cbCardData,pOutCard->cbCardCount,true,m_cbTurnCardCount==0);
	m_cbTurnCardCount = pOutCard->cbCardCount;

	//删除自己扑克
	if (pOutCard->wOutCardUser == m_pIAndroidUserItem->GetChairID())
	{
		m_GameLogic.RemoveCard(m_cbTurnCardData, m_cbTurnCardCount, m_cbHandCardData, m_cbHandCardCount);
		m_cbHandCardCount -= pOutCard->cbCardCount;
	}

	//删除扑克
	{
		m_GameLogic.RemoveCard(m_cbTurnCardData, m_cbTurnCardCount, m_cbAllCardData[pOutCard->wOutCardUser], m_cbAllCardCount[pOutCard->wOutCardUser]);
		m_cbAllCardCount[pOutCard->wOutCardUser] -= m_cbTurnCardCount;
		m_GameLogic.OnHandCardChange(m_cbAllCardData, m_cbAllCardCount);
	}

	//设置定时器
	if (pOutCard->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, GetAndroidOpTime(IDI_OUT_CARD));
	}
    
	return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubPassCard(void * pData, WORD wDataSize)
{
	//参数声明
	CMD_S_PassCard *pPassCard = (CMD_S_PassCard *) pData;
	
	m_wCurrentUser = pPassCard->wCurrentUser;

	//参数设置
	WORD wMeChairID =m_pIAndroidUserItem->GetChairID();
	m_GameLogic.SetOutCardImfomation(wMeChairID,pPassCard->wPassUser,pPassCard->wCurrentUser,m_cbHandCardData,m_cbHandCardCount,m_cbTurnCardData,m_cbTurnCardCount,false,false);

	//清空出牌数据
	if (pPassCard ->bNewTurn)
	{
		ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
		m_cbTurnCardCount = 0;
	}
	
	//设置定时器
	if (pPassCard->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, GetAndroidOpTime(IDI_OUT_CARD));
	}

	return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//删除时间
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_JINGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_HUIGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_REQUEST);

	//重置数据
	m_cbHandCardCount = 0;
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	m_cbTurnCardCount = 0;
	ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
	ZeroMemory(m_cbAllCardCount, sizeof(m_cbAllCardCount));
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));
	m_cbTributeCard = 0;
	m_wFromUser = INVALID_CHAIR;
	m_wCurrentUser = INVALID_CHAIR;
	m_wOutCardUser = INVALID_CHAIR;
	m_wRequestQuitID = INVALID_CHAIR;

	//发送准备消息
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, GetAndroidOpTime(IDI_START_GAME));

	return true;
}

//响应玩家退出请求
bool CAndroidUserItemSink::OnSubRequestQuit( void * pData, WORD wDataSize )
{
	CMD_S_RequestQuit *pRequest = (CMD_S_RequestQuit *)pData;
	m_wRequestQuitID = pRequest->wChairID;
	m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST, GetAndroidOpTime(IDI_REQUEST));
	return true;
}

//获取玩家所有手牌
bool CAndroidUserItemSink::OnSubGetAllCard( void * pData, WORD wDataSize )
{
	CMD_S_AllCard *pAllCard = (CMD_S_AllCard*) pData;
	CopyMemory(m_cbAllCardCount, pAllCard->cbHandCardCount, sizeof(m_cbAllCardCount));
	CopyMemory(m_cbAllCardData, pAllCard->cbHandCardData, sizeof(m_cbAllCardData));
	m_GameLogic.OnHandCardChange(m_cbAllCardData, m_cbAllCardCount);
	return true;
}

//搜索出牌
void CAndroidUserItemSink::SearchOutCardData()
{
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	//扑克分析
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	m_GameLogic.SearchOutCardAI(wMeChairID, m_wOutCardUser, m_cbHandCardData, m_cbHandCardCount, m_cbTurnCardData, m_cbTurnCardCount, OutCardResult);

	//首家出牌
	if (m_cbTurnCardCount == 0)
	{
		ASSERT(OutCardResult.cbCardCount > 0);
		BYTE cbType = m_GameLogic.GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
		ASSERT(cbType != AI_CT_ERROR);

		if (cbType != AI_CT_BOMB_TW && OutCardResult.cbResultCard[0] >= 0x4E && OutCardResult.cbCardCount >= 3)
		{
			OutCardResult.cbCardCount = 1;
		}

		if (cbType == AI_CT_ERROR)
		{
			OutCardResult.cbCardCount = 1;
			OutCardResult.cbResultCard[0] = m_cbHandCardData[m_cbHandCardCount-1];
		}

		//发送出牌消息
		if (OutCardResult.cbCardCount > 0)
		{
			CMD_C_OutCard OutCardData;
			ZeroMemory(&OutCardData, sizeof(OutCardData));

			OutCardData.cbCardCount = OutCardResult.cbCardCount;
			CopyMemory(OutCardData.cbCardData, OutCardResult.cbResultCard, OutCardResult.cbCardCount*sizeof(BYTE));
			m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCardData, sizeof(OutCardData)-sizeof(OutCardData.cbCardData)+OutCardData.cbCardCount*sizeof(BYTE));
		}
	}
	//接牌
	else	
	{
		if (OutCardResult.cbCardCount == 0)
		{
			//发送PASS消息
			m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
		}
		else
		{
			BYTE cbType = m_GameLogic.GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			if (cbType != AI_CT_BOMB_TW && OutCardResult.cbResultCard[0] >= 0x4E && OutCardResult.cbCardCount >= 3)
			{
				//发送PASS消息
				m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
			}

			bool bCanOut = m_GameLogic.CompareMagicCard(m_cbTurnCardData, OutCardResult.cbResultCard ,m_cbTurnCardCount, OutCardResult.cbCardCount);
			//ASSERT(bCanOut);
			if (bCanOut == false)
			{
				//发送PASS消息
				m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
			}

			//发送出牌消息
			if (OutCardResult.cbCardCount > 0)
			{
				CMD_C_OutCard OutCardData;
				ZeroMemory(&OutCardData, sizeof(OutCardData));

				OutCardData.cbCardCount = OutCardResult.cbCardCount;
				CopyMemory(OutCardData.cbCardData, OutCardResult.cbResultCard, OutCardResult.cbCardCount);
				m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCardData, sizeof(OutCardData)-sizeof(OutCardData.cbCardData)+OutCardData.cbCardCount*sizeof(BYTE));
			}
		}
	}

	//::OutputDebugString(_T("机器人出牌"));
}

//设置机器人出牌时间 
bool CAndroidUserItemSink::ReadAndroidConfig(void)
{
	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szPath),szPath);	
	TCHAR szFileName[MAX_PATH];
	_sntprintf_s(szFileName, sizeof(szFileName), TEXT("%s\\GuanDanServer.ini"), szPath);

	try
	{
		//最小设置时间
		m_cbMinSetTableTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinSetTableTime"), 2, szFileName); 
		//最大设置时间
		m_cbMaxSetTableTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxSetTableTime"), 5, szFileName); 
		ASSERT(m_cbMaxSetTableTime >= m_cbMinSetTableTime);
		//最小开始时间
		m_cbMinStartTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinStartTime"), 2, szFileName); 
		//最大开始时间
		m_cbMaxStartTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxStartTime"), 10, szFileName);
		ASSERT(m_cbMaxStartTime >= m_cbMinStartTime);
		//最小进贡时间
		m_cbMinTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinTributeTime"), 2, szFileName); 
		//最大进贡时间
		m_cbMaxTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxTributeTime"), 10, szFileName);
		ASSERT(m_cbMaxTributeTime >= m_cbMinTributeTime);
		//最小回贡时间
		m_cbMinBackTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinBackTributeTime"), 2, szFileName); 
		//最大回贡时间
		m_cbMaxBackTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxBackTributeTime"), 10, szFileName); 
		ASSERT(m_cbMaxBackTributeTime >= m_cbMinBackTributeTime);
		//最小出牌时间
		m_cbMinOutCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinOutCardTime"), 2, szFileName); 
		//最大出牌时间
		m_cbMaxOutCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxOutCardTime"), 10, szFileName); 
		ASSERT(m_cbMaxOutCardTime >= m_cbMinOutCardTime);

		//最小设置级数
		m_cbMinSetSeries = GetPrivateProfileInt(TEXT("AndroidSeries"), TEXT("MinSetSeries"), 3, szFileName);
		//最大设置级数
		m_cbMaxSetSeries = GetPrivateProfileInt(TEXT("AndroidSeries"), TEXT("MaxSetSeries"), 8, szFileName);
		ASSERT(m_cbMaxSetSeries >= m_cbMinSetSeries);
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
	case IDI_START_GAME:
		uElapse = m_cbMinStartTime + ((m_cbMaxStartTime-m_cbMinStartTime)>0 ? rand()%(m_cbMaxStartTime-m_cbMinStartTime) : 0);
		break;
	case IDI_OUT_CARD:
		uElapse = m_cbMinOutCardTime + ((m_cbMaxOutCardTime-m_cbMinOutCardTime)>0 ? rand()%(m_cbMaxOutCardTime-m_cbMinOutCardTime) : 0);
		break;
	case IDI_JINGONG:
		uElapse = m_cbMinTributeTime + ((m_cbMaxTributeTime-m_cbMinTributeTime)>0 ? rand()%(m_cbMaxTributeTime-m_cbMinTributeTime) : 0);
		break;
	case IDI_HUIGONG:
		uElapse = m_cbMinBackTributeTime + ((m_cbMaxBackTributeTime-m_cbMinBackTributeTime)>0 ? rand()%(m_cbMaxBackTributeTime-m_cbMinBackTributeTime) : 0);
		break;
	case IDI_REQUEST:
		uElapse = 1 + rand()%3;
		break;
	default:
		break;
	}

	//CString strMsg;
	//strMsg.Format(_T("GetAndroidOpTime OpType=%d, Time=%d\n"), uOpType, uElapse);
	//::OutputDebugString(strMsg);

	return uElapse;
}

//////////////////////////////////////////////////////////////////////////
