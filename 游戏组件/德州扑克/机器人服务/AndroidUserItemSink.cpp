#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//���캯��
CAndroidUserItemSink::CAndroidUserItemSink()
{	
	//�ӿڱ���
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
	m_pIAndroidUserItem->KillGameTimer(0);
	OnSubClear();
	return true;
}

//ʱ����Ϣ
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

				if (lScore < m_lAddTakeGoldMin)  //���ϵ�Ǯ���ܵ�������
				{
					//ASSERT(0);
					return false;
				}

				LONG lMaxLimit = AddTakeGold.lAddTakeGold + m_lUserMaxScore[wMeChairID];
				if (lMaxLimit > lScore)  //��Ϸ��ʼ ����Ϸ�� ����
				{
					AddTakeGold.lAddTakeGold = lScore - m_lUserMaxScore[wMeChairID];
				}

				lMaxLimit = AddTakeGold.lAddTakeGold + m_lSelfTakeGold;
				if (lMaxLimit > lScore)  //�ܴ��벻�ܳ�����ӵ�н��
				{
					AddTakeGold.lAddTakeGold = lScore - m_lSelfTakeGold;
				}

				if (lMaxLimit > m_lAddTakeGoldMax)  //�ܴ��벻�ܳ�����
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
	case IDI_START_GAME:		//��ʼ��ʱ��
		{		
			m_pIAndroidUserItem->KillGameTimer(IDI_START_GAME);			
			//����׼��
			m_pIAndroidUserItem->SendUserReady(NULL,0);		
			
			//CString str;
			//str.Format(_T("������ %s ��ʱ�� %d ִ�����\n"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), IDI_START_GAME);
			//::OutputDebugString(str);

			return true;
		}
	case IDI_USER_ROBOTDELAY:	//�������ӳٲ���
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
	case  IDI_REQUEST://��Ӧ����
		{
			//���ͻظ�			
			return true;
		}
	case IDI_SET_BASESCORE://���õ�ע��ʱ
		{
			//������Ϣ
			CMD_C_SetBaseScore BaseScore;
			ZeroMemory(&BaseScore, sizeof(BaseScore));
			BaseScore.bSetOrConfirm = m_bSetBaseScore;
			BaseScore.bUserChairID = m_pIAndroidUserItem->GetChairID();
			if ( SETBASE_SET == BaseScore.bSetOrConfirm )   //����С����
			{
				BaseScore.lBaseScore = m_lCellScore;
				BaseScore.lBaseScore = __max(1,BaseScore.lBaseScore);
			}
			m_pIAndroidUserItem->SendSocketData(SUB_C_SET_BASESCORE,&BaseScore,sizeof(BaseScore));

			IServerUserItem * pUserItem = m_pIAndroidUserItem->GetMeUserItem();
			if (m_bSetBaseScore > 0 && pUserItem->GetUserStatus() == US_SIT)  //�����˻��в�׼�������
			{
				m_pIAndroidUserItem->SendUserReady(NULL,0);
			}
			return true;	

		}
	case IDI_USER_PEICARD:
		{			
			return true;
		}
	case IDI_USER_ENDPEICARD:	//�������ӳٲ���
		{				
			return true;
		}
		break;
	case IDI_USER_FAN_END://���ƽ���
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
//�����˲���
bool CAndroidUserItemSink::OnGameRobot(const void * pBuffer, WORD wDataSize)
{
	//Ч������
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
//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAN_READY:	//����׼��
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
	case SUB_S_GAME_START://��Ϸ��ʼ
		{
			return OnSubGameStart(pData,wDataSize);
		}
	case SUB_S_ADD_TAKEGOLD:
	{
		return OnUserAddTakeGold(pData, wDataSize);
	}
	case SUB_S_USER_ROBOT://����������
		{
			return OnGameRobot(pData,wDataSize);
		}
	case SUB_S_GIVE_UP:			//�û�����
		{
			//��Ϣ����
			return OnSubGiveUp(pData,wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			//��Ϣ����
			return OnSubGameEnd(pData,wDataSize);
		}
	case SUB_S_ADD_SCORE:		//�û���ע
		{
			return OnSubAddScore(pData,wDataSize);
		}	
	case SUB_S_LOOK_CARD://����
		{
			return true;
		}
	case SUB_S_TURN_OVER:	//һ�ֽ���
		{
			return OnSubTurnOver(pData,wDataSize);
		}
	case SUB_S_SEND_CARD:
		{
			return OnSubSendCard(pData,wDataSize);
		}
	case SUB_S_SET_BASESCORE:	//���õ�
		{
			//��Ϣ����
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
	//�������
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
		//�������
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

//��Ϸ��Ϣ
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pBuffer, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//����״̬
		{
			//Ч������
			VERIFY_RETURN_FALSE(wDataSize==sizeof(CMD_S_StatusFree));

			//��������
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
			//Ч������
			VERIFY_RETURN_FALSE(wDataSize==sizeof(CMD_S_StatusPlay));
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;
			m_pIAndroidUserItem->SetGameStatus(GAME_STATUS_PLAY);
			//���ñ���
			m_wCurrentUser=pStatusPlay->wCurrentUser;//��ǰ���
			CopyMemory(m_lTableScore,pStatusPlay->lTableScore,sizeof(m_lTableScore));//��ע��Ŀ
			CopyMemory(m_cbPlayStatus,pStatusPlay->cbPlayStatus,sizeof(m_cbPlayStatus));//�û���Ϸ״̬
			CopyMemory(m_lTurnMaxScore,pStatusPlay->lTurnMaxScore,sizeof(m_lTurnMaxScore));//�û���Ϸ״̬
			CopyMemory(m_cbCenterCardData,pStatusPlay->cbCenterCardData,sizeof(m_cbCenterCardData));//�û�����״̬

			m_iChipPoolCount = pStatusPlay->cbChipPoolCount;
			//��ע��Ϣ
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

//�û�����
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
	//m_lSelfTakeGold = 0;
	m_lSelfTakeGold = m_pIAndroidUserItem->GetMeUserItem()->GetUserScore();

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
void CAndroidUserItemSink::OnSubClear()
{
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));	
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));	
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));	
	ZeroMemory(m_lUserMaxScore,sizeof(m_lUserMaxScore));	

	m_wCurrentUser = INVALID_CHAIR;
	m_lCellScore	= 0;							//��Ԫ��ע	
	m_bSetBaseScore  = 0;
	m_lTurnLessScore = 0;
	m_lAddLessScore  = 0;
	m_cbBalanceCount = 0;
	m_iChipPoolCount = 0;

}
//��Ϸ��ʼ
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
	//Ч������
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;	
	OnSubClear();
	//������Ϣ
	m_wCurrentUser = pGameStart->wCurrentUser;//��ǰ���
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

	//����ʱ��
	if( m_wCurrentUser == m_pIAndroidUserItem->GetChairID() )
	{
		//��עʱ��		
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
	//���ƽ���
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
	//��ע����
	if (pAddScore->wAddScoreUser != wMeChairID)
	{
		//��ע����
		if(pAddScore->lAddScoreCount>0)
		{			
			m_lTableScore[pAddScore->wAddScoreUser] +=pAddScore->lAddScoreCount;			
		}
	}

	//m_lSelfTakeGold = pAddScore->lUserTakeGold[wMeChairID];

	//���ñ���
	m_wCurrentUser = pAddScore->wCurrentUser;

	m_lTurnLessScore = pAddScore->lTurnLessScore;
	m_lAddLessScore = pAddScore->lAddLessScore;
	if( VALIDE_CHAIRID(m_wCurrentUser) )
	{
		ASSERT(m_cbPlayStatus[m_wCurrentUser] == TRUE);
		m_lTurnMaxScore[m_wCurrentUser] = pAddScore->lTurnMaxScore[m_wCurrentUser];
	}
	//���ƽ���
	if (m_wCurrentUser == wMeChairID)
	{			
		m_RobottLogic.m_lTurnLessScore = m_lTurnLessScore;
		m_RobottLogic.m_lAddLessScore = m_lAddLessScore;		
		m_pIAndroidUserItem->SetGameTimer(IDI_USER_ROBOTDELAY,1);		
	}

	return true;
}

//һ�ֽ���
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
	//��ע����
	if (pTurnOver->wAddScoreUser != wMeChairID)
	{
		//��ע����
		if (pTurnOver->lAddScoreCount > 0)
		{
			m_lTableScore[pTurnOver->wAddScoreUser] += pTurnOver->lAddScoreCount;
		}
	}

	//���ñ���
	m_wCurrentUser = INVALID_CHAIR;

	m_lTurnLessScore = pTurnOver->lTurnLessScore;
	m_lAddLessScore = pTurnOver->lAddLessScore;

	return true;
}

//�û�����
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
	//::OutputDebugString(TEXT("CAndroidUserItemSink::OnSubGiveUp()"));

	//Ч������
	VERIFY_RETURN_FALSE (wDataSize==sizeof(CMD_S_GiveUp));
	CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

	m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);		
	//���ñ���
	ASSERT(VALIDE_CHAIRID(pGiveUp->wGiveUpUser));
	m_cbPlayStatus[pGiveUp->wGiveUpUser] = FALSE;
	//CString str;
	//str.Format(TEXT("nTableID %d wChairID %d ������ע wGiveUpUser %d \n"),m_pIAndroidUserItem->GetTableID(),m_pIAndroidUserItem->GetChairID(),pGiveUp->wGiveUpUser);
	//::OutputDebugString(str);
	return true;
}

bool CAndroidUserItemSink::RobotGameMessage(WORD wChairID, LONG lScore,BYTE ID) 
{
	//::OutputDebugString(TEXT("CAndroidUserItemSink::RobotGameMessage()"));

	switch(ID)
	{
	case CHIP_TYPE_GIVEUP://�����˷���
		{
			//״̬�ж�
			ASSERT(m_cbPlayStatus[wChairID]==TRUE);
			if (m_cbPlayStatus[wChairID]==FALSE) return false;		
			//CString str;
			//str.Format(TEXT("nTableID %d wChairID %d ����������ע \n"),m_pIAndroidUserItem->GetTableID(),m_pIAndroidUserItem->GetChairID());
			//::OutputDebugString(str);
			//��Ϣ����
			m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP);
			break;
		}
	case CHIP_TYPE_PASS://��
	case CHIP_TYPE_GEN://��
	case CHIP_TYPE_ADD://��ע
	case CHIP_TYPE_ALLIN://ȫѹ
		{
			ASSERT(m_cbPlayStatus[wChairID]==TRUE);
			if (m_cbPlayStatus[wChairID]==FALSE) return false;
			CMD_C_AddScore AddScore;
			AddScore.ID  = ID;	
			if( ID == CHIP_TYPE_PASS)//��
			{
				m_RobottLogic.m_lRobotsScore = 0;			
			}
			else if( ID == CHIP_TYPE_GEN )//��
			{
			}		
			else if ( ID == CHIP_TYPE_ALLIN ) //ȫѺ
			{
			}
			else if( ID == CHIP_TYPE_ADD )//��ע
			{
				if(m_RobottLogic.m_lRobotsScore > m_lTurnMaxScore[wChairID]) //�������ʣ����룬ȫѹ
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
			//��Ϣ����
			m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));
			break;
		}
	}
	return false;
}

//��Ϸ����
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
	//Ч�����
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
	str.Format(TEXT("��Ϸ���� ������ lUserTakeGold[0] %d 1. %d 2. %d 3. %d 4. %d 5. %d 6. %d 7. %d 8. %d wChairID %d m_lSelfTakeGold %d \n"), pGameEnd->lChipScore[0], pGameEnd->lChipScore[1],
		pGameEnd->lChipScore[2], pGameEnd->lChipScore[3], pGameEnd->lChipScore[4], pGameEnd->lChipScore[5], pGameEnd->lChipScore[6],
		pGameEnd->lChipScore[7], pGameEnd->lChipScore[8], wChairID, m_lSelfTakeGold);
	::OutputDebugString(str);
#endif

	if (m_lSelfTakeGold < m_lAddTakeGoldMin)
	{
		//UINT nElapse = rand() % TIME_START_GAME + 2;
		//m_pIAndroidUserItem->SetGameTimer(IDI_ADDTAKEGOLD, nElapse);
		CString str;
		str.Format(_T("������ %s ���н�� %d С����С������ %d \n"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), m_lSelfTakeGold, m_lAddTakeGoldMin);
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
		//str.Format(_T("������ %s ���ö�ʱ�� %d ��ʱʱ�� %d \n"), m_pIAndroidUserItem->GetMeUserItem()->GetNickName(), IDI_START_GAME, nElapse);
		//::OutputDebugString(str);
		
	}

	return true;
}

bool CAndroidUserItemSink::OnSubDaShang( void * pData, WORD wDataSize )
{
	//::OutputDebugString(TEXT("CAndroidUserItemSink::OnSubDaShang()"));

	//Ч�����
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

//�����������
DECLARE_CREATE_MODULE(AndroidUserItemSink);
//////////////////////////////////////////////////////////////////////////
