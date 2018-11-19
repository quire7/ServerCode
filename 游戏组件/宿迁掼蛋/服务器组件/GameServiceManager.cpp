#include "StdAfx.h"
#include "Resource.h"
#include "TableFrameSink.h"
#include "GameServiceManager.h"

//////////////////////////////////////////////////////////////////////////
//机器定义;
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("GuanDanAndroid.dll")	//组件名字;
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("GuanDanAndroidD.dll")	//组件名字;
#endif
//构造函数;
CGameServiceManager::CGameServiceManager()
{
	//控件变量;
	m_pDlgCustomRule = NULL;

	//设置属性;
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_EDUCATE | SCORE_GENRE_NORMAL);

	//功能标志;
	m_GameServiceAttrib.cbDynamicJoin=FALSE;
	m_GameServiceAttrib.cbAndroidUser=TRUE;
	m_GameServiceAttrib.cbOffLineTrustee=FALSE;
	//服务属性;
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;
	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,szTreasureDB,CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("GuanDanClient.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("GuanDanServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));

	return;
}

//析构函数;
CGameServiceManager::~CGameServiceManager()
{
	//删除对象;
	SafeDelete(m_pDlgCustomRule);
}

//接口查询;
VOID * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//获取属性;
bool CGameServiceManager::GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	GameServiceAttrib=m_GameServiceAttrib;
	return true;
}

//参数修改;
bool CGameServiceManager::RectifyParameter(tagGameServiceOption & GameServiceOption)
{
	//效验参数;
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//单元积分;
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	//积分下限;
	//金币游戏;
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinTableScore=__max(GameServiceOption.lCellScore*8L,GameServiceOption.lMinTableScore);
	}

	//输分限制(原来的积分上限);
	if (GameServiceOption.lRestrictScore!=0L)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinTableScore);
	}

	//积分上限;
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

	//开始时间;
	if ((pCustomRule->cbTimeStartGame < 2) || (pCustomRule->cbTimeStartGame>30))
	{
		pCustomRule->cbTimeStartGame = 5;
	}

	//进贡时间;
	if ((pCustomRule->cbTimePayTribute < 5) || (pCustomRule->cbTimePayTribute>60))
	{
		pCustomRule->cbTimePayTribute = 10;
	}

	//回贡时间;
	if ((pCustomRule->cbTimeRetTribute < 5) || (pCustomRule->cbTimeRetTribute>60))
	{
		pCustomRule->cbTimeRetTribute = 15;
	}

	//首出时间;
	if ((pCustomRule->cbTimeHeadOutCard < 5) || (pCustomRule->cbTimeHeadOutCard>60))
	{
		pCustomRule->cbTimeHeadOutCard = 20;
	}

	//出牌时间;
	if ((pCustomRule->cbTimeOutCard < 5) || (pCustomRule->cbTimeOutCard>60))
	{
		pCustomRule->cbTimeOutCard = 15;
	}

	//托管时间;
	if ((pCustomRule->cbTimeTrustee < 1) || (pCustomRule->cbTimeTrustee>20))
	{
		pCustomRule->cbTimeTrustee = 1;
	}

	return true;
}

//创建桌子;
VOID * CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//变量定义;
	CTableFrameSink * pTableFrameSink=NULL;

	try
	{
		//建立对象;
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");

		//查询接口;
		VOID * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	//删除对象;
	SafeDelete(pTableFrameSink);

	return NULL;
}

//创建机器;
VOID * CGameServiceManager::CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//创建组件;
		if( m_AndroidServiceHelper.GetInterface() == NULL )
		{
			m_AndroidServiceHelper.SetModuleCreateInfo(ANDROID_SERVICE_DLL_NAME,"CreateGameServiceManager");

			if( !m_AndroidServiceHelper.CreateInstance() ) throw 0;
		}

		//创建机器人;
		VOID *pAndroidObject = m_AndroidServiceHelper->CreateAndroidUserItemSink(Guid,dwQueryVer);
		if( pAndroidObject == NULL ) 
		{
			CTraceService::TraceString(TEXT("CGameServiceManager创建机器人失败"),TraceLevel_Exception);
			throw TEXT("创建机器人失败");
		}
		return pAndroidObject;
	}
	catch(...) {}
	return NULL;
}


//获取配置;
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//效验状态;
	ASSERT(m_pDlgCustomRule != NULL);
	if (m_pDlgCustomRule == NULL) return false;

	//变量定义;
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//获取配置;
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule) == false)
	{
		return false;
	}

	return true;
}

//默认配置;
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//变量定义;
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	tagCustomRule * pCustomRule = (tagCustomRule *)pcbCustomRule;

	//设置变量;
	pCustomRule->cbTimeOutCard = 15;
	pCustomRule->cbTimeStartGame = 5;
	pCustomRule->cbTimeHeadOutCard = 20;

	pCustomRule->cbTimeTrustee = 1;
	pCustomRule->cbTimePayTribute = 10;
	pCustomRule->cbTimeRetTribute = 15;

	return true;
}

//创建窗口;
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//创建窗口;
	if (m_pDlgCustomRule == NULL)
	{
		m_pDlgCustomRule = new CDlgCustomRule;
	}

	//创建窗口;
	if (m_pDlgCustomRule->m_hWnd == NULL)
	{
		//设置资源;
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口;
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE, pParentWnd);

		//还原资源;
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量;
	ASSERT(wCustonSize >= sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//显示窗口;
	m_pDlgCustomRule->SetWindowPos(NULL, rcCreate.left, rcCreate.top, rcCreate.Width(), rcCreate.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	static CGameServiceManager GameServiceManager;
	return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
