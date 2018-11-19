#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////
//��������;
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("GuanDanAndroid.dll")	//�������;
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("GuanDanAndroidD.dll")	//�������;
#endif
//���캯��;
CGameServiceManager::CGameServiceManager()
{
	//�ؼ�����;
	m_pDlgCustomRule = NULL;

	//��������;
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_EDUCATE | SCORE_GENRE_NORMAL);

	//���ܱ�־;
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;
	//��������;
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("GuanDanClient.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("GuanDanServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//��������;
CGameServiceManager::~CGameServiceManager()
{
	//ɾ������;
	SafeDelete(m_pDlgCustomRule);
}

//�ӿڲ�ѯ;
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��ȡ����;
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
	return true;
}

//�����޸�;
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����;
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//��Ԫ����;
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	//��������;
	//�����Ϸ;
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore*8L,GameServiceOption.lMinTableScore);
	}

	//�������(ԭ���Ļ�������);
	if (GameServiceOption.lRestrictScore!=0L)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

	//��������;
	if (GameServiceOption.wServerType!=GAME_GENRE_SCORE && GameServiceOption.wServerType !=0)
	{
		if (GameServiceOption.lMaxEnterScore <= GameServiceOption.lMinTableScore)
		{
			GameServiceOption.lMaxEnterScore = 0L;
		}
		else if (GameServiceOption.lRestrictScore > 0)
		{
			GameServiceOption.lMaxEnterScore = __min(GameServiceOption.lMaxEnterScore, GameServiceOption.lRestrictScore);
		}
	}

	tagCustomRule * pCustomRule = (tagCustomRule *)GameServiceOption.cbCustomRule;

	//��ʼʱ��;
	if ((pCustomRule->cbTimeStartGame < 2) || (pCustomRule->cbTimeStartGame>30))
	{
		pCustomRule->cbTimeStartGame = 5;
	}

	//����ʱ��;
	if ((pCustomRule->cbTimePayTribute < 5) || (pCustomRule->cbTimePayTribute>60))
	{
		pCustomRule->cbTimePayTribute = 10;
	}

	//�ع�ʱ��;
	if ((pCustomRule->cbTimeRetTribute < 5) || (pCustomRule->cbTimeRetTribute>60))
	{
		pCustomRule->cbTimeRetTribute = 15;
	}

	//�׳�ʱ��;
	if ((pCustomRule->cbTimeHeadOutCard < 5) || (pCustomRule->cbTimeHeadOutCard>60))
	{
		pCustomRule->cbTimeHeadOutCard = 20;
	}

	//����ʱ��;
	if ((pCustomRule->cbTimeOutCard < 5) || (pCustomRule->cbTimeOutCard>60))
	{
		pCustomRule->cbTimeOutCard = 15;
	}

	//�й�ʱ��;
	if ((pCustomRule->cbTimeTrustee < 1) || (pCustomRule->cbTimeTrustee>20))
	{
		pCustomRule->cbTimeTrustee = 1;
	}

	return true;
}

//��������;
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//��������;
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//��������;
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("����ʧ��");

		//��ѯ�ӿ�;
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");

		return pObject;
	}
	catch (...) {}

	//ɾ������;
	SafeDelete(pTableFrameSink);

	return NULL;
}

//��������;
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//�������;
		if( m_AndroidServiceHelper.GetInterface() == NULL )
		{
			m_AndroidServiceHelper.SetModuleCreateInfo(ANDROID_SERVICE_DLL_NAME,"CreateGameServiceManager");

			if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
		}

		//����������;
		VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
		if( pAndroidObject == NULL ) 
		{
			CTraceService::TraceString(TEXT("CGameServiceManager����������ʧ��"),TraceLevel_Exception);
			throw TEXT("����������ʧ��");
		}
		return pAndroidObject;
	}
	catch(...) {}
	return NULL;
}


//��ȡ����;
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//Ч��״̬;
	ASSERT(m_pDlgCustomRule != NULL);
	if (m_pDlgCustomRule == NULL) return false;

	//��������;
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//��ȡ����;
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule) == false)
	{
		return false;
	}

	return true;
}

//Ĭ������;
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������;
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//���ñ���;
	pCustomRule->cbTimeOutCard = 15;
	pCustomRule->cbTimeStartGame = 5;
	pCustomRule->cbTimeHeadOutCard = 20;

	pCustomRule->cbTimeTrustee = 1;
	pCustomRule->cbTimePayTribute = 10;
	pCustomRule->cbTimeRetTribute = 15;

	return true;
}

//��������;
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������;
	if (m_pDlgCustomRule == NULL)
	{
		m_pDlgCustomRule = new CDlgCustomRule;
	}

	//��������;
	if (m_pDlgCustomRule->m_hWnd == NULL)
	{
		//������Դ;
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������;
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE, pParentWnd);

		//��ԭ��Դ;
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���;
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//��ʾ����;
	m_pDlgCustomRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
