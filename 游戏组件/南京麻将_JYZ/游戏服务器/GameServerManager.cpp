#include "StdAfx.h"
#include "GameLottery.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////
//��������
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("JYZAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("JYZAndroid.dll")	//�������
#endif

//////////////////////////////////////////////////////////////////////////
//���캯��
CGameServiceManager::CGameServiceManager(void)
{
	m_hDllInstance = NULL;

	//��������
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_EDUCATE | GAME_GENRE_PRIVATE);

	//���ܱ�־
	m_GameServiceAttrib.cbDynamicJoin = FALSE;
	m_GameServiceAttrib.cbAndroidUser = TRUE;
	m_GameServiceAttrib.cbOffLineTrustee = FALSE;

	//��������
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, szTreasureDB, CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("JYZ.EXE"), CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("JYZServer.DLL"), CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������
CGameServiceManager::~CGameServiceManager()
{
}
//�ӿڲ�ѯ
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager, Guid, dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager, Guid, dwQueryVer);
	return NULL;
}

//������Ϸ��
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink = NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) throw TEXT("����ʧ��");
		void * pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��ȡ����
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib = m_GameServiceAttrib;
	return true;
}

//�����޸�
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption != NULL);
	if (&GameServiceOption == NULL) return false;

	//��Ԫ����
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//��������
	if (GameServiceOption.wServerType == GAME_GENRE_GOLD)
	{
		GameServiceOption.lMinTableScore = __max(GameServiceOption.lCellScore * 50L, GameServiceOption.lMinTableScore);
	}
	return true;
}

//��ȡ����
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	return true;
}

//Ĭ������
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	return true;
}

//��������
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	return NULL;
}

//��������
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//����ģ��
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(ANDROID_SERVICE_DLL_NAME);
			if (m_hDllInstance == NULL) throw TEXT("�����˷���ģ�鲻����");
		}

		//Ѱ�Һ���
		ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateAndroidUserItemSink");
		if (CreateProc == NULL)
		{
			CTraceService::TraceString(TEXT("����������ʧ��"), TraceLevel_Exception);
			throw TEXT("����������ʧ��");
		}

		//�������
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}

	return NULL;
}

//��������
VOID * CGameServiceManager::CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(REFGUID Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid, dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
