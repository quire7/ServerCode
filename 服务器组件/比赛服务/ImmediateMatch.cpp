#include "StdAfx.h"
#include "ImmediateMatch.h"
#include "..\��Ϸ������\DataBasePacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define INVALID_VALUE				0xFFFF								//��Чֵ

//////////////////////////////////////////////////////////////////////////

//ʱ�Ӷ���
#define IDI_DISTRIBUTE_USER		    (IDI_MATCH_MODULE_START+100)		//�����û�

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CImmediateMatch::CImmediateMatch()
{
	//��������
	m_pMatchOption=NULL;	
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//����ָ��
	m_ppITableFrame=NULL;
	m_pCurMatchGroup=NULL;

	//�ں˽ӿ�
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//����ӿ�
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;
	m_pIServerUserItemSink=NULL;

	memset(&m_MatchDesc, 0, sizeof(m_MatchDesc));
}

CImmediateMatch::~CImmediateMatch(void)
{
	SafeDeleteArray(m_ppITableFrame);
	m_aryFreeTableID.RemoveAll();
	
	m_LoopTimer.RemoveAll();
	SafeDelete(m_pCurMatchGroup);
	m_MatchGroup.Append(m_OverMatchGroup);
	for (int i=0; i<m_MatchGroup.GetCount(); i++)
	{
		SafeDelete(m_MatchGroup[i]);
	}

	m_MatchGroup.RemoveAll();
	m_OverMatchGroup.RemoveAll();
}

//�ӿڲ�ѯ
VOID* CImmediateMatch::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);	
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//����֪ͨ
void CImmediateMatch::OnStartService()
{
	//��������
	if (m_pCurMatchGroup==NULL)
	{
		m_pCurMatchGroup=new CImmediateGroup((DWORD)time(NULL),m_pMatchOption,this);
	}

	//���ͬʱ9����
	for (int i=0;i<79;i++)
	{
		m_LoopTimer.Add(i);
	}
	m_pITimerEngine->SetTimer(IDI_CHECK_OVER_MATCH,60000L,TIMES_INFINITY,0);
	m_pITimerEngine->SetTimer(IDI_DELETE_OVER_MATCH, 6000,TIMES_INFINITY,0);
//#ifdef _DEBUG
//	m_pITimerEngine->SetTimer(IDI_ANDROID_SIGNUP,1000,TIMES_INFINITY,0);
//#else
//	m_pITimerEngine->SetTimer(IDI_ANDROID_SIGNUP,3000,TIMES_INFINITY,0);
//#endif
}

//������
bool CImmediateMatch::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//��������
	CTableFrameHook * pTableFrameHook=new CTableFrameHook();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame,IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//���ýӿ�
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_ppITableFrame[wTableID]=pTableFrame;

	return true;
}

//��ʼ���ӿ�
bool CImmediateMatch::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	//��������
	m_pMatchOption=MatchManagerParameter.pGameMatchOption;
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;
	m_pImmediate = &(m_pMatchOption->MatchType.Immediate);

	//�ں����
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//�������		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//������������
	CImmediateGroup::m_wChairCount = m_pGameServiceAttrib->wChairCount;
	if (m_pMatchOption->cbMatchType == MATCH_TYPE_IMMEDIATE)
	{
		WORD wStartUserCount = m_pImmediate->wStartUserCount;
		if (CImmediateGroup::m_wChairCount > wStartUserCount)
		{
			CImmediateGroup::m_wChairCount = wStartUserCount;
		}
	}

	if (m_ppITableFrame==NULL)
	{
		m_ppITableFrame=new ITableFrame*[m_pGameServiceOption->wTableCount];

		for (WORD i = 0; i < m_pGameServiceOption->wTableCount; i++)
		{
			m_aryFreeTableID.Add(i);
		}
	}

	return true;
}

//ʱ���¼�
bool CImmediateMatch::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	if(dwTimerID>=IDI_GROUP_TIMER_START)
	{
		for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
		{
			//�鿴���ĳ������Ķ�ʱ��
			if (m_MatchGroup[i]->m_LoopTimer==((dwTimerID-IDI_GROUP_TIMER_START)/10))
			{
				m_MatchGroup[i]->OnTimeMessage(dwTimerID-(m_MatchGroup[i]->m_LoopTimer*10),dwBindParameter);
			}
		}
		return true;
	}

	switch(dwTimerID)
	{
	case IDI_CHECK_OVER_MATCH:
		{
			//��ʱ���һ�� ��Щ����������������������
			for (INT_PTR i=m_MatchGroup.GetCount()-1;i>=0;i--)
			{
				//�����û�С���������� ��Ҫ����
				m_MatchGroup[i]->CheckMatchUser();
			}
			return true;
		}
	case IDI_DELETE_OVER_MATCH:
		{
			for (int i=0;m_OverMatchGroup.GetCount();i++)
			{
				CImmediateGroup * pOverMatch=m_OverMatchGroup[i];
				m_OverMatchGroup.RemoveAt(i--);
				SafeDelete(pOverMatch);
			}
			return true;
		}
	}

	return true;
}


//���ݿ��¼�
bool CImmediateMatch::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_MATCH_SIGNUP_RESULT:		//�������
		{
			//����Ч��
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;			

			//����ʧ��
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//�������
			if(m_pCurMatchGroup->OnUserSignUpMatch(pIServerUserItem)==false)
			{
				SendRoomMessage(pIServerUserItem, TEXT("����ʧ��"),SMT_CHAT);
				return false;
			}

			//�����ɹ�
			m_OnMatchUserItem.Add(pIServerUserItem);
			//SendMatchInfo(NULL);
			if(m_pIGameServiceFrame!=NULL)
			{
				SendRoomMessage(pIServerUserItem, TEXT("��ϲ�������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);
			}

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchSignupResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pIServerUserItem->GetUserInfo()->dwExperience;

				//��������
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	case DBO_GR_MATCH_UNSIGNUP_RESULT:		//�������
		{
			//����Ч��
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBO_GR_MatchSingupResult)) return false;

			//��ȡ����
			DBO_GR_MatchSingupResult * pMatchSignupResult = (DBO_GR_MatchSingupResult*)pData;

			//�˷�ʧ��
			if(pMatchSignupResult->bResultCode==false)
			{
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pMatchSignupResult->szDescribeString,SMT_EJECT);

				return true;
			}

			//����״̬
			pIServerUserItem->SetSignUpStatus(MatchStatus_Null);

			//��������
			LPCTSTR pszMatchFeeType[]={TEXT("��Ϸ��"),TEXT("Ԫ��")};
			if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
			{
				//�˷�֪ͨ
				TCHAR szDescribe[128]=TEXT("");
				_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("�����ɹ����˻�������%I64d%s��"), m_pMatchOption->lMatchFee, pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);
				m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,szDescribe,SMT_EJECT);
			}

			//��Ҹ���
			if(pIServerUserItem->IsAndroidUser()==false)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchSignupResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchSignupResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pIServerUserItem->GetUserInfo()->dwExperience;

				//��������
				return SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}	
	case DBO_GR_MATCH_RANK_LIST:			//��������
		{
			//����У��
			ASSERT(wDataSize<=sizeof(DBO_GR_MatchRankList));
			if(wDataSize>sizeof(DBO_GR_MatchRankList)) return false;

			break;
		}
	case DBO_GR_MATCH_REWARD_RESULT:		//�������
		{
			//����Ч��
			if(pIServerUserItem==NULL) return true;
			if(wDataSize>sizeof(DBR_GR_MatchRewardResult)) return false;

			//��ȡ����
			DBR_GR_MatchRewardResult * pMatchRewardResult = (DBR_GR_MatchRewardResult*)pData;

			//�����ɹ�
			if(pMatchRewardResult->bResultCode==true)
			{
				//����ṹ
				CMD_GR_MatchGoldUpdate MatchGoldUpdate;

				//���ñ���
				MatchGoldUpdate.lCurrGold=pMatchRewardResult->lCurrGold;
				MatchGoldUpdate.lCurrIngot=pMatchRewardResult->lCurrIngot;
				MatchGoldUpdate.dwCurrExprience=pMatchRewardResult->dwCurrExperience;

				//��������
				SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_GOLDUPDATE,&MatchGoldUpdate,sizeof(MatchGoldUpdate));
			}

			break;
		}
	}
	return true;
}

//������Ϣ
bool CImmediateMatch::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_FEE:	//��������
		{
			//����Ч��
			ASSERT(wDataSize==sizeof(SCORE));
			if(wDataSize!=sizeof(SCORE)) return false;

			//��ȡ����
			SCORE lMatchFee=*(SCORE*)pData;
			if(lMatchFee!=m_pMatchOption->lMatchFee)return false;

			//��������
			DBR_GR_MatchSignup MatchSignup;
			ZeroMemory(&MatchSignup,sizeof(MatchSignup));

			//����ṹ
			MatchSignup.dwUserID=pIServerUserItem->GetUserID();
			MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
			MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
			MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
			MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
			MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
			lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_SIGNUP,dwSocketID,&MatchSignup,sizeof(MatchSignup));

			return true;
		}
	case SUB_GR_LEAVE_MATCH:	//�˳�����
		{
			//����Ч��
			ASSERT(pIServerUserItem!=NULL);
			if(pIServerUserItem==NULL) return false;

			OnEventUserQuitMatch(pIServerUserItem,0,0,dwSocketID);

			return true;
		}
	}
	return true;
}

//�û���¼
bool CImmediateMatch::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//return SendMatchInfo(pIServerUserItem);
	pIServerUserItem->SetClientReady(true);
	return true;
}

//�û��ǳ�
bool CImmediateMatch::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//�����¼�
bool CImmediateMatch::OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile)
{
	//������Ϣ
	CMD_GR_LogonUserID * pLogonUserID = (CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword) - 1] = 0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID) - 1] = 0;

	//��������
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//��������
	LogonUserID.dwClientAddr = dwUserIP;
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	LogonUserID.dwMatchID = m_pMatchOption->dwMatchID;
	LogonUserID.dwMatchNO = m_pMatchOption->dwMatchNO;
	lstrcpyn(LogonUserID.szPassword, pLogonUserID->szPassword, CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID, pLogonUserID->szMachineID, CountArray(LogonUserID.szMachineID));

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID, DBR_GR_LOGON_USERID, dwSocketID, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//�û�����
bool CImmediateMatch::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL)
	{
		CTraceService::TraceString(TEXT("�û�ָ��Ϊ��!"),TraceLevel_Warning);
		return true;
	}

	if(CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule))
	{
		SendRoomMessage(pIServerUserItem, TEXT("��Ǹ������ϵͳά�����÷����ֹ�û�������������"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_pCurMatchGroup==NULL || m_pCurMatchGroup->m_enMatchStatus==MatchStatus_Wait)
	{
		SendRoomMessage(pIServerUserItem, TEXT("��Ǹ���÷�������������������Ժ��ٱ������߽�����һ���������"),SMT_CHAT);
		return true;
	}

	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]->GetUserID()==pIServerUserItem->GetUserID())
		{
			SendRoomMessage(pIServerUserItem, TEXT("���Ѿ��ɹ������������ظ�������"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	if(m_LoopTimer.GetCount()==0)
	{
		SendRoomMessage(pIServerUserItem, TEXT("��Ǹ��������Ų������Եȡ�"),SMT_CHAT);
		return true;
	}

	if(pIServerUserItem->GetUserStatus()!=US_FREE || pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRoomMessage(pIServerUserItem, TEXT("�Բ�������ǰ��״̬������μӱ�����"),SMT_CHAT);
		return true;
	}

	if (m_pImmediate->dwInitalScore==0&&pIServerUserItem->GetUserScore()<m_pImmediate->dwInitalBase*m_pImmediate->wMinEnterGold+m_pMatchOption->lMatchFee)
	{
		TCHAR szMsg[256]=TEXT("");
		_sntprintf_s(szMsg, CountArray(szMsg), TEXT("�Բ�������ǰ�Ľ�ҵ�����%d�����ܲμӱ�����"),
			m_pImmediate->dwInitalBase*m_pImmediate->wMinEnterGold+m_pMatchOption->lMatchFee);
		SendRoomMessage(pIServerUserItem, szMsg,SMT_CHAT|SMT_EJECT);
		return true;
	}

	//���˿۱�����
	if(m_pMatchOption->lMatchFee>0 && pIServerUserItem->IsAndroidUser()==false)
	{
		ASSERT(m_pIGameServiceFrame!=NULL);
		if(m_pIGameServiceFrame==NULL)
		{
			CTraceService::TraceString(TEXT("������ָ��Ϊ��!"),TraceLevel_Warning);
			return true;
		}

		//����ṹ
		CMD_GR_Match_Fee MatchFee;
		MatchFee.lMatchFee=m_pMatchOption->lMatchFee;

		//������ʾ
		if(MatchFee.lMatchFee>0)
		{
			//��������
			LPCTSTR pszMatchFeeType[]={TEXT("��Ϸ��"),TEXT("Ԫ��")};
			if(m_pMatchOption->cbMatchFeeType<CountArray(pszMatchFeeType))
			{
				_sntprintf_s(MatchFee.szNotifyContent, CountArray(MatchFee.szNotifyContent), TEXT("�������۳������� %I64d %s��ȷ��Ҫ������"), m_pMatchOption->lMatchFee, pszMatchFeeType[m_pMatchOption->cbMatchFeeType]);
			}
		}

		//������Ϣ
		WORD wDataSize=sizeof(MatchFee)-sizeof(MatchFee.szNotifyContent);
		wDataSize+=CountStringBuffer(MatchFee.szNotifyContent);
		m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_FEE,&MatchFee,wDataSize);

		return true;
	}

	//�����˱���
	if(m_pMatchOption->lMatchFee==0 || pIServerUserItem->IsAndroidUser()==true)
	{
		//�������ж�
		if(pIServerUserItem->IsAndroidUser()==true && (m_pCurMatchGroup->m_wAndroidUserCount >= 
		   m_pImmediate->wAndroidUserCount))
		{
			return true;
		}

		return OnEventSocketMatch(SUB_GR_MATCH_FEE,&m_pMatchOption->lMatchFee,sizeof(SCORE),pIServerUserItem,dwSocketID);
	}

	//�����ɹ�
	m_OnMatchUserItem.Add(pIServerUserItem);
	//SendMatchInfo(NULL);
	if(m_pIGameServiceFrame!=NULL)
	{
		SCORE lScore=SCORE(m_pMatchOption->lMatchFee)*-1;

		SendRoomMessage(pIServerUserItem, TEXT("��ϲ�������ɹ�����ȴ�ϵͳΪ��������λ��"),SMT_CHAT);
	}

	return true;
}

//�û�����
bool CImmediateMatch::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϸ״̬
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING)
	{
		return true;
	}

	//���ڱ����׶�
	BYTE cbUserEnlist=pIServerUserItem->GetSignUpStatus();
	if (cbUserEnlist==MS_MATCHING)
	{
		return true;
	}

	//�����������
	if(RemoveMatchUserItem(pIServerUserItem))
	{
		//�����ڱ���������ɾ�������
		INT_PTR nGroupCount=m_MatchGroup.GetCount();
		for (INT_PTR i=0;i<nGroupCount;i++)
		{
			CImmediateGroup *pMatch=m_MatchGroup[i];
			if(pMatch->OnUserQuitMatch(pIServerUserItem))
			{
				//if(pMatch->m_enMatchStatus!=MatchStatus_Signup && pMatch->m_enMatchStatus!=MatchStatus_Wait)
				//{
				//	SendRoomMessage(pIServerUserItem, TEXT("�����ɹ�����ӭ�������μ�����������"), SMT_CHAT|SMT_EJECT);
				//}
				if (pMatch->m_enMatchStatus!=MatchStatus_Null)
				{
					SendRoomMessage(pIServerUserItem, TEXT("�����ɹ�����ӭ�������μ�����������"), SMT_CHAT|SMT_EJECT);
				}
				return true;
			}
		}

		//�����ڱ���������ɾ�������
		if(m_pCurMatchGroup->OnUserQuitMatch(pIServerUserItem))
		{
			//�˻�������
			if(m_pMatchOption->lMatchFee>0 && !pIServerUserItem->IsAndroidUser())
			{
				//��������
				DBR_GR_MatchSignup MatchSignup;
				MatchSignup.dwUserID=pIServerUserItem->GetUserID();

				//����ṹ
				MatchSignup.lMatchFee=m_pMatchOption->lMatchFee;
				MatchSignup.dwInoutIndex=pIServerUserItem->GetInoutIndex();
				MatchSignup.dwClientAddr=pIServerUserItem->GetClientAddr();
				MatchSignup.dwMatchID=m_pMatchOption->dwMatchID;
				MatchSignup.dwMatchNO=m_pCurMatchGroup->m_dwMatchNO;
				lstrcpyn(MatchSignup.szMachineID,pIServerUserItem->GetMachineID(),CountArray(MatchSignup.szMachineID));

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(MatchSignup.dwUserID,DBR_GR_MATCH_UNSIGNUP,dwContextID,&MatchSignup,sizeof(MatchSignup));
			}

			if (m_pCurMatchGroup->m_enMatchStatus!=MatchStatus_Null)
			{
				SendRoomMessage(pIServerUserItem, TEXT("�����ɹ�����ӭ�������μ�����������"), SMT_CHAT|SMT_EJECT);
			}

			//SendMatchInfo(NULL);
			return true;
		}
		//SendMatchInfo(NULL);
	}

	return false;
}

//�û�����
bool CImmediateMatch::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CImmediateGroup * pImmediateGroup = static_cast<CImmediateGroup *>(pIServerUserItem->GetMatchData());
	if(pImmediateGroup==NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,cbReason);
	}

	//���ͷ���
	pImmediateGroup->SendUserScore(pIServerUserItem);

	//��������
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();

	//��������
	CMD_GR_UserScore UserScore;

	//��������
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	UserScore.UserScore.dwExperience = pUserInfo->dwExperience;

	//�������
	UserScore.UserScore.lScore = pUserInfo->lScore;
	UserScore.UserScore.lScore += pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lScore += pIServerUserItem->GetFrozenedScore();

	//��������
	m_pIGameServiceFrame->SendDataToTable(pIServerUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));

	//��ʱд��
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//������Ϣ
		WriteGameScore.dwMatchID=m_pMatchOption->dwMatchID;
		WriteGameScore.dwMatchNO=pImmediateGroup->m_dwMatchNO;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);
	}

	return true;
}

//�û�״̬
bool CImmediateMatch::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//�������
	if (pIServerUserItem->GetUserStatus() == US_FREE)
	{
		pIServerUserItem->SetMatchData(NULL);
	}

	//�ص��ӿ�
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wOldTableID,wOldChairID);
	}

	return true;
}

//�û�Ȩ��
bool CImmediateMatch::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem,dwAddRight,dwRemoveRight,cbRightKind);
	}

	return true;
}

//������������ʼ����
bool CImmediateMatch::OnEventMatchStart(CImmediateGroup *pMatch)
{
	ASSERT(pMatch==m_pCurMatchGroup&&m_LoopTimer.GetCount()>0);
	
	//��Ų��� ���ܱ�����
	pMatch->m_LoopTimer=m_LoopTimer[0];
	m_LoopTimer.RemoveAt(0);
	m_MatchGroup.Add(pMatch);

	//׼���µ�һ�����
	m_pCurMatchGroup=new CImmediateGroup((DWORD)time(NULL),m_pMatchOption,this);

	//SendMatchInfo(NULL);
	BYTE cbMatchStatus=MS_MATCHING;
	pMatch->SendData(NULL, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));

	return true;
}


//��������
bool CImmediateMatch::OnEventMatchOver(CImmediateGroup *pMatch)
{
	//��������
	if(pMatch!=NULL)
	{
		DBR_GR_MatchOver MatchOver;
		ZeroMemory(&MatchOver,sizeof(MatchOver));

		//��������						
		MatchOver.dwMatchID=m_pMatchOption->dwMatchID;
		MatchOver.dwMatchNO=pMatch->m_dwMatchNO;		
		MatchOver.cbMatchType=MATCH_TYPE_IMMEDIATE;
		MatchOver.wServerID=m_pGameServiceOption->wServerID;
		
		//��ȡʱ��
		MatchOver.MatchStartTime=pMatch->m_MatchStartTime;
		CTime::GetCurrentTime().GetAsSystemTime(MatchOver.MatchEndTime);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(0,DBR_GR_MATCH_OVER,0,&MatchOver,sizeof(MatchOver));
	}

	return true;
}

//�رն�ʱ��
bool CImmediateMatch::KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)
{
	m_pITimerEngine->KillTimer(dwTimerID);

	return false;
}

//���ö�ʱ��
bool CImmediateMatch::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)
{
	return m_pITimerEngine->SetTimer(dwTimerID,dwElapse,dwRepeat,dwBindParameter);
}


//��������
bool CImmediateMatch::SendMatchInfo(IServerUserItem * pIServerUserItem)
{
	CMD_GR_Match_Num MatchNum;
	MatchNum.dwWaitting=(DWORD)m_pCurMatchGroup->m_OnMatchUserMap.GetCount();
	MatchNum.dwTotal=m_pImmediate->wStartUserCount;
	if(pIServerUserItem==NULL)
	{
		m_pIGameServiceFrame->SendData(BG_ALL_CLIENT,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
		return true;
	}

	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_NUM,&MatchNum, sizeof(MatchNum));
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_MATCH,SUB_GR_MATCH_DESC, &m_MatchDesc, sizeof(m_MatchDesc));
	INT_PTR dwUserCount=m_OnMatchUserItem.GetCount();
	for (INT_PTR i=0;i<dwUserCount;i++)
	{
		if(m_OnMatchUserItem[i]==pIServerUserItem)
		{
			BYTE cbMatchStatus=pIServerUserItem->GetSignUpStatus();
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GR_MATCH, SUB_GR_MATCH_STATUS, &cbMatchStatus, sizeof(cbMatchStatus));
			return true;
		}
	}

	tagUserInfo *pUserScore=pIServerUserItem->GetUserInfo();
	pUserScore->dwWinCount=pUserScore->dwLostCount=pUserScore->dwFleeCount=pUserScore->dwDrawCount=pUserScore->cbGender=0;
	if((SCORE)m_pImmediate->dwInitalScore!=0)
	{
		pUserScore->lScore=(LONG)m_pImmediate->dwInitalScore;
	}

	//SendGroupUserScore(pIServerUserItem, NULL);
	return true;
}

//������Ϸ��Ϣ
bool CImmediateMatch::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	return m_pIGameServiceFrame->SendGameMessage(pIServerUserItem, lpszMessage, wMessageType);
}

//������Ϣ
bool CImmediateMatch::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	return m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, lpszMessage, wMessageType);
}

//��������
bool CImmediateMatch::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if (pIServerUserItem != NULL)
	{
		return m_pIGameServiceFrame->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	else
	{
		return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, wMainCmdID, wSubCmdID, pData, wDataSize);
	}

	return true;
}

//Ϊ���ֲ��������
void CImmediateMatch::GetNullTable(CImmediateGroup* pImmediateGroup, CTableFrameMananerArray& aryTableFrameInfo, WORD wTableCount)
{
	ASSERT(m_aryFreeTableID.GetCount() >= wTableCount);
	if (m_aryFreeTableID.GetCount()<wTableCount)
	{
		return;
	}

	//��������
	for (WORD i = 0; i < wTableCount; i++)
	{
		WORD wFreeTableID = m_aryFreeTableID[i];
		m_aryFreeTableID.RemoveAt(i);

		tagTableFrameInfo * pTableFrameInfo = new tagTableFrameInfo;
		ZeroMemory(pTableFrameInfo, sizeof(tagTableFrameInfo));
		pTableFrameInfo->wTableID = wFreeTableID;
		pTableFrameInfo->pTableFrame = m_ppITableFrame[wFreeTableID];
		pTableFrameInfo->dwBaseScore = m_pImmediate->dwInitalBase;
		ITableFrameHook * pFrameHook = QUERY_OBJECT_PTR_INTERFACE(m_ppITableFrame[wFreeTableID]->GetTableFrameHook(), ITableFrameHook);
		pFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(pImmediateGroup, IUnknownEx));
		aryTableFrameInfo.Add(pTableFrameInfo);
	}
}

//��������
void CImmediateMatch::RecoverTable(CTableFrameMananerArray& aryTableFrameInfo)
{
	INT_PTR nCount = aryTableFrameInfo.GetCount();
	for (INT_PTR i = 0; i < nCount; i++)
	{
		m_aryFreeTableID.Add(aryTableFrameInfo[i]->wTableID);
	}
}

//��ȡһ�����еĻ�����
IAndroidUserItem * CImmediateMatch::GetFreeAndroidUserItem()
{

	if(m_pAndroidUserManager->GetAndroidCount()==0) return NULL;

	//����״̬
	tagAndroidUserInfo AndroidSimulate;
	m_pAndroidUserManager->GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE|ANDROID_PASSIVITY|ANDROID_INITIATIVE);

	if (AndroidSimulate.wFreeUserCount == 0)
	{
		return NULL;
	}
	else
	{
		//�����ѱ����Ļ������ظ�����
		for(WORD i=0; i<AndroidSimulate.wFreeUserCount;i++)
		{
			IAndroidUserItem * pIAndroidUser = AndroidSimulate.pIAndroidUserFree[i];
			if (pIAndroidUser->GetMeUserItem()->GetSignUpStatus() == MatchStatus_Null)
			{
				ASSERT(pIAndroidUser->GetUserID() == pIAndroidUser->GetMeUserItem()->GetUserID());
				return pIAndroidUser;
			}
		}
	}

	return NULL;
}

//д�뽱��
bool CImmediateMatch::WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo)
{
	//����Ч��
	ASSERT(pIServerUserItem!=NULL && pMatchRankInfo!=NULL);
	if(pIServerUserItem==NULL || pMatchRankInfo==NULL) return false;

	//д���ж�
	bool bWriteReward=(pMatchRankInfo->lRewardGold+pMatchRankInfo->dwRewardIngot+pMatchRankInfo->dwRewardExperience)>0;

	//д�뽱��
	if(bWriteReward==true)
	{
		//��������
		CMD_GR_MatchResult MatchResult;
		ZeroMemory(&MatchResult,sizeof(MatchResult));

		//��������
		MatchResult.lGold += pMatchRankInfo->lRewardGold;
		MatchResult.dwIngot+=pMatchRankInfo->dwRewardIngot;
		MatchResult.dwExperience+=pMatchRankInfo->dwRewardExperience;

		//����ʾ
		_sntprintf_s(MatchResult.szDescribe, CountArray(MatchResult.szDescribe), TEXT("%s, ��ϲ����%s�л�õ�%d��,�������£�"),
			pIServerUserItem->GetNickName(),m_pMatchOption->szMatchName,pMatchRankInfo->wRankID);

		//�����û���Ϣ
		tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
		//pUserInfo->lIngot+=pMatchRankInfo->dwRewardIngot;
		pUserInfo->dwExperience+=pMatchRankInfo->dwRewardExperience;
		//SendGroupUserScore(pIServerUserItem, NULL);

		//��������
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			m_pIGameServiceFrame->SendData(pIServerUserItem, MDM_GF_FRAME, SUB_GR_MATCH_RESULT, &MatchResult, sizeof(MatchResult));
		}
	}
	else
	{
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�����ѽ�������ϲ����õ�%d������ӭ���μ��������εı�����"), pMatchRankInfo->wRankID);
		SendGameMessage(pIServerUserItem, szMessage, SMT_CLOSE_GAME);
		SendRoomMessage(pIServerUserItem, szMessage, SMT_CHAT|SMT_EJECT);

		return true;
	}

	//д���¼
	if(m_pIDataBaseEngine!=NULL)
	{
		//��������
		DBR_GR_MatchReward MatchReward;
		ZeroMemory(&MatchReward,sizeof(MatchReward));
		
		//����ṹ
		MatchReward.dwUserID=pIServerUserItem->GetUserID();	
		MatchReward.lRewardGold = pMatchRankInfo->lRewardGold;
		MatchReward.dwRewardIngot=pMatchRankInfo->dwRewardIngot;
		MatchReward.dwRewardExperience=pMatchRankInfo->dwRewardExperience;
		MatchReward.dwClientAddr=pIServerUserItem->GetClientAddr();

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(MatchReward.dwUserID,DBR_GR_MATCH_REWARD,0,&MatchReward,sizeof(MatchReward));
	}

	return true;
}

//ɾ���û�
bool CImmediateMatch::DeleteUserItem(DWORD dwUserIndex)
{
	try
	{
		if (m_pITCPNetworkEngineEvent == NULL)
		{
			throw 0;
		}
		m_pITCPNetworkEngineEvent->OnEventTCPNetworkShut(dwUserIndex,0,0L);
	}
	catch (...)
	{
		//�������
		ASSERT(FALSE);
		return false;
	}
	return true;
}

//�Ƴ������û�
bool CImmediateMatch::RemoveMatchUserItem(IServerUserItem *pIServerUserItem)
{
	for (INT_PTR i=0;i<m_OnMatchUserItem.GetCount();i++)
	{
		//��ȡ�û�
		IServerUserItem * pOnLineUserItem=m_OnMatchUserItem[i];
		
		//�û��ж�
		if (pOnLineUserItem==pIServerUserItem)
		{
			ASSERT(pOnLineUserItem->GetUserID()==pIServerUserItem->GetUserID());

			m_OnMatchUserItem.RemoveAt(i);
			return true;
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
