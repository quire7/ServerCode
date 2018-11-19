#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerInfoBuffer::CServerInfoBuffer()
{
}

//��������
CServerInfoBuffer::~CServerInfoBuffer()
{
	//��������
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//ɾ������
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//��������
bool CServerInfoBuffer::ResetServerInfo()
{
	//��������
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//ɾ������
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//ɾ������
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//��������
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//ɾ������
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//���ñ���
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//��������
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//Ч�����
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//�����ִ�
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//�����ж�
	if (pGameServerInsert==NULL)
	{
		//��������
		pGameServerInsert=CreateServerInfo();

		//����ж�
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//��ȡ��Ŀ
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//��������
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//��������
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//��������
	tagGameServerInfo * pGameServerInfo=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameServerInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameServerInfo=m_GameServerInfoArray[nArrayCount-1];
			m_GameServerInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameServerInfo=new tagGameServerInfo;
			if (pGameServerInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}


//////////////////////////////////////////////////////////////////////////////////

//���캯��
CMatchOptionBuffer::CMatchOptionBuffer()
{
}

//��������
CMatchOptionBuffer::~CMatchOptionBuffer()
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//ɾ������
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);
	for (INT_PTR i=0;i<m_GameMatchOptionBuffer.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionBuffer[i];
		SafeDelete(pGameMatchOption);
	}

	//ɾ������
	m_GameMatchOptionArray.RemoveAll();
	m_GameMatchOptionBuffer.RemoveAll();

	return;
}

//��������
bool CMatchOptionBuffer::ResetMatchOption()
{
	//��Ӷ���
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);

	//ɾ��Ԫ��
	m_GameMatchOptionArray.RemoveAll();

	return true;
}

//ɾ������
bool CMatchOptionBuffer::DeleteMatchOption(DWORD dwMatchID,DWORD wMatchNO)
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//���Ҷ���
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID && pGameMatchOption->dwMatchNO==wMatchNO)
		{
			//ɾ������
			m_GameMatchOptionArray.RemoveAt(i);
			m_GameMatchOptionBuffer.Add(pGameMatchOption);

			//���ñ���
			ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

			break;
		}
	}	

	return true;
}

//��������
bool CMatchOptionBuffer::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//Ч�����
	ASSERT(pGameMatchOption!=NULL);
	if (pGameMatchOption==NULL) return false;

	//�����ִ�
	tagGameMatchOption * pGameMatchInsert=SearchMatchOption(pGameMatchOption->dwMatchID,pGameMatchOption->dwMatchNO);

	//�����ж�
	if (pGameMatchInsert==NULL)
	{
		//��������
		pGameMatchInsert=CreateMatchOption();

		//����ж�
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameMatchOptionArray.Add(pGameMatchInsert);
	CopyMemory(pGameMatchInsert,pGameMatchOption,sizeof(tagGameMatchOption));

	return true;
}

//��ȡ��Ŀ
DWORD CMatchOptionBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_GameMatchOptionArray.GetCount());
}

//��������
tagGameMatchOption * CMatchOptionBuffer::SearchMatchOption(DWORD dwMatchID,DWORD wMatchNO)
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//���Ҷ���
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID && pGameMatchOption->dwMatchNO==wMatchNO)
		{
			return pGameMatchOption;
		}
	}

	return NULL;
}

//��������
tagGameMatchOption * CMatchOptionBuffer::CreateMatchOption()
{
	//��������
	tagGameMatchOption * pGameMatchOption=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameMatchOptionBuffer.GetCount();
		if (nArrayCount>0)
		{
			pGameMatchOption=m_GameMatchOptionBuffer[nArrayCount-1];
			m_GameMatchOptionBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameMatchOption=new tagGameMatchOption;
			if (pGameMatchOption==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchOption;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerInfoManager::CServerInfoManager()
{
}

//��������
CServerInfoManager::~CServerInfoManager()
{
}

//���ط���
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter((WORD)0);
		PlatformDBModule->AddParameter(szMachineID);

#ifdef _DEBUG
		CTraceService::TraceString(szMachineID, TraceLevel_Exception);
#endif

		//��ȡ�б�
		if (PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameRoomItem"))
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			//����б�
			ServerInfoBuffer.ResetServerInfo();

			//��ȡ�б�
			if (rs.GetCount()>0)
			{
				do
				{
					//��ȡ����
					tagGameServerInfo GameServerResult;
					ReadGameServerInfo(rs, GameServerResult);

					//�����б�
					ServerInfoBuffer.InsertServerInfo(&GameServerResult);
				} while (rs.Fetch());
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ط���
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//��ȡ�б�
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);
		PlatformDBModule->AddParameter(szMachineID);

		//��ȡ�б�
		if (PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameRoomItem"))
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			if (rs.GetCount() > 0)
			{
				//��ȡ����
				ReadGameServerInfo(rs, GameServerResult);
			}

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ؽ���
WORD CServerInfoManager::LoadGameMatchReward(DWORD dwMatchID,DWORD dwMatchNO)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(dwMatchID);
		PlatformDBModule->AddParameter(dwMatchNO);

		//��ȡ�б�
		if (PlatformDBModule->ExecuteProcess("GSP_GS_LoadMatchReward"))
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			return (WORD)rs.GetCount();
		}		

		return 0;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return 0;
	}

	return 0;
}

//���ر���
bool CServerInfoManager::LoadGameMatchOption(WORD wKindID,CMatchOptionBuffer & MatchOptionBuffer)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wKindID);
		PlatformDBModule->AddParameter((WORD)0);
		PlatformDBModule->AddParameter((WORD)0);

		BOOL bResult = PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameMatchItem");
		if (bResult==FALSE)
		{
			ASSERT(FALSE);
			return false;
		}

		CleanupResults objCleanup(PlatformDBModule);
		SQLRecordset rs(PlatformDBModule);

		//����б�
		MatchOptionBuffer.ResetMatchOption();

		if (rs.GetCount() > 0)
		{
			//��ȡ�б�
			do
			{
				//��ȡ����
				tagGameMatchOption GameMatchResult;
				ReadGameMatchOption(rs, GameMatchResult);

				//�����б�
				MatchOptionBuffer.InsertMatchOption(&GameMatchResult);
			} while (rs.Fetch());
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���ط���
bool CServerInfoManager::LoadGameMatchOption(WORD wKindID,DWORD dwMatchID,DWORD dwMatchNO,tagGameMatchOption & GameMatchResult)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//��ȡ�б�
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wKindID);
		PlatformDBModule->AddParameter(dwMatchID);
		PlatformDBModule->AddParameter(dwMatchNO);
		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//��ȡ�б�
		BOOL bResult = PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameMatchItem");
		if (bResult == FALSE)
		{
			ASSERT(FALSE);
			return false;
		}

		CleanupResults objCleanup(PlatformDBModule);
		SQLRecordset rs(PlatformDBModule, false);
		rs.FetchAll();

		PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
		SQLRecordset rsRet(PlatformDBModule);

		LONG lResultCode = rsRet.GetValue_LONG("@nRet");
		//��ȡ�б�
		if (lResultCode != DB_SUCCESS)
		{
			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			//��ȡ��Ϣ
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//��ȡ����
		if (rs.GetCount()>0)
		{
			ReadGameMatchOption(rs,GameMatchResult);
			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//ɾ������
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);
		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//ִ������
		BOOL bResult = PlatformDBModule->ExecuteProcess("GSP_GS_DeleteGameRoom");
		if (bResult == FALSE)
		{
			ASSERT(FALSE);
			return false;
		}

		CleanupResults objCleanup(PlatformDBModule);
		SQLRecordset rs(PlatformDBModule, false);
		rs.FetchAll();

		PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
		SQLRecordset rsRet(PlatformDBModule);

		LONG lResultCode = rsRet.GetValue_LONG("@nRet");

		//��ȡ�б�
		if (lResultCode != DB_SUCCESS)
		{
			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			//��ȡ��Ϣ
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//���뷿��
bool CServerInfoManager::InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//��չ����
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i = 0; i < CountArray(pGameServerCreate->cbCustomRule); i++)
		{
			_stprintf(&szCustomRule[i * 2], TEXT("%02X"), pGameServerCreate->cbCustomRule[i]);
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(pGameServerCreate->wGameID);
		PlatformDBModule->AddParameter(pGameServerCreate->wKindID);
		//PlatformDBModule->AddParameter(pGameServerCreate->wNodeID);
		PlatformDBModule->AddParameter(pGameServerCreate->wSortID);
		PlatformDBModule->AddParameter(pGameServerCreate->lCellScore);
		PlatformDBModule->AddParameter(pGameServerCreate->wRevenueRatio);
		PlatformDBModule->AddParameter(pGameServerCreate->lServiceScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lRestrictScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lMinTableScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lMinEnterScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lMaxEnterScore);

		PlatformDBModule->AddParameter(pGameServerCreate->lDefPutScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lLowPutScore);
		PlatformDBModule->AddParameter(pGameServerCreate->iExchangeRatio);

		PlatformDBModule->AddParameter(pGameServerCreate->lMagicExpScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lGratuityScore);
		PlatformDBModule->AddParameter(pGameServerCreate->dwWinExperience);
		PlatformDBModule->AddParameter(pGameServerCreate->dwFailExperience);

		PlatformDBModule->AddParameter(pGameServerCreate->cbMinEnterMember);
		PlatformDBModule->AddParameter(pGameServerCreate->cbMaxEnterMember);
		PlatformDBModule->AddParameter(pGameServerCreate->dwServerRule);
		PlatformDBModule->AddParameter(pGameServerCreate->dwAttachUserRight);
		PlatformDBModule->AddParameter(pGameServerCreate->wMaxPlayer);
		PlatformDBModule->AddParameter(pGameServerCreate->wTableCount);
		PlatformDBModule->AddParameter(pGameServerCreate->wServerPort);
		PlatformDBModule->AddParameter(pGameServerCreate->wServerKind);
		PlatformDBModule->AddParameter(pGameServerCreate->wServerType);
		PlatformDBModule->AddParameter(pGameServerCreate->szServerName);
		PlatformDBModule->AddParameter(pGameServerCreate->szServerPasswd);
		PlatformDBModule->AddParameter(pGameServerCreate->cbDistributeRule);
		PlatformDBModule->AddParameter(pGameServerCreate->wMinDistributeUser);
		PlatformDBModule->AddParameter(pGameServerCreate->wDistributeTimeSpace);
		PlatformDBModule->AddParameter(pGameServerCreate->wDistributeDrawCount);
		PlatformDBModule->AddParameter(pGameServerCreate->wMinPartakeGameUser);
		PlatformDBModule->AddParameter(pGameServerCreate->wMaxPartakeGameUser);
		//PlatformDBModule->AddParameter(pGameServerCreate->szDataBaseName);
		//PlatformDBModule->AddParameter(pGameServerCreate->szDataBaseAddr);

		////�����������ַ
		//PlatformDBModule->AddParameter(pGameServerCreate->szCacheServerAddr);
		////����������˿�
		//PlatformDBModule->AddParameter(pGameServerCreate->wCacheServerPort);

		PlatformDBModule->AddParameter(szCustomRule);
		PlatformDBModule->AddParameter(pGameServerCreate->szServiceMachine);

		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//ִ������
		BOOL bResult = PlatformDBModule->ExecuteProcess("GSP_GS_InsertGameRoom");
		if (bResult == FALSE)
		{
			ASSERT(FALSE);
			return false;
		}

		CleanupResults objCleanup(PlatformDBModule);
		SQLRecordset rs(PlatformDBModule, false);
		rs.FetchAll();

		PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
		SQLRecordset rsRet(PlatformDBModule);

		LONG lResultCode = rsRet.GetValue_LONG("@nRet");
		//��ȡ�б�
		if (lResultCode != DB_SUCCESS)
		{
			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			//��ȡ��Ϣ
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameServerInfo(rs,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//�޸ķ���
bool CServerInfoManager::ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//��չ����
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i = 0; i < CountArray(pGameServerCreate->cbCustomRule); i++)
		{
			_stprintf(&szCustomRule[i * 2], TEXT("%02X"), pGameServerCreate->cbCustomRule[i]);
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(pGameServerCreate->wServerID);
		PlatformDBModule->AddParameter(pGameServerCreate->wKindID);
		//PlatformDBModule->AddParameter(pGameServerCreate->wNodeID);
		PlatformDBModule->AddParameter(pGameServerCreate->wSortID);
		PlatformDBModule->AddParameter(pGameServerCreate->lCellScore);
		PlatformDBModule->AddParameter(pGameServerCreate->wRevenueRatio);
		PlatformDBModule->AddParameter(pGameServerCreate->lServiceScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lRestrictScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lMinTableScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lMinEnterScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lMaxEnterScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lDefPutScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lLowPutScore);
		PlatformDBModule->AddParameter(pGameServerCreate->iExchangeRatio);
		PlatformDBModule->AddParameter(pGameServerCreate->lMagicExpScore);
		PlatformDBModule->AddParameter(pGameServerCreate->lGratuityScore);
		
		PlatformDBModule->AddParameter(pGameServerCreate->dwWinExperience);
		PlatformDBModule->AddParameter(pGameServerCreate->dwFailExperience);

		PlatformDBModule->AddParameter(pGameServerCreate->cbMinEnterMember);
		PlatformDBModule->AddParameter(pGameServerCreate->cbMaxEnterMember);
		PlatformDBModule->AddParameter(pGameServerCreate->dwServerRule);
		PlatformDBModule->AddParameter(pGameServerCreate->dwAttachUserRight);
		PlatformDBModule->AddParameter(pGameServerCreate->wMaxPlayer);
		PlatformDBModule->AddParameter(pGameServerCreate->wTableCount);
		PlatformDBModule->AddParameter(pGameServerCreate->wServerPort);
		PlatformDBModule->AddParameter(pGameServerCreate->wServerKind);
		PlatformDBModule->AddParameter(pGameServerCreate->wServerType);
		PlatformDBModule->AddParameter(pGameServerCreate->szServerName);
		PlatformDBModule->AddParameter(pGameServerCreate->szServerPasswd);
		PlatformDBModule->AddParameter(pGameServerCreate->cbDistributeRule);
		PlatformDBModule->AddParameter(pGameServerCreate->wMinDistributeUser);
		PlatformDBModule->AddParameter(pGameServerCreate->wDistributeTimeSpace);
		PlatformDBModule->AddParameter(pGameServerCreate->wDistributeDrawCount);
		PlatformDBModule->AddParameter(pGameServerCreate->wMinPartakeGameUser);
		PlatformDBModule->AddParameter(pGameServerCreate->wMaxPartakeGameUser);
		//PlatformDBModule->AddParameter(pGameServerCreate->szDataBaseName);
		//PlatformDBModule->AddParameter(pGameServerCreate->szDataBaseAddr);

		////�����������ַ
		//PlatformDBModule->AddParameter(pGameServerCreate->szCacheServerAddr);
		////����������˿�
		//PlatformDBModule->AddParameter(pGameServerCreate->wCacheServerPort);

		PlatformDBModule->AddParameter(szCustomRule);
		PlatformDBModule->AddParameter(pGameServerCreate->szServiceMachine);
		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//ִ������
		BOOL bResult = PlatformDBModule->ExecuteProcess("GSP_GS_ModifyGameRoom");
		if (bResult == FALSE)
		{
			ASSERT(FALSE);
			return false;
		}

		CleanupResults objCleanup(PlatformDBModule);
		SQLRecordset rs(PlatformDBModule, false);
		rs.FetchAll();

		PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
		SQLRecordset rsRet(PlatformDBModule);
		LONG lResultCode = rsRet.GetValue_LONG("@nRet");
		//��ȡ�б�
		if (lResultCode != DB_SUCCESS)
		{
			//�����Ϣ
			TCHAR szDescribeString[128] = TEXT("");
			//��ȡ��Ϣ
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameServerInfo(rs, GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//��ȡ����
bool CServerInfoManager::GetAndroidParameter(WORD wServerID, CYSArray<tagAndroidParameter> & AndParameterAry)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);

		//ִ�в�ѯ
		BOOL bResultCode = PlatformDBModule->ExecuteProcess("GSP_GP_AndroidGetParameter");
		if (bResultCode)
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			if (rs.GetCount() > 0)
			{
				//��ȡ��¼
				do
				{
					tagAndroidParameter mAndParam;
					ReadAndroidParameter(rs, &mAndParam);
					AndParameterAry.Add(mAndParam);
				} while (rs.Fetch());
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//ɾ������
bool CServerInfoManager::DeleteAndroidParameter(WORD wServerID, DWORD dwBatchID)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(dwBatchID);

		//ִ�в�ѯ
		BOOL bResultCode = PlatformDBModule->ExecuteProcess("GSP_GP_AndroidDeleteParameter");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//�޸Ĳ���
bool CServerInfoManager::ModifyAndroidParameter(WORD wServerID, tagAndroidParameter *  pAndroidParameter)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(pAndroidParameter->dwBatchID);
		PlatformDBModule->AddParameter(pAndroidParameter->dwServiceMode);
		PlatformDBModule->AddParameter(pAndroidParameter->dwAndroidCount);
		PlatformDBModule->AddParameter(pAndroidParameter->dwEnterTime);
		PlatformDBModule->AddParameter(pAndroidParameter->dwLeaveTime);
		PlatformDBModule->AddParameter(pAndroidParameter->dwEnterMinInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->dwEnterMaxInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->dwLeaveMinInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->dwLeaveMaxInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->lTakeMinScore);
		PlatformDBModule->AddParameter(pAndroidParameter->lTakeMaxScore);
		PlatformDBModule->AddParameter(pAndroidParameter->dwSwitchMinInnings);
		PlatformDBModule->AddParameter(pAndroidParameter->dwSwitchMaxInnings);

		//ִ�в�ѯ
		BOOL bResultCode = PlatformDBModule->ExecuteProcess("GSP_GP_AndroidModifyParameter");
		if (bResultCode)
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			if (rs.GetCount() > 0)
			{
				ReadAndroidParameter(rs, pAndroidParameter);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//��Ӳ���
bool CServerInfoManager::AddAndroidParameter(WORD wServerID, tagAndroidParameter *  pAndroidParameter)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�������
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);
		PlatformDBModule->AddParameter(pAndroidParameter->dwServiceMode);
		PlatformDBModule->AddParameter(pAndroidParameter->dwAndroidCount);
		PlatformDBModule->AddParameter(pAndroidParameter->dwEnterTime);
		PlatformDBModule->AddParameter(pAndroidParameter->dwLeaveTime);
		PlatformDBModule->AddParameter(pAndroidParameter->dwEnterMinInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->dwEnterMaxInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->dwLeaveMinInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->dwLeaveMaxInterval);
		PlatformDBModule->AddParameter(pAndroidParameter->lTakeMinScore);
		PlatformDBModule->AddParameter(pAndroidParameter->lTakeMaxScore);
		PlatformDBModule->AddParameter(pAndroidParameter->dwSwitchMinInnings);
		PlatformDBModule->AddParameter(pAndroidParameter->dwSwitchMaxInnings);

		//ִ�в�ѯ
		BOOL bResultCode = PlatformDBModule->ExecuteProcess("GSP_GP_AndroidAddParameter");
		if (bResultCode)
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			if (rs.GetCount() > 0)
			{
				ReadAndroidParameter(rs, pAndroidParameter);
			}
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

//��������
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//��������
	PlatformDBModule->SetDBInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//��������
	PlatformDBModule->Open();

	return true;
}

//��ȡ����
bool CServerInfoManager::ReadGameServerInfo(SQLRecordset & rs, tagGameServerInfo & GameServerResult)
{
	//���ñ���
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//��������
	GameServerResult.wGameID=rs.GetValue_WORD("GameID");
	GameServerResult.wServerID=rs.GetValue_WORD("ServerID");

	//�ҽ�����
	GameServerResult.wKindID=rs.GetValue_WORD("KindID");
	//GameServerResult.wNodeID=rs.GetValue_WORD("NodeID");
	GameServerResult.wSortID=rs.GetValue_WORD("SortID");

	//�汾��Ϣ
	GameServerResult.dwClientVersion=rs.GetValue_DWORD("ClientVersion");
	GameServerResult.dwServerVersion=rs.GetValue_DWORD("ServerVersion");

	//ģ������
	rs.GetValue_String("GameName",GameServerResult.szGameName,CountArray(GameServerResult.szGameName));
	rs.GetValue_String("ServerDLLName",GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));


	//˰������
	GameServerResult.lCellScore=rs.GetValue_LONG("CellScore");
	GameServerResult.wRevenueRatio=rs.GetValue_WORD("RevenueRatio");
	GameServerResult.lServiceScore=rs.GetValue_LONGLONG("ServiceScore");

	//��������
	GameServerResult.lRestrictScore=rs.GetValue_LONGLONG("RestrictScore");
	GameServerResult.lMinTableScore=rs.GetValue_LONGLONG("MinTableScore");
	GameServerResult.lMinEnterScore=rs.GetValue_LONGLONG("MinEnterScore");
	GameServerResult.lMaxEnterScore=rs.GetValue_LONGLONG("MaxEnterScore");

	//�������
	GameServerResult.lDefPutScore = rs.GetValue_LONGLONG("DefPutScore");
	GameServerResult.lLowPutScore = rs.GetValue_LONGLONG("LowPutScore");
	GameServerResult.iExchangeRatio = rs.GetValue_INT("ExchangeRatio");

	//��������
	GameServerResult.lMagicExpScore = rs.GetValue_LONGLONG("MagicExpScore");
	GameServerResult.lGratuityScore = rs.GetValue_LONGLONG("GratuityScore");
	GameServerResult.dwWinExperience = rs.GetValue_LONGLONG("WinExperience");
	GameServerResult.dwFailExperience = rs.GetValue_LONGLONG("FailExperience");

	//��Ա����
	GameServerResult.cbMinEnterMember=rs.GetValue_BYTE("MinEnterMember");
	GameServerResult.cbMaxEnterMember=rs.GetValue_BYTE("MaxEnterMember");

	//��������
	GameServerResult.dwServerRule=rs.GetValue_DWORD("ServerRule");
	GameServerResult.dwAttachUserRight=rs.GetValue_DWORD("AttachUserRight");

	//��������
	GameServerResult.wMaxPlayer=rs.GetValue_WORD("MaxPlayer");
	GameServerResult.wTableCount=rs.GetValue_WORD("TableCount");
	GameServerResult.wServerPort=rs.GetValue_WORD("ServerPort");
	GameServerResult.wServerKind=rs.GetValue_WORD("ServerKind");
	GameServerResult.wServerType=rs.GetValue_WORD("ServerType");
	rs.GetValue_String("ServerName",GameServerResult.szServerName,CountArray(GameServerResult.szServerName));
	rs.GetValue_String("ServerPasswd",GameServerResult.szServerPasswd,CountArray(GameServerResult.szServerPasswd));

	//��������
	GameServerResult.cbDistributeRule=rs.GetValue_BYTE("DistributeRule");
	GameServerResult.wMinDistributeUser=rs.GetValue_WORD("MinDistributeUser");
	GameServerResult.wDistributeTimeSpace=rs.GetValue_WORD("DistributeTimeSpace");
	GameServerResult.wDistributeDrawCount=rs.GetValue_WORD("DistributeDrawCount");
	GameServerResult.wMinPartakeGameUser=rs.GetValue_WORD("MinPartakeGameUser");
	GameServerResult.wMaxPartakeGameUser=rs.GetValue_WORD("MaxPartakeGameUser");

	////��������
	//rs.GetValue_String("DataBaseName",GameServerResult.szDataBaseName,CountArray(GameServerResult.szDataBaseName));
	//rs.GetValue_String("DataBaseAddr",GameServerResult.szDataBaseAddr,CountArray(GameServerResult.szDataBaseAddr));

	////��������
	//rs.GetValue_String("CacheServerAddr", GameServerResult.szCacheServerAddr, CountArray(GameServerResult.szCacheServerAddr));
	//GameServerResult.wCacheServerPort = rs.GetValue_WORD("CacheServerPort");

	//��չ����
	TCHAR szCustomRule[CountArray(GameServerResult.cbCustomRule)*2+1];
	rs.GetValue_String("CustomRule",szCustomRule,CountArray(szCustomRule));
	rs.GetValue_String("ServiceMachine",GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));

	//��չ����
	if (szCustomRule[0]!=0)
	{
		//��ȡ����
		ASSERT(lstrlen(szCustomRule)%2==0);
		INT nCustomRuleSize=lstrlen(szCustomRule)/2;

		//ת���ַ�
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//��ȡ�ַ�
			TCHAR cbChar1=szCustomRule[i*2];
			TCHAR cbChar2=szCustomRule[i*2+1];

			//Ч���ַ�
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//���ɽ��
			if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')))
			{
				GameServerResult.cbCustomRule[i] += (cbChar2 - TEXT('0'));
			}

			if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')))
			{
				GameServerResult.cbCustomRule[i] += (cbChar2 - TEXT('A') + 0x0A);
			}

			//���ɽ��
			if ((cbChar1 >= TEXT('0')) && (cbChar1 <= TEXT('9')))
			{
				GameServerResult.cbCustomRule[i] += (cbChar1 - TEXT('0')) * 0x10;
			}

			if ((cbChar1 >= TEXT('A')) && (cbChar1 <= TEXT('F')))
			{
				GameServerResult.cbCustomRule[i] += (cbChar1 - TEXT('A') + 0x0A) * 0x10;
			}
		}
	}

	//���ذ汾
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CYSService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	return true;
}

//��ȡ����
bool CServerInfoManager::ReadGameMatchOption(SQLRecordset & rs, tagGameMatchOption & GameMatchResult)
{
	//���ñ���
	ZeroMemory(&GameMatchResult,sizeof(GameMatchResult));

	//��������
	GameMatchResult.dwMatchID=rs.GetValue_DWORD("MatchID");
	GameMatchResult.dwMatchNO=rs.GetValue_DWORD("MatchNo");

	//������Ϣ
	GameMatchResult.cbMatchFeeType= rs.GetValue_BYTE("MatchFeeType");
	GameMatchResult.lMatchFee= rs.GetValue_LONGLONG("MatchFee");

	//������Ϣ
	GameMatchResult.cbMemberOrder = rs.GetValue_BYTE("MemberOrder");
	GameMatchResult.cbMatchType = rs.GetValue_BYTE("MatchType");
	rs.GetValue_String("MatchName",GameMatchResult.szMatchName,CountArray(GameMatchResult.szMatchName));

	//��ʱ����
	if(GameMatchResult.cbMatchType==MATCH_TYPE_LOCKTIME)
	{
		//��������
		GameMatchResult.MatchType.LockTime.wMatchPlayCount= rs.GetValue_WORD("MinPlayCount");

		//ʱ������
		rs.GetValue_SystemTime("StartTime",GameMatchResult.MatchType.LockTime.MatchStartTime);
		rs.GetValue_SystemTime("EndTime",GameMatchResult.MatchType.LockTime.MatchEndTime);

		//��������
		GameMatchResult.MatchType.LockTime.lMatchInitScore = rs.GetValue_LONGLONG("InitScore");
		GameMatchResult.MatchType.LockTime.lMatchCullScore = rs.GetValue_LONGLONG("CullScore");
	}

	//��ʱ����
	if(GameMatchResult.cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		//������Ϣ
		GameMatchResult.MatchType.Immediate.wStartUserCount = rs.GetValue_WORD("StartUserCount");
		GameMatchResult.MatchType.Immediate.wAndroidUserCount = rs.GetValue_WORD("AndroidUserCount");

		//������Ϣ
		GameMatchResult.MatchType.Immediate.dwInitalBase = rs.GetValue_DWORD("InitialBase");
		GameMatchResult.MatchType.Immediate.dwInitalScore = rs.GetValue_DWORD("InitialScore");
		GameMatchResult.MatchType.Immediate.wMinEnterGold = rs.GetValue_WORD("MinEnterGold");

		//������Ϣ
		GameMatchResult.MatchType.Immediate.cbPlayCount = rs.GetValue_BYTE("PlayCount");
		GameMatchResult.MatchType.Immediate.cbSwitchTableCount = rs.GetValue_BYTE("SwitchTableCount");
		GameMatchResult.MatchType.Immediate.wPrecedeTimer = rs.GetValue_WORD("PrecedeTimer");
	}
	
	return true;
}

//��ȡ����
bool CServerInfoManager::ReadAndroidParameter(SQLRecordset & rs, tagAndroidParameter *  pAndroidParameter)
{
	pAndroidParameter->dwBatchID = rs.GetValue_DWORD("BatchID");
	pAndroidParameter->dwAndroidCount = rs.GetValue_DWORD("AndroidCount");
	pAndroidParameter->dwServiceMode = rs.GetValue_DWORD("ServiceMode");
	pAndroidParameter->dwEnterTime = rs.GetValue_DWORD("EnterTime");
	pAndroidParameter->dwLeaveTime = rs.GetValue_DWORD("LeaveTime");
	pAndroidParameter->dwEnterMinInterval = rs.GetValue_DWORD("EnterMinInterval");
	pAndroidParameter->dwEnterMaxInterval = rs.GetValue_DWORD("EnterMaxInterval");
	pAndroidParameter->dwLeaveMinInterval = rs.GetValue_DWORD("LeaveMinInterval");
	pAndroidParameter->dwLeaveMaxInterval = rs.GetValue_DWORD("LeaveMaxInterval");
	pAndroidParameter->dwSwitchMinInnings = rs.GetValue_DWORD("SwitchMinInnings");
	pAndroidParameter->dwSwitchMaxInnings = rs.GetValue_DWORD("SwitchMaxInnings");
	pAndroidParameter->lTakeMinScore = rs.GetValue_LONGLONG("TakeMinScore");
	pAndroidParameter->lTakeMaxScore = rs.GetValue_LONGLONG("TakeMaxScore");

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
