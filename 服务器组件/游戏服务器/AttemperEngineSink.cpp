#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////
//时间标识

#define IDI_LOAD_ANDROID_USER		(IDI_MAIN_MODULE_START+1)			//机器信息
#define IDI_REPORT_SERVER_INFO		(IDI_MAIN_MODULE_START+2)			//房间信息
#define IDI_CONNECT_CORRESPOND		(IDI_MAIN_MODULE_START+3)			//连接时间
#define IDI_GAME_SERVICE_PULSE		(IDI_MAIN_MODULE_START+4)			//服务脉冲
#define IDI_DISTRIBUTE_ANDROID		(IDI_MAIN_MODULE_START+5)			//分配机器
#define IDI_DBCORRESPOND_NOTIFY		(IDI_MAIN_MODULE_START+6)			//缓存通知
#define IDI_LOAD_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+7)			//系统消息
#define IDI_SEND_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+8)			//系统消息
#define IDI_LOAD_SENSITIVE_WORD		(IDI_MAIN_MODULE_START+9)			//加载敏感词
#define IDI_DISTRIBUTE_USER		    (IDI_MAIN_MODULE_START+10)			//分配用户
#define IDI_REFRESH_ONLINE_USER		(IDI_MAIN_MODULE_START+11)			//刷新在线玩家

//////////////////////////////////////////////////////////////////////////////////
//时间定义 秒

#define TIME_LOAD_ANDROID_USER		15L									//加载机器
#define TIME_DISTRIBUTE_ANDROID		15L									//分配用户
#define TIME_REPORT_SERVER_INFO		30L									//上报时间
#define TIME_DBCORRESPOND_NOTIFY	3L									//缓存通知时间
#define TIME_LOAD_SYSTEM_MESSAGE	600L								//系统消息时间
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //系统消息时间
#define TIME_LOAD_SENSITIVE_WORD	5L									//加载敏感词时间
//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define TEMP_MESSAGE_ID				INVALID_DWORD			


//间隔时间
#define MIN_INTERVAL_TIME			10									//间隔时间
#define MAX_INTERVAL_TIME			1*60								//临时标识

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//控制变量
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//绑定数据
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//会员参数
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//任务参数
	m_wTaskCount=0;
	ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

	//状态变量
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;

	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	//配置数据
	ZeroMemory(&m_DataConfigProperty,sizeof(m_DataConfigProperty));

	//比赛变量
	m_pIMatchServiceManager=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
	//删除数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//删除桌子
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//删除消息
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	for (INT_PTR i=0;i<m_SystemMessageBuffer.GetCount();i++)
	{
		SafeDelete(m_SystemMessageBuffer[i]);
	}

	//清空数组
	m_SystemMessageActive.RemoveAll();
	m_SystemMessageBuffer.RemoveAll();

	//清理数据	
	m_KickUserItemMap.RemoveAll();
	m_DistributeManage.RemoveAll();

	return;
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定信息
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*MAX_ANDROID);

	//绑定信息
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//配置比赛
	InitMatchServiceManager();

	//配置机器
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置桌子
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//设置接口
	if(m_pIMatchServiceManager!=NULL)
	{
		if (m_ServerUserManager.SetServerUserItemSink(m_pIMatchServiceManager->GetServerUserItemSink())==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}
	else
	{
		if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//启动机器
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置规则
	m_DistributeManage.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);

	//设置时间
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,15*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,5*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,TIME_LOAD_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,TIME_SEND_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
#endif

	//延时加载敏感词
	m_pITimerEngine->SetTimer(IDI_LOAD_SENSITIVE_WORD,TIME_LOAD_SENSITIVE_WORD*1000L,TIMES_INFINITY,NULL);

	//防作弊模式
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0)
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//加载机器
	if (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule) || 
		CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule) )
	{
#ifdef _DEBUG			
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,5000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);
#endif
	}

	//m_pITimerEngine->SetTimer(IDI_REFRESH_ONLINE_USER, m_pInitParameter->m_dwRefrashOnlineUserTime * 1000, TIMES_INFINITY, NULL);

	RegServerInfoInRedis();

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
#ifdef USE_REDIS
	CStringA strHost = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerAddr);

	//创建缓存连接
	CRedisInstance	mRedis;
	bool bRet = mRedis.connect(strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, "", 500000);
	if (bRet)
	{
		stringstream ss;
		int64_t ret = 0;
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
		{
			//删除kindID下的SERVERID;
			char szBuf[32] = {0};
			sprintf_s(szBuf, sizeof(szBuf), "kind%d.%d", m_pGameServiceOption->wKindID, m_pGameServiceOption->wServerType);
			string strKindServerKey(szBuf);
			
			mRedis.lrem(strKindServerKey, CYSService::ToString(m_pGameServiceOption->wServerID), ret);

			uint64_t nLen = 0;
			mRedis.llen(strKindServerKey, nLen);
			if ( nLen == 0 )
			{	//删除服务器列表中的KINDID;
				mRedis.lrem("serverlist", CYSService::ToString(m_pGameServiceOption->wKindID), ret);
			}

			string strPrivateID = "privateserverid." + CYSService::ToString(m_pGameServiceOption->wServerID);
			uint32_t nDelNum = 0;
			mRedis.hdel(strPrivateID, "ServerIP", nDelNum);
			mRedis.hdel(strPrivateID, "Port", nDelNum);
				
		}
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
		{
			ss << "matchserverid." << m_pGameMatchOption->dwMatchID;
			mRedis.del(ss.str(), ret);
			mRedis.lrem("matchlist", ss.str(), ret);
		}
		else
		{
			ss << "roomserverid." << m_pGameServiceOption->wServerID;
			mRedis.del(ss.str(), ret);
			mRedis.lrem("roomlist", ss.str(), ret);
		}
	}
#endif

	//状态变量
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//配置信息
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//数据引擎
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//绑定数据
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//任务变量
	m_wTaskCount=0;
	ZeroMemory(&m_TaskParameter,sizeof(m_TaskParameter));

	//会员参数
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//先停止服务和停止比赛,再关闭桌子(modify by fxd);
	//停止服务;
	m_AndroidUserManager.ConcludeService();

	//停止比赛;
	if (m_pIMatchServiceManager != NULL)
		m_pIMatchServiceManager->StopService();

	//删除桌子
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//删除用户
	m_TableFrameArray.RemoveAll();
	m_DistributeManage.RemoveAll();
	m_ServerUserManager.DeleteUserItem();
	m_ServerListManager.ResetServerList();
	
	////停止服务;
	//m_AndroidUserManager.ConcludeService();

	////停止比赛;
	//if(m_pIMatchServiceManager!=NULL)
	//	m_pIMatchServiceManager->StopService();

	//移除消息
	RemoveSystemMessage();

	//移除任务
	m_UserTaskManager.ResetTaskManager();

	//复位关键字
	m_WordsFilter.ResetSensitiveWordArray();

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//连接协调
		{
			//发起连接
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//构造提示
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("正在连接协调服务器 [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//提示消息
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//加载配置
		{
			//加载配置
			//m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//加载机器
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_PARAMETER,0L,NULL,0L);			

			//加载消息
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

			//加载任务
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

			//加载道具
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GAME_PROPERTY,0L,NULL,0L);

			return true;
		}
	case CT_SERVICE_CONTROL:
		{
			//效验消息;
			ASSERT(wDataSize == sizeof(CP_ServiceControlRequest));
			if (wDataSize != sizeof(CP_ServiceControlRequest)) return 0;

			//变量定义;
			CP_ServiceControlRequest * pServiceControlRequest = (CP_ServiceControlRequest *)pData;
			switch (pServiceControlRequest->cbServiceControlRequestType)
			{
			case eServiceControlRequest_RunParameter:
				{
					ShowBurnInfo();
					return true;
				}
				break;
			default:
				break;
			}
		}
	}

	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//调度时间
	if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
	{
		//时间处理
		switch (dwTimerID)
		{
		case IDI_LOAD_ANDROID_USER:		//加载机器
			{
				//变量定义
				DWORD dwBatchID=0;
				DWORD dwAndroidCount=0;

				//加载判断
				if(m_AndroidUserManager.GetAndroidLoadInfo(dwBatchID,dwAndroidCount))
				{
					//构造结构
					DBR_GR_LoadAndroidUser LoadAndroidUser;
					LoadAndroidUser.dwBatchID=dwBatchID;
					LoadAndroidUser.dwAndroidCount=dwAndroidCount;

					//加载机器
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,&LoadAndroidUser,sizeof(LoadAndroidUser));
				}

				return true;
			}
		case IDI_REPORT_SERVER_INFO:	//房间信息
			{
				//变量定义
				CMD_CS_C_ServerOnLine ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//设置变量				
				ServerOnLine.dwAndroidCount=m_ServerUserManager.GetAndroidCount();
				ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount()-ServerOnLine.dwAndroidCount;

				//发送数据
				m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine));

				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//连接协调
			{
				//发起连接
				tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
				m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

				//构造提示
				TCHAR szString[512]=TEXT("");
				_sntprintf_s(szString, CountArray(szString), TEXT("正在连接协调服务器 [ %s:%d ]"), pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

				//提示消息
				CTraceService::TraceString(szString,TraceLevel_Normal);

				return true;
			}
		case IDI_GAME_SERVICE_PULSE:	//服务维护
			{
				return true;
			}
		case IDI_DISTRIBUTE_ANDROID:	//分配机器
			{
				//动作处理
				if (m_AndroidUserManager.GetAndroidCount()>0 && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
				{
					//变量定义
					bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
					bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
					bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
					bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));

					//模拟处理
					if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
					{
						//机器状态
						tagAndroidUserInfo AndroidSimulate; 
						m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);

						//机器处理
						if (AndroidSimulate.wFreeUserCount>0)
						{
							WORD wTime = 8;
							if (wTime > AndroidSimulate.wFreeUserCount)
							{
								wTime = AndroidSimulate.wFreeUserCount;
							}

							for (WORD i = 0; i<wTime; i++)
							{
								//随机桌子
								WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));

								//获取桌子
								CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
								if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

								//桌子状况
								tagTableUserInfo TableUserInfo;
								WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

								//分配判断
								if (TableUserInfo.wTableUserCount>0) continue;
								if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

								//坐下判断
								if (AndroidSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
								{
									//变量定义
									WORD wHandleCount=0;
									WORD wWantAndroidCount=TableUserInfo.wMinUserCount;

									//数据调整
									if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
									{
										WORD wChairCount=m_pGameServiceAttrib->wChairCount;
										WORD wFreeUserCount=AndroidSimulate.wFreeUserCount;
										WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
										wWantAndroidCount+=((wOffUserCount > 0) ? (rand()%(wOffUserCount+1)) : 0);
									}

									//坐下处理
									for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
									{
										//变量定义
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//无效过滤
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										//用户坐下
										IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
										bool bResult = pTableFrame->PerformSitDownAction(wChairID, pIAndroidUserItem->GetMeUserItem());
										if (bResult)
										{
											//设置变量
											wHandleCount++;

											//完成判断
											if (wHandleCount>=wWantAndroidCount) 
											{
												return true;
											}
										}
									}

									if (wHandleCount > 0)
									{
										return true;
									}
								}
							}
						}
					}

					//陪打处理
					if (bAllowAndroidAttend==true)
					{
						//被动状态
						tagAndroidUserInfo AndroidPassivity;
						m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

						if(bAllowAvertCheatMode)
						{
							//坐下处理
							for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
							{
								IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
								bool bResult = InsertDistribute(pIAndroidUserItem->GetMeUserItem());
								if (bResult)
								{
									return true;
								}
							}
						}
						else
						{
							//被动处理
							if (AndroidPassivity.wFreeUserCount>0)
							{
								//百人游戏
								if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
								{
									for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
									{
										//获取桌子
										CTableFrame * pTableFrame=m_TableFrameArray[i];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

										//桌子状况
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//分配判断
										if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;

										//变量定义
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//无效过滤
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										//坐下处理
										for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
										{
											IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
											bool bResult = pTableFrame->PerformSitDownAction(wChairID, pIAndroidUserItem->GetMeUserItem());
											if (bResult)
											{
												return true;
											}
										}
									}
								}
								else
								{
									for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
									{
										//获取桌子
										CTableFrame * pTableFrame=m_TableFrameArray[i];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

										//桌子状况
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//分配判断
										if (wUserSitCount==0) continue;
										if (TableUserInfo.wTableUserCount==0) continue;
										if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(rand()%10>5)) continue;

										//变量定义
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//无效过滤
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										//坐下处理
										for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
										{
											IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
											bool bResult = pTableFrame->PerformSitDownAction(wChairID, pIAndroidUserItem->GetMeUserItem());
											if (bResult)
											{
												return true;
											}
										}
									}
								}
							}
						}
					}

					//陪打处理
					if (bAllowAndroidAttend==true)
					{
						//主动状态
						tagAndroidUserInfo AndroidInitiative;
						m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
						WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;

						if(bAllowAvertCheatMode)
						{
							//坐下处理
							for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
							{
								IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
								bool bResult = InsertDistribute(pIAndroidUserItem->GetMeUserItem());
								if (bResult)
								{
									return true;
								}
							}
						}
						else
						{
							//主动处理
							if (AndroidInitiative.wFreeUserCount>0)
							{
								for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
								{
									//获取桌子
									CTableFrame * pTableFrame=m_TableFrameArray[i];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//桌子状况
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//分配判断
									if((m_pGameServiceAttrib->wChairCount<MAX_CHAIR) && wUserSitCount>(TableUserInfo.wMinUserCount-1)) continue;

									//变量定义
									IServerUserItem * pIServerUserItem=NULL;
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//无效过滤
									//ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR) continue;

									//坐下处理
									for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
									{
										IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
										bool bResult = pTableFrame->PerformSitDownAction(wChairID, pIAndroidUserItem->GetMeUserItem());
										if (bResult)
										{
											return true;
										}
									}
								}
							}
						}
					}

					//变量定义
					bool bMaxPlayerGame=m_pGameServiceAttrib->wChairCount>=MAX_CHAIR;

					//类型判断
					if(bMaxPlayerGame)
					{
						//获取时间
						DWORD dwCurrTime = (DWORD)time(NULL);
						if(dwCurrTime<m_dwIntervalTime+m_dwLastDisposeTime) return true;

						//设置变量
						m_dwLastDisposeTime=dwCurrTime;
						m_dwIntervalTime=MIN_INTERVAL_TIME+rand()%(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME);   
					}

					//起立处理
					WORD wStandUpCount=0;
					WORD wRandCount=((rand()%3)+1);
					INT_PTR nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
					for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
					{
						//获取桌子
						INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
						CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
						if (pTableFrame->IsGameStarted()==true) continue;

						//桌子状况
						tagTableUserInfo TableUserInfo;
						WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

						//用户过虑
						bool bRand = ((rand()%100)>50);
						if (TableUserInfo.wTableAndroidCount==0) continue;
						if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
						if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;

						//起立处理
						for (WORD j=0;j<pTableFrame->GetChairCount();j++)
						{
							//获取用户
							IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
							if (pIServerUserItem==NULL) continue;

							//用户起立
							if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
							{
								wStandUpCount++;
								if (wStandUpCount >= wRandCount)
								{
									return true;
								}
								else
								{
									break;
								}
							}
						}
					}

					//起立处理
					nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
					for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
					{
						//获取桌子
						INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
						CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
						if (pTableFrame->IsGameStarted()==true) continue;

						//桌子状况
						tagTableUserInfo TableUserInfo;
						WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

						//用户过虑
						bool bRand = ((rand()%100)>50);
						if (TableUserInfo.wTableAndroidCount==0) continue;
						if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;

						//起立处理
						for (WORD j=0;j<pTableFrame->GetChairCount();j++)
						{
							//获取用户
							IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
							if (pIServerUserItem==NULL) continue;

							//用户起立
							if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
							{
								wStandUpCount++;
								if(wStandUpCount>=wRandCount)
									return true;
								else
									break;
							}
						}
					}
				}	

				//比赛类型
				if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
				{
					//陪玩标识
					bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);

					//陪玩处理
					if (bAllowAndroidAttend==true)
					{
						//被动状态
						tagAndroidUserInfo AndroidPassivity;
						m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);
							
						//坐下处理
						WORD wIndex = rand()%(AndroidPassivity.wFreeUserCount+1);
						for (WORD j=wIndex;j<AndroidPassivity.wFreeUserCount+wIndex;j++)
						{
							WORD wAndroidIndex = j%AndroidPassivity.wFreeUserCount;
							IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[wAndroidIndex];
							if (InsertDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
						}
					}
				}

				return true;
			}
		case IDI_DBCORRESPOND_NOTIFY: //缓存定时处理
			{
				if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnTimerNotify();
				return true;
			}
		case IDI_LOAD_SYSTEM_MESSAGE: //系统消息
			{
				//清除消息数据
				RemoveSystemMessage();

				//加载消息
				m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

				return true;
			}
		case IDI_LOAD_SENSITIVE_WORD:	//加载敏感词
			{
				//投递请求
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
				return true;
			}
		case IDI_SEND_SYSTEM_MESSAGE: //系统消息
			{
				//数量判断
				if(m_SystemMessageActive.GetCount()==0) return true;

				//时效判断
				DWORD dwCurrTime = (DWORD)time(NULL);
				for(INT_PTR nIndex=m_SystemMessageActive.GetCount()-1;nIndex>=0;nIndex--)
				{
					tagSystemMessage *pTagSystemMessage = m_SystemMessageActive[nIndex];

					//时效判断
					if(pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
					{
						m_SystemMessageActive.RemoveAt(nIndex);
						SafeDelete(pTagSystemMessage);

						continue;
					}

					//间隔判断
					if(pTagSystemMessage->dwLastTime+pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
					{
						//更新数据
						pTagSystemMessage->dwLastTime=dwCurrTime;

						//构造消息
						CMD_GR_SendMessage SendMessage = {};
						SendMessage.cbAllRoom = (pTagSystemMessage->SystemMessage.dwMessageID==TEMP_MESSAGE_ID)?TRUE:FALSE;
						SendMessage.cbGame = (pTagSystemMessage->SystemMessage.cbMessageType==1)?TRUE:FALSE;
						SendMessage.cbRoom = (pTagSystemMessage->SystemMessage.cbMessageType==2)?TRUE:FALSE;
						if(pTagSystemMessage->SystemMessage.cbMessageType==3)
						{
							SendMessage.cbGame = TRUE;
							SendMessage.cbRoom = TRUE;
						}
						lstrcpyn(SendMessage.szSystemMessage,pTagSystemMessage->SystemMessage.szSystemMessage,CountArray(SendMessage.szSystemMessage));
						SendMessage.wChatLength = lstrlen(SendMessage.szSystemMessage)+1;

						//发送消息
						WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+CountStringBuffer(SendMessage.szSystemMessage);
						SendSystemMessage(&SendMessage,wSendSize);
					}
				}

				return true;
			}
		case IDI_DISTRIBUTE_USER: //分配用户
			{
				//执行分组
				PerformDistribute();

				return true;
			}
		case IDI_REFRESH_ONLINE_USER:
			{
				DBR_GR_RefreshOnlineUser mData;
				ZeroMemory(&mData, sizeof(DBR_GR_RefreshOnlineUser));
				if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
				{
					sprintf_s(mData.szKey, sizeof(mData.szKey), "matchserverid.%d", m_pGameMatchOption->dwMatchID);
				}
				else
				{
					sprintf_s(mData.szKey, sizeof(mData.szKey), "roomserverid.%d", m_pGameServiceOption->wServerID);
				}
				sprintf_s(mData.szUserCount, sizeof(mData.szUserCount), "%d", m_ServerUserManager.GetUserItemCount()+m_ServerUserManager.GetAndroidCount());

				m_pIKernelDataBaseEngine->PostDataBaseRequest(DBR_GR_REFRESH_ONLINE_USER, 0, &mData, sizeof(mData));

				return true;
			}
		}
	}

	//机器时器
	if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
	{
		//时间处理
		m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

		return true;
	}

	//比赛定时器
	if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
	{
		if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventTimer(dwTimerID,wBindParam);
		return true;
	}

	//桌子时间
	if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
	{
		//桌子号码
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
		WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

		//时间效验
		if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
		{
			ASSERT(FALSE);
			return false;
		}

		//时间通知
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_LOGON_SUCCESS:			//登录成功
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOGON_FAILURE:			//登录失败
		{
			return OnDBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_PARAMETER:	//机器参数
		{
			return OnDBGameAndroidParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_INFO:		//机器信息
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_INFO:		//道具信息
		{
			return OnDBGamePropertyInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_PROPERTY_INFO:		//道具信息
		{
			return OnDBUserPropertyInfo(dwContextID, pData, wDataSize);
		}
	case DBO_GR_USER_INSURE_INFO:		//银行信息
		{
			return OnDBUserInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_SUCCESS:	//银行成功
		{
			return OnDBUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_FAILURE:	//银行失败
		{
			return OnDBUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_USER_INFO:  //用户信息
		{
			return OnDBUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	//case DBO_GR_PROPERTY_SUCCESS:		//道具成功
	//	{
	//		return OnDBPropertySuccess(dwContextID,pData,wDataSize);
	//	}
	case DBO_GR_SYSTEM_MESSAGE_RESULT:  //系统消息
		{
			return OnDBSystemMessage(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_FINISH:	//加载完成
		{
			return OnDBSystemMessageFinish(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SENSITIVE_WORDS:		//加载敏感词
		{
			return OnDBSensitiveWords(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST:				//任务列表
		{
			return OnDBUserTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST_END:			//任务结束
		{
			return OnDBUserTaskListEnd(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_INFO:				//任务信息
		{
			return OnDBUserTaskInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_RESULT:			//任务结果
		{
			return OnDBUserTaskResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PURCHASE_RESULT:		//购买结果
		{
			return OnDBPurchaseResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_EXCHANGE_RESULT:		//兑换结果
		{
			return OnDBExChangeResult(dwContextID,pData,wDataSize);
		}
	case DBR_GR_SEND_EMAIL_SUCCESS:		//邮件发放成功
		{
			return onDBSendEMailSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_GR_REREG_SERVER_IN_REDIS:
		{
			return OnDBReRegServerInRedis(dwContextID, pData, wDataSize);
		}
	}

	//比赛事件
	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//参数效验
		if(m_pIMatchServiceManager==NULL) return false;

		//查找用户
		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem * pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		return m_pIMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//设置变量
		m_bCollectUser=false;

		//删除时间
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//重连判断
		if (m_bNeekCorrespond==true)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("与协调服务器的连接关闭了，%ld 秒后将重新连接"), m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("协调服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在发送游戏房间注册信息..."),TraceLevel_Normal);

		//变量定义
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//服务端口
		CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
		RegisterServer.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//构造数据
		RegisterServer.wKindID=m_pGameServiceOption->wKindID;
		//RegisterServer.wNodeID=m_pGameServiceOption->wNodeID;
		RegisterServer.wSortID=m_pGameServiceOption->wSortID;
		RegisterServer.wServerID=m_pGameServiceOption->wServerID;
		RegisterServer.wServerKind=m_pGameServiceOption->wServerKind;
		RegisterServer.wServerType=m_pGameServiceOption->wServerType;		
		RegisterServer.lCellScore=m_pGameServiceOption->lCellScore;
		RegisterServer.lEnterScore=m_pGameServiceOption->lMinEnterScore;
		RegisterServer.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
		RegisterServer.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		RegisterServer.dwServerRule=m_pGameServiceOption->dwServerRule&SR_ALLOW_AVERT_CHEAT_MODE;
		lstrcpyn(RegisterServer.szServerName,m_pGameServiceOption->szServerName,CountArray(RegisterServer.szServerName));
		lstrcpyn(RegisterServer.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(RegisterServer.szServerAddr));

		//发送数据
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVER,&RegisterServer,sizeof(RegisterServer));

		//设置时间
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//注册服务
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_SERVICE_INFO:	//服务信息
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//用户汇总
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //管理服务
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_ANDROID_SERVICE: //机器服务
			{
				return OnTCPSocketMainAndroidService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_WEB_SERVICE:	//网站服务
			{
				return OnTCPSocketMainWebService(Command.wSubCmdID, pData, wDataSize);
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//设置变量
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义;
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;

	//获取用户;
	IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;
	WORD wTableID	= INVALID_WORD;
	DWORD dwUserID	= INVALID_DWORD;

	try
	{
		//用户处理;
		if (pIServerUserItem!=NULL)
		{
			//变量定义;
			wTableID = pIServerUserItem->GetTableID();
			dwUserID = pIServerUserItem->GetUserID();

			//断线处理;
			if (wTableID!=INVALID_TABLE)
			{
				//解除绑定;
				pIServerUserItem->DetachBindStatus();

				//断线通知;
				ASSERT(wTableID<m_pGameServiceOption->wTableCount);
				m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
			}
			else
			{
				pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
			}
		}
	}
	catch(...)
	{
		TCHAR szMessage[128]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("关闭连接异常: wTableID=%d"), wTableID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	CString strMsg;
	BYTE* pIP = (BYTE*)&dwClientAddr;
	strMsg.Format(TEXT("ip: %d.%d.%d.%d, userID:[%d] 离线"), pIP[0], pIP[1], pIP[2], pIP[3], dwUserID);
	CTraceService::TraceString(strMsg, TraceLevel_Normal);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_USER:		//用户命令
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_LOGON:		//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:		//游戏命令
		{
			return OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_FRAME:		//框架命令
		{
			return OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_INSURE:		//银行命令
		{
			return OnTCPNetworkMainInsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_TASK:		//任务命令
		{
			return OnTCPNetworkMainTask(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_EXCHANGE:	//兑换命令
		{
			return OnTCPNetworkMainExchange(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGE:		//管理命令
		{
			return OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MATCH:		//比赛命令
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PRIVATE:	//私人场命令;
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	//SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
	SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	//SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
	SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->IsOnline())
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		//SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
		SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

		//变量定义
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//关闭处理
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,true);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->IsOnline()&&pIServerUserItem->IsClientReady())
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		//SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
		SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

		//变量定义
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//关闭处理
			if ((wType&(SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
			{
				m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID, true);
			}
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	//SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
	SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	if (bAndroid)
	{
		//机器用户
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//常规用户
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//机器数据
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//用户数据
	if (cbSendMask==BG_ALL_CLIENT)
	{
		//m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_MOBILE);
		//m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_COMPUTER);
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID, wSubCmdID, pData, wDataSize, cbSendMask);
	}
	else if (cbSendMask == BG_COMPUTER)
	{
		//m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);
		
		// 发送给PC玩家
		WORD wEnumIndex = 0;
		while (wEnumIndex < m_ServerUserManager.GetUserItemCount())
		{
			//过滤用户
			IServerUserItem *pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) continue;
			if (!pIServerUserItem->IsOnline()) continue;
			if (pIServerUserItem->IsAndroidUser()) continue;

			//常规用户
			WORD wBindIndex = pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
		}
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//发送数据
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//机器用户
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//网络用户
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	if (pIServerUserItem->IsOnline())
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//机器用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//常规用户
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}

	return false;
}

//群发数据
bool CAttemperEngineSink::SendDataToTable(WORD wTableID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ASSERT(wTableID != INVALID_TABLE);
	if (wTableID != INVALID_TABLE)
	{
		m_TableFrameArray[wTableID]->SendLookonData(INVALID_CHAIR, wSubCmdID, pData, wDataSize, wMainCmdID);
		return m_TableFrameArray[wTableID]->SendTableData(INVALID_CHAIR, wSubCmdID, pData, wDataSize, wMainCmdID);
	}

	return false;
}

//用户积分
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
	
	//变量定义
	CMD_GR_UserScore UserScore;

	//构造数据
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	UserScore.UserScore.dwExperience = pUserInfo->dwExperience;

	//构造积分
	UserScore.UserScore.lScore = pUserInfo->lScore;
	UserScore.UserScore.lHoldScore = pUserInfo->lScore + pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lHoldScore += pIServerUserItem->GetFrozenedScore();

	//发送数据
	SendDataToTable(pIServerUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));

	//即时写分
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//提取积分
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//调整分数
		//if(pIServerUserItem->IsAndroidUser()==true)
		//{
		//	WriteGameScore.VariationInfo.lScore=0;
		//	WriteGameScore.VariationInfo.lGrade=0;
		//	WriteGameScore.VariationInfo.lInsure=0;
		//}

		//if (pUserTaskEntry != NULL) WriteGameScore.bTaskForward = true;

		WriteGameScore.cbMaxCardType = pIServerUserItem->GetMaxCardData(WriteGameScore.aryCardData, MAX_CARD_DATA_COUNT);

		//投递请求
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);

		//推进任务
		if (pIServerUserItem->IsAndroidUser() == false && cbReason != 0)
		{
			tagUserTaskEntry * pUserTaskEntry = m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(), TASK_STATUS_UNFINISH);
			if (pUserTaskEntry != NULL)
			{
				PerformTaskProgress(pIServerUserItem, pUserTaskEntry, WriteGameScore.VariationInfo, cbReason);
			}
		}
	}

	//通知桌子, 桌子未处理暂时屏蔽
	//if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	//{
	//	m_TableFrameArray[pIServerUserItem->GetTableID()]->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
	//}

	return true;
}

//用户状态
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//当前状态
	BYTE cbCurUserStatus = pIServerUserItem->GetUserStatus();

	//构造数据
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus = cbCurUserStatus;

	//发送数据
	if (US_SIT == cbCurUserStatus || (US_READY == cbCurUserStatus&&GAME_GENRE_MATCH==m_pGameServiceOption->wServerType))
	{
		//如果客户端已经准备好则群发桌子消息，否则只发给自已
		if ( pIServerUserItem->IsClientReady() )
		{
			SendDataToTable(pIServerUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}
		else
		{
			SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}
	}
	else
	{
		//玩家站起后无法从桌子群发消息里接收消息，所以这儿对自已补发一条状态改变消息
		if (US_FREE == cbCurUserStatus && !pIServerUserItem->IsClientReady())
		{
			SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}

		SendDataToTable(wOldTableID, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		
	}

	//离开判断
	if (cbCurUserStatus == US_NULL)
	{
		//获取绑定
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//绑带处理
		if (pBindParameter!=NULL)
		{
			//绑定处理
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//中断网络
			if (pBindParameter->dwSocketID!=0L)
			{
				if (LOWORD(pBindParameter->dwSocketID)>=INDEX_ANDROID)
				{
					m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,false);
				}
				else
				{
					m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
				}
			}
		}

		//离开处理
		OnEventUserLogout(pIServerUserItem,0L);
	}
	else if (cbCurUserStatus == US_LOOKON)
	{
		// 如果是旁观状态，只发送给自己
		SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
	}

	return true;
}

//登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//废弃判断
	if ((pBindParameter->pIServerUserItem!=NULL)||(pBindParameter->dwSocketID!=dwContextID))
	{
		//错误断言
		ASSERT(FALSE);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_NORMAL);

		return true;
	}

	//变量定义
	bool bAndroidUser=(wBindIndex>=INDEX_ANDROID);
	bool bMobileUser=(pBindParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//被踢判断
	DWORD dwKickTime;
	if(m_KickUserItemMap.Lookup(pDBOLogonSuccess->dwUserID,dwKickTime)==TRUE)
	{
		//时效判断
		DWORD dwCurrTime = (DWORD)time(NULL);
		if(dwKickTime+VALID_TIME_KICK_BY_MANAGER > dwCurrTime)
		{
			//发送失败
			SendLogonFailure(TEXT("您已被管理员请出房间,1小时之内不能进入！"),0,pBindParameter->dwSocketID);

			//解除锁定
			PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

			return true;
		}
		else
		{
			//移除玩家
			m_KickUserItemMap.RemoveKey(pDBOLogonSuccess->dwUserID);
		}
	}

	//切换判断
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		//切换用户
		SwitchUserItemConnect(pIServerUserItem, pDBOLogonSuccess->szMachineID, dwContextID, pDBOLogonSuccess->cbDeviceType);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_USER_IMPACT);

		return true;
	}

	////维护判断
	//if ((CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule)==true)&&(pDBOLogonSuccess->cbMasterOrder==0))
	//{
	//	//发送失败
	//	SendLogonFailure(TEXT("抱歉，由于系统维护的原因，当前游戏房间禁止用户进入！"),0,pBindParameter->dwSocketID);

	//	//解除锁定
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SYSTEM);

	//	return true;
	//}

	////查找机器
	//if (bAndroidUser==true && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
	//{
	//	//查找机器
	//	DWORD dwUserID=pDBOLogonSuccess->dwUserID;
	//	IAndroidUserItem * pIAndroidUserItem=m_AndroidUserManager.SearchAndroidUserItem(dwUserID,dwContextID);

	//	//修改积分
	//	if (pIAndroidUserItem!=NULL)
	//	{
	//		//获取配置
	//		tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

	//		//调整积分
	//		if ((pAndroidParameter->lTakeMinScore!=0L)&&(pAndroidParameter->lTakeMaxScore!=0L))
	//		{
	//			//变量定义
	//			SCORE lTakeMinScore=(SCORE)pAndroidParameter->lTakeMinScore;
	//			SCORE lTakeMaxScore=(SCORE)__max(pAndroidParameter->lTakeMaxScore,pAndroidParameter->lTakeMinScore);

	//			//调整积分
	//			if ((lTakeMaxScore-lTakeMinScore)>0L)
	//			{
	//				SCORE lTakeScore = (lTakeMaxScore-lTakeMinScore)/10;
	//				pDBOLogonSuccess->lScore=(SCORE)(lTakeMinScore+(rand()%10)*lTakeScore+rand()%lTakeScore);
	//			}
	//			else
	//			{
	//				pDBOLogonSuccess->lScore=(SCORE)lTakeMaxScore;
	//			}
	//		}
	//	}
	//}

	////最低分数
	//if ((m_pGameServiceOption->lMinEnterScore!=0L)&&(pDBOLogonSuccess->lScore<m_pGameServiceOption->lMinEnterScore))
	//{
	//	//发送失败
	//	TCHAR szMsg[128]=TEXT("");
	//	_sntprintf(szMsg,CountArray(szMsg), TEXT("抱歉，您的游戏成绩低于当前游戏房间的最低进入成绩%I64d，不能进入当前游戏房间！"), m_pGameServiceOption->lMinEnterScore);
	//	SendLogonFailure(szMsg,0,pBindParameter->dwSocketID);

	//	//解除锁定
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	////最高分数
	//if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(pDBOLogonSuccess->lScore>m_pGameServiceOption->lMaxEnterScore))
	//{
	//	//发送失败
	//	TCHAR szMsg[128]=TEXT("");
	//	_sntprintf(szMsg,CountArray(szMsg), TEXT("抱歉，您的游戏成绩高于当前游戏房间的最高进入成绩%I64d，不能进入当前游戏房间！"), m_pGameServiceOption->lMaxEnterScore);
	//	SendLogonFailure(szMsg,0,pBindParameter->dwSocketID);

	//	//解除锁定
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	////会员判断
	//if(m_pGameServiceOption->cbMinEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder < m_pGameServiceOption->cbMinEnterMember)
	//{
	//	//发送失败
	//	SendLogonFailure(TEXT("抱歉，您的会员级别低于当前游戏房间的最低进入会员条件，不能进入当前游戏房间！"),0,pBindParameter->dwSocketID);

	//	//解除锁定
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	////会员判断
	//if(m_pGameServiceOption->cbMaxEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder > m_pGameServiceOption->cbMaxEnterMember)
	//{
	//	//发送失败
	//	SendLogonFailure(TEXT("抱歉，您的会员级别高于当前游戏房间的最高进入会员条件，不能进入当前游戏房间！"),0,pBindParameter->dwSocketID);

	//	//解除锁定
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	//满人判断
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	if ((pDBOLogonSuccess->cbMemberOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-RESERVE_USER_COUNT)))
	{
		//发送失败
		SendLogonFailure(TEXT("抱歉，由于此房间已经人满，普通玩家不能继续进入了！"),0,pBindParameter->dwSocketID);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		return true;
	}

	//用户变量
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//属性资料
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	//UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;
	UserInfo.dwCustomFaceVer = pDBOLogonSuccess->dwCustomFaceVer;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));

	//用户资料
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//状态设置
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//积分信息
	UserInfo.lScore = pDBOLogonSuccess->lGold;
	UserInfo.lGold = pDBOLogonSuccess->lGold;
	UserInfo.lDiamond=pDBOLogonSuccess->lDiamond;
	UserInfo.lInsure=pDBOLogonSuccess->lInsure;
	UserInfo.dwWinCount=pDBOLogonSuccess->dwWinCount;
	UserInfo.dwLostCount=pDBOLogonSuccess->dwLostCount;
	UserInfo.dwDrawCount=pDBOLogonSuccess->dwDrawCount;
	UserInfo.dwFleeCount=pDBOLogonSuccess->dwFleeCount;	
	UserInfo.dwExperience=pDBOLogonSuccess->dwExperience;
	UserInfo.lLoveLiness=pDBOLogonSuccess->lLoveLiness;
	UserInfo.cbBenefitTimes = pDBOLogonSuccess->cbBenefitTimes;
	CopyMemory(&UserInfo.LastBenefitTime, &pDBOLogonSuccess->LastBenefitTime, sizeof(SYSTEMTIME));

	//最大牌型
	UserInfoPlus.cbMaxCardType = pDBOLogonSuccess->cbMaxCardType;
	CopyMemory(UserInfoPlus.aryCardData, pDBOLogonSuccess->aryCardData, sizeof(UserInfoPlus.aryCardData));

	//登录信息
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

	//用户权限
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=0;

	//辅助变量
	UserInfoPlus.bAndroidUser=bAndroidUser;
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	UserInfoPlus.lDefPutScore = m_pGameServiceOption->lDefPutScore;
	UserInfoPlus.lLowPutScore = m_pGameServiceOption->lLowPutScore;
	UserInfoPlus.iExchangeRatio = m_pGameServiceOption->iExchangeRatio;
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfoPlus.szPassword));

	//连接信息
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogonSuccess->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//激活用户
	m_ServerUserManager.InsertUserItem(&pIServerUserItem,UserInfo,UserInfoPlus);

	//if (bAndroidUser == false)
	//{
	//	//用户任务
	//	if (pDBOLogonSuccess->wTaskCount > 0)
	//	{
	//		m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(), pDBOLogonSuccess->UserTaskInfo, pDBOLogonSuccess->wTaskCount);
	//	}

	//	//加载道具
	//	DBR_GR_LOADPROPERTY LoadProperty;
	//	ZeroMemory(&LoadProperty, sizeof(LoadProperty));
	//	LoadProperty.dwUserID = pIServerUserItem->GetUserID();
	//	m_pIDBCorrespondManager->PostDataBaseRequest(LoadProperty.dwUserID, DBR_GR_LOAD_USER_PROPERTY, 0, &LoadProperty, sizeof(LoadProperty));
	//}
	
	//错误判断
	if (pIServerUserItem==NULL)
	{
		//错误断言
		ASSERT(FALSE);

		//解除锁定
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		//断开用户
		if (bAndroidUser==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(dwContextID,true);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
		}

		return true;
	}

	//设置用户
	pBindParameter->pIServerUserItem=pIServerUserItem;

	//登录事件
	OnEventUserLogon(pIServerUserItem,false);

	//汇总用户
	if (m_bCollectUser==true)
	{
		//变量定义
		CMD_CS_C_UserEnter UserEnter;
		ZeroMemory(&UserEnter,sizeof(UserEnter));

		//设置变量
		UserEnter.dwUserID=pIServerUserItem->GetUserID();
		//UserEnter.dwGameID=pIServerUserItem->GetGameID();
		lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

		//辅助信息
		UserEnter.cbGender=pIServerUserItem->GetGender();
		UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
		UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

		//发送消息
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
	}

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem!=NULL)) return true;

	//发送错误
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	//断开连接
	if (LOWORD(dwContextID)>=INDEX_ANDROID)
	{
		CString strTmp;
		strTmp.Format(_T("机器人 OnDBLogonFailure %s"), pLogonFailure->szDescribeString);
		CTraceService::TraceString(strTmp, TraceLevel_Exception);
		m_AndroidUserManager.DeleteAndroidUserItem(dwContextID,true);
	}
	else
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	return true;
}

//机器信息
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidAccountsInfo);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0])))) return false;

	//设置机器
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.InsertAndroidInfo(pGameAndroidInfo->AndroidAccountsInfo,pGameAndroidInfo->wAndroidCount,pGameAndroidInfo->dwBatchID);
	}

	return true;
}

//机器参数
bool CAttemperEngineSink::OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameAndroidParameter * pGameAndroidParameter=(DBO_GR_GameAndroidParameter *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidParameter)-sizeof(pGameAndroidParameter->AndroidParameter);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0])))) return false;

	//设置机器
	if (pGameAndroidParameter->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.AddAndroidParameter(pGameAndroidParameter->AndroidParameter,pGameAndroidParameter->wParameterCount);
	}

	return true;
}

//道具信息
bool CAttemperEngineSink::OnDBGamePropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GamePropertyInfo * pGamePropertyInfo = (DBO_GR_GamePropertyInfo *)pData;
	WORD wHeadSize = sizeof(DBO_GR_GamePropertyInfo) - sizeof(pGamePropertyInfo->PropertyInfo);

	//效验参数
	ASSERT((wDataSize >= wHeadSize) && (wDataSize == (wHeadSize + pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0]))));
	if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0])))) return false;

	//获取状态
	CServiceUnits * pServiceUnits = CServiceUnits::g_pServiceUnits;
	enServiceStatus ServiceStatus = pServiceUnits->GetServiceStatus();

	//设置道具
	if (pGamePropertyInfo->lResultCode == DB_SUCCESS)
	{
		//设置管理
		m_GamePropertyManager.SetGamePropertyInfo(pGamePropertyInfo->PropertyInfo, pGamePropertyInfo->cbPropertyCount);

		//拷贝数据
		m_DataConfigProperty.cbPropertyCount = pGamePropertyInfo->cbPropertyCount;
		CopyMemory(m_DataConfigProperty.PropertyInfo, pGamePropertyInfo->PropertyInfo, pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0]));
	}

	//事件通知
	if (ServiceStatus != ServiceStatus_Service)
	{
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//道具信息
bool CAttemperEngineSink::OnDBUserPropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)pData;
	if (pPropertyListInfo->wPropertyCount <= 0) return true;

	WORD propsize = sizeof(tagUserProperty);

	//效验参数
	ASSERT(wDataSize == pPropertyListInfo->wPropertyCount*propsize + sizeof(DBO_GR_PropertyListInfo));
	if (wDataSize != pPropertyListInfo->wPropertyCount*propsize + sizeof(DBO_GR_PropertyListInfo)) return false;

	//获取用户
	//ASSERT(GetBindUserItem(LOWORD(dwContextID)) != NULL);
	IServerUserItem * pIServerUserItem = GetBindUserItem(LOWORD(dwContextID));
	if (pIServerUserItem == NULL)	return true;

	tagUserProperty* pUserProperty = (tagUserProperty*)((byte*)pPropertyListInfo + sizeof(DBO_GR_PropertyListInfo));

	tagUserProperty prop;

	for (WORD i = 0; i < pPropertyListInfo->wPropertyCount; i++)
	{
		CopyMemory(&prop, (pUserProperty + i), propsize);
		pIServerUserItem->SetUserProperty(prop);
	}

	return true;
}

//银行信息
bool CAttemperEngineSink::OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserInsureInfo * pUserInsureInfo=(DBO_GR_UserInsureInfo *)pData;

	//变量定义
	CMD_GR_S_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//构造数据
	UserInsureInfo.cbActivityGame=pUserInsureInfo->cbActivityGame;
	UserInsureInfo.cbEnjoinTransfer=pUserInsureInfo->cbEnjoinTransfer;
	UserInsureInfo.wRevenueTake=pUserInsureInfo->wRevenueTake;
	UserInsureInfo.wRevenueTransfer=pUserInsureInfo->wRevenueTransfer;
	UserInsureInfo.wRevenueTransferMember=pUserInsureInfo->wRevenueTransferMember;
	UserInsureInfo.wServerID=pUserInsureInfo->wServerID;
	UserInsureInfo.lUserInsure=pUserInsureInfo->lUserInsure;
	UserInsureInfo.lUserScore+=pIServerUserItem->GetUserScore();
	UserInsureInfo.lUserScore+=pIServerUserItem->GetTrusteeScore();
	UserInsureInfo.lUserScore+=pIServerUserItem->GetFrozenedScore();
	UserInsureInfo.lTransferPrerequisite=pUserInsureInfo->lTransferPrerequisite;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_INSURE,SUB_GR_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	return true;
}

//银行成功
bool CAttemperEngineSink::OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserInsureSuccess * pUserInsureSuccess=(DBO_GR_UserInsureSuccess *)pData;

	//变量定义
	SCORE lFrozenedScore=pUserInsureSuccess->lFrozenedScore;
	SCORE lInsureRevenue=pUserInsureSuccess->lInsureRevenue;
	SCORE lVariationScore=pUserInsureSuccess->lVariationScore;
	SCORE lVariationInsure=pUserInsureSuccess->lVariationInsure;

	//解冻积分
	if ((lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//银行操作
	if (pIServerUserItem->ModifyUserInsure(lVariationScore,lVariationInsure,lInsureRevenue)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	CMD_GR_S_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//构造变量
	UserInsureSuccess.cbActivityGame=pUserInsureSuccess->cbActivityGame;
	UserInsureSuccess.lUserScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//发送数据
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送错误
	DBO_GR_UserInsureFailure * pUserInsureFailure=(DBO_GR_UserInsureFailure *)pData;
	SendInsureFailure(pIServerUserItem,pUserInsureFailure->szDescribeString,pUserInsureFailure->lResultCode,pUserInsureFailure->cbActivityGame);

	//解冻积分
	if ((pUserInsureFailure->lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(pUserInsureFailure->lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//用户信息
bool CAttemperEngineSink::OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//获取用户
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//变量定义
	DBO_GR_UserTransferUserInfo * pTransferUserInfo=(DBO_GR_UserTransferUserInfo *)pData;

	//变量定义
	CMD_GR_S_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//构造变量
	UserTransferUserInfo.cbActivityGame=pTransferUserInfo->cbActivityGame;
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szAccounts,pTransferUserInfo->szAccounts,CountArray(UserTransferUserInfo.szAccounts));

	//发送数据
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_TRANSFER_USER_INFO,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

////道具成功
//bool CAttemperEngineSink::OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
//{
//	//变量定义
//	DBO_GR_S_PropertySuccess * pPropertySuccess=(DBO_GR_S_PropertySuccess *)pData;
//
//	//获取用户
//	IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwSourceUserID);
//	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwTargetUserID);
//
//	//赠送用户
//	if (pISourceUserItem!=NULL)
//	{
//		//变量定义
//		SCORE lFrozenedScore=pPropertySuccess->lFrozenedScore;
//
//		//解冻积分
//		if ((lFrozenedScore>0L)&&(pISourceUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
//		{
//			ASSERT(FALSE);
//			return false;
//		}
//	}
//
//	//更新魅力
//	if (pISourceUserItem!=NULL)
//	{
//		pISourceUserItem->ModifyUserProperty(0,pPropertySuccess->lSendLoveLiness);
//	}
//
//	//更新魅力
//	if(pITargetUserItem!=NULL)
//	{
//		pITargetUserItem->ModifyUserProperty(0,pPropertySuccess->lRecvLoveLiness);
//	}
//
//	//变量定义
//	DWORD dwCurrentTime=(DWORD)time(NULL);
//	tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();
//
//	//道具处理	
//	switch(pPropertySuccess->wPropertyIndex)
//	{
//	case PROPERTY_ID_VIP1_CARD: //月会员
//	case PROPERTY_ID_VIP2_CARD:	//季度会员
//	case PROPERTY_ID_VIP3_CARD: //半年会员
//		{
//			//用户信息
//			tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
//			if(pUserInfo==NULL) return true;
//
//			//更新会员
//			pUserInfo->cbMemberOrder=pPropertySuccess->cbMemberOrder;
//			
//			//修改权限
//			pITargetUserItem->ModifyUserRight(pPropertySuccess->dwUserRight,0);
//
//			//发送消息
//			SendPropertyEffect(pITargetUserItem);
//
//			break;
//		}
//	default:  // 全部礼物
//		{			
//			break;
//		}
//	}
//
//	//消费方式
//	if(pPropertySuccess->cbConsumeScore==FALSE)
//	{
//		pISourceUserItem->ModifyUserInsure(0,-pPropertySuccess->lConsumeGold,0);
//	}
//
//	//变量定义
//	CMD_GR_S_PropertySuccess PropertySuccess;
//	ZeroMemory(&PropertySuccess,sizeof(PropertySuccess));
//
//	//设置变量
//	PropertySuccess.cbRequestArea=pPropertySuccess->cbRequestArea;
//	PropertySuccess.wItemCount=pPropertySuccess->wItemCount;
//	PropertySuccess.wPropertyIndex=pPropertySuccess->wPropertyIndex;
//	PropertySuccess.dwSourceUserID=pPropertySuccess->dwSourceUserID;
//	PropertySuccess.dwTargetUserID=pPropertySuccess->dwTargetUserID;
//
//	//发送消息
//	if (pISourceUserItem!=NULL)
//	{
//		SendData(pISourceUserItem,MDM_GR_USER,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
//	}
//
//	//发送消息
//	if (pITargetUserItem!=NULL && pITargetUserItem!=pISourceUserItem)
//	{
//		SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
//	}
//
//	//广播礼物
//	if(GetPropertyType(pPropertySuccess->wPropertyIndex)==PT_TYPE_PRESENT)
//		SendPropertyMessage(PropertySuccess.dwSourceUserID,PropertySuccess.dwTargetUserID,PropertySuccess.wPropertyIndex,
//		PropertySuccess.wItemCount);
//
//	return true;
//}
//
////道具失败
//bool CAttemperEngineSink::OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
//{
//	//参数校验
//	ASSERT(wDataSize==sizeof(DBO_GR_PropertyFailure));
//	if(wDataSize!=sizeof(DBO_GR_PropertyFailure)) return false;
//
//	//提取数据
//	DBO_GR_PropertyFailure * pPropertyFailure = (DBO_GR_PropertyFailure *)pData;
//	if(pPropertyFailure==NULL) return false;
//
//	//变量定义
//	WORD wBindIndex=LOWORD(dwContextID);
//	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
//	if(pBindParameter==NULL) return false;
//	if(pBindParameter->pIServerUserItem==NULL) return false;
//
//	//发送消息
//	return SendPropertyFailure(pBindParameter->pIServerUserItem,pPropertyFailure->szDescribeString,0L,pPropertyFailure->cbRequestArea);
//}

//购买结果
bool CAttemperEngineSink::OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//提取数据
	DBO_GR_PurchaseResult * pPurchaseResult=(DBO_GR_PurchaseResult *)pData;

	//构造结构
	CMD_GR_PurchaseResult PurchaseResult;
	ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

	//设置变量
	PurchaseResult.bSuccessed=pPurchaseResult->bSuccessed;
	PurchaseResult.lCurrScore=pPurchaseResult->lCurrScore;
	PurchaseResult.cbMemberOrder=pPurchaseResult->cbMemberOrder;
	lstrcpyn(PurchaseResult.szNotifyContent,pPurchaseResult->szNotifyContent,CountArray(PurchaseResult.szNotifyContent));

	//发送数据
	WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_PURCHASE_RESULT,&PurchaseResult,wSendDataSize);

	//结果判断
	if(PurchaseResult.bSuccessed==true)
	{
		//设置权限
		pIServerUserItem->ModifyUserRight(pPurchaseResult->dwUserRight,0,UR_KIND_SERVER);		

		//更新游戏币
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
		{
			pIServerUserItem->ModifyUserInsure(PurchaseResult.lCurrScore-pIServerUserItem->GetUserScore(),0,0);
		}
	}

	return true;
}

//兑换结果
bool CAttemperEngineSink::OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//提取数据
	DBO_GR_ExchangeResult * pExchangeResult=(DBO_GR_ExchangeResult *)pData;

	//构造结构
	CMD_GP_ExchangeResult ExchangeResult;
	ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

	//设置变量
	ExchangeResult.bSuccessed=pExchangeResult->bSuccessed;
	ExchangeResult.lCurrScore=pExchangeResult->lCurrScore;
	ExchangeResult.lCurrIngot=pExchangeResult->lCurrIngot;
	lstrcpyn(ExchangeResult.szNotifyContent,pExchangeResult->szNotifyContent,CountArray(ExchangeResult.szNotifyContent));

	//发送数据
	WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_RESULT,&ExchangeResult,wSendDataSize);

	//更新游戏币
	if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD && ExchangeResult.bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(ExchangeResult.lCurrScore-pIServerUserItem->GetUserScore(),0,0);
	}

	return true;
}

//邮件发送成功
bool CAttemperEngineSink::onDBSendEMailSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	ASSERT(wDataSize <= sizeof(tagSendBenefitSuc));
	if (wDataSize > sizeof(tagSendBenefitSuc)) return false;

	//变量定义
	tagSendBenefitSuc * pBenefitSuc = (tagSendBenefitSuc *)pData;
	IServerUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pBenefitSuc->dwUserID);
	if (pIServerUserItem != NULL)
	{
		pIServerUserItem->SetBenefitInfo(pBenefitSuc);
	}

	return true;
}

//在缓存中重新注册
bool CAttemperEngineSink::OnDBReRegServerInRedis(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	RegServerInfoInRedis();

	return true;
}

//任务列表
bool CAttemperEngineSink::OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_TaskListInfo * pTaskListInfo=(DBO_GR_TaskListInfo *)pData;

	//拷贝数据	
	WORD wTaskCount = __min(pTaskListInfo->wTaskCount, CountArray(m_TaskParameter)-m_wTaskCount);
	CopyMemory(m_TaskParameter+m_wTaskCount, ++pTaskListInfo, sizeof(tagTaskParameter)*wTaskCount);

	//设置变量
	m_wTaskCount += wTaskCount;

	return true;
}

//任务结束
bool CAttemperEngineSink::OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//移除任务
	m_UserTaskManager.RemoveTaskParameter();

	//添加任务
	m_UserTaskManager.AddTaskParameter(m_TaskParameter, m_wTaskCount);

	return true;
}

//任务信息
bool CAttemperEngineSink::OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskInfo));
	if (wDataSize>sizeof(DBO_GR_TaskInfo)) return false;

	//变量定义
	DBO_GR_TaskInfo * pTaskInfo=(DBO_GR_TaskInfo *)pData;

	//网络数据
	WORD wTaskCount=0;
	WORD wSendSize=0,wParameterSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	LPBYTE pDataBuffer = cbDataBuffer;

	//设置数量
	pDataBuffer += sizeof(wTaskCount);
	wSendSize = sizeof(wTaskCount);

	//变量定义	
	POSITION Position=NULL;	
	do
	{
		tagTaskParameter * pTaskParameter=m_UserTaskManager.EnumTaskParameter(Position);			
		if(pTaskParameter!=NULL)
		{
			//越界判断
			if(wSendSize+sizeof(tagTaskParameter)>SOCKET_TCP_PACKET)
			{
				//重置变量
				pDataBuffer = cbDataBuffer;
				*(WORD*)pDataBuffer = wTaskCount; 

				//发送数据
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,pDataBuffer,wSendSize);

				//重置变量
				wTaskCount = 0;
				pDataBuffer += sizeof(wSendSize);
				wSendSize = sizeof(wSendSize);				
			}

			//计算大小
			wParameterSize = sizeof(tagTaskParameter);// -sizeof(pTaskParameter->szTaskDescribe) + CountStringBuffer(pTaskParameter->szTaskDescribe);
			*(WORD *)pDataBuffer = wParameterSize;
			pDataBuffer += sizeof(wParameterSize);
			wSendSize += sizeof(wParameterSize);

			//拷贝数据
			CopyMemory(pDataBuffer,pTaskParameter,wParameterSize);

			//推进指针
			wTaskCount++;
			pDataBuffer += wParameterSize;
			wSendSize += wParameterSize;
		}
	}while(Position!=NULL);

	//剩余发送
	if(wTaskCount>0 && wSendSize)
	{
		//重置变量
		pDataBuffer = cbDataBuffer;
		*(WORD *)pDataBuffer = wTaskCount; 

		//发送数据
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,pDataBuffer,wSendSize);
	}

	//构造结构
	CMD_GR_S_TaskInfo TaskInfo;
	TaskInfo.wTaskCount = pTaskInfo->wTaskCount;
	CopyMemory(TaskInfo.TaskStatus,pTaskInfo->TaskStatus,sizeof(TaskInfo.TaskStatus[0])*pTaskInfo->wTaskCount);

	//发送数据
	WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
	wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_INFO,&TaskInfo,wSendDataSize);

	return true;
}

//任务结果
bool CAttemperEngineSink::OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//判断在线
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskResult));
	if(wDataSize>sizeof(DBO_GR_TaskResult)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBO_GR_TaskResult * pTaskResult=(DBO_GR_TaskResult *)pData;

	//变量定义
	CMD_GR_S_TaskResult TaskResult;
	ZeroMemory(&TaskResult,sizeof(TaskResult));

	//构造变量
	TaskResult.wCommandID=pTaskResult->wCommandID;
	TaskResult.bSuccessed=pTaskResult->bSuccessed;
	TaskResult.lCurrScore=pTaskResult->lCurrScore;
	TaskResult.lCurrIngot=pTaskResult->lCurrIngot;
	lstrcpyn(TaskResult.szNotifyContent,pTaskResult->szNotifyContent,CountArray(TaskResult.szNotifyContent));

	//发送数据
	WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_RESULT,&TaskResult,wSendSize);

	//设置任务
	if(TaskResult.bSuccessed==true && TaskResult.wCommandID==SUB_GR_TASK_TAKE)
	{
		tagTaskParameter * pTaskParameter=m_UserTaskManager.SearchTaskParameter(pTaskResult->wCurrTaskID);
		if(pTaskParameter!=NULL && pTaskParameter->wKindID==m_pGameServiceOption->wKindID)
		{
			//变量定义
			tagUserTaskInfo UserTaskInfo;

			//设置任务		
			UserTaskInfo.wTaskProgress=0;
			UserTaskInfo.cbTaskStatus=TASK_STATUS_UNFINISH;
			UserTaskInfo.wTaskID=pTaskResult->wCurrTaskID;
			UserTaskInfo.dwResidueTime=pTaskParameter->dwTimeLimit;
			UserTaskInfo.dwLastUpdateTime=(DWORD)time(NULL);

			//设置任务
			m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(),&UserTaskInfo,1);
		}
	}

	return true;
}

//系统消息
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBR_GR_SystemMessage));
	if(wDataSize!=sizeof(DBR_GR_SystemMessage)) return false;

	//提取数据
	DBR_GR_SystemMessage * pSystemMessage = (DBR_GR_SystemMessage *)pData;
	if(pSystemMessage==NULL) return false;

	for(INT_PTR nIndex=m_SystemMessageBuffer.GetCount()-1;nIndex>=0;nIndex--)
	{
		tagSystemMessage *pTagSystemMessage = m_SystemMessageBuffer[nIndex];
		if(pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID==pSystemMessage->dwMessageID)
		{
			CopyMemory(&pTagSystemMessage->SystemMessage, pSystemMessage, sizeof(DBR_GR_SystemMessage));
			m_SystemMessageActive.Add(pTagSystemMessage);
			m_SystemMessageBuffer.RemoveAt(nIndex);

			return true;
		}
	}

	//定义变量
	tagSystemMessage  *pSendMessage=new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//设置变量
	pSendMessage->dwLastTime=(DWORD)pSystemMessage->tStartTime;
	CopyMemory(&pSendMessage->SystemMessage,pSystemMessage, sizeof(DBR_GR_SystemMessage));

	//记录消息
	m_SystemMessageActive.Add(pSendMessage);

	return true;
}

//加载完成
bool CAttemperEngineSink::OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//处理临时消息
	if(m_SystemMessageBuffer.GetCount()>0)
	{
		//变量定义
		tagSystemMessage *pTagSystemMessage=NULL;

		for(INT_PTR nIndex=m_SystemMessageBuffer.GetCount()-1;nIndex>=0;nIndex--)
		{
			pTagSystemMessage = m_SystemMessageBuffer[nIndex];
			if(pTagSystemMessage && pTagSystemMessage->SystemMessage.dwMessageID==TEMP_MESSAGE_ID)
			{
				m_SystemMessageActive.Add(pTagSystemMessage);
				m_SystemMessageBuffer.RemoveAt(nIndex);
			}
		}
	}

	return true;
}

//加载敏感词
bool CAttemperEngineSink::OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//开始加载
	if(dwContextID==0xfffe)
	{
		m_WordsFilter.ResetSensitiveWordArray();
		m_pITimerEngine->KillTimer(IDI_LOAD_SENSITIVE_WORD);
		return true;			
	}

	//加载完成
	if(dwContextID==0xffff)
	{
		m_WordsFilter.FinishAdd();
		return true;
	}

	//加载敏感词
	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.AddSensitiveWords(pWords);
	return true;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//变量定义
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//效验参数
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
			}

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//房间信息
		{
			//废弃列表
			m_ServerListManager.DisuseServerItem();

			//注册比赛
			if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				//构造结构
				CMD_CS_C_RegisterMatch RegitsterMatch;
				ZeroMemory(&RegitsterMatch,sizeof(RegitsterMatch));

				//设置结构
				RegitsterMatch.wServerID = m_pGameServiceOption->wServerID;
				RegitsterMatch.dwMatchID =  m_pGameMatchOption->dwMatchID;
				RegitsterMatch.dwMatchNO =  m_pGameMatchOption->dwMatchNO;
				RegitsterMatch.cbMatchType = m_pGameMatchOption->cbMatchType;
				RegitsterMatch.lMatchFee =  m_pGameMatchOption->lMatchFee;
				RegitsterMatch.cbMatchFeeType =  m_pGameMatchOption->cbMatchFeeType;
				RegitsterMatch.cbMemberOrder =  m_pGameMatchOption->cbMemberOrder;
				RegitsterMatch.wRewardCount = m_pGameMatchOption->wRewardCount;

				//拷贝信息
				lstrcpyn(RegitsterMatch.szMatchName,m_pGameMatchOption->szMatchName,CountArray(RegitsterMatch.szMatchName));

				//定时赛制
				if(m_pGameMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
				{	
					RegitsterMatch.MatchEndTime = m_pGameMatchOption->MatchType.LockTime.MatchEndTime;
					RegitsterMatch.MatchStartTime = m_pGameMatchOption->MatchType.LockTime.MatchStartTime;					
					RegitsterMatch.wMatchPlayCount = m_pGameMatchOption->MatchType.LockTime.wMatchPlayCount;
				}

				//即时赛制
				if(m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
				{
					RegitsterMatch.wMatchPlayCount = m_pGameMatchOption->MatchType.Immediate.cbPlayCount;
					RegitsterMatch.wStartUserCount = m_pGameMatchOption->MatchType.Immediate.wStartUserCount;				
				}				

				//发送数据
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_MATCH,&RegitsterMatch,sizeof(RegitsterMatch));
			}

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//房间人数
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//变量定义
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);

			//设置人数
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//房间插入
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//变量定义
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//更新数据
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//房间修改
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//变量定义
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//设置房间
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwFullCount=pServerModify->dwFullCount;
				lstrcpyn(pGameServerItem->m_GameServer.szServerName,pServerModify->szServerName,CountArray(pGameServerItem->m_GameServer.szServerName));
				lstrcpyn(pGameServerItem->m_GameServer.szServerAddr,pServerModify->szServerAddr,CountArray(pGameServerItem->m_GameServer.szServerAddr));
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//房间删除
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//变量定义
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//变量定义
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//房间完成
		{
			//清理列表
			m_ServerListManager.CleanServerItem();

			//事件处理
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//汇总事件
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//用户汇总
		{
			//变量定义
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//发送用户
			WORD wIndex=0;
			do
			{
				//获取用户
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//设置变量
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				//UserEnter.dwGameID=pIServerUserItem->GetGameID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//辅助信息
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

				//发送数据
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));

			} while (true);

			//汇报完成
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:	//系统消息
		{
			//消息处理
			SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);

			return true;
		}
	//case SUB_CS_S_PROPERTY_TRUMPET:  //喇叭消息
	//	{
	//		//发送数据
	//		//m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_PROPERTY_TRUMPET,pData,wDataSize,BG_COMPUTER);

	//		return true;
	//	}
	//case SUB_CS_S_PLATFORM_PARAMETER: //平台参数
	//	{
	//		//清除任务			
	//		m_wTaskCount=0;
	//		ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

	//		//加载任务
	//		m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

	//		return true;
	//	}
	}

	return true;
}

//机器服务
bool CAttemperEngineSink::OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_ADDPARAMETER:		//添加参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_S_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_S_AddParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_S_AddParameter * pAddParameter = (CMD_CS_S_AddParameter *)pData;

			//添加参数
			m_AndroidUserManager.AddAndroidParameter(&pAddParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_MODIFYPARAMETER:  //修改参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_S_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_S_ModifyParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_S_ModifyParameter * pModifyParameter = (CMD_CS_S_ModifyParameter *)pData;

			//修改参数
			m_AndroidUserManager.AddAndroidParameter(&pModifyParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_DELETEPARAMETER:  //删除参数
		{
			//参数校验
			ASSERT(sizeof(CMD_CS_S_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_S_DeleteParameter)!=wDataSize) return false;

			//提取数据
			CMD_CS_S_DeleteParameter * pDeleteParameter = (CMD_CS_S_DeleteParameter *)pData;

			//删除参数
			m_AndroidUserManager.RemoveAndroidParameter(pDeleteParameter->dwBatchID);

			return true;
		}
	}

	return true;
}

//网站服务
bool CAttemperEngineSink::OnTCPSocketMainWebService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_BUY_PROP_FROM_WEB:
	{
		//参数校验
		ASSERT(sizeof(CMD_CS_S_BuyPropFromWeb) == wDataSize);
		if (sizeof(CMD_CS_S_BuyPropFromWeb) != wDataSize) return false;

		//提取数据
		CMD_CS_S_BuyPropFromWeb * pBuyProp = (CMD_CS_S_BuyPropFromWeb *)pData;

		//查询玩家
		IServerUserItem * pIUserItem = m_ServerUserManager.SearchUserItem(pBuyProp->dwUserID);
		if ( NULL==pIUserItem )
		{
			return true;
		}

		tagUserProperty prop;
		for (BYTE i = 0; i < pBuyProp->cbBuyNum; i++)
		{
			prop.dwDBID = pBuyProp->aryDBID[i];
			prop.wPropertyUseMark = pBuyProp->aryPropID[i];
			prop.wPropertyCount = pBuyProp->aryPropCount[i];
			pIUserItem->AddUserProperty(prop);
		}

		return true;
	}
	default:
		break;
	}


	return false;
}

//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (SUB_GR_LOGON_USERID == wSubCmdID)		//I D 登录
	{
		return OnTCPNetworkSubLogonUserID(pData,wDataSize,dwSocketID);
	}

	return true;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//用户规则
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_LOOKON:		//用户旁观
		{
			return OnTCPNetworkSubUserLookon(pData, wDataSize, dwSocketID);
		}
	case SUB_GR_USER_SITDOWN:		//用户坐下
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_STANDUP:		//用户起立
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAT:			//用户聊天
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_EXPRESSION:	//用户表情
		{
			return OnTCPNetworkSubUserExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_CHAT:		//用户私聊
		{
			return OnTCPNetworkSubWisperChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_EXPRESSION:	//私聊表情
		{
			return OnTCPNetworkSubWisperExpression(pData,wDataSize,dwSocketID);
		}
	//case SUB_GR_PROPERTY_BUY:		//购买道具
	//	{
	//		return OnTCPNetworkSubPropertyBuy(pData,wDataSize,dwSocketID);
	//	}
	//case SUB_GR_PROPERTY_TRUMPET:   //喇叭道具
	//	{
	//		return OnTCPNetwordSubSendTrumpet(pData,wDataSize,dwSocketID);
	//	}
	case SUB_GR_USER_INVITE_REQ:    //邀请用户
		{
			return OnTCPNetworkSubUserInviteReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_REPULSE_SIT:   //拒绝厌友
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_KICK_USER:    //踢出用户
		{
			return OnTCPNetworkSubMemberKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //请求更换位置
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//银行处理
bool CAttemperEngineSink::OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_ENABLE_INSURE_REQUEST:	//开通银行
		{
			return OnTCPNetworkSubEnableInsureRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_INSURE_INFO:		//银行查询
		{
			return OnTCPNetworkSubQueryInsureInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SAVE_SCORE_REQUEST:		//存款请求
		{
			return OnTCPNetworkSubSaveScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TAKE_SCORE_REQUEST:		//取款请求
		{
			return OnTCPNetworkSubTakeScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TRANSFER_SCORE_REQUEST:	//转帐请求
		{
			return OnTCPNetworkSubTransferScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_USER_INFO_REQUEST://查询用户
		{
			return OnTCPNetworkSubQueryUserInfoRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//任务处理
bool CAttemperEngineSink::OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_TASK_TAKE:				//领取任务
		{
			return OnTCPNetworkSubTakeTaskRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_REWARD:			//领取奖励
		{
			return OnTCPNetworkSubTaskRewardRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_LOAD_INFO:			//加载任务
		{
			return OnTCPNetworkSubLoadTaskInfoRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//兑换处理
bool CAttemperEngineSink::OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_EXCHANGE_LOAD_INFO:	//查询信息
		{
			return OnTCPNetworkSubQueryExchangeInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_EXCHANGE_SCORE:		//兑换游戏币
		{
			return OnTCPNetworkSubExchangeScore(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//管理处理
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_QUERY_OPTION:		//查询设置
		{
			return OnTCPNetworkSubQueryOption(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_OPTION_SERVER:		//房间设置
		{
			return OnTCPNetworkSubOptionServer(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KILL_USER:          //踢出用户
		{
			return OnTCPNetworkSubManagerKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LIMIT_USER_CHAT:	//限制聊天
		{
			return OnTCPNetworkSubLimitUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KICK_ALL_USER:		//踢出用户
		{
			return OnTCPNetworkSubKickAllUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_MESSAGE:		//发布消息
		{
			return OnTCPNetworkSubSendMessage(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_DISMISSGAME:        //解散游戏
		{
			return OnTCPNetworkSubDismissGame(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_WARNING:		//警告用户
		{
			return OnTCPNetworkSubWarningUser(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//比赛命令
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//效验接口
	ASSERT(m_pIMatchServiceManager!=NULL);
	if (m_pIMatchServiceManager==NULL) return false;

	//消息处理
	return m_pIMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

//游戏处理
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架处理
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//I D 登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonUserID));
	if (wDataSize<sizeof(CMD_GR_LogonUserID)) return false;

	//处理消息
	CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

	//绑定信息
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//重复判断
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	////密码校验
	//if((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0)
	//{
	//	//密码比较
	//	if(lstrcmp(pLogonUserID->szServerPasswd,m_pGameServiceOption->szServerPasswd)!=0)
	//	{
	//		//发送失败
	//		SendLogonFailure(TEXT("抱歉，您输入的房间密码不正确，请重新输入！"),0,dwSocketID);

	//		return true;
	//	}
	//}

	////房间判断
	//if(pLogonUserID->wKindID != m_pGameServiceOption->wKindID)
	//{
	//	//发送失败
	//	SendLogonFailure(TEXT("很抱歉，此游戏房间已经关闭了，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
	//	return true;
	//}

	//机器人和真人不许互踢
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("该帐号已在此房间游戏，且不允许踢出，请咨询管理员！"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return true;
		}
	}

	//密码判断
	CString strPassword = pLogonUserID->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//发送失败
		SendLogonFailure(TEXT("很抱歉，您的登录密码错误，不允许继续进入！"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//版本信息
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonUserID->dwPlazaVersion;

	//if(LOWORD(dwSocketID) < INDEX_ANDROID)
	//{
	//	//大厅版本
	//	DWORD dwPlazaVersion=pLogonUserID->dwPlazaVersion;
	//	DWORD dwFrameVersion=pLogonUserID->dwFrameVersion;
	//	DWORD dwClientVersion=pLogonUserID->dwProcessVersion;
	//	if (PerformCheckVersion(dwPlazaVersion,dwFrameVersion,dwClientVersion,dwSocketID)==false) return true;
	//}

	//切换判断
	if((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonUserID->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem, pLogonUserID->szMachineID, dwSocketID);
		return true;
	}

	//比赛效验
	if(m_pIMatchServiceManager!=NULL&&m_pIMatchServiceManager->OnEventEnterMatch(dwSocketID, pData, pBindParameter->dwClientAddr,false))
	{
		//SendRoomMessage(dwSocketID, szPrint, SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
		return true;
	}

	//变量定义
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwMatchID=m_pGameMatchOption->dwMatchID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));

	return true;
}

//用户规则
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true||m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) return true;

	//消息处理
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//规则标志
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//规则属性
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//桌子密码
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//变量定义
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//提取处理
		while (true)
		{
			//提取数据
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//数据分析
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//桌子密码
				{
					//效验数据
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//设置数据
					if (DataDescribe.wDataSize<=sizeof(pUserRule->szPassword))
					{
						CopyMemory(&pUserRule->szPassword,pDataBuffer,DataDescribe.wDataSize);
						pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;
					}

					break;
				}
			}
		}
	}

	return true;
}

bool CAttemperEngineSink::OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UserLookon));
	if (wDataSize != sizeof(CMD_GR_UserLookon)) return false;

	//效验数据
	CMD_GR_UserLookon * pUserLookon = (CMD_GR_UserLookon *)pData;
	//if (pUserLookon->wChairID >= m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID >= (WORD)m_TableFrameArray.GetCount()) return false;

	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//消息处理
	WORD wTableID = pIServerUserItem->GetTableID();
	//保存自己原来椅子号，发送给客户端用来做视图转换
	WORD wChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
	if ((wTableID == pUserLookon->wTableID) && (cbUserStatus == US_LOOKON)) return true;

	//用户判断
	if (cbUserStatus == US_PLAYING)
	{
		//SendRequestFailure(pIServerUserItem, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"), 0);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//玩家做在桌子上离开处理
	if (wTableID != INVALID_TABLE)
	{
		CTableFrame * pTableFrame = m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem) == false) return true;
	}

	//坐下处理
	CTableFrame * pTableFrame = m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(wChairID, pIServerUserItem);

	return true;
}

//用户坐下
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//效验数据
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//重复判断
	if ((pUserSitDown->wTableID<m_pGameServiceOption->wTableCount)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		//SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//插入分组
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//加入比赛
		InsertDistribute(pIServerUserItem);	
		
		return true;
	}

	bool isLookOn = false;
	WORD oldTableId = pIServerUserItem->GetTableID();
	WORD oldChairID = pIServerUserItem->GetChairID();

	//玩家本身就是旁观状态，坐下失败后恢复旁观
	if (wTableID != INVALID_TABLE && pIServerUserItem->GetUserStatus() == US_LOOKON)
	{
		isLookOn = true;
	}

	////离开处理
	//if (wTableID!=INVALID_TABLE)
	//{
	//	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	//	if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	//}
	//
	////防作弊(暂时注释)
	//if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	//{
	//	if(m_TableFrameArray[0]->EfficacyEnterTableScoreRule(0, pIServerUserItem))
	//	{
	//		InsertDistribute(pIServerUserItem);
	//	}
	//
	//	return true;
	//}

	//请求调整
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;
	
	// 桌子号和椅子号都为初始值时，服务器安排玩家座位
	if (wRequestTableID == INVALID_TABLE && wRequestChairID == INVALID_CHAIR)
	{
		UserImmediately(pIServerUserItem);
		return true;
	}

	//桌子调整
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//起始桌子
		WORD wStartTableID=0;
		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
		if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//动态加入
		bool bDynamicJoin=true;
		if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
		if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		//寻找位置
		for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
		{
			//游戏状态
			if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

			//获取空位
			WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

			//调整结果
			if (wNullChairID!=INVALID_CHAIR)
			{
				//设置变量
				wRequestTableID=i;
				wRequestChairID=wNullChairID;

				break;
			}
		}

		//结果判断
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			//SendRequestFailure(pIServerUserItem,TEXT("当前游戏房间已经人满为患了，暂时没有可以让您加入的位置，请稍后再试！"),0);
			SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_FULL_PEOPLE);
			return true;
		}
	}

	//椅子调整
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//效验参数
		ASSERT(wRequestTableID<m_TableFrameArray.GetCount());
		if (wRequestTableID>=m_TableFrameArray.GetCount()) return false;

		//查找空位
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//结果判断
		if (wRequestChairID==INVALID_CHAIR)
		{
			//SendRequestFailure(pIServerUserItem,TEXT("由于此游戏桌暂时没有可以让您加入的位置了，请选择另外的游戏桌！"),0);
			SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_CHAIR);
			return true;
		}
	}

	//坐下处理
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	
	//离开处理
	if (wTableID != INVALID_TABLE)
	{
		if (pTableFrame->PerformStandUpAction(pIServerUserItem) == false) return true;
	}

	//做下失败，恢复旁观
	if (pTableFrame->PerformSitDownAction(wRequestChairID, pIServerUserItem, pUserSitDown->szPassword)== false && isLookOn)
	{

		CTableFrame * pOldTableFrame = m_TableFrameArray[oldTableId];
		pOldTableFrame->PerformRecoerLookOn(oldChairID, pIServerUserItem);
	}

	return true;
}

//用户起立
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//定义变量
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;

	//取消分组
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		//取消分组
		DeleteDistribute(pIServerUserItem);

		if(pUserStandUp->wTableID==INVALID_TABLE) return true;
	}

	//效验数据
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//用户判断
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		//SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	return true;
}

//用户聊天
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//变量定义
	CMD_GR_C_UserChat * pUserChat=(CMD_GR_C_UserChat *)pData;

	//效验参数
	ASSERT(wDataSize<=sizeof(CMD_GR_C_UserChat));
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

	//效验参数
	if (wDataSize>sizeof(CMD_GR_C_UserChat)) return false;
	if (wDataSize<(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//权限判断
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有大厅发言的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pUserChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//构造消息
	CMD_GR_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//字符过滤
	SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));
	
	//构造数据
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pUserChat->dwSendUserID;
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//转发消息
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}

//用户表情
bool CAttemperEngineSink::OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_UserExpression));
	if (wDataSize!=sizeof(CMD_GR_C_UserExpression)) return false;

	//变量定义
	CMD_GR_C_UserExpression * pUserExpression=(CMD_GR_C_UserExpression *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserExpression->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserExpression->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//权限判断
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有大厅发言的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pUserExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//构造消息
	CMD_GR_S_UserExpression UserExpression;
	ZeroMemory(&UserExpression,sizeof(UserExpression));

	//构造数据
	UserExpression.wItemIndex=pUserExpression->wItemIndex;
	UserExpression.dwSendUserID=pUserExpression->dwSendUserID;
	UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

	//转发消息
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));

	return true;
}

//用户私聊
bool CAttemperEngineSink::OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//变量定义
	CMD_GR_C_WisperChat * pWisperChat=(CMD_GR_C_WisperChat *)pData;

	//效验参数
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0])));

	//效验参数
	if (wDataSize<(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0]))) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperChat->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//同桌判断
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//变量定义
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//规则判断
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//提示消息
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("抱歉，此游戏房间不允许在游戏中与同桌的玩家私聊！"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//权限判断
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有发送私聊的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pWisperChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//变量定义
	CMD_GR_S_WisperChat WisperChat;
	ZeroMemory(&WisperChat,sizeof(WisperChat));

	//字符过滤
	SensitiveWordFilter(pWisperChat->szChatString,WisperChat.szChatString,CountArray(WisperChat.szChatString));

	//构造数据
	WisperChat.dwChatColor=pWisperChat->dwChatColor;
	WisperChat.wChatLength=pWisperChat->wChatLength;
	WisperChat.dwSendUserID=pISendUserItem->GetUserID();
	WisperChat.dwTargetUserID=pIRecvUserItem->GetUserID();
	WisperChat.wChatLength=CountStringBuffer(WisperChat.szChatString);

	//转发消息
	WORD wHeadSize=sizeof(WisperChat)-sizeof(WisperChat.szChatString);
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//转发消息
	WisperChat.dwSendUserID=pWisperChat->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//机器判断
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//枚举用户
		WORD wEnumIndex=0;
		IServerUserItem * pTempServerUserItem=NULL;
		do
		{
			pTempServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if(pTempServerUserItem!=NULL)
			{
				if(pTempServerUserItem==pISendUserItem) continue;
				if(pTempServerUserItem==pIRecvUserItem) continue;
				if(pTempServerUserItem->GetMasterOrder()>0 && 
				   CMasterRight::CanManagerAndroid(pTempServerUserItem->GetMasterRight())==true)
				{
					SendData(pTempServerUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));
				}
			}
		}while(pTempServerUserItem!=NULL);
	}

	return true;
}

//用户表情
bool CAttemperEngineSink::OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_WisperExpression));
	if (wDataSize!=sizeof(CMD_GR_C_WisperExpression)) return false;

	//变量定义
	CMD_GR_C_WisperExpression * pWisperExpression=(CMD_GR_C_WisperExpression *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperExpression->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//同桌判断
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//变量定义
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//规则判断
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//提示消息
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("抱歉，此游戏房间不允许在游戏中与同桌的玩家私聊！"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//权限判断
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("抱歉，您没有发送私聊的权限，若需要帮助，请联系游戏客服咨询！"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//权限判断
	if (pWisperExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//变量定义
	CMD_GR_S_WisperExpression WisperExpression;
	ZeroMemory(&WisperExpression,sizeof(WisperExpression));

	//构造数据
	WisperExpression.wItemIndex=pWisperExpression->wItemIndex;
	WisperExpression.dwSendUserID=pISendUserItem->GetUserID();
	WisperExpression.dwTargetUserID=pWisperExpression->dwTargetUserID;

	//转发消息
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//转发消息
	WisperExpression.dwSendUserID=pWisperExpression->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//机器判断
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//枚举用户
		WORD wEnumIndex=0;
		IServerUserItem * pTempServerUserItem=NULL;
		do
		{
			pTempServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if(pTempServerUserItem!=NULL)
			{
				if(pTempServerUserItem==pISendUserItem) continue;
				if(pTempServerUserItem==pIRecvUserItem) continue;
				if(pTempServerUserItem->GetMasterOrder()>0)
				{
					SendData(pTempServerUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));
				}
			}
		}while(pTempServerUserItem!=NULL);
	}

	return true;
}

////购买道具
//bool CAttemperEngineSink::OnTCPNetworkSubPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
//{
//	//事件处理
//	int cbResult = OnPropertyBuy(pData, wDataSize, dwSocketID);
//
//	//结果判断
//	if(cbResult == RESULT_ERROR) return false;
//	if(cbResult == RESULT_FAIL) return true;
//	if(cbResult == RESULT_SUCCESS) return true;
//
//	return true;
//}

////购买道具
//int CAttemperEngineSink::OnPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
//{
//	//效验参数
//	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyBuy));
//	if (wDataSize!=sizeof(CMD_GR_C_PropertyBuy)) return RESULT_ERROR;
//
//	//变量定义
//	CMD_GR_C_PropertyBuy * pPropertyBuy=(CMD_GR_C_PropertyBuy *)pData;
//
//	//数据效验
//	ASSERT(pPropertyBuy->wItemCount>0);
//	if (pPropertyBuy->wItemCount==0) return RESULT_ERROR;
//
//	//获取用户
//	WORD wBindIndex=LOWORD(dwSocketID);
//	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
//
//	//用户效验
//	ASSERT(pIServerUserItem!=NULL);
//	if (pIServerUserItem==NULL) return RESULT_ERROR;
//
//	//目标用户
//	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pPropertyBuy->dwTargetUserID);
//	if (pITargerUserItem==NULL) return RESULT_ERROR;
//
//	//比赛房间
//	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
//	{
//		//发送消息
//		SendPropertyFailure(pIServerUserItem,TEXT("比赛房间不可以使用此功能！"),0L,pPropertyBuy->cbRequestArea);
//
//		return RESULT_FAIL;
//	}
//
//	//练习房间
//	if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
//	{
//		SendPropertyFailure(pIServerUserItem,TEXT("练习房间不可以使用此功能！"),0L,pPropertyBuy->cbRequestArea);
//		return RESULT_FAIL;
//	}
//
//	//购前事件
//	if(OnEventPropertyBuyPrep(pPropertyBuy->cbRequestArea,pPropertyBuy->wPropertyIndex,pIServerUserItem,pITargerUserItem)==false)
//		return RESULT_FAIL;
//
//	//变量定义
//	DBR_GR_PropertyRequest PropertyRequest;
//	ZeroMemory(&PropertyRequest,sizeof(PropertyRequest));
//
//	//查找道具
//	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(pPropertyBuy->wPropertyIndex);
//	if (pPropertyInfo==NULL)
//	{
//		SendPropertyFailure(pIServerUserItem,TEXT("您购买的道具不存在或在维护中，请与管理员联系！"),0L,pPropertyBuy->cbRequestArea);
//		return RESULT_FAIL;
//	}
//
//	//消费方式
//	if (pPropertyBuy->cbConsumeScore==TRUE)
//	{
//		//房间判断
//		ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
//		if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return RESULT_FAIL;		
//
//		//锁定计算
//		PropertyRequest.cbConsumeScore=TRUE;
//		PropertyRequest.lFrozenedScore=pPropertyInfo->lPropertyGold*pPropertyBuy->wItemCount;
//
//		//会员折扣
//		if (pIServerUserItem->GetMemberOrder()>0)
//		{
//			PropertyRequest.lFrozenedScore = PropertyRequest.lFrozenedScore*pPropertyInfo->wDiscount/100L;
//		}
//
//		//锁定积分
//		if (pIServerUserItem->FrozenedUserScore(PropertyRequest.lFrozenedScore)==false)
//		{
//			SendPropertyFailure(pIServerUserItem,TEXT("您的游戏币余额不足，道具购买失败！"),0L,pPropertyBuy->cbRequestArea);
//			return RESULT_FAIL;
//		}
//	}
//	else
//	{
//		//银行扣费
//		PropertyRequest.lFrozenedScore=0L;
//		PropertyRequest.cbConsumeScore=FALSE;
//
//		//变量定义
//		SCORE lInsure = pIServerUserItem->GetUserInsure();
//		SCORE lConsumeScore = pPropertyInfo->lPropertyGold*pPropertyBuy->wItemCount;
//
//		//会员折扣
//		if (pIServerUserItem->GetMemberOrder()>0)
//		{
//			lConsumeScore = lConsumeScore*pPropertyInfo->wDiscount/100L;
//		}
//
//		//银行校验
//		if(lInsure < lConsumeScore)
//		{
//			SendPropertyFailure(pIServerUserItem,TEXT("您的银行余额不足，请存款后再次购买！"),0L,pPropertyBuy->cbRequestArea);
//			return RESULT_FAIL;
//		}		
//	}
//
//	//购买信息
//	PropertyRequest.cbRequestArea=pPropertyBuy->cbRequestArea;
//	PropertyRequest.wItemCount=pPropertyBuy->wItemCount;
//	PropertyRequest.wPropertyIndex=pPropertyBuy->wPropertyIndex;
//	PropertyRequest.dwSourceUserID=pIServerUserItem->GetUserID();
//	PropertyRequest.dwTargetUserID=pITargerUserItem->GetUserID();
//	PropertyRequest.dwUserRight = 0;
//
//	//系统信息
//	PropertyRequest.wTableID=INVALID_TABLE;
//	PropertyRequest.dwInoutIndex=pIServerUserItem->GetInoutIndex();
//	PropertyRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
//	lstrcpyn(PropertyRequest.szMachineID,pIServerUserItem->GetMachineID(),CountArray(PropertyRequest.szMachineID));
//
//	//投递数据
//	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_REQUEST,dwSocketID,&PropertyRequest,sizeof(PropertyRequest));
//
//	return RESULT_SUCCESS;
//}
//
////使用道具
//bool CAttemperEngineSink::OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
//{
//	//效验参数
//	ASSERT(wDataSize==sizeof(CMD_GR_C_SendTrumpet));
//	if (wDataSize!=sizeof(CMD_GR_C_SendTrumpet)) return false;
//
//	//变量定义
//	CMD_GR_C_SendTrumpet * pSendTrumpet=(CMD_GR_C_SendTrumpet *)pData;
//
//	//获取用户
//	WORD wBindIndex=LOWORD(dwSocketID);
//	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
//	if(pIServerUserItem==NULL) return false;
//
//	//构造结构
//	CMD_GR_C_PropertyBuy  PropertyBuy;
//	PropertyBuy.cbRequestArea=pSendTrumpet->cbRequestArea;
//	PropertyBuy.dwTargetUserID=pIServerUserItem->GetUserID();
//	PropertyBuy.wPropertyIndex=pSendTrumpet->wPropertyIndex;
//	PropertyBuy.cbConsumeScore=FALSE;
//	PropertyBuy.wItemCount=1;
//    
//	//购买
//	int cbResult = OnPropertyBuy((void *)&PropertyBuy,sizeof(PropertyBuy),dwSocketID);
//
//	//结果判断
//	if(cbResult == RESULT_ERROR) return false;
//	if(cbResult != RESULT_SUCCESS) return true;
//
//	//获取道具
//	tagUserProperty * pUserProperty = pIServerUserItem->GetUserProperty();
//
//	//道具索引
//	BYTE cbIndex= 2;
//
//	//构造结构
//	CMD_GR_S_SendTrumpet  SendTrumpet;
//	SendTrumpet.dwSendUserID=pIServerUserItem->GetUserID();
//	SendTrumpet.wPropertyIndex=pSendTrumpet->wPropertyIndex;
//	SendTrumpet.TrumpetColor=pSendTrumpet->TrumpetColor;
//	ZeroMemory(SendTrumpet.szTrumpetContent,sizeof(SendTrumpet.szTrumpetContent));
//	CopyMemory(SendTrumpet.szSendNickName,pIServerUserItem->GetNickName(),sizeof(SendTrumpet.szSendNickName));
//
//	//字符过滤
//	SensitiveWordFilter(pSendTrumpet->szTrumpetContent,SendTrumpet.szTrumpetContent,CountArray(SendTrumpet.szTrumpetContent));
//
//    //房间转发
//	if(cbIndex==3)
//	{
//		//广播房间
//		WORD wUserIndex=0;
//		CMD_CS_S_SendTrumpet * pSendTrumpet = (CMD_CS_S_SendTrumpet *)&SendTrumpet;
//		if(m_pITCPSocketService)
//		{
//			m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,pSendTrumpet,sizeof(CMD_CS_S_SendTrumpet));
//		}
//	}
//
//	//游戏转发
//	if(cbIndex==2)
//	{
//		//发送数据
//		m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet),BG_COMPUTER);
//	}
//
//	return true;
//}

//邀请用户
bool CAttemperEngineSink::OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//消息处理
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//效验状态
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//目标用户
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//发送消息
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

	return true;
}

//拒绝厌友
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//消息处理
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//获取用户
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//发送消息
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("此桌有玩家设置了不与您同桌游戏！"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//弹起玩家
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//踢出命令
bool CAttemperEngineSink::OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//目标用户
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//权限判断
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//禁踢管理员
	if(pITargetUserItem->GetMasterOrder() > 0)
	{
		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("很抱歉，踢出管理员是不被允许的！"),SMT_EJECT);
		return true;
	}

	//百人游戏
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("很抱歉，百人游戏不许踢人！"),SMT_EJECT);
		return true;
	}

	//用户状态
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//变量定义
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("由于玩家 [ %s ] 正在游戏中,您不能将它踢出游戏！"), pITargetUserItem->GetNickName());

		//发送消息
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	////防踢判断
	//if((pITargetUserItem->GetUserProperty()->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
	//{
	//	//变量定义
	//	DWORD dwCurrentTime=(DWORD)time(NULL);
	//	tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

	//	//时效判断
	//	DWORD dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
	//	if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime>dwCurrentTime)
	//	{
	//		//变量定义
	//		TCHAR szMessage[256]=TEXT("");
	//		_sntprintf(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在使用防踢卡,您无法将它踢出游戏！"),pITargetUserItem->GetNickName());

	//		//发送消息
	//		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);

	//		return true; 
	//	}
	//	else
	//		pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_GUARDKICK_CARD;
	//}

	//请离桌子
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//定义变量
		TCHAR szMessage[64]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("你已被%s请离桌子！"), pIServerUserItem->GetNickName());

		//发送消息
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//请求更换位置
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户状态
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//失败
		//m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),REQUEST_FAILURE_NORMAL);
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//查找桌子
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=(i+nStartIndex)%m_pGameServiceOption->wTableCount;

		//过滤同桌
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//离开处理
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		////定义变量
		//WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		//WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		////更新信息
		//if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		//{
		//	WORD wNewPos = (nTableIndex/wTagerDeskCount)*wTagerDeskCount;
		//	pIServerUserItem->SetMobileUserDeskPos(wNewPos);
		//}

		//用户坐下
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//查找桌子
	nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=(i+nStartIndex)%m_pGameServiceOption->wTableCount;

		//过滤同桌
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//离开处理
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		////定义变量
		//WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		//WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		////更新信息
		//if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		//{
		//	WORD wNewPos = (nTableIndex/wTagerDeskCount)*wTagerDeskCount;
		//	pIServerUserItem->SetMobileUserDeskPos(wNewPos);
		//}

		//用户坐下
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//查找同桌
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//无效过滤
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//离开处理
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//用户坐下
				pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
				return true;
			}
		}
	}

	//失败
	//m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("没找到可进入的游戏桌！"),REQUEST_FAILURE_NORMAL);
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_TABLE);
	return true;
}

//查询参数
bool CAttemperEngineSink::OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//构造结构
	CMD_GR_ExchangeParameter ExchangeParameter;
	ZeroMemory(&ExchangeParameter,sizeof(ExchangeParameter));

	//设置变量
	ExchangeParameter.wMemberCount=m_wMemberCount;
	ExchangeParameter.wExchangeRate=0;	
	CopyMemory(ExchangeParameter.MemberParameter,m_MemberParameter,sizeof(tagMemberParameter)*m_wMemberCount);

	//计算大小
	WORD wSendDataSize = sizeof(ExchangeParameter)-sizeof(ExchangeParameter.MemberParameter);
	wSendDataSize += sizeof(tagMemberParameter)*ExchangeParameter.wMemberCount;

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_PARAM_INFO,&ExchangeParameter,wSendDataSize);

	return true;
}

//兑换游戏币
bool CAttemperEngineSink::OnTCPNetworkSubExchangeScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_ExchangeScore));
	if(wDataSize!=sizeof(CMD_GR_ExchangeScore)) return false;

	//构造结构
	CMD_GR_ExchangeScore * pExchangeScore = (CMD_GR_ExchangeScore*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_ExchangeScore ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//设置变量
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.lExchangeIngot=pExchangeScore->lExchangeIngot;
	ExchangeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(ExchangeScore.dwUserID,DBR_GR_EXCHANGE_SCORE,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

	return true;
}

//查询银行
bool CAttemperEngineSink::OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryInsureInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryInsureInfoRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_C_QueryInsureInfoRequest * pQueryInsureInfoRequest = (CMD_GR_C_QueryInsureInfoRequest *)pData;

	//变量定义
	DBR_GR_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//构造数据
	QueryInsureInfo.cbActivityGame=pQueryInsureInfoRequest->cbActivityGame;
	QueryInsureInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryInsureInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(QueryInsureInfo.szPassword,pQueryInsureInfoRequest->szInsurePass,CountArray(QueryInsureInfo.szPassword));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(QueryInsureInfo.dwUserID,DBR_GR_QUERY_INSURE_INFO,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));

	return true;
}

//开通银行
bool CAttemperEngineSink::OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_EnableInsureRequest));
	if (wDataSize!=sizeof(CMD_GR_C_EnableInsureRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_C_EnableInsureRequest * pEnableInsureRequest = (CMD_GR_C_EnableInsureRequest *)pData;

	//变量定义
	DBR_GR_UserEnableInsure UserEnableInsure;
	ZeroMemory(&UserEnableInsure,sizeof(UserEnableInsure));

	//构造数据	
	UserEnableInsure.dwUserID=pIServerUserItem->GetUserID();
	UserEnableInsure.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserEnableInsure.cbActivityGame=pEnableInsureRequest->cbActivityGame;
	lstrcpyn(UserEnableInsure.szLogonPass,pEnableInsureRequest->szLogonPass,CountArray(UserEnableInsure.szLogonPass));
	lstrcpyn(UserEnableInsure.szInsurePass,pEnableInsureRequest->szInsurePass,CountArray(UserEnableInsure.szInsurePass));
	lstrcpyn(UserEnableInsure.szMachineID,pEnableInsureRequest->szMachineID,CountArray(UserEnableInsure.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(UserEnableInsure.dwUserID,DBR_GR_USER_ENABLE_INSURE,dwSocketID,&UserEnableInsure,sizeof(UserEnableInsure));

	return true;
}

//存款请求
bool CAttemperEngineSink::OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_SaveScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_SaveScoreRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GR_C_SaveScoreRequest * pSaveScoreRequest=(CMD_GR_C_SaveScoreRequest *)pData;

	//效验参数
	ASSERT(pSaveScoreRequest->lSaveScore>0L);
	if (pSaveScoreRequest->lSaveScore<=0L) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	if(pSaveScoreRequest->cbActivityGame == FALSE)
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止房间存款，存入操作失败！"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//规则判断
	if(pSaveScoreRequest->cbActivityGame == TRUE)
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止游戏存款，存入操作失败！"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//变量定义
	SCORE lConsumeQuota=0L;
	SCORE lUserWholeScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();

	//获取限额
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		lConsumeQuota=m_TableFrameArray[wTableID]->QueryConsumeQuota(pIServerUserItem);
	}
	else
	{
		lConsumeQuota=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	}

	//限额判断
	if (pSaveScoreRequest->lSaveScore>lConsumeQuota)
	{
		if (lConsumeQuota<lUserWholeScore)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("由于您正在游戏中，游戏币可存入额度为 %I64d，存入操作失败！"), lConsumeQuota);

			//发送数据
			SendInsureFailure(pIServerUserItem,szDescribe,0L,pSaveScoreRequest->cbActivityGame);
		}
		else
		{
			//发送数据
			SendInsureFailure(pIServerUserItem,TEXT("您的游戏币余额不足，存入操作失败！"),0L,pSaveScoreRequest->cbActivityGame);
		}

		return true;
	}

	//锁定积分
	if (pIServerUserItem->FrozenedUserScore(pSaveScoreRequest->lSaveScore)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//变量定义
	DBR_GR_UserSaveScore UserSaveScore;
	ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

	//构造数据
	UserSaveScore.cbActivityGame=pSaveScoreRequest->cbActivityGame;
	UserSaveScore.dwUserID=pIServerUserItem->GetUserID();
	UserSaveScore.lSaveScore=pSaveScoreRequest->lSaveScore;
	UserSaveScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserSaveScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserSaveScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_SAVE_SCORE,dwSocketID,&UserSaveScore,sizeof(UserSaveScore));

	return true;
}

//取款请求
bool CAttemperEngineSink::OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_TakeScoreRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GR_C_TakeScoreRequest * pTakeScoreRequest=(CMD_GR_C_TakeScoreRequest *)pData;
	pTakeScoreRequest->szInsurePass[CountArray(pTakeScoreRequest->szInsurePass)-1]=0;

	//效验参数
	ASSERT(pTakeScoreRequest->lTakeScore>0L);
	if (pTakeScoreRequest->lTakeScore<=0L) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//规则判断
	if(pTakeScoreRequest->cbActivityGame == FALSE )
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止房间取款，取出操作失败！"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//规则判断
	if(pTakeScoreRequest->cbActivityGame == TRUE )
	{
		//发送数据
		SendInsureFailure(pIServerUserItem,TEXT("此房间禁止游戏取款，取出操作失败！"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//变量定义
	DBR_GR_UserTakeScore UserTakeScore;
	ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

	//构造数据
	UserTakeScore.cbActivityGame=pTakeScoreRequest->cbActivityGame;
	UserTakeScore.dwUserID=pIServerUserItem->GetUserID();
	UserTakeScore.lTakeScore=pTakeScoreRequest->lTakeScore;
	UserTakeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserTakeScore.szPassword,pTakeScoreRequest->szInsurePass,CountArray(UserTakeScore.szPassword));
	lstrcpyn(UserTakeScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTakeScore.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TAKE_SCORE,dwSocketID,&UserTakeScore,sizeof(UserTakeScore));

	return true;
}

//转帐请求
bool CAttemperEngineSink::OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GP_C_TransferScoreRequest));
	if (wDataSize!=sizeof(CMD_GP_C_TransferScoreRequest)) return false;

	//房间判断
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GP_C_TransferScoreRequest * pTransferScoreRequest=(CMD_GP_C_TransferScoreRequest *)pData;
	pTransferScoreRequest->szAccounts[CountArray(pTransferScoreRequest->szAccounts)-1]=0;
	pTransferScoreRequest->szInsurePass[CountArray(pTransferScoreRequest->szInsurePass)-1]=0;

	//效验参数
	ASSERT(pTransferScoreRequest->lTransferScore>0L);
	if (pTransferScoreRequest->lTransferScore<=0L) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	DBR_GR_UserTransferScore UserTransferScore;
	ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

	//构造数据
	UserTransferScore.cbActivityGame=pTransferScoreRequest->cbActivityGame;
	UserTransferScore.dwUserID=pIServerUserItem->GetUserID();
	UserTransferScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserTransferScore.lTransferScore=pTransferScoreRequest->lTransferScore;
	lstrcpyn(UserTransferScore.szAccounts,pTransferScoreRequest->szAccounts,CountArray(UserTransferScore.szAccounts));
	lstrcpyn(UserTransferScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTransferScore.szMachineID));
	lstrcpyn(UserTransferScore.szPassword,pTransferScoreRequest->szInsurePass,CountArray(UserTransferScore.szPassword));
	lstrcpyn(UserTransferScore.szTransRemark,pTransferScoreRequest->szTransRemark,CountArray(UserTransferScore.szTransRemark));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TRANSFER_SCORE,dwSocketID,&UserTransferScore,sizeof(UserTransferScore));

	return true;
}

//查询用户请求
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryUserInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryUserInfoRequest)) return false;

	//房间判断
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//变量定义
	CMD_GR_C_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GR_C_QueryUserInfoRequest *)pData;
	pQueryUserInfoRequest->szAccounts[CountArray(pQueryUserInfoRequest->szAccounts)-1]=0;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//ID判断
	if(pQueryUserInfoRequest->cbByNickName==FALSE)
	{
		//长度判断
		int nLen=lstrlen(pQueryUserInfoRequest->szAccounts);
		if(nLen>=8)
		{
			SendInsureFailure(pIServerUserItem,TEXT("请输入合法的玩家ID！"), 0, pQueryUserInfoRequest->cbActivityGame);
			return true;
		}

		//合法判断
		for(int i=0; i<nLen; i++)
		{
			if(pQueryUserInfoRequest->szAccounts[i] < TEXT('0') || pQueryUserInfoRequest->szAccounts[i] > TEXT('9'))
			{
				SendInsureFailure(pIServerUserItem,TEXT("请输入合法的玩家ID！"), 0, pQueryUserInfoRequest->cbActivityGame);
				return true;
			}
		}
	}

	//变量定义
	DBR_GR_QueryTransferUserInfo QueryTransferUserInfo;
	ZeroMemory(&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	//构造数据	
	QueryTransferUserInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryTransferUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
	QueryTransferUserInfo.cbActivityGame=pQueryUserInfoRequest->cbActivityGame;	
	lstrcpyn(QueryTransferUserInfo.szAccounts,pQueryUserInfoRequest->szAccounts,CountArray(QueryTransferUserInfo.szAccounts));	

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_TRANSFER_USER_INFO,dwSocketID,&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	return true;
}

//领取任务
bool CAttemperEngineSink::OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeTask));
	if(wDataSize!=sizeof(CMD_GR_C_TakeTask)) return false;

	//提取数据
	CMD_GR_C_TakeTask * pTaskTake = (CMD_GR_C_TakeTask *)pData;
	pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
	pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskTake TaskTake;
	TaskTake.dwUserID = pTaskTake->dwUserID;
	TaskTake.wTaskID = pTaskTake->wTaskID;
	TaskTake.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
	lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_TAKE,dwSocketID,&TaskTake,sizeof(TaskTake));

	return true;
}

//领取奖励
bool CAttemperEngineSink::OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_TaskReward));
	if(wDataSize!=sizeof(CMD_GR_C_TaskReward)) return false;

	//提取数据
	CMD_GR_C_TaskReward * pTaskReward = (CMD_GR_C_TaskReward *)pData;
	pTaskReward->szPassword[CountArray(pTaskReward->szPassword)-1]=0;
	pTaskReward->szMachineID[CountArray(pTaskReward->szMachineID)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskReward->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskReward TaskReward;
	TaskReward.dwUserID = pTaskReward->dwUserID;
	TaskReward.wTaskID = pTaskReward->wTaskID;
	TaskReward.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskReward.szPassword,pTaskReward->szPassword,CountArray(TaskReward.szPassword));
	lstrcpyn(TaskReward.szMachineID,pTaskReward->szMachineID,CountArray(TaskReward.szMachineID));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_REWARD,dwSocketID,&TaskReward,sizeof(TaskReward));

	return true;
}

//加载任务
bool CAttemperEngineSink::OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_C_LoadTaskInfo));
	if(wDataSize!=sizeof(CMD_GR_C_LoadTaskInfo)) return false;

	//提取数据
	CMD_GR_C_LoadTaskInfo * pLoadTaskInfo = (CMD_GR_C_LoadTaskInfo *)pData;
	pLoadTaskInfo->szPassword[CountArray(pLoadTaskInfo->szPassword)-1]=0;

	//查找用户
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLoadTaskInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBR_GR_TaskQueryInfo TaskQueryInfo;
	TaskQueryInfo.dwUserID = pLoadTaskInfo->dwUserID;
	lstrcpyn(TaskQueryInfo.szPassword,pLoadTaskInfo->szPassword,CountArray(TaskQueryInfo.szPassword));

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_QUERY_INFO,dwSocketID,&TaskQueryInfo,sizeof(TaskQueryInfo));

	return true;
}

//查询设置
bool CAttemperEngineSink::OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//变量定义
	CMD_GR_OptionCurrent OptionCurrent;
	ZeroMemory(&OptionCurrent,sizeof(OptionCurrent));

	//挂接属性
	OptionCurrent.ServerOptionInfo.wKindID=m_pGameServiceOption->wKindID;
	//OptionCurrent.ServerOptionInfo.wNodeID=m_pGameServiceOption->wNodeID;
	OptionCurrent.ServerOptionInfo.wSortID=m_pGameServiceOption->wSortID;

	//房间配置
	OptionCurrent.ServerOptionInfo.wRevenueRatio=m_pGameServiceOption->wRevenueRatio;
	OptionCurrent.ServerOptionInfo.lServiceScore=m_pGameServiceOption->lServiceScore;
	OptionCurrent.ServerOptionInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	OptionCurrent.ServerOptionInfo.lMinTableScore=m_pGameServiceOption->lMinTableScore;
	OptionCurrent.ServerOptionInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	OptionCurrent.ServerOptionInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

	//会员限制
	OptionCurrent.ServerOptionInfo.cbMinEnterMember=m_pGameServiceOption->cbMinEnterMember;
	OptionCurrent.ServerOptionInfo.cbMaxEnterMember=m_pGameServiceOption->cbMaxEnterMember;

	//房间属性
	OptionCurrent.ServerOptionInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	lstrcpyn(OptionCurrent.ServerOptionInfo.szServerName,m_pGameServiceOption->szServerName,CountArray(OptionCurrent.ServerOptionInfo.szServerName));

	//聊天规则
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_ON_GAME;

	//房间规则
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_LOOKON;

	//其他规则
	//OptionCurrent.dwRuleMask|=SR_RECORD_GAME_TRACK;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_RULE;
	OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_SIMULATE;

	//组件规则
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_DYNAMIC_JOIN;
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_ATTEND;
	//if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_OFFLINE_TRUSTEE;

	//模式规则
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_RECORD_GAME_SCORE;
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_IMMEDIATE_WRITE_SCORE;

	//发送数据
	SendData(pIServerUserItem,MDM_GR_MANAGE,SUB_GR_OPTION_CURRENT,&OptionCurrent,sizeof(OptionCurrent));

	return true;
}

//房间设置
bool CAttemperEngineSink::OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_ServerOption));
	if (wDataSize!=sizeof(CMD_GR_ServerOption)) return false;

	//变量定义
	CMD_GR_ServerOption * pServerOption=(CMD_GR_ServerOption *)pData;
	tagServerOptionInfo * pServerOptionInfo=&pServerOption->ServerOptionInfo;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//变量定义
	bool bModifyServer=false;

	////挂接节点
	//if (m_pGameServiceOption->wNodeID!=pServerOptionInfo->wNodeID)
	//{
	//	bModifyServer=true;
	//	m_pGameServiceOption->wNodeID=pServerOptionInfo->wNodeID;
	//}

	//挂接类型
	if ((pServerOptionInfo->wKindID!=0)&&(m_pGameServiceOption->wKindID!=pServerOptionInfo->wKindID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wKindID=pServerOptionInfo->wKindID;
	}
	
	//挂接排序
	if ((pServerOptionInfo->wSortID!=0)&&(m_pGameServiceOption->wSortID!=pServerOptionInfo->wSortID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wSortID=pServerOptionInfo->wSortID;
	}

	//房间名字
	if ((pServerOptionInfo->szServerName[0]!=0)&&(lstrcmp(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName)!=0))
	{
		bModifyServer=true;
		lstrcpyn(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName,CountArray(m_pGameServiceOption->szServerName));
	}

	//税收配置
	m_pGameServiceOption->wRevenueRatio=pServerOptionInfo->wRevenueRatio;
	m_pGameServiceOption->lServiceScore=pServerOptionInfo->lServiceScore;

	//房间配置
	m_pGameServiceOption->lRestrictScore=pServerOptionInfo->lRestrictScore;
	m_pGameServiceOption->lMinTableScore=pServerOptionInfo->lMinTableScore;
	m_pGameServiceOption->lMinEnterScore=pServerOptionInfo->lMinEnterScore;
	m_pGameServiceOption->lMaxEnterScore=pServerOptionInfo->lMaxEnterScore;

	//会员限制
	m_pGameServiceOption->cbMinEnterMember=pServerOptionInfo->cbMinEnterMember;
	m_pGameServiceOption->cbMaxEnterMember=pServerOptionInfo->cbMaxEnterMember;

	//聊天规则
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperOnGame(pServerOptionInfo->dwServerRule));

	//房间规则
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameLookon(pServerOptionInfo->dwServerRule));

	//其他规则
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameTrack(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameRule(pServerOptionInfo->dwServerRule));

	//动态加入
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowDynamicJoin(pServerOptionInfo->dwServerRule));
	}

	//机器管理
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowAndroidAttend(pServerOptionInfo->dwServerRule));
	}

	//断线托管
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowOffLineTrustee(pServerOptionInfo->dwServerRule));
	}

	//记录成绩
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameScore(pServerOptionInfo->dwServerRule));
	}

	//立即写分
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,CServerRule::IsImmediateWriteScore(pServerOptionInfo->dwServerRule));
	}

	//调整参数
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->RectifyServiceParameter();

	//发送修改
	if (bModifyServer==true)
	{
		//变量定义
		CMD_CS_C_ServerModify ServerModify;
		ZeroMemory(&ServerModify,sizeof(ServerModify));

		//服务端口
		ServerModify.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//房间信息
		ServerModify.wKindID=m_pGameServiceOption->wKindID;
		//ServerModify.wNodeID=m_pGameServiceOption->wNodeID;
		ServerModify.wSortID=m_pGameServiceOption->wSortID;
		ServerModify.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
		ServerModify.dwAndroidCount=m_AndroidUserManager.GetAndroidCount();
		ServerModify.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		lstrcpyn(ServerModify.szServerName,m_pGameServiceOption->szServerName,CountArray(ServerModify.szServerName));
		lstrcpyn(ServerModify.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(ServerModify.szServerAddr));

		//发送数据
		m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_MODIFY,&ServerModify,sizeof(ServerModify));
	}

	//发送信息
	SendRoomMessage(pIServerUserItem,TEXT("当前游戏服务器房间的“运行值”状态配置数据修改成功"),SMT_CHAT|SMT_EJECT);

	//输出信息
	TCHAR szBuffer[128]=TEXT("");
	_sntprintf_s(szBuffer, CountArray(szBuffer), TEXT("远程修改房间配置通知 管理员 %s [ %ld ]"), pIServerUserItem->GetNickName(), pIServerUserItem->GetUserID());

	//输出信息
	CTraceService::TraceString(szBuffer,TraceLevel_Info);

	return true;
}

//踢出用户
bool CAttemperEngineSink::OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//权限判断
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//目标用户
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户状态
	if(pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//踢出记录
	m_KickUserItemMap[pITargetUserItem->GetUserID()]=(DWORD)time(NULL);

	//请离桌子
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//发送消息
		SendGameMessage(pITargetUserItem,TEXT("你已被管理员请离桌子！"),SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	//发送通知
	LPCTSTR pszMessage=TEXT("你已被管理员请离此游戏房间！");
	SendRoomMessage(pITargetUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

	pITargetUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//限制聊天
bool CAttemperEngineSink::OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
	if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

	//消息处理
	CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//目标用户
	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
	if (pITargerUserItem==NULL) return true;

	//变量定义
	DWORD dwAddRight = 0, dwRemoveRight = 0;

	//大厅聊天
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
	{
		if (CMasterRight::CanLimitRoomChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_ROOM_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
	}

	//游戏聊天
	if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
	{
		if (CMasterRight::CanLimitGameChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_GAME_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_GAME_CHAT;
	}

	//大厅私聊
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
	{
		if (CMasterRight::CanLimitWisper(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_WISPER;
		else
			dwRemoveRight |= UR_CANNOT_WISPER;
	}

	//发送喇叭
	if(pLimitUserChat->cbLimitFlags==OSF_SEND_BUGLE)
	{
		if (CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==false) return false;

		if(pLimitUserChat->cbLimitValue == TRUE)
			dwAddRight |= UR_CANNOT_BUGLE;
		else
			dwRemoveRight |= UR_CANNOT_BUGLE;
	}

	if( dwAddRight != 0 || dwRemoveRight != 0 )
	{
		pITargerUserItem->ModifyUserRight(dwAddRight,dwRemoveRight);

		//发送通知
		CMD_GR_ConfigUserRight cur = {0};
		cur.dwUserRight = pITargerUserItem->GetUserRight();

		SendData( pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

		//发送消息
		SendRoomMessage(pIServerUserItem,TEXT("用户聊天权限配置成功！"),SMT_CHAT);
	}
	else return false;

	return true;
}

//踢出所有用户
bool CAttemperEngineSink::OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//消息处理
	CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;

	//效验数据
	ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
	if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
	ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
	if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//权限判断
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//解散所有游戏
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	//tagBindParameter *pBindParameter = m_pNormalParameter;
	//for( INT i = 0; i < m_pGameServiceOption->wMaxPlayer; i++ )
	//{
	//	//目录用户
	//	IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
	//	if (pITargerUserItem==NULL || pITargerUserItem==pIServerUserItem ) 
	//	{
	//		pBindParameter++;
	//		continue;
	//	}

	//	//发送消息
	//	SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

	//	pBindParameter++;
	//} 

	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	while(pITargerUserItem!=NULL)
	{
		if(pITargerUserItem!=pIServerUserItem)
		{
			//发送消息
			SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);
		}

		//枚举用户
		pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	}

	return true;
}

//发布消息
bool CAttemperEngineSink::OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//权限判断
	ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

	//变量定义
	CMD_GR_SendMessage *pSysMessage = (CMD_GR_SendMessage *)pData;

	if(pSysMessage->cbLoop==TRUE)
	{
		//定于变量
		tagSystemMessage  *pSendMessage=new tagSystemMessage;
		ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

		//构造消息
		pSendMessage->dwLastTime=(DWORD)time(NULL);
		pSendMessage->SystemMessage.dwMessageID=TEMP_MESSAGE_ID;
		pSendMessage->SystemMessage.cbAllRoom=pSysMessage->cbAllRoom;
		if(pSysMessage->cbRoom==TRUE && pSysMessage->cbGame==TRUE)
			pSendMessage->SystemMessage.cbMessageType=3;
		else if(pSysMessage->cbRoom==TRUE)
			pSendMessage->SystemMessage.cbMessageType=2;
		else if(pSysMessage->cbGame==TRUE)
			pSendMessage->SystemMessage.cbMessageType=1;
		pSendMessage->SystemMessage.dwTimeRate=pSysMessage->dwTimeRate;
		pSendMessage->SystemMessage.tStartTime=(DWORD)time(NULL);
		pSendMessage->SystemMessage.tConcludeTime=pSysMessage->tConcludeTime;
		lstrcpyn(pSendMessage->SystemMessage.szSystemMessage, pSysMessage->szSystemMessage, CountArray(pSendMessage->SystemMessage.szSystemMessage));

		//记录消息
		m_SystemMessageActive.Add(pSendMessage);
	}


	//消息处理
	return SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);
}

//解散游戏
bool CAttemperEngineSink::OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
	if (wDataSize!=sizeof(CMD_GR_DismissGame)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//权限判断
	ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

	//消息处理
	CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;

	//效验数据
	if(pDismissGame->wDismissTableNum >= m_TableFrameArray.GetCount()) return true;

	//解散游戏
	CTableFrame *pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
	if(pTableFrame)
	{
		if(pTableFrame->IsGameStarted()) pTableFrame->DismissGame();
		pTableFrame->SendGameMessage(TEXT("抱歉，此桌已被管理员解散游戏"),SMT_EJECT);
	}

	return true;
}

//警告命令
bool CAttemperEngineSink::OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_SendWarning));
	if (wDataSize!=sizeof(CMD_GR_SendWarning)) return false;

	//提取数据
	CMD_GR_SendWarning * pSendWarning = (CMD_GR_SendWarning*)pData;
	ASSERT(pSendWarning!=NULL);

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pSendWarning->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//权限判断
	ASSERT(CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==false) return false;

	//权限判断
	ASSERT(pIServerUserItem->GetMasterOrder()>pITargetUserItem->GetMasterOrder());
	if(pIServerUserItem->GetMasterOrder()<pITargetUserItem->GetMasterOrder()) return false;

	//发送警告
	TCHAR szMessage[128]=TEXT("");
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("警告：%s"), pSendWarning->szWarningMessage);
	SendRoomMessage(pITargetUserItem,szMessage,SMT_EJECT);

	return true;
}

//用户登录
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//获取参数
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bAndroidUser=pIServerUserItem->IsAndroidUser();
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//登录处理
	//变量定义
	CMD_GR_LogonSuccess LogonSuccess;
	ZeroMemory(&LogonSuccess, sizeof(LogonSuccess));

	//登录成功
	LogonSuccess.dwUserID = pIServerUserItem->GetUserID();
	LogonSuccess.dwUserRight = pIServerUserItem->GetUserRight();
	LogonSuccess.dwMasterRight = pIServerUserItem->GetMasterRight();
	SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_SUCCESS, &LogonSuccess, sizeof(LogonSuccess));

	////变量定义
	//CMD_GR_ConfigServer ConfigServer;
	//ZeroMemory(&ConfigServer,sizeof(ConfigServer));

	////房间配置
	//ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
	//ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
	//ConfigServer.wServerType=m_pGameServiceOption->wServerType;
	//ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
	//SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

	//配置完成
	//SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

	//发送房间配置
	CMD_GR_ConfigServer Config;
	ZeroMemory(&Config, sizeof(Config));
	CopyMemory(Config.szRoomName, m_pGameServiceOption->szServerName, sizeof(m_pGameServiceOption->szServerName));
	Config.wRoomID = m_pGameServiceOption->wServerID;
	Config.lCellScore = m_pGameServiceOption->lCellScore;
	Config.lDefPutScore = m_pGameServiceOption->lDefPutScore;
	Config.iExchangeRatio = m_pGameServiceOption->iExchangeRatio;
	Config.lMagicExpScore = m_pGameServiceOption->lMagicExpScore;
	Config.lGratuityScore = m_pGameServiceOption->lGratuityScore;
	SendData(pBindParameter->dwSocketID, MDM_GR_CONFIG, SUB_GR_CONFIG_SERVER, &Config, sizeof(Config));

	//群发给PC用户
	//SendUserInfoPacket(pIServerUserItem, INVALID_DWORD);

	// 发送给自己
	SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

	//登录完成
	SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_FINISH, NULL, 0);

	//立即登录
	if (pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//发送同一桌上的玩家给手机端
		WORD wEnumIndex = 0;
		while (wEnumIndex < m_ServerUserManager.GetUserItemCount())
		{
			//过滤用户
			IServerUserItem *pIUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIUserItem == NULL || pIUserItem == pIServerUserItem) continue;
			if (pIUserItem->GetTableID() != pIServerUserItem->GetTableID()) continue;

			// 发送给自己
			SendUserInfoPacket(pIUserItem, pBindParameter->dwSocketID);
		}
	}

	//欢迎消息
	if (bAndroidUser == false)
	{
		//构造提示
		TCHAR szMessage[128] = TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("欢迎您进入“%s”游戏房间，祝您游戏愉快！"), m_pGameServiceAttrib->szGameName);

		//发送消息
		SendRoomMessage(pIServerUserItem, szMessage, SMT_TABLE_ROLL);
	}

	//网络设置
	if (bAndroidUser==false)
	{
		if (pBindParameter->cbClientKind==CLIENT_KIND_MOBILE)
		{
			m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID,true,BG_MOBILE);
		}
		else
		{
			m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID,true,BG_COMPUTER);
		}
	}

	//登录通知
	if (m_pIMatchServiceManager != NULL)
	{
		m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);
	}

	return;
}

//用户离开
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//变量定义
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//提取成绩
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

	//用户信息
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//连接信息
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	LeaveGameServer.cbMaxCardType = pIServerUserItem->GetMaxCardData(LeaveGameServer.aryCardData, MAX_CARD_DATA_COUNT);

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);

	//推进任务
	tagUserTaskEntry * pUserTaskEntry = m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(), TASK_STATUS_UNFINISH | TASK_STATUS_FAILED);
	if(pUserTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem, pUserTaskEntry, LeaveGameServer.VariationInfo, TASK_TYPE_WIN_INNINGS | TASK_TYPE_SUM_INNINGS );
	}
	
	//汇总用户
	if (m_bCollectUser==true)
	{
		//变量定义
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//设置变量
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//发送消息
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//玩家退赛
	if(m_pIMatchServiceManager!=NULL) 
	{
		m_pIMatchServiceManager->OnEventUserQuitMatch(pIServerUserItem, 0);
	}

	//删除分组
	DeleteDistribute(pIServerUserItem);

	//移除任务
	m_UserTaskManager.RemoveUserTask(pIServerUserItem->GetUserID());

	//删除用户
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	//登出通知
	if (m_pIMatchServiceManager != NULL)
	{
		m_pIMatchServiceManager->OnEventUserLogout(pIServerUserItem);
	}

	return;
}

//解锁游戏币
bool CAttemperEngineSink::PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason)
{
	//变量定义
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//设置变量
	LeaveGameServer.dwUserID=dwUserID;
	LeaveGameServer.dwInoutIndex=dwInoutIndex;
	LeaveGameServer.dwLeaveReason=dwLeaveReason;

	//投递请求
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return true;
}

//推进任务
bool CAttemperEngineSink::PerformTaskProgress(IServerUserItem * pIServerUserItem, tagUserTaskEntry * pUserTaskEntry, tagVariationInfo &VariationInfo, WORD wTaskType)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL && pUserTaskEntry!=NULL);
	if(pIServerUserItem==NULL || pUserTaskEntry==NULL) return false;

	//状态校验
	if(pUserTaskEntry->pTaskParameter==NULL) return true;
	if(pUserTaskEntry->cbTaskStatus!=TASK_STATUS_UNFINISH) return true;
	if(pUserTaskEntry->pTaskParameter->wKindID!=m_pGameServiceOption->wKindID) return true;

	//任务参数
	tagTaskParameter * pTaskParameter = pUserTaskEntry->pTaskParameter;

	//获取时间
	DWORD dwSpanSecond=(DWORD)time(NULL)-pUserTaskEntry->dwLastUpdateTime;
	dwSpanSecond=__min(dwSpanSecond,pUserTaskEntry->dwResidueTime);

	//更新时间	
	pUserTaskEntry->dwResidueTime-=dwSpanSecond;
	pUserTaskEntry->dwLastUpdateTime=(DWORD)time(NULL);

	//任务超时
	if(pUserTaskEntry->dwResidueTime == 0)
	{		
		pUserTaskEntry->wTaskProgress=0;
		pUserTaskEntry->pTaskParameter=NULL;		
		pUserTaskEntry->cbTaskStatus=TASK_STATUS_FAILED;		
	}

	//任务未完成，并且是指定任务
	if (pUserTaskEntry->cbTaskStatus == TASK_STATUS_UNFINISH && (pTaskParameter->wTaskType&wTaskType)>0)
	{
		//上次任务进度
		WORD wLastTaskProgress = pUserTaskEntry->wTaskProgress;

		//任务类型
		switch(pTaskParameter->wTaskType)
		{
		case TASK_TYPE_WIN_INNINGS:			//赢局任务
			{
				//更新状态
				pUserTaskEntry->wTaskProgress += VariationInfo.dwWinCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}

				break;
			};
		case TASK_TYPE_SUM_INNINGS:			//总局任务
			{
				//更新状态
				pUserTaskEntry->wTaskProgress += VariationInfo.dwWinCount + VariationInfo.dwLostCount + VariationInfo.dwDrawCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}
				break;
			};
		case TASK_TYPE_MAGIC_EXP:			//魔法表情
			{
				pUserTaskEntry->wTaskProgress++;
				if (pUserTaskEntry->wTaskProgress >= pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus = TASK_STATUS_SUCCESS;
				}
				break;
			};
		case TASK_TYPE_RECHARGE:
			{
				break;
			};
		default:
			{
				ASSERT(FALSE);
				break;
			};
		}

		// 任务进度发生变化，更新数据库
		if (wLastTaskProgress != pUserTaskEntry->wTaskProgress)
		{
			//变量定义
			DBR_GR_TaskProgress TaskProgress;
			ZeroMemory(&TaskProgress, sizeof(TaskProgress));

			//设置变量
			TaskProgress.dwUserID = pIServerUserItem->GetUserID();
			TaskProgress.dwID = pUserTaskEntry->dwID;
			TaskProgress.wTaskProgress = pUserTaskEntry->wTaskProgress;
			TaskProgress.cbTaskStatus = pUserTaskEntry->cbTaskStatus;

			//投递请求
			m_pIDBCorrespondManager->PostDataBaseRequest(TaskProgress.dwUserID, DBR_GR_TASK_PROGRESS, 0L, &TaskProgress, sizeof(TaskProgress));
		}

		//任务完成
		if (pUserTaskEntry->cbTaskStatus == TASK_STATUS_SUCCESS)
		{
			//构造结构
			CMD_GR_S_TaskFinish TaskFinish;
			TaskFinish.wFinishTaskID = pTaskParameter->wTaskID;
			SendData(pIServerUserItem, MDM_GR_TASK, SUB_GR_TASK_FINISH, &TaskFinish, sizeof(TaskFinish));
		}
	}

	//递归调用
	if(pUserTaskEntry->pNextTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem, pUserTaskEntry->pNextStatusEntry, VariationInfo,wTaskType);
	}

	return true;
}

//切换连接
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], DWORD dwSocketID, BYTE cbDeviceType)
{
	WORD wTargetIndex = LOWORD(dwSocketID);
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD)) return false;

	//断开用户
	if (pIServerUserItem->IsOnline())
	{
		//发送通知
		LPCTSTR pszMessage=TEXT("请注意，您的帐号在另一地方进入了此游戏房间，您被迫离开！");
		SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

		//绑定参数
		WORD wSourceIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pSourceParameter=GetBindParameter(wSourceIndex);

		//解除绑定
		ASSERT((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem));
		if ((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem)) pSourceParameter->pIServerUserItem=NULL;

		//断开用户
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID,true);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//状态切换
	bool bIsOffLine=false;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
	{
		//变量定义
		WORD wTableID=pIServerUserItem->GetTableID();
		WORD wChairID=pIServerUserItem->GetChairID();

		//设置状态
		bIsOffLine=true;
		//私人类型;
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
		{
			BYTE cbReUserStatus = pIServerUserItem->GetReUserStatus();
			pIServerUserItem->SetUserStatus(cbReUserStatus, wTableID, wChairID);
		}
		else
		{
			pIServerUserItem->SetUserStatus(US_PLAYING, wTableID, wChairID);
		}
	}

	//机器判断
	LPCTSTR pszMachineID=pIServerUserItem->GetMachineID();
	bool bSameMachineID=(lstrcmp(pszMachineID,szMachineID)==0);

	//变量定义
	bool bAndroidUser=(wTargetIndex>=INDEX_ANDROID);
	tagBindParameter * pTargetParameter=GetBindParameter(wTargetIndex);

	//激活用户
	pTargetParameter->pIServerUserItem=pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr,wTargetIndex,szMachineID,bAndroidUser,false);

	//取消托管
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		if (pIServerUserItem->IsTrusteeUser()==true && pIServerUserItem->GetUserStatus()!=US_PLAYING)
		{
			pIServerUserItem->SetTrusteeUser(false);
		}
	}

	//重新加载断线玩家信息
	if (bAndroidUser == false)
	{
		//加载道具
		DBR_GR_RELINE_USERID RelineUserID;
		ZeroMemory(&RelineUserID, sizeof(RelineUserID));
		RelineUserID.dwUserID = pIServerUserItem->GetUserID();
		m_pIDBCorrespondManager->PostDataBaseRequest(RelineUserID.dwUserID, DBR_GR_LOGON_RELINE, 0, &RelineUserID, sizeof(RelineUserID));
	}

	//断线重连登录事件
	OnEventUserLogon(pIServerUserItem, true);

	//安全提示
	if ((bAndroidUser == false) && (bIsOffLine == false) && (bSameMachineID == false))
	{
		SendRoomMessage(pIServerUserItem, TEXT("请注意，您的帐号在另一地方进入了此游戏房间，对方被迫离开！"), SMT_EJECT | SMT_CHAT | SMT_GLOBAL);
	}

	return true;
}

//登录失败
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//变量定义
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//构造数据
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString, CYSCharUtils::ConvertToUTF8(pszString), CountArray(LogonFailure.szDescribeString));

	//数据属性
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//发送数据
	SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//银行失败
bool CAttemperEngineSink::SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszString, LONG lErrorCode,BYTE cbActivityGame)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_S_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//构造数据
	UserInsureFailure.cbActivityGame=cbActivityGame;
	UserInsureFailure.lErrorCode=lErrorCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pszString,CountArray(UserInsureFailure.szDescribeString));

	//数据属性
	WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);

	//发送数据
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDataSize);

	return true;
}

//请求失败
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//设置变量
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//发送数据
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//道具失败
bool CAttemperEngineSink::SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea)
{
	////变量定义
	//CMD_GR_PropertyFailure PropertyFailure;
	//ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

	////设置变量
	//PropertyFailure.lErrorCode=lErrorCode;
	//PropertyFailure.wRequestArea=wRequestArea;
	//lstrcpyn(PropertyFailure.szDescribeString,pszDescribe,CountArray(PropertyFailure.szDescribeString));

	////发送数据
	//WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
	//WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
	//SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_PROPERTY_FAILURE,&PropertyFailure,wHeadSize+wDataSize);

	return true;
}

//发送用户
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID,bool bSendAndroidFalg)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	BYTE cbBuffer[SOCKET_TCP_PACKET] = {0};
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));

	//用户属性
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	//pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwCustomFaceVer = pUserInfo->dwCustomFaceVer;

	//用户属性	
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//用户状态
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	////用户局数
	//pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	//pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	//pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	//pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	//pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	//pUserInfoHead->lLoveLiness=pUserInfo->lLoveLiness;

	////用户积分
	//pUserInfoHead->lDiamond = pUserInfo->lDiamond;
	//pUserInfoHead->lInsure=pUserInfo->lInsure;

	//用户成绩
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lHoldScore = pUserInfo->lScore + pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lHoldScore += pIServerUserItem->GetFrozenedScore();

	//昵称判断
	ASSERT(pUserInfo->szNickName != NULL);
	if (pUserInfo->szNickName[0] == 0)
	{
		_sntprintf_s(pUserInfo->szNickName, CountArray(pUserInfo->szNickName), TEXT("%d"), pUserInfo->dwUserID);
	}

	//叠加信息
	SendPacket.AddPacket(CYSCharUtils::ConvertToUTF8(pUserInfo->szNickName), DTP_GR_NICK_NAME);
	//SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);

	//发送数据
	if (dwSocketID==INVALID_DWORD)
	{
		//WORD wHeadSize=sizeof(tagUserInfoHead);
		//SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}
	else
	{
		WORD wHeadSize=sizeof(tagUserInfoHead);
		SendData(dwSocketID,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wHeadSize+SendPacket.GetDataSize());
	}

	return true;
}

////广播道具
//bool CAttemperEngineSink::SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount)
//{
//	//构造结构
//	CMD_GR_S_PropertyMessage  PropertyMessage;
//	PropertyMessage.wPropertyIndex=wPropertyIndex;
//	PropertyMessage.dwSourceUserID=dwSourceID;
//	PropertyMessage.dwTargerUserID=dwTargerID;
//	PropertyMessage.wPropertyCount=wPropertyCount;
//
//	//在线用户
//	WORD wUserIndex=0;
//	IServerUserItem * pIServerUserItemSend=NULL;
//	while (true)
//	{
//		pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
//		if (pIServerUserItemSend==NULL) break;
//		SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_PROPERTY_MESSAGE,&PropertyMessage,sizeof(PropertyMessage));
//	}
//
//	return true;
//}
//
////道具效应
//bool CAttemperEngineSink::SendPropertyEffect(IServerUserItem * pIServerUserItem)
//{
//	//参数校验
//	if(pIServerUserItem==NULL) return false;
//
//	//构造结构
//	CMD_GR_S_PropertyEffect  PropertyEffect;
//	PropertyEffect.wUserID =pIServerUserItem->GetUserID();
//	PropertyEffect.cbMemberOrder=pIServerUserItem->GetMemberOrder();
//
//	//在线用户
//	WORD wUserIndex=0;
//	IServerUserItem * pIServerUserItemSend=NULL;
//	while (true)
//	{
//		pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wUserIndex++);
//		if (pIServerUserItemSend==NULL) break;
//		SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_PROPERTY_EFFECT,&PropertyEffect,sizeof(PropertyEffect));
//	}
//
//	return true;
//}
//
////道具事件
//bool CAttemperEngineSink::OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem)
//{
//	//目标玩家
//	if ( pTargetUserItem == NULL )
//	{
//		//发送消息
//		SendPropertyFailure(pISourceUserItem,TEXT("赠送失败，您要赠送的玩家已经离开！"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//房间判断
//	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH ) )
//	{
//		//发送消息
//		SendPropertyFailure(pISourceUserItem,TEXT("此房间不可以使用此道具,购买失败"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//查找道具
//	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(wPropertyIndex);
//
//	//有效效验
//	if(pPropertyInfo==NULL)
//	{
//		//发送消息
//		SendPropertyFailure(pISourceUserItem,TEXT("此道具还未启用,购买失败！"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//自己使用
//    if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_MESELF)==0 && pISourceUserItem==pTargetUserItem) 
//	{
//		//发送消息
//		SendPropertyFailure(pISourceUserItem,TEXT("此道具不可自己使用,购买失败！"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//玩家使用
//	if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_PLAYER)==0 && pISourceUserItem!=pTargetUserItem) 
//	{
//		//发送消息
//		SendPropertyFailure(pISourceUserItem,TEXT("此道具不可赠送给玩家,只能自己使用,购买失败！"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//旁观范围
//	if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_LOOKON)==0)  
//	{
//		//变量定义
//		WORD wTableID = pTargetUserItem->GetTableID();
//		if(wTableID!=INVALID_TABLE)
//		{
//			//变量定义
//			WORD wEnumIndex=0;
//			IServerUserItem * pIServerUserItem=NULL;
//
//			//获取桌子
//			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
//	
//			//枚举用户
//			do
//			{
//				//获取用户
//				pIServerUserItem=pTableFrame->EnumLookonUserItem(wEnumIndex++);
//				if( pIServerUserItem==NULL) break;
//				if( pIServerUserItem==pTargetUserItem )
//				{
//					//发送消息
//					SendPropertyFailure(pISourceUserItem,TEXT("此道具不可赠送给旁观用户,购买失败！"), 0L,cbRequestArea);
//
//					return false;
//				}
//			} while (true);
//		}
//	}
//
//	return true;
//}

//绑定用户
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//获取参数
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//获取用户
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//机器连接
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//道具类型
WORD CAttemperEngineSink::GetPropertyType(WORD wPropertyIndex)
{
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_VIP1_CARD:
	case PROPERTY_ID_VIP2_CARD:
	case PROPERTY_ID_VIP3_CARD: 
		{
			return PT_TYPE_PROPERTY;
		};
	}

	ASSERT(false);

	return PT_TYPE_ERROR;
}

//分配用户
bool CAttemperEngineSink::PerformDistribute()
{
	//人数校验
	if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinDistributeUser) return false;

	//分配用户
	while(true)
	{
		//人数校验
		if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinPartakeGameUser) break;

		//变量定义
		CDistributeInfoArray DistributeInfoArray;

        //获取用户
		WORD wRandCount = __max(m_pGameServiceOption->wMaxPartakeGameUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pGameServiceOption->wMinPartakeGameUser+rand()%wRandCount;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if(wDistributeCount < m_pGameServiceOption->wMinPartakeGameUser) break;

		//寻找位置
		CTableFrame * pCurrTableFrame=NULL;
		for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
		{
			//获取对象
			ASSERT(m_TableFrameArray[i]!=NULL);
			CTableFrame * pTableFrame=m_TableFrameArray[i];

			//状态判断
			if (pTableFrame->GetSitUserCount()==0)
			{
				pCurrTableFrame=pTableFrame;
				break;
			}
		}

		//桌子判断
		if(pCurrTableFrame==NULL) break;

		//玩家坐下
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//变量定义
			WORD wChairID=pCurrTableFrame->GetNullChairID();

			//分配用户
			if (wChairID!=INVALID_CHAIR)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//用户坐下
				if(pCurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//坐下结果
		if(bSitSuccess)
		{
			//移除结点
			while(DistributeInfoArray.GetCount()>0)
			{
				m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[0].pPertainNode);
				DistributeInfoArray.RemoveAt(0);
			}
		}
		else
		{
			for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
			{
				if(nSitFailedIndex==nIndex)
				{
					m_DistributeManage.RemoveDistributeNode((tagDistributeNode *)DistributeInfoArray[nIndex].pPertainNode);
				}
				else
				{
					//用户起立
					if (DistributeInfoArray[nIndex].pIServerUserItem->GetTableID()!=INVALID_TABLE)
					{
						WORD wTableID=DistributeInfoArray[nIndex].pIServerUserItem->GetTableID();
						m_TableFrameArray[wTableID]->PerformStandUpAction(DistributeInfoArray[nIndex].pIServerUserItem);
					}
				}
			}
		}		
	}

	return true;
}

//配置机器
bool CAttemperEngineSink::InitAndroidUser()
{
	//机器参数
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//配置参数
	AndroidUserParameter.bServiceContinue=true;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;

	//服务组件
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//服务配置
	if(m_pGameServiceAttrib->wChairCount>=MAX_CHAIR)
	{
		AndroidUserParameter.bServiceContinue=false;
		AndroidUserParameter.dwMinSitInterval=MIN_INTERVAL_TIME;
		AndroidUserParameter.dwMaxSitInterval=MAX_INTERVAL_TIME;	
	}

	//设置对象
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//配置桌子
bool CAttemperEngineSink::InitTableFrameArray()
{
	//桌子参数
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//内核组件
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//服务组件
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;
	TableFrameParameter.pBenefitInfo = &m_pInitParameter->m_BenefitInfo;

	//配置参数
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	//桌子容器
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(),m_pGameServiceOption->wTableCount*sizeof(CTableFrame *));

	//创建桌子
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//创建对象
		m_TableFrameArray[i]=new CTableFrame;

		//配置桌子
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		//绑定桌子
		if(m_pIMatchServiceManager!=NULL)
		{
			m_pIMatchServiceManager->BindTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
		}
	}	

	return true;
}

//配置比赛
bool CAttemperEngineSink::InitMatchServiceManager()
{
	//参数校验
	if(m_pIMatchServiceManager==NULL) return true;

	//桌子参数
	tagMatchManagerParameter MatchManagerParameter;
	ZeroMemory(&MatchManagerParameter,sizeof(MatchManagerParameter));

	//配置参数
	MatchManagerParameter.pGameMatchOption=m_pGameMatchOption;
	MatchManagerParameter.pGameServiceOption=m_pGameServiceOption;
	MatchManagerParameter.pGameServiceAttrib=m_pGameServiceAttrib;

	//内核组件
	MatchManagerParameter.pITimerEngine=m_pITimerEngine;
	MatchManagerParameter.pICorrespondManager=m_pIDBCorrespondManager;
	MatchManagerParameter.pTCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//服务组件
	MatchManagerParameter.pIMainServiceFrame=this;
	MatchManagerParameter.pIServerUserItemSink=this;
	MatchManagerParameter.pIAndroidUserManager=&m_AndroidUserManager;
	MatchManagerParameter.pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);		

	//初始化接口
	m_pIMatchServiceManager->InitMatchInterface(MatchManagerParameter);

	return true;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//插入分配
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//状态判断
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//比赛类型
	if(m_pIMatchServiceManager!=NULL) 
	{
		//获取参数
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//加入比赛
		return m_pIMatchServiceManager->OnEventUserJoinMatch(pIServerUserItem,0,pBindParameter->dwSocketID);
	}

	//变量定义
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//设置变量
	DistributeInfo.pIServerUserItem=pIServerUserItem;
	DistributeInfo.wLastTableID=pIServerUserItem->GetTableID();
	DistributeInfo.pPertainNode=NULL;

	//加入数组
	if(m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	//用户起立
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		m_TableFrameArray[wTableID]->PerformStandUpAction(pIServerUserItem);
	}

	//发送通知
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	//分配判断
	bool bDistribute=false;
	if (m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_IMMEDIATE) bDistribute=true;

	//执行分组
	if (bDistribute==true)
	{
		PerformDistribute();
	}

	return true;
}

//敏感词过滤
void CAttemperEngineSink::SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)
{
	m_WordsFilter.Filtrate(pMsg,pszFiltered,nMaxLen);
}

//删除用户
bool CAttemperEngineSink::DeleteDistribute(IServerUserItem * pIServerUserItem)
{
	//移除节点
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}

//立即登录
bool CAttemperEngineSink::UserImmediately(IServerUserItem * pIServerUserItem)
{
	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	bool bSitFail = false;

	//查找桌子（有玩家的桌子）
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//定义变量
		INT nTableIndex=(i+nStartIndex)%m_pGameServiceOption->wTableCount;

		//获取桌子
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//用户坐下
		if (pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem) == false)
		{
			bSitFail = true;
			break;
		}

		return true;
	}

	if (!bSitFail)
	{
		//查找桌子（无玩家的桌子）
		nStartIndex = rand() % m_pGameServiceOption->wTableCount;
		for (INT_PTR i = 0; i < (m_pGameServiceOption->wTableCount); i++)
		{
			//定义变量
			INT nTableIndex = (i + nStartIndex) % m_pGameServiceOption->wTableCount;

			//获取桌子
			CTableFrame * pTableFrame = m_TableFrameArray[nTableIndex];
			if ((pTableFrame->IsGameStarted() == true) && (bDynamicJoin == false)) continue;
			if (pTableFrame->IsTableLocked()) continue;

			//无效过滤
			WORD wChairID = pTableFrame->GetRandNullChairID();
			if (wChairID == INVALID_CHAIR) continue;

			//用户坐下
			if (pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem) == false)
			{
				break;
			}

			return true;
		}
	}

	// 找不到桌子坐下，安排旁观,发送桌子上其他玩家消息给客户端，通知客户端请求场景消息
	CTableFrame * pTableFrame = m_TableFrameArray[nStartIndex];
	INT nChairId = rand() % pTableFrame->GetChairCount();
	pTableFrame->PerformLookonAction(nChairId, pIServerUserItem);

	// 玩家坐下，如果是手机玩家，将桌子上所有玩家发送给自己
	if ( m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	{
		for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
		{
			//获取用户
			IServerUserItem *pIUserItem = pTableFrame->GetTableUserItem(i);
			if (pIUserItem == NULL) continue;
			if (pIUserItem->GetUserID() == pIServerUserItem->GetUserID()) continue;

			SendUserInfo(pIUserItem, pIServerUserItem->GetBindIndex());
		}
	}

	// 通知客户端请求场景消息
	tagBindParameter * pBindParameter = GetBindParameter(pIServerUserItem->GetBindIndex());
	if (pBindParameter == nullptr)
	{
		return false;
	}

	SendData(pBindParameter->dwSocketID, MDM_GF_FRAME, SUB_GF_GAME_OPTION_TO_USER, NULL, 0);

	//失败
	//m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("没找到可进入的游戏桌！"),REQUEST_FAILURE_NORMAL);
	//m_TableFrameArray[0]->SendMobileReqFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_TABLE);
	return true;
}

//发送当前桌子的用户给手机登录
bool CAttemperEngineSink::SendUserInfo(IServerUserItem * pIServerUserItem, WORD wBindIndex)
{
	if (pIServerUserItem == nullptr || pIServerUserItem->GetTableID() == INVALID_TABLE)
	{
		return false;
	}
	
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
	if (pBindParameter == nullptr)
	{
		return false;
	}

	// 别的玩家发送给自己
	SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

	return true;
}

//发送系统消息
bool CAttemperEngineSink::SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize)
{
	//消息处理
	ASSERT(pSendMessage!=NULL);

	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength);
	if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength) 
		return false;

	//所有房间
	if(pSendMessage->cbAllRoom == TRUE)
	{
		pSendMessage->cbAllRoom=FALSE;
		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_SYSTEM_MESSAGE,pSendMessage,wDataSize);
	}
	else
	{
		//发送系统消息
		if(pSendMessage->cbGame == TRUE)
			SendGameMessage(pSendMessage->szSystemMessage,SMT_CHAT);
		if(pSendMessage->cbRoom == TRUE)
			SendRoomMessage(pSendMessage->szSystemMessage,SMT_CHAT);
	}

	return true;
}

//移除消息
void CAttemperEngineSink::RemoveSystemMessage()
{
	//缓存消息
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	m_SystemMessageActive.RemoveAll();
}

void CAttemperEngineSink::RegServerInfoInRedis()
{
#ifdef USE_REDIS
	//保存比赛信息到缓存服务器
	CStringA strHost = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerAddr);
	CStringA strCachePASS = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerPass);
	//创建缓存连接
	CRedisInstance	mRedis;
	bool bRet = mRedis.connect(strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, strCachePASS.GetBuffer(), 500000);
	if (bRet)
	{
		char szBuf[32] = { 0 };

		if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
		{
			sprintf_s(szBuf, sizeof(szBuf), "privateserverid.%d", m_pGameServiceOption->wServerID);
			string strKey(szBuf);

			string strServerID(CYSService::ToString(m_pGameServiceOption->wServerID));

			ValueMap vMap;
			vMap["ID"]			= strServerID;
			vMap["Name"]		= CYSCharUtils::ConvertToUTF8(m_pGameServiceOption->szServerName).GetBuffer();
			vMap["KindID"]		= CYSService::ToString(m_pGameServiceOption->wKindID);
			vMap["wMaxTable"]	= CYSService::ToString(m_pGameServiceOption->wTableCount);
			vMap["wCurTable"]	= "0";
			vMap["ServerIP"]	= CYSCharUtils::ConvertToUTF8(m_pInitParameter->m_ServiceAddress.szAddress).GetBuffer();
			WORD wPort			= CServiceUnits::g_pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();
			vMap["Port"]		= CYSService::ToString(wPort);

			int64_t retval = 0;
			//设置缓存,先存入列表，再存入HASH表
			ValueList ls;
			mRedis.lrange("privatelist", 0, -1, ls);
			list<string>::iterator it = find(ls.begin(), ls.end(), strKey);
			if (it == ls.end())
			{
				mRedis.lpush("privatelist", strKey, retval);
			}

			mRedis.hmset(strKey, vMap, retval);

			//先设置游戏服务器列表;
			string strKindID(CYSService::ToString(m_pGameServiceOption->wKindID));

			ValueList serverList;
			mRedis.lrange("serverlist", 0, -1, serverList);
			list<string>::iterator itFind = find(serverList.begin(), serverList.end(), strKindID);
			if (itFind == serverList.end())
			{
				mRedis.lpush("serverlist", strKindID, retval);
			}

			//再设置kindID下的SERVERID;
			memset(szBuf, 0, sizeof(szBuf));
			sprintf_s(szBuf, sizeof(szBuf), "kind%d.%d", m_pGameServiceOption->wKindID, m_pGameServiceOption->wServerType);
			string strKindServerKey(szBuf);

			ValueList kindServerList;
			mRedis.lrange(strKindServerKey, 0, -1, kindServerList);
			itFind = find(kindServerList.begin(), kindServerList.end(), strServerID);
			if (itFind == kindServerList.end())
			{
				mRedis.lpush(strKindServerKey, strServerID, retval);
			}

		}
		else if (m_pGameServiceOption->wServerType == GAME_GENRE_MATCH)
		{
			sprintf_s(szBuf, sizeof(szBuf), "matchserverid.%d", m_pGameMatchOption->dwMatchID);
			string strKey(szBuf);

			ValueMap vMap;
			vMap["ID"] = CYSService::ToString(m_pGameMatchOption->dwMatchID);
			vMap["Name"] = CYSCharUtils::ConvertToUTF8(m_pGameMatchOption->szMatchName).GetBuffer();
			vMap["AwardContent"] = CYSCharUtils::ConvertToUTF8(m_pGameMatchOption->szMatchAwardContent).GetBuffer();
			vMap["Content"] = CYSCharUtils::ConvertToUTF8(m_pGameMatchOption->szMatchContent).GetBuffer();
			vMap["ServerIP"] = CYSCharUtils::ConvertToUTF8(m_pInitParameter->m_ServiceAddress.szAddress).GetBuffer();
			WORD wPort = CServiceUnits::g_pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();
			vMap["Port"] = CYSService::ToString(wPort);

			int64_t retval = 0;
			//设置缓存,先存入列表，再存入HASH表
			ValueList ls;
			mRedis.lrange("matchlist", 0, -1, ls);
			list<string>::iterator it = find(ls.begin(), ls.end(), strKey);
			if (it == ls.end())
			{
				mRedis.lpush("matchlist", strKey, retval);
			}

			mRedis.hmset(strKey, vMap, retval);
		}
		else
		{
			sprintf_s(szBuf, sizeof(szBuf), "roomserverid.%d", m_pGameServiceOption->wServerID);
			string strKey(szBuf);

			ValueMap vMap;
			vMap["ID"] = CYSService::ToString(m_pGameServiceOption->wServerID);
			vMap["Name"] = CYSCharUtils::ConvertToUTF8(m_pGameServiceOption->szServerName).GetBuffer();
			vMap["MinCellScore"] = CYSService::ToString(m_pGameServiceOption->lCellScore / 2);
			vMap["MaxCellScore"] = CYSService::ToString(m_pGameServiceOption->lCellScore);
			vMap["MinTakeScore"] = CYSService::ToString(m_pGameServiceOption->lMinEnterScore);
			vMap["MaxTakeScore"] = CYSService::ToString(m_pGameServiceOption->lMaxEnterScore);
			vMap["OnlineUserNum"] = "0";
			vMap["ServerIP"] = CYSCharUtils::ConvertToUTF8(m_pInitParameter->m_ServiceAddress.szAddress).GetBuffer();
			WORD wPort = CServiceUnits::g_pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();
			vMap["Port"] = CYSService::ToString(wPort);

			int64_t retval = 0;
			//设置缓存,先存入列表，再存入HASH表
			ValueList ls;
			mRedis.lrange("roomlist", 0, -1, ls);
			list<string>::iterator it = find(ls.begin(), ls.end(), strKey);
			if (it == ls.end())
			{
				mRedis.lpush("roomlist", strKey, retval);
			}

			mRedis.hmset(strKey, vMap, retval);
		}
	}
#endif
}

//加载牌库
bool CAttemperEngineSink::OnDBGameCardLibrary(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//开始加载
	if (dwContextID == 0xfffe)
	{
		CTableFrame::ClearGameCardLibrary();
		return true;
	}

	//加载完成
	if (dwContextID == 0xffff)
	{
		CServerUserItem::g_dwCardLibCount = CTableFrame::m_GameCardLibraryBuffer.GetCount();
		return true;
	}

	DBO_GR_GameCardLibrary* pGameCard = (DBO_GR_GameCardLibrary*)pData;

	tagGameCardLibrary* pNewData = new tagGameCardLibrary();
	ZeroMemory(pNewData, sizeof(tagGameCardLibrary));

	CString strSub;
	int nCount = 0;
	while (AfxExtractSubString(strSub, pGameCard->szCardData, nCount, ','))
	{
		ASSERT(nCount <= LEN_CARD_LIB);
		if (nCount > pGameCard->cbCardCount&&nCount > LEN_CARD_LIB)
		{
			break;
		}
		TCHAR* lpStrEnd = NULL;
		pNewData->cbData[nCount] = (BYTE)_tcstol(strSub.GetBuffer(), &lpStrEnd, 16);
		nCount++;
	}

	if (nCount == pGameCard->cbCardCount)
	{
		pNewData->dwLibID = pGameCard->dwLibID;
		pNewData->dwCustomID = pGameCard->dwCustomID;
		pNewData->cbCount = pGameCard->cbCardCount;

		CTableFrame::AddGameCardLibrary(pNewData);
	}
	else
	{
		SafeDelete(pNewData);
	}

	return true;
}

//牌库配置;
bool CAttemperEngineSink::OnDBGameCardLibraryCfg(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBO_GR_GameCardLibraryCfg));
	if (wDataSize != sizeof(DBO_GR_GameCardLibraryCfg)) return false;

	DBO_GR_GameCardLibraryCfg* pCardLibCfg = (DBO_GR_GameCardLibraryCfg*)pData;

	CServerUserItem::g_dwCardLibCustomCount = pCardLibCfg->dwCount;
	CServerUserItem::g_cbCardLibEnable = pCardLibCfg->cbEnable;
	CServerUserItem::g_dwLibStartDateTime = pCardLibCfg->dwStartDateTime;
	CServerUserItem::g_dwLibEndDateTime = pCardLibCfg->dwEndDateTime;

	return true;
}

void CAttemperEngineSink::ShowBurnInfo(void)
{
	WORD wOnlineCount = m_ServerUserManager.GetUserItemCount();
	WORD wAndroidCount = m_ServerUserManager.GetAndroidCount();
	int iGameStartCount = 0;
	int iGameUserCount = 0;
	int iGameAndroidCount = 0;
	for (WORD i = 0, j = 0; i < m_pGameServiceOption->wTableCount; i++)
	{
		//获取对象;
		ASSERT(m_TableFrameArray[i] != NULL);
		CTableFrame * pTableFrame = m_TableFrameArray[i];
		if ( NULL == pTableFrame )
		{	//走到这个地方,肯定出错了;
			continue;
		}
		
		if (pTableFrame->IsGameStarted())
		{
			//桌子状况;
			tagTableUserInfo tableUserInfo;
			WORD wUserSitCount = pTableFrame->GetTableUserInfo(tableUserInfo);

			iGameStartCount++;
			iGameAndroidCount	+= tableUserInfo.wTableAndroidCount;
			iGameUserCount		+= tableUserInfo.wTableUserCount;
		}
	}
	CString str;
	str.Format(_T("当前在线%u人,其中机器人%u个\n当前游戏%d桌,玩家%d人,机器人%d个."), wOnlineCount, wAndroidCount, iGameStartCount, iGameUserCount, iGameAndroidCount);

	if ( m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_IMMEDIATE )
	{
		WORD wQueueUser = m_DistributeManage.GetRealCount();
		WORD wQueueAndroid = m_DistributeManage.GetAndroidCount();
		str.AppendFormat(_T("\n当前队列中:玩家%u人,机器人%u人"), wQueueUser, wQueueAndroid);
	}
	CTraceService::TraceString(str, TraceLevel_Info);
}

//////////////////////////////////////////////////////////////////////////////////
