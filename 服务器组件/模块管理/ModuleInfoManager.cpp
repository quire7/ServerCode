#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleInfoBuffer::CModuleInfoBuffer()
{
}

//��������
CModuleInfoBuffer::~CModuleInfoBuffer()
{
	//��������
	WORD wModuleID=0;
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);
		SafeDelete(pGameModuleInfo);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_GameModuleInfoArray.GetCount();i++)
	{
		pGameModuleInfo=m_GameModuleInfoArray[i];
		SafeDelete(pGameModuleInfo);
	}

	//ɾ������
	m_GameModuleInfoMap.RemoveAll();
	m_GameModuleInfoArray.RemoveAll();

	return;
}

//��������
bool CModuleInfoBuffer::ResetModuleInfo()
{
	//��������
	WORD wModuleID=0;
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);
		m_GameModuleInfoArray.Add(pGameModuleInfo);
	}

	//ɾ������
	m_GameModuleInfoMap.RemoveAll();

	return true;
}

//ɾ������
bool CModuleInfoBuffer::DeleteModuleInfo(WORD wModuleID)
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;
	if (m_GameModuleInfoMap.Lookup(wModuleID,pGameModuleInfo)==FALSE) return false;

	//ɾ������
	m_GameModuleInfoMap.RemoveKey(wModuleID);
	m_GameModuleInfoArray.Add(pGameModuleInfo);

	//���ñ���
	ZeroMemory(pGameModuleInfo,sizeof(tagGameModuleInfo));

	return true;
}

//��������
bool CModuleInfoBuffer::InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo)
{
	//Ч�����
	ASSERT(pGameModuleInfo!=NULL);
	if (pGameModuleInfo==NULL) return false;

	//�����ִ�
	WORD wGameID=pGameModuleInfo->wGameID;
	tagGameModuleInfo * pGameModuleInsert=SearchModuleInfo(wGameID);

	//�����ж�
	if (pGameModuleInsert==NULL)
	{
		//��������
		pGameModuleInsert=CreateModuleInfo();

		//����ж�
		if (pGameModuleInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameModuleInfoMap[wGameID]=pGameModuleInsert;
	CopyMemory(pGameModuleInsert,pGameModuleInfo,sizeof(tagGameModuleInfo));

	return true;
}

//��ȡ��Ŀ
DWORD CModuleInfoBuffer::GetModuleInfoCount()
{
	return (DWORD)(m_GameModuleInfoMap.GetCount());
}

//��������
tagGameModuleInfo * CModuleInfoBuffer::SearchModuleInfo(WORD wModuleID)
{
	tagGameModuleInfo * pGameModuleInfo=NULL;
	m_GameModuleInfoMap.Lookup(wModuleID,pGameModuleInfo);
	return pGameModuleInfo;
}

//��������
tagGameModuleInfo * CModuleInfoBuffer::CreateModuleInfo()
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameModuleInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameModuleInfo=m_GameModuleInfoArray[nArrayCount-1];
			m_GameModuleInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameModuleInfo=new tagGameModuleInfo;
			if (pGameModuleInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameModuleInfo,sizeof(tagGameModuleInfo));

	return pGameModuleInfo;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleInfoManager::CModuleInfoManager()
{
}

//��������
CModuleInfoManager::~CModuleInfoManager()
{
}

//ע��ģ��
bool CModuleInfoManager::RegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//ע��ģ��
bool CModuleInfoManager::UnRegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//����ģ��
bool CModuleInfoManager::LoadGameModuleInfo(CModuleInfoBuffer & ModuleInfoBuffer)
{
	//��������
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetDBInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
			pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

		//��������
		PlatformDBModule->Open();

		//��ȡ�б�
		PlatformDBModule->ClearParameters();
		if (PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameItem"))
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			//����б�
			ModuleInfoBuffer.ResetModuleInfo();

			//��ȡ�б�
			do
			{
				//��������
				tagGameModuleInfo GameModuleInfo;
				ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

				//ģ������
				GameModuleInfo.wGameID=rs.GetValue_WORD(("GameID"));
				GameModuleInfo.dwClientVersion=rs.GetValue_DWORD(("ClientVersion"));
				GameModuleInfo.dwServerVersion=rs.GetValue_DWORD(("ServerVersion"));

				//��������
				rs.GetValue_String("GameName",GameModuleInfo.szGameName,CountArray(GameModuleInfo.szGameName));
				rs.GetValue_String("DataBaseAddr",GameModuleInfo.szDataBaseAddr,CountArray(GameModuleInfo.szDataBaseAddr));
				rs.GetValue_String("DataBaseName",GameModuleInfo.szDataBaseName,CountArray(GameModuleInfo.szDataBaseName));

				//��Ϸ����
				rs.GetValue_String("ServerDLLName",GameModuleInfo.szServerDLLName,CountArray(GameModuleInfo.szServerDLLName));

				//���ذ汾
				LPCTSTR pszServerDLLName=GameModuleInfo.szServerDLLName;
				CYSService::GetModuleVersion(pszServerDLLName,GameModuleInfo.dwNativeVersion);

				//�б���
				ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);
			}
			while (rs.Fetch());
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

//ģ������
bool CModuleInfoManager::GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	ZeroMemory(&GameServiceAttrib,sizeof(GameServiceAttrib));

	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pszModuleName,GAME_SERVICE_CREATE_NAME);

	//����ģ��
	if (GameServiceManager.CreateInstance()==true)
	{
		GameServiceManager->GetServiceAttrib(GameServiceAttrib);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
