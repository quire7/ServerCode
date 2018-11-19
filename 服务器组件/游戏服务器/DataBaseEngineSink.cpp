#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//���ñ���
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;

	//��������
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//�ͷŶ���
	SafeRelease(m_pIGameDataBaseEngineSink);
	m_wArrDailyTaskID.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//��ȡ����
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	//Ч��״̬
	ASSERT(m_TreasureDBModule.GetInterface() != NULL);
	if (m_TreasureDBModule.GetInterface() == NULL) return NULL;

	//��ѯ����
	IDataBase * pIDataBase = m_TreasureDBModule.GetInterface();
	VOID * pIQueryObject=pIDataBase->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//��ȡ����
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//Ч��״̬
	ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//��ѯ����
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//Ͷ�ݽ��
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��ѯ����
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//��������
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//������Ϸ
	try
	{
		//������Ϣ
		tagDataBaseParameter * pTreasureDBParameter = &m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountsDBParameter = &m_pInitParameter->m_AccountsDBParameter;

		//��������
		m_AccountsDBModule->SetDBInfo(pAccountsDBParameter->szDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName, pAccountsDBParameter->szDataBaseUser, pAccountsDBParameter->szDataBasePass);

		//��������
		m_TreasureDBModule->SetDBInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);

		//��������
		m_PlatformDBModule->SetDBInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//��������
		m_AccountsDBModule->Open();

		//��������
		m_TreasureDBModule->Open();

		//��������
		m_PlatformDBModule->Open();

		//���ݹ���
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameDataBaseEngineSink=(IGameDataBaseEngineSink *)m_pIGameServiceManager->CreateGameDataBaseEngineSink(IID_IGameDataBaseEngineSink,VER_IGameDataBaseEngineSink);

		//���ö���
		if ((m_pIGameDataBaseEngineSink!=NULL)&&(m_pIGameDataBaseEngineSink->InitializeSink(QUERY_ME_INTERFACE(IUnknownEx))==false))
		{
			//�������
			ASSERT(FALSE);

			//�����Ϣ
			CTraceService::TraceString(TEXT("��Ϸ���ݿ���չ���������������ʧ��"),TraceLevel_Exception);

			return false;
		}

#ifdef USE_REDIS
		//������������
		CStringA strHost = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerAddr);
		bool bRet = m_RedisInstance.connect(strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, "", 500000);
		if (bRet)
		{
			/* PING server */
			CString str;
			str.Format(TEXT("PING: %s\n"), m_RedisInstance.ping() ? _T("OK") : _T("Fail"));
			::OutputDebugString(str);

			///* Try a GET and two INCR */
			//string strValue;
			//m_RedisInstance.get("foo", strValue);
			//str.Format(TEXT("GET foo: %s\n"), CYSCharUtils::ANSIToUnicode(strValue.c_str()));
			//::OutputDebugString(str);
		}
#endif

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//��������
	UnLockAndroidUser();

	//���ñ���
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//�ͷŶ���
	SafeRelease(m_pIGameDataBaseEngineSink);

	//�ر�����
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->Close();
		m_AccountsDBModule.CloseInstance();
	}

	//�ر�����
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->Close();
		m_TreasureDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->Close();
		m_PlatformDBModule.CloseInstance();
	}

#ifdef USE_REDIS
	//���ٻ�������
	m_RedisInstance.disConnect();
#endif

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//������
	switch (wRequestID)
	{
	case DBR_GR_LOGON_USERID:			//I D ��¼
		{
			bSucceed = OnRequestLogonUserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_RELINE:
		{
			bSucceed = OnRequestLogonReLine(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_WRITE_GAME_SCORE:		//��Ϸд��
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:		//�뿪����
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:		//��Ϸ��¼
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_PARAMETER:	//��������
		{
			bSucceed = OnRequestLoadAndroidParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_USER:		//���ػ���
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_PROPERTY:		//���ص���
		{
			bSucceed = OnRequestLoadGameProperty(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_USER_PROPERTY:		//�����û�����
		{
			bSucceed = OnRequestLoadUserProperty(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_USER_SAVE_SCORE:		//������Ϸ��
		{
			bSucceed = OnRequestUserSaveScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TAKE_SCORE:		//��ȡ��Ϸ��
		{
			bSucceed = OnRequestUserTakeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_INSURE_INFO:		//��ѯ����
		{
			bSucceed = OnRequestQueryInsureInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PROPERTY_REQUEST:		//��������
		{
			bSucceed = OnRequestPropertyRequest(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SYSTEM_MESSAGE:   //ϵͳ��Ϣ
		{
			bSucceed = OnRequestLoadSystemMessage(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_SIGNUP:			//��������
		{
			bSucceed = OnRequestMatchSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_UNSIGNUP:			//�˳�����
		{
			bSucceed = OnRequestMatchUnSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_START:			//������ʼ
		{
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_ELIMINATE:		//������̭
		{
			bSucceed = OnRequestMatchEliminate(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_OVER:				//��������
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);
		}
		break;	
	case DBR_GR_MATCH_REWARD:			//��������
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;	
	case DBR_GR_TASK_LOAD_LIST:			//��������
		{
			bSucceed = OnRequestLoadTaskList(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_QUERY_INFO:		//��ѯ����
		{
			bSucceed = OnRequestTaskQueryInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_REWARD:			//��ȡ����
		{
			bSucceed = OnRequestTaskReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_PROGRESS:			//�������
		{
			bSucceed = OnRequestTaskProgress(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_EXCHANGE_SCORE:			//�һ���Ϸ��
		{
			bSucceed = OnRequestExchangeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_SEND_EMAIL:				//�ʼ�����
		{
			bSucceed = OnRequestSendEMail(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_REFRESH_ONLINE_USER:	//ˢ����������
		{
			bSucceed = OnRequestRefreshOnlineUser(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_PRIVATE_INFO:			//��������
		{
			bSucceed = OnRequestPrivateInfo(dwContextID, pData, wDataSize, dwUserID);
		}
	break;
	case DBR_GR_CREATE_PRIVAT:			//����Ƿ���Դ�������
		{
			bSucceed = OnRequestCreatePrivate(dwContextID, pData, wDataSize, dwUserID);
		}
	break;
	case DBR_GR_CREATE_PRIVAT_COST:			//�۳�����
		{
			bSucceed = OnRequestCreatePrivateCost(dwContextID, pData, wDataSize, dwUserID);
		}
	break;
	case DBR_GR_PRIVATE_GAME_RECORD:		//˽�˳�����Ϸ��¼;
	{
		bSucceed = OnRequestPrivateGameRecord(dwContextID, pData, wDataSize, dwUserID);
	}
	break;
	case DBR_GR_SINGLE_PRIVATE_RECORD:		//˽�˳�������¼;
	{
		bSucceed = OnRequestSinglePrivateGameRecord(dwContextID, pData, wDataSize, dwUserID);
	}
	break;
	}

	//Э��֪ͨ
	if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	return bSucceed;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch)
{
	//ִ�в�ѯ
	DBR_GR_LogonUserID * pLogonUserID=(DBR_GR_LogonUserID *)pData;
	dwUserID = pLogonUserID->dwUserID;

	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonUserID)) return false;

#ifdef USE_REDIS
	//�����ȡ�������
	ASSERT(m_RedisInstance.isConneced());
	if (m_RedisInstance.isConneced())
	{
		ValueMap vMap;
		char szBuf[32] = { 0 };
		sprintf_s(szBuf, sizeof(szBuf), "id.%d", dwUserID);
		string strKey(szBuf);
		if (m_RedisInstance.hgetall(strKey, vMap))
		{
			OnLogonDisposeResult(dwContextID, pLogonUserID->szPassword, vMap);
			return true;
		}
	}
#endif

	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonUserID->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(pLogonUserID->dwUserID);
		m_AccountsDBModule->AddParameter(pLogonUserID->szPassword);
		m_AccountsDBModule->AddParameter(szClientAddr);
		m_AccountsDBModule->AddParameter(pLogonUserID->szMachineID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);

		////��������
		//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		//{
		//	m_GameDBModule->AddParameter(pLogonUserID->dwMatchID);
		//	m_GameDBModule->AddParameter(pLogonUserID->dwMatchNO);	
		//}

		//�������
		m_AccountsDBModule->AddParameterOutput("@strErrorDescribe");
		m_AccountsDBModule->AddParameterOutput("@nRet");

		//ִ�в�ѯ
		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_EfficacyUserID");
		if (bResult)
		{
			//�û���Ϣ
			lstrcpyn(m_LogonSuccess.szPassword, pLogonUserID->szPassword, CountArray(m_LogonSuccess.szPassword));
			lstrcpyn(m_LogonSuccess.szMachineID, pLogonUserID->szMachineID, CountArray(m_LogonSuccess.szMachineID));

			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);

			//�����Ϣ;
			TCHAR szDescribeString[128] = TEXT("");
			LONG lResultCode			= DB_SUCCESS;

			if (rs.GetCount() == 0)
			{
				m_AccountsDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
				SQLRecordset rsRet(m_AccountsDBModule);

				//��ȡ��Ϣ
				rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));
				lResultCode = rsRet.GetValue_LONG("@nRet");
			}

			OnLogonDisposeResult(dwContextID, rs, lResultCode, szDescribeString, false);

			return true;
		}
		
		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		SQLRecordset rs;

		//������
		OnLogonDisposeResult(dwContextID, rs, DB_ERROR, TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), false);

		return false;
	}

	return true;
}

// �û�����
bool CDataBaseEngineSink::OnRequestLogonReLine(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_RELINE_USERID));
		if (wDataSize != sizeof(DBR_GR_RELINE_USERID)) return false;
		
		//ִ�в�ѯ
		DBR_GR_RELINE_USERID * pLogonUserID = (DBR_GR_RELINE_USERID *)pData;
		dwUserID = pLogonUserID->dwUserID;

		//�����ȡ�������
		//ASSERT(m_RedisInstance.isConneced());
		//if (m_RedisInstance.isConneced())
		//{
		//	ValueMap vMap;
		//	char szBuf[32] = { 0 };
		//	sprintf_s(szBuf, sizeof(szBuf), "id.%d", dwUserID);
		//	string strKey(szBuf);
		//	if (m_RedisInstance.hgetall(strKey, vMap))
		//	{
		//		GetUserPropFromRedis(dwContextID, pLogonUserID->dwUserID);
		//		return true;
		//	}
		//}

		//�������
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(pLogonUserID->dwUserID);

		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadUserProperty");

		//ִ�гɹ�
		if (bResult)
		{
			//��������
			tagUserProperty prop;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET - 1024] = { 0 };
			DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);

			//���ñ���
			WORD wPropertyCount = 0;
			WORD wSendDataSize = sizeof(DBO_GR_PropertyListInfo);

			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);
			if (rs.GetCount() > 0)
			{
				//��������
				do
				{
					ZeroMemory(&prop, sizeof(tagUserProperty));

					//��ȡ����
					prop.dwDBID = rs.GetValue_WORD("id");
					prop.wPropertyUseMark = rs.GetValue_WORD("PropID");
					prop.wPropertyCount = rs.GetValue_WORD("PropCount");
					rs.GetValue_SystemTime_String("OverTime", prop.OverTime);

					//�����ж�				
					if (wSendDataSize + sizeof(DBO_GR_PropertyListInfo) > sizeof(cbDataBuffer))
					{
						//���ñ���
						pPropertyListInfo->wPropertyCount = wPropertyCount;

						//��������
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);

						//���ñ���
						wPropertyCount = 0;
						wSendDataSize = sizeof(DBO_GR_PropertyListInfo);
						pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);
					}

					//��������
					CopyMemory(pDataBuffer, &prop, sizeof(tagUserProperty));

					//���ñ���
					wPropertyCount++;
					wSendDataSize += sizeof(tagUserProperty);
					pDataBuffer += sizeof(tagUserProperty);

				} while (rs.Fetch());
			}

			//ʣ�෢��
			if (wPropertyCount > 0 && wSendDataSize > 0)
			{
				//���ñ���
				pPropertyListInfo->wPropertyCount = wPropertyCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ϸд��
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBModule->ClearParameters();

		//�û���Ϣ
		m_AccountsDBModule->AddParameter(pWriteGameScore->dwUserID);
		m_AccountsDBModule->AddParameter(pWriteGameScore->dwDBQuestID);
		m_AccountsDBModule->AddParameter(pWriteGameScore->dwInoutIndex);

		//����ɼ�
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.lScore);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.lGrade);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.lInsure);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.lRevenue);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.dwWinCount);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.dwLostCount);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.dwDrawCount);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.dwFleeCount);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.lIngot);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.dwExperience);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.lLoveLiness);
		m_AccountsDBModule->AddParameter(pWriteGameScore->VariationInfo.dwPlayTimeCount);

		//�������
		m_AccountsDBModule->AddParameter(pWriteGameScore->bTaskForward);

		////��������
		//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		//{
		//	//������Ϣ
		//	m_GameDBModule->AddParameter(pWriteGameScore->dwMatchID);
		//	m_GameDBModule->AddParameter(pWriteGameScore->dwMatchNO);
		//}

		//������Ϣ
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_AccountsDBModule->AddParameter(szClientAddr);

		//ִ�в�ѯ
		m_AccountsDBModule->ExecuteProcess("GSP_GR_WriteGameScore");

		BYTE* pDt = pWriteGameScore->aryCardData;
		CString strCardData;
		strCardData.Format(_T("%d,%d,%d,%d,%d"), pDt[0], pDt[1], pDt[2], pDt[3], pDt[4]);

		UpdateCachedData(dwUserID, pWriteGameScore->VariationInfo, pWriteGameScore->cbMaxCardType, strCardData);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�뿪����
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();

		//�û���Ϣ
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwUserID);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwOnLineTimeCount);

		//ϵͳ��Ϣ
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwInoutIndex);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwLeaveReason);

		//��¼�ɼ�
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.lScore);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.lGrade);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.lInsure);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.lRevenue);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.dwWinCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.dwLostCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.dwDrawCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.dwFleeCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.lIngot);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.dwExperience);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.lLoveLiness);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->RecordInfo.dwPlayTimeCount);

		//����ɼ�
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.lScore);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.lGrade);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.lInsure);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.lRevenue);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.dwWinCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.dwLostCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.dwDrawCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.dwFleeCount);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.lIngot);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.dwExperience);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.lLoveLiness);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->VariationInfo.dwPlayTimeCount);

		//�������
		m_TreasureDBModule->AddParameter(pLeaveGameServer->cbMaxCardType);
		BYTE* pDt = pLeaveGameServer->aryCardData;
		CString strCardData;
		strCardData.Format(_T("%d,%d,%d,%d,%d"), pDt[0], pDt[1], pDt[2], pDt[3], pDt[4]);
		m_TreasureDBModule->AddParameter(strCardData);

		//��������
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->szMachineID);

		//ִ�в�ѯ
		m_TreasureDBModule->ExecuteProcess("GSP_GR_LeaveGameServer");

		UpdateCachedData(dwUserID, pLeaveGameServer->VariationInfo, pLeaveGameServer->cbMaxCardType, strCardData);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ϸ��¼
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
		dwUserID=INVALID_DWORD;

		//Ч�����
		ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
		ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
		ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

		//Ч�����
		if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
		if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
		if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

		//������Ϣ
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);

		//������Ϣ
		m_TreasureDBModule->AddParameter(pGameScoreRecord->wTableID);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->wUserCount);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->wAndroidCount);

		//˰�����
		m_TreasureDBModule->AddParameter(pGameScoreRecord->lWasteCount);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->lRevenueCount);

		//ͳ����Ϣ
		m_TreasureDBModule->AddParameter(pGameScoreRecord->dwUserMemal);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->dwPlayTimeCount);

		//ʱ����Ϣ
		m_TreasureDBModule->AddParameter(pGameScoreRecord->SystemTimeStart);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->SystemTimeConclude);

		//ִ�в�ѯ
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_RecordDrawInfo");

		//д���¼
		if (bReturn)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);
			if (rs.GetCount()>0)
			{
				//��ȡ��ʶ
				DWORD dwDrawID = rs.GetValue_DWORD("DrawID");

				//д���¼
				for (WORD i = 0; i < pGameScoreRecord->wRecordCount; i++)
				{
					//���ò���
					m_TreasureDBModule->ClearParameters();

					//������Ϣ
					m_TreasureDBModule->AddParameter(dwDrawID);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwUserID);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].wChairID);

					//�û���Ϣ
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwDBQuestID);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwInoutIndex);

					//�ɼ���Ϣ
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].lScore);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].lGrade);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].lRevenue);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

					//ִ�в�ѯ
					m_TreasureDBModule->ExecuteProcess("GSP_GR_RecordDrawScore");
				}
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//����У��
	ASSERT(sizeof(DBR_GR_LoadAndroidUser)==wDataSize);
	if(sizeof(DBR_GR_LoadAndroidUser)!=wDataSize) return false;

	try
	{		
		//��ȡ����
		DBR_GR_LoadAndroidUser * pLoadAndroidUser = (DBR_GR_LoadAndroidUser *)pData;
        if(pLoadAndroidUser==NULL) return false;

		//��������
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//��������
		GameAndroidInfo.dwBatchID=pLoadAndroidUser->dwBatchID;

		//�û��ʻ�
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_AccountsDBModule->AddParameter(pLoadAndroidUser->dwBatchID);
		m_AccountsDBModule->AddParameter(pLoadAndroidUser->dwAndroidCount);
		m_AccountsDBModule->AddParameterOutput("@nRet");

		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadAndroidUser");
		if ( bResult )
		{
			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule, false);
			rs.FetchAll();

			m_AccountsDBModule->ExecuteSQL("select @nRet");
			SQLRecordset rsRet(m_AccountsDBModule);
			//ִ�в�ѯ
			GameAndroidInfo.lResultCode = rsRet.GetValue_LONG("@nRet");

				//��ȡ��Ϣ
			for (WORD i = 0; i < CountArray(GameAndroidInfo.AndroidAccountsInfo); i++)
			{
				//�����ж�
				if (i>=rs.GetCount()) break;

				//���Ч��
				ASSERT(GameAndroidInfo.wAndroidCount < CountArray(GameAndroidInfo.AndroidAccountsInfo));
				if (GameAndroidInfo.wAndroidCount >= CountArray(GameAndroidInfo.AndroidAccountsInfo)) break;

				//��ȡ����
				GameAndroidInfo.wAndroidCount++;
				GameAndroidInfo.AndroidAccountsInfo[i].dwUserID = rs.GetValue_DWORD("UserID");
				rs.GetValue_String("LogonPass", GameAndroidInfo.AndroidAccountsInfo[i].szPassword, CountArray(GameAndroidInfo.AndroidAccountsInfo[i].szPassword));

				//�ƶ���¼
				rs.Goto(i);
			}

			//������Ϣ
			WORD wHeadSize = sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
			WORD wDataSize = GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidAccountsInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO, dwContextID, &GameAndroidInfo, wHeadSize + wDataSize);

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//�������
		GameAndroidInfo.wAndroidCount=0L;
		GameAndroidInfo.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize);
	}

	return false;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//�û��ʻ�
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(m_pGameServiceAttrib->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadAndroidConfigure");

		if (bResult)
		{
			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);

			//��ȡ��Ϣ
			for (WORD i = 0; i < CountArray(GameAndroidParameter.AndroidParameter); i++)
			{
				//�����ж�
				if (i>=rs.GetCount()) break;

				//���Ч��
				ASSERT(GameAndroidParameter.wParameterCount < CountArray(GameAndroidParameter.AndroidParameter));
				if (GameAndroidParameter.wParameterCount >= CountArray(GameAndroidParameter.AndroidParameter)) break;

				//��ȡ����
				GameAndroidParameter.wParameterCount++;
				GameAndroidParameter.AndroidParameter[i].dwBatchID = rs.GetValue_DWORD("BatchID");
				GameAndroidParameter.AndroidParameter[i].dwServiceMode = rs.GetValue_DWORD("ServiceMode");
				GameAndroidParameter.AndroidParameter[i].dwAndroidCount = rs.GetValue_DWORD("AndroidCount");
				GameAndroidParameter.AndroidParameter[i].dwEnterTime = rs.GetValue_DWORD("EnterTime");
				GameAndroidParameter.AndroidParameter[i].dwLeaveTime = rs.GetValue_DWORD("LeaveTime");
				GameAndroidParameter.AndroidParameter[i].lTakeMinScore = rs.GetValue_LONGLONG("TakeMinScore");
				GameAndroidParameter.AndroidParameter[i].lTakeMaxScore = rs.GetValue_LONGLONG("TakeMaxScore");
				GameAndroidParameter.AndroidParameter[i].dwEnterMinInterval = rs.GetValue_DWORD("EnterMinInterval");
				GameAndroidParameter.AndroidParameter[i].dwEnterMaxInterval = rs.GetValue_DWORD("EnterMaxInterval");
				GameAndroidParameter.AndroidParameter[i].dwLeaveMinInterval = rs.GetValue_DWORD("LeaveMinInterval");
				GameAndroidParameter.AndroidParameter[i].dwLeaveMaxInterval = rs.GetValue_DWORD("LeaveMaxInterval");
				GameAndroidParameter.AndroidParameter[i].dwSwitchMinInnings = rs.GetValue_DWORD("SwitchMinInnings");
				GameAndroidParameter.AndroidParameter[i].dwSwitchMaxInnings = rs.GetValue_DWORD("SwitchMaxInnings");

				//�ƶ���¼
				rs.Fetch();
			}

			//������Ϣ
			WORD wHeadSize = sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
			WORD wDataSize = GameAndroidParameter.wParameterCount*sizeof(GameAndroidParameter.AndroidParameter[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER, dwContextID, &GameAndroidParameter, wHeadSize + wDataSize);

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//��������
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//�������
		GameAndroidParameter.wParameterCount=0L;
		GameAndroidParameter.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER,dwContextID,&GameAndroidParameter,wHeadSize);
	}

	return false;
}

//���ص���
bool CDataBaseEngineSink::OnRequestLoadGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//��������
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo, sizeof(GamePropertyInfo));

		//�������
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);

		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadGameProperty");

		if (bResult)
		{
			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule, false);
			rs.FetchAll();

			m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_TreasureDBModule);

			//ִ�в�ѯ
			GamePropertyInfo.lResultCode = rsRet.GetValue_LONG("@nRet");

			//��ȡ��Ϣ
			for (WORD i = 0; i < CountArray(GamePropertyInfo.PropertyInfo); i++)
			{
				//�����ж�
				if (i >= rs.GetCount()) break;

				//���Ч��
				ASSERT(GamePropertyInfo.cbPropertyCount < CountArray(GamePropertyInfo.PropertyInfo));
				if (GamePropertyInfo.cbPropertyCount >= CountArray(GamePropertyInfo.PropertyInfo)) break;

				//��ȡ����
				GamePropertyInfo.cbPropertyCount++;
				GamePropertyInfo.PropertyInfo[i].wIndex = rs.GetValue_WORD(("ID"));
				GamePropertyInfo.PropertyInfo[i].wDiscount = rs.GetValue_WORD(("Discount"));
				GamePropertyInfo.PropertyInfo[i].wIssueArea = rs.GetValue_WORD(("IssueArea"));
				GamePropertyInfo.PropertyInfo[i].dPropertyCash = rs.GetValue_DOUBLE(("Cash"));
				GamePropertyInfo.PropertyInfo[i].lPropertyGold = rs.GetValue_LONGLONG(("Gold"));
				GamePropertyInfo.PropertyInfo[i].lSendLoveLiness = rs.GetValue_LONGLONG(("SendLoveLiness"));
				GamePropertyInfo.PropertyInfo[i].lRecvLoveLiness = rs.GetValue_LONGLONG(("RecvLoveLiness"));

				//�ƶ���¼
				rs.Goto(i);
			}

			//������Ϣ
			WORD wHeadSize = sizeof(GamePropertyInfo) - sizeof(GamePropertyInfo.PropertyInfo);
			WORD wDataSize = GamePropertyInfo.cbPropertyCount*sizeof(GamePropertyInfo.PropertyInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO, dwContextID, &GamePropertyInfo, wHeadSize + wDataSize);
		}

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//��������
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo, sizeof(GamePropertyInfo));

		//�������
		GamePropertyInfo.cbPropertyCount = 0L;
		GamePropertyInfo.lResultCode = DB_ERROR;

		//������Ϣ
		WORD wHeadSize = sizeof(GamePropertyInfo) - sizeof(GamePropertyInfo.PropertyInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO, dwContextID, &GamePropertyInfo, wHeadSize);

		return false;
	}

	return true;
}

//�����û�����
bool CDataBaseEngineSink::OnRequestLoadUserProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_LOADPROPERTY));
		if (wDataSize != sizeof(DBR_GR_LOADPROPERTY)) return false;

		//������
		DBR_GR_LOADPROPERTY * pLoadProperty = (DBR_GR_LOADPROPERTY *)pData;
		dwUserID = pLoadProperty->dwUserID;

		//�����ȡ�������
		//ASSERT(m_RedisInstance.isConneced());
		//if (m_RedisInstance.isConneced())
		//{
		//	ValueMap vMap;
		//	char szBuf[32] = { 0 };
		//	sprintf_s(szBuf, sizeof(szBuf), "id.%d", dwUserID);
		//	string strKey(szBuf);
		//	if (m_RedisInstance.hgetall(strKey, vMap))
		//	{
		//		GetUserPropFromRedis(dwContextID, pLoadProperty->dwUserID);
		//		return true;
		//	}
		//}

		//�������
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(pLoadProperty->dwUserID);

		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadUserProperty");

		//ִ�гɹ�
		if (bResult)
		{
			//��������
			tagUserProperty prop;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET - 1024] = { 0 };
			DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);

			//���ñ���
			WORD wPropertyCount = 0;
			WORD wSendDataSize = sizeof(DBO_GR_PropertyListInfo);

			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);
			if (rs.GetCount() > 0)
			{
				//��������
				do
				{
					ZeroMemory(&prop, sizeof(tagUserProperty));

					//��ȡ����
					prop.dwDBID = rs.GetValue_WORD("id");
					prop.wPropertyUseMark = rs.GetValue_WORD("PropID");
					prop.wPropertyCount = rs.GetValue_WORD("PropCount");
					//prop.dwValidNum = rs.GetValue_DWORD("LastTime");
					//rs.GetValue_SystemTime_String("UsedTime", prop.EffectTime);
					rs.GetValue_SystemTime_String("OverTime", prop.OverTime);

					//�����ж�				
					if (wSendDataSize + sizeof(DBO_GR_PropertyListInfo) > sizeof(cbDataBuffer))
					{
						//���ñ���
						pPropertyListInfo->wPropertyCount = wPropertyCount;

						//��������
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);

						//���ñ���
						wPropertyCount = 0;
						wSendDataSize = sizeof(DBO_GR_PropertyListInfo);
						pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);
					}

					//��������
					CopyMemory(pDataBuffer, &prop, sizeof(tagUserProperty));

					//���ñ���
					wPropertyCount++;
					wSendDataSize += sizeof(tagUserProperty);
					pDataBuffer += sizeof(tagUserProperty);

				} while (rs.Fetch());
			}

			//ʣ�෢��
			if (wPropertyCount > 0 && wSendDataSize > 0)
			{
				//���ñ���
				pPropertyListInfo->wPropertyCount = wPropertyCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//������Ϸ��
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserSaveScore));
	if (wDataSize!=sizeof(DBR_GR_UserSaveScore)) return false;

	//��������
	DBR_GR_UserSaveScore * pUserSaveScore=(DBR_GR_UserSaveScore *)pData;
	dwUserID=pUserSaveScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pUserSaveScore->dwUserID);
		m_TreasureDBModule->AddParameter(pUserSaveScore->lSaveScore);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pUserSaveScore->szMachineID);

		//�������
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//ִ�в�ѯ
		BOOL bResult = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserSaveScore");
		if ( bResult )
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_TreasureDBModule);

			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");

			//��ȡ��Ϣ
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");

			OnInsureDisposeResult(dwContextID, rs, lResultCode, pUserSaveScore->lSaveScore, szDescribeString, false, pUserSaveScore->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		SQLRecordset rs;

		//������
		OnInsureDisposeResult(dwContextID, rs, DB_ERROR,pUserSaveScore->lSaveScore,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserSaveScore->cbActivityGame);

		return false;
	}

	return true;
}

//��ȡ��Ϸ��
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UserTakeScore));
	if (wDataSize!=sizeof(DBR_GR_UserTakeScore)) return false;

	//��������
	DBR_GR_UserTakeScore * pUserTakeScore=(DBR_GR_UserTakeScore *)pData;
	dwUserID=pUserTakeScore->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pUserTakeScore->dwUserID);
		m_TreasureDBModule->AddParameter(pUserTakeScore->lTakeScore);
		m_TreasureDBModule->AddParameter(pUserTakeScore->szPassword);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pUserTakeScore->szMachineID);

		//�������
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//ִ�в�ѯ
		BOOL bResult = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserTakeScore");

		//�������
		if (bResult)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_TreasureDBModule);

			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");

			OnInsureDisposeResult(dwContextID, rs, lResultCode, 0L, szDescribeString, false, pUserTakeScore->cbActivityGame);
		}
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		SQLRecordset rs;

		//������
		OnInsureDisposeResult(dwContextID,rs, DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pUserTakeScore->cbActivityGame);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_QueryInsureInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryInsureInfo)) return false;

	//������
	DBR_GR_QueryInsureInfo * pQueryInsureInfo=(DBR_GR_QueryInsureInfo *)pData;
	dwUserID=pQueryInsureInfo->dwUserID;

	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pQueryInsureInfo->dwUserID);
		m_TreasureDBModule->AddParameter(pQueryInsureInfo->szPassword);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//�������
		BOOL bResult = m_TreasureDBModule->ExecuteProcess("GSP_GR_QueryUserInsureInfo");
		if (bResult)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_TreasureDBModule);
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");

			if (lResultCode==DB_SUCCESS)
			{
				//��������
				DBO_GR_UserInsureInfo UserInsureInfo;
				ZeroMemory(&UserInsureInfo, sizeof(UserInsureInfo));

				//������Ϣ
				UserInsureInfo.cbActivityGame = pQueryInsureInfo->cbActivityGame;
				UserInsureInfo.cbEnjoinTransfer = rs.GetValue_BYTE("EnjoinTransfer");
				UserInsureInfo.wRevenueTake = rs.GetValue_WORD("RevenueTake");
				UserInsureInfo.wRevenueTransfer = rs.GetValue_WORD("RevenueTransfer");
				UserInsureInfo.wRevenueTransferMember = rs.GetValue_WORD("RevenueTransferMember");
				UserInsureInfo.wServerID = rs.GetValue_WORD("ServerID");
				UserInsureInfo.lUserScore = rs.GetValue_LONGLONG("Score");
				UserInsureInfo.lUserInsure = rs.GetValue_LONGLONG("Insure");
				UserInsureInfo.lTransferPrerequisite = rs.GetValue_LONGLONG("TransferPrerequisite");

				//���ͽ��
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_INFO, dwContextID, &UserInsureInfo, sizeof(UserInsureInfo));
			}
			else
			{
				//��ȡ����
				TCHAR szDescribeString[128] = TEXT("");

				//��ȡ��Ϣ
				rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

				//������
				OnInsureDisposeResult(dwContextID, rs, lResultCode, 0L, szDescribeString, false, pQueryInsureInfo->cbActivityGame);
			}

			return true;
		}
		ASSERT(FALSE);
		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		SQLRecordset rs;
		OnInsureDisposeResult(dwContextID, rs, DB_ERROR, 0L, TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false,pQueryInsureInfo->cbActivityGame);

		return false;
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestTaskProgress(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_TaskProgress));
		if (wDataSize != sizeof(DBR_GR_TaskProgress)) return false;

		DBR_GR_TaskProgress * pTaskProgress = (DBR_GR_TaskProgress*)pData;
		dwUserID = pTaskProgress->dwUserID;

		//�������
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter(pTaskProgress->dwUserID);
		m_PlatformDBModule->AddParameter(pTaskProgress->dwID);
		m_PlatformDBModule->AddParameter(pTaskProgress->wTaskProgress);
		m_PlatformDBModule->AddParameter(pTaskProgress->cbTaskStatus);
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//ִ�нű�
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_TaskForward");

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
	return true;
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskReward));
		if (wDataSize!=sizeof(DBR_GR_TaskReward)) return false;

		//������
		DBR_GR_TaskReward * pTaskReward=(DBR_GR_TaskReward *)pData;

		//���ñ���
		dwUserID = pTaskReward->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskReward->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter(pTaskReward->dwUserID);
		m_PlatformDBModule->AddParameter(pTaskReward->wTaskID);
		m_PlatformDBModule->AddParameter(pTaskReward->szPassword);
		m_PlatformDBModule->AddParameter(szClientAddr);
		m_PlatformDBModule->AddParameter(pTaskReward->szMachineID);
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//ִ�нű�
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_TaskReward");

		if ( bResult )
		{
			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);

			//��������
			DBO_GR_TaskResult TaskResult;
			ZeroMemory(&TaskResult, sizeof(TaskResult));

			m_PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_PlatformDBModule);

			//��ȡ����
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");
			rsRet.GetValue_String("@strErrorDescribe", TaskResult.szNotifyContent, CountArray(TaskResult.szNotifyContent));

			//������Ϣ
			TaskResult.wCommandID = SUB_GR_TASK_REWARD;
			TaskResult.wCurrTaskID = pTaskReward->wTaskID;
			TaskResult.bSuccessed = lResultCode == DB_SUCCESS;

			//��ȡ����
			if (TaskResult.bSuccessed == true)
			{
				TaskResult.lCurrScore = rs.GetValue_LONGLONG("Score");
				TaskResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
			}

			//���ͽ��
			WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);

			return true;
		}
		ASSERT(FALSE);
		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_REWARD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		m_wArrDailyTaskID.RemoveAll();

		//���ñ���
		dwUserID = 0;

		//�������
		m_PlatformDBModule->ClearParameters();

		//ִ������
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_LoadTaskList");

		//ִ�гɹ�
		if (bResult)
		{
			//��������
			tagTaskParameter TaskParameter;	
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};					
			DBO_GR_TaskListInfo * pTaskListInfo = (DBO_GR_TaskListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(DBO_GR_TaskListInfo);

			//���ñ���
			WORD wTaskCount=0;
			WORD wSendDataSize=sizeof(DBO_GR_TaskListInfo);

			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);
			if ( rs.GetCount()>0 )
			{
				//��������
				do
				{
					//��ȡ����
					TaskParameter.wTaskID = rs.GetValue_WORD("TaskID");
					TaskParameter.wTaskType = rs.GetValue_WORD("TaskType");
					TaskParameter.cbPlayerType = rs.GetValue_BYTE("UserType");
					TaskParameter.wKindID = rs.GetValue_WORD("KindID");
					TaskParameter.wTaskObject = rs.GetValue_WORD("Innings");
					TaskParameter.dwTimeLimit = rs.GetValue_DWORD("TimeLimit");
					TaskParameter.lStandardAwardGold = rs.GetValue_LONGLONG("StandardAwardGold");
					TaskParameter.lStandardAwardMedal = rs.GetValue_LONGLONG("StandardAwardMedal");
					TaskParameter.lMemberAwardGold = rs.GetValue_LONGLONG("MemberAwardGold");
					TaskParameter.lMemberAwardMedal = rs.GetValue_LONGLONG("MemberAwardMedal");

					//������Ϣ
					//rs.GetValue_String("TaskName", TaskParameter.szTaskName, CountArray(TaskParameter.szTaskName));
					//rs.GetValue_String("TaskDescription", TaskParameter.szTaskDescribe, CountArray(TaskParameter.szTaskDescribe));

					//�����ж�				
					if (wSendDataSize + sizeof(tagTaskParameter) > sizeof(cbDataBuffer))
					{
						//���ñ���
						pTaskListInfo->wTaskCount = wTaskCount;

						//��������
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST, dwContextID, pTaskListInfo, wSendDataSize);

						//���ñ���
						wTaskCount = 0;
						wSendDataSize = sizeof(DBO_GR_TaskListInfo);
						pDataBuffer = cbDataBuffer + sizeof(DBO_GR_TaskListInfo);
					}

					//��������
					CopyMemory(pDataBuffer, &TaskParameter, sizeof(tagTaskParameter));

					//���ñ���
					wTaskCount++;
					wSendDataSize += sizeof(tagTaskParameter);
					pDataBuffer += sizeof(tagTaskParameter);

					m_wArrDailyTaskID.Add(TaskParameter.wTaskID);

				} while (rs.Fetch());
			}

			//ʣ�෢��
			if(wTaskCount>0 && wSendDataSize>0)
			{
				//���ñ���
				pTaskListInfo->wTaskCount = wTaskCount;

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);
			}

			//����֪ͨ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST_END,dwContextID,NULL,0);						
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//����֪ͨ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST_END, dwContextID, NULL, 0);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_TaskQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_TaskQueryInfo)) return false;

		//������
		DBR_GR_TaskQueryInfo * pTaskQueryInfo=(DBR_GR_TaskQueryInfo *)pData;

		//���ñ���
		dwUserID = pTaskQueryInfo->dwUserID;

		//�������
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter((WORD)0);
		m_PlatformDBModule->AddParameter(pTaskQueryInfo->dwUserID);
		m_PlatformDBModule->AddParameter(pTaskQueryInfo->szPassword);

		//�������
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//ִ�нű�
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_QueryTaskInfo");

		//ִ�гɹ�
		if (bResult)
		{
			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule, false);
			rs.FetchAll();

			m_PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_PlatformDBModule);
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");

			if (lResultCode == DB_SUCCESS)
			{
				//��������
				DBO_GR_TaskInfo TaskInfo;
				tagTaskStatus * pTaskStatus = NULL;
				ZeroMemory(&TaskInfo, sizeof(TaskInfo));

				for (int i = 0; i < rs.GetCount(); i++)
				{
					rs.Goto(i);

					//���ñ���
					pTaskStatus = &TaskInfo.TaskStatus[TaskInfo.wTaskCount++];

					//��ȡ����
					pTaskStatus->wTaskID = rs.GetValue_WORD("TaskID");
					pTaskStatus->cbTaskStatus = rs.GetValue_BYTE("TaskStatus");
					pTaskStatus->wTaskProgress = rs.GetValue_WORD("Progress");
				}

				//���ͽ��
				WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
				wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_INFO, dwContextID, &TaskInfo, wSendDataSize);
			}
			else
			{
				//��������
				DBO_GR_TaskResult TaskResult;
				ZeroMemory(&TaskResult, sizeof(TaskResult));

				//��ȡ��Ϣ
				rsRet.GetValue_String("@strErrorDescribe", TaskResult.szNotifyContent, CountArray(TaskResult.szNotifyContent));

				//������Ϣ
				TaskResult.bSuccessed = false;
				TaskResult.wCommandID = SUB_GR_TASK_LOAD_INFO;

				//���ͽ��
				WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);
			}

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_LOAD_INFO;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//�һ���Ϸ��
bool CDataBaseEngineSink::OnRequestExchangeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ExchangeScore));
		if (wDataSize!=sizeof(DBR_GR_ExchangeScore)) return false;

		//������
		DBR_GR_ExchangeScore * pExchangeScore=(DBR_GR_ExchangeScore *)pData;

		//���ñ���
		dwUserID = pExchangeScore->dwUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pExchangeScore->dwUserID);
		m_TreasureDBModule->AddParameter(pExchangeScore->lExchangeIngot);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pExchangeScore->szMachineID);

		//�������
		m_TreasureDBModule->AddParameterOutput("@strNotifyContent");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//ִ�нű�
		BOOL bResultCode = m_TreasureDBModule->ExecuteProcess(("GSP_GR_ExchangeScore"));

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//ִ�гɹ�
		if (bResultCode)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			//���ñ���
			ExchangeResult.bSuccessed=true;

			//��������
			if (rs.GetCount()>0)
			{
				//��ȡ����
				ExchangeResult.lCurrScore = rs.GetValue_LONGLONG("CurrScore");
				ExchangeResult.lCurrIngot = rs.GetValue_LONGLONG("CurrIngot");
			}
			else
			{
				m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
				SQLRecordset rsRet(m_TreasureDBModule);

				//��ʾ����
				rsRet.GetValue_String("@strNotifyContent", ExchangeResult.szNotifyContent, CountArray(ExchangeResult.szNotifyContent));
			}
		}

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//���ñ���
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("���ݿ��쳣�����Ժ����ԣ�"),CountArray(ExchangeResult.szNotifyContent));

		//�����С
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//���Ͳ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestPropertyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_PropertyRequest));
		if (wDataSize!=sizeof(DBR_GR_PropertyRequest)) return false;

		//������
		DBR_GR_PropertyRequest * pPropertyRequest=(DBR_GR_PropertyRequest *)pData;
		dwUserID=pPropertyRequest->dwSourceUserID;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyRequest->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBModule->ClearParameters();

		//������Ϣ
		m_AccountsDBModule->AddParameter(pPropertyRequest->dwSourceUserID);
		m_AccountsDBModule->AddParameter(pPropertyRequest->dwTargetUserID);
		m_AccountsDBModule->AddParameter(pPropertyRequest->wPropertyIndex);
		m_AccountsDBModule->AddParameter(pPropertyRequest->wItemCount);

		//��������
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_AccountsDBModule->AddParameter(pPropertyRequest->wTableID);

		////����ʽ
		//m_AccountsDBModule->AddParameter(pPropertyRequest->cbConsumeScore);
		//m_AccountsDBModule->AddParameter(pPropertyRequest->lFrozenedScore);

		//ϵͳ��Ϣ
		m_AccountsDBModule->AddParameter(pPropertyRequest->dwInoutIndex);
		m_AccountsDBModule->AddParameter(szClientAddr);
		m_AccountsDBModule->AddParameter(pPropertyRequest->UsedTime);
		m_AccountsDBModule->AddParameter(pPropertyRequest->szMachineID);

		//�������
		m_AccountsDBModule->AddParameterOutput("@strErrorDescribe");
		m_AccountsDBModule->AddParameterOutput("@nRet");

		BOOL bReturn = m_AccountsDBModule->ExecuteProcess("GSP_GR_ConsumeProperty");

		ModifyUserPropNumFromRedis(pPropertyRequest->dwDBID, pPropertyRequest->dwSourceUserID, pPropertyRequest->wPropertyIndex, 0 - pPropertyRequest->wItemCount);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchSignup)) return false;

	//��������
	DBR_GR_MatchSignup * pMatchSignup=(DBR_GR_MatchSignup *)pData;
	dwUserID=pMatchSignup->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchSignup->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchSignup->lMatchFee);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchSignup->szMachineID);

		//�������
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//�������
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserMatchFee");

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		if (bReturn)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount() == 0)
			{
				m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
				SQLRecordset rsRet(m_TreasureDBModule);

				//���ñ���
				MatchSignupResult.bResultCode = FALSE;

				//��ȡ��Ϣ
				rsRet.GetValue_String("@strErrorDescribe", MatchSignupResult.szDescribeString, CountArray(MatchSignupResult.szDescribeString));
			}
			else
			{
				//���ñ���
				MatchSignupResult.bResultCode = TRUE;
				MatchSignupResult.lCurrGold = rs.GetValue_LONGLONG("Score");
				MatchSignupResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
			}
		}

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���		
		MatchSignupResult.bResultCode=false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(MatchSignupResult.szDescribeString));

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}

	return true;
}

//�˳�����
bool CDataBaseEngineSink::OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchSignup)) return false;

	//��������
	DBR_GR_MatchSignup * pMatchSignup=(DBR_GR_MatchSignup *)pData;
	dwUserID=pMatchSignup->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchSignup->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchSignup->lMatchFee);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchSignup->szMachineID);

		//�������
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//�������
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserMatchQuit");

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		if (bReturn)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount() == 0)
			{
				m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
				SQLRecordset rsRet(m_TreasureDBModule);

				//���ñ���
				MatchSignupResult.bResultCode = FALSE;

				//��ȡ��Ϣ
				rsRet.GetValue_String("@strErrorDescribe", MatchSignupResult.szDescribeString, CountArray(MatchSignupResult.szDescribeString));
			}
			else
			{
				//���ñ���
				MatchSignupResult.bResultCode = TRUE;

				MatchSignupResult.lCurrGold = rs.GetValue_LONGLONG("Score");
				MatchSignupResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
			}
		}

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_UNSIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//���ñ���
		MatchSignupResult.bResultCode=false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),CountArray(MatchSignupResult.szDescribeString));

		//���ͽ��
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}
}

//������ʼ
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//��������
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;

	//������
	try
	{
		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchStart->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchStart->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchStart->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchStart->cbMatchType);

		//�������
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchStart");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		SQLRecordset rs;
		OnInsureDisposeResult(dwContextID,rs, DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//��������
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;

	//������
	try
	{
		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchOver->wServerID);
		m_TreasureDBModule->AddParameter(pMatchOver->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchOver->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchOver->cbMatchType);
		m_TreasureDBModule->AddParameter(pMatchOver->MatchStartTime);
		m_TreasureDBModule->AddParameter(pMatchOver->MatchEndTime);

		//�������
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchOver");

		//ִ�гɹ�
		if(bReturn)
		{
			//����ṹ
			DBO_GR_MatchRankList MatchRankList;
			ZeroMemory(&MatchRankList,sizeof(MatchRankList));

			//��������
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//���ñ���
			MatchRankList.dwMatchID=pMatchOver->dwMatchID;
			MatchRankList.dwMatchNO=pMatchOver->dwMatchNO;

			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount() > 0)
			{
				//��ȡ��¼
				do
				{
					pMatchRankInfo = &MatchRankList.MatchRankInfo[MatchRankList.wUserCount++];
					pMatchRankInfo->wRankID = rs.GetValue_WORD("RankID");
					pMatchRankInfo->dwUserID = rs.GetValue_DWORD("UserID");
					pMatchRankInfo->lMatchScore = rs.GetValue_LONGLONG("Score");
					pMatchRankInfo->lRewardGold = rs.GetValue_LONGLONG("RewardGold");
					pMatchRankInfo->dwRewardIngot = rs.GetValue_DWORD("RewardMedal");
					pMatchRankInfo->dwRewardExperience = rs.GetValue_DWORD("RewardExperience");
				} while (rs.Fetch());
			}

			//��������
			WORD wSendDataSize = sizeof(MatchRankList)-sizeof(MatchRankList.MatchRankInfo);
			wSendDataSize += MatchRankList.wUserCount*sizeof(MatchRankList.MatchRankInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_RANK_LIST,dwContextID,&MatchRankList,wSendDataSize);
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//��������
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=pMatchReward->dwUserID;

	//������
	try
	{
		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchReward->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchReward->lRewardGold);
		m_TreasureDBModule->AddParameter(pMatchReward->dwRewardIngot);
		m_TreasureDBModule->AddParameter(pMatchReward->dwRewardExperience);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);

		//����ṹ
		DBR_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchReward");

		//�������
		if ( bReturn )
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount() == 0)
			{
				//���ñ���
				MatchRewardResult.bResultCode = FALSE;
			}
			else
			{
				//���ñ���
				MatchRewardResult.bResultCode = TRUE;

				//��ȡ�Ƹ�
				MatchRewardResult.lCurrGold = rs.GetValue_LONGLONG("Score");
				MatchRewardResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
				MatchRewardResult.dwCurrExperience = rs.GetValue_DWORD("Experience");
			}
		}
		else
		{
			//���ñ���
			MatchRewardResult.bResultCode = FALSE;
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//����ṹ
		DBR_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		//���ñ���
		MatchRewardResult.bResultCode=false;

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return false;
	}

	return true;
}

//������̭
bool CDataBaseEngineSink::OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchEliminate));
	if (wDataSize!=sizeof(DBR_GR_MatchEliminate)) return false;

	//��������
	DBR_GR_MatchEliminate * pMatchEliminate=(DBR_GR_MatchEliminate *)pData;

	//������
	try
	{
		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchEliminate->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchEliminate->wServerID);
		m_TreasureDBModule->AddParameter(pMatchEliminate->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchEliminate->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchEliminate->cbMatchType);

		//�������
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchEliminate");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//ϵͳ��Ϣ
bool CDataBaseEngineSink::OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//�������
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_LoadSystemMessage");

		//�������
		if (bReturn)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount()>0)
			{
				do
				{
					//�������
					TCHAR szServerRange[1024] = { 0 };
					CString strServerRange;
					bool bSendMessage = false;
					bool bAllRoom = false;

					//��ȡ��Χ
					rs.GetValue_String("ServerRange", szServerRange, CountArray(szServerRange));
					szServerRange[1023] = 0;
					strServerRange.Format(TEXT("%s"), szServerRange);

					//��Χ�ж�
					while (true)
					{
						int nfind = strServerRange.Find(TEXT(','));
						if (nfind != -1 && nfind > 0)
						{
							CString strID = strServerRange.Left(nfind);
							WORD wServerID = StrToInt(strID);
							bSendMessage = (wServerID == 0 || wServerID == m_pGameServiceOption->wServerID);
							if (wServerID == 0)bAllRoom = true;

							if (bSendMessage) break;

							strServerRange = strServerRange.Right(strServerRange.GetLength() - nfind - 1);
						}
						else
						{
							WORD wServerID = StrToInt(szServerRange);
							bSendMessage = (wServerID == 0 || wServerID == m_pGameServiceOption->wServerID);
							if (wServerID == 0)bAllRoom = true;

							break;
						}
					}

					//������Ϣ
					if (bSendMessage)
					{
						//�������
						DBR_GR_SystemMessage SystemMessage;
						ZeroMemory(&SystemMessage, sizeof(SystemMessage));

						//��ȡ��Ϣ
						SystemMessage.dwMessageID = rs.GetValue_DWORD("ID");
						SystemMessage.cbMessageType = rs.GetValue_BYTE("MessageType");
						SystemMessage.dwTimeRate = rs.GetValue_DWORD("TimeRate");
						SystemMessage.cbAllRoom = bAllRoom ? TRUE : FALSE;
						rs.GetValue_String("MessageString", SystemMessage.szSystemMessage, CountArray(SystemMessage.szSystemMessage));

						//��ȡʱ��
						SYSTEMTIME systime;
						ZeroMemory(&systime, sizeof(systime));

						//��ʼʱ��
						rs.GetValue_SystemTime("StartTime", systime);
						CTime StarTime(systime);
						SystemMessage.tStartTime = StarTime.GetTime();

						//����ʱ��
						rs.GetValue_SystemTime("ConcludeTime", systime);
						CTime ConcludeTime(systime);
						SystemMessage.tConcludeTime = ConcludeTime.GetTime();

						//���ͽ��
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_RESULT, dwContextID, &SystemMessage, sizeof(SystemMessage));
					}
				} while (rs.Fetch());
			}

			//�������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_FINISH,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		SQLRecordset rs;
		OnInsureDisposeResult(dwContextID,rs,DB_ERROR,0L,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::UnLockAndroidUser()
{
	try
	{
		//�������
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBModule->ExecuteProcess("GSP_GR_UnlockAndroidUser");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
	}

	return false;
}

//��¼���
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, LPCTSTR lpsPassword, ValueMap& vMap)
{
	memset(&m_LogonSuccess, 0, sizeof(DBO_GR_LogonSuccess));
	lstrcpyn(m_LogonSuccess.szPassword, CYSCharUtils::UTF8ToCString(vMap["Password"].c_str()), CountArray(m_LogonSuccess.szPassword));
	if (_tcscmp(m_LogonSuccess.szPassword, lpsPassword) == 0)
	{
		m_LogonSuccess.dwUserID = CYSService::ToDword(vMap["UserID"]);
		m_LogonSuccess.lGold = CYSService::ToLong(vMap["Gold"]);
		m_LogonSuccess.wFaceID = CYSService::ToWord(vMap["FaceID"]);
		m_LogonSuccess.cbGender = CYSService::ToByte(vMap["Gender"]);
		m_LogonSuccess.lDiamond = CYSService::ToScore(vMap["Diamond"]);
		m_LogonSuccess.dwExperience = CYSService::ToDword(vMap["Experience"]);
		m_LogonSuccess.cbMemberOrder = CYSService::ToByte(vMap["MemberOrder"]);

		m_LogonSuccess.dwWinCount = CYSService::ToDword(vMap["WinCount"]);
		m_LogonSuccess.dwLostCount = CYSService::ToDword(vMap["LostCount"]);
		m_LogonSuccess.dwDrawCount = CYSService::ToDword(vMap["DrawCount"]);
		m_LogonSuccess.dwFleeCount = CYSService::ToDword(vMap["FleeCount"]);
		m_LogonSuccess.lLoveLiness = CYSService::ToLong(vMap["LoveLiness"]);

		m_LogonSuccess.dwCustomFaceVer = CYSService::ToDword(vMap["CustomFaceVer"]);

		//�ȼý�
		m_LogonSuccess.cbBenefitTimes = CYSService::ToByte(vMap["BenefitTimes"]);
		CString strTime = CYSCharUtils::UTF8ToCString(vMap["LastBenefitTime"].c_str());
		CYSService::StringToSystemTime(strTime, m_LogonSuccess.LastBenefitTime);

		m_LogonSuccess.cbMaxCardType = CYSService::ToByte(vMap["MaxCardType"]);

		CString strMaxCardData = CYSCharUtils::UTF8ToCString(vMap["MaxCardData"].c_str());
		CString	strSub;
		int nCount = 0;
		while (AfxExtractSubString(strSub, strMaxCardData, nCount, _T(',')))
		{
			if (strSub.IsEmpty()) continue;
			m_LogonSuccess.aryCardData[nCount++] = _ttoi(strSub.GetBuffer());

			if (nCount>=MAX_CARD_DATA_COUNT)
			{
				break;
			}
		}

		lstrcpyn(m_LogonSuccess.szNickName, CYSCharUtils::UTF8ToCString(vMap["NickName"].c_str()), CountArray(m_LogonSuccess.szNickName));

		BYTE isAndroid = CYSService::ToByte(vMap["IsAndroid"]);
		//�������
		m_LogonSuccess.wTaskCount = 0;
		//���ǻ�����
		if (isAndroid == 0)
		{
			GetUserTaskInfo(dwContextID, m_pGameServiceOption->wKindID, m_LogonSuccess);

			//GetUserPropFromRedis(dwContextID, m_LogonSuccess.dwUserID);
		}

		//���ͽ��
		WORD wDataSize = sizeof(m_LogonSuccess.UserTaskInfo[0])*m_LogonSuccess.wTaskCount;
		WORD wHeadSize = sizeof(m_LogonSuccess)-sizeof(m_LogonSuccess.UserTaskInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS, dwContextID, &m_LogonSuccess, wHeadSize + wDataSize);
	}
	else
	{
		//��������
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure, sizeof(LogonFailure));

		//��������
		LogonFailure.lResultCode = 1;
		lstrcpyn(LogonFailure.szDescribeString, _T("�������"), CountArray(LogonFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize = CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize = sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE, dwContextID, &LogonFailure, wHeadSize + wDataSize);
	}

}

//��¼���
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, SQLRecordset& inRs, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbDeviceType)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		m_LogonSuccess.wFaceID = inRs.GetValue_WORD("FaceID");
		m_LogonSuccess.dwUserID = inRs.GetValue_DWORD("UserID");
		m_LogonSuccess.dwCustomID = inRs.GetValue_DWORD("CustomID");
		inRs.GetValue_String("NickName", m_LogonSuccess.szNickName, CountArray(m_LogonSuccess.szNickName));

		//�û�����
		m_LogonSuccess.cbGender = inRs.GetValue_BYTE("Gender");
		m_LogonSuccess.cbMemberOrder = inRs.GetValue_BYTE("MemberOrder");
		inRs.GetValue_String("UnderWrite", m_LogonSuccess.szUnderWrite, CountArray(m_LogonSuccess.szUnderWrite));

		//������Ϣ
		m_LogonSuccess.lGold = inRs.GetValue_LONGLONG("Gold");
		m_LogonSuccess.lDiamond = inRs.GetValue_LONG("Diamond");
		m_LogonSuccess.lInsure = inRs.GetValue_LONGLONG("Insure");

		//������Ϣ
		m_LogonSuccess.dwWinCount = inRs.GetValue_LONG("WinCount");
		m_LogonSuccess.dwLostCount = inRs.GetValue_LONG("LostCount");
		m_LogonSuccess.dwDrawCount = inRs.GetValue_LONG("DrawCount");
		m_LogonSuccess.dwFleeCount = inRs.GetValue_LONG("FleeCount");
		m_LogonSuccess.dwExperience = inRs.GetValue_LONG("Experience");
		m_LogonSuccess.lLoveLiness = inRs.GetValue_LONG("LoveLiness");

		m_LogonSuccess.dwCustomFaceVer = inRs.GetValue_DWORD("CustomFaceVer");

		DWORD dwPlayTimeCount = inRs.GetValue_LONG("PlayTimeCount");
		DWORD dwOnLineTimeCount = inRs.GetValue_LONG("OnLineTimeCount");

		//�������
		m_LogonSuccess.cbMaxCardType = inRs.GetValue_BYTE("MaxCardType");

		TCHAR szMaxCardData[32] = { 0 };
		inRs.GetValue_String("MaxCardData", szMaxCardData, sizeof(szMaxCardData));

		CString	strSub;
		int nCount = 0;
		while (AfxExtractSubString(strSub, szMaxCardData, nCount, _T(',')))
		{
			if (strSub.IsEmpty()) continue;
			m_LogonSuccess.aryCardData[nCount++] = _ttoi(strSub.GetBuffer());

			if (nCount >= MAX_CARD_DATA_COUNT)
			{
				break;
			}
		}

		//������Ϣ		
		m_LogonSuccess.dwUserRight = inRs.GetValue_DWORD("UserRight");
		m_LogonSuccess.cbDeviceType=cbDeviceType;

		//�ȼý�
		m_LogonSuccess.cbBenefitTimes = inRs.GetValue_BYTE("BenefitTimes");
		TCHAR szLastBenefitTime[32] = {0};
		inRs.GetValue_String("LastBenefitTime", szLastBenefitTime, sizeof(szLastBenefitTime));
		inRs.GetValue_SystemTime_String("LastBenefitTime", m_LogonSuccess.LastBenefitTime);

		BYTE isAndroid = inRs.GetValue_BYTE("isAndroid");

		//��������
		m_LogonSuccess.dwInoutIndex = inRs.GetValue_DWORD("InoutIndex");

		//��ȡ��Ϣ
		if (pszErrorString != NULL)
		{
			lstrcpyn(m_LogonSuccess.szDescribeString, pszErrorString, CountArray(m_LogonSuccess.szDescribeString));
		}

		//�������
		m_LogonSuccess.wTaskCount=0;
		ZeroMemory(m_LogonSuccess.UserTaskInfo,sizeof(m_LogonSuccess.UserTaskInfo));

		//���ǻ�����
		if (isAndroid == 0)
		{
			GetUserTaskInfo(dwContextID, m_pGameServiceOption->wKindID, m_LogonSuccess);
		}

		//���ͽ��
		WORD wDataSize = sizeof(m_LogonSuccess.UserTaskInfo[0])*m_LogonSuccess.wTaskCount;
		WORD wHeadSize = sizeof(m_LogonSuccess) - sizeof(m_LogonSuccess.UserTaskInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS, dwContextID, &m_LogonSuccess, wHeadSize + wDataSize);

#ifdef USE_REDIS
		try
		{
			//���»���
			ASSERT(m_RedisInstance.isConneced());
			if (m_RedisInstance.isConneced())
			{
				char szBuf[32] = { 0 };
				sprintf_s(szBuf, sizeof(szBuf), "id.%d", m_LogonSuccess.dwUserID);
				string strKey(szBuf);

				int64_t ret = 0;
				ValueMap vMap;
				vMap["Gold"] = CYSService::ToString(m_LogonSuccess.lGold);
				vMap["UserID"] = CYSService::ToString(m_LogonSuccess.dwUserID);
				vMap["FaceID"] = CYSService::ToString(m_LogonSuccess.wFaceID);
				vMap["Gender"] = CYSService::ToString(m_LogonSuccess.cbGender);
				vMap["Diamond"] = CYSService::ToString(m_LogonSuccess.lDiamond);
				vMap["MemberOrder"] = CYSService::ToString(m_LogonSuccess.cbMemberOrder);
				vMap["IsAndroid"] = CYSService::ToString(isAndroid);

				vMap["Grade"] = CYSCharUtils::ConvertToUTF8(TEXT("����")).GetBuffer();
				vMap["FriendNum"] = CYSService::ToString((BYTE)0);

				vMap["Experience"] = CYSService::ToString(m_LogonSuccess.dwExperience);
				vMap["WinRate"] = "0";

				vMap["WinCount"] = CYSService::ToString(m_LogonSuccess.dwWinCount);
				vMap["LostCount"] = CYSService::ToString(m_LogonSuccess.dwLostCount);
				vMap["DrawCount"] = CYSService::ToString(m_LogonSuccess.dwDrawCount);
				vMap["FleeCount"] = CYSService::ToString(m_LogonSuccess.dwFleeCount);
				vMap["LoveLiness"] = CYSService::ToString(m_LogonSuccess.lLoveLiness);

				vMap["CustomFaceVer"] = CYSService::ToString(m_LogonSuccess.dwCustomFaceVer);

				vMap["PlayTimeCount"] = CYSService::ToString(dwPlayTimeCount);
				vMap["OnLineTimeCount"] = CYSService::ToString(dwOnLineTimeCount);
				vMap["BenefitTimes"] = CYSService::ToString(m_LogonSuccess.cbBenefitTimes);

				vMap["LastBenefitTime"] = CYSCharUtils::ConvertToUTF8(szLastBenefitTime).GetBuffer();

				vMap["MaxCardType"] = CYSService::ToString(m_LogonSuccess.cbMaxCardType);
				vMap["MaxCardData"] = CYSCharUtils::ConvertToUTF8(szMaxCardData).GetBuffer();

				vMap["Accounts"] = CYSCharUtils::ConvertToUTF8(m_LogonSuccess.szNickName).GetBuffer();
				vMap["Password"] = CYSCharUtils::ConvertToUTF8(m_LogonSuccess.szPassword).GetBuffer();
				vMap["NickName"] = CYSCharUtils::ConvertToUTF8(m_LogonSuccess.szNickName).GetBuffer();

				CTime st = CTime::GetCurrentTime();
				vMap["LastTime"] = CYSCharUtils::ConvertToUTF8(st.Format(_T("%Y-%m-%d %H:%M:%S"))).GetBuffer();

				m_RedisInstance.hmset(strKey, vMap, ret);
			}
		}
		catch (...)
		{
			CTraceService::TraceString(_T("OnLogonDisposeResult redis error"), TraceLevel_Exception);
		}
#endif
	}
	else
	{
		//��������
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//��������
		LogonFailure.lResultCode=dwErrorCode;
		lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
	}

	return;
}

//�����ѯ
bool CDataBaseEngineSink::OnQueryTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess)
{
	try
	{
		//���ñ���
		DWORD dwUserID = m_LogonSuccess.dwUserID;

		//�������
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter((WORD)0);
		m_PlatformDBModule->AddParameter(dwUserID);
		m_PlatformDBModule->AddParameter(m_LogonSuccess.szPassword);

		//�������
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//ִ�нű�
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_QueryTaskInfo");

		//ִ�гɹ�
		if (bResult)
		{
			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);

			if (rs.GetCount() == 0)
			{
				return false;
			}
			else
			{
				//��������
				tagUserTaskInfo * pUserTaskInfo = NULL;

				//ѭ����ȡ
				do
				{
					//���ñ���
					pUserTaskInfo = &m_LogonSuccess.UserTaskInfo[m_LogonSuccess.wTaskCount++];

					//��ȡ����
					pUserTaskInfo->dwID = rs.GetValue_DWORD("ID");
					pUserTaskInfo->wTaskID = rs.GetValue_WORD("TaskID");
					pUserTaskInfo->cbTaskStatus = rs.GetValue_BYTE("TaskStatus");
					pUserTaskInfo->wTaskProgress = rs.GetValue_WORD("Progress");
					pUserTaskInfo->dwResidueTime = rs.GetValue_DWORD("ResidueTime");
					pUserTaskInfo->dwLastUpdateTime = (DWORD)time(NULL);
				} while (rs.Fetch());

				return true;
			}
		}
			
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed = false;
		TaskResult.wCommandID = SUB_GR_TASK_LOAD_INFO;
		lstrcpyn(TaskResult.szNotifyContent, TEXT("�������ݿ�����쳣�������Ժ����ԣ�"), CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize = sizeof(TaskResult) - sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);

		return false;
	}

	return true;
}

//��ʼ������
bool CDataBaseEngineSink::InitUserTask(DWORD dwContextID, DBO_GR_LogonSuccess m_LogonSuccess)
{
	try
	{
		DBR_GR_TaskTake TaskTake;

		//���һ���
		for (int i = 0; i < m_wArrDailyTaskID.GetCount(); i++)
		{
			ZeroMemory(&TaskTake, sizeof(TaskTake));

			TaskTake.dwUserID = m_LogonSuccess.dwUserID;
			TaskTake.wTaskID = m_wArrDailyTaskID[i];
			CopyMemory(TaskTake.szPassword, m_LogonSuccess.szPassword, sizeof(TaskTake.szPassword));
			CopyMemory(TaskTake.szMachineID, m_LogonSuccess.szMachineID, sizeof(TaskTake.szMachineID));

			//�������
			m_PlatformDBModule->ClearParameters();
			m_PlatformDBModule->AddParameter(TaskTake.dwUserID);
			m_PlatformDBModule->AddParameter(TaskTake.wTaskID);
			m_PlatformDBModule->AddParameter(TaskTake.szPassword);
			m_PlatformDBModule->AddParameter("");
			m_PlatformDBModule->AddParameter(TaskTake.szMachineID);
			m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
			m_PlatformDBModule->AddParameterOutput("@nRet");

			//ִ�нű�
			BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_TaskTake");
			if (!bResult)
			{
				CString str;
				str.Format(TEXT("��ʼ����� %d �ճ����� %d ʧ�ܣ�"), TaskTake.dwUserID, TaskTake.wTaskID);
				CTraceService::TraceString(str, TraceLevel_Exception);
			}
		} 

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//����ṹ
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed = false;
		TaskResult.wCommandID = SUB_GR_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent, TEXT("�������ݿ�����쳣�������Ժ����ԣ�"), CountArray(TaskResult.szNotifyContent));

		//���ͽ��
		WORD wSendSize = sizeof(TaskResult) - sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);

		return false;
	}

	return true;
}

//���н��
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, SQLRecordset& inRs, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbActivityGame)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//��������
		DBO_GR_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//�������
		UserInsureSuccess.cbActivityGame=cbActivityGame;
		UserInsureSuccess.lFrozenedScore=lFrozenedScore;
		UserInsureSuccess.dwUserID = inRs.GetValue_DWORD("UserID");
		UserInsureSuccess.lSourceScore = inRs.GetValue_LONGLONG("SourceScore");
		UserInsureSuccess.lSourceInsure = inRs.GetValue_LONGLONG("SourceInsure");
		UserInsureSuccess.lInsureRevenue = inRs.GetValue_LONGLONG("InsureRevenue");
		UserInsureSuccess.lVariationScore = inRs.GetValue_LONGLONG("VariationScore");
		UserInsureSuccess.lVariationInsure = inRs.GetValue_LONGLONG("VariationInsure");
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//��������
		DBO_GR_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//�������
		UserInsureFailure.cbActivityGame=cbActivityGame;
		UserInsureFailure.lResultCode=dwErrorCode;
		UserInsureFailure.lFrozenedScore=lFrozenedScore;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//���ͽ��
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//���»�������
void CDataBaseEngineSink::UpdateCachedData(DWORD dwUserID, tagVariationInfo& VariationInfo, BYTE cbCardType, LPCTSTR pszCardData)
{
#ifdef USE_REDIS

	try
	{
		//���뻺��
		ASSERT(m_RedisInstance.isConneced());
		if (m_RedisInstance.isConneced())
		{
			int64_t ret = 0;
			//�齨KEY
			char szBuf[32] = { 0 };
			sprintf_s(szBuf, sizeof(szBuf), "id.%d", dwUserID);
			string strKey(szBuf);

			//��ȡHASH��
			ValueMap vMap;
			if (m_RedisInstance.hgetall(strKey, vMap))
			{
				if (VariationInfo.lScore != 0)
				{
					//���»��ֻ���
					m_RedisInstance.hincrby(strKey, "Gold", (int)VariationInfo.lScore, ret);
				}

				if (VariationInfo.dwExperience!=0)
				{
					//���¾��黺��
					m_RedisInstance.hincrby(strKey, "Experience", (int)VariationInfo.dwExperience, ret);
				}

				if (VariationInfo.dwWinCount!=0)
				{
					//����ʤ������
					m_RedisInstance.hincrby(strKey, "WinCount", (int)VariationInfo.dwWinCount, ret);
				}

				if (VariationInfo.dwLostCount!=0)
				{
					//����ʧ�ܾ���
					m_RedisInstance.hincrby(strKey, "LostCount", (int)VariationInfo.dwLostCount, ret);
				}

				if (VariationInfo.dwDrawCount!=0)
				{
					//����ƽ�־���
					m_RedisInstance.hincrby(strKey, "DrawCount", (int)VariationInfo.dwDrawCount, ret);
				}

				if (VariationInfo.dwFleeCount!=0)
				{
					//���·�������
					m_RedisInstance.hincrby(strKey, "FleeCount", (int)VariationInfo.dwFleeCount, ret);
				}

				if (VariationInfo.lLoveLiness!=0)
				{
					//����������Ϣ
					m_RedisInstance.hincrby(strKey, "LoveLiness", VariationInfo.lLoveLiness, ret);
				}

				if (VariationInfo.dwPlayTimeCount!=0)
				{
					//��Ϸʱ��
					m_RedisInstance.hincrby(strKey, "PlayTimeCount", (int)VariationInfo.dwPlayTimeCount, ret);
				}

				ValueMap vNewMap;
				if (cbCardType > 0)
				{
					vNewMap["MaxCardType"] = CYSService::ToString(cbCardType);
					vNewMap["MaxCardData"] = CYSCharUtils::ConvertToUTF8(pszCardData).GetBuffer();
				}

				//if (VariationInfo.dwPlayTimeCount != 0)
				//{
				//	//����ʱ��
				//	m_RedisInstance.hincrby(strKey, "OnLineTimeCount", (int)VariationInfo.dwOnLineTimeCount);
				//}

				CTime st = CTime::GetCurrentTime();
				vNewMap["LastTime"] = CYSCharUtils::ConvertToUTF8(st.Format(_T("%Y-%m-%d %H:%M:%S"))).GetBuffer();

				m_RedisInstance.hmset(strKey, vNewMap, ret);
			}
		}
	}
	catch (...)
	{
		CTraceService::TraceString(_T("UpdateCachedData error"), TraceLevel_Exception);
	}

#endif
}

// �����ʼ�
bool CDataBaseEngineSink::OnRequestSendEMail(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_EMAIL * pEMail = (DBR_GR_EMAIL *)pData;
	dwUserID = pEMail->dwRecUserID;

	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_EMAIL));
		if (wDataSize != sizeof(DBR_GR_EMAIL)) return false;

		//�������
		m_AccountsDBModule->ClearParameters();

		//�û���Ϣ
		m_AccountsDBModule->AddParameter(pEMail->dwRecUserID);
		m_AccountsDBModule->AddParameter(pEMail->cbType);
		m_AccountsDBModule->AddParameter(pEMail->createTime);
		m_AccountsDBModule->AddParameter(pEMail->szRewardProp);
		m_AccountsDBModule->AddParameter(pEMail->szEmailTitle);
		m_AccountsDBModule->AddParameter(pEMail->szEmailMsg);
		
		//�������
		m_AccountsDBModule->AddParameterOutput("@strErrorDescribe");
		m_AccountsDBModule->AddParameterOutput("@nRet");

		//ִ�в�ѯ
		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_WriteEMail");

		//���³ɹ�֪ͨ�������
		if (bResult && pEMail->cbType == EN_EMAIL_SYSTIME)
		{
			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);

			if (rs.GetCount() > 0)
			{
				tagSendBenefitSuc Benefit;
				ZeroMemory(&Benefit, sizeof(tagSendBenefitSuc));

				//��ȡ����
				Benefit.dwUserID = rs.GetValue_DWORD("UserID");
				Benefit.cbBenefitTimes = rs.GetValue_WORD("BenefitTimes");
				rs.GetValue_SystemTime_String("LastBenefitTime", Benefit.LastBenefitTime);
				TCHAR szLastBenefitTime[32] = { 0 };
				rs.GetValue_String("LastBenefitTime", szLastBenefitTime, sizeof(szLastBenefitTime));

				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_SEND_EMAIL_SUCCESS, dwContextID, &Benefit, sizeof(tagSendBenefitSuc));

				try
				{
					//���»���
					ASSERT(m_RedisInstance.isConneced());
					if (m_RedisInstance.isConneced())
					{
						char szBuf[32] = { 0 };
						sprintf_s(szBuf, sizeof(szBuf), "id.%d", Benefit.dwUserID);
						string strKey(szBuf);

						int64_t ret = 0;
						ValueMap vMap;

						vMap["BenefitTimes"] = CYSService::ToString(Benefit.cbBenefitTimes);
						vMap["LastBenefitTime"] = CYSCharUtils::ConvertToUTF8(szLastBenefitTime).GetBuffer();

						CTime st = CTime::GetCurrentTime();
						vMap["LastTime"] = CYSCharUtils::ConvertToUTF8(st.Format(_T("%Y-%m-%d %H:%M:%S"))).GetBuffer();

						m_RedisInstance.hmset(strKey, vMap, ret);

					}
				}
				catch (...)
				{
					CTraceService::TraceString(_T("OnRequestSendEMail error"), TraceLevel_Exception);
				}
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//ˢ����������
bool CDataBaseEngineSink::OnRequestRefreshOnlineUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
#ifdef USE_REDIS
	//���»���
	ASSERT(m_RedisInstance.isConneced());
	if (m_RedisInstance.isConneced())
	{
		int64_t nRetValue = 0;
		DBR_GR_RefreshOnlineUser* pOnline = (DBR_GR_RefreshOnlineUser*)pData;

		if (m_RedisInstance.exists(pOnline->szKey))
		{
			m_RedisInstance.hset(pOnline->szKey, "OnlineUserNum", pOnline->szUserCount, nRetValue);
		}
		else
		{
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_REREG_SERVER_IN_REDIS, dwContextID, NULL, 0);
		}
	}
#endif
	return true;
}

//��ȡ�������
void CDataBaseEngineSink::GetUserTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess)
{
	bool isQuery = OnQueryTaskInfo(dwContextID, wKindID, m_LogonSuccess);
	if (isQuery == false && m_LogonSuccess.wTaskCount <= 0)
	{
		// �����ճ�����;
		InitUserTask(dwContextID, m_LogonSuccess);

		// ���²�ѯ����;
		OnQueryTaskInfo(dwContextID, wKindID, m_LogonSuccess);
	}
}

//�ӻ����л�ȡ����
void CDataBaseEngineSink::GetUserPropFromRedis(DWORD dwContextID, DWORD dwUserID)
{
	char szBuf[64] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf), "UserPropertyList.%d", dwUserID);

	//�ȴӻ����ж�ȡ��ҵ����б�
	ValueList list;
	m_RedisInstance.lrange(szBuf, 0, -1, list);

	if (list.size() > 0)
	{
		//��������
		tagUserProperty prop;
		BYTE cbDataBuffer[SOCKET_TCP_PACKET - 1024] = { 0 };
		DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)cbDataBuffer;
		LPBYTE pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);

		//���ñ���
		WORD wPropertyCount = 0;
		WORD wSendDataSize = sizeof(DBO_GR_PropertyListInfo);

		//�����б��ȡÿ�����߶�Ӧ��HASHֵ
		for (ValueList::iterator it = list.begin(); it != list.end(); it++)
		{
			ValueMap v;
			bool bRet = m_RedisInstance.hgetall(*it, v);
			if (bRet && (v.size() > 0))
			{
				ZeroMemory(&prop, sizeof(tagUserProperty));
				
				//��ȡ����
				prop.dwDBID = CYSService::ToDword(v["ID"]);
				prop.wPropertyUseMark = CYSService::ToWord(v["PropID"]);
				prop.wPropertyCount = CYSService::ToWord(v["Count"]);
				if ( v["ValidTime"].compare("0")!=0 )
				{
					COleDateTime odt;
					odt.ParseDateTime(CYSCharUtils::UTF8ToCString(v["ValidTime"].c_str()));

					if (odt.m_status == COleDateTime::valid)
					{
						VariantTimeToSystemTime(odt, &prop.OverTime);
					}
				}

				//�����ж�				
				if (wSendDataSize + sizeof(DBO_GR_PropertyListInfo) > sizeof(cbDataBuffer))
				{
					//���ñ���
					pPropertyListInfo->wPropertyCount = wPropertyCount;

					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);

					//���ñ���
					wPropertyCount = 0;
					wSendDataSize = sizeof(DBO_GR_PropertyListInfo);
					pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);
				}

				//��������
				CopyMemory(pDataBuffer, &prop, sizeof(tagUserProperty));

				//���ñ���
				wPropertyCount++;
				wSendDataSize += sizeof(tagUserProperty);
				pDataBuffer += sizeof(tagUserProperty);
			}
		}

		//ʣ�෢��
		if (wPropertyCount > 0 && wSendDataSize > 0)
		{
			//���ñ���
			pPropertyListInfo->wPropertyCount = wPropertyCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);
		}
	}
}

//�޸Ļ�������ҵ�������
void CDataBaseEngineSink::ModifyUserPropNumFromRedis(DWORD dwDBID, DWORD dwUserID, WORD wPropID, int nChangeValue)
{
	char szBuf[64] = { 0 };
	//sprintf_s(szBuf, sizeof(szBuf), "UserPropertyList.%d", dwUserID);

	////�ȴӻ����ж�ȡ��ҵ����б�
	//ValueList list;
	//m_RedisInstance.lrange(szBuf, 0, -1, list);

	//memset(szBuf, 0, sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "UserProperty.%d", dwDBID);
	//ValueList::iterator it = find(list.begin(), list.end(), szBuf);
	//if (it == list.end())
	//{
	//	return;
	//}

	//��������
	int64_t ret = 0;
	m_RedisInstance.hincrby(szBuf, "Count", nChangeValue, ret);
}
//////////////////////////////////////////////////////////////////////////////////

bool CDataBaseEngineSink::OnRequestPrivateInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBR_GR_Private_Info));
	if (wDataSize != sizeof(DBR_GR_Private_Info)) return false;

	//��������
	DBR_GR_Private_Info * pPrivateInfo = (DBR_GR_Private_Info *)pData;


	//������
	try
	{
		//�������
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter( pPrivateInfo->wKindID );

		//ִ�в�ѯ
		BOOL bReturn = m_PlatformDBModule->ExecuteProcess("GSP_GR_LoadPrivateInfo");

		if (bReturn)
		{
			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);

			DBO_GR_Private_Info kPrivateInfo;
			ZeroMemory(&kPrivateInfo, sizeof(kPrivateInfo));

			if (rs.GetCount() == 0)
			{
				kPrivateInfo.cbDBExecSuccess = DB_ERROR;
				kPrivateInfo.wKindID		 = pPrivateInfo->wKindID;
			}
			else
			{
				kPrivateInfo.wKindID		= rs.GetValue_WORD(TEXT("KindID"));
				kPrivateInfo.lCostGold		= rs.GetValue_LONG(TEXT("CostGold"));
				kPrivateInfo.bPlayCout[0] = rs.GetValue_BYTE(TEXT("PlayCount1"));
				kPrivateInfo.bPlayCout[1] = rs.GetValue_BYTE(TEXT("PlayCount2"));
				kPrivateInfo.bPlayCout[2] = rs.GetValue_BYTE(TEXT("PlayCount3"));
				kPrivateInfo.bPlayCout[3] = rs.GetValue_BYTE(TEXT("PlayCount4"));

				kPrivateInfo.lPlayCost[0] = rs.GetValue_LONG(TEXT("PlayCost1"));
				kPrivateInfo.lPlayCost[1] = rs.GetValue_LONG(TEXT("PlayCost2"));
				kPrivateInfo.lPlayCost[2] = rs.GetValue_LONG(TEXT("PlayCost3"));
				kPrivateInfo.lPlayCost[3] = rs.GetValue_LONG(TEXT("PlayCost4"));
			}
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PRIVATE_INFO, dwContextID, &kPrivateInfo, sizeof(kPrivateInfo));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestCreatePrivate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//Ч�����;
	ASSERT(wDataSize == sizeof(DBR_GR_Create_Private));
	if (wDataSize != sizeof(DBR_GR_Create_Private)) return false;

	//��������;
	DBR_GR_Create_Private * pPrivateInfo = (DBR_GR_Create_Private *)pData;
	dwUserID = pPrivateInfo->dwUserID;

	//������;
	try
	{

		DBO_GR_CreatePrivateResult kCreatePrivate;
		ZeroMemory(&kCreatePrivate, sizeof(kCreatePrivate));

		kCreatePrivate.wAgaginTable = pPrivateInfo->dwAgaginTable;
		kCreatePrivate.bGameRuleIdex = pPrivateInfo->bGameRuleIdex;
		kCreatePrivate.bGameTypeIdex = pPrivateInfo->bGameTypeIdex;
		kCreatePrivate.bPlayCoutIdex = pPrivateInfo->bPlayCoutIdex;
		kCreatePrivate.cbRoomType = pPrivateInfo->cbRoomType;

		kCreatePrivate.cbEnterRoomScoreType = pPrivateInfo->cbEnterRoomScoreType;
		kCreatePrivate.lInitEnterRoomScore	= pPrivateInfo->lInitEnterRoomScore;
		kCreatePrivate.bSucess				= FALSE;

		//�������;
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter(pPrivateInfo->dwUserID);
		m_PlatformDBModule->AddParameter(pPrivateInfo->wKindID);
		m_PlatformDBModule->AddParameter(pPrivateInfo->dwCost);
		m_PlatformDBModule->AddParameter(pPrivateInfo->dwCostType);
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");

		BOOL bExceSuccess = m_PlatformDBModule->ExecuteProcess("GSP_GR_CreatePrivate");
		if (bExceSuccess)
		{
			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);

			if (rs.GetCount() == 0)
			{
				m_PlatformDBModule->ExecuteSQL("select @strErrorDescribe;");
				SQLRecordset rsRet(m_PlatformDBModule);

				//���ñ���
				kCreatePrivate.bSucess = FALSE;
				//��ȡ��Ϣ
				rsRet.GetValue_String("@strErrorDescribe", kCreatePrivate.szDescribeString, CountArray(kCreatePrivate.szDescribeString));
			}
			else
			{
				//���ñ���;
				kCreatePrivate.bSucess = TRUE;
				kCreatePrivate.lCurSocre = rs.GetValue_LONGLONG("CurSocre");
			}
		}

		if (kCreatePrivate.bSucess && (pPrivateInfo->dwAgaginTable == INVALID_DWORD))
		{	//�����ɹ��������´�������ʱ,�����ݿ��в���յ�¼���¼�����¼;
			BYTE szTotalData[1] = { 0 };

			m_TreasureDBModule->ClearParameters();
			m_TreasureDBModule->AddParameter(0);
			m_TreasureDBModule->AddParameter((LPCTSTR)&szTotalData[0]);

			//ִ�в�ѯ;
			BOOL bExecSuccess = m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecord");

			//д���¼;
			if (bExecSuccess)
			{
				CleanupResults objCleanup(m_TreasureDBModule);
				SQLRecordset rs(m_TreasureDBModule);
				if (rs.GetCount() == 0)
				{
					//���ñ���;
					kCreatePrivate.bSucess = FALSE;
					TCHAR* szErrorDes = _T("ִ��[GSP_GR_PrivateGameRecord],���������.");
					lstrcpyn(kCreatePrivate.szDescribeString, szErrorDes, strlen(szErrorDes));
				}
				else
				{
					//��ȡ��ʶ;
					kCreatePrivate.dwRecordID = rs.GetValue_DWORD("RecordID");
				}
			}
			else
			{
				//���ñ���;
				kCreatePrivate.bSucess = FALSE;
				TCHAR* szErrorDes = _T("ִ��[GSP_GR_PrivateGameRecord]����.");
				lstrcpyn(kCreatePrivate.szDescribeString, szErrorDes, strlen(szErrorDes));
			}
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_PRIVATE, dwContextID, &kCreatePrivate, sizeof(kCreatePrivate));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//˽�˷����ѷ���
bool CDataBaseEngineSink::OnRequestCreatePrivateCost(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����;
	ASSERT(wDataSize == sizeof(DBR_GR_Create_Private_Cost));
	if (wDataSize != sizeof(DBR_GR_Create_Private_Cost)) return false;

	//��������;
	DBR_GR_Create_Private_Cost * pPrivateInfo = (DBR_GR_Create_Private_Cost *)pData;


	//������;
	try
	{
		//�������;
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter(pPrivateInfo->dwUserID);
		m_PlatformDBModule->AddParameter(pPrivateInfo->dwCost);
		m_PlatformDBModule->AddParameter(pPrivateInfo->dwCostType);

		BOOL bExceSuccess = m_PlatformDBModule->ExecuteProcess("GSP_GR_BackCreatePrivateCost");

		if (bExceSuccess)
			m_PlatformDBModule->CleanupResults();

		CString strDebugInfo;
		strDebugInfo.Format(_T("dwUserID[%d], wCostScore[%d], lReturnValue[%ld]"), pPrivateInfo->dwUserID, pPrivateInfo->dwCost, bExceSuccess);
		::OutputDebugString(strDebugInfo.GetBuffer());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}


bool CDataBaseEngineSink::OnRequestPrivateGameRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		dwUserID = INVALID_DWORD;

		DataStream kDataStream(pData, wDataSize);

		if (kDataStream.size() == 0)
		{
			return true;
		}
		std::vector<tagPrivateRandRecordChild>	kRecordChild;
		tagPrivateRandTotalRecord kTotalRecord;
		kTotalRecord.StreamValue(kDataStream, false);

		for (int i = 0; i < (int)kTotalRecord.kRecordChild.size(); i++)
		{
			kRecordChild.push_back(kTotalRecord.kRecordChild[i]);
		}
		kTotalRecord.kRecordChild.clear();

		DataStream kTempData;
		kTotalRecord.StreamValue(kTempData, true);


		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(kTotalRecord.iRecordID);
		m_TreasureDBModule->AddBinaryParameter((LPCTSTR)&kTempData[0], (int)kTempData.size());

		//ִ�в�ѯ;
		BOOL bResultCode = m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecord");

		//д���¼;
		if (bResultCode)
		{
			//д���¼;
			for (WORD i = 0; i < (WORD)kTotalRecord.kUserID.size(); i++)
			{
				//���ò���;
				m_TreasureDBModule->ClearParameters();

				//������Ϣ;
				m_TreasureDBModule->AddParameter(kTotalRecord.iRecordID);
				m_TreasureDBModule->AddParameter(kTotalRecord.kUserID[i]);
				m_TreasureDBModule->AddParameter(kTotalRecord.dwKindID);

				//ִ�в�ѯ;
				m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecordUserRecordID");
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


bool CDataBaseEngineSink::OnRequestSinglePrivateGameRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		dwUserID = INVALID_DWORD;

		DataStream kDataStream(pData, wDataSize);
		if (kDataStream.size() == 0)
		{
			return true;
		}

		tagPrivateRandRecordChild singleGameRecord;
		singleGameRecord.StreamValue(kDataStream, false);

		CString strUserScore = "";
		for (int i = 0; i < (int)singleGameRecord.kScore.size(); i++)
		{
			CString strTmp = "";

			if (!strUserScore.IsEmpty())
			{
				if (singleGameRecord.kScore[i] > 0)
				{
					strTmp.Format(",+%d", singleGameRecord.kScore[i]);
				}
				else
				{
					strTmp.Format(",%d", singleGameRecord.kScore[i]);
				}
			}
			else
			{
				if (singleGameRecord.kScore[i] > 0)
				{
					strTmp.Format("+%d", singleGameRecord.kScore[i]);
				}
				else
				{
					strTmp.Format("%d", singleGameRecord.kScore[i]);
				}
			}

			strUserScore += strTmp;
		}

		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(singleGameRecord.iRecordID);
		m_TreasureDBModule->AddBinaryParameter((LPCTSTR)strUserScore.GetBuffer(), strUserScore.GetLength());
		m_TreasureDBModule->AddBinaryParameter((LPCTSTR)pData, wDataSize);

		//ִ�в�ѯ;
		m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecordChild");

	}
	catch (IDataBaseException * pIException)
	{
		//�������;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


//˽�˷�����;
bool CDataBaseEngineSink::OnRequestPrivateGameEnd(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_GR_PrivateGameEnd));
	if (wDataSize != sizeof(DBO_GR_PrivateGameEnd)) return false;

	//��������
	DBO_GR_PrivateGameEnd * pGameEnd = (DBO_GR_PrivateGameEnd *)pData;

	try
	{
		dwUserID = INVALID_DWORD;

		m_TreasureDBModule->ClearParameters();

		//m_GameDBAide.AddParameter(TEXT("@dwUserID"), pGameEnd->dwUserID);
		//m_GameDBAide.AddParameter(TEXT("@lScore"), pGameEnd->lScore);
		//m_GameDBAide.AddParameter(TEXT("@wPlayCount"), pGameEnd->wPlayCount);
		//m_GameDBAide.AddParameter(TEXT("@dwCardLibIndex"), pGameEnd->dwCardLibIndex);
		//m_GameDBAide.AddParameter(TEXT("@wKindID"), pGameEnd->wKindID);
		//m_GameDBAide.AddParameter(TEXT("@wServerID"), pGameEnd->wServerID);

		////ִ�в�ѯ;
		//m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_PrivateGameEnd"), true);

	}
	catch (IDataBaseException * pIException)
	{
		//�������;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


