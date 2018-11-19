#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoBuffer::CServerInfoBuffer()
{
}

//析构函数
CServerInfoBuffer::~CServerInfoBuffer()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//删除数组
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//删除引用
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//重置数据
bool CServerInfoBuffer::ResetServerInfo()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//删除索引
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//删除数据
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//查找类型
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//删除数据
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//插入数据
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//效验参数
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//查找现存
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//创建判断
	if (pGameServerInsert==NULL)
	{
		//创建对象
		pGameServerInsert=CreateServerInfo();

		//结果判断
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//获取数目
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//查找数据
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//创建对象
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//变量定义
	tagGameServerInfo * pGameServerInfo=NULL;

	//创建对象
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

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMatchOptionBuffer::CMatchOptionBuffer()
{
}

//析构函数
CMatchOptionBuffer::~CMatchOptionBuffer()
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//删除数组
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);
	for (INT_PTR i=0;i<m_GameMatchOptionBuffer.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionBuffer[i];
		SafeDelete(pGameMatchOption);
	}

	//删除引用
	m_GameMatchOptionArray.RemoveAll();
	m_GameMatchOptionBuffer.RemoveAll();

	return;
}

//重置数据
bool CMatchOptionBuffer::ResetMatchOption()
{
	//添加对象
	m_GameMatchOptionBuffer.Append(m_GameMatchOptionArray);

	//删除元素
	m_GameMatchOptionArray.RemoveAll();

	return true;
}

//删除数据
bool CMatchOptionBuffer::DeleteMatchOption(DWORD dwMatchID,DWORD wMatchNO)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_GameMatchOptionArray.GetCount();i++)
	{
		pGameMatchOption=m_GameMatchOptionArray[i];
		if(pGameMatchOption->dwMatchID==dwMatchID && pGameMatchOption->dwMatchNO==wMatchNO)
		{
			//删除数据
			m_GameMatchOptionArray.RemoveAt(i);
			m_GameMatchOptionBuffer.Add(pGameMatchOption);

			//设置变量
			ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

			break;
		}
	}	

	return true;
}

//插入数据
bool CMatchOptionBuffer::InsertMatchOption(tagGameMatchOption * pGameMatchOption)
{
	//效验参数
	ASSERT(pGameMatchOption!=NULL);
	if (pGameMatchOption==NULL) return false;

	//查找现存
	tagGameMatchOption * pGameMatchInsert=SearchMatchOption(pGameMatchOption->dwMatchID,pGameMatchOption->dwMatchNO);

	//创建判断
	if (pGameMatchInsert==NULL)
	{
		//创建对象
		pGameMatchInsert=CreateMatchOption();

		//结果判断
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameMatchOptionArray.Add(pGameMatchInsert);
	CopyMemory(pGameMatchInsert,pGameMatchOption,sizeof(tagGameMatchOption));

	return true;
}

//获取数目
DWORD CMatchOptionBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_GameMatchOptionArray.GetCount());
}

//查找数据
tagGameMatchOption * CMatchOptionBuffer::SearchMatchOption(DWORD dwMatchID,DWORD wMatchNO)
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//查找对象
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

//创建对象
tagGameMatchOption * CMatchOptionBuffer::CreateMatchOption()
{
	//变量定义
	tagGameMatchOption * pGameMatchOption=NULL;

	//创建对象
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

	//设置变量
	ZeroMemory(pGameMatchOption,sizeof(tagGameMatchOption));

	return pGameMatchOption;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoManager::CServerInfoManager()
{
}

//析构函数
CServerInfoManager::~CServerInfoManager()
{
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//插入参数
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter((WORD)0);
		PlatformDBModule->AddParameter(szMachineID);

#ifdef _DEBUG
		CTraceService::TraceString(szMachineID, TraceLevel_Exception);
#endif

		//读取列表
		if (PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameRoomItem"))
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			//清空列表
			ServerInfoBuffer.ResetServerInfo();

			//读取列表
			if (rs.GetCount()>0)
			{
				do
				{
					//读取数据
					tagGameServerInfo GameServerResult;
					ReadGameServerInfo(rs, GameServerResult);

					//插入列表
					ServerInfoBuffer.InsertServerInfo(&GameServerResult);
				} while (rs.Fetch());
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//读取列表
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);
		PlatformDBModule->AddParameter(szMachineID);

		//读取列表
		if (PlatformDBModule->ExecuteProcess("GSP_GS_LoadGameRoomItem"))
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			if (rs.GetCount() > 0)
			{
				//读取数据
				ReadGameServerInfo(rs, GameServerResult);
			}

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载奖励
WORD CServerInfoManager::LoadGameMatchReward(DWORD dwMatchID,DWORD dwMatchNO)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//插入参数
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(dwMatchID);
		PlatformDBModule->AddParameter(dwMatchNO);

		//读取列表
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
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return 0;
	}

	return 0;
}

//加载比赛
bool CServerInfoManager::LoadGameMatchOption(WORD wKindID,CMatchOptionBuffer & MatchOptionBuffer)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//插入参数
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

		//清空列表
		MatchOptionBuffer.ResetMatchOption();

		if (rs.GetCount() > 0)
		{
			//读取列表
			do
			{
				//读取数据
				tagGameMatchOption GameMatchResult;
				ReadGameMatchOption(rs, GameMatchResult);

				//插入列表
				MatchOptionBuffer.InsertMatchOption(&GameMatchResult);
			} while (rs.Fetch());
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载房间
bool CServerInfoManager::LoadGameMatchOption(WORD wKindID,DWORD dwMatchID,DWORD dwMatchNO,tagGameMatchOption & GameMatchResult)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//读取列表
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wKindID);
		PlatformDBModule->AddParameter(dwMatchID);
		PlatformDBModule->AddParameter(dwMatchNO);
		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//读取列表
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
		//读取列表
		if (lResultCode != DB_SUCCESS)
		{
			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			//获取信息
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//读取数据
		if (rs.GetCount()>0)
		{
			ReadGameMatchOption(rs,GameMatchResult);
			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//删除房间
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//插入参数
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);
		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//执行命令
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

		//读取列表
		if (lResultCode != DB_SUCCESS)
		{
			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			//获取信息
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//插入房间
bool CServerInfoManager::InsertGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//扩展配置
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i = 0; i < CountArray(pGameServerCreate->cbCustomRule); i++)
		{
			_stprintf(&szCustomRule[i * 2], TEXT("%02X"), pGameServerCreate->cbCustomRule[i]);
		}

		//插入参数
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

		////缓存服务器地址
		//PlatformDBModule->AddParameter(pGameServerCreate->szCacheServerAddr);
		////缓存服务器端口
		//PlatformDBModule->AddParameter(pGameServerCreate->wCacheServerPort);

		PlatformDBModule->AddParameter(szCustomRule);
		PlatformDBModule->AddParameter(pGameServerCreate->szServiceMachine);

		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//执行命令
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
		//读取列表
		if (lResultCode != DB_SUCCESS)
		{
			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			//获取信息
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(rs,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//修改房间
bool CServerInfoManager::ModifyGameServer(tagGameServerCreate * pGameServerCreate, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//扩展配置
		TCHAR szCustomRule[CountArray(pGameServerCreate->cbCustomRule)*2+1]=TEXT("");
		for (INT i = 0; i < CountArray(pGameServerCreate->cbCustomRule); i++)
		{
			_stprintf(&szCustomRule[i * 2], TEXT("%02X"), pGameServerCreate->cbCustomRule[i]);
		}

		//插入参数
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

		////缓存服务器地址
		//PlatformDBModule->AddParameter(pGameServerCreate->szCacheServerAddr);
		////缓存服务器端口
		//PlatformDBModule->AddParameter(pGameServerCreate->wCacheServerPort);

		PlatformDBModule->AddParameter(szCustomRule);
		PlatformDBModule->AddParameter(pGameServerCreate->szServiceMachine);
		PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		PlatformDBModule->AddParameterOutput("@nRet");

		//执行命令
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
		//读取列表
		if (lResultCode != DB_SUCCESS)
		{
			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			//获取信息
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString, TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString, MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(rs, GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//获取参数
bool CServerInfoManager::GetAndroidParameter(WORD wServerID, CYSArray<tagAndroidParameter> & AndParameterAry)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//构造参数
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(wServerID);

		//执行查询
		BOOL bResultCode = PlatformDBModule->ExecuteProcess("GSP_GP_AndroidGetParameter");
		if (bResultCode)
		{
			CleanupResults objCleanup(PlatformDBModule);
			SQLRecordset rs(PlatformDBModule);

			if (rs.GetCount() > 0)
			{
				//读取记录
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
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//删除参数
bool CServerInfoManager::DeleteAndroidParameter(WORD wServerID, DWORD dwBatchID)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//构造参数
		PlatformDBModule->ClearParameters();
		PlatformDBModule->AddParameter(dwBatchID);

		//执行查询
		BOOL bResultCode = PlatformDBModule->ExecuteProcess("GSP_GP_AndroidDeleteParameter");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//修改参数
bool CServerInfoManager::ModifyAndroidParameter(WORD wServerID, tagAndroidParameter *  pAndroidParameter)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//构造参数
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

		//执行查询
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
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//添加参数
bool CServerInfoManager::AddAndroidParameter(WORD wServerID, tagAndroidParameter *  pAndroidParameter)
{
	//变量定义
	CDataBaseHelper PlatformDBModule;

	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//构造参数
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

		//执行查询
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
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//连接数据
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	PlatformDBModule->SetDBInfo(pDataBaseParameter->szDataBaseAddr,pDataBaseParameter->wDataBasePort,
		pDataBaseParameter->szDataBaseName,pDataBaseParameter->szDataBaseUser,pDataBaseParameter->szDataBasePass);

	//发起连接
	PlatformDBModule->Open();

	return true;
}

//读取房间
bool CServerInfoManager::ReadGameServerInfo(SQLRecordset & rs, tagGameServerInfo & GameServerResult)
{
	//设置变量
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//索引变量
	GameServerResult.wGameID=rs.GetValue_WORD("GameID");
	GameServerResult.wServerID=rs.GetValue_WORD("ServerID");

	//挂接属性
	GameServerResult.wKindID=rs.GetValue_WORD("KindID");
	//GameServerResult.wNodeID=rs.GetValue_WORD("NodeID");
	GameServerResult.wSortID=rs.GetValue_WORD("SortID");

	//版本信息
	GameServerResult.dwClientVersion=rs.GetValue_DWORD("ClientVersion");
	GameServerResult.dwServerVersion=rs.GetValue_DWORD("ServerVersion");

	//模块属性
	rs.GetValue_String("GameName",GameServerResult.szGameName,CountArray(GameServerResult.szGameName));
	rs.GetValue_String("ServerDLLName",GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));


	//税收配置
	GameServerResult.lCellScore=rs.GetValue_LONG("CellScore");
	GameServerResult.wRevenueRatio=rs.GetValue_WORD("RevenueRatio");
	GameServerResult.lServiceScore=rs.GetValue_LONGLONG("ServiceScore");

	//房间配置
	GameServerResult.lRestrictScore=rs.GetValue_LONGLONG("RestrictScore");
	GameServerResult.lMinTableScore=rs.GetValue_LONGLONG("MinTableScore");
	GameServerResult.lMinEnterScore=rs.GetValue_LONGLONG("MinEnterScore");
	GameServerResult.lMaxEnterScore=rs.GetValue_LONGLONG("MaxEnterScore");

	//带入积分
	GameServerResult.lDefPutScore = rs.GetValue_LONGLONG("DefPutScore");
	GameServerResult.lLowPutScore = rs.GetValue_LONGLONG("LowPutScore");
	GameServerResult.iExchangeRatio = rs.GetValue_INT("ExchangeRatio");

	//其他设置
	GameServerResult.lMagicExpScore = rs.GetValue_LONGLONG("MagicExpScore");
	GameServerResult.lGratuityScore = rs.GetValue_LONGLONG("GratuityScore");
	GameServerResult.dwWinExperience = rs.GetValue_LONGLONG("WinExperience");
	GameServerResult.dwFailExperience = rs.GetValue_LONGLONG("FailExperience");

	//会员限制
	GameServerResult.cbMinEnterMember=rs.GetValue_BYTE("MinEnterMember");
	GameServerResult.cbMaxEnterMember=rs.GetValue_BYTE("MaxEnterMember");

	//房间配置
	GameServerResult.dwServerRule=rs.GetValue_DWORD("ServerRule");
	GameServerResult.dwAttachUserRight=rs.GetValue_DWORD("AttachUserRight");

	//房间属性
	GameServerResult.wMaxPlayer=rs.GetValue_WORD("MaxPlayer");
	GameServerResult.wTableCount=rs.GetValue_WORD("TableCount");
	GameServerResult.wServerPort=rs.GetValue_WORD("ServerPort");
	GameServerResult.wServerKind=rs.GetValue_WORD("ServerKind");
	GameServerResult.wServerType=rs.GetValue_WORD("ServerType");
	rs.GetValue_String("ServerName",GameServerResult.szServerName,CountArray(GameServerResult.szServerName));
	rs.GetValue_String("ServerPasswd",GameServerResult.szServerPasswd,CountArray(GameServerResult.szServerPasswd));

	//分组属性
	GameServerResult.cbDistributeRule=rs.GetValue_BYTE("DistributeRule");
	GameServerResult.wMinDistributeUser=rs.GetValue_WORD("MinDistributeUser");
	GameServerResult.wDistributeTimeSpace=rs.GetValue_WORD("DistributeTimeSpace");
	GameServerResult.wDistributeDrawCount=rs.GetValue_WORD("DistributeDrawCount");
	GameServerResult.wMinPartakeGameUser=rs.GetValue_WORD("MinPartakeGameUser");
	GameServerResult.wMaxPartakeGameUser=rs.GetValue_WORD("MaxPartakeGameUser");

	////数据属性
	//rs.GetValue_String("DataBaseName",GameServerResult.szDataBaseName,CountArray(GameServerResult.szDataBaseName));
	//rs.GetValue_String("DataBaseAddr",GameServerResult.szDataBaseAddr,CountArray(GameServerResult.szDataBaseAddr));

	////缓存配置
	//rs.GetValue_String("CacheServerAddr", GameServerResult.szCacheServerAddr, CountArray(GameServerResult.szCacheServerAddr));
	//GameServerResult.wCacheServerPort = rs.GetValue_WORD("CacheServerPort");

	//扩展配置
	TCHAR szCustomRule[CountArray(GameServerResult.cbCustomRule)*2+1];
	rs.GetValue_String("CustomRule",szCustomRule,CountArray(szCustomRule));
	rs.GetValue_String("ServiceMachine",GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));

	//扩展配置
	if (szCustomRule[0]!=0)
	{
		//获取长度
		ASSERT(lstrlen(szCustomRule)%2==0);
		INT nCustomRuleSize=lstrlen(szCustomRule)/2;

		//转换字符
		for (INT i=0;i<nCustomRuleSize;i++)
		{
			//获取字符
			TCHAR cbChar1=szCustomRule[i*2];
			TCHAR cbChar2=szCustomRule[i*2+1];

			//效验字符
			ASSERT((cbChar1>=TEXT('0'))&&(cbChar1<=TEXT('9'))||(cbChar1>=TEXT('A'))&&(cbChar1<=TEXT('F')));
			ASSERT((cbChar2>=TEXT('0'))&&(cbChar2<=TEXT('9'))||(cbChar2>=TEXT('A'))&&(cbChar2<=TEXT('F')));

			//生成结果
			if ((cbChar2 >= TEXT('0')) && (cbChar2 <= TEXT('9')))
			{
				GameServerResult.cbCustomRule[i] += (cbChar2 - TEXT('0'));
			}

			if ((cbChar2 >= TEXT('A')) && (cbChar2 <= TEXT('F')))
			{
				GameServerResult.cbCustomRule[i] += (cbChar2 - TEXT('A') + 0x0A);
			}

			//生成结果
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

	//本地版本
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CYSService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	return true;
}

//读取比赛
bool CServerInfoManager::ReadGameMatchOption(SQLRecordset & rs, tagGameMatchOption & GameMatchResult)
{
	//设置变量
	ZeroMemory(&GameMatchResult,sizeof(GameMatchResult));

	//索引变量
	GameMatchResult.dwMatchID=rs.GetValue_DWORD("MatchID");
	GameMatchResult.dwMatchNO=rs.GetValue_DWORD("MatchNo");

	//费用信息
	GameMatchResult.cbMatchFeeType= rs.GetValue_BYTE("MatchFeeType");
	GameMatchResult.lMatchFee= rs.GetValue_LONGLONG("MatchFee");

	//比赛信息
	GameMatchResult.cbMemberOrder = rs.GetValue_BYTE("MemberOrder");
	GameMatchResult.cbMatchType = rs.GetValue_BYTE("MatchType");
	rs.GetValue_String("MatchName",GameMatchResult.szMatchName,CountArray(GameMatchResult.szMatchName));

	//定时赛制
	if(GameMatchResult.cbMatchType==MATCH_TYPE_LOCKTIME)
	{
		//比赛局数
		GameMatchResult.MatchType.LockTime.wMatchPlayCount= rs.GetValue_WORD("MinPlayCount");

		//时间配置
		rs.GetValue_SystemTime("StartTime",GameMatchResult.MatchType.LockTime.MatchStartTime);
		rs.GetValue_SystemTime("EndTime",GameMatchResult.MatchType.LockTime.MatchEndTime);

		//比赛分数
		GameMatchResult.MatchType.LockTime.lMatchInitScore = rs.GetValue_LONGLONG("InitScore");
		GameMatchResult.MatchType.LockTime.lMatchCullScore = rs.GetValue_LONGLONG("CullScore");
	}

	//定时赛制
	if(GameMatchResult.cbMatchType==MATCH_TYPE_IMMEDIATE)
	{
		//人数信息
		GameMatchResult.MatchType.Immediate.wStartUserCount = rs.GetValue_WORD("StartUserCount");
		GameMatchResult.MatchType.Immediate.wAndroidUserCount = rs.GetValue_WORD("AndroidUserCount");

		//分数信息
		GameMatchResult.MatchType.Immediate.dwInitalBase = rs.GetValue_DWORD("InitialBase");
		GameMatchResult.MatchType.Immediate.dwInitalScore = rs.GetValue_DWORD("InitialScore");
		GameMatchResult.MatchType.Immediate.wMinEnterGold = rs.GetValue_WORD("MinEnterGold");

		//局数信息
		GameMatchResult.MatchType.Immediate.cbPlayCount = rs.GetValue_BYTE("PlayCount");
		GameMatchResult.MatchType.Immediate.cbSwitchTableCount = rs.GetValue_BYTE("SwitchTableCount");
		GameMatchResult.MatchType.Immediate.wPrecedeTimer = rs.GetValue_WORD("PrecedeTimer");
	}
	
	return true;
}

//读取参数
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
