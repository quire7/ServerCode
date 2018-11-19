#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//配置变量
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIDataBaseEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pIGameDataBaseEngineSink=NULL;
	m_pIDBCorrespondManager=NULL;

	//辅助变量
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);
	m_wArrDailyTaskID.RemoveAll();

	return;
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBase(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	ASSERT(m_TreasureDBModule.GetInterface() != NULL);
	if (m_TreasureDBModule.GetInterface() == NULL) return NULL;

	//查询对象
	IDataBase * pIDataBase = m_TreasureDBModule.GetInterface();
	VOID * pIQueryObject=pIDataBase->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//获取对象
VOID * CDataBaseEngineSink::GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer)
{
	//效验状态
	ASSERT(m_pIDataBaseEngine!=NULL);
	if (m_pIDataBaseEngine==NULL) return NULL;

	//查询对象
	VOID * pIQueryObject=m_pIDataBaseEngine->QueryInterface(Guid,dwQueryVer);

	return pIQueryObject;
}

//投递结果
bool CDataBaseEngineSink::PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return true;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//查询对象
	ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine)!=NULL);
	m_pIDataBaseEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IDataBaseEngine);

	//创建对象
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//连接游戏
	try
	{
		//连接信息
		tagDataBaseParameter * pTreasureDBParameter = &m_pInitParameter->m_TreasureDBParameter;
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountsDBParameter = &m_pInitParameter->m_AccountsDBParameter;

		//设置连接
		m_AccountsDBModule->SetDBInfo(pAccountsDBParameter->szDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName, pAccountsDBParameter->szDataBaseUser, pAccountsDBParameter->szDataBasePass);

		//设置连接
		m_TreasureDBModule->SetDBInfo(pTreasureDBParameter->szDataBaseAddr,pTreasureDBParameter->wDataBasePort,
			pTreasureDBParameter->szDataBaseName,pTreasureDBParameter->szDataBaseUser,pTreasureDBParameter->szDataBasePass);

		//设置连接
		m_PlatformDBModule->SetDBInfo(pPlatformDBParameter->szDataBaseAddr,pPlatformDBParameter->wDataBasePort,
			pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);

		//发起连接
		m_AccountsDBModule->Open();

		//发起连接
		m_TreasureDBModule->Open();

		//发起连接
		m_PlatformDBModule->Open();

		//数据钩子
		ASSERT(m_pIGameServiceManager!=NULL);
		m_pIGameDataBaseEngineSink=(IGameDataBaseEngineSink *)m_pIGameServiceManager->CreateGameDataBaseEngineSink(IID_IGameDataBaseEngineSink,VER_IGameDataBaseEngineSink);

		//配置对象
		if ((m_pIGameDataBaseEngineSink!=NULL)&&(m_pIGameDataBaseEngineSink->InitializeSink(QUERY_ME_INTERFACE(IUnknownEx))==false))
		{
			//错误断言
			ASSERT(FALSE);

			//输出信息
			CTraceService::TraceString(TEXT("游戏数据库扩展钩子引擎对象配置失败"),TraceLevel_Exception);

			return false;
		}

#ifdef USE_REDIS
		//创建缓存连接
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
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//解锁机器
	UnLockAndroidUser();

	//配置变量
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pIGameServiceManager=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//释放对象
	SafeRelease(m_pIGameDataBaseEngineSink);

	//关闭连接
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->Close();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->Close();
		m_TreasureDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->Close();
		m_PlatformDBModule.CloseInstance();
	}

#ifdef USE_REDIS
	//销毁缓存连接
	m_RedisInstance.disConnect();
#endif

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//请求处理
	switch (wRequestID)
	{
	case DBR_GR_LOGON_USERID:			//I D 登录
		{
			bSucceed = OnRequestLogonUserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOGON_RELINE:
		{
			bSucceed = OnRequestLogonReLine(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_WRITE_GAME_SCORE:		//游戏写分
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:		//离开房间
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:		//游戏记录
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_PARAMETER:	//加载配置
		{
			bSucceed = OnRequestLoadAndroidParameter(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_ANDROID_USER:		//加载机器
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_GAME_PROPERTY:		//加载道具
		{
			bSucceed = OnRequestLoadGameProperty(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_USER_PROPERTY:		//加载用户道具
		{
			bSucceed = OnRequestLoadUserProperty(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_USER_SAVE_SCORE:		//存入游戏币
		{
			bSucceed = OnRequestUserSaveScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_USER_TAKE_SCORE:		//提取游戏币
		{
			bSucceed = OnRequestUserTakeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_QUERY_INSURE_INFO:		//查询银行
		{
			bSucceed = OnRequestQueryInsureInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_PROPERTY_REQUEST:		//道具请求
		{
			bSucceed = OnRequestPropertyRequest(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LOAD_SYSTEM_MESSAGE:   //系统消息
		{
			bSucceed = OnRequestLoadSystemMessage(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_SIGNUP:			//比赛费用
		{
			bSucceed = OnRequestMatchSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_UNSIGNUP:			//退出比赛
		{
			bSucceed = OnRequestMatchUnSignup(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_START:			//比赛开始
		{
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_ELIMINATE:		//比赛淘汰
		{
			bSucceed = OnRequestMatchEliminate(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_OVER:				//比赛结束
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);
		}
		break;	
	case DBR_GR_MATCH_REWARD:			//比赛奖励
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;	
	case DBR_GR_TASK_LOAD_LIST:			//加载任务
		{
			bSucceed = OnRequestLoadTaskList(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_QUERY_INFO:		//查询任务
		{
			bSucceed = OnRequestTaskQueryInfo(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_REWARD:			//领取奖励
		{
			bSucceed = OnRequestTaskReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_TASK_PROGRESS:			//任务跟进
		{
			bSucceed = OnRequestTaskProgress(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_EXCHANGE_SCORE:			//兑换游戏币
		{
			bSucceed = OnRequestExchangeScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_SEND_EMAIL:				//邮件发送
		{
			bSucceed = OnRequestSendEMail(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_REFRESH_ONLINE_USER:	//刷新在线人数
		{
			bSucceed = OnRequestRefreshOnlineUser(dwContextID, pData, wDataSize, dwUserID);
		}
		break;
	case DBR_GR_PRIVATE_INFO:			//比赛奖励
		{
			bSucceed = OnRequestPrivateInfo(dwContextID, pData, wDataSize, dwUserID);
		}
	break;
	case DBR_GR_CREATE_PRIVAT:			//检查是否可以创建房间
		{
			bSucceed = OnRequestCreatePrivate(dwContextID, pData, wDataSize, dwUserID);
		}
	break;
	case DBR_GR_CREATE_PRIVAT_COST:			//扣除房费
		{
			bSucceed = OnRequestCreatePrivateCost(dwContextID, pData, wDataSize, dwUserID);
		}
	break;
	case DBR_GR_PRIVATE_GAME_RECORD:		//私人场总游戏记录;
	{
		bSucceed = OnRequestPrivateGameRecord(dwContextID, pData, wDataSize, dwUserID);
	}
	break;
	case DBR_GR_SINGLE_PRIVATE_RECORD:		//私人场单场记录;
	{
		bSucceed = OnRequestSinglePrivateGameRecord(dwContextID, pData, wDataSize, dwUserID);
	}
	break;
	}

	//协调通知
	if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnPostRequestComplete(dwUserID, bSucceed);

	return bSucceed;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch)
{
	//执行查询
	DBR_GR_LogonUserID * pLogonUserID=(DBR_GR_LogonUserID *)pData;
	dwUserID = pLogonUserID->dwUserID;

	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_LogonUserID));
	if (wDataSize!=sizeof(DBR_GR_LogonUserID)) return false;

#ifdef USE_REDIS
	//缓存获取玩家数据
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
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonUserID->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(pLogonUserID->dwUserID);
		m_AccountsDBModule->AddParameter(pLogonUserID->szPassword);
		m_AccountsDBModule->AddParameter(szClientAddr);
		m_AccountsDBModule->AddParameter(pLogonUserID->szMachineID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);

		////比赛参数
		//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		//{
		//	m_GameDBModule->AddParameter(pLogonUserID->dwMatchID);
		//	m_GameDBModule->AddParameter(pLogonUserID->dwMatchNO);	
		//}

		//输出参数
		m_AccountsDBModule->AddParameterOutput("@strErrorDescribe");
		m_AccountsDBModule->AddParameterOutput("@nRet");

		//执行查询
		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_EfficacyUserID");
		if (bResult)
		{
			//用户信息
			lstrcpyn(m_LogonSuccess.szPassword, pLogonUserID->szPassword, CountArray(m_LogonSuccess.szPassword));
			lstrcpyn(m_LogonSuccess.szMachineID, pLogonUserID->szMachineID, CountArray(m_LogonSuccess.szMachineID));

			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);

			//输出信息;
			TCHAR szDescribeString[128] = TEXT("");
			LONG lResultCode			= DB_SUCCESS;

			if (rs.GetCount() == 0)
			{
				m_AccountsDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
				SQLRecordset rsRet(m_AccountsDBModule);

				//获取信息
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
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		SQLRecordset rs;

		//错误处理
		OnLogonDisposeResult(dwContextID, rs, DB_ERROR, TEXT("由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), false);

		return false;
	}

	return true;
}

// 用户重连
bool CDataBaseEngineSink::OnRequestLogonReLine(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_RELINE_USERID));
		if (wDataSize != sizeof(DBR_GR_RELINE_USERID)) return false;
		
		//执行查询
		DBR_GR_RELINE_USERID * pLogonUserID = (DBR_GR_RELINE_USERID *)pData;
		dwUserID = pLogonUserID->dwUserID;

		//缓存获取玩家数据
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

		//构造参数
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(pLogonUserID->dwUserID);

		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadUserProperty");

		//执行成功
		if (bResult)
		{
			//变量定义
			tagUserProperty prop;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET - 1024] = { 0 };
			DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);

			//设置变量
			WORD wPropertyCount = 0;
			WORD wSendDataSize = sizeof(DBO_GR_PropertyListInfo);

			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);
			if (rs.GetCount() > 0)
			{
				//变量定义
				do
				{
					ZeroMemory(&prop, sizeof(tagUserProperty));

					//读取数据
					prop.dwDBID = rs.GetValue_WORD("id");
					prop.wPropertyUseMark = rs.GetValue_WORD("PropID");
					prop.wPropertyCount = rs.GetValue_WORD("PropCount");
					rs.GetValue_SystemTime_String("OverTime", prop.OverTime);

					//发送判断				
					if (wSendDataSize + sizeof(DBO_GR_PropertyListInfo) > sizeof(cbDataBuffer))
					{
						//设置变量
						pPropertyListInfo->wPropertyCount = wPropertyCount;

						//发送数据
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);

						//重置变量
						wPropertyCount = 0;
						wSendDataSize = sizeof(DBO_GR_PropertyListInfo);
						pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);
					}

					//拷贝数据
					CopyMemory(pDataBuffer, &prop, sizeof(tagUserProperty));

					//设置变量
					wPropertyCount++;
					wSendDataSize += sizeof(tagUserProperty);
					pDataBuffer += sizeof(tagUserProperty);

				} while (rs.Fetch());
			}

			//剩余发送
			if (wPropertyCount > 0 && wSendDataSize > 0)
			{
				//设置变量
				pPropertyListInfo->wPropertyCount = wPropertyCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//游戏写分
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBModule->ClearParameters();

		//用户信息
		m_AccountsDBModule->AddParameter(pWriteGameScore->dwUserID);
		m_AccountsDBModule->AddParameter(pWriteGameScore->dwDBQuestID);
		m_AccountsDBModule->AddParameter(pWriteGameScore->dwInoutIndex);

		//变更成绩
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

		//任务参数
		m_AccountsDBModule->AddParameter(pWriteGameScore->bTaskForward);

		////比赛参数
		//if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		//{
		//	//比赛信息
		//	m_GameDBModule->AddParameter(pWriteGameScore->dwMatchID);
		//	m_GameDBModule->AddParameter(pWriteGameScore->dwMatchNO);
		//}

		//属性信息
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_AccountsDBModule->AddParameter(szClientAddr);

		//执行查询
		m_AccountsDBModule->ExecuteProcess("GSP_GR_WriteGameScore");

		BYTE* pDt = pWriteGameScore->aryCardData;
		CString strCardData;
		strCardData.Format(_T("%d,%d,%d,%d,%d"), pDt[0], pDt[1], pDt[2], pDt[3], pDt[4]);

		UpdateCachedData(dwUserID, pWriteGameScore->VariationInfo, pWriteGameScore->cbMaxCardType, strCardData);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//离开房间
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();

		//用户信息
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwUserID);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwOnLineTimeCount);

		//系统信息
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwInoutIndex);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->dwLeaveReason);

		//记录成绩
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

		//变更成绩
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

		//最大牌型
		m_TreasureDBModule->AddParameter(pLeaveGameServer->cbMaxCardType);
		BYTE* pDt = pLeaveGameServer->aryCardData;
		CString strCardData;
		strCardData.Format(_T("%d,%d,%d,%d,%d"), pDt[0], pDt[1], pDt[2], pDt[3], pDt[4]);
		m_TreasureDBModule->AddParameter(strCardData);

		//其他参数
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pLeaveGameServer->szMachineID);

		//执行查询
		m_TreasureDBModule->ExecuteProcess("GSP_GR_LeaveGameServer");

		UpdateCachedData(dwUserID, pLeaveGameServer->VariationInfo, pLeaveGameServer->cbMaxCardType, strCardData);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//游戏记录
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
		dwUserID=INVALID_DWORD;

		//效验参数
		ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
		ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
		ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

		//效验参数
		if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
		if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
		if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

		//房间信息
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);

		//桌子信息
		m_TreasureDBModule->AddParameter(pGameScoreRecord->wTableID);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->wUserCount);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->wAndroidCount);

		//税收损耗
		m_TreasureDBModule->AddParameter(pGameScoreRecord->lWasteCount);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->lRevenueCount);

		//统计信息
		m_TreasureDBModule->AddParameter(pGameScoreRecord->dwUserMemal);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->dwPlayTimeCount);

		//时间信息
		m_TreasureDBModule->AddParameter(pGameScoreRecord->SystemTimeStart);
		m_TreasureDBModule->AddParameter(pGameScoreRecord->SystemTimeConclude);

		//执行查询
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_RecordDrawInfo");

		//写入记录
		if (bReturn)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);
			if (rs.GetCount()>0)
			{
				//获取标识
				DWORD dwDrawID = rs.GetValue_DWORD("DrawID");

				//写入记录
				for (WORD i = 0; i < pGameScoreRecord->wRecordCount; i++)
				{
					//重置参数
					m_TreasureDBModule->ClearParameters();

					//房间信息
					m_TreasureDBModule->AddParameter(dwDrawID);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwUserID);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].wChairID);

					//用户信息
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwDBQuestID);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwInoutIndex);

					//成绩信息
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].lScore);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].lGrade);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].lRevenue);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
					m_TreasureDBModule->AddParameter(pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

					//执行查询
					m_TreasureDBModule->ExecuteProcess("GSP_GR_RecordDrawScore");
				}
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//加载机器
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//参数校验
	ASSERT(sizeof(DBR_GR_LoadAndroidUser)==wDataSize);
	if(sizeof(DBR_GR_LoadAndroidUser)!=wDataSize) return false;

	try
	{		
		//提取变量
		DBR_GR_LoadAndroidUser * pLoadAndroidUser = (DBR_GR_LoadAndroidUser *)pData;
        if(pLoadAndroidUser==NULL) return false;

		//变量定义
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//设置批次
		GameAndroidInfo.dwBatchID=pLoadAndroidUser->dwBatchID;

		//用户帐户
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
			//执行查询
			GameAndroidInfo.lResultCode = rsRet.GetValue_LONG("@nRet");

				//读取信息
			for (WORD i = 0; i < CountArray(GameAndroidInfo.AndroidAccountsInfo); i++)
			{
				//结束判断
				if (i>=rs.GetCount()) break;

				//溢出效验
				ASSERT(GameAndroidInfo.wAndroidCount < CountArray(GameAndroidInfo.AndroidAccountsInfo));
				if (GameAndroidInfo.wAndroidCount >= CountArray(GameAndroidInfo.AndroidAccountsInfo)) break;

				//读取数据
				GameAndroidInfo.wAndroidCount++;
				GameAndroidInfo.AndroidAccountsInfo[i].dwUserID = rs.GetValue_DWORD("UserID");
				rs.GetValue_String("LogonPass", GameAndroidInfo.AndroidAccountsInfo[i].szPassword, CountArray(GameAndroidInfo.AndroidAccountsInfo[i].szPassword));

				//移动记录
				rs.Goto(i);
			}

			//发送信息
			WORD wHeadSize = sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
			WORD wDataSize = GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidAccountsInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO, dwContextID, &GameAndroidInfo, wHeadSize + wDataSize);

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//变量定义
		DBO_GR_GameAndroidInfo GameAndroidInfo;
		ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

		//构造变量
		GameAndroidInfo.wAndroidCount=0L;
		GameAndroidInfo.lResultCode=DB_ERROR;

		//发送信息
		WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidAccountsInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize);
	}

	return false;
}

//加载机器
bool CDataBaseEngineSink::OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//用户帐户
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(m_pGameServiceAttrib->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadAndroidConfigure");

		if (bResult)
		{
			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);

			//读取信息
			for (WORD i = 0; i < CountArray(GameAndroidParameter.AndroidParameter); i++)
			{
				//结束判断
				if (i>=rs.GetCount()) break;

				//溢出效验
				ASSERT(GameAndroidParameter.wParameterCount < CountArray(GameAndroidParameter.AndroidParameter));
				if (GameAndroidParameter.wParameterCount >= CountArray(GameAndroidParameter.AndroidParameter)) break;

				//读取数据
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

				//移动记录
				rs.Fetch();
			}

			//发送信息
			WORD wHeadSize = sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
			WORD wDataSize = GameAndroidParameter.wParameterCount*sizeof(GameAndroidParameter.AndroidParameter[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER, dwContextID, &GameAndroidParameter, wHeadSize + wDataSize);

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//变量定义
		DBO_GR_GameAndroidParameter GameAndroidParameter;
		ZeroMemory(&GameAndroidParameter,sizeof(GameAndroidParameter));

		//构造变量
		GameAndroidParameter.wParameterCount=0L;
		GameAndroidParameter.lResultCode=DB_ERROR;

		//发送信息
		WORD wHeadSize=sizeof(GameAndroidParameter)-sizeof(GameAndroidParameter.AndroidParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_ANDROID_PARAMETER,dwContextID,&GameAndroidParameter,wHeadSize);
	}

	return false;
}

//加载道具
bool CDataBaseEngineSink::OnRequestLoadGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//变量定义
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo, sizeof(GamePropertyInfo));

		//构造参数
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

			//执行查询
			GamePropertyInfo.lResultCode = rsRet.GetValue_LONG("@nRet");

			//读取信息
			for (WORD i = 0; i < CountArray(GamePropertyInfo.PropertyInfo); i++)
			{
				//结束判断
				if (i >= rs.GetCount()) break;

				//溢出效验
				ASSERT(GamePropertyInfo.cbPropertyCount < CountArray(GamePropertyInfo.PropertyInfo));
				if (GamePropertyInfo.cbPropertyCount >= CountArray(GamePropertyInfo.PropertyInfo)) break;

				//读取数据
				GamePropertyInfo.cbPropertyCount++;
				GamePropertyInfo.PropertyInfo[i].wIndex = rs.GetValue_WORD(("ID"));
				GamePropertyInfo.PropertyInfo[i].wDiscount = rs.GetValue_WORD(("Discount"));
				GamePropertyInfo.PropertyInfo[i].wIssueArea = rs.GetValue_WORD(("IssueArea"));
				GamePropertyInfo.PropertyInfo[i].dPropertyCash = rs.GetValue_DOUBLE(("Cash"));
				GamePropertyInfo.PropertyInfo[i].lPropertyGold = rs.GetValue_LONGLONG(("Gold"));
				GamePropertyInfo.PropertyInfo[i].lSendLoveLiness = rs.GetValue_LONGLONG(("SendLoveLiness"));
				GamePropertyInfo.PropertyInfo[i].lRecvLoveLiness = rs.GetValue_LONGLONG(("RecvLoveLiness"));

				//移动记录
				rs.Goto(i);
			}

			//发送信息
			WORD wHeadSize = sizeof(GamePropertyInfo) - sizeof(GamePropertyInfo.PropertyInfo);
			WORD wDataSize = GamePropertyInfo.cbPropertyCount*sizeof(GamePropertyInfo.PropertyInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO, dwContextID, &GamePropertyInfo, wHeadSize + wDataSize);
		}

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//变量定义
		DBO_GR_GamePropertyInfo GamePropertyInfo;
		ZeroMemory(&GamePropertyInfo, sizeof(GamePropertyInfo));

		//构造变量
		GamePropertyInfo.cbPropertyCount = 0L;
		GamePropertyInfo.lResultCode = DB_ERROR;

		//发送信息
		WORD wHeadSize = sizeof(GamePropertyInfo) - sizeof(GamePropertyInfo.PropertyInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_GAME_PROPERTY_INFO, dwContextID, &GamePropertyInfo, wHeadSize);

		return false;
	}

	return true;
}

//加载用户道具
bool CDataBaseEngineSink::OnRequestLoadUserProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_LOADPROPERTY));
		if (wDataSize != sizeof(DBR_GR_LOADPROPERTY)) return false;

		//请求处理
		DBR_GR_LOADPROPERTY * pLoadProperty = (DBR_GR_LOADPROPERTY *)pData;
		dwUserID = pLoadProperty->dwUserID;

		//缓存获取玩家数据
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

		//构造参数
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(pLoadProperty->dwUserID);

		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_LoadUserProperty");

		//执行成功
		if (bResult)
		{
			//变量定义
			tagUserProperty prop;
			BYTE cbDataBuffer[SOCKET_TCP_PACKET - 1024] = { 0 };
			DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);

			//设置变量
			WORD wPropertyCount = 0;
			WORD wSendDataSize = sizeof(DBO_GR_PropertyListInfo);

			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);
			if (rs.GetCount() > 0)
			{
				//变量定义
				do
				{
					ZeroMemory(&prop, sizeof(tagUserProperty));

					//读取数据
					prop.dwDBID = rs.GetValue_WORD("id");
					prop.wPropertyUseMark = rs.GetValue_WORD("PropID");
					prop.wPropertyCount = rs.GetValue_WORD("PropCount");
					//prop.dwValidNum = rs.GetValue_DWORD("LastTime");
					//rs.GetValue_SystemTime_String("UsedTime", prop.EffectTime);
					rs.GetValue_SystemTime_String("OverTime", prop.OverTime);

					//发送判断				
					if (wSendDataSize + sizeof(DBO_GR_PropertyListInfo) > sizeof(cbDataBuffer))
					{
						//设置变量
						pPropertyListInfo->wPropertyCount = wPropertyCount;

						//发送数据
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);

						//重置变量
						wPropertyCount = 0;
						wSendDataSize = sizeof(DBO_GR_PropertyListInfo);
						pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);
					}

					//拷贝数据
					CopyMemory(pDataBuffer, &prop, sizeof(tagUserProperty));

					//设置变量
					wPropertyCount++;
					wSendDataSize += sizeof(tagUserProperty);
					pDataBuffer += sizeof(tagUserProperty);

				} while (rs.Fetch());
			}

			//剩余发送
			if (wPropertyCount > 0 && wSendDataSize > 0)
			{
				//设置变量
				pPropertyListInfo->wPropertyCount = wPropertyCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//存入游戏币
bool CDataBaseEngineSink::OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserSaveScore));
	if (wDataSize!=sizeof(DBR_GR_UserSaveScore)) return false;

	//变量定义
	DBR_GR_UserSaveScore * pUserSaveScore=(DBR_GR_UserSaveScore *)pData;
	dwUserID=pUserSaveScore->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserSaveScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pUserSaveScore->dwUserID);
		m_TreasureDBModule->AddParameter(pUserSaveScore->lSaveScore);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pUserSaveScore->szMachineID);

		//输出参数
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//执行查询
		BOOL bResult = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserSaveScore");
		if ( bResult )
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_TreasureDBModule);

			//输出信息
			TCHAR szDescribeString[128] = TEXT("");

			//获取信息
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");

			OnInsureDisposeResult(dwContextID, rs, lResultCode, pUserSaveScore->lSaveScore, szDescribeString, false, pUserSaveScore->cbActivityGame);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		SQLRecordset rs;

		//错误处理
		OnInsureDisposeResult(dwContextID, rs, DB_ERROR,pUserSaveScore->lSaveScore,TEXT("由于数据库操作异常，请您稍后重试！"),false,pUserSaveScore->cbActivityGame);

		return false;
	}

	return true;
}

//提取游戏币
bool CDataBaseEngineSink::OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_UserTakeScore));
	if (wDataSize!=sizeof(DBR_GR_UserTakeScore)) return false;

	//变量定义
	DBR_GR_UserTakeScore * pUserTakeScore=(DBR_GR_UserTakeScore *)pData;
	dwUserID=pUserTakeScore->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pUserTakeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pUserTakeScore->dwUserID);
		m_TreasureDBModule->AddParameter(pUserTakeScore->lTakeScore);
		m_TreasureDBModule->AddParameter(pUserTakeScore->szPassword);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pUserTakeScore->szMachineID);

		//输出参数
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//执行查询
		BOOL bResult = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserTakeScore");

		//结果处理
		if (bResult)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_TreasureDBModule);

			//输出信息
			TCHAR szDescribeString[128] = TEXT("");
			rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");

			OnInsureDisposeResult(dwContextID, rs, lResultCode, 0L, szDescribeString, false, pUserTakeScore->cbActivityGame);
		}
	
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		SQLRecordset rs;

		//错误处理
		OnInsureDisposeResult(dwContextID,rs, DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false,pUserTakeScore->cbActivityGame);

		return false;
	}

	return true;
}

//查询银行
bool CDataBaseEngineSink::OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_QueryInsureInfo));
	if (wDataSize!=sizeof(DBR_GR_QueryInsureInfo)) return false;

	//请求处理
	DBR_GR_QueryInsureInfo * pQueryInsureInfo=(DBR_GR_QueryInsureInfo *)pData;
	dwUserID=pQueryInsureInfo->dwUserID;

	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pQueryInsureInfo->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pQueryInsureInfo->dwUserID);
		m_TreasureDBModule->AddParameter(pQueryInsureInfo->szPassword);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//结果处理
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
				//变量定义
				DBO_GR_UserInsureInfo UserInsureInfo;
				ZeroMemory(&UserInsureInfo, sizeof(UserInsureInfo));

				//银行信息
				UserInsureInfo.cbActivityGame = pQueryInsureInfo->cbActivityGame;
				UserInsureInfo.cbEnjoinTransfer = rs.GetValue_BYTE("EnjoinTransfer");
				UserInsureInfo.wRevenueTake = rs.GetValue_WORD("RevenueTake");
				UserInsureInfo.wRevenueTransfer = rs.GetValue_WORD("RevenueTransfer");
				UserInsureInfo.wRevenueTransferMember = rs.GetValue_WORD("RevenueTransferMember");
				UserInsureInfo.wServerID = rs.GetValue_WORD("ServerID");
				UserInsureInfo.lUserScore = rs.GetValue_LONGLONG("Score");
				UserInsureInfo.lUserInsure = rs.GetValue_LONGLONG("Insure");
				UserInsureInfo.lTransferPrerequisite = rs.GetValue_LONGLONG("TransferPrerequisite");

				//发送结果
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_INFO, dwContextID, &UserInsureInfo, sizeof(UserInsureInfo));
			}
			else
			{
				//获取参数
				TCHAR szDescribeString[128] = TEXT("");

				//获取信息
				rsRet.GetValue_String("@strErrorDescribe", szDescribeString, CountArray(szDescribeString));

				//错误处理
				OnInsureDisposeResult(dwContextID, rs, lResultCode, 0L, szDescribeString, false, pQueryInsureInfo->cbActivityGame);
			}

			return true;
		}
		ASSERT(FALSE);
		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//结果处理
		SQLRecordset rs;
		OnInsureDisposeResult(dwContextID, rs, DB_ERROR, 0L, TEXT("由于数据库操作异常，请您稍后重试！"),false,pQueryInsureInfo->cbActivityGame);

		return false;
	}

	return true;
}

//任务跟进
bool CDataBaseEngineSink::OnRequestTaskProgress(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_TaskProgress));
		if (wDataSize != sizeof(DBR_GR_TaskProgress)) return false;

		DBR_GR_TaskProgress * pTaskProgress = (DBR_GR_TaskProgress*)pData;
		dwUserID = pTaskProgress->dwUserID;

		//构造参数
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter(pTaskProgress->dwUserID);
		m_PlatformDBModule->AddParameter(pTaskProgress->dwID);
		m_PlatformDBModule->AddParameter(pTaskProgress->wTaskProgress);
		m_PlatformDBModule->AddParameter(pTaskProgress->cbTaskStatus);
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//执行脚本
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_TaskForward");

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
	return true;
}

//领取奖励
bool CDataBaseEngineSink::OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_TaskReward));
		if (wDataSize!=sizeof(DBR_GR_TaskReward)) return false;

		//请求处理
		DBR_GR_TaskReward * pTaskReward=(DBR_GR_TaskReward *)pData;

		//设置变量
		dwUserID = pTaskReward->dwUserID;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pTaskReward->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter(pTaskReward->dwUserID);
		m_PlatformDBModule->AddParameter(pTaskReward->wTaskID);
		m_PlatformDBModule->AddParameter(pTaskReward->szPassword);
		m_PlatformDBModule->AddParameter(szClientAddr);
		m_PlatformDBModule->AddParameter(pTaskReward->szMachineID);
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//执行脚本
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_TaskReward");

		if ( bResult )
		{
			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);

			//变量定义
			DBO_GR_TaskResult TaskResult;
			ZeroMemory(&TaskResult, sizeof(TaskResult));

			m_PlatformDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
			SQLRecordset rsRet(m_PlatformDBModule);

			//获取参数
			LONG lResultCode = rsRet.GetValue_LONG("@nRet");
			rsRet.GetValue_String("@strErrorDescribe", TaskResult.szNotifyContent, CountArray(TaskResult.szNotifyContent));

			//银行信息
			TaskResult.wCommandID = SUB_GR_TASK_REWARD;
			TaskResult.wCurrTaskID = pTaskReward->wTaskID;
			TaskResult.bSuccessed = lResultCode == DB_SUCCESS;

			//获取分数
			if (TaskResult.bSuccessed == true)
			{
				TaskResult.lCurrScore = rs.GetValue_LONGLONG("Score");
				TaskResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
			}

			//发送结果
			WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);

			return true;
		}
		ASSERT(FALSE);
		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_REWARD;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//加载任务
bool CDataBaseEngineSink::OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		m_wArrDailyTaskID.RemoveAll();

		//设置变量
		dwUserID = 0;

		//构造参数
		m_PlatformDBModule->ClearParameters();

		//执行命令
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_LoadTaskList");

		//执行成功
		if (bResult)
		{
			//变量定义
			tagTaskParameter TaskParameter;	
			BYTE cbDataBuffer[SOCKET_TCP_PACKET-1024]={0};					
			DBO_GR_TaskListInfo * pTaskListInfo = (DBO_GR_TaskListInfo *)cbDataBuffer;
			LPBYTE pDataBuffer=cbDataBuffer+sizeof(DBO_GR_TaskListInfo);

			//设置变量
			WORD wTaskCount=0;
			WORD wSendDataSize=sizeof(DBO_GR_TaskListInfo);

			CleanupResults objCleanup(m_PlatformDBModule);
			SQLRecordset rs(m_PlatformDBModule);
			if ( rs.GetCount()>0 )
			{
				//变量定义
				do
				{
					//读取数据
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

					//描述信息
					//rs.GetValue_String("TaskName", TaskParameter.szTaskName, CountArray(TaskParameter.szTaskName));
					//rs.GetValue_String("TaskDescription", TaskParameter.szTaskDescribe, CountArray(TaskParameter.szTaskDescribe));

					//发送判断				
					if (wSendDataSize + sizeof(tagTaskParameter) > sizeof(cbDataBuffer))
					{
						//设置变量
						pTaskListInfo->wTaskCount = wTaskCount;

						//发送数据
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST, dwContextID, pTaskListInfo, wSendDataSize);

						//重置变量
						wTaskCount = 0;
						wSendDataSize = sizeof(DBO_GR_TaskListInfo);
						pDataBuffer = cbDataBuffer + sizeof(DBO_GR_TaskListInfo);
					}

					//拷贝数据
					CopyMemory(pDataBuffer, &TaskParameter, sizeof(tagTaskParameter));

					//设置变量
					wTaskCount++;
					wSendDataSize += sizeof(tagTaskParameter);
					pDataBuffer += sizeof(tagTaskParameter);

					m_wArrDailyTaskID.Add(TaskParameter.wTaskID);

				} while (rs.Fetch());
			}

			//剩余发送
			if(wTaskCount>0 && wSendDataSize>0)
			{
				//设置变量
				pTaskListInfo->wTaskCount = wTaskCount;

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST,dwContextID,pTaskListInfo,wSendDataSize);
			}

			//发送通知
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST_END,dwContextID,NULL,0);						
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		
		//发送通知
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_LIST_END, dwContextID, NULL, 0);

		return false;
	}

	return true;
}

//查询任务
bool CDataBaseEngineSink::OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_TaskQueryInfo));
		if (wDataSize!=sizeof(DBR_GR_TaskQueryInfo)) return false;

		//请求处理
		DBR_GR_TaskQueryInfo * pTaskQueryInfo=(DBR_GR_TaskQueryInfo *)pData;

		//设置变量
		dwUserID = pTaskQueryInfo->dwUserID;

		//构造参数
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter((WORD)0);
		m_PlatformDBModule->AddParameter(pTaskQueryInfo->dwUserID);
		m_PlatformDBModule->AddParameter(pTaskQueryInfo->szPassword);

		//输出参数
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//执行脚本
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_QueryTaskInfo");

		//执行成功
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
				//变量定义
				DBO_GR_TaskInfo TaskInfo;
				tagTaskStatus * pTaskStatus = NULL;
				ZeroMemory(&TaskInfo, sizeof(TaskInfo));

				for (int i = 0; i < rs.GetCount(); i++)
				{
					rs.Goto(i);

					//设置变量
					pTaskStatus = &TaskInfo.TaskStatus[TaskInfo.wTaskCount++];

					//读取数据
					pTaskStatus->wTaskID = rs.GetValue_WORD("TaskID");
					pTaskStatus->cbTaskStatus = rs.GetValue_BYTE("TaskStatus");
					pTaskStatus->wTaskProgress = rs.GetValue_WORD("Progress");
				}

				//发送结果
				WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
				wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_INFO, dwContextID, &TaskInfo, wSendDataSize);
			}
			else
			{
				//变量定义
				DBO_GR_TaskResult TaskResult;
				ZeroMemory(&TaskResult, sizeof(TaskResult));

				//获取信息
				rsRet.GetValue_String("@strErrorDescribe", TaskResult.szNotifyContent, CountArray(TaskResult.szNotifyContent));

				//银行信息
				TaskResult.bSuccessed = false;
				TaskResult.wCommandID = SUB_GR_TASK_LOAD_INFO;

				//发送结果
				WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);
			}

			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed=false;
		TaskResult.wCommandID=SUB_GR_TASK_LOAD_INFO;
		lstrcpyn(TaskResult.szNotifyContent,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize=sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT,dwContextID,&TaskResult,wSendSize);

		return false;
	}
}

//兑换游戏币
bool CDataBaseEngineSink::OnRequestExchangeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_ExchangeScore));
		if (wDataSize!=sizeof(DBR_GR_ExchangeScore)) return false;

		//请求处理
		DBR_GR_ExchangeScore * pExchangeScore=(DBR_GR_ExchangeScore *)pData;

		//设置变量
		dwUserID = pExchangeScore->dwUserID;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pExchangeScore->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pExchangeScore->dwUserID);
		m_TreasureDBModule->AddParameter(pExchangeScore->lExchangeIngot);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pExchangeScore->szMachineID);

		//输出参数
		m_TreasureDBModule->AddParameterOutput("@strNotifyContent");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//执行脚本
		BOOL bResultCode = m_TreasureDBModule->ExecuteProcess(("GSP_GR_ExchangeScore"));

		//构造结构
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//执行成功
		if (bResultCode)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			//设置变量
			ExchangeResult.bSuccessed=true;

			//变量定义
			if (rs.GetCount()>0)
			{
				//读取数据
				ExchangeResult.lCurrScore = rs.GetValue_LONGLONG("CurrScore");
				ExchangeResult.lCurrIngot = rs.GetValue_LONGLONG("CurrIngot");
			}
			else
			{
				m_TreasureDBModule->ExecuteSQL("select @strErrorDescribe,@nRet");
				SQLRecordset rsRet(m_TreasureDBModule);

				//提示内容
				rsRet.GetValue_String("@strNotifyContent", ExchangeResult.szNotifyContent, CountArray(ExchangeResult.szNotifyContent));
			}
		}

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GR_ExchangeResult ExchangeResult;
		ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

		//设置变量
		lstrcpyn(ExchangeResult.szNotifyContent,TEXT("数据库异常，请稍后再试！"),CountArray(ExchangeResult.szNotifyContent));

		//计算大小
		WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
		wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);

		//发送参数
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_EXCHANGE_RESULT,dwContextID,&ExchangeResult,wSendDataSize);

		return false;
	}
}

//道具请求
bool CDataBaseEngineSink::OnRequestPropertyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_PropertyRequest));
		if (wDataSize!=sizeof(DBR_GR_PropertyRequest)) return false;

		//请求处理
		DBR_GR_PropertyRequest * pPropertyRequest=(DBR_GR_PropertyRequest *)pData;
		dwUserID=pPropertyRequest->dwSourceUserID;

		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pPropertyRequest->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_AccountsDBModule->ClearParameters();

		//消费信息
		m_AccountsDBModule->AddParameter(pPropertyRequest->dwSourceUserID);
		m_AccountsDBModule->AddParameter(pPropertyRequest->dwTargetUserID);
		m_AccountsDBModule->AddParameter(pPropertyRequest->wPropertyIndex);
		m_AccountsDBModule->AddParameter(pPropertyRequest->wItemCount);

		//消费区域
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_AccountsDBModule->AddParameter(pPropertyRequest->wTableID);

		////购买方式
		//m_AccountsDBModule->AddParameter(pPropertyRequest->cbConsumeScore);
		//m_AccountsDBModule->AddParameter(pPropertyRequest->lFrozenedScore);

		//系统信息
		m_AccountsDBModule->AddParameter(pPropertyRequest->dwInoutIndex);
		m_AccountsDBModule->AddParameter(szClientAddr);
		m_AccountsDBModule->AddParameter(pPropertyRequest->UsedTime);
		m_AccountsDBModule->AddParameter(pPropertyRequest->szMachineID);

		//输出参数
		m_AccountsDBModule->AddParameterOutput("@strErrorDescribe");
		m_AccountsDBModule->AddParameterOutput("@nRet");

		BOOL bReturn = m_AccountsDBModule->ExecuteProcess("GSP_GR_ConsumeProperty");

		ModifyUserPropNumFromRedis(pPropertyRequest->dwDBID, pPropertyRequest->dwSourceUserID, pPropertyRequest->wPropertyIndex, 0 - pPropertyRequest->wItemCount);
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//比赛报名
bool CDataBaseEngineSink::OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchSignup)) return false;

	//变量定义
	DBR_GR_MatchSignup * pMatchSignup=(DBR_GR_MatchSignup *)pData;
	dwUserID=pMatchSignup->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchSignup->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchSignup->lMatchFee);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchSignup->szMachineID);

		//输出参数
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//结果处理
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserMatchFee");

		//构造结构
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

				//设置变量
				MatchSignupResult.bResultCode = FALSE;

				//获取信息
				rsRet.GetValue_String("@strErrorDescribe", MatchSignupResult.szDescribeString, CountArray(MatchSignupResult.szDescribeString));
			}
			else
			{
				//设置变量
				MatchSignupResult.bResultCode = TRUE;
				MatchSignupResult.lCurrGold = rs.GetValue_LONGLONG("Score");
				MatchSignupResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
			}
		}

		//发送结果
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//设置变量		
		MatchSignupResult.bResultCode=false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(MatchSignupResult.szDescribeString));

		//发送结果
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}

	return true;
}

//退出比赛
bool CDataBaseEngineSink::OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchSignup));
	if (wDataSize!=sizeof(DBR_GR_MatchSignup)) return false;

	//变量定义
	DBR_GR_MatchSignup * pMatchSignup=(DBR_GR_MatchSignup *)pData;
	dwUserID=pMatchSignup->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchSignup->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchSignup->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchSignup->lMatchFee);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchSignup->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchSignup->szMachineID);

		//输出参数
		m_TreasureDBModule->AddParameterOutput("@strErrorDescribe");
		m_TreasureDBModule->AddParameterOutput("@nRet");

		//结果处理
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_UserMatchQuit");

		//构造结构
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

				//设置变量
				MatchSignupResult.bResultCode = FALSE;

				//获取信息
				rsRet.GetValue_String("@strErrorDescribe", MatchSignupResult.szDescribeString, CountArray(MatchSignupResult.szDescribeString));
			}
			else
			{
				//设置变量
				MatchSignupResult.bResultCode = TRUE;

				MatchSignupResult.lCurrGold = rs.GetValue_LONGLONG("Score");
				MatchSignupResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
			}
		}

		//发送结果
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_UNSIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBO_GR_MatchSingupResult MatchSignupResult;
		ZeroMemory(&MatchSignupResult,sizeof(MatchSignupResult));

		//设置变量
		MatchSignupResult.bResultCode=false;
		lstrcpyn(MatchSignupResult.szDescribeString,TEXT("由于数据库操作异常，请您稍后重试！"),CountArray(MatchSignupResult.szDescribeString));

		//发送结果
		WORD wSendDataSize=sizeof(MatchSignupResult)-sizeof(MatchSignupResult.szDescribeString);
		wSendDataSize+=CountStringBuffer(MatchSignupResult.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_SIGNUP_RESULT,dwContextID,&MatchSignupResult,wSendDataSize);

		return false;
	}
}

//比赛开始
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//变量定义
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;

	//请求处理
	try
	{
		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchStart->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchStart->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchStart->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchStart->cbMatchType);

		//结果处理
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchStart");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		SQLRecordset rs;
		OnInsureDisposeResult(dwContextID,rs, DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//比赛结束
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//变量定义
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;

	//请求处理
	try
	{
		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchOver->wServerID);
		m_TreasureDBModule->AddParameter(pMatchOver->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchOver->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchOver->cbMatchType);
		m_TreasureDBModule->AddParameter(pMatchOver->MatchStartTime);
		m_TreasureDBModule->AddParameter(pMatchOver->MatchEndTime);

		//结果处理
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchOver");

		//执行成功
		if(bReturn)
		{
			//构造结构
			DBO_GR_MatchRankList MatchRankList;
			ZeroMemory(&MatchRankList,sizeof(MatchRankList));

			//变量定义
			tagMatchRankInfo * pMatchRankInfo=NULL;

			//设置变量
			MatchRankList.dwMatchID=pMatchOver->dwMatchID;
			MatchRankList.dwMatchNO=pMatchOver->dwMatchNO;

			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount() > 0)
			{
				//读取记录
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

			//发送数据
			WORD wSendDataSize = sizeof(MatchRankList)-sizeof(MatchRankList.MatchRankInfo);
			wSendDataSize += MatchRankList.wUserCount*sizeof(MatchRankList.MatchRankInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_RANK_LIST,dwContextID,&MatchRankList,wSendDataSize);
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//比赛奖励
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//变量定义
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=pMatchReward->dwUserID;

	//请求处理
	try
	{
		//转化地址
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchReward->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchReward->lRewardGold);
		m_TreasureDBModule->AddParameter(pMatchReward->dwRewardIngot);
		m_TreasureDBModule->AddParameter(pMatchReward->dwRewardExperience);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wKindID);
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);
		m_TreasureDBModule->AddParameter(szClientAddr);

		//构造结构
		DBR_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchReward");

		//结果处理
		if ( bReturn )
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount() == 0)
			{
				//设置变量
				MatchRewardResult.bResultCode = FALSE;
			}
			else
			{
				//设置变量
				MatchRewardResult.bResultCode = TRUE;

				//读取财富
				MatchRewardResult.lCurrGold = rs.GetValue_LONGLONG("Score");
				MatchRewardResult.lCurrIngot = rs.GetValue_LONGLONG("Ingot");
				MatchRewardResult.dwCurrExperience = rs.GetValue_DWORD("Experience");
			}
		}
		else
		{
			//设置变量
			MatchRewardResult.bResultCode = FALSE;
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//构造结构
		DBR_GR_MatchRewardResult MatchRewardResult;
		ZeroMemory(&MatchRewardResult,sizeof(MatchRewardResult));

		//设置变量
		MatchRewardResult.bResultCode=false;

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MATCH_REWARD_RESULT,dwContextID,&MatchRewardResult,sizeof(MatchRewardResult));

		return false;
	}

	return true;
}

//比赛淘汰
bool CDataBaseEngineSink::OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(DBR_GR_MatchEliminate));
	if (wDataSize!=sizeof(DBR_GR_MatchEliminate)) return false;

	//变量定义
	DBR_GR_MatchEliminate * pMatchEliminate=(DBR_GR_MatchEliminate *)pData;

	//请求处理
	try
	{
		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(pMatchEliminate->dwUserID);
		m_TreasureDBModule->AddParameter(pMatchEliminate->wServerID);
		m_TreasureDBModule->AddParameter(pMatchEliminate->dwMatchID);
		m_TreasureDBModule->AddParameter(pMatchEliminate->dwMatchNO);
		m_TreasureDBModule->AddParameter(pMatchEliminate->cbMatchType);

		//结果处理
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_MatchEliminate");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//系统消息
bool CDataBaseEngineSink::OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	try
	{
		//构造参数
		m_TreasureDBModule->ClearParameters();
		m_TreasureDBModule->AddParameter(m_pGameServiceOption->wServerID);

		//执行查询
		BOOL bReturn = m_TreasureDBModule->ExecuteProcess("GSP_GR_LoadSystemMessage");

		//结果处理
		if (bReturn)
		{
			CleanupResults objCleanup(m_TreasureDBModule);
			SQLRecordset rs(m_TreasureDBModule);

			if (rs.GetCount()>0)
			{
				do
				{
					//定义变量
					TCHAR szServerRange[1024] = { 0 };
					CString strServerRange;
					bool bSendMessage = false;
					bool bAllRoom = false;

					//读取范围
					rs.GetValue_String("ServerRange", szServerRange, CountArray(szServerRange));
					szServerRange[1023] = 0;
					strServerRange.Format(TEXT("%s"), szServerRange);

					//范围判断
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

					//发送消息
					if (bSendMessage)
					{
						//定义变量
						DBR_GR_SystemMessage SystemMessage;
						ZeroMemory(&SystemMessage, sizeof(SystemMessage));

						//读取消息
						SystemMessage.dwMessageID = rs.GetValue_DWORD("ID");
						SystemMessage.cbMessageType = rs.GetValue_BYTE("MessageType");
						SystemMessage.dwTimeRate = rs.GetValue_DWORD("TimeRate");
						SystemMessage.cbAllRoom = bAllRoom ? TRUE : FALSE;
						rs.GetValue_String("MessageString", SystemMessage.szSystemMessage, CountArray(SystemMessage.szSystemMessage));

						//读取时间
						SYSTEMTIME systime;
						ZeroMemory(&systime, sizeof(systime));

						//开始时间
						rs.GetValue_SystemTime("StartTime", systime);
						CTime StarTime(systime);
						SystemMessage.tStartTime = StarTime.GetTime();

						//结束时间
						rs.GetValue_SystemTime("ConcludeTime", systime);
						CTime ConcludeTime(systime);
						SystemMessage.tConcludeTime = ConcludeTime.GetTime();

						//发送结果
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_RESULT, dwContextID, &SystemMessage, sizeof(SystemMessage));
					}
				} while (rs.Fetch());
			}

			//加载完成
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYSTEM_MESSAGE_FINISH,dwContextID,NULL,0);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//错误处理
		SQLRecordset rs;
		OnInsureDisposeResult(dwContextID,rs,DB_ERROR,0L,TEXT("由于数据库操作异常，请您稍后重试！"),false);

		return false;
	}

	return true;
}

//解锁机器
bool CDataBaseEngineSink::UnLockAndroidUser()
{
	try
	{
		//构造参数
		m_AccountsDBModule->ClearParameters();
		m_AccountsDBModule->AddParameter(m_pGameServiceOption->wServerID);

		//执行查询
		LONG lResultCode = m_AccountsDBModule->ExecuteProcess("GSP_GR_UnlockAndroidUser");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
	}

	return false;
}

//登录结果
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

		//救济金
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
		//任务变量
		m_LogonSuccess.wTaskCount = 0;
		//不是机器人
		if (isAndroid == 0)
		{
			GetUserTaskInfo(dwContextID, m_pGameServiceOption->wKindID, m_LogonSuccess);

			//GetUserPropFromRedis(dwContextID, m_LogonSuccess.dwUserID);
		}

		//发送结果
		WORD wDataSize = sizeof(m_LogonSuccess.UserTaskInfo[0])*m_LogonSuccess.wTaskCount;
		WORD wHeadSize = sizeof(m_LogonSuccess)-sizeof(m_LogonSuccess.UserTaskInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS, dwContextID, &m_LogonSuccess, wHeadSize + wDataSize);
	}
	else
	{
		//变量定义
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure, sizeof(LogonFailure));

		//构造数据
		LogonFailure.lResultCode = 1;
		lstrcpyn(LogonFailure.szDescribeString, _T("密码错误"), CountArray(LogonFailure.szDescribeString));

		//发送结果
		WORD wDataSize = CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize = sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE, dwContextID, &LogonFailure, wHeadSize + wDataSize);
	}

}

//登录结果
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, SQLRecordset& inRs, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbDeviceType)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//属性资料
		m_LogonSuccess.wFaceID = inRs.GetValue_WORD("FaceID");
		m_LogonSuccess.dwUserID = inRs.GetValue_DWORD("UserID");
		m_LogonSuccess.dwCustomID = inRs.GetValue_DWORD("CustomID");
		inRs.GetValue_String("NickName", m_LogonSuccess.szNickName, CountArray(m_LogonSuccess.szNickName));

		//用户资料
		m_LogonSuccess.cbGender = inRs.GetValue_BYTE("Gender");
		m_LogonSuccess.cbMemberOrder = inRs.GetValue_BYTE("MemberOrder");
		inRs.GetValue_String("UnderWrite", m_LogonSuccess.szUnderWrite, CountArray(m_LogonSuccess.szUnderWrite));

		//积分信息
		m_LogonSuccess.lGold = inRs.GetValue_LONGLONG("Gold");
		m_LogonSuccess.lDiamond = inRs.GetValue_LONG("Diamond");
		m_LogonSuccess.lInsure = inRs.GetValue_LONGLONG("Insure");

		//局数信息
		m_LogonSuccess.dwWinCount = inRs.GetValue_LONG("WinCount");
		m_LogonSuccess.dwLostCount = inRs.GetValue_LONG("LostCount");
		m_LogonSuccess.dwDrawCount = inRs.GetValue_LONG("DrawCount");
		m_LogonSuccess.dwFleeCount = inRs.GetValue_LONG("FleeCount");
		m_LogonSuccess.dwExperience = inRs.GetValue_LONG("Experience");
		m_LogonSuccess.lLoveLiness = inRs.GetValue_LONG("LoveLiness");

		m_LogonSuccess.dwCustomFaceVer = inRs.GetValue_DWORD("CustomFaceVer");

		DWORD dwPlayTimeCount = inRs.GetValue_LONG("PlayTimeCount");
		DWORD dwOnLineTimeCount = inRs.GetValue_LONG("OnLineTimeCount");

		//最大牌型
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

		//附加信息		
		m_LogonSuccess.dwUserRight = inRs.GetValue_DWORD("UserRight");
		m_LogonSuccess.cbDeviceType=cbDeviceType;

		//救济金
		m_LogonSuccess.cbBenefitTimes = inRs.GetValue_BYTE("BenefitTimes");
		TCHAR szLastBenefitTime[32] = {0};
		inRs.GetValue_String("LastBenefitTime", szLastBenefitTime, sizeof(szLastBenefitTime));
		inRs.GetValue_SystemTime_String("LastBenefitTime", m_LogonSuccess.LastBenefitTime);

		BYTE isAndroid = inRs.GetValue_BYTE("isAndroid");

		//索引变量
		m_LogonSuccess.dwInoutIndex = inRs.GetValue_DWORD("InoutIndex");

		//获取信息
		if (pszErrorString != NULL)
		{
			lstrcpyn(m_LogonSuccess.szDescribeString, pszErrorString, CountArray(m_LogonSuccess.szDescribeString));
		}

		//任务变量
		m_LogonSuccess.wTaskCount=0;
		ZeroMemory(m_LogonSuccess.UserTaskInfo,sizeof(m_LogonSuccess.UserTaskInfo));

		//不是机器人
		if (isAndroid == 0)
		{
			GetUserTaskInfo(dwContextID, m_pGameServiceOption->wKindID, m_LogonSuccess);
		}

		//发送结果
		WORD wDataSize = sizeof(m_LogonSuccess.UserTaskInfo[0])*m_LogonSuccess.wTaskCount;
		WORD wHeadSize = sizeof(m_LogonSuccess) - sizeof(m_LogonSuccess.UserTaskInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS, dwContextID, &m_LogonSuccess, wHeadSize + wDataSize);

#ifdef USE_REDIS
		try
		{
			//更新缓存
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

				vMap["Grade"] = CYSCharUtils::ConvertToUTF8(TEXT("菜鸟")).GetBuffer();
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
		//变量定义
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//构造数据
		LogonFailure.lResultCode=dwErrorCode;
		lstrcpyn(LogonFailure.szDescribeString,pszErrorString,CountArray(LogonFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
		WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,&LogonFailure,wHeadSize+wDataSize);
	}

	return;
}

//任务查询
bool CDataBaseEngineSink::OnQueryTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess)
{
	try
	{
		//设置变量
		DWORD dwUserID = m_LogonSuccess.dwUserID;

		//构造参数
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter((WORD)0);
		m_PlatformDBModule->AddParameter(dwUserID);
		m_PlatformDBModule->AddParameter(m_LogonSuccess.szPassword);

		//输出参数
		m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
		m_PlatformDBModule->AddParameterOutput("@nRet");

		//执行脚本
		BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_QueryTaskInfo");

		//执行成功
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
				//变量定义
				tagUserTaskInfo * pUserTaskInfo = NULL;

				//循环读取
				do
				{
					//设置变量
					pUserTaskInfo = &m_LogonSuccess.UserTaskInfo[m_LogonSuccess.wTaskCount++];

					//读取数据
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
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//构造结构
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed = false;
		TaskResult.wCommandID = SUB_GR_TASK_LOAD_INFO;
		lstrcpyn(TaskResult.szNotifyContent, TEXT("由于数据库操作异常，请您稍后重试！"), CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize = sizeof(TaskResult) - sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);

		return false;
	}

	return true;
}

//初始化任务
bool CDataBaseEngineSink::InitUserTask(DWORD dwContextID, DBO_GR_LogonSuccess m_LogonSuccess)
{
	try
	{
		DBR_GR_TaskTake TaskTake;

		//查找缓冲
		for (int i = 0; i < m_wArrDailyTaskID.GetCount(); i++)
		{
			ZeroMemory(&TaskTake, sizeof(TaskTake));

			TaskTake.dwUserID = m_LogonSuccess.dwUserID;
			TaskTake.wTaskID = m_wArrDailyTaskID[i];
			CopyMemory(TaskTake.szPassword, m_LogonSuccess.szPassword, sizeof(TaskTake.szPassword));
			CopyMemory(TaskTake.szMachineID, m_LogonSuccess.szMachineID, sizeof(TaskTake.szMachineID));

			//构造参数
			m_PlatformDBModule->ClearParameters();
			m_PlatformDBModule->AddParameter(TaskTake.dwUserID);
			m_PlatformDBModule->AddParameter(TaskTake.wTaskID);
			m_PlatformDBModule->AddParameter(TaskTake.szPassword);
			m_PlatformDBModule->AddParameter("");
			m_PlatformDBModule->AddParameter(TaskTake.szMachineID);
			m_PlatformDBModule->AddParameterOutput("@strErrorDescribe");
			m_PlatformDBModule->AddParameterOutput("@nRet");

			//执行脚本
			BOOL bResult = m_PlatformDBModule->ExecuteProcess("GSP_GR_TaskTake");
			if (!bResult)
			{
				CString str;
				str.Format(TEXT("初始化玩家 %d 日常任务 %d 失败！"), TaskTake.dwUserID, TaskTake.wTaskID);
				CTraceService::TraceString(str, TraceLevel_Exception);
			}
		} 

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//构造结构
		DBO_GR_TaskResult TaskResult;
		TaskResult.bSuccessed = false;
		TaskResult.wCommandID = SUB_GR_TASK_TAKE;
		lstrcpyn(TaskResult.szNotifyContent, TEXT("由于数据库操作异常，请您稍后重试！"), CountArray(TaskResult.szNotifyContent));

		//发送结果
		WORD wSendSize = sizeof(TaskResult) - sizeof(TaskResult.szNotifyContent) + CountStringBuffer(TaskResult.szNotifyContent);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TASK_RESULT, dwContextID, &TaskResult, wSendSize);

		return false;
	}

	return true;
}

//银行结果
VOID CDataBaseEngineSink::OnInsureDisposeResult(DWORD dwContextID, SQLRecordset& inRs, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbActivityGame)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		DBO_GR_UserInsureSuccess UserInsureSuccess;
		ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

		//构造变量
		UserInsureSuccess.cbActivityGame=cbActivityGame;
		UserInsureSuccess.lFrozenedScore=lFrozenedScore;
		UserInsureSuccess.dwUserID = inRs.GetValue_DWORD("UserID");
		UserInsureSuccess.lSourceScore = inRs.GetValue_LONGLONG("SourceScore");
		UserInsureSuccess.lSourceInsure = inRs.GetValue_LONGLONG("SourceInsure");
		UserInsureSuccess.lInsureRevenue = inRs.GetValue_LONGLONG("InsureRevenue");
		UserInsureSuccess.lVariationScore = inRs.GetValue_LONGLONG("VariationScore");
		UserInsureSuccess.lVariationInsure = inRs.GetValue_LONGLONG("VariationInsure");
		lstrcpyn(UserInsureSuccess.szDescribeString,pszErrorString,CountArray(UserInsureSuccess.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureSuccess.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_SUCCESS,dwContextID,&UserInsureSuccess,wHeadSize+wDataSize);
	}
	else
	{
		//变量定义
		DBO_GR_UserInsureFailure UserInsureFailure;
		ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

		//构造变量
		UserInsureFailure.cbActivityGame=cbActivityGame;
		UserInsureFailure.lResultCode=dwErrorCode;
		UserInsureFailure.lFrozenedScore=lFrozenedScore;
		lstrcpyn(UserInsureFailure.szDescribeString,pszErrorString,CountArray(UserInsureFailure.szDescribeString));

		//发送结果
		WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
		WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_INSURE_FAILURE,dwContextID,&UserInsureFailure,wHeadSize+wDataSize);
	}

	return;
}

//更新缓存数据
void CDataBaseEngineSink::UpdateCachedData(DWORD dwUserID, tagVariationInfo& VariationInfo, BYTE cbCardType, LPCTSTR pszCardData)
{
#ifdef USE_REDIS

	try
	{
		//存入缓存
		ASSERT(m_RedisInstance.isConneced());
		if (m_RedisInstance.isConneced())
		{
			int64_t ret = 0;
			//组建KEY
			char szBuf[32] = { 0 };
			sprintf_s(szBuf, sizeof(szBuf), "id.%d", dwUserID);
			string strKey(szBuf);

			//获取HASH表
			ValueMap vMap;
			if (m_RedisInstance.hgetall(strKey, vMap))
			{
				if (VariationInfo.lScore != 0)
				{
					//更新积分缓存
					m_RedisInstance.hincrby(strKey, "Gold", (int)VariationInfo.lScore, ret);
				}

				if (VariationInfo.dwExperience!=0)
				{
					//更新经验缓存
					m_RedisInstance.hincrby(strKey, "Experience", (int)VariationInfo.dwExperience, ret);
				}

				if (VariationInfo.dwWinCount!=0)
				{
					//更新胜利局数
					m_RedisInstance.hincrby(strKey, "WinCount", (int)VariationInfo.dwWinCount, ret);
				}

				if (VariationInfo.dwLostCount!=0)
				{
					//更新失败局数
					m_RedisInstance.hincrby(strKey, "LostCount", (int)VariationInfo.dwLostCount, ret);
				}

				if (VariationInfo.dwDrawCount!=0)
				{
					//更新平局局数
					m_RedisInstance.hincrby(strKey, "DrawCount", (int)VariationInfo.dwDrawCount, ret);
				}

				if (VariationInfo.dwFleeCount!=0)
				{
					//更新放弃局数
					m_RedisInstance.hincrby(strKey, "FleeCount", (int)VariationInfo.dwFleeCount, ret);
				}

				if (VariationInfo.lLoveLiness!=0)
				{
					//更新魅力信息
					m_RedisInstance.hincrby(strKey, "LoveLiness", VariationInfo.lLoveLiness, ret);
				}

				if (VariationInfo.dwPlayTimeCount!=0)
				{
					//游戏时间
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
				//	//在线时间
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

// 发送邮件
bool CDataBaseEngineSink::OnRequestSendEMail(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//请求处理
	DBR_GR_EMAIL * pEMail = (DBR_GR_EMAIL *)pData;
	dwUserID = pEMail->dwRecUserID;

	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_EMAIL));
		if (wDataSize != sizeof(DBR_GR_EMAIL)) return false;

		//构造参数
		m_AccountsDBModule->ClearParameters();

		//用户信息
		m_AccountsDBModule->AddParameter(pEMail->dwRecUserID);
		m_AccountsDBModule->AddParameter(pEMail->cbType);
		m_AccountsDBModule->AddParameter(pEMail->createTime);
		m_AccountsDBModule->AddParameter(pEMail->szRewardProp);
		m_AccountsDBModule->AddParameter(pEMail->szEmailTitle);
		m_AccountsDBModule->AddParameter(pEMail->szEmailMsg);
		
		//输出参数
		m_AccountsDBModule->AddParameterOutput("@strErrorDescribe");
		m_AccountsDBModule->AddParameterOutput("@nRet");

		//执行查询
		BOOL bResult = m_AccountsDBModule->ExecuteProcess("GSP_GR_WriteEMail");

		//更新成功通知缓存更新
		if (bResult && pEMail->cbType == EN_EMAIL_SYSTIME)
		{
			CleanupResults objCleanup(m_AccountsDBModule);
			SQLRecordset rs(m_AccountsDBModule);

			if (rs.GetCount() > 0)
			{
				tagSendBenefitSuc Benefit;
				ZeroMemory(&Benefit, sizeof(tagSendBenefitSuc));

				//读取数据
				Benefit.dwUserID = rs.GetValue_DWORD("UserID");
				Benefit.cbBenefitTimes = rs.GetValue_WORD("BenefitTimes");
				rs.GetValue_SystemTime_String("LastBenefitTime", Benefit.LastBenefitTime);
				TCHAR szLastBenefitTime[32] = { 0 };
				rs.GetValue_String("LastBenefitTime", szLastBenefitTime, sizeof(szLastBenefitTime));

				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_GR_SEND_EMAIL_SUCCESS, dwContextID, &Benefit, sizeof(tagSendBenefitSuc));

				try
				{
					//更新缓存
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
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//刷新在线人数
bool CDataBaseEngineSink::OnRequestRefreshOnlineUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
#ifdef USE_REDIS
	//更新缓存
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
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_REREG_SERVER_IN_REDIS, dwContextID, NULL, 0);
		}
	}
#endif
	return true;
}

//获取玩家任务
void CDataBaseEngineSink::GetUserTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess)
{
	bool isQuery = OnQueryTaskInfo(dwContextID, wKindID, m_LogonSuccess);
	if (isQuery == false && m_LogonSuccess.wTaskCount <= 0)
	{
		// 插入日常任务;
		InitUserTask(dwContextID, m_LogonSuccess);

		// 重新查询任务;
		OnQueryTaskInfo(dwContextID, wKindID, m_LogonSuccess);
	}
}

//从缓存中获取道具
void CDataBaseEngineSink::GetUserPropFromRedis(DWORD dwContextID, DWORD dwUserID)
{
	char szBuf[64] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf), "UserPropertyList.%d", dwUserID);

	//先从缓存中读取玩家道具列表
	ValueList list;
	m_RedisInstance.lrange(szBuf, 0, -1, list);

	if (list.size() > 0)
	{
		//变量定义
		tagUserProperty prop;
		BYTE cbDataBuffer[SOCKET_TCP_PACKET - 1024] = { 0 };
		DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)cbDataBuffer;
		LPBYTE pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);

		//设置变量
		WORD wPropertyCount = 0;
		WORD wSendDataSize = sizeof(DBO_GR_PropertyListInfo);

		//遍历列表获取每个道具对应的HASH值
		for (ValueList::iterator it = list.begin(); it != list.end(); it++)
		{
			ValueMap v;
			bool bRet = m_RedisInstance.hgetall(*it, v);
			if (bRet && (v.size() > 0))
			{
				ZeroMemory(&prop, sizeof(tagUserProperty));
				
				//读取数据
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

				//发送判断				
				if (wSendDataSize + sizeof(DBO_GR_PropertyListInfo) > sizeof(cbDataBuffer))
				{
					//设置变量
					pPropertyListInfo->wPropertyCount = wPropertyCount;

					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);

					//重置变量
					wPropertyCount = 0;
					wSendDataSize = sizeof(DBO_GR_PropertyListInfo);
					pDataBuffer = cbDataBuffer + sizeof(DBO_GR_PropertyListInfo);
				}

				//拷贝数据
				CopyMemory(pDataBuffer, &prop, sizeof(tagUserProperty));

				//设置变量
				wPropertyCount++;
				wSendDataSize += sizeof(tagUserProperty);
				pDataBuffer += sizeof(tagUserProperty);
			}
		}

		//剩余发送
		if (wPropertyCount > 0 && wSendDataSize > 0)
		{
			//设置变量
			pPropertyListInfo->wPropertyCount = wPropertyCount;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_PROPERTY_INFO, dwContextID, pPropertyListInfo, wSendDataSize);
		}
	}
}

//修改缓存中玩家道具数量
void CDataBaseEngineSink::ModifyUserPropNumFromRedis(DWORD dwDBID, DWORD dwUserID, WORD wPropID, int nChangeValue)
{
	char szBuf[64] = { 0 };
	//sprintf_s(szBuf, sizeof(szBuf), "UserPropertyList.%d", dwUserID);

	////先从缓存中读取玩家道具列表
	//ValueList list;
	//m_RedisInstance.lrange(szBuf, 0, -1, list);

	//memset(szBuf, 0, sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "UserProperty.%d", dwDBID);
	//ValueList::iterator it = find(list.begin(), list.end(), szBuf);
	//if (it == list.end())
	//{
	//	return;
	//}

	//更新数量
	int64_t ret = 0;
	m_RedisInstance.hincrby(szBuf, "Count", nChangeValue, ret);
}
//////////////////////////////////////////////////////////////////////////////////

bool CDataBaseEngineSink::OnRequestPrivateInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_Private_Info));
	if (wDataSize != sizeof(DBR_GR_Private_Info)) return false;

	//变量定义
	DBR_GR_Private_Info * pPrivateInfo = (DBR_GR_Private_Info *)pData;


	//请求处理
	try
	{
		//构造参数
		m_PlatformDBModule->ClearParameters();
		m_PlatformDBModule->AddParameter( pPrivateInfo->wKindID );

		//执行查询
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
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

bool CDataBaseEngineSink::OnRequestCreatePrivate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//效验参数;
	ASSERT(wDataSize == sizeof(DBR_GR_Create_Private));
	if (wDataSize != sizeof(DBR_GR_Create_Private)) return false;

	//变量定义;
	DBR_GR_Create_Private * pPrivateInfo = (DBR_GR_Create_Private *)pData;
	dwUserID = pPrivateInfo->dwUserID;

	//请求处理;
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

		//构造参数;
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

				//设置变量
				kCreatePrivate.bSucess = FALSE;
				//获取信息
				rsRet.GetValue_String("@strErrorDescribe", kCreatePrivate.szDescribeString, CountArray(kCreatePrivate.szDescribeString));
			}
			else
			{
				//设置变量;
				kCreatePrivate.bSucess = TRUE;
				kCreatePrivate.lCurSocre = rs.GetValue_LONGLONG("CurSocre");
			}
		}

		if (kCreatePrivate.bSucess && (pPrivateInfo->dwAgaginTable == INVALID_DWORD))
		{	//创建成功并且是新创建房间时,向数据库中插入空的录像记录主表记录;
			BYTE szTotalData[1] = { 0 };

			m_TreasureDBModule->ClearParameters();
			m_TreasureDBModule->AddParameter(0);
			m_TreasureDBModule->AddParameter((LPCTSTR)&szTotalData[0]);

			//执行查询;
			BOOL bExecSuccess = m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecord");

			//写入记录;
			if (bExecSuccess)
			{
				CleanupResults objCleanup(m_TreasureDBModule);
				SQLRecordset rs(m_TreasureDBModule);
				if (rs.GetCount() == 0)
				{
					//设置变量;
					kCreatePrivate.bSucess = FALSE;
					TCHAR* szErrorDes = _T("执行[GSP_GR_PrivateGameRecord],结果集出错.");
					lstrcpyn(kCreatePrivate.szDescribeString, szErrorDes, strlen(szErrorDes));
				}
				else
				{
					//获取标识;
					kCreatePrivate.dwRecordID = rs.GetValue_DWORD("RecordID");
				}
			}
			else
			{
				//设置变量;
				kCreatePrivate.bSucess = FALSE;
				TCHAR* szErrorDes = _T("执行[GSP_GR_PrivateGameRecord]出错.");
				lstrcpyn(kCreatePrivate.szDescribeString, szErrorDes, strlen(szErrorDes));
			}
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_PRIVATE, dwContextID, &kCreatePrivate, sizeof(kCreatePrivate));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//私人房花费返还
bool CDataBaseEngineSink::OnRequestCreatePrivateCost(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//效验参数;
	ASSERT(wDataSize == sizeof(DBR_GR_Create_Private_Cost));
	if (wDataSize != sizeof(DBR_GR_Create_Private_Cost)) return false;

	//变量定义;
	DBR_GR_Create_Private_Cost * pPrivateInfo = (DBR_GR_Create_Private_Cost *)pData;


	//请求处理;
	try
	{
		//构造参数;
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
		//错误信息;
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

		//执行查询;
		BOOL bResultCode = m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecord");

		//写入记录;
		if (bResultCode)
		{
			//写入记录;
			for (WORD i = 0; i < (WORD)kTotalRecord.kUserID.size(); i++)
			{
				//重置参数;
				m_TreasureDBModule->ClearParameters();

				//房间信息;
				m_TreasureDBModule->AddParameter(kTotalRecord.iRecordID);
				m_TreasureDBModule->AddParameter(kTotalRecord.kUserID[i]);
				m_TreasureDBModule->AddParameter(kTotalRecord.dwKindID);

				//执行查询;
				m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecordUserRecordID");
			}
		}
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误;
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

		//执行查询;
		m_TreasureDBModule->ExecuteProcess("GSP_GR_PrivateGameRecordChild");

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


//私人房结束;
bool CDataBaseEngineSink::OnRequestPrivateGameEnd(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//效验参数
	ASSERT(wDataSize == sizeof(DBO_GR_PrivateGameEnd));
	if (wDataSize != sizeof(DBO_GR_PrivateGameEnd)) return false;

	//变量定义
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

		////执行查询;
		//m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_PrivateGameEnd"), true);

	}
	catch (IDataBaseException * pIException)
	{
		//输出错误;
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


