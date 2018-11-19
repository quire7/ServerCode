#include "StdAfx.h"
#include "PrivateGame.h"
#include "..\��Ϸ������\DataBasePacket.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
PriaveteGame::PriaveteGame()
{
	m_pGameServiceOption=NULL;
	m_pGameServiceAttrib=NULL;

	//�ں˽ӿ�
	m_pTableInfo=NULL;
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngineEvent=NULL;

	//����ӿ�
	m_pIGameServiceFrame=NULL;
	m_pIServerUserManager=NULL;
	m_pAndroidUserManager=NULL;

	ZeroMemory(&m_kPrivateInfo, sizeof(CMD_GR_Private_Info));
}

PriaveteGame::~PriaveteGame(void)
{
	for (int i = 0; i < m_pGameServiceOption->wTableCount; i++)
	{
		if (m_pTableInfo[i].dwRoomNum != 0)
		{
			DelPRMRedisKey(m_pTableInfo[i].dwRoomNum);
		}
		DelRoomIDFromIDRedisKey(&m_pTableInfo[i]);
	}
	
	//�ͷ���Դ
	SafeDeleteArray(m_pTableInfo);

	//�رն�ʱ��
	m_pITimerEngine->KillTimer(IDI_DISMISS_WAITE_END);
}

//�ӿڲ�ѯ
VOID* PriaveteGame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{	
	QUERYINTERFACE(IGameMatchItem,Guid,dwQueryVer);
	QUERYINTERFACE(IMatchEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameMatchItem,Guid,dwQueryVer);
	return NULL;
}

//������
bool PriaveteGame::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	if(pTableFrame==NULL || wTableID>m_pGameServiceOption->wTableCount)
	{
		ASSERT(false);
		return false;
	}

	//��������
	CTableFramePrivate * pTableFrameHook=new CTableFramePrivate();
	pTableFrameHook->InitTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrame, IUnknownEx));
	pTableFrameHook->SetMatchEventSink(QUERY_OBJECT_PTR_INTERFACE(this,IUnknownEx));

	//���ýӿ�
	pTableFrame->SetTableFrameHook(QUERY_OBJECT_PTR_INTERFACE(pTableFrameHook,IUnknownEx));
	m_pTableInfo[wTableID].pITableFrame=pTableFrame;
	m_pTableInfo[wTableID].restValue();


	return true;
}

//��ʼ���ӿ�
bool PriaveteGame::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	m_pGameServiceOption=MatchManagerParameter.pGameServiceOption;
	m_pGameServiceAttrib=MatchManagerParameter.pGameServiceAttrib;

	//�ں����
	m_pITimerEngine=MatchManagerParameter.pITimerEngine;
	m_pIDataBaseEngine=MatchManagerParameter.pICorrespondManager;
	m_pITCPNetworkEngineEvent=MatchManagerParameter.pTCPNetworkEngine;

	//�������		
	m_pIGameServiceFrame=MatchManagerParameter.pIMainServiceFrame;		
	m_pIServerUserManager=MatchManagerParameter.pIServerUserManager;
	m_pAndroidUserManager=MatchManagerParameter.pIAndroidUserManager;
	m_pIServerUserItemSink=MatchManagerParameter.pIServerUserItemSink;

	//��������
	if (m_pTableInfo==NULL)
	{
		m_pTableInfo = new PrivateTableInfo[m_pGameServiceOption->wTableCount];
	}

	CStringA strHost = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerAddr);
	CStringA strCachePASS = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerPass);
	
	UINT nTimeOut = 50000;
	//������������;
	mRedis.init(strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, strCachePASS.GetBuffer(), nTimeOut);

	CString strTmp;
	strTmp.Format(_T(" Connect Redis Host:[%s],Port:[%d],password:[%s],timeout:[%d]."), strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, strCachePASS.GetBuffer(), nTimeOut);
	//CTraceService::TraceString(strTmp, TraceLevel_Normal);

#ifdef UNICODE
	CTraceService::TraceString(CYSCharUtils::ANSIToUnicode(strTmp), TraceLevel_Exception);
	TRACE(CYSCharUtils::ANSIToUnicode(strTmp));
#else
	CTraceService::TraceString(strTmp, TraceLevel_Normal);
	TRACE(strTmp);
#endif // UNICODE

	bool bRet = mRedis.connect();
	if (!bRet){
		strTmp.Format(_T(" Failed Connect Redis Host:[%s],Port:[%d],password:[%s],timeout:[%d]."), strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, strCachePASS.GetBuffer(), nTimeOut);
		CTraceService::TraceString(strTmp, TraceLevel_Exception);
		return false;
	}

	//���ó�����;
	bRet = mRedis.enableKeepAlive();
	if (!bRet){
		strTmp.Format(_T(" Failed EnableKeepAlive Redis Host:[%s],Port:[%d],password:[%s],timeout:[%d]."), strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, strCachePASS.GetBuffer(), nTimeOut);
		CTraceService::TraceString(strTmp, TraceLevel_Exception);
		return false;
	}

	strTmp.Format(_T(" Success Connect Redis Host:[%s],Port:[%d],password:[%s],timeout:[%d]."), strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, strCachePASS.GetBuffer(), nTimeOut);
	CTraceService::TraceString(strTmp, TraceLevel_Normal);

	////��ȡ��ʼ����  ��ȡ·��
	//TCHAR szWorkDir[MAX_PATH] = TEXT("");
	//CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	////����·��
	//TCHAR szIniFile[MAX_PATH] = TEXT("");
	//_sntprintf_s(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	////��ȡ����
	//CWHIniData IniData;
	//IniData.SetIniFilePath(szIniFile);

	//CString strSubItem;
	//strSubItem.Format(_T("Server%dInitScore"), m_pGameServiceOption->wServerID);

	//��ȡ����
	//m_nInitScore = IniData.ReadInt(TEXT("ServerInfo"), strSubItem, 95);

	return true;
}
void PriaveteGame::OnStartService()
{
	//��������
	DBR_GR_Private_Info kPrivateInfo;
	ZeroMemory(&kPrivateInfo,sizeof(kPrivateInfo));
	kPrivateInfo.wKindID=m_pGameServiceOption->wKindID;
	m_pIDataBaseEngine->PostDataBaseRequest(0L,DBR_GR_PRIVATE_INFO,0L,&kPrivateInfo,sizeof(kPrivateInfo));

	m_pITimerEngine->SetTimer(IDI_DISMISS_WAITE_END,5000L,TIMES_INFINITY,0);
}

//ʱ���¼�
bool PriaveteGame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	
	switch(dwTimerID)
	{
	case IDI_DISMISS_WAITE_END:				//��ɢ�ȴ�ʱ�� 10s
		{
			for(int i = 0;i<m_pGameServiceOption->wTableCount;i++)
			{
				PrivateTableInfo* pTableInfo = &m_pTableInfo[i];
				if (pTableInfo->bInEnd)
				{
					pTableInfo->fAgainPastTime += 5.0f;
					if (pTableInfo->fAgainPastTime >= AGAIN_WAITE_TIME)
					{
						ClearRoom(pTableInfo);
					}
				}

				//�Ѿ���������û�п�ʼ������
				if (pTableInfo->dwCreaterUserID>0 && !pTableInfo->bStart)
				{
					pTableInfo->fCreateTime += 5.0f;
					if (pTableInfo->fCreateTime >= CREATE_PRIVATE_FREE_TIME)
					{
						DismissRoom(pTableInfo);
					}
				}

				if (pTableInfo->kDismissChairID.size())
				{
					pTableInfo->fDismissPastTime += 5.0f;
					if (pTableInfo->fDismissPastTime >= DISMISS_WAITE_TIME)
					{
						if (pTableInfo->kNotAgreeChairID.size() <= 1)
						{
							DismissRoom(pTableInfo);
						}
					}
				}
			}

			mRedis.ping();

			return true;
		}
	}
	
	return true;
}

//��������
bool PriaveteGame::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	if(pIServerUserItem!=NULL)
		return m_pIGameServiceFrame->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	else
		return m_pIGameServiceFrame->SendData(BG_ALL_CLIENT, wMainCmdID,wSubCmdID, pData, wDataSize);

	return true;
}
bool PriaveteGame::SendTableData(ITableFrame*	pITableFrame, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return pITableFrame->SendTableData(INVALID_CHAIR,wSubCmdID,pData,wDataSize,wMainCmdID);
}
void PriaveteGame::CreatePrivateCost(PrivateTableInfo* pTableInfo)
{
	if (pTableInfo->cbRoomType == Type_Private)
	{
		CString strDebugInfo;
		strDebugInfo.Format(_T("RoomID[%d], dwUserID[%d], dwCost[%d]"), pTableInfo->dwRoomNum, pTableInfo->dwCreaterUserID, pTableInfo->dwPlayCost);
		::OutputDebugString(strDebugInfo.GetBuffer());

		if (pTableInfo->IsCardAARule())
		{
			EveryOnePrivateCost(pTableInfo, true);
		}
		else
		{
			CreaterPlayerPrivateCost(pTableInfo);
		}
	}
	if (pTableInfo->cbRoomType == Type_Public)
	{
		EveryOnePrivateCost(pTableInfo, false);
	}

}

PrivateTableInfo* PriaveteGame::getTableInfoByRoomID(DWORD dwRoomID)
{
	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].dwRoomNum == dwRoomID)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}
PrivateTableInfo* PriaveteGame::getTableInfoByCreaterID(DWORD dwUserID)
{
	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].dwCreaterUserID == dwUserID)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}
PrivateTableInfo* PriaveteGame::getTableInfoByTableID(WORD wTableID)
{

	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].pITableFrame && m_pTableInfo[i].pITableFrame->GetTableID() == wTableID)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}
PrivateTableInfo* PriaveteGame::getTableInfoByTableFrame(ITableFrame* pTableFrame)
{
	for (int i = 0;i<m_pGameServiceOption->wTableCount;i++)
	{
		if (m_pTableInfo[i].pITableFrame == pTableFrame)
		{
			return &m_pTableInfo[i];
		}
	}
	return NULL;
}

//���ݿ��¼�
bool PriaveteGame::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_PRIVATE_INFO:		//˽�˳���Ϣ
		{
			//����Ч��
			if(wDataSize>sizeof(DBO_GR_Private_Info)) return false;

			//��ȡ����
			DBO_GR_Private_Info * pPrivate = (DBO_GR_Private_Info*)pData;
			if ( pPrivate->cbDBExecSuccess == DB_SUCCESS )
			{
				m_kPrivateInfo.wKindID = pPrivate->wKindID;
				m_kPrivateInfo.lCostGold = pPrivate->lCostGold;
				memcpy(&m_kPrivateInfo.bPlayCout, pPrivate->bPlayCout, sizeof(m_kPrivateInfo.bPlayCout));
				memcpy(&m_kPrivateInfo.lPlayCost, pPrivate->lPlayCost, sizeof(m_kPrivateInfo.lPlayCost));
			}
			else
			{
				CString strTmp;
				strTmp.Format(_T("fkplatormdb���ݿ���PrivateInfo��:KindID:[%d]����Ϊ��."), pPrivate->wKindID);
				CTraceService::TraceString(strTmp, TraceLevel_Exception);
			}

			break;
		}
	case DBO_GR_CREATE_PRIVATE:		//˽�˳���Ϣ
		{
			OnEventCreatePrivate(wRequestID,pIServerUserItem,pData,wDataSize,"");
			break;
		}
	case DBO_GR_SINGLE_RECORD:
		{
			//����Ч��;
			if (wDataSize != sizeof(DBO_GR_SingleGameRecord)) return false;

			DBO_GR_SingleGameRecord* pSingleGameRecord = (DBO_GR_SingleGameRecord*)pData;
			PrivateTableInfo* pTableInfo = getTableInfoByCreaterID(pSingleGameRecord->dwCreaterUserID);
			if ( NULL != pTableInfo )
			{
				pTableInfo->kTotalRecord.iRecordID = pSingleGameRecord->dwRecordID;
			}
			break;
		}
	break;
	}
	return true;
}

bool PriaveteGame::joinPrivateRoom(IServerUserItem * pIServerUserItem,ITableFrame * pICurrTableFrame)
{
	WORD wGaveInChairID = INVALID_CHAIR;
	for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
	{
		if (pICurrTableFrame->GetTableUserItem(i)==pIServerUserItem)
		{
			wGaveInChairID = i;
			break;
		}
	}
	if (wGaveInChairID!=INVALID_CHAIR)
	{
		pIServerUserItem->SetUserStatus(US_READY,pICurrTableFrame->GetTableID(),wGaveInChairID);
		return true;
	}
	WORD wChairID = INVALID_CHAIR;
	//��������
	for (WORD i=0;i<pICurrTableFrame->GetChairCount();i++)
	{
		if (pICurrTableFrame->GetTableUserItem(i)==NULL)
		{
			wChairID = i;
			break;
		}
	}

	//�����û�
	if (wChairID!=INVALID_CHAIR)
	{
		//�û�����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			return pIServerUserItem->GetTableID() == pICurrTableFrame->GetTableID();
		}

		//�û�����
		if(pICurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
		{
			return false;
		}
		pIServerUserItem->SetUserStatus(US_READY,pICurrTableFrame->GetTableID(),wChairID);
		return true;
	}	
	return false;
}
//��������
bool PriaveteGame::OnEventCreatePrivate(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,std::string kChannel)
{
	//����Ч��
	if(pIServerUserItem==NULL) return true;
	if (wDataSize>sizeof(DBO_GR_CreatePrivateResult)) return false;

	//��ȡ����;
	DBO_GR_CreatePrivateResult* pPrivate = (DBO_GR_CreatePrivateResult*)pData;
	WORD wAgaginTable = pPrivate->wAgaginTable;
	//����ʧ��
	if(pPrivate->bSucess==false)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,pPrivate->szDescribeString,SMT_EJECT);
		CTraceService::TraceString(pPrivate->szDescribeString, TraceLevel_Normal);
		
		return true;
	}

	if (pPrivate->bPlayCoutIdex >= 4)
	{
		return false;
	}

	//Ѱ��λ��
	ITableFrame * pICurrTableFrame=NULL;
	PrivateTableInfo* pCurrTableInfo=NULL;
	if (wAgaginTable != INVALID_TABLE)
	{
		pCurrTableInfo = getTableInfoByTableID(wAgaginTable);
		if (!pCurrTableInfo)
		{
			return false;
		}
		pICurrTableFrame = pCurrTableInfo->pITableFrame;
		if (pCurrTableInfo->bInEnd == false)
		{
			joinPrivateRoom(pIServerUserItem,pICurrTableFrame);
			return true;
		}
		else
		{
			pCurrTableInfo->restAgainValue();
			sendPrivateRoomInfo(NULL,pCurrTableInfo);
		}
	}
	else
	{
		WORD wTableID = 0;
		for (wTableID=0;wTableID<m_pGameServiceOption->wTableCount;wTableID++)
		{
			//��ȡ����
			ASSERT(m_pTableInfo[wTableID].pITableFrame!=NULL);
			ITableFrame * pITableFrame=m_pTableInfo[wTableID].pITableFrame;
			if(m_pTableInfo[wTableID].bInEnd)
			{
				continue;
			}
			//״̬�ж�
			if (pITableFrame->GetNullChairCount()==pITableFrame->GetChairCount())
			{
				pICurrTableFrame = pITableFrame;
				pCurrTableInfo = &m_pTableInfo[wTableID];
				pCurrTableInfo->restValue();
				break;
			}
		}
		if (getTableInfoByCreaterID(pIServerUserItem->GetUserID()))
		{
			return true;
		}
	}
	//�����ж�
	if(pICurrTableFrame==NULL)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("������������������ʧ�ܡ�"),SMT_EJECT);
		return true;
	}


	if(!joinPrivateRoom(pIServerUserItem,pICurrTableFrame))
	{
		//m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("���뷿��ʧ�ܡ�"),SMT_EJECT);
		return true;
	}

	if (m_pGameServiceOption->wServerID >= 90)
	{
		ASSERT(false);
		return true;
	}
	int iRandNum = m_allocationID.PopValue();
	iRandNum = (m_pGameServiceOption->wServerID+10)*10000+iRandNum;
	pCurrTableInfo->dwPlayCout = (DWORD)m_kPrivateInfo.bPlayCout[pPrivate->bPlayCoutIdex];
	if (pPrivate->cbRoomType == Type_Private)
	{
		pCurrTableInfo->dwPlayCost = (DWORD)m_kPrivateInfo.lPlayCost[pPrivate->bPlayCoutIdex];
	}
	else
	{
		pCurrTableInfo->dwPlayCost = (DWORD)m_kPrivateInfo.lCostGold;
	}
	pCurrTableInfo->setRoomNum(iRandNum);
	pCurrTableInfo->dwCreaterUserID = pIServerUserItem->GetUserID();
	pCurrTableInfo->kHttpChannel = kChannel;
	pCurrTableInfo->cbRoomType = pPrivate->cbRoomType;

	pCurrTableInfo->dwGameRuleIdex = pPrivate->bGameRuleIdex;
	pCurrTableInfo->bGameTypeIdex = pPrivate->bGameTypeIdex;
	pCurrTableInfo->bPlayCoutIdex = pPrivate->bPlayCoutIdex;
	GetLocalTime(&pCurrTableInfo->kTotalRecord.kPlayTime);

	pCurrTableInfo->cbEnterRoomScoreType	= pPrivate->cbEnterRoomScoreType;
	pCurrTableInfo->lInitEnterRoomScore		= pPrivate->lInitEnterRoomScore;

	if (pPrivate->wAgaginTable == INVALID_TABLE)
	{
		pCurrTableInfo->kTotalRecord.iRecordID = pPrivate->dwRecordID;
	}

	tagPrivateFrameParameter PriavateFrame;
	PriavateFrame.cbGameTypeIdex = (BYTE)pCurrTableInfo->dwGameRuleIdex;
	PriavateFrame.cbGameCout = (BYTE)pCurrTableInfo->dwPlayCout;
	PriavateFrame.dwGameRule = pCurrTableInfo->dwGameRuleIdex;

	pICurrTableFrame->SetPrivateInfo(PriavateFrame);

	CMD_GF_Create_Private_Sucess kSucessInfo;
	kSucessInfo.lCurSocre = pPrivate->bSucess;
	kSucessInfo.dwRoomNum = pCurrTableInfo->dwRoomNum;
	SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GR_CREATE_PRIVATE_SUCESS,&kSucessInfo,sizeof(kSucessInfo));

	CString strTmp;
	strTmp.Format(_T("UserID:[%d] ��������: %d, %d"), pIServerUserItem->GetUserID(), iRandNum, pPrivate->bPlayCoutIdex);
	CTraceService::TraceString(strTmp, TraceLevel_Normal);

	sendPrivateRoomInfo(pIServerUserItem, pCurrTableInfo);

	if (!AddPRMRedisKey(iRandNum))
	{
		strTmp.Format(_T(" REDIS CreateRoom:%d, set failed."), iRandNum);
		CTraceService::TraceString(strTmp, TraceLevel_Warning);
	}

	if (!AddRoomIDToIDRedisKey(pIServerUserItem->GetUserID(), iRandNum))
	{
		strTmp.Format(_T(" REDIS id.%d: add roomid: [%d] failed."), pIServerUserItem->GetUserID(), iRandNum);
		CTraceService::TraceString(strTmp, TraceLevel_Warning);
	}

	// ���÷�������;
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (NULL == pTableInfo) return true;
	if (pTableInfo->bInEnd) return true;

	if (pTableInfo->IsSetOneTimeRoomScoreType())
	{	//���뷿��ʱ��ֵΪ��ʼ����;
		SCORE lScore = pTableInfo->lInitEnterRoomScore - pIServerUserItem->GetUserScore();
		pIServerUserItem->WriteUserScore(lScore, 0, 0, 0, SCORE_TYPE_PRESENT, 0, 0);
	}

	return true;
}

	//����˽�˳�
bool PriaveteGame::OnTCPNetworkSubCreatePrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	// �жϷ�����ά��;
	if (CServerRule::IsForfendGameStart(m_pGameServiceOption->dwServerRule) == true)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("����������ά�������ע΢��Ⱥ֪ͨ��"), SMT_EJECT | SMT_CHAT | SMT_CLOSE_LINK);
		return true;
	}
	//����Ч��;
	ASSERT(wDataSize == 0);
	if (wDataSize != 0) return false;

	if (!mRedis.isConneced() && !mRedis.connect())
	{
		return false;
	}

	DWORD dwUserID = pIServerUserItem->GetUserID();
	char szBuf[32] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf) - 1, "CR.%d", dwUserID);

	
	ValueMap valueMap;
	bool bRet = mRedis.hgetall(szBuf, valueMap);
	if (!bRet || valueMap.empty())
	{
		return false;
	}

	DBO_GR_CreatePrivateResult CreatePrivateResult;
	CreatePrivateResult.bSucess = TRUE;
	CreatePrivateResult.cbRoomType = Type_Private;
	CreatePrivateResult.wAgaginTable = INVALID_TABLE;
	CreatePrivateResult.lCurSocre = 0;

	CreatePrivateResult.bPlayCoutIdex = CYSService::ToByte(valueMap["cbPCI"]);
	CreatePrivateResult.bGameTypeIdex = CYSService::ToByte(valueMap["cbGTI"]);
	CreatePrivateResult.bGameRuleIdex = CYSService::ToDword(valueMap["dwGRI"]);

	CreatePrivateResult.cbEnterRoomScoreType= CYSService::ToByte(valueMap["cbERST"]);
	CreatePrivateResult.lInitEnterRoomScore = CYSService::ToLong(valueMap["lIERS"]);
	CreatePrivateResult.dwRecordID			= CYSService::ToLong(valueMap["recordID"]);

	//ɾ�����������REDIS key :CR.UserID.
	DelRedisKey(szBuf);

	return OnEventCreatePrivate(0, pIServerUserItem, &CreatePrivateResult, sizeof(CreatePrivateResult), "");
}
//����˽�˳�
bool PriaveteGame::OnTCPNetworkSubJoinPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//����Ч��
	ASSERT(wDataSize==sizeof(CMD_GR_Join_Private));
	if(wDataSize!=sizeof(CMD_GR_Join_Private)) return false;

	// �жϷ�����ά��;
	if (CServerRule::IsForfendGameStart(m_pGameServiceOption->dwServerRule) == true)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("����������ά�������ע΢��Ⱥ֪ͨ��"), SMT_EJECT | SMT_CHAT | SMT_CLOSE_LINK);
		return true;
	}

	bool bRet = true;
	CString strMessage;

	CMD_GR_Join_Private* pCMDInfo = (CMD_GR_Join_Private*)pData;
	PrivateTableInfo* pTableInfo = getTableInfoByRoomID(pCMDInfo->dwRoomNum);
	do 
	{
		if (!pTableInfo)
		{
			bRet = false;
			strMessage = TEXT("�Բ���û���ҵ��÷��䣬���ܷ����Ѿ��˳���");
			break;
		}
		if (pTableInfo->pITableFrame->GetNullChairCount() <= 0)
		{
			bRet = false;
			strMessage = TEXT("�Բ�����Ϸ�����������޷����롣");
			break;
		}
		if (pTableInfo->bStart || pTableInfo->bInEnd)
		{
			bRet = false;
			strMessage = TEXT("�Բ�����Ϸ�Ѿ���ʼ���޷����롣");
			break;
		}
		if (!pTableInfo->IsCanJoinTable(pIServerUserItem, strMessage))
		{
			bRet = false;
			break;
		}
	} while (0);

	if (!bRet)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, strMessage, SMT_EJECT | SMT_CHAT | SMT_CLOSE_LINK);
		return true;
	}

	if (!joinPrivateRoom(pIServerUserItem,pTableInfo->pITableFrame))
	{
		//m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem, TEXT("���뷿��ʧ�ܡ�"), SMT_EJECT | SMT_CHAT | SMT_CLOSE_LINK);
	}

	//����˽�˳���Ϣ(//add by fxd);
	sendPrivateRoomInfo(pIServerUserItem, pTableInfo);

	AddRoomIDToIDRedisKey(pIServerUserItem->GetUserID(), pCMDInfo->dwRoomNum);

	return true;
}
bool PriaveteGame::OnTCPNetworkSubDismissPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	//CTraceService::TraceString(TEXT("SUB_GR_PRIVATE_DISMISS"),TraceLevel_Normal);

	//����Ч��
	ASSERT(wDataSize==sizeof(CMD_GR_Dismiss_Private));
	if(wDataSize!=sizeof(CMD_GR_Dismiss_Private)) return false;

	CMD_GR_Dismiss_Private* pCMDInfo = (CMD_GR_Dismiss_Private*)pData;

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return false;
	}
	if (!pTableInfo->bStart && pTableInfo->dwCreaterUserID != pIServerUserItem->GetUserID())
	{
		return true;
	}
	if (pTableInfo->bInEnd)
	{
		return true;
	}

	if (pTableInfo->kDismissChairID.size() == 0 && !pCMDInfo->bDismiss)
	{
		return true;
	}

	if(pCMDInfo->bDismiss)
	{
		for (int i = 0;i<(int)pTableInfo->kDismissChairID.size();i++)
		{
			if (pTableInfo->kDismissChairID[i] == pIServerUserItem->GetChairID())
			{
				return true;
			}
		}
		pTableInfo->kDismissChairID.push_back(pIServerUserItem->GetChairID());
	}
	else
	{
		for (int i = 0;i<(int)pTableInfo->kNotAgreeChairID.size();i++)
		{
			if (pTableInfo->kNotAgreeChairID[i] == pIServerUserItem->GetChairID())
			{
				return true;
			}
		}
		pTableInfo->kNotAgreeChairID.push_back(pIServerUserItem->GetChairID());
	}
	CMD_GF_Private_Dismiss_Info kNetInfo;
	kNetInfo.dwDissUserCout = pTableInfo->kDismissChairID.size();
	kNetInfo.dwNotAgreeUserCout = pTableInfo->kNotAgreeChairID.size();
	for (int i = 0;i<(int)pTableInfo->kDismissChairID.size();i++)
	{
		kNetInfo.dwDissChairID[i] = pTableInfo->kDismissChairID[i];
	}
	for (int i = 0;i<(int)pTableInfo->kNotAgreeChairID.size();i++)
	{
		kNetInfo.dwNotAgreeChairID[i] = pTableInfo->kNotAgreeChairID[i];
	}
	SendTableData(pTableInfo->pITableFrame,MDM_GR_PRIVATE,SUB_GR_PRIVATE_DISMISS,&kNetInfo,sizeof(kNetInfo));

	if (pTableInfo->kNotAgreeChairID.size() >= 2)
	{
		pTableInfo->kNotAgreeChairID.clear();
		pTableInfo->kDismissChairID.clear();
		pTableInfo->fDismissPastTime = 0;

		CMD_GF_Private_Dismiss_Result mResult;
		mResult.bSuccess = false;
		SendTableData(pTableInfo->pITableFrame, MDM_GR_PRIVATE, SUB_GR_PRIVATE_DISMISS_RESULT, &mResult, sizeof(mResult));
	}

	if (!pTableInfo->bStart || (int)kNetInfo.dwDissUserCout >= (int)pTableInfo->pITableFrame->GetSitUserCount()-1)
	{
		CMD_GF_Private_Dismiss_Result mResult;
		mResult.bSuccess = true;
		SendTableData(pTableInfo->pITableFrame, MDM_GR_PRIVATE, SUB_GR_PRIVATE_DISMISS_RESULT, &mResult, sizeof(mResult));

		pTableInfo->pITableFrame->SendGameMessage(TEXT("�����ѱ���ɢ��"),SMT_EJECT);

		DismissRoom(pTableInfo);

		ClearRoom(pTableInfo);
	}


	return true;
}
//�����¼�
bool PriaveteGame::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_PRIVATE_INFO:	//˽�˳���Ϣ
		{
			SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GR_PRIVATE_INFO,&m_kPrivateInfo,sizeof(m_kPrivateInfo));
			return true;
		}
	case SUB_GR_CREATE_PRIVATE:	//����˽�˳�
		{
			return OnTCPNetworkSubCreatePrivate(pData,wDataSize,pIServerUserItem,dwSocketID);
		}
	case SUB_GR_JOIN_PRIVATE:	//����˽�˳�
		{
			return OnTCPNetworkSubJoinPrivate(pData,wDataSize,pIServerUserItem,dwSocketID);	
		}
	case SUB_GR_PRIVATE_DISMISS:	//��ɢ
		{
			return OnTCPNetworkSubDismissPrivate(pData,wDataSize,pIServerUserItem,dwSocketID);	
		}
	}
	return true;
}

//������λ
void PriaveteGame::DismissRoom(PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo!=NULL);
	if (pTableInfo==NULL) return;

	ITableFrame* pTableFrame=pTableInfo->pITableFrame;
	ASSERT(pTableFrame!=NULL);
	if (pTableFrame==NULL) return;

	//ǿ�ƽ�ɢ��Ϸ
	if (pTableFrame->IsGameStarted()==true)
	{
		bool bSuccess=pTableFrame->DismissGame();
		if (bSuccess==false)
		{
			CTraceService::TraceString(TEXT("PriaveteGame ��ɢ��Ϸ ʧ�ܣ�"),TraceLevel_Exception);
			return;
		}
	}

	//ɾ��REDIS�е�����;
	DelRoomIDFromIDRedisKey(pTableInfo);
	DelPRMRedisKey(pTableInfo->dwRoomNum);

	DWORD dwRoomNum = pTableInfo->dwRoomNum;

	if (pTableInfo->bStart)
	{
		SendGamePrivateEndInfo(pTableInfo);

		pTableInfo->bInEnd = true;
		pTableInfo->dwCreaterUserID = 0;

		//���ÿ�ֵļ�¼ֻ�����ݿ��¼�ܵļ�¼;
		pTableInfo->kTotalRecord.kRecordChild.clear();

		DataStream kDataStream;
		pTableInfo->kTotalRecord.StreamValue(kDataStream,true);

		m_pIDataBaseEngine->PostDataBaseRequest(INVALID_DWORD,DBR_GR_PRIVATE_GAME_RECORD,0,&kDataStream[0],kDataStream.size());	

		pTableInfo->bStart = false;
		sendPrivateRoomInfo(NULL,pTableInfo);

		gameEnd(pTableInfo);
	}
	else
	{
		ClearRoom(pTableInfo);
	}

	m_allocationID.PushValue(dwRoomNum % 10000);

	CString strTmp;
	strTmp.Format(_T("��ɢ����: %d"), dwRoomNum);
	CTraceService::TraceString(strTmp, TraceLevel_Normal);

	return;
}
void PriaveteGame::ClearRoom(PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo!=NULL);
	if (pTableInfo==NULL) return;

	ITableFrame* pTableFrame=pTableInfo->pITableFrame;
	ASSERT(pTableFrame!=NULL);
	if (pTableFrame==NULL) return;

	//�����û�ȫ���뿪
	for (int i=0;i<pTableFrame->GetChairCount();i++)
	{
		IServerUserItem* pUserItem=pTableFrame->GetTableUserItem(i);
		if(pUserItem&&pUserItem->GetTableID()!=INVALID_TABLE)
		{
			//pTableFrame->PerformStandUpActionEx(pUserItem);		//�ȸ�ע�͵�;
			pTableFrame->PerformStandUpAction(pUserItem);			//add by fxd(�����);
		}
	}
	pTableInfo->restValue();
}

//����˽�˳�������Ϣ,������Ϣ����ÿ����Ϸ�ڲ�
void PriaveteGame::SendGamePrivateEndInfo(PrivateTableInfo* pTableInfo)
{
	ITableFrame* pTableFrame = pTableInfo->pITableFrame;
	ASSERT(pTableFrame != NULL);
	if (pTableFrame == NULL) return;

	DataStream kDataStream;
	pTableFrame->GetPrivateEndInfo(kDataStream, true);
	SendTableData(pTableFrame, MDM_GR_PRIVATE, SUB_GF_GM_PRIVATE_END, &kDataStream[0], kDataStream.size());
}

//�û�����
bool PriaveteGame::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	m_pIServerUserItemSink->OnEventUserItemScore(pIServerUserItem,cbReason);
	return true;
}

//�û�״̬
bool PriaveteGame::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemStatus(pIServerUserItem,wOldTableID,wOldChairID);
	}

	return true;
}

//�û�Ȩ��
bool PriaveteGame::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	if(m_pIServerUserItemSink!=NULL)
	{
		return m_pIServerUserItemSink->OnEventUserItemRight(pIServerUserItem, dwAddRight, dwRemoveRight, cbRightKind);
	}

	return true;
}

//�û���¼
bool PriaveteGame::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	return true;
}

//�û��ǳ�
bool PriaveteGame::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	return true;
}

//�����¼�
bool PriaveteGame::OnEventEnterMatch(DWORD dwSocketID, VOID* pData, DWORD dwUserIP, bool bIsMobile)
{
	//�ֻ��û�
	if(bIsMobile == true)
	{
		//������Ϣ
		CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
		pLogonMobile->szPassword[CountArray(pLogonMobile->szPassword)-1]=0;
		pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0;

		//��������
		DBR_GR_LogonMobile LogonMobile;
		ZeroMemory(&LogonMobile,sizeof(LogonMobile));

		//��������
		LogonMobile.dwUserID=pLogonMobile->dwUserID;
		LogonMobile.dwClientAddr=dwUserIP;		
		LogonMobile.cbDeviceType=pLogonMobile->cbDeviceType;
		LogonMobile.wBehaviorFlags=pLogonMobile->wBehaviorFlags;
		LogonMobile.wPageTableCount=pLogonMobile->wPageTableCount;		
		lstrcpyn(LogonMobile.szPassword,pLogonMobile->szPassword,CountArray(LogonMobile.szPassword));
		lstrcpyn(LogonMobile.szMachineID,pLogonMobile->szMachineID,CountArray(LogonMobile.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonMobile.dwUserID,DBR_GR_LOGON_MOBILE,dwSocketID,&LogonMobile,sizeof(LogonMobile));		
	}
	else
	{
		//������Ϣ
		CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
		pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
		pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

		//��������
		DBR_GR_LogonUserID LogonUserID;
		ZeroMemory(&LogonUserID,sizeof(LogonUserID));

		//��������
		LogonUserID.dwClientAddr=dwUserIP;
		LogonUserID.dwUserID=pLogonUserID->dwUserID;
		lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
		lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));
	}
	return true;
}
bool PriaveteGame::AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex)
{
	//PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pTbableFrame);
	//ASSERT(pTableInfo);
	//if (!pTableInfo)
	//{
	//	return true;
	//}
	//if (dwChairID >= 100 || bActionIdex >= MAX_PRIVATE_ACTION)
	//{
	//	return true;
	//}
	//pTableInfo->lPlayerAction[dwChairID][bActionIdex] ++;
	return true;
}
bool PriaveteGame::addPrivatePlayCout(ITableFrame* pTbableFrame, WORD wCout)
{
	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pTbableFrame);
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return true;
	}

	pTableInfo->dwFinishPlayCout += wCout;
	return true;
}
//�û�
bool PriaveteGame::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, DWORD dwSocketID)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GR_PRIVATE_INFO,&m_kPrivateInfo,sizeof(m_kPrivateInfo));

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	//�ж�״̬
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		sendPrivateRoomInfo(pIServerUserItem,getTableInfoByTableID(pIServerUserItem->GetTableID()));
	}
	return true;
}

//�û�����
bool PriaveteGame::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	return true;
}

//��Ϸ��ʼ
bool PriaveteGame::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pITableFrame);
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return true;
	}
	pTableInfo->dwStartPlayCout ++;
	pTableInfo->bStart = true;
	pTableInfo->newRandChild();

	sendPrivateRoomInfo(NULL,pTableInfo);

	// ��ʾ��Ϸ��ʼ;
	CString strDebugInfo = _T("");
	strDebugInfo.Format("EventGameStart RoomID = %d, TableID = %d", pTableInfo->dwRoomNum, pITableFrame->GetTableID());
	::OutputDebugString(strDebugInfo.GetBuffer());

	return true;
}

//��Ϸ����
bool PriaveteGame::OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	ASSERT(pITableFrame!=NULL);
	if (pITableFrame==NULL) return false;

	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pITableFrame);
	if (!pTableInfo)
	{
		return true;
	}

	if (pTableInfo->cbRoomType == Type_Private)
	{
		// ��ʾ��Ϸ��ʼ;
		CString strDebugInfo = _T("");
		strDebugInfo.Format("EventGameEnd RoomID = %d, TableID = %d", pTableInfo->dwRoomNum, pITableFrame->GetTableID());
		::OutputDebugString(strDebugInfo.GetBuffer());

		// һ��û������ǰ��ɢ���۷���;
		if ((cbReason != GER_DISMISS) && (pTableInfo->dwStartPlayCout == 1))
		{
			CreatePrivateCost(pTableInfo);
		}

		if (pITableFrame->IsPrivateEnd())
		{
			DismissRoom(pTableInfo);
			return true;
		}

		sendPrivateRoomInfo(NULL, pTableInfo);
	}
	else if (pTableInfo->cbRoomType == Type_Public)
	{
		CreatePrivateCost(pTableInfo);
		DismissRoom(pTableInfo);
		ClearRoom(pTableInfo);
	}
	return true;
}
bool PriaveteGame::WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount,DataStream& kData)
{
	PrivateTableInfo* pTableInfo = getTableInfoByTableFrame(pITableFrame);
	if (!pTableInfo)
	{
		return true;
	}

	// ����˽�˳���Ϣ;
	tagGameRecord gameRecord;
	gameRecord.StreamValue(kData, false);
	gameRecord.roomInfo.cbRoomType = pTableInfo->cbRoomType;
	gameRecord.roomInfo.bStartGame = pTableInfo->bStart;
	gameRecord.roomInfo.dwRoomNum = pTableInfo->dwRoomNum;
	gameRecord.roomInfo.dwPlayCout = pTableInfo->dwFinishPlayCout - 1;		// ��ȥ���ֵľ���;
	gameRecord.roomInfo.dwCreateUserID = pTableInfo->dwCreaterUserID;
	gameRecord.roomInfo.bGameRuleIdex = pTableInfo->dwGameRuleIdex;
	gameRecord.roomInfo.bGameTypeIdex = pTableInfo->bGameTypeIdex;
	gameRecord.roomInfo.bPlayCoutIdex = pTableInfo->bPlayCoutIdex;
	gameRecord.roomInfo.dwPlayTotal = pTableInfo->dwPlayCout;

	DataStream tmpGameRecord;
	gameRecord.StreamValue(tmpGameRecord, true);
	pTableInfo->writeSocre(ScoreInfoArray, wScoreCount, tmpGameRecord);

	//д�뵥һһ�����ݵ����ݿ�;
	std::vector<tagPrivateRandRecordChild>& refkRecordChild = pTableInfo->kTotalRecord.kRecordChild;
	if (refkRecordChild.size() > 0)
	{
		tagPrivateRandRecordChild& refkSingleRecord = refkRecordChild.back();
		refkSingleRecord.iRecordID = pTableInfo->kTotalRecord.iRecordID;

		DataStream singleGameRecordData;
		refkSingleRecord.StreamValue(singleGameRecordData, true);

		m_pIDataBaseEngine->PostDataBaseRequest(INVALID_DWORD, DBR_GR_SINGLE_PRIVATE_RECORD, 0L, (BYTE*)&singleGameRecordData[0], singleGameRecordData.size());
	}

	return true;
}
void PriaveteGame::sendPrivateRoomInfo(IServerUserItem * pIServerUserItem,PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return;
	}

	CMD_GF_Private_Room_Info kNetInfo;
	kNetInfo.cbRoomType = pTableInfo->cbRoomType;
	kNetInfo.bStartGame = pTableInfo->bStart;
	kNetInfo.dwRoomNum = pTableInfo->dwRoomNum;
	//kNetInfo.dwPlayCout = pTableInfo->dwStartPlayCout;
	kNetInfo.dwPlayCout = pTableInfo->dwFinishPlayCout;
	kNetInfo.dwCreateUserID = pTableInfo->dwCreaterUserID;
	kNetInfo.bGameRuleIdex = pTableInfo->dwGameRuleIdex;
	kNetInfo.bGameTypeIdex = pTableInfo->bGameTypeIdex;
	kNetInfo.bPlayCoutIdex = pTableInfo->bPlayCoutIdex;
	kNetInfo.dwPlayTotal = pTableInfo->dwPlayCout;
	kNetInfo.kWinLoseScore.resize(pTableInfo->pITableFrame->GetChairCount());
	for (WORD i = 0;i<pTableInfo->pITableFrame->GetChairCount();i++)
	{
		kNetInfo.kWinLoseScore[i] =  (int)pTableInfo->lPlayerWinLose[i];
	}

	DataStream kDataStream;
	kNetInfo.StreamValue(kDataStream,true);
	
	if (pIServerUserItem)
	{
		SendData(pIServerUserItem,MDM_GR_PRIVATE,SUB_GF_PRIVATE_ROOM_INFO,&kDataStream[0],kDataStream.size());
	}
	else
	{
		SendTableData(pTableInfo->pITableFrame,MDM_GR_PRIVATE,SUB_GF_PRIVATE_ROOM_INFO,&kDataStream[0],kDataStream.size());
	}
}
//����
bool PriaveteGame::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return false;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return false;
	}
	if (pTableInfo->dwCreaterUserID == pIServerUserItem->GetUserID() && !pTableInfo->bInEnd)
	{
		return true;
	}
	if (pTableInfo->bInEnd)
	{
		//pTableInfo->pITableFrame->PerformStandUpActionEx(pIServerUserItem);  //�ȸ�ע�͵�;
	}
	return true;
}
//�û�����
bool PriaveteGame::OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{ 
	ASSERT(pIServerUserItem);
	if (NULL == pIServerUserItem) return true;


	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (NULL == pTableInfo) return true;
	if (pTableInfo->bInEnd) return false;

	if (pTableInfo->IsSetOneTimeRoomScoreType())
	{	//���뷿��ʱ��ֵΪ��ʼ����;
		SCORE lScore = pTableInfo->lInitEnterRoomScore - pIServerUserItem->GetUserScore();
		pIServerUserItem->WriteUserScore(lScore, 0, 0, 0, SCORE_TYPE_PRESENT, 0, 0);
	}

	return true; 
}

//�û�����
bool PriaveteGame::OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//�Ƴ�����
	return true;
}

 //�û�ͬ��
bool PriaveteGame::OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{ 
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return true;
	}

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (!pTableInfo)
	{
		return true;
	}
	if (pTableInfo->bInEnd)
	{
		return false;
	}

	if ( pTableInfo->IsEveryTimeRoomScoreType() )
	{	
		//ÿ�����¸�ֵΪ��ʼ����;
		SCORE lScore = pTableInfo->lInitEnterRoomScore - pIServerUserItem->GetUserScore();
		pIServerUserItem->WriteUserScore(lScore, 0, 0, 0, SCORE_TYPE_PRESENT, 0, 0);
	}

	return true; 
}

//�жϷ����Ƿ�
bool PriaveteGame::OnActionUserFangKaCheck(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure)
{
	ASSERT(pIServerUserItem);
	if (NULL == pIServerUserItem) return true;

	PrivateTableInfo* pTableInfo = getTableInfoByTableID(wTableID);
	if (NULL == pTableInfo) return true;
	if (pTableInfo->bInEnd) return false;

	if (pTableInfo->IsCardAARule())
	{
		ASSERT((pTableInfo->dwPlayCost / pTableInfo->pITableFrame->GetChairCount())>0);

		// AA ������жϷ�������;
		if (lUserInsure < (pTableInfo->dwPlayCost / pTableInfo->pITableFrame->GetChairCount()))
		{
			return false;
		}
	}
	else
	{
		return (lUserInsure > pTableInfo->dwPlayCost);
	}

	return true;
}

bool PriaveteGame::OnEventReqStandUP(IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem);
	if (!pIServerUserItem)
	{
		return true;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	ASSERT(pTableInfo);
	if (!pTableInfo)
	{
		return true;
	}
	if (pTableInfo->bStart && !pTableInfo->bInEnd)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("��Ϸ�Ѿ���ʼ���޷��˳���"),SMT_EJECT);
		return true;
	}
	if (getTableInfoByCreaterID(pIServerUserItem->GetUserID()) && !pTableInfo->bInEnd)
	{
		m_pIGameServiceFrame->SendRoomMessage(pIServerUserItem,TEXT("���ѷ��ش���,���佫�����������"),SMT_CLOSE_GAME);
		return true;
	}
	//pTableInfo->pITableFrame->PerformStandUpActionEx(pIServerUserItem);		//�ȸ�ע�͵�;
	return true;
}

void PriaveteGame::CreaterPlayerPrivateCost(PrivateTableInfo* pTableInfo)
{
	DBR_GR_Create_Private_Cost kNetInfo;
	kNetInfo.dwUserID	= pTableInfo->dwCreaterUserID;
	kNetInfo.dwCost		= pTableInfo->dwPlayCost;
	kNetInfo.dwCostType = pTableInfo->cbRoomType;
	m_pIDataBaseEngine->PostDataBaseRequest(0L, DBR_GR_CREATE_PRIVAT_COST, 0L, &kNetInfo, sizeof(kNetInfo));
}

void PriaveteGame::EveryOnePrivateCost(PrivateTableInfo* pTableInfo, bool bDivideCost)
{
	if (NULL == pTableInfo) return;

	ITableFrame* pTableFrame = pTableInfo->pITableFrame;
	if ( NULL == pTableFrame) return;

	WORD nUserCount = pTableFrame->GetSitUserCount();
	DWORD dwCost	= pTableInfo->dwPlayCost;
	if (bDivideCost)
	{
		dwCost = (dwCost / nUserCount);
	}
	for (int i = 0; i < nUserCount; i++)
	{
		IServerUserItem* pServerUserItem = pTableFrame->GetTableUserItem(i);
		if (NULL == pServerUserItem)
		{
			continue;
		}
		DBR_GR_Create_Private_Cost kNetInfo;
		kNetInfo.dwUserID	= pServerUserItem->GetUserID();
		kNetInfo.dwCost		= dwCost;
		kNetInfo.dwCostType = pTableInfo->cbRoomType;
		m_pIDataBaseEngine->PostDataBaseRequest(0L, DBR_GR_CREATE_PRIVAT_COST, 0L, &kNetInfo, sizeof(kNetInfo));
	}
}

void PriaveteGame::gameEnd(PrivateTableInfo* pTableInfo)
{
	ASSERT(pTableInfo != nullptr);
	if (NULL == pTableInfo) return;

	ITableFrame* pITableFrame = pTableInfo->pITableFrame;
	ASSERT(pITableFrame != nullptr);
	if (nullptr == pITableFrame) return;

	DBO_GR_PrivateGameEnd mGameEnd;
	mGameEnd.wKindID	= m_pGameServiceOption->wKindID;
	mGameEnd.wServerID	= m_pGameServiceOption->wServerID;
	mGameEnd.wPlayCount = pTableInfo->dwFinishPlayCout;

	WORD wChairCount = pITableFrame->GetChairCount();
	for (WORD i = 0; i < wChairCount; i++)
	{
		IServerUserItem * pServerItem = pITableFrame->GetTableUserItem(i);
		ASSERT(pServerItem != nullptr);
		if (pServerItem == nullptr)
		{
			continue;
		}

		mGameEnd.dwUserID = pServerItem->GetUserID();
		mGameEnd.lScore = pServerItem->GetUserScore();
		mGameEnd.dwCardLibIndex = pServerItem->getCardLibIndex();
		m_pIDataBaseEngine->PostDataBaseRequest(INVALID_DWORD, DBR_GR_PRIVATE_GAME_END, 0, &mGameEnd, sizeof(DBO_GR_PrivateGameEnd));
	}
}

bool PriaveteGame::DelPRMRedisKey(DWORD dwRoomNum)
{
	if (!mRedis.isConneced() && !mRedis.connect())
	{
		return false;
	}

	DecrCurTableOfPrivateserveridKey();

	int64_t retval = 0;
	char szPRMKey[32] = { 0 };
	sprintf_s(szPRMKey, sizeof(szPRMKey) - 1, "prm%d", dwRoomNum);
	return mRedis.del(szPRMKey, retval);
}

bool PriaveteGame::AddPRMRedisKey(DWORD dwRoomNum)
{
	if (!mRedis.isConneced() && !mRedis.connect())
	{
		return false;
	}

	IncrCurTableOfPrivateserveridKey();

	//��REDIS��������;
	char szCRMKey[32] = { 0 };
	sprintf_s(szCRMKey, sizeof(szCRMKey) - 1, "prm%d", dwRoomNum);
	char szCRMValue[64] = { 0 };
	sprintf_s(szCRMValue, sizeof(szCRMValue) - 1, "privateserverid.%d", m_pGameServiceOption->wServerID);
	return mRedis.set(szCRMKey, szCRMValue);
}

bool PriaveteGame::AddRoomIDToIDRedisKey(DWORD dwUserID, DWORD dwRoomNum)
{
	if (!mRedis.isConneced() && !mRedis.connect())
	{
		return false;
	}

	char szBuf[32] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf) - 1, "id.%d", dwUserID);
	//string strKey(szBuf);

	int64_t ret = 0;
	ValueMap vMap;

	vMap["RoomID"] = CYSService::ToString(dwRoomNum);

	return mRedis.hmset(szBuf, vMap, ret);
}

bool PriaveteGame::DelRoomIDFromIDRedisKey(DWORD dwUserID)
{
	return AddRoomIDToIDRedisKey(dwUserID, 0);
}

bool PriaveteGame::DelRoomIDFromIDRedisKey(PrivateTableInfo* pTableInfo)
{
	if (NULL == pTableInfo)
	{
		return false;
	}
	ITableFrame* pITableFrame = pTableInfo->pITableFrame;
	if (NULL == pITableFrame)
	{ 
		return false;
	}
	WORD nChairCount = pTableInfo->getChairCout();
	for (WORD i = 0; i < nChairCount; i++)
	{
		IServerUserItem * pServerItem = pITableFrame->GetTableUserItem(i);
		if (pServerItem == NULL) continue;

		DelRoomIDFromIDRedisKey(pServerItem->GetUserID());
	}

	return true;
}

bool PriaveteGame::OnEventUserReturnMatch(ITableFrame *pITableFrame, IServerUserItem * pIServerUserItem)
{
	ASSERT(pIServerUserItem);
	if (NULL == pIServerUserItem)
	{
		return true;
	}
	PrivateTableInfo* pTableInfo = getTableInfoByTableID(pIServerUserItem->GetTableID());
	if (NULL == pTableInfo)
	{
		return true;
	}
	sendPrivateRoomInfo(pIServerUserItem, pTableInfo);


	//����ط������Ҫ��???;(fxd);
	if (pTableInfo->kDismissChairID.size())
	{
		CMD_GF_Private_Dismiss_Info kNetInfo;
		kNetInfo.dwDissUserCout = pTableInfo->kDismissChairID.size();
		kNetInfo.dwNotAgreeUserCout = pTableInfo->kNotAgreeChairID.size();
		for (int i = 0; i < (int)pTableInfo->kDismissChairID.size(); i++)
		{
			kNetInfo.dwDissChairID[i] = pTableInfo->kDismissChairID[i];
		}
		for (int i = 0; i < (int)pTableInfo->kNotAgreeChairID.size(); i++)
		{
			kNetInfo.dwNotAgreeChairID[i] = pTableInfo->kNotAgreeChairID[i];
		}
		SendTableData(pTableInfo->pITableFrame, MDM_GR_PRIVATE, SUB_GR_PRIVATE_DISMISS, &kNetInfo, sizeof(kNetInfo));
	}

	return true;
}

bool PriaveteGame::DelRedisKey(const char* szKey)
{
	if (!mRedis.isConneced() && !mRedis.connect())
	{
		return false;
	}

	int64_t retval = 0;
	return mRedis.del(szKey, retval);
}

bool PriaveteGame::IncrCurTableOfPrivateserveridKey(int nIncValue)
{
	if (!mRedis.isConneced() && !mRedis.connect())
	{
		return false;
	}

	int64_t retval = 0;
	char szPrivateServerIDKey[64] = { 0 };
	sprintf_s(szPrivateServerIDKey, sizeof(szPrivateServerIDKey) - 1, "privateserverid.%d", m_pGameServiceOption->wServerID);
	return mRedis.hincrby(szPrivateServerIDKey, "wCurTable", nIncValue, retval);
}

bool PriaveteGame::DecrCurTableOfPrivateserveridKey()
{
	return IncrCurTableOfPrivateserveridKey(-1);
}
