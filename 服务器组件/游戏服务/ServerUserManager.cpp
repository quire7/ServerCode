#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
DWORD CServerUserItem::g_dwCardLibCount = 0;
BYTE CServerUserItem::g_cbCardLibEnable = 0;
DWORD CServerUserItem::g_dwLibStartDateTime = 0;
DWORD CServerUserItem::g_dwLibEndDateTime = 0;
DWORD CServerUserItem::g_dwCardLibCustomCount = 0;

//���캯��
CServerUserItem::CServerUserItem()
{
	//���ӱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//�йܱ���
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;

	//��������
	m_lDefPutScore=0L;					//Ĭ��һ�δ�����ٻ���
	m_lLowPutScore=0L;					//���ڶ��ٴ������
	m_lNextPutScore = 0L;				//�´δ������
	//m_iExchangeRatio=0;					//�һ�����

	//��¼��Ϣ
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//�������
	m_cbMaxCardType = 0;
	ZeroMemory(m_aryCardData, sizeof(m_aryCardData));

	//�ͻ�����
	m_bAndroidUser=false;

	//����ӿ�
	m_pIServerUserItemSink=NULL;

	//ϵͳ����
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_NONE;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//������Ϣ
	m_dwSignUpTime=0;
	m_cbSignUpStatus=0;
	m_pMatchData=NULL;

	//��������
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));
	m_cbReUserStatus = US_NULL;

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//��¼��Ϣ
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//�ƿ�
	m_dwCardLibIndex = 0;
	m_dwStartLibTime = 0;
	m_dwStopLibTime = 0;
	m_cbCardLibEnabled = 0;

	return;
}

//��������
CServerUserItem::~CServerUserItem()
{
	m_UserProperty.RemoveAll();
}

//�ӿڲ�ѯ
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//����������
BYTE CServerUserItem::GetMaxCardData(BYTE* pData, BYTE cbCount)
{
	ASSERT(cbCount <= MAX_CARD_DATA_COUNT);
	if ( cbCount>MAX_CARD_DATA_COUNT )
	{
		return 0;
	}

	for (int i = 0; i < cbCount; i++)
	{
		pData[i] = m_aryCardData[i];
	}

	return m_cbMaxCardType;
}

//�û�ʤ��
WORD CServerUserItem::GetUserWinRate()
{
	//����ʤ��
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserLostRate()
{
	//��������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserDrawRate()
{
	//�������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//�û�����
WORD CServerUserItem::GetUserFleeRate()
{
	//��������
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//�Ա��ʺ�
bool CServerUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//Ч�����
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//���ȶԱ�
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//�ַ��Ա�
	if (nContrastLen!=nSourceLen) return false;
	int nRet = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pszNickName, nContrastLen, m_UserInfo.szNickName, nSourceLen);
	if (nRet!=CSTR_EQUAL) return false;

	return true;
}

//�Ա�����
bool CServerUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//���ȶԱ�
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//����Ա�
	if (nTargetLen!=nSourceLen) return false;
	int nRet = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pszPassword, nTargetLen, m_szLogonPass, nSourceLen);
	if (nRet!=CSTR_EQUAL) return false;

	return true;
}

//����״̬
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//��¼��Ϣ
	WORD wOldTableID=m_UserInfo.wTableID;
	WORD wOldChairID=m_UserInfo.wChairID;

	//���ñ���
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;
	if (cbUserStatus == US_PLAYING)
	{
		m_UserInfo.wLastTableID = wTableID;
	}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink != NULL)
	{
 		m_pIServerUserItemSink->OnEventUserItemStatus(this, wOldTableID, wOldChairID);
	}

	return true;
}

//д�����
bool CServerUserItem::WriteUserScore(SCORE lScore, SCORE lGrade, SCORE lRevenue, SCORE lIngot, BYTE cbScoreType, DWORD dwPlayTimeCount,DWORD dwWinExperience)
{
	//Ч��״̬
	ASSERT((m_UserInfo.dwUserID!=0L)&&(cbScoreType!=SCORE_TYPE_NULL));
	if ((m_UserInfo.dwUserID==0L)||(cbScoreType==SCORE_TYPE_NULL)) return false;

	//�޸�ʤ��
	switch (cbScoreType)
	{
	case SCORE_TYPE_WIN:	//ʤ��
		{
			m_UserInfo.dwWinCount++;
			m_RecordInfo.dwWinCount++;
			m_VariationInfo.dwWinCount++;
			break;
		}
	case SCORE_TYPE_LOSE:	//���
		{
			m_UserInfo.dwLostCount++;
			m_RecordInfo.dwLostCount++;
			m_VariationInfo.dwLostCount++;
			break;
		}
	case SCORE_TYPE_DRAW:	//�;�
		{
			m_UserInfo.dwDrawCount++;
			m_RecordInfo.dwDrawCount++;
			m_VariationInfo.dwDrawCount++;
			break;
		}
	case SCORE_TYPE_FLEE:	//�Ӿ�
		{
			m_UserInfo.dwFleeCount++;
			m_RecordInfo.dwFleeCount++;
			m_VariationInfo.dwFleeCount++;
			break;
		}
	}

	//���ñ�־
	m_bModifyScore=true;

	//���û���
	m_UserInfo.lScore+=lScore;
	//m_UserInfo.lGrade+=lGrade;
	//m_UserInfo.lIngot+=lIngot;
	//m_UserInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;	
	m_UserInfo.dwExperience += dwWinExperience;

	//��¼��Ϣ
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lGrade+=lGrade;
	m_RecordInfo.lRevenue+=lRevenue;
	//m_RecordInfo.lIngot+=lIngot;
	m_RecordInfo.dwPlayTimeCount+=dwPlayTimeCount;
	//m_RecordInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;
	m_RecordInfo.dwExperience += dwWinExperience;

	//�����Ϣ
	//m_VariationInfo.lIngot+=lIngot;
	m_VariationInfo.dwPlayTimeCount+=dwPlayTimeCount;
	//m_VariationInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;
	m_VariationInfo.dwExperience += dwWinExperience;
	
	//�������
	m_VariationInfo.lScore += lScore;
	m_VariationInfo.lGrade+=lGrade;
	m_VariationInfo.lRevenue+=lRevenue;	

	//�������
	if (m_lNextPutScore > 0)
	{
		// �ܻ���
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
		if (lTotalScore > m_lNextPutScore)
		{
			m_UserInfo.lScore = m_lNextPutScore;
			m_lTrusteeScore = (lTotalScore - m_lNextPutScore);// / m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0;
		}

		m_lNextPutScore = 0;
	}
	else
	{
		SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
		SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
		if (lLowPutScore > 0 && lPutScore > 0)
		{
			// ���ϻ���С�ڴ���������ƣ����������
			if (m_UserInfo.lScore < lLowPutScore)
			{
				if (IsAndroidUser() == true)
				{
					CString str;
					str.Format(TEXT("������ %s ����ǰ���н�� %d "), m_UserInfo.szNickName, m_UserInfo.lScore);
					::OutputDebugString(str);
					str.Format(TEXT("�����ܽ��Ϊ %d \n"), m_lTrusteeScore);
					::OutputDebugString(str);
				}

				// �ܻ���
				SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

				// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
				if (lTotalScore > lPutScore)
				{
					m_UserInfo.lScore = lPutScore;
					m_lTrusteeScore = (lTotalScore - lPutScore); // m_iExchangeRatio;
				}
				else
				{
					m_UserInfo.lScore = lTotalScore;
					m_lTrusteeScore = 0;
				}

				if (IsAndroidUser() == true)
				{
					CString str;
					str.Format(TEXT("������ %s �������н�� %d "), m_UserInfo.szNickName, m_UserInfo.lScore);
					::OutputDebugString(str);
					str.Format(TEXT("�����ܽ��Ϊ %d \n"), m_lTrusteeScore);
					::OutputDebugString(str);
				}
			}
		}
	}

	//�йܻ���
	//if (m_lRestrictScore>0L)
	//{
	//	//��������
	//	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

	//	//�йܵ���
	//	if (lTotalScore>m_lRestrictScore)
	//	{
	//		m_UserInfo.lScore=m_lRestrictScore;
	//		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	//	}
	//	else
	//	{
	//		m_lTrusteeScore=0L;
	//		m_UserInfo.lScore=lTotalScore;
	//	}
	//}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink != NULL)
	{
		m_pIServerUserItemSink->OnEventUserItemScore(this, TASK_TYPE_WIN_INNINGS | TASK_TYPE_SUM_INNINGS);
	}

	return true;
}

//��ѯ��¼
bool CServerUserItem::QueryRecordInfo(tagVariationInfo & RecordInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//���ñ���
	RecordInfo=m_RecordInfo;

	return true;
}

//��ȡ���
bool CServerUserItem::DistillVariation(tagVariationInfo & VariationInfo)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�޸��ж�
	if (m_bModifyScore==true)
	{
		//���ñ���
		VariationInfo=m_VariationInfo;
	}
	else
	{
		//���ñ���
		ZeroMemory(&VariationInfo,sizeof(VariationInfo));
	}

	//��ԭ����
	m_bModifyScore=false;
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	return true;
}

//�������
bool CServerUserItem::FrozenedUserScore(SCORE lScore)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	////Ч�����
	//ASSERT((lScore>=0L)&&((m_UserInfo.lScore+m_lTrusteeScore)>=lScore));
	//if ((lScore<0L)||((m_UserInfo.lScore+m_lTrusteeScore)<lScore)) return false;

	////���ñ���
	//m_lFrozenedScore+=lScore;

	////�����û�
	//if (m_lTrusteeScore<lScore)
	//{
	//	//��������
	//	lScore-=m_lTrusteeScore;
	//	m_UserInfo.lScore-=lScore;
	//	m_lTrusteeScore=0L;
	//}
	//else
	//{
	//	//�й�����
	//	m_lTrusteeScore-=lScore;
	//}

	SCORE lTrusteeScore = m_lTrusteeScore;// *m_iExchangeRatio;
	SCORE lAllScore = m_UserInfo.lScore + lTrusteeScore;
	
	//Ч�����
	ASSERT((lScore >= 0L) && (lAllScore >= lScore));
	if ((lScore < 0L) || (lAllScore < lScore)) return false;

	//���ñ���
	m_lFrozenedScore += lScore;

	//�����û�
	if (lTrusteeScore < lScore)
	{
		//��������
		lScore -= lTrusteeScore;
		m_UserInfo.lScore -= lScore;
		m_lTrusteeScore = 0L;
	}
	else
	{
		//�й�����
		m_lTrusteeScore -= lScore;// / m_iExchangeRatio;
	}

	return true;
}

//�ⶳ����
bool CServerUserItem::UnFrozenedUserScore(SCORE lScore)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//Ч�����
	ASSERT((lScore>=0L)&&(m_lFrozenedScore>=lScore));
	if ((lScore<0L)||(m_lFrozenedScore<lScore)) return false;

	//���ñ���
	m_lFrozenedScore -= lScore;
	m_UserInfo.lScore += lScore;

	//�������
	if (m_lNextPutScore > 0)
	{
		// �ܻ���
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
		if (lTotalScore > m_lNextPutScore)
		{
			m_UserInfo.lScore = m_lNextPutScore;
			m_lTrusteeScore = (lTotalScore - m_lNextPutScore); // m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0;
		}

		m_lNextPutScore = 0;
	}
	else
	{
		SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
		SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
		if (lLowPutScore > 0 && lPutScore > 0)
		{
			// ���ϻ���С�ڴ���������ƣ����������
			if (m_UserInfo.lScore < lLowPutScore)
			{
				// �ܻ���
				SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

				// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
				if (lTotalScore > lPutScore)
				{
					m_UserInfo.lScore = lPutScore;
					m_lTrusteeScore = (lTotalScore - lPutScore);// / m_iExchangeRatio;
				}
				else
				{
					m_UserInfo.lScore = lTotalScore;
					m_lTrusteeScore = 0;
				}
			}
		}
	}

	////�йܻ���
	//if (m_lRestrictScore>0L)
	//{
	//	//��������
	//	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

	//	//�йܵ���
	//	if (lTotalScore>m_lRestrictScore)
	//	{
	//		m_UserInfo.lScore=m_lRestrictScore;
	//		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	//	}
	//	else
	//	{
	//		m_lTrusteeScore=0L;
	//		m_UserInfo.lScore=lTotalScore;
	//	}
	//}

	return true;
}

//�޸���Ϣ
bool CServerUserItem::ModifyUserProperty(SCORE lScore,LONG lLoveLiness)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//���û���
	//m_UserInfo.lScore+=lScore;
	m_UserInfo.lLoveLiness+=lLoveLiness;

	//��¼��Ϣ
	//m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lLoveLiness+=lLoveLiness;

	//�����Ϣ
	if (lLoveLiness!=0L)
	{
		m_bModifyScore=true;
		m_VariationInfo.lLoveLiness+=lLoveLiness;
	}

	/**
	////�������
	//if (m_lNextPutScore > 0)
	//{
	//	// �ܻ���
	//	SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

	//	// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
	//	if (lTotalScore > m_lNextPutScore)
	//	{
	//		m_UserInfo.lScore = m_lNextPutScore;
	//		m_lTrusteeScore = (lTotalScore - m_lNextPutScore); // m_iExchangeRatio;
	//	}
	//	else
	//	{
	//		m_UserInfo.lScore = lTotalScore;
	//		m_lTrusteeScore = 0;
	//	}

	//	m_lNextPutScore = 0;
	//}
	//else
	//{
	//	SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
	//	SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
	//	if (lLowPutScore > 0 && lPutScore > 0)
	//	{
	//		// ���ϻ���С�ڴ���������ƣ����������
	//		if (m_UserInfo.lScore < lLowPutScore)
	//		{
	//			// �ܻ���
	//			SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

	//			// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
	//			if (lTotalScore > lPutScore)
	//			{
	//				m_UserInfo.lScore = lPutScore;
	//				m_lTrusteeScore = (lTotalScore - lPutScore); // m_iExchangeRatio;
	//			}
	//			else
	//			{
	//				m_UserInfo.lScore = lTotalScore;
	//				m_lTrusteeScore = 0;
	//			}
	//		}
	//	}
	//}

	//////�йܻ���
	////if ((m_lRestrictScore>0L)&&(lScore!=0L))
	////{
	////	//��������
	////	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;
	////
	////	//�йܵ���
	////	if (lTotalScore>m_lRestrictScore)
	////	{
	////		m_UserInfo.lScore=m_lRestrictScore;
	////		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	////	}
	////	else
	////	{
	////		m_lTrusteeScore=0L;
	////		m_UserInfo.lScore=lTotalScore;
	////	}
	////}

	////����״̬
	//ASSERT(m_pIServerUserItemSink!=NULL);
	////if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_PROPERTY);
	//if (m_pIServerUserItemSink != NULL)
	//{
	//	m_pIServerUserItemSink->OnEventUserItemScore(this, 0);
	//}
	*/

	return true;
}

//����������
bool CServerUserItem::ModifyMaxCardType(BYTE cbCurType, BYTE aryData[], BYTE cbPokerCount)
{
	ASSERT(cbPokerCount <= MAX_CARD_DATA_COUNT);
	if (cbPokerCount > MAX_CARD_DATA_COUNT)
	{
		return false;
	}

	m_cbMaxCardType = cbCurType;
	for (BYTE i = 0; i < cbPokerCount; i++)
	{
		if (i >= MAX_CARD_DATA_COUNT)
		{
			ASSERT(FALSE);
			return false;
		}

		m_aryCardData[i] = aryData[i];
	}

	return true;
}

//�����
bool CServerUserItem::DetachBindStatus()
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�û�����
	m_bClientReady=false;

	//��������
	m_wBindIndex=INVALID_WORD;

	return true;
}

//���в���
bool CServerUserItem::ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)
{
	//���û���
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lInsure+=lInsure;

	//��¼��Ϣ
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lInsure+=lInsure;
	m_RecordInfo.lRevenue+=lRevenue;
	
	//�������
	if (m_lNextPutScore > 0)
	{
		// �ܻ���
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
		if (lTotalScore > m_lNextPutScore)
		{
			m_UserInfo.lScore = m_lNextPutScore;
			m_lTrusteeScore = (lTotalScore - m_lNextPutScore); // m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0;
		}

		m_lNextPutScore = 0;
	}
	else
	{
		SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
		SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
		if (lLowPutScore > 0 && lPutScore > 0)
		{
			// ���ϻ���С�ڴ���������ƣ����������
			if (m_UserInfo.lScore < lLowPutScore)
			{
				// �ܻ���
				SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

				// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
				if (lTotalScore > lPutScore)
				{
					m_UserInfo.lScore = lPutScore;
					m_lTrusteeScore = (lTotalScore - lPutScore); // m_iExchangeRatio;
				}
				else
				{
					m_UserInfo.lScore = lTotalScore;
					m_lTrusteeScore = 0;
				}
			}
		}
	}

	////�йܻ���
	//if (m_lRestrictScore>0L)
	//{
	//	//��������
	//	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

	//	//�йܵ���
	//	if (lTotalScore>m_lRestrictScore)
	//	{
	//		m_UserInfo.lScore=m_lRestrictScore;
	//		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	//	}
	//	else
	//	{
	//		m_lTrusteeScore=0L;
	//		m_UserInfo.lScore=lTotalScore;
	//	}
	//}

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	//if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_INSURE);
	if (m_pIServerUserItemSink != NULL)
	{
		m_pIServerUserItemSink->OnEventUserItemScore(this, 0);
	}

	return true;
}

//��Ҵ���
SCORE CServerUserItem::TakeGold(SCORE lScore, bool isAtOnce)
{
	if (lScore <= 0) return lScore;

	// ���̴���
	if (isAtOnce)
	{
		// �ܻ���
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// �ܻ��ִ���Ĭ�ϴ�����֣�����Ĭ�ϻ���ֵ
		if (lTotalScore > lScore)
		{
			m_UserInfo.lScore = lScore;
			m_lTrusteeScore = (lTotalScore - lScore); // m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0L;
		}
	}
	else
	{
		m_lNextPutScore = lScore;
	}

	return m_UserInfo.lScore;
}

//������������
bool CServerUserItem::WriteOtherScore(SCORE lScore, BYTE cbType)
{
	//Ч��״̬
	ASSERT((m_UserInfo.dwUserID != 0L));
	if (m_UserInfo.dwUserID == 0L) return false;
	
	SCORE lTrusteeScore = m_lTrusteeScore;// *m_iExchangeRatio;
	SCORE lAllScore = m_UserInfo.lScore + lTrusteeScore;

	//Ч�����
	ASSERT((lScore >= 0L) && (lAllScore >= lScore));
	if ((lScore < 0L) || (lAllScore < lScore)) return false;

	//�����û�
	if (lTrusteeScore < lScore)
	{
		//��������
		lScore -= lTrusteeScore;
		m_UserInfo.lScore -= lScore;
		m_lTrusteeScore = 0L;
	}
	else
	{
		//�й�����
		m_lTrusteeScore -= lScore;// / m_iExchangeRatio;
	}

	//���ñ�־
	m_bModifyScore = true;

	//��¼��Ϣ
	m_RecordInfo.lScore -= lScore;

	//�������
	m_VariationInfo.lScore -= lScore;

	//����״̬
	ASSERT(m_pIServerUserItemSink != NULL);
	if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, cbType);

	return true;
}

//���ò���
bool CServerUserItem::SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, bool bClientReady)
{
	//Ч��״̬
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//�û�����
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//��������
	m_wBindIndex=wBindIndex;
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	return true;
}

//�޸�Ȩ��
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	//���Ȩ��
	m_dwUserRight |= dwAddRight;

	//ɾ��Ȩ��
	m_dwUserRight &= ~dwRemoveRight;

	//����״̬
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink != NULL)
	{
		m_pIServerUserItemSink->OnEventUserItemRight(this, dwAddRight, dwRemoveRight, cbRightKind);
	}

	return;
}

//�޸ľȼý���Ϣ
VOID CServerUserItem::SetBenefitInfo(tagSendBenefitSuc* pbenefit)
{
	if (pbenefit == NULL) return;

	ASSERT(pbenefit->dwUserID == m_UserInfo.dwUserID);
	m_UserInfo.cbBenefitTimes = pbenefit->cbBenefitTimes;
	CopyMemory(&m_UserInfo.LastBenefitTime, &pbenefit->LastBenefitTime, sizeof(SYSTEMTIME));

	return;
}

//���õ���
void CServerUserItem::SetUserProperty(tagUserProperty &Property)
{
	//�����ִ�
	if (m_UserProperty.GetCount() > 0)
	{
		//��������
		for (INT_PTR nIndex = 0; nIndex < m_UserProperty.GetCount(); nIndex++)
		{
			if (m_UserProperty[nIndex].wPropertyUseMark == Property.wPropertyUseMark)
			{
				m_UserProperty.RemoveAt(nIndex);
				break;
			}
		}
	}

	m_UserProperty.Add(Property);

	return;
}

//���ӵ���
void CServerUserItem::AddUserProperty(tagUserProperty &Property)
{
	//�����ִ�
	if (m_UserProperty.GetCount() > 0)
	{
		//��������
		for (INT_PTR nIndex = 0; nIndex < m_UserProperty.GetCount(); nIndex++)
		{
			if (m_UserProperty[nIndex].wPropertyUseMark == Property.wPropertyUseMark)
			{
				m_UserProperty[nIndex].wPropertyCount += Property.wPropertyCount;
				return;
			}
		}
	}

	m_UserProperty.Add(Property);
}

//ʹ�õ���
EN_PROPERTY_ERROR CServerUserItem::UsedProperty(CMD_GR_C_Property *pProperty, SYSTEMTIME &SystemTime, DWORD &dwPropDBID)
{
	if (pProperty == NULL) return EN_PROPERTY_ERROR::EN_PROP_NULL;

	//�����ִ�
	if (m_UserProperty.GetCount() <= 0) return EN_PROPERTY_ERROR::EN_PROP_NULL;

	//���ҵ���
	tagUserProperty* ptmpProp = NULL;

	for (INT_PTR nIndex = 0; nIndex < m_UserProperty.GetCount(); nIndex++)
	{
		if (m_UserProperty[nIndex].wPropertyUseMark == pProperty->wPropertyIndex)
		{
			ptmpProp = &m_UserProperty[nIndex];
			break;
		}
	}

	if (ptmpProp != NULL)
	{
		// ʵ������С��ʹ������
		if (ptmpProp->wPropertyCount < pProperty->wItemCount) return EN_PROPERTY_ERROR::EN_PROP_NUMBER;

		//�����ж�
		COleDateTime dTimeNow(SystemTime);
		COleDateTime dTimeOver(ptmpProp->OverTime);

		//��Чʱ��ֵ�ж�ʱ���; ��Чʱ���ʾ���ÿ���
		if (dTimeOver.m_status == COleDateTime::valid)
		{
			COleDateTimeSpan dTimeSpan = dTimeNow - dTimeOver;
			double nSecnonSpan = dTimeSpan.GetTotalSeconds();

			//����
			if (nSecnonSpan > 0)
			{
				return EN_PROPERTY_ERROR::EN_PROP_OVER;
			}
		}

		//����ʹ��
		if (pProperty->wItemCount > 0)
		{
			ptmpProp->wPropertyCount -= pProperty->wItemCount;
		}

		dwPropDBID = ptmpProp->dwDBID;

		return EN_PROPERTY_ERROR::EN_PROP_SUCC;
	}

	return EN_PROPERTY_ERROR::EN_PROP_NULL;
}

//��������
VOID CServerUserItem::ResetUserItem()
{
	//���ӱ���
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//�йܱ���
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//��¼��Ϣ
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//�������
	m_cbMaxCardType = 0;
	ZeroMemory(m_aryCardData, sizeof(m_aryCardData));

	//ϵͳ����
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//�ͻ�����
	m_bAndroidUser=false;

	//������Ϣ
	m_dwSignUpTime=0;
	m_cbSignUpStatus=0;
	m_pMatchData=NULL;

	//��������
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//���Ա���
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//��¼��Ϣ
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	m_UserProperty.RemoveAll();

	//�ӿڱ���
	m_pIServerUserItemSink=NULL;

	return;
}

//�Ƿ����ʹ���ƿ�
bool CServerUserItem::canUseCardLib()
{
	if (m_cbCardLibEnabled == 0)
	{
		return false;
	}

	CTime cTime = CTime::GetCurrentTime();
	DWORD dwCurTime = cTime.GetTime();
	if (dwCurTime >= m_dwStartLibTime && dwCurTime <= m_dwStopLibTime)
	{
		return true;
	}

	return false;
}

//��ȡ�ƿ�����
DWORD CServerUserItem::getCardLibIndex()
{
	return m_dwCardLibIndex;
}

//�����ƿ�����
void CServerUserItem::setCardLibIndex(DWORD dwCardLibIndex)
{
	m_dwCardLibIndex = dwCardLibIndex;
}

//�ۼ��ƿ�����
void CServerUserItem::addCardLibIndex()
{
	//if (++m_dwCardLibIndex >= g_dwCardLibCount)
	//{
	//	m_dwCardLibIndex = 0;
	//}

	if (++m_dwCardLibIndex > g_dwCardLibCustomCount)
	{
		m_dwCardLibIndex = 1;
	}
}

//�����ƿ������
void CServerUserItem::setCardLibEnabled(bool bEnabled)
{
	m_cbCardLibEnabled = bEnabled ? 1 : 0;
}

//�����ƿ�ʹ��ʱ���
void CServerUserItem::setUseCardLibTime(DWORD dwStartTime, DWORD dwEndTime)
{
	m_dwStartLibTime = dwStartTime;
	m_dwStopLibTime = dwEndTime;
}


//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerUserManager::CServerUserManager()
{
	//����ӿ�
	m_pIServerUserItemSink=NULL;

	//��������
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//��������
CServerUserManager::~CServerUserManager()
{
	//�ͷ��û�
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//�ɹ��ж�
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//ö���û�
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//�����û�
IServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//��������
DWORD CServerUserManager::GetAndroidCount()
{
	//��������
	DWORD dwAndroidCount=0;
	CServerUserItem * pServerUserItem=NULL;

	//�����û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->IsAndroidUser() == true)
		{
			dwAndroidCount++;
		}
	}

	return dwAndroidCount;
}

//�����û�
IServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszNickName)
{
	//��������
	CServerUserItem * pServerUserItem=NULL;

	//�����û�
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastNickName(pszNickName) == true)
		{
			return pServerUserItem;
		}
	}

	return NULL;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem()
{
	//�洢����
	m_UserItemStore.Append(m_UserItemArray);

	//ɾ������
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//ɾ���û�
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	TRACE2("CServerUserManager::DeleteUserItem %d Name %s", pIServerUserItem, pIServerUserItem->GetNickName());

	//��������
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//Ѱ�Ҷ���
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//��ȡ�û�
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//���ö���
		pTempUserItem->ResetUserItem();

		//ɾ������
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�����û�
bool CServerUserManager::InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo,tagUserInfoPlus & UserInfoPlus)
{
	//��������
	CServerUserItem * pServerUserItem=NULL;

	//��ȡָ��
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);

		//���ö���
		pServerUserItem->ResetUserItem();		
	}
	else
	{
		try
		{
			pServerUserItem=new CServerUserItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//���ýӿ�
	pServerUserItem->m_pIServerUserItemSink=m_pIServerUserItemSink;

	//ԭʼ����
	pServerUserItem->m_ScoreFormer.lScore=UserInfo.lScore;
	pServerUserItem->m_ScoreFormer.dwWinCount=UserInfo.dwWinCount;
	pServerUserItem->m_ScoreFormer.dwLostCount=UserInfo.dwLostCount;
	pServerUserItem->m_ScoreFormer.dwDrawCount=UserInfo.dwDrawCount;
	pServerUserItem->m_ScoreFormer.dwFleeCount=UserInfo.dwFleeCount;
	pServerUserItem->m_ScoreFormer.dwExperience=UserInfo.dwExperience;

	//���Ա���
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&pServerUserItem->m_RecordInfo,sizeof(pServerUserItem->m_RecordInfo));
	ZeroMemory(&pServerUserItem->m_VariationInfo,sizeof(pServerUserItem->m_VariationInfo));

	//��¼��Ϣ
	pServerUserItem->m_dwLogonTime=UserInfoPlus.dwLogonTime;
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//�û�Ȩ��
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;
	pServerUserItem->m_lRestrictScore=UserInfoPlus.lRestrictScore;

	//��������
	pServerUserItem->m_lDefPutScore = UserInfoPlus.lDefPutScore;
	pServerUserItem->m_lLowPutScore = UserInfoPlus.lLowPutScore;
	//pServerUserItem->m_iExchangeRatio = UserInfoPlus.iExchangeRatio;

	//�������
	pServerUserItem->m_cbMaxCardType = UserInfoPlus.cbMaxCardType;
	CopyMemory(pServerUserItem->m_aryCardData, UserInfoPlus.aryCardData, sizeof(UserInfoPlus.aryCardData));

	//������Ϣ
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;
	pServerUserItem->m_dwClientAddr=UserInfoPlus.dwClientAddr;
	lstrcpyn(pServerUserItem->m_szMachineID,UserInfoPlus.szMachineID,CountArray(pServerUserItem->m_szMachineID));

	//��������
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bTrusteeUser=false;
	pServerUserItem->m_bAndroidUser=UserInfoPlus.bAndroidUser;
	lstrcpyn(pServerUserItem->m_szLogonPass,UserInfoPlus.szPassword,CountArray(pServerUserItem->m_szLogonPass));

	////�йܵ���
	//if ((UserInfoPlus.lRestrictScore>0L)&&(UserInfo.lScore>UserInfoPlus.lRestrictScore))
	//{
	//	pServerUserItem->m_UserInfo.lScore=UserInfoPlus.lRestrictScore;
	//	pServerUserItem->m_lTrusteeScore=UserInfo.lScore-UserInfoPlus.lRestrictScore;
	//}

	//�������
	SCORE lputScore = UserInfoPlus.lDefPutScore * UserInfoPlus.iExchangeRatio;
	if (lputScore > 0)
	{
		ASSERT(UserInfoPlus.lDefPutScore > 0);

		if (UserInfo.lScore >= UserInfoPlus.lDefPutScore)
		{
			pServerUserItem->m_UserInfo.lScore = lputScore;
			pServerUserItem->m_lTrusteeScore = UserInfo.lScore - UserInfoPlus.lDefPutScore;
		}
		else
		{
			pServerUserItem->m_UserInfo.lScore = UserInfo.lScore * UserInfoPlus.iExchangeRatio;
			pServerUserItem->m_lTrusteeScore = 0;
		}
	}

	//�����û�
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//���ñ���
	*pIServerUserResult=pServerUserItem;

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
