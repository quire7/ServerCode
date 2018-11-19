#include "Stdafx.h"
#include "AndroidUserItemSink.h"
#include "AndroidAI.h"

//////////////////////////////////////////////////////////////////////////

//����ʱ��

#ifdef _DEBUG
#define TIME_LESS					3									//����ʱ��
#define TIME_START_GAME				3									//��ʼʱ��
#define TIME_OPERATE_CARD			3									//������ʱ��
#else
//#define TIME_LESS					2									//����ʱ��
//#define TIME_START_GAME				10									//��ʼʱ��
//#define TIME_OPERATE_CARD			8									//������ʱ��

#define TIME_LESS					3									//����ʱ��
#define TIME_START_GAME				3									//��ʼʱ��
#define TIME_OPERATE_CARD			3									//������ʱ��
#endif

//��Ϸʱ��
#define IDI_START_GAME				(IDI_REBOT_MODULE_START+1)			//��ʼʱ��
#define IDI_OPERATE_CARD			(IDI_REBOT_MODULE_START+2)			//������ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�˿˱���
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return;
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
	//��Ϸ����
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = 0;
	m_cbActionCard = 0;

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�˿˱���
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));

	return true;
}

//ʱ����Ϣ
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_START_GAME:	//��ʼ��Ϸ
		{
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);

			//��ʼ�ж�
			if( m_pIAndroidUserItem->GetGameStatus()==GS_MJ_FREE )
				m_pIAndroidUserItem->SendUserReady(NULL,0);

			return true;
		}
	case IDI_OPERATE_CARD:		//������ʱ��
		{
			tagOutCardResult OutCardResult;
			WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
#ifdef	_DEBUG
			BYTE byCardCount = m_GameLogic.GetCardCount( m_cbCardIndex[wMeChairId] );
			if( wMeChairId == m_wCurrentUser )
				ASSERT( (byCardCount+1)%3 == 0 );
			else ASSERT( byCardCount%3 == 1 );
#endif

			try
			{
				//��������
				if( SearchOutCard(OutCardResult) )
				{
					if( OutCardResult.cbOperateCode != WIK_NULL )
					{
						//Ч��
						ASSERT( OutCardResult.cbOperateCode&m_cbActionMask );
						if( !(OutCardResult.cbOperateCode&m_cbActionMask) ) throw 0;

						//��Ӧ����
						OnOperateCard(OutCardResult.cbOperateCode,OutCardResult.cbOperateCard);
					}
					else
					{
						//Ч��
						ASSERT( m_cbCardIndex[m_pIAndroidUserItem->GetChairID()][m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] > 0 );
						if( m_cbCardIndex[m_pIAndroidUserItem->GetChairID()][m_GameLogic.SwitchToCardIndex(OutCardResult.cbOperateCard)] == 0 ) throw 0;

						//����
						OnOutCard(OutCardResult.cbOperateCard);
					}
				}else
				{
					//Ч��
					ASSERT( wMeChairId != m_wCurrentUser );
					if( wMeChairId == m_wCurrentUser ) throw 0;

					//��Ӧ����
					OnOperateCard(WIK_NULL,0);
				}
			}catch(...)
			{
				ASSERT(FALSE);
				//�쳣����
				if( wMeChairId == m_wCurrentUser )
				{
					for( BYTE i = 0; i < MAX_INDEX; i++ )
					{
						if( m_cbCardIndex[wMeChairId][i] > 0 )
						{
							OnOutCard( m_GameLogic.SwitchToCardData(i) );
							return true;
						}
					}
				}
				else OnOperateCard( WIK_NULL,0 );
			}

			return true;
		}
	}

	return false;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_OUT_CARD:		//�û�����
		{
			return OnSubOutCard(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:		//������Ϣ
		{
			return OnSubSendCard(pData,wDataSize);
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
	case SUB_S_TRUSTEE:			//�û��й�
		{
			return true;
		}
	case SUB_S_CHI_HU:
		{
			return true;
		}
	case SUB_S_GANG_SCORE:
		{
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��������
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

			//�������
			if (pIServerUserItem->GetUserStatus()!=US_READY)
			{
				UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
				m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);
			}

			return true;
		}
	case GS_MJ_PLAY:		//��Ϸ״̬
		{
			ASSERT( FALSE );
			return true;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return true;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
void  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�����
void  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
void  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
void  CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	return;
}

//ׯ����Ϣ
bool CAndroidUserItemSink::OnSubGameStart(const void * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_AndroidGameStart));
	if (wDataSize != sizeof(CMD_S_AndroidGameStart)) return false;

	//��������
	CMD_S_AndroidGameStart * pAGS = (CMD_S_AndroidGameStart *)pData;

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_PLAY);

	//���ñ���
	m_wBankerUser = pAGS->GameStart.wBankerUser;
	m_wCurrentUser = pAGS->GameStart.wCurrentUser;
	m_cbLeftCardCount=MAX_REPERTORY-GAME_PLAYER*(MAX_COUNT-1);
	m_cbActionMask = pAGS->GameStart.cbUserAction;
	m_cbActionCard = 0;

	//������Ϣ
	m_cbOutCardData=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//����˿�
	ZeroMemory(m_cbWeaveCount,sizeof(m_cbWeaveCount));
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));

	//�����˿�
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardCount = MAX_COUNT-1;
	//m_GameLogic.SwitchToCardIndex(pAGS->GameStart.cbCardData, cbCardCount, m_cbCardIndex[wMeChairId]);

	for( WORD i = 0; i < GAME_PLAYER; i++ )
	{
		//if( i == wMeChairId ) continue;
		m_GameLogic.SwitchToCardIndex(&pAGS->cbCardData[MAX_COUNT*i], cbCardCount, m_cbCardIndex[i]);
	}

// 	//��������
// 	if ((pGameStart->cbUserAction!=WIK_NULL) || m_wCurrentUser==wMeChairId )
// 	{
// 		UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS+5;
// 		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
// 	}

	return true;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//����״̬
	m_pIAndroidUserItem->SetGameStatus(GS_MJ_FREE);

	//����
	UINT nElapse = rand()%TIME_START_GAME+TIME_LESS;
	m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

	return true;
}

//����֪ͨ
bool CAndroidUserItemSink::OnSubOperateNotify( const void *pBuffer,WORD wDataSize )
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_OperateNotify));
	if (wDataSize!=sizeof(CMD_S_OperateNotify)) return false;

	//��������
	CMD_S_OperateNotify * pOperateNotify=(CMD_S_OperateNotify *)pBuffer;

	//�û�����
	if ((pOperateNotify->cbActionMask!=WIK_NULL))
	{
		//��ȡ����
		WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
		m_cbActionMask = pOperateNotify->cbActionMask;
		m_cbActionCard = pOperateNotify->cbActionCard;

		//����ʱ��
		UINT nElapse = rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//�������֪ͨ
bool CAndroidUserItemSink::OnSubOperateResult( const void *pBuffer,WORD wDataSize )
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OperateResult));
	if (wDataSize!=sizeof(CMD_S_OperateResult)) return false;

	//��Ϣ����
	CMD_S_OperateResult * pOperateResult=(CMD_S_OperateResult *)pBuffer;

	m_pIAndroidUserItem->KillGameTimer( IDI_OPERATE_CARD );

	//��������
	BYTE cbPublicCard=TRUE;
	WORD wOperateUser=pOperateResult->wOperateUser;
	BYTE cbOperateCard = pOperateResult->cbOperateCard;

	//���Ʊ���
	if (pOperateResult->cbOperateCode!=WIK_NULL)
	{
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//���ñ���
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//�������
	if ((pOperateResult->cbOperateCode&WIK_GANG)!=0)
	{
		//���ñ���
		m_wCurrentUser=INVALID_CHAIR;

		//����˿�
		BYTE cbWeaveIndex=0xFF;
		for (BYTE i=0;i<m_cbWeaveCount[wOperateUser];i++)
		{
			BYTE cbWeaveKind=m_WeaveItemArray[wOperateUser][i].cbWeaveKind;
			BYTE cbCenterCard=m_WeaveItemArray[wOperateUser][i].cbCenterCard;
			if ((cbCenterCard==cbOperateCard)&&(cbWeaveKind==WIK_PENG))
			{
				cbWeaveIndex=i;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
				m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
				break;
			}
		}

		//����˿�
		if (cbWeaveIndex==0xFF)
		{
			//�����ж�
			cbPublicCard=(pOperateResult->wProvideUser==wOperateUser)?FALSE:TRUE;

			//�����˿�
			cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=cbPublicCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
			m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;
		}

		//�˿�����
		m_cbCardIndex[wOperateUser][m_GameLogic.SwitchToCardIndex(cbOperateCard)]=0;
	}
	else if (pOperateResult->cbOperateCode!=WIK_NULL&&pOperateResult->cbOperateCode!=WIK_CHI_HU)
	{
		//���ñ���
		m_wCurrentUser=pOperateResult->wOperateUser;

		//�������
		BYTE cbWeaveIndex=m_cbWeaveCount[wOperateUser]++;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbPublicCard=TRUE;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbCenterCard=cbOperateCard;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].cbWeaveKind=pOperateResult->cbOperateCode;
		m_WeaveItemArray[wOperateUser][cbWeaveIndex].wProvideUser=pOperateResult->wProvideUser;

		//��Ͻ���
		BYTE cbWeaveCard[4]={0,0,0,0},cbWeaveKind=pOperateResult->cbOperateCode;
		BYTE cbWeaveCardCount=m_GameLogic.GetWeaveCard(cbWeaveKind,cbOperateCard,cbWeaveCard);

		//ɾ���˿�
		m_GameLogic.RemoveCard(cbWeaveCard,cbWeaveCardCount,&cbOperateCard,1);
		m_GameLogic.RemoveCard(m_cbCardIndex[wOperateUser],cbWeaveCard,cbWeaveCardCount-1);
	}

	//����ʱ��
	if (m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
	{
		//����ʱ��
		UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
		//����ʱ��
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubOutCard( const void *pBuffer,WORD wDataSize )
{
	//Ч����Ϣ
	ASSERT(wDataSize==sizeof(CMD_S_OutCard));
	if (wDataSize!=sizeof(CMD_S_OutCard)) return false;

	//��Ϣ����
	CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pBuffer;

	//��������
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;
	m_wOutCardUser=pOutCard->wOutCardUser;
	ASSERT( pOutCard->cbOutCardData != 0 );
	m_cbOutCardData=pOutCard->cbOutCardData;

	//ɾ���˿�
	if( wMeChairID != pOutCard->wOutCardUser )
		m_GameLogic.RemoveCard(m_cbCardIndex[pOutCard->wOutCardUser],pOutCard->cbOutCardData);

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubSendCard( const void *pBuffer,WORD wDataSize )
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_SendCard));
	if (wDataSize!=sizeof(CMD_S_SendCard)) return false;

	//��������
	CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

	//���ñ���
	WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
	m_wCurrentUser=pSendCard->wCurrentUser;

	//�����˿�
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		//�����˿�
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser]++] = m_cbOutCardData;
		//���ñ���
		m_cbOutCardData=0;
		m_wOutCardUser=INVALID_CHAIR;
	}

	//���ƴ���
	if (pSendCard->cbCardData!=0)
	{
		m_cbCardIndex[pSendCard->wCurrentUser][m_GameLogic.SwitchToCardIndex(pSendCard->cbCardData)]++;	
		//�۳��˿�
		m_cbLeftCardCount--;
	}

	//����ʱ��
	if( wMeChairID == m_wCurrentUser )
	{
		m_cbActionMask = pSendCard->cbActionMask;
		m_cbActionCard = pSendCard->cbCardData;

		//����ʱ��
		UINT nElapse=rand()%TIME_OPERATE_CARD+TIME_LESS;
		m_pIAndroidUserItem->SetGameTimer(IDI_OPERATE_CARD,nElapse);
	}

	return true;
}

//����
void CAndroidUserItemSink::OnOutCard( BYTE cbOutCard )
{
	//ɾ����ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//���ñ���
	m_wCurrentUser=INVALID_CHAIR;
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//ɾ����
	m_GameLogic.RemoveCard(m_cbCardIndex[m_pIAndroidUserItem->GetChairID()],cbOutCard);

	//��������
	CMD_C_OutCard OutCard;
	OutCard.cbCardData=cbOutCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)) )
	{
		ASSERT( FALSE );
		return ;
	}
	
	return ;
}

//������
void CAndroidUserItemSink::OnOperateCard( BYTE cbOperateCode,BYTE cbOperateCard )
{
	//ɾ��ʱ��
	m_pIAndroidUserItem->KillGameTimer(IDI_OPERATE_CARD);

	//���ñ���
	m_cbActionMask = WIK_NULL;
	m_cbActionCard = 0;

	//��������
	CMD_C_OperateCard OperateCard;
	OperateCard.cbOperateCode=cbOperateCode;
	OperateCard.cbOperateCard = cbOperateCard;
	if( !m_pIAndroidUserItem->SendSocketData(SUB_C_OPERATE_CARD,&OperateCard,sizeof(OperateCard)) )
	{
		ASSERT( FALSE );
		return ;
	}

	return ;
}

//��������
bool CAndroidUserItemSink::SearchTingCard( tagTingCardResult &TingCardResult )
{
	//��������
	ZeroMemory(&TingCardResult,sizeof(TingCardResult));
	BYTE bAbandonCardCount=0;

	//�����˿�
	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	BYTE cbCardIndexTemp[MAX_INDEX];
	CopyMemory(cbCardIndexTemp,m_cbCardIndex[wMeChairId],sizeof(cbCardIndexTemp));

	BYTE cbCardCount = m_GameLogic.GetCardCount(cbCardIndexTemp);
	if( (cbCardCount-2)%3 == 0 )
	{
		//���Ʒ���
		for (BYTE i=0;i<MAX_INDEX-ZI_PAI_COUNT;i++)
		{
			//���ƹ���
			if (cbCardIndexTemp[i]==0) continue;

			//���ƴ���
			cbCardIndexTemp[i]--;

			//�����ж�
			bool bHuCard = false;
			bAbandonCardCount = TingCardResult.bAbandonCount;
			CChiHuRight chr;
			for (BYTE j=0;j<MAX_INDEX-ZI_PAI_COUNT;j++)
			{
				//���Ʒ���
				BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(j);
				BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],cbCurrentCard,chr);

				//����ж�
				if (cbHuCardKind!=WIK_NULL)
				{
					bHuCard = true;
					TingCardResult.bTingCard[bAbandonCardCount][TingCardResult.bTingCardCount[bAbandonCardCount]++] = cbCurrentCard;
				}
			}
			if( bHuCard == true )
			{
				TingCardResult.bAbandonCard[TingCardResult.bAbandonCount++] = m_GameLogic.SwitchToCardData(i);
			}
			//��ԭ����
			cbCardIndexTemp[i]++;
		}
	}
	else
	{
		//�����ж�
		bAbandonCardCount = TingCardResult.bAbandonCount;
		CChiHuRight chr;
		for (BYTE j=0;j<MAX_INDEX-ZI_PAI_COUNT;j++)
		{
			//���Ʒ���
			BYTE cbCurrentCard=m_GameLogic.SwitchToCardData(j);
			BYTE cbHuCardKind=m_GameLogic.AnalyseChiHuCard(cbCardIndexTemp,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],cbCurrentCard,chr);

			//����ж�
			if (cbHuCardKind!=WIK_NULL)
			{
				TingCardResult.bTingCard[bAbandonCardCount][TingCardResult.bTingCardCount[bAbandonCardCount]++] = cbCurrentCard;
			}
		}
	}

	//����ʣ����
	BYTE n = 0;
	while( TingCardResult.bTingCardCount[n] > 0 )
	{
		TingCardResult.bRemainCount[n] = TingCardResult.bTingCardCount[n]*4;

		for( BYTE i = 0; i < TingCardResult.bTingCardCount[n]; i++ )
		{
			BYTE bCardData = TingCardResult.bTingCard[n][i];
			//���Լ���
			if( cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(bCardData)] > 0 )
			{
				TingCardResult.bRemainCount[n] -= cbCardIndexTemp[m_GameLogic.SwitchToCardIndex(bCardData)];
				ASSERT( TingCardResult.bRemainCount[n]>=0 );
			}

			for( BYTE j = 0; j < GAME_PLAYER; j++ )
			{
				//�������
				BYTE k = 0;
				for( k = 0; k < m_cbWeaveCount[j]; k++ )
				{
					if( m_WeaveItemArray[j][k].cbCenterCard == bCardData )
					{
						TingCardResult.bRemainCount[n] -= m_WeaveItemArray[j][k].cbWeaveKind==WIK_GANG?4:3;
						ASSERT( TingCardResult.bRemainCount[n]>=0 );
					}
				}
				//��������
				for( k = 0; k < m_cbDiscardCount[j]; k++ )
				{
					if( bCardData == m_cbDiscardCard[j][k] )
					{
						TingCardResult.bRemainCount[n]--;
						ASSERT( TingCardResult.bRemainCount[n]>=0 );
					}
				}
			}
		}

		n++;
	}

	return true;
}

//��������
bool CAndroidUserItemSink::SearchOutCard( tagOutCardResult &OutCardResult )
{
	//��ʼ��
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	WORD wMeChairId = m_pIAndroidUserItem->GetChairID();
	ASSERT( wMeChairId != INVALID_CHAIR );
	if( wMeChairId == INVALID_CHAIR ) return false;

	//�жϺ�
	if( (m_cbActionMask&WIK_CHI_HU) != WIK_NULL )
	{
		OutCardResult.cbOperateCode = WIK_CHI_HU;
		OutCardResult.cbOperateCard = m_cbActionCard;
		return true;
	}

	//ת������
	BYTE byCard[MAX_COUNT],byCardCount = 0;
	for( BYTE i = 0; i < MAX_INDEX; i++ )
	{
		for( BYTE j = 0; j < m_cbCardIndex[wMeChairId][i]; j++ )
		{
			byCard[byCardCount++] = i;
		}
	}

	//�ж���
	//	if( WIK_LISTEN == m_GameLogic.AnalyseTingCard(m_cbCardIndex[wMeChairId],m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]) )
	if(m_GameLogic.AnalyseTingCard(m_cbCardIndex[wMeChairId],m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]) )
	{
		tagTingCardResult TingCardResult;
		if( SearchTingCard(TingCardResult) )
		{
			BYTE bMostIndex = 0;
			int nMostCount = -1;
			BYTE i = 0;
			while(true)
			{
				if( TingCardResult.bTingCardCount[i] == 0 ) break;

				if( TingCardResult.bRemainCount[i] > nMostCount )
				{
					bMostIndex = i;
					nMostCount = TingCardResult.bRemainCount[i];
				}

				i++;
			}
			//���ƿ���
			if( nMostCount > 0 )
			{
				//��������
				if( wMeChairId != m_wCurrentUser ) return false;

				OutCardResult.cbOperateCode = WIK_NULL;
				OutCardResult.cbOperateCard = TingCardResult.bAbandonCard[bMostIndex];
				return true;
			}
			//������
			else if( wMeChairId == m_wCurrentUser )
			{
				//����AI
				CAndroidAI AndroidAi;
				AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
				AndroidAi.Think();
				//������в���
				BYTE byBadlyCard = 0xff;
				if( byCardCount <= 2 )
					byBadlyCard = AndroidAi.GetBadlyCard();
				else
					byBadlyCard = AndroidAi.GetBadlyIn2Card();
				if( 0xff != byBadlyCard )
				{
					OutCardResult.cbOperateCode = WIK_NULL;
					OutCardResult.cbOperateCard = m_GameLogic.SwitchToCardData(byBadlyCard);
					return true;
				}
				else
				{
					//�������ֻ����в���
					byBadlyCard = AndroidAi.GetBadlyIn3Card();
					if( 0xff != byBadlyCard )
					{
						OutCardResult.cbOperateCode = WIK_NULL;
						OutCardResult.cbOperateCard = m_GameLogic.SwitchToCardData(byBadlyCard);
						return true;
					}
				}
			}
		}
	}

	BYTE cbActionCard = m_cbActionCard;

	//������ֲ����÷�
	BYTE cbOperateCode[] = { WIK_GANG,WIK_PENG,WIK_LEFT,WIK_CENTER,WIK_RIGHT,WIK_NULL };
	int nOperateScore[] = { 0,0,0,0,0,0 };
	//����ԭʼ��
	CAndroidAI AndroidAi;
	AndroidAi.SetEnjoinOutCard( NULL,0 );
	AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
	AndroidAi.Think();
	int nOrgScore = AndroidAi.GetMaxScore();
	//�жϸ�
	if( m_cbActionMask & WIK_GANG )
	{
		if( m_wCurrentUser == wMeChairId )
		{
			tagGangCardResult GangCardResult;
			m_GameLogic.AnalyseGangCard(m_cbCardIndex[wMeChairId],m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId],GangCardResult);

			//Ѱ����߷ָ���
			BYTE cbGangCard = 0;
			int nMaxGangScore = -1;
			for( BYTE i = 0; i < GangCardResult.cbCardCount; i++ )
			{
				ASSERT( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] > 0 );
				if( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i])] == 0 ) throw 0;

				//����ܺ�÷�
				AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
				AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(GangCardResult.cbCardData[i]));
				AndroidAi.Think();

				int nScore = AndroidAi.GetMaxScore();
				if( nScore > nMaxGangScore )
				{
					nMaxGangScore = nScore;
					cbGangCard = GangCardResult.cbCardData[i];
				}
			}

			ASSERT(nMaxGangScore!=-1&&cbGangCard!=0);
			cbActionCard = cbGangCard;
			nOperateScore[0] = nMaxGangScore-nOrgScore;
		}
		else
		{
			ASSERT( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(cbActionCard)] > 0 );
			if( m_cbCardIndex[wMeChairId][m_GameLogic.SwitchToCardIndex(cbActionCard)] == 0 ) throw 0;

			//����ܺ�÷�
			AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
			AndroidAi.SetAction(WIK_GANG,m_GameLogic.SwitchToCardIndex(cbActionCard));
			AndroidAi.Think();
			nOperateScore[0] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//�ж���
	if( m_cbActionMask & WIK_PENG )
	{
		AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
		AndroidAi.SetAction(WIK_PENG,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[1] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//���
	if( m_cbActionMask & WIK_LEFT )
	{
		AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
		AndroidAi.SetAction(WIK_LEFT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[2] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//�г�
	if( m_cbActionMask & WIK_CENTER )
	{
		AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
		AndroidAi.SetAction(WIK_CENTER,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[3] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}
	//�ҳ�
	if( m_cbActionMask & WIK_RIGHT )
	{
		AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
		AndroidAi.SetAction(WIK_RIGHT,m_GameLogic.SwitchToCardIndex(cbActionCard));
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
		}
		if( byBadlyIndex != 0xff )
		{
			AndroidAi.RemoveCardData( byBadlyIndex );
			AndroidAi.Think();
			nOperateScore[4] = AndroidAi.GetMaxScore()-nOrgScore;
		}
	}

	//��������
	BYTE cbOutCardData = cbActionCard;
	if( m_wCurrentUser == wMeChairId )
	{
		AndroidAi.SetCardData(byCard,byCardCount,m_WeaveItemArray[wMeChairId],m_cbWeaveCount[wMeChairId]);
		AndroidAi.Think();
		BYTE byBadlyIndex = AndroidAi.GetBadlyCard();
		if( byBadlyIndex == 0xff )
		{
			byBadlyIndex = AndroidAi.GetBadlyIn2Card();
			if( byBadlyIndex == 0xff )
			{
				byBadlyIndex = AndroidAi.GetBadlyIn3Card();
				ASSERT( byBadlyIndex != 0xff );
				if( byBadlyIndex == 0xff ) throw 0;
			}
		}
		AndroidAi.RemoveCardData( byBadlyIndex );
		AndroidAi.Think();
		nOperateScore[5] = AndroidAi.GetMaxScore()-nOrgScore;
		cbOutCardData = m_GameLogic.SwitchToCardData(byBadlyIndex);
	}

	//��ȡ��߷ֲ���
	BYTE cbIndex = 0;
	for( BYTE i = 1; i < CountArray(nOperateScore); i++ )
	{
		if( nOperateScore[cbIndex] < nOperateScore[i] )
			cbIndex = i;
	}
	if( (cbOperateCode[cbIndex]&m_cbActionMask) && 
		( cbOperateCode[cbIndex]==WIK_GANG || nOperateScore[cbIndex] > 0 ) )
	{
		OutCardResult.cbOperateCode = cbOperateCode[cbIndex];
		OutCardResult.cbOperateCard = cbActionCard;
		return true;
	}
	else
	{
		if( m_wCurrentUser == wMeChairId )
		{
			OutCardResult.cbOperateCard = cbOutCardData;
			return true;
		}
		else return false;
	}

	ASSERT( FALSE );
	throw 0;
}
//////////////////////////////////////////////////////////////////////////
