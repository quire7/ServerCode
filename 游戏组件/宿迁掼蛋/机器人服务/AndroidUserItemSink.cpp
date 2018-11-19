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
//�궨��
#define IDI_START_GAME				(0)			//��ʼʱ��
#define IDI_OUT_CARD				(1)			//����ʱ��
#define IDI_JINGONG					(2)			//������ʱ��
#define	IDI_HUIGONG					(3)			//�ع���ʱ��
#define IDI_REQUEST					(4)			//��Ӧ����
#define IDI_SET_TABLE				(5)			//��������
////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��ʼ������
	m_cbTributeCard = 0;
	m_wFromUser = INVALID_CHAIR;
	m_cbHandCardCount = 0;												//�˿���Ŀ
	ZeroMemory(m_cbHandCardData, MAX_COUNT);							//�����˿�
	m_cbTurnCardCount = 0;												//������Ŀ
	ZeroMemory(m_cbTurnCardData, MAX_COUNT);							//�����б�
	ZeroMemory(m_cbAllCardCount, sizeof(m_cbAllCardCount));				//��������Ŀ
	ZeroMemory(m_cbAllCardData, sizeof(m_cbAllCardData));				//������

	m_wCurrentUser = INVALID_CHAIR;
	m_wOutCardUser = INVALID_CHAIR;										//�������
	m_wRequestQuitID = INVALID_CHAIR;									//�����˳����	

	m_cbCurSeries = 0;
	m_lBaseScore = 0L;

	//�ӿڱ���
	m_pIAndroidUserItem = NULL;
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{

}

//�ӿڲ�ѯ
VOID * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
	if (m_pIAndroidUserItem==NULL) return false;

	if (!m_bLoadConfig)
	{
		ReadAndroidConfig();

		m_bLoadConfig = true;
	}

	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//��ʼ��Ϸ
		{
			//��ʼ�ж�
			m_pIAndroidUserItem->SendUserReady(NULL,0);
			return true;
		}
	case IDI_OUT_CARD:		//�û�����
		{
			SearchOutCardData();
			return true;
		}
	case IDI_JINGONG:
		{
			//������Ϣ
 			CMD_C_PayTribute PayTribute;
 			PayTribute.cbCard = m_cbTributeCard;
 			m_pIAndroidUserItem->SendSocketData(SUB_C_PAY_TRIBUTE, &PayTribute, sizeof(PayTribute));
			return true;
		}
	case IDI_HUIGONG:
		{
			//������Ϣ
 			CMD_C_PayTribute PayTribute;
			PayTribute.cbCard = m_cbTributeCard;
			m_pIAndroidUserItem->SendSocketData(SUB_C_RETURN_TRIBUTE, &PayTribute, sizeof(PayTribute));
			return true;
		}
	case  IDI_REQUEST:		//��Ӧ����
		{
			//���ͻظ�
			CMD_C_ResposeQuit  answer;
			answer.wChairID = m_wRequestQuitID;
			answer.cbResponseQuitType = 1+rand()%2;
			m_pIAndroidUserItem->SendSocketData(SUB_C_RESPONSE_QUIT,&answer,sizeof(answer));
			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ;
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_ALL_CARD:	//�����˿�;
		{
			//����У��
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
	case SUB_S_SEND_CARD:	//������Ϣ;
		{
			//����У��
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
	case SUB_S_START_OUT:	//��Ϸ��ʼ;
		{
			//����У��
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
	case SUB_S_OUT_CARD:	//�û�����;
		{
			CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
			WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);
			//Ч������;
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
	case SUB_S_PASS_CARD:	//�û�����;
		{
			//����У��;
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
	case SUB_S_PAY_TRIBUTE_NOTIFY: //������ʼ;
		{
			//����У��;
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
	case SUB_S_PAY_TRIBUTE_CARD:	//��������;
		{
			//����У��;
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
	case SUB_S_PAY_TRIBUTE_RESULT: //�������ع����;
		{
			//����У��;
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
	case SUB_S_RETURN_TRIBUTE:		//�ع����;
		{
			//����У��;
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
	case SUB_S_REQUEST_QUIT: //�����뿪;
		{
			//����У��;
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
	case SUB_S_GAME_END:	//��Ϸ����;
		{
			//����У��;
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
	case SUB_S_USER_TRUST:	//�û��й�;
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

	//�������
	ASSERT(FALSE);
	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch(cbGameStatus)
	{
		case GS_WK_FREE:			//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			IServerUserItem *pIServerUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if (pIServerUserItem==NULL) return false;

			//��������
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

	//�������
	ASSERT(FALSE);
	return false;
}

//�����˿�
bool CAndroidUserItemSink::OnSubTributeCardNotify(void * pData, WORD wDataSize)
{
	//��Ϣ����
	CMD_S_PayTributeNotify *pPayTribute = (CMD_S_PayTributeNotify *)pData;
	//�Լ�����
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	if (pPayTribute->cbPayStatus == enNone || pPayTribute->cbPayStatus == enKang_Gong)
	{
		return true;
	}
	else if(pPayTribute->cbPayStatus==enJin_Gong)
	{
		ASSERT(m_cbHandCardCount == 27);

		//ѡ�������
		BYTE cbTributeCard = m_GameLogic.GetTributeCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount);
		ASSERT(cbTributeCard >= 0x01 && cbTributeCard <= 0x4F);
		//ɾ��������
		m_GameLogic.RemoveCard(&cbTributeCard, 1, m_cbHandCardData, m_cbHandCardCount);
		m_cbHandCardCount--;
		m_cbTributeCard = cbTributeCard;
		m_wFromUser = INVALID_CHAIR;
		m_pIAndroidUserItem->SetGameTimer(IDI_JINGONG, GetAndroidOpTime(IDI_JINGONG));

		return true;
	}

	return false;
}

//�����˿�;
bool CAndroidUserItemSink::OnSubTributeOneCard(void * pData, WORD wDataSize)
{
	return true;
}

//���ع�����
bool CAndroidUserItemSink::OnSubTributeCardResult(void * pData, WORD wDataSize)
{
	//��Ϣ����
	CMD_S_PayTributeResult *pPayTribute = (CMD_S_PayTributeResult *)pData;

	//�Լ�����
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	for (WORD i = 0; i < 2; i++)
	{
		if (pPayTribute->stTribute[i].wTo != INVALID_CHAIR)
		{
			if (pPayTribute->stTribute[i].wTo == wMeChairID)
			{
				ASSERT(m_cbHandCardCount == 27);
				//������˽�������
				m_cbHandCardData[m_cbHandCardCount++] = pPayTribute->stTribute[i].cbCard;
				m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);
				ASSERT(pPayTribute->stTribute[i].cbCard >= 0x01 && pPayTribute->stTribute[i].cbCard <= 0x4F);
				m_wFromUser = pPayTribute->stTribute[i].wFrom;

				//ѡȡ�ع�����
				bool bToFriend = (m_wFromUser == (wMeChairID + 2) % GAME_PLAYER);
				BYTE cbBackTributeCard = m_GameLogic.GetBackTributeCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount, 2, bToFriend);
				ASSERT(cbBackTributeCard >= 0x01 && cbBackTributeCard <= 0x4F);
				//ɾ���ع���
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

//�ع�
bool CAndroidUserItemSink::OnSubReturnTribute(void * pData, WORD wDataSize)
{
	//��Ϣ����
	TributeInfo *pTributeInfo = (TributeInfo *)pData;

	//�Լ�����
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	if (pTributeInfo->wTo != INVALID_CHAIR && pTributeInfo->wTo == wMeChairID)
	{
		ASSERT(m_cbHandCardCount == 26);
		//������˽�������
		m_cbHandCardData[m_cbHandCardCount++] = pTributeInfo->cbCard;
		m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);
		m_GameLogic.SetUserHandCard(wMeChairID, m_cbHandCardData, m_cbHandCardCount);
	}

	return true;
}

//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubSendCard(void * pData, WORD wDataSize)
{	
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_JINGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_HUIGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_REQUEST);

	//��������
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

//ׯ����Ϣ
bool CAndroidUserItemSink::OnSubStartOutCard(void * pData, WORD wDataSize)
{
	//У������
	ASSERT(wDataSize == sizeof(CMD_S_StartOut));
	if (wDataSize != sizeof(CMD_S_StartOut)) return false;

	//��������
	CMD_S_StartOut *pStartOut = (CMD_S_StartOut*) pData;

	m_wCurrentUser = pStartOut->wCurrentUser;

	//�����˿�
	CopyMemory(m_cbHandCardData, pStartOut->cbHandCardData, 27);
	m_cbHandCardCount = 27;
	m_GameLogic.SortCardList(m_cbHandCardData, m_cbHandCardCount, ST_ORDER);

	//���ö�ʱ��
	if (pStartOut->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, GetAndroidOpTime(IDI_OUT_CARD));
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubOutCard(void * pData, WORD wDataSize)
{
	//��������
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;
	WORD wHeadSize=sizeof(CMD_S_OutCard)-sizeof(pOutCard->cbCardData);

	//Ч������
	if (wDataSize<wHeadSize) return false;
	if (wDataSize!=(wHeadSize+pOutCard->cbCardCount*sizeof(pOutCard->cbCardData[0]))) return false;


	//���ò���
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	m_wCurrentUser = pOutCard->wCurrentUser;
	m_wOutCardUser = pOutCard->wOutCardUser;

	CopyMemory(m_cbTurnCardData, pOutCard->cbCardData, pOutCard->cbCardCount);

	m_GameLogic.SetOutCardImfomation(wMeChairID,pOutCard->wOutCardUser,pOutCard->wCurrentUser,m_cbHandCardData,m_cbHandCardCount,pOutCard->cbCardData,pOutCard->cbCardCount,true,m_cbTurnCardCount==0);
	m_cbTurnCardCount = pOutCard->cbCardCount;

	//ɾ���Լ��˿�
	if (pOutCard->wOutCardUser == m_pIAndroidUserItem->GetChairID())
	{
		m_GameLogic.RemoveCard(m_cbTurnCardData, m_cbTurnCardCount, m_cbHandCardData, m_cbHandCardCount);
		m_cbHandCardCount -= pOutCard->cbCardCount;
	}

	//ɾ���˿�
	{
		m_GameLogic.RemoveCard(m_cbTurnCardData, m_cbTurnCardCount, m_cbAllCardData[pOutCard->wOutCardUser], m_cbAllCardCount[pOutCard->wOutCardUser]);
		m_cbAllCardCount[pOutCard->wOutCardUser] -= m_cbTurnCardCount;
		m_GameLogic.OnHandCardChange(m_cbAllCardData, m_cbAllCardCount);
	}

	//���ö�ʱ��
	if (pOutCard->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, GetAndroidOpTime(IDI_OUT_CARD));
	}
    
	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubPassCard(void * pData, WORD wDataSize)
{
	//��������
	CMD_S_PassCard *pPassCard = (CMD_S_PassCard *) pData;
	
	m_wCurrentUser = pPassCard->wCurrentUser;

	//��������
	WORD wMeChairID =m_pIAndroidUserItem->GetChairID();
	m_GameLogic.SetOutCardImfomation(wMeChairID,pPassCard->wPassUser,pPassCard->wCurrentUser,m_cbHandCardData,m_cbHandCardCount,m_cbTurnCardData,m_cbTurnCardCount,false,false);

	//��ճ�������
	if (pPassCard ->bNewTurn)
	{
		ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));
		m_cbTurnCardCount = 0;
	}
	
	//���ö�ʱ��
	if (pPassCard->wCurrentUser == m_pIAndroidUserItem->GetChairID())
	{
		m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD, GetAndroidOpTime(IDI_OUT_CARD));
	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//ɾ��ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);
	m_pIAndroidUserItem->KillGameTimer(IDI_JINGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_HUIGONG);
	m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);
	m_pIAndroidUserItem->KillGameTimer(IDI_REQUEST);

	//��������
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

	//����׼����Ϣ
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME, GetAndroidOpTime(IDI_START_GAME));

	return true;
}

//��Ӧ����˳�����
bool CAndroidUserItemSink::OnSubRequestQuit( void * pData, WORD wDataSize )
{
	CMD_S_RequestQuit *pRequest = (CMD_S_RequestQuit *)pData;
	m_wRequestQuitID = pRequest->wChairID;
	m_pIAndroidUserItem->SetGameTimer(IDI_REQUEST, GetAndroidOpTime(IDI_REQUEST));
	return true;
}

//��ȡ�����������
bool CAndroidUserItemSink::OnSubGetAllCard( void * pData, WORD wDataSize )
{
	CMD_S_AllCard *pAllCard = (CMD_S_AllCard*) pData;
	CopyMemory(m_cbAllCardCount, pAllCard->cbHandCardCount, sizeof(m_cbAllCardCount));
	CopyMemory(m_cbAllCardData, pAllCard->cbHandCardData, sizeof(m_cbAllCardData));
	m_GameLogic.OnHandCardChange(m_cbAllCardData, m_cbAllCardCount);
	return true;
}

//��������
void CAndroidUserItemSink::SearchOutCardData()
{
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	//�˿˷���
	tagOutCardResult OutCardResult;
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	m_GameLogic.SearchOutCardAI(wMeChairID, m_wOutCardUser, m_cbHandCardData, m_cbHandCardCount, m_cbTurnCardData, m_cbTurnCardCount, OutCardResult);

	//�׼ҳ���
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

		//���ͳ�����Ϣ
		if (OutCardResult.cbCardCount > 0)
		{
			CMD_C_OutCard OutCardData;
			ZeroMemory(&OutCardData, sizeof(OutCardData));

			OutCardData.cbCardCount = OutCardResult.cbCardCount;
			CopyMemory(OutCardData.cbCardData, OutCardResult.cbResultCard, OutCardResult.cbCardCount*sizeof(BYTE));
			m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD, &OutCardData, sizeof(OutCardData)-sizeof(OutCardData.cbCardData)+OutCardData.cbCardCount*sizeof(BYTE));
		}
	}
	//����
	else	
	{
		if (OutCardResult.cbCardCount == 0)
		{
			//����PASS��Ϣ
			m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
		}
		else
		{
			BYTE cbType = m_GameLogic.GetMagicCardType(OutCardResult.cbResultCard, OutCardResult.cbCardCount);
			if (cbType != AI_CT_BOMB_TW && OutCardResult.cbResultCard[0] >= 0x4E && OutCardResult.cbCardCount >= 3)
			{
				//����PASS��Ϣ
				m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
			}

			bool bCanOut = m_GameLogic.CompareMagicCard(m_cbTurnCardData, OutCardResult.cbResultCard ,m_cbTurnCardCount, OutCardResult.cbCardCount);
			//ASSERT(bCanOut);
			if (bCanOut == false)
			{
				//����PASS��Ϣ
				m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
			}

			//���ͳ�����Ϣ
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

	//::OutputDebugString(_T("�����˳���"));
}

//���û����˳���ʱ�� 
bool CAndroidUserItemSink::ReadAndroidConfig(void)
{
	//�����ļ���
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szPath),szPath);	
	TCHAR szFileName[MAX_PATH];
	_sntprintf_s(szFileName, sizeof(szFileName), TEXT("%s\\GuanDanServer.ini"), szPath);

	try
	{
		//��С����ʱ��
		m_cbMinSetTableTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinSetTableTime"), 2, szFileName); 
		//�������ʱ��
		m_cbMaxSetTableTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxSetTableTime"), 5, szFileName); 
		ASSERT(m_cbMaxSetTableTime >= m_cbMinSetTableTime);
		//��С��ʼʱ��
		m_cbMinStartTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinStartTime"), 2, szFileName); 
		//���ʼʱ��
		m_cbMaxStartTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxStartTime"), 10, szFileName);
		ASSERT(m_cbMaxStartTime >= m_cbMinStartTime);
		//��С����ʱ��
		m_cbMinTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinTributeTime"), 2, szFileName); 
		//������ʱ��
		m_cbMaxTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxTributeTime"), 10, szFileName);
		ASSERT(m_cbMaxTributeTime >= m_cbMinTributeTime);
		//��С�ع�ʱ��
		m_cbMinBackTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinBackTributeTime"), 2, szFileName); 
		//���ع�ʱ��
		m_cbMaxBackTributeTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxBackTributeTime"), 10, szFileName); 
		ASSERT(m_cbMaxBackTributeTime >= m_cbMinBackTributeTime);
		//��С����ʱ��
		m_cbMinOutCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinOutCardTime"), 2, szFileName); 
		//������ʱ��
		m_cbMaxOutCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxOutCardTime"), 10, szFileName); 
		ASSERT(m_cbMaxOutCardTime >= m_cbMinOutCardTime);

		//��С���ü���
		m_cbMinSetSeries = GetPrivateProfileInt(TEXT("AndroidSeries"), TEXT("MinSetSeries"), 3, szFileName);
		//������ü���
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

//��ȡ�����˲���ʱ��
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
