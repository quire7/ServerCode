#include "StdAfx.h"
#include "servercontrolitemsink.h"


CServerControlItemSink::CServerControlItemSink(void)
{
	m_cbExcuteTimes = 0;								
	m_cbControlStyle = 0;	
	m_cbWinAreaCount = 0;
	ZeroMemory(m_bWinArea, sizeof(m_bWinArea));	
	ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
	ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
	ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
	ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//����������
bool  CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
	//��������й���ԱȨ�� �򷵻ش���
	if( !CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) )
	{
		return false;
	}

	const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pDataBuffer);
	switch(AdminReq->cbReqType)
	{
	case RQ_RESET_CONTROL:
		{	
			m_cbControlStyle=0;
			m_cbWinAreaCount=0;
			m_cbExcuteTimes=0;
			ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

			CMD_S_CommandResult cResult;
			cResult.cbResult=CR_ACCEPT;
			cResult.cbAckType=ACK_RESET_CONTROL;
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;
		}
	case RQ_SET_WIN_AREA:
		{

			const tagAdminReq*pAdminReq=reinterpret_cast<const tagAdminReq*>(AdminReq->cbExtendData);
			switch(pAdminReq->m_cbControlStyle)
			{
			case CS_BET_AREA:	//��������
				{
					m_cbControlStyle=pAdminReq->m_cbControlStyle;
					m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
					m_cbWinAreaCount=0;
					BYTE cbIndex=0;
					for(cbIndex=0;cbIndex<AREA_COUNT;cbIndex++)
					{
						m_bWinArea[cbIndex]=pAdminReq->m_bWinArea[cbIndex];
						if(m_bWinArea[cbIndex])
							m_cbWinAreaCount++;
					}
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_ACCEPT;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
					break;
				}
			case CS_BANKER_LOSE:	//ׯ������
				{
					m_cbControlStyle=pAdminReq->m_cbControlStyle;
					m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_ACCEPT;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

					break;
				}
			case CS_BANKER_WIN:		//ׯ��Ӯ��
				{
					m_cbControlStyle=pAdminReq->m_cbControlStyle;
					m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_ACCEPT;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
					break;
				}
			default:	//�ܾ�����
				{
					CMD_S_CommandResult cResult;
					cResult.cbResult=CR_REFUSAL;
					cResult.cbAckType=ACK_SET_WIN_AREA;
					pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
					break;
				}
			}

			break;
		}
	case RQ_PRINT_SYN:
		{
			CMD_S_CommandResult cResult;
			cResult.cbResult=CR_ACCEPT;
			cResult.cbAckType=ACK_PRINT_SYN;
			tagAdminReq*pAdminReq=reinterpret_cast<tagAdminReq*>(cResult.cbExtendData);
			pAdminReq->m_cbControlStyle=m_cbControlStyle;
			pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
			memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
			pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
			break;
		}
	default:
		{
			break;
		}
	}
	return true;
}

//��Ҫ����
bool  CServerControlItemSink::NeedControl()
{
	if(m_cbControlStyle > 0 && m_cbExcuteTimes > 0)
	{
		return true;
	}

	return false;
}

//�������
bool  CServerControlItemSink::MeetControl(tagControlInfo ControlInfo)
{
	return true;
}

//��ɿ���
bool  CServerControlItemSink::CompleteControl()
{
	if (m_cbExcuteTimes > 0)
		m_cbExcuteTimes--;	

	if ( m_cbExcuteTimes == 0)
	{
		m_cbExcuteTimes = 0;								
		m_cbControlStyle = 0;	
		m_cbWinAreaCount = 0;
		ZeroMemory(m_bWinArea, sizeof(m_bWinArea));	
		ZeroMemory(m_nCompareCard, sizeof(m_nCompareCard));
		ZeroMemory(m_cbTableCardArray, sizeof(m_cbTableCardArray));
		ZeroMemory(m_cbTableCard, sizeof(m_cbTableCard));
		ZeroMemory(m_lAllJettonScore, sizeof(m_lAllJettonScore));
	}

	return true;
}

//�����Ƿ��ܳɹ�
bool CServerControlItemSink::IsSettingSuccess( BYTE cbControlArea[MAX_INDEX] )
{
	return true;
}

//���ؿ�������
bool  CServerControlItemSink::ReturnControlArea( tagControlInfo& ControlInfo )
{
	return true;
}

//��������
void  CServerControlItemSink::GetSuitResult( BYTE cbTableCardArray[][MAX_CARD], BYTE cbTableCard[], LONGLONG lAllJettonScore[] )
{
	//������
	memcpy(m_cbTableCardArray, cbTableCardArray, sizeof(m_cbTableCardArray));
	memcpy(m_cbTableCard, cbTableCard, sizeof(m_cbTableCard));
	memcpy(m_lAllJettonScore, lAllJettonScore, sizeof(m_lAllJettonScore));

	//�����˿�
	BYTE cbSuitStack[MAX_CARDGROUP] = {};

	//�Զ����
	GetSuitCardCombine(cbSuitStack);

	//��������ֽ������
	BYTE UserCard[MAX_CARDGROUP][MAX_CARD] = {};
	memcpy(UserCard,m_cbTableCardArray,sizeof(UserCard));
	BYTE cbIndex=0;
	for(cbIndex=0;cbIndex<MAX_CARDGROUP;cbIndex++)
		memcpy(cbTableCardArray[cbIndex],UserCard[cbSuitStack[cbIndex]],sizeof(BYTE)*MAX_CARD);
	memcpy(cbTableCard, m_cbTableCard, sizeof(m_cbTableCard));
}

//�Ƿ�����Ч���
bool CServerControlItemSink::GetSuitCardCombine(BYTE cbStack[])
{
	switch(m_cbControlStyle)
	{
	case CS_BET_AREA:
		{
			AreaWinCard(cbStack);
			break;
		}
	case CS_BANKER_WIN:
		{
			BankerWinCard(true,cbStack);
			break;
		}
	case CS_BANKER_LOSE:
		{
			BankerWinCard(false,cbStack);
			break;
		}
	}

	return true;
}

bool CServerControlItemSink::AreaWinCard(BYTE cbStack[])
{
	bool bIsUser[MAX_CARDGROUP]={0};	//�Ƿ��Ѿ�ʹ��
	BYTE cbStackCount=0;	//ջ��Ԫ������
	BYTE cbIndex=0;
	BYTE cbMultiple=0;
	//�Ƚ���֮���ϵ
	for(cbIndex=0;cbIndex<MAX_CARDGROUP;cbIndex++)
	{
		for(BYTE j=cbIndex;j<MAX_CARDGROUP;j++)
		{

			m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
			m_nCompareCard[j][cbIndex]*=cbMultiple;
			m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
		}
	}

	//�����������
	for(cbIndex=0;cbIndex<=MAX_CARDGROUP;)
	{
		if(cbIndex<MAX_CARDGROUP)
		{
			if(bIsUser[cbIndex]) cbIndex++;
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_CARDGROUP)	//����Ѿ��ҵ�һ�����
				{

					if(m_bWinArea[0]==(m_nCompareCard[cbStack[1]][cbStack[0]]>0?true:false)		\
						&&m_bWinArea[1]==(m_nCompareCard[cbStack[2]][cbStack[0]]>0?true:false)	\
						&&m_bWinArea[2]==(m_nCompareCard[cbStack[3]][cbStack[0]]>0?true:false)	\
						&&m_bWinArea[3]==(m_nCompareCard[cbStack[4]][cbStack[0]]>0?true:false))
						break;
					cbIndex=MAX_CARDGROUP;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else break;
		}
	}
	return true;
}

LONGLONG CServerControlItemSink::GetBankerWinScore(int nWinMultiple[])
{
	//ׯ������
	LONGLONG lBankerWinScore = 0;

	for(BYTE cbIndex=0;cbIndex<AREA_COUNT;cbIndex++)
	{
		lBankerWinScore+=m_lAllJettonScore[cbIndex+1]*nWinMultiple[cbIndex];
	}
	return lBankerWinScore;
}

void CServerControlItemSink::BankerWinCard(bool bIsWin,BYTE cbStack[])
{
	bool bIsUser[MAX_CARDGROUP]={0};	//�Ƿ��Ѿ�ʹ��
	BYTE cbStackCount=0;	//ջ��Ԫ������
	BYTE cbIndex=0;
	BYTE cbMultiple;
	//�Ƚ���֮���ϵ
	for(cbIndex=0;cbIndex<MAX_CARDGROUP;cbIndex++)
	{
		for(BYTE j=cbIndex;j<MAX_CARDGROUP;j++)
		{
			m_nCompareCard[j][cbIndex]=m_GameLogic.CompareCard(m_cbTableCardArray[cbIndex],MAX_CARD,m_cbTableCardArray[j],MAX_CARD,cbMultiple);
			m_nCompareCard[j][cbIndex]*=cbMultiple;
			m_nCompareCard[cbIndex][j]=-m_nCompareCard[j][cbIndex];
		}
	}

	//�����������
	for(cbIndex=0;cbIndex<=MAX_CARDGROUP;)
	{
		if(cbIndex<MAX_CARDGROUP)
		{
			if(bIsUser[cbIndex]) cbIndex++;
			else
			{
				cbStack[cbStackCount]=cbIndex;
				bIsUser[cbIndex]=true;
				cbStackCount++;
				if(cbStackCount==MAX_CARDGROUP)	//����Ѿ��ҵ�һ�����
				{
					int nWinMultiple[4];
					for(int x=0;x<4;x++)
					{
						nWinMultiple[x]=m_nCompareCard[cbStack[x+1]][cbStack[0]];
					}

					if(bIsWin&&GetBankerWinScore(nWinMultiple)<0)
						break;
					if(!bIsWin&&GetBankerWinScore(nWinMultiple)>=0)
						break;
					cbIndex=MAX_CARDGROUP;
				}
				else
				{
					cbIndex=0;
					continue;
				}
			}
		}
		else
		{
			if(cbStackCount>0)
			{
				cbIndex=cbStack[--cbStackCount];
				bIsUser[cbIndex]=false;
				cbIndex++;
				continue;
			}
			else break;
		}
	}
}
