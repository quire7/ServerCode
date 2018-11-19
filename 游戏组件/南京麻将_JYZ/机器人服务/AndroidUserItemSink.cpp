#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include <strsafe.h>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////
//��Ϸʱ��
#define	IDI_ANDROID_ITEM_SINK		100
#define IDI_START_GAME_A			(IDI_ANDROID_ITEM_SINK+0)			//��ʼʱ��
#define IDI_SHOOT_DICE_A			(IDI_ANDROID_ITEM_SINK+1)			//������ʱ��
#define IDI_DISPATCH_CARD_A			(IDI_ANDROID_ITEM_SINK+2)			//���ƶ���ʱ��
#define IDI_OPERATE_CARD_A			(IDI_ANDROID_ITEM_SINK+3)			//����ʱ��
#define IDI_SET_BASESCORE_A			(IDI_ANDROID_ITEM_SINK+4)			//���õ�עʱ��
////////////////////////////////////////////////////////////////////////////////////////////
//��̬����
BYTE CAndroidUserItemSink::m_cbMinReadyTime = 2;						//��С׼��ʱ��
BYTE CAndroidUserItemSink::m_cbMaxReadyTime = 5;						//���׼��ʱ��
BYTE CAndroidUserItemSink::m_cbMinOperateCardTime = 2;					//��С����ʱ��
BYTE CAndroidUserItemSink::m_cbMaxOperateCardTime = 5;					//������ʱ��
BYTE CAndroidUserItemSink::m_cbMinBaseScoreTime = 2;					//��С���õ�עʱ��
BYTE CAndroidUserItemSink::m_cbMaxBaseScoreTime = 5;					//������õ�עʱ��

////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	m_lBaseScore = 0L;
	m_cbSetBaseScoreStatus = STATUS_SBS_ERROR;
	ReInit();
}

//��������
CAndroidUserItemSink::~CAndroidUserItemSink()
{

}

//�ӿڲ�ѯ
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
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
	
	return true;
}

//���ýӿ�
bool CAndroidUserItemSink::RepositionSink()
{
	ReInit();

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME_A:		//��ʼ��Ϸ
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
	case IDI_OPERATE_CARD_A:		//������ʱ��
		{
			ASSERT(m_pIAndroidUserItem->GetGameStatus()==GS_MJ_PLAY && m_wCurrentUser==m_pIAndroidUserItem->GetChairID());
			if (m_pIAndroidUserItem->GetGameStatus()==GS_MJ_PLAY && m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
			{
				DWORD dwAction = m_dwActionMask & (~WIK_CHU);
				if( dwAction != WIK_NULL )       //�г�������������
				{
					OnOperateCard();
				}
				else if( (m_dwActionMask & WIK_CHU) > 0 )   //���Ʋ���
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
	case IDI_SET_BASESCORE_A:		//���õ�ע��ʱ��
		{
			//������Ϣ
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

			//�����˻��в�׼�������
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

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_SET_BASESCORE:	//���õ�ע
		{
			return OnSubSetBaseScore(pData,wDataSize);
		}
	case SUB_S_SHOOT_DICE:		//��������Ϣ
		{
			return OnSubShootDice(pData,wDataSize);
		}
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//������Ϣ
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_SEND_BUHUA:		//������Ϣ
		{
            return OnSubUserBuHua(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//�û�����
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_OPERATE_NOTIFY:	//������ʾ
		{
			return OnSubOperateNotify(pData,wDataSize);
		}
	case SUB_S_OPERATE_RESULT:	//�������
		{
			return OnSubOperateResult(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_KICK_OUT:		//�߳����
		{
			m_pIAndroidUserItem->JudgeAndroidActionAndRemove(ANDROID_WAITLEAVE);
			return true;
		}
	case SUB_S_PAY_MONEY:		//С����Ϣ
		{
			return true;
		}
	case SUB_S_USER_TRUST:		//�û��й�
		{
			return true;
		}
	}

	return true;
}

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//����״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//�����õ�ע����
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
	case GS_MJ_PLAY:	//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

			//���ñ���
			m_wBankerUser=pStatusPlay->wBankerUser;
			m_wCurrentUser=pStatusPlay->wCurrentUser;
			m_cbLeftCardCount=pStatusPlay->cbLeftCardCount;

			//����״̬
			WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
//			m_bTingStatus=(pStatusPlay->cbHearStatus[wMeChairID]==TRUE)?true:false;

			//��ʷ����
			m_wOutCardUser=pStatusPlay->wOutCardUser;
			m_cbOutCardData=pStatusPlay->cbOutCardData;
			CopyMemory(m_cbDiscardCard,pStatusPlay->cbDiscardCard,sizeof(m_cbDiscardCard));
			CopyMemory(m_cbDiscardCount,pStatusPlay->cbDiscardCount,sizeof(m_cbDiscardCount));

			//��
			CopyMemory(m_cbWeaveCount,pStatusPlay->cbWeaveCount,sizeof(m_cbWeaveCount));
			CopyMemory(m_WeaveItemArray,pStatusPlay->WeaveItemArray,sizeof(m_WeaveItemArray));
			m_GameLogic.SwitchToCardIndex(pStatusPlay->cbCardData,pStatusPlay->cbCardCount[wMeChairID],m_cbCardIndex[wMeChairID]);

			////��������
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

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�������õ�ע��Ϣ
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

//��������Ϣ
bool CAndroidUserItemSink::OnSubShootDice( const void *pBuffer, WORD wDataSize )
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ShootDice));
	if (wDataSize!=sizeof(CMD_S_ShootDice)) return false;
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_DICE);

	CMD_S_ShootDice *pGameShootDice = (CMD_S_ShootDice *)pBuffer;

	m_wBankerUser = pGameShootDice->wBankerUser;
// 	//ׯ�����Լ�
// 	if (m_pIAndroidUserItem->GetChairID() == m_wBankerUser)
// 	{
// 		//����
// 		UINT nElapse = rand()%TIME_SHOOT_DICE+TIME_LESS;
// 		m_pIAndroidUserItem->SetGameTimer(IDI_SHOOT_DICE_A, nElapse);
// 	}

	return true;
}

//ׯ����Ϣ
bool CAndroidUserItemSink::OnSubGameStart(void * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCH_CARD_A);

	//��������
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	ReInit();
	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_PLAY);

	m_bHaveGameEnd = false;
	m_cbLeftCardCount = MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1);
	m_dwActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;

	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//������
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardCount = (wMeChairId==m_wBankerUser)?MAX_COUNT:(MAX_COUNT-1);
	m_GameLogic.SwitchToCardIndex(pGameStart->cbCardData,(MAX_COUNT-1),m_cbCardIndex[wMeChairId]);

	//����
	UINT nElapse = GetAndroidOpTime(IDI_DISPATCH_CARD_A);
	m_pIAndroidUserItem->SetGameTimer(IDI_DISPATCH_CARD_A, nElapse);
	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(void * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

	m_bHaveGameEnd = true;
	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_FREE);

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_SHOOT_DICE_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_DISPATCH_CARD_A);
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	//����
	UINT nElapse = GetAndroidOpTime(IDI_START_GAME_A);
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME_A, nElapse);
	return true;
}

//������ʾ
bool CAndroidUserItemSink::OnSubOperateNotify( const void *pBuffer,WORD wDataSize )
{
	//Ч������
	ASSERT(wDataSize >= sizeof(CMD_S_OperateNotify_Send));
	if (wDataSize < sizeof(CMD_S_OperateNotify_Send)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	DataStream kDataStream((void*)pBuffer, wDataSize);

	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	CMD_S_OperateNotify_Send kOperateNotify;
	kOperateNotify.StreamValue(kDataStream, false);
	m_wCurrentUser = kOperateNotify.wOperateUser;

	//�û�����
	if ((kOperateNotify.wOperateUser == wMeChairID) && (kOperateNotify.dwActionMask != WIK_NULL))
	{
		//��ȡ����
		m_dwActionMask = kOperateNotify.dwActionMask;
		m_cbActionCard = kOperateNotify.cbActionCard;

		CopyMemory(&m_GangResult, &kOperateNotify.GangPaiResult, sizeof(tagGangPaiResult));

		// ��������Ϣ;
		if ((kOperateNotify.dwActionMask&WIK_TING) != 0 && kDataStream.size() > 0)
		{
			tagTingPaiDataStream kTingPaiStream;
			kTingPaiStream.StreamValue(kDataStream, false);
			
			tagTingPaiResult TingPaiResult(kTingPaiStream);
			CopyMemory(&m_TingResult, &TingPaiResult, sizeof(tagTingPaiResult));
		}

		//����ʱ��
		UINT nElapse = GetAndroidOpTime(IDI_OPERATE_CARD_A);
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD_A, nElapse);
	}
	
	return true;
}

//
bool CAndroidUserItemSink::OnSubOperateResult( const void *pBuffer,WORD wDataSize )
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//ɾ��֮ǰ�Ķ�ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);
	//��Ϣ����
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	BYTE cbPublicCard=TRUE;
	WORD wOperateUser = pOperateResult->wOperateUser;
	WORD wProvideUser = pOperateResult->wProvideUser;
	BYTE cbOperateCard = pOperateResult->cbOperateCard;
	DWORD dwOperateCode = pOperateResult->dwOperateCode;

	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
	//���Ʊ���
	if (dwOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//��������
	m_wCurrentUser = INVALID_CHAIR;
	m_dwActionMask = WIK_NULL;
	m_cbActionCard =0;
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	//�������
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
				//���� ����ϼҴ������
				BYTE cbDiscardCount = --m_cbDiscardCount[wProvideUser];
				m_cbDiscardCard[wProvideUser][cbDiscardCount] = 0;
			}
			//�����˿�
			BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard = cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard = cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].dwWeaveKind  = dwOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser = wProvideUser;
		}
		//�˿�����
		if( wOperateUser == wMeChairID )
		{
			BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard);
			m_cbCardIndex[wMeChairID][cbCardIndex] = 0;
		}
	}
	else if ( (dwOperateCode & WIK_PENG)>0 || (dwOperateCode & WIK_CHI)>0 )
	{
		m_wCurrentUser = wOperateUser;
		//����ϼҴ������
		BYTE cbDiscardCount = --m_cbDiscardCount[wProvideUser];
		m_cbDiscardCard[wProvideUser][cbDiscardCount] = 0;

		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].dwWeaveKind=dwOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=wProvideUser;

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=dwOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);

		//ɾ���˿�
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
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//ɾ��������ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	//��Ϣ����
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

	//������������
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;

	//���ƴ���
	if ( (pSendCard->cbCardData!=0) && (m_wCurrentUser==wMeChairID) )
	{
		BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData);
		m_cbCardIndex[m_wCurrentUser][cbCardIndex]++;	

		m_cbSendCardData = pSendCard->cbCardData;
	}
	//�۳��˿�
	m_cbLeftCardCount--;
	return true;
}

bool CAndroidUserItemSink::OnSubUserBuHua( const void *pBuffer, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_S_SendBuHua));
	if (wDataSize!=sizeof(CMD_S_SendBuHua)) return false;

	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);

	CMD_S_SendBuHua *pSendBuHua = (CMD_S_SendBuHua *)pBuffer;
	ASSERT(pSendBuHua->cbBuHuaCount>0);

	//���ñ���
	WORD wMeChairID = m_pIAndroidUserItem->GetChairID();

	if ( wMeChairID == pSendBuHua->wBuHuaUser )   //�Լ�����
	{
		m_GameLogic.RemoveCard(m_cbCardIndex[wMeChairID],pSendBuHua->cbBuHuaData,pSendBuHua->cbBuHuaCount);
		for (int i = 0; i < pSendBuHua->cbBuHuaCount; ++i)
		{
			BYTE cbSendIndex = m_GameLogic.SwitchToCardIndex(pSendBuHua->cbReplaceCardData[i]);
			m_cbCardIndex[wMeChairID][cbSendIndex]++;
		}
	}
	//�۳��˿�
	m_cbLeftCardCount -= pSendBuHua->cbBuHuaCount;
	return true;
}

//�û�����
bool CAndroidUserItemSink::OnOutCard()
{
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	ASSERT(m_wCurrentUser == wMeChairID /*&& !m_bTingStatus*/);
	
	if ( wMeChairID !=m_wCurrentUser)
	{
		return false;
	}
	//������
	const BYTE MaxOutCount = 14;    //���ɳ�������
	
	BYTE cbIsolateCard[MaxOutCount];
	memset(cbIsolateCard,0,sizeof(cbIsolateCard));

	BYTE cbIsolateCount = CalIsolatedCard(m_cbCardIndex[wMeChairID],cbIsolateCard);
	BYTE cbCardData = ChooseIsolatedCard(cbIsolateCard,cbIsolateCount);

	if( m_GameLogic.IsValidCard(cbCardData) && m_GameLogic.IsZiCard(cbCardData))
	{
		DoOutCard(cbCardData);
		return true;
	}

	//������  ���ݷ������������ݴ����׼ȷ
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
	//��ȱ��
	if( OutCardSameColor() )
		return true;
	
	//���߶�
	if ( OutCardQiDui(cbIsolateCard,cbIsolateCount) )
	{
		return true;
	}
	//�����
	if( m_GameLogic.IsValidCard(cbCardData) )
	{
		DoOutCard(cbCardData);
		return true;
	}
	
	//û�й���
	BYTE cbOneCard[MaxOutCount] = {0};
	BYTE cbOneCount = GetCardFromCount(cbOneCard,1);
	BYTE cbTwoCard[MaxOutCount] = {0};
	BYTE cbTwoCount = GetCardFromCount(cbTwoCard,2);
	BYTE cbThreeCard[MaxOutCount] = {0};
	BYTE cbThreeCount = GetCardFromCount(cbThreeCard,3);
	
	//����������ѡ��
	if (cbOneCount > 0)  //���ȴ���
	{
		const BYTE TypeCount = 4;
		BYTE cbCount[TypeCount];    //����
		BYTE cbOutCard[TypeCount][MaxOutCount]; //��ֵ
		ZeroMemory(cbCount,sizeof(cbCount));
		ZeroMemory(cbOutCard,sizeof(cbOutCard));
		for (int i = 0; i < cbOneCount; ++i)
		{
			BYTE cbCard = cbOneCard[i];
			DWORD dwChiType = GetCardShunType(cbCard);

			if (dwChiType == WIK_NULL)    //����˳
				cbOutCard[0][cbCount[0]++] = cbCard;
			else if( dwChiType == WIK_OXX || dwChiType == WIK_XXO )  //
				cbOutCard[1][cbCount[1]++] = cbCard;
			else if(dwChiType == WIK_XOX)            //˳ ���м�һ����
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
		//���ȷ�����
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
	float fGameProgress = (float)m_cbLeftCardCount / (MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1);  //��Ϸ����
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
	const BYTE MaxOutCount = 14;    //�������

	BYTE cbWanCards[MaxOutCount] = {0};  //��
	BYTE cbWanCount = GetSameColorCard(cbWanCards,0);
	BYTE cbTiaoCards[MaxOutCount] = {0};  //��
	BYTE cbTiaoCount = GetSameColorCard(cbTiaoCards,1);
	BYTE cbTongCards[MaxOutCount] = {0};  //Ͳ
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
		//�����ƻ�ɫ
		for (int i = 0; i < m_cbWeaveCount[wMeChairID]; ++i)
		{
			const tagWeaveItem & weaveitem = m_WeaveItemArray[wMeChairID][i];
			BYTE cbLeftColor = m_GameLogic.GetCardColor(weaveitem.cbCenterCard);
			if (cbLeftColor != cbColor && cbLeftColor < 3)
			{
				return false;
			}
		}
		if (cbOutCount > 0)  //���������ֻ�ɫ����ѡ�����ų���
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
			//û�й��ƾ�ѡһ�Ŵ��������
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
	//�߶��ж�
	BYTE cbDuiZiNum = GetDuiZiNum();
	float fGameProgress = (float)m_cbLeftCardCount / (MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1);  //��Ϸ����

	bool bJudgeQiDui = false;  //�ж��Ƿ�������߶�����

	if (cbDuiZiNum == 4 && fGameProgress >= 0.6f)
		bJudgeQiDui = true;
	else if (cbDuiZiNum == 5 && fGameProgress >= 0.4f)
		bJudgeQiDui = true;
	else if (cbDuiZiNum >= 6 && fGameProgress >= 0.2f)
		bJudgeQiDui = true;

	if ( bJudgeQiDui )  //������е����Ѿ�������������
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
	//ɾ��ʱ��
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

//�����ж�
bool CAndroidUserItemSink::VerdictOutCard(BYTE cbCardData)
{
	//�����ж�
	if (m_bTingStatus)
	{
		//��������
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		BYTE cbWeaveCount=m_cbWeaveCount[wMeChairID];

		//������
		BYTE cbCardIndexTemp[MAX_INDEX];
		CopyMemory(cbCardIndexTemp,m_cbCardIndex[wMeChairID],sizeof(cbCardIndexTemp));

		//ɾ����
		m_GameLogic.RemoveCard(cbCardIndexTemp,cbCardData);

		//�����ж�
		BYTE i = 0;
		for (;i<MAX_INDEX;i++)
		{
			//���Ʒ���
			BYTE wChiHuRight=0;
			BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(i);
			DWORD dwHuCardKind=0;//m_GameLogic.AnalyseHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairID],cbWeaveCount,cbCurrentCard,ChiHuResult);

			//����ж�
			if (dwHuCardKind!=HU_NULL) 
				break;
		}
		//�����ж�
		return (i!=MAX_INDEX);
	}
	return true;
}

void CAndroidUserItemSink::ReInit()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	m_dwActionMask = WIK_NULL;
	m_cbActionCard = 0;
    
	m_bHaveGameEnd = true;
	m_bTingStatus = false;
	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));
	m_cbSendCardData = 0;

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�˿˱���
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
	//���ý���
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD_A);
	m_cbSendCardData=0;

	//���ñ���
	m_dwActionMask = WIK_NULL;

	VERIFY(m_GameLogic.IsValidCard(cbOutCard));
	if( !m_GameLogic.IsValidCard(cbOutCard)) return;

	//��������
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
	if ((dwOperateCode&WIK_HU) > 0)				//����
	{
		dwOperateCode = WIK_HU;
	}
	else if ((dwOperateCode&WIK_GANG) > 0)   //�и����
	{
		//���ѡ�����еĸ���
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

	//��������
	m_dwActionMask = WIK_NULL;    

	//��������
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
	//��ʱ����
	BYTE cbTempCardIndex[MAX_INDEX];
	ZeroMemory(cbTempCardIndex,sizeof(cbTempCardIndex));
	CopyMemory(cbTempCardIndex,cbCalCards,sizeof(cbTempCardIndex));

	BYTE cbCardData[MAX_COUNT];
	BYTE cbCardCount=m_GameLogic.SwitchToCardData(cbTempCardIndex,cbCardData);
	ASSERT(cbCardCount <= 14);

	BYTE cbRandCount = 0;
	for (int i = 0; i < cbCardCount; ++i)    //���ֹ���
	{
		BYTE cbCard = cbCardData[i];
		if(cbTempCardIndex[m_GameLogic.SwitchToCardIndex(cbCard)] != 1) continue;   //����ʱ�������
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
	for (int j = 0; j < cbCardCount; ++j)   //19����
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
	for (int k = 0; k < cbCardCount; ++k)  //����ͨ���� ����2�Ŷ�û��
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
	for (int m = 0; m < cbCardCount; ++m)   //19���� �鲻��˳��
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
	for (int n = 0; n < cbCardCount; ++n)  //����ͨ����  �鲻��˳��  ���Ҷ�û����
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
	for (int n = 0; n < cbCardCount; ++n)  //����ͨ����  �鲻��˳��  ��ͨ�Ƶ�����ֻ��һ��
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

	BYTE cbNearCard[4];  //����4�����ڵ����Ƿ����
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

	//���Ʋ���
	if(dwOperateCode & WIK_PENG)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_PENG;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbPengIsolateCount= CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}
	//��Բ���
	if(dwOperateCode & WIK_OXX)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_OXX;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbLeftIsolateCount=CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}
	//�гԲ���
	if(dwOperateCode & WIK_XOX)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_XOX;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbCenterIsolateCount=CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}
	//�ҳԲ���
	if(dwOperateCode & WIK_XXO)
	{
		BYTE cbTempCardIndex[MAX_INDEX];
		CopyMemory(cbTempCardIndex,m_cbCardIndex[wMeChairID],sizeof(cbTempCardIndex));

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0};
		DWORD dwWeaveKind=WIK_XXO;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(dwWeaveKind,cbOperateCard,cbWeaveCard);
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(cbTempCardIndex,cbWeaveCard,cbWeaveCardCount-1);
		cbRightIsolateCount=CalIsolatedCard(cbTempCardIndex,cbIsolateCard,true);
	}

	//����Ȩλ
	if( (dwOperateCode & WIK_PENG) > 0 )
	{
		//�߶��ж�
		BYTE cbDuiZiNum = GetDuiZiNum();
		float fGameProgress = (float)m_cbLeftCardCount / (MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1)-1);  //��Ϸ����

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

	//ʣ�����ٹ��ƵĲ���
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
		//�������
		for (int j = 0; j < m_cbDiscardCount[i];++j)
		{
			if(m_cbDiscardCard[i][j] == cbCardData)
				++cbCount;
		}
		//�����ܵ���
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
			//����
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
	//����Ѿ�������
	BYTE cbMaxPutCount = 0xff;
	BYTE cbOutCard = 0xff;
	//���������ȴ�
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

	//�۾���
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
	if ( cbMaxPutCount >= 3 && m_GameLogic.IsValidCard(cbOutCard))  //���ȳ��Ѿ���������ŵ���
	{
		return cbOutCard;
	}

	//������ ���۾���
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
	
	if ( cbMaxPutCount >= 3 && m_GameLogic.IsValidCard(cbOutCard))  //���ȳ��Ѿ���������ŵ���
	{
		return cbOutCard;
	}

	return cbCanOutCard[0];
}

BYTE CAndroidUserItemSink::GetCardFromCount( OUT BYTE cbAllCard[],IN const int nCardCount )
{
	const BYTE MaxOutCount = 14;    //�������
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
	//�Ƿ������
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
				if (m_GameLogic.IsValidCard(cbHuCard))       //�жϺ����ƻ���
				{
					BYTE cbLeft = 4 - CalCardCountInTable(cbHuCard);
					ASSERT(cbLeft <= 4);
					if(cbLeft == 0)  continue;

					cbHuLeft += cbLeft;
				}
			}
			if ( cbHuLeft > cbMaxLeft)
			{
				cbPutCard = TingPaiItem.cbOutCardData;   //Ҫ������� ����ʣ������
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
	//����׼��
	BYTE cbCardDataTemp[MAX_DEAL_COUNT] = {0};  //����ԭ����
	CopyMemory(cbCardDataTemp, cbCardData, sizeof(BYTE)*cbMaxCount);
	BYTE cbCardPutIn[MAX_DEAL_COUNT] = {0};  //���ջ��ҽ��

	//��ʼ������
	srand(GetTickCount()|(rand()<<8));

	//������
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

//��ȡ����������
bool CAndroidUserItemSink::ReadAndroidConfig()
{
	//�����ļ���
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
		//��С׼��ʱ��
		m_cbMinReadyTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinReadyTime"), 2, szFileName); 
		//���׼��ʱ��
		m_cbMaxReadyTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxReadyTime"), 5, szFileName);
		ASSERT(m_cbMaxReadyTime >= m_cbMinReadyTime);

		//��С����ʱ��
		m_cbMinOperateCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinOperateCardTime"), 2, szFileName); 
		//��С����ʱ��
		m_cbMaxOperateCardTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MaxOperateCardTime"), 5, szFileName); 
		ASSERT(m_cbMaxOperateCardTime >= m_cbMinOperateCardTime);

		//��С���õ�עʱ��
		m_cbMinBaseScoreTime = GetPrivateProfileInt(TEXT("AndroidTime"), TEXT("MinBaseScoreTime"), 2, szFileName); 
		//������õ�עʱ��
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

//��ȡ�����˲���ʱ��
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

//����Ŀ¼
bool CAndroidUserItemSink::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
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

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);
