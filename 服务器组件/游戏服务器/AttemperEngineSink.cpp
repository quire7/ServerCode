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
//ʱ���ʶ

#define IDI_LOAD_ANDROID_USER		(IDI_MAIN_MODULE_START+1)			//������Ϣ
#define IDI_REPORT_SERVER_INFO		(IDI_MAIN_MODULE_START+2)			//������Ϣ
#define IDI_CONNECT_CORRESPOND		(IDI_MAIN_MODULE_START+3)			//����ʱ��
#define IDI_GAME_SERVICE_PULSE		(IDI_MAIN_MODULE_START+4)			//��������
#define IDI_DISTRIBUTE_ANDROID		(IDI_MAIN_MODULE_START+5)			//�������
#define IDI_DBCORRESPOND_NOTIFY		(IDI_MAIN_MODULE_START+6)			//����֪ͨ
#define IDI_LOAD_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+7)			//ϵͳ��Ϣ
#define IDI_SEND_SYSTEM_MESSAGE		(IDI_MAIN_MODULE_START+8)			//ϵͳ��Ϣ
#define IDI_LOAD_SENSITIVE_WORD		(IDI_MAIN_MODULE_START+9)			//�������д�
#define IDI_DISTRIBUTE_USER		    (IDI_MAIN_MODULE_START+10)			//�����û�
#define IDI_REFRESH_ONLINE_USER		(IDI_MAIN_MODULE_START+11)			//ˢ���������

//////////////////////////////////////////////////////////////////////////////////
//ʱ�䶨�� ��

#define TIME_LOAD_ANDROID_USER		15L									//���ػ���
#define TIME_DISTRIBUTE_ANDROID		15L									//�����û�
#define TIME_REPORT_SERVER_INFO		30L									//�ϱ�ʱ��
#define TIME_DBCORRESPOND_NOTIFY	3L									//����֪ͨʱ��
#define TIME_LOAD_SYSTEM_MESSAGE	600L								//ϵͳ��Ϣʱ��
#define TIME_SEND_SYSTEM_MESSAGE	10L								    //ϵͳ��Ϣʱ��
#define TIME_LOAD_SENSITIVE_WORD	5L									//�������д�ʱ��
//////////////////////////////////////////////////////////////////////////////////
//��������
#define TEMP_MESSAGE_ID				INVALID_DWORD			


//���ʱ��
#define MIN_INTERVAL_TIME			10									//���ʱ��
#define MAX_INTERVAL_TIME			1*60								//��ʱ��ʶ

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//���Ʊ���
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//������
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//��Ա����
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//�������
	m_wTaskCount=0;
	ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

	//״̬����
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	//��������
	ZeroMemory(&m_DataConfigProperty,sizeof(m_DataConfigProperty));

	//��������
	m_pIMatchServiceManager=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//ɾ����Ϣ
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	for (INT_PTR i=0;i<m_SystemMessageBuffer.GetCount();i++)
	{
		SafeDelete(m_SystemMessageBuffer[i]);
	}

	//�������
	m_SystemMessageActive.RemoveAll();
	m_SystemMessageBuffer.RemoveAll();

	//��������	
	m_KickUserItemMap.RemoveAll();
	m_DistributeManage.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//����Ϣ
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*MAX_ANDROID);

	//����Ϣ
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//���ñ���
	InitMatchServiceManager();

	//���û���
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//���ýӿ�
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

	//��������
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ù���
	m_DistributeManage.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);

	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,15*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,5*1000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_SYSTEM_MESSAGE,TIME_LOAD_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,TIME_SEND_SYSTEM_MESSAGE*1000L,TIMES_INFINITY,NULL);
#endif

	//��ʱ�������д�
	m_pITimerEngine->SetTimer(IDI_LOAD_SENSITIVE_WORD,TIME_LOAD_SENSITIVE_WORD*1000L,TIMES_INFINITY,NULL);

	//������ģʽ
	if ((m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_ALLOW)!=0)
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000,TIMES_INFINITY,NULL);
	}

	//���ػ���
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

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
#ifdef USE_REDIS
	CStringA strHost = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerAddr);

	//������������
	CRedisInstance	mRedis;
	bool bRet = mRedis.connect(strHost.GetBuffer(), m_pGameServiceOption->wCacheServerPort, "", 500000);
	if (bRet)
	{
		stringstream ss;
		int64_t ret = 0;
		if (m_pGameServiceOption->wServerType == GAME_GENRE_PRIVATE)
		{
			//ɾ��kindID�µ�SERVERID;
			char szBuf[32] = {0};
			sprintf_s(szBuf, sizeof(szBuf), "kind%d.%d", m_pGameServiceOption->wKindID, m_pGameServiceOption->wServerType);
			string strKindServerKey(szBuf);
			
			mRedis.lrem(strKindServerKey, CYSService::ToString(m_pGameServiceOption->wServerID), ret);

			uint64_t nLen = 0;
			mRedis.llen(strKindServerKey, nLen);
			if ( nLen == 0 )
			{	//ɾ���������б��е�KINDID;
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

	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;

	//������Ϣ
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//�������
	m_wTaskCount=0;
	ZeroMemory(&m_TaskParameter,sizeof(m_TaskParameter));

	//��Ա����
	m_wMemberCount=0;
	ZeroMemory(m_MemberParameter,sizeof(m_MemberParameter));

	//��ֹͣ�����ֹͣ����,�ٹر�����(modify by fxd);
	//ֹͣ����;
	m_AndroidUserManager.ConcludeService();

	//ֹͣ����;
	if (m_pIMatchServiceManager != NULL)
		m_pIMatchServiceManager->StopService();

	//ɾ������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//ɾ���û�
	m_TableFrameArray.RemoveAll();
	m_DistributeManage.RemoveAll();
	m_ServerUserManager.DeleteUserItem();
	m_ServerListManager.ResetServerList();
	
	////ֹͣ����;
	//m_AndroidUserManager.ConcludeService();

	////ֹͣ����;
	//if(m_pIMatchServiceManager!=NULL)
	//	m_pIMatchServiceManager->StopService();

	//�Ƴ���Ϣ
	RemoveSystemMessage();

	//�Ƴ�����
	m_UserTaskManager.ResetTaskManager();

	//��λ�ؼ���
	m_WordsFilter.ResetSensitiveWordArray();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
			m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("��������Э�������� [ %s:%d ]"),pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//��������
		{
			//��������
			//m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_PARAMETER,0L,NULL,0L);

			//���ػ���
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_PARAMETER,0L,NULL,0L);			

			//������Ϣ
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

			//��������
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

			//���ص���
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_GAME_PROPERTY,0L,NULL,0L);

			return true;
		}
	case CT_SERVICE_CONTROL:
		{
			//Ч����Ϣ;
			ASSERT(wDataSize == sizeof(CP_ServiceControlRequest));
			if (wDataSize != sizeof(CP_ServiceControlRequest)) return 0;

			//��������;
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

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//����ʱ��
	if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
	{
		//ʱ�䴦��
		switch (dwTimerID)
		{
		case IDI_LOAD_ANDROID_USER:		//���ػ���
			{
				//��������
				DWORD dwBatchID=0;
				DWORD dwAndroidCount=0;

				//�����ж�
				if(m_AndroidUserManager.GetAndroidLoadInfo(dwBatchID,dwAndroidCount))
				{
					//����ṹ
					DBR_GR_LoadAndroidUser LoadAndroidUser;
					LoadAndroidUser.dwBatchID=dwBatchID;
					LoadAndroidUser.dwAndroidCount=dwAndroidCount;

					//���ػ���
					m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,&LoadAndroidUser,sizeof(LoadAndroidUser));
				}

				return true;
			}
		case IDI_REPORT_SERVER_INFO:	//������Ϣ
			{
				//��������
				CMD_CS_C_ServerOnLine ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//���ñ���				
				ServerOnLine.dwAndroidCount=m_ServerUserManager.GetAndroidCount();
				ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount()-ServerOnLine.dwAndroidCount;

				//��������
				m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_ONLINE,&ServerOnLine,sizeof(ServerOnLine));

				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//����Э��
			{
				//��������
				tagAddressInfo * pCorrespondAddress=&m_pInitParameter->m_CorrespondAddress;
				m_pITCPSocketService->Connect(pCorrespondAddress->szAddress,m_pInitParameter->m_wCorrespondPort);

				//������ʾ
				TCHAR szString[512]=TEXT("");
				_sntprintf_s(szString, CountArray(szString), TEXT("��������Э�������� [ %s:%d ]"), pCorrespondAddress->szAddress, m_pInitParameter->m_wCorrespondPort);

				//��ʾ��Ϣ
				CTraceService::TraceString(szString,TraceLevel_Normal);

				return true;
			}
		case IDI_GAME_SERVICE_PULSE:	//����ά��
			{
				return true;
			}
		case IDI_DISTRIBUTE_ANDROID:	//�������
			{
				//��������
				if (m_AndroidUserManager.GetAndroidCount()>0 && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
				{
					//��������
					bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
					bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
					bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
					bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));

					//ģ�⴦��
					if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
					{
						//����״̬
						tagAndroidUserInfo AndroidSimulate; 
						m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);

						//��������
						if (AndroidSimulate.wFreeUserCount>0)
						{
							WORD wTime = 8;
							if (wTime > AndroidSimulate.wFreeUserCount)
							{
								wTime = AndroidSimulate.wFreeUserCount;
							}

							for (WORD i = 0; i<wTime; i++)
							{
								//�������
								WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));

								//��ȡ����
								CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
								if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

								//����״��
								tagTableUserInfo TableUserInfo;
								WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

								//�����ж�
								if (TableUserInfo.wTableUserCount>0) continue;
								if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

								//�����ж�
								if (AndroidSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
								{
									//��������
									WORD wHandleCount=0;
									WORD wWantAndroidCount=TableUserInfo.wMinUserCount;

									//���ݵ���
									if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
									{
										WORD wChairCount=m_pGameServiceAttrib->wChairCount;
										WORD wFreeUserCount=AndroidSimulate.wFreeUserCount;
										WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
										wWantAndroidCount+=((wOffUserCount > 0) ? (rand()%(wOffUserCount+1)) : 0);
									}

									//���´���
									for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
									{
										//��������
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//��Ч����
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										//�û�����
										IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
										bool bResult = pTableFrame->PerformSitDownAction(wChairID, pIAndroidUserItem->GetMeUserItem());
										if (bResult)
										{
											//���ñ���
											wHandleCount++;

											//����ж�
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

					//�����
					if (bAllowAndroidAttend==true)
					{
						//����״̬
						tagAndroidUserInfo AndroidPassivity;
						m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);

						if(bAllowAvertCheatMode)
						{
							//���´���
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
							//��������
							if (AndroidPassivity.wFreeUserCount>0)
							{
								//������Ϸ
								if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
								{
									for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
									{
										//��ȡ����
										CTableFrame * pTableFrame=m_TableFrameArray[i];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

										//����״��
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//�����ж�
										if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;

										//��������
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//��Ч����
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										//���´���
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
										//��ȡ����
										CTableFrame * pTableFrame=m_TableFrameArray[i];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

										//����״��
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//�����ж�
										if (wUserSitCount==0) continue;
										if (TableUserInfo.wTableUserCount==0) continue;
										if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(rand()%10>5)) continue;

										//��������
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//��Ч����
										//ASSERT(wChairID!=INVALID_CHAIR);
										if (wChairID==INVALID_CHAIR) continue;

										//���´���
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

					//�����
					if (bAllowAndroidAttend==true)
					{
						//����״̬
						tagAndroidUserInfo AndroidInitiative;
						m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
						WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;

						if(bAllowAvertCheatMode)
						{
							//���´���
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
							//��������
							if (AndroidInitiative.wFreeUserCount>0)
							{
								for (INT_PTR i=0;i<m_pGameServiceOption->wTableCount;i++)
								{
									//��ȡ����
									CTableFrame * pTableFrame=m_TableFrameArray[i];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//����״��
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//�����ж�
									if((m_pGameServiceAttrib->wChairCount<MAX_CHAIR) && wUserSitCount>(TableUserInfo.wMinUserCount-1)) continue;

									//��������
									IServerUserItem * pIServerUserItem=NULL;
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//��Ч����
									//ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR) continue;

									//���´���
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

					//��������
					bool bMaxPlayerGame=m_pGameServiceAttrib->wChairCount>=MAX_CHAIR;

					//�����ж�
					if(bMaxPlayerGame)
					{
						//��ȡʱ��
						DWORD dwCurrTime = (DWORD)time(NULL);
						if(dwCurrTime<m_dwIntervalTime+m_dwLastDisposeTime) return true;

						//���ñ���
						m_dwLastDisposeTime=dwCurrTime;
						m_dwIntervalTime=MIN_INTERVAL_TIME+rand()%(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME);   
					}

					//��������
					WORD wStandUpCount=0;
					WORD wRandCount=((rand()%3)+1);
					INT_PTR nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
					for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
					{
						//��ȡ����
						INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
						CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
						if (pTableFrame->IsGameStarted()==true) continue;

						//����״��
						tagTableUserInfo TableUserInfo;
						WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

						//�û�����
						bool bRand = ((rand()%100)>50);
						if (TableUserInfo.wTableAndroidCount==0) continue;
						if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
						if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;

						//��������
						for (WORD j=0;j<pTableFrame->GetChairCount();j++)
						{
							//��ȡ�û�
							IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
							if (pIServerUserItem==NULL) continue;

							//�û�����
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

					//��������
					nIndex = rand()%(__max(m_pGameServiceOption->wTableCount,1));
					for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
					{
						//��ȡ����
						INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
						CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
						if (pTableFrame->IsGameStarted()==true) continue;

						//����״��
						tagTableUserInfo TableUserInfo;
						WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

						//�û�����
						bool bRand = ((rand()%100)>50);
						if (TableUserInfo.wTableAndroidCount==0) continue;
						if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;

						//��������
						for (WORD j=0;j<pTableFrame->GetChairCount();j++)
						{
							//��ȡ�û�
							IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
							if (pIServerUserItem==NULL) continue;

							//�û�����
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

				//��������
				if(m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
				{
					//�����ʶ
					bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);

					//���洦��
					if (bAllowAndroidAttend==true)
					{
						//����״̬
						tagAndroidUserInfo AndroidPassivity;
						m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);
							
						//���´���
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
		case IDI_DBCORRESPOND_NOTIFY: //���涨ʱ����
			{
				if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnTimerNotify();
				return true;
			}
		case IDI_LOAD_SYSTEM_MESSAGE: //ϵͳ��Ϣ
			{
				//�����Ϣ����
				RemoveSystemMessage();

				//������Ϣ
				m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_SYSTEM_MESSAGE,0L,NULL,0L);

				return true;
			}
		case IDI_LOAD_SENSITIVE_WORD:	//�������д�
			{
				//Ͷ������
				m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
				return true;
			}
		case IDI_SEND_SYSTEM_MESSAGE: //ϵͳ��Ϣ
			{
				//�����ж�
				if(m_SystemMessageActive.GetCount()==0) return true;

				//ʱЧ�ж�
				DWORD dwCurrTime = (DWORD)time(NULL);
				for(INT_PTR nIndex=m_SystemMessageActive.GetCount()-1;nIndex>=0;nIndex--)
				{
					tagSystemMessage *pTagSystemMessage = m_SystemMessageActive[nIndex];

					//ʱЧ�ж�
					if(pTagSystemMessage->SystemMessage.tConcludeTime < dwCurrTime)
					{
						m_SystemMessageActive.RemoveAt(nIndex);
						SafeDelete(pTagSystemMessage);

						continue;
					}

					//����ж�
					if(pTagSystemMessage->dwLastTime+pTagSystemMessage->SystemMessage.dwTimeRate < dwCurrTime)
					{
						//��������
						pTagSystemMessage->dwLastTime=dwCurrTime;

						//������Ϣ
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

						//������Ϣ
						WORD wSendSize = sizeof(SendMessage)-sizeof(SendMessage.szSystemMessage)+CountStringBuffer(SendMessage.szSystemMessage);
						SendSystemMessage(&SendMessage,wSendSize);
					}
				}

				return true;
			}
		case IDI_DISTRIBUTE_USER: //�����û�
			{
				//ִ�з���
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

	//����ʱ��
	if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
	{
		//ʱ�䴦��
		m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

		return true;
	}

	//������ʱ��
	if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
	{
		if(m_pIMatchServiceManager!=NULL) m_pIMatchServiceManager->OnEventTimer(dwTimerID,wBindParam);
		return true;
	}

	//����ʱ��
	if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
	{
		//���Ӻ���
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
		WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

		//ʱ��Ч��
		if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
		{
			ASSERT(FALSE);
			return false;
		}

		//ʱ��֪ͨ
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_LOGON_SUCCESS:			//��¼�ɹ�
		{
			return OnDBLogonSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOGON_FAILURE:			//��¼ʧ��
		{
			return OnDBLogonFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_PARAMETER:	//��������
		{
			return OnDBGameAndroidParameter(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_ANDROID_INFO:		//������Ϣ
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_GAME_PROPERTY_INFO:		//������Ϣ
		{
			return OnDBGamePropertyInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_PROPERTY_INFO:		//������Ϣ
		{
			return OnDBUserPropertyInfo(dwContextID, pData, wDataSize);
		}
	case DBO_GR_USER_INSURE_INFO:		//������Ϣ
		{
			return OnDBUserInsureInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_SUCCESS:	//���гɹ�
		{
			return OnDBUserInsureSuccess(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_FAILURE:	//����ʧ��
		{
			return OnDBUserInsureFailure(dwContextID,pData,wDataSize);
		}
	case DBO_GR_USER_INSURE_USER_INFO:  //�û���Ϣ
		{
			return OnDBUserInsureUserInfo(dwContextID,pData,wDataSize);
		}
	//case DBO_GR_PROPERTY_SUCCESS:		//���߳ɹ�
	//	{
	//		return OnDBPropertySuccess(dwContextID,pData,wDataSize);
	//	}
	case DBO_GR_SYSTEM_MESSAGE_RESULT:  //ϵͳ��Ϣ
		{
			return OnDBSystemMessage(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SYSTEM_MESSAGE_FINISH:	//�������
		{
			return OnDBSystemMessageFinish(dwContextID,pData,wDataSize);
		}
	case DBO_GR_SENSITIVE_WORDS:		//�������д�
		{
			return OnDBSensitiveWords(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST:				//�����б�
		{
			return OnDBUserTaskList(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_LIST_END:			//�������
		{
			return OnDBUserTaskListEnd(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_INFO:				//������Ϣ
		{
			return OnDBUserTaskInfo(dwContextID,pData,wDataSize);
		}
	case DBO_GR_TASK_RESULT:			//������
		{
			return OnDBUserTaskResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_PURCHASE_RESULT:		//������
		{
			return OnDBPurchaseResult(dwContextID,pData,wDataSize);
		}
	case DBO_GR_EXCHANGE_RESULT:		//�һ����
		{
			return OnDBExChangeResult(dwContextID,pData,wDataSize);
		}
	case DBR_GR_SEND_EMAIL_SUCCESS:		//�ʼ����ųɹ�
		{
			return onDBSendEMailSuccess(dwContextID, pData, wDataSize);
		}
	case DBO_GR_REREG_SERVER_IN_REDIS:
		{
			return OnDBReRegServerInRedis(dwContextID, pData, wDataSize);
		}
	}

	//�����¼�
	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//����Ч��
		if(m_pIMatchServiceManager==NULL) return false;

		//�����û�
		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem * pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		return m_pIMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//���ñ���
		m_bCollectUser=false;

		//ɾ��ʱ��
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//�����ж�
		if (m_bNeekCorrespond==true)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Э�������������ӹر��ˣ�%ld �����������"), m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);
		}

		return true;
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڷ�����Ϸ����ע����Ϣ..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//����˿�
		CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
		RegisterServer.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//��������
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

		//��������
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVER,&RegisterServer,sizeof(RegisterServer));

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_SERVICE_INFO:	//������Ϣ
			{
				return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_USER_COLLECT:	//�û�����
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE: //�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_ANDROID_SERVICE: //��������
			{
				return OnTCPSocketMainAndroidService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_WEB_SERVICE:	//��վ����
			{
				return OnTCPSocketMainWebService(Command.wSubCmdID, pData, wDataSize);
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);

		return true;
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������;
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	if(pBindParameter==NULL) return false;

	//��ȡ�û�;
	IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;
	WORD wTableID	= INVALID_WORD;
	DWORD dwUserID	= INVALID_DWORD;

	try
	{
		//�û�����;
		if (pIServerUserItem!=NULL)
		{
			//��������;
			wTableID = pIServerUserItem->GetTableID();
			dwUserID = pIServerUserItem->GetUserID();

			//���ߴ���;
			if (wTableID!=INVALID_TABLE)
			{
				//�����;
				pIServerUserItem->DetachBindStatus();

				//����֪ͨ;
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
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("�ر������쳣: wTableID=%d"), wTableID);
		CTraceService::TraceString(szMessage,TraceLevel_Normal);
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	CString strMsg;
	BYTE* pIP = (BYTE*)&dwClientAddr;
	strMsg.Format(TEXT("ip: %d.%d.%d.%d, userID:[%d] ����"), pIP[0], pIP[1], pIP[2], pIP[3], dwUserID);
	CTraceService::TraceString(strMsg, TraceLevel_Normal);

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_USER:		//�û�����
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_LOGON:		//��¼����
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:		//��Ϸ����
		{
			return OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_FRAME:		//�������
		{
			return OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_INSURE:		//��������
		{
			return OnTCPNetworkMainInsure(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_TASK:		//��������
		{
			return OnTCPNetworkMainTask(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_EXCHANGE:	//�һ�����
		{
			return OnTCPNetworkMainExchange(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MANAGE:		//��������
		{
			return OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MATCH:		//��������
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_PRIVATE:	//˽�˳�����;
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID, pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	//SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
	SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	//SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
	SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->IsOnline())
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		//SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
		SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID,true);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->IsOnline()&&pIServerUserItem->IsClientReady())
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		//SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
		SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
			{
				m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID, true);
			}
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_GF_FRAME,SUB_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	//SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString, CYSCharUtils::ConvertToUTF8(lpszMessage), CountArray(SystemMessage.szString));
	SystemMessage.wLength = lstrlen(SystemMessage.szString) + 1;

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	if (bAndroid)
	{
		//�����û�
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//�����û�
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//�û�����
	if (cbSendMask==BG_ALL_CLIENT)
	{
		//m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_MOBILE);
		//m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,BG_COMPUTER);
		m_pITCPNetworkEngine->SendDataBatch(wMainCmdID, wSubCmdID, pData, wDataSize, cbSendMask);
	}
	else if (cbSendMask == BG_COMPUTER)
	{
		//m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);
		
		// ���͸�PC���
		WORD wEnumIndex = 0;
		while (wEnumIndex < m_ServerUserManager.GetUserItemCount())
		{
			//�����û�
			IServerUserItem *pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) continue;
			if (!pIServerUserItem->IsOnline()) continue;
			if (pIServerUserItem->IsAndroidUser()) continue;

			//�����û�
			WORD wBindIndex = pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//�����û�
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//�����û�
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->IsOnline())
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}

	return false;
}

//Ⱥ������
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

//�û�����
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	
	tagUserInfo * pUserInfo = pIServerUserItem->GetUserInfo();
	
	//��������
	CMD_GR_UserScore UserScore;

	//��������
	UserScore.dwUserID = pUserInfo->dwUserID;
	UserScore.UserScore.dwWinCount = pUserInfo->dwWinCount;
	UserScore.UserScore.dwLostCount = pUserInfo->dwLostCount;
	UserScore.UserScore.dwDrawCount = pUserInfo->dwDrawCount;
	UserScore.UserScore.dwFleeCount = pUserInfo->dwFleeCount;
	UserScore.UserScore.dwExperience = pUserInfo->dwExperience;

	//�������
	UserScore.UserScore.lScore = pUserInfo->lScore;
	UserScore.UserScore.lHoldScore = pUserInfo->lScore + pIServerUserItem->GetTrusteeScore();
	UserScore.UserScore.lHoldScore += pIServerUserItem->GetFrozenedScore();

	//��������
	SendDataToTable(pIServerUserItem->GetTableID(), MDM_GR_USER, SUB_GR_USER_SCORE, &UserScore, sizeof(UserScore));

	//��ʱд��
	if ((CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->IsVariation()==true))
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwDBQuestID=pIServerUserItem->GetDBQuestID();
		WriteGameScore.dwClientAddr=pIServerUserItem->GetClientAddr();
		WriteGameScore.dwInoutIndex=pIServerUserItem->GetInoutIndex();

		//��ȡ����
		pIServerUserItem->DistillVariation(WriteGameScore.VariationInfo);

		//��������
		//if(pIServerUserItem->IsAndroidUser()==true)
		//{
		//	WriteGameScore.VariationInfo.lScore=0;
		//	WriteGameScore.VariationInfo.lGrade=0;
		//	WriteGameScore.VariationInfo.lInsure=0;
		//}

		//if (pUserTaskEntry != NULL) WriteGameScore.bTaskForward = true;

		WriteGameScore.cbMaxCardType = pIServerUserItem->GetMaxCardData(WriteGameScore.aryCardData, MAX_CARD_DATA_COUNT);

		//Ͷ������
		m_pIDBCorrespondManager->PostDataBaseRequest(WriteGameScore.dwUserID,DBR_GR_WRITE_GAME_SCORE,0L,&WriteGameScore,sizeof(WriteGameScore), TRUE);

		//�ƽ�����
		if (pIServerUserItem->IsAndroidUser() == false && cbReason != 0)
		{
			tagUserTaskEntry * pUserTaskEntry = m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(), TASK_STATUS_UNFINISH);
			if (pUserTaskEntry != NULL)
			{
				PerformTaskProgress(pIServerUserItem, pUserTaskEntry, WriteGameScore.VariationInfo, cbReason);
			}
		}
	}

	//֪ͨ����, ����δ������ʱ����
	//if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	//{
	//	m_TableFrameArray[pIServerUserItem->GetTableID()]->OnUserScroeNotify(pIServerUserItem->GetChairID(),pIServerUserItem,cbReason);
	//}

	return true;
}

//�û�״̬
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//��ǰ״̬
	BYTE cbCurUserStatus = pIServerUserItem->GetUserStatus();

	//��������
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus = cbCurUserStatus;

	//��������
	if (US_SIT == cbCurUserStatus || (US_READY == cbCurUserStatus&&GAME_GENRE_MATCH==m_pGameServiceOption->wServerType))
	{
		//����ͻ����Ѿ�׼������Ⱥ��������Ϣ������ֻ��������
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
		//���վ����޷�������Ⱥ����Ϣ�������Ϣ��������������Ѳ���һ��״̬�ı���Ϣ
		if (US_FREE == cbCurUserStatus && !pIServerUserItem->IsClientReady())
		{
			SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}

		SendDataToTable(wOldTableID, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		
	}

	//�뿪�ж�
	if (cbCurUserStatus == US_NULL)
	{
		//��ȡ��
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//�������
		if (pBindParameter!=NULL)
		{
			//�󶨴���
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//�ж�����
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

		//�뿪����
		OnEventUserLogout(pIServerUserItem,0L);
	}
	else if (cbCurUserStatus == US_LOOKON)
	{
		// ������Թ�״̬��ֻ���͸��Լ�
		SendData(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
	}

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	DBO_GR_LogonSuccess * pDBOLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//�����ж�
	if ((pBindParameter->pIServerUserItem!=NULL)||(pBindParameter->dwSocketID!=dwContextID))
	{
		//�������
		ASSERT(FALSE);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_NORMAL);

		return true;
	}

	//��������
	bool bAndroidUser=(wBindIndex>=INDEX_ANDROID);
	bool bMobileUser=(pBindParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//�����ж�
	DWORD dwKickTime;
	if(m_KickUserItemMap.Lookup(pDBOLogonSuccess->dwUserID,dwKickTime)==TRUE)
	{
		//ʱЧ�ж�
		DWORD dwCurrTime = (DWORD)time(NULL);
		if(dwKickTime+VALID_TIME_KICK_BY_MANAGER > dwCurrTime)
		{
			//����ʧ��
			SendLogonFailure(TEXT("���ѱ�����Ա�������,1Сʱ֮�ڲ��ܽ��룡"),0,pBindParameter->dwSocketID);

			//�������
			PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

			return true;
		}
		else
		{
			//�Ƴ����
			m_KickUserItemMap.RemoveKey(pDBOLogonSuccess->dwUserID);
		}
	}

	//�л��ж�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		//�л��û�
		SwitchUserItemConnect(pIServerUserItem, pDBOLogonSuccess->szMachineID, dwContextID, pDBOLogonSuccess->cbDeviceType);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_USER_IMPACT);

		return true;
	}

	////ά���ж�
	//if ((CServerRule::IsForfendRoomEnter(m_pGameServiceOption->dwServerRule)==true)&&(pDBOLogonSuccess->cbMasterOrder==0))
	//{
	//	//����ʧ��
	//	SendLogonFailure(TEXT("��Ǹ������ϵͳά����ԭ�򣬵�ǰ��Ϸ�����ֹ�û����룡"),0,pBindParameter->dwSocketID);

	//	//�������
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SYSTEM);

	//	return true;
	//}

	////���һ���
	//if (bAndroidUser==true && m_pGameServiceOption->wServerType!=GAME_GENRE_MATCH)
	//{
	//	//���һ���
	//	DWORD dwUserID=pDBOLogonSuccess->dwUserID;
	//	IAndroidUserItem * pIAndroidUserItem=m_AndroidUserManager.SearchAndroidUserItem(dwUserID,dwContextID);

	//	//�޸Ļ���
	//	if (pIAndroidUserItem!=NULL)
	//	{
	//		//��ȡ����
	//		tagAndroidParameter * pAndroidParameter=pIAndroidUserItem->GetAndroidParameter();

	//		//��������
	//		if ((pAndroidParameter->lTakeMinScore!=0L)&&(pAndroidParameter->lTakeMaxScore!=0L))
	//		{
	//			//��������
	//			SCORE lTakeMinScore=(SCORE)pAndroidParameter->lTakeMinScore;
	//			SCORE lTakeMaxScore=(SCORE)__max(pAndroidParameter->lTakeMaxScore,pAndroidParameter->lTakeMinScore);

	//			//��������
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

	////��ͷ���
	//if ((m_pGameServiceOption->lMinEnterScore!=0L)&&(pDBOLogonSuccess->lScore<m_pGameServiceOption->lMinEnterScore))
	//{
	//	//����ʧ��
	//	TCHAR szMsg[128]=TEXT("");
	//	_sntprintf(szMsg,CountArray(szMsg), TEXT("��Ǹ��������Ϸ�ɼ����ڵ�ǰ��Ϸ�������ͽ���ɼ�%I64d�����ܽ��뵱ǰ��Ϸ���䣡"), m_pGameServiceOption->lMinEnterScore);
	//	SendLogonFailure(szMsg,0,pBindParameter->dwSocketID);

	//	//�������
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	////��߷���
	//if ((m_pGameServiceOption->lMaxEnterScore!=0L)&&(pDBOLogonSuccess->lScore>m_pGameServiceOption->lMaxEnterScore))
	//{
	//	//����ʧ��
	//	TCHAR szMsg[128]=TEXT("");
	//	_sntprintf(szMsg,CountArray(szMsg), TEXT("��Ǹ��������Ϸ�ɼ����ڵ�ǰ��Ϸ�������߽���ɼ�%I64d�����ܽ��뵱ǰ��Ϸ���䣡"), m_pGameServiceOption->lMaxEnterScore);
	//	SendLogonFailure(szMsg,0,pBindParameter->dwSocketID);

	//	//�������
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	////��Ա�ж�
	//if(m_pGameServiceOption->cbMinEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder < m_pGameServiceOption->cbMinEnterMember)
	//{
	//	//����ʧ��
	//	SendLogonFailure(TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ��Ϸ�������ͽ����Ա���������ܽ��뵱ǰ��Ϸ���䣡"),0,pBindParameter->dwSocketID);

	//	//�������
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	////��Ա�ж�
	//if(m_pGameServiceOption->cbMaxEnterMember != 0 && pDBOLogonSuccess->cbMemberOrder > m_pGameServiceOption->cbMaxEnterMember)
	//{
	//	//����ʧ��
	//	SendLogonFailure(TEXT("��Ǹ�����Ļ�Ա������ڵ�ǰ��Ϸ�������߽����Ա���������ܽ��뵱ǰ��Ϸ���䣡"),0,pBindParameter->dwSocketID);

	//	//�������
	//	PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_CONDITIONS);

	//	return true;
	//}

	//�����ж�
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	if ((pDBOLogonSuccess->cbMemberOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-RESERVE_USER_COUNT)))
	{
		//����ʧ��
		SendLogonFailure(TEXT("��Ǹ�����ڴ˷����Ѿ���������ͨ��Ҳ��ܼ��������ˣ�"),0,pBindParameter->dwSocketID);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		return true;
	}

	//�û�����
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//��������
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	//UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;
	UserInfo.dwCustomFaceVer = pDBOLogonSuccess->dwCustomFaceVer;
	lstrcpyn(UserInfo.szNickName,pDBOLogonSuccess->szNickName,CountArray(UserInfo.szNickName));

	//�û�����
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	UserInfo.cbMemberOrder=pDBOLogonSuccess->cbMemberOrder;
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogonSuccess->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//״̬����
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//������Ϣ
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

	//�������
	UserInfoPlus.cbMaxCardType = pDBOLogonSuccess->cbMaxCardType;
	CopyMemory(UserInfoPlus.aryCardData, pDBOLogonSuccess->aryCardData, sizeof(UserInfoPlus.aryCardData));

	//��¼��Ϣ
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

	//�û�Ȩ��
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=0;

	//��������
	UserInfoPlus.bAndroidUser=bAndroidUser;
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	UserInfoPlus.lDefPutScore = m_pGameServiceOption->lDefPutScore;
	UserInfoPlus.lLowPutScore = m_pGameServiceOption->lLowPutScore;
	UserInfoPlus.iExchangeRatio = m_pGameServiceOption->iExchangeRatio;
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogonSuccess->szPassword,CountArray(UserInfoPlus.szPassword));

	//������Ϣ
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogonSuccess->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//�����û�
	m_ServerUserManager.InsertUserItem(&pIServerUserItem,UserInfo,UserInfoPlus);

	//if (bAndroidUser == false)
	//{
	//	//�û�����
	//	if (pDBOLogonSuccess->wTaskCount > 0)
	//	{
	//		m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(), pDBOLogonSuccess->UserTaskInfo, pDBOLogonSuccess->wTaskCount);
	//	}

	//	//���ص���
	//	DBR_GR_LOADPROPERTY LoadProperty;
	//	ZeroMemory(&LoadProperty, sizeof(LoadProperty));
	//	LoadProperty.dwUserID = pIServerUserItem->GetUserID();
	//	m_pIDBCorrespondManager->PostDataBaseRequest(LoadProperty.dwUserID, DBR_GR_LOAD_USER_PROPERTY, 0, &LoadProperty, sizeof(LoadProperty));
	//}
	
	//�����ж�
	if (pIServerUserItem==NULL)
	{
		//�������
		ASSERT(FALSE);

		//�������
		PerformUnlockScore(pDBOLogonSuccess->dwUserID,pDBOLogonSuccess->dwInoutIndex,LER_SERVER_FULL);

		//�Ͽ��û�
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

	//�����û�
	pBindParameter->pIServerUserItem=pIServerUserItem;

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,false);

	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserEnter UserEnter;
		ZeroMemory(&UserEnter,sizeof(UserEnter));

		//���ñ���
		UserEnter.dwUserID=pIServerUserItem->GetUserID();
		//UserEnter.dwGameID=pIServerUserItem->GetGameID();
		lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

		//������Ϣ
		UserEnter.cbGender=pIServerUserItem->GetGender();
		UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
		UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

		//������Ϣ
		ASSERT(m_pITCPSocketService!=NULL);
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem!=NULL)) return true;

	//���ʹ���
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;
	SendLogonFailure(pLogonFailure->szDescribeString,pLogonFailure->lResultCode,dwContextID);

	//�Ͽ�����
	if (LOWORD(dwContextID)>=INDEX_ANDROID)
	{
		CString strTmp;
		strTmp.Format(_T("������ OnDBLogonFailure %s"), pLogonFailure->szDescribeString);
		CTraceService::TraceString(strTmp, TraceLevel_Exception);
		m_AndroidUserManager.DeleteAndroidUserItem(dwContextID,true);
	}
	else
	{
		m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidAccountsInfo);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidAccountsInfo[0])))) return false;

	//���û���
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.InsertAndroidInfo(pGameAndroidInfo->AndroidAccountsInfo,pGameAndroidInfo->wAndroidCount,pGameAndroidInfo->dwBatchID);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameAndroidParameter * pGameAndroidParameter=(DBO_GR_GameAndroidParameter *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidParameter)-sizeof(pGameAndroidParameter->AndroidParameter);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidParameter->wParameterCount*sizeof(pGameAndroidParameter->AndroidParameter[0])))) return false;

	//���û���
	if (pGameAndroidParameter->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.AddAndroidParameter(pGameAndroidParameter->AndroidParameter,pGameAndroidParameter->wParameterCount);
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGamePropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GamePropertyInfo * pGamePropertyInfo = (DBO_GR_GamePropertyInfo *)pData;
	WORD wHeadSize = sizeof(DBO_GR_GamePropertyInfo) - sizeof(pGamePropertyInfo->PropertyInfo);

	//Ч�����
	ASSERT((wDataSize >= wHeadSize) && (wDataSize == (wHeadSize + pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0]))));
	if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0])))) return false;

	//��ȡ״̬
	CServiceUnits * pServiceUnits = CServiceUnits::g_pServiceUnits;
	enServiceStatus ServiceStatus = pServiceUnits->GetServiceStatus();

	//���õ���
	if (pGamePropertyInfo->lResultCode == DB_SUCCESS)
	{
		//���ù���
		m_GamePropertyManager.SetGamePropertyInfo(pGamePropertyInfo->PropertyInfo, pGamePropertyInfo->cbPropertyCount);

		//��������
		m_DataConfigProperty.cbPropertyCount = pGamePropertyInfo->cbPropertyCount;
		CopyMemory(m_DataConfigProperty.PropertyInfo, pGamePropertyInfo->PropertyInfo, pGamePropertyInfo->cbPropertyCount*sizeof(pGamePropertyInfo->PropertyInfo[0]));
	}

	//�¼�֪ͨ
	if (ServiceStatus != ServiceStatus_Service)
	{
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBUserPropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_PropertyListInfo * pPropertyListInfo = (DBO_GR_PropertyListInfo *)pData;
	if (pPropertyListInfo->wPropertyCount <= 0) return true;

	WORD propsize = sizeof(tagUserProperty);

	//Ч�����
	ASSERT(wDataSize == pPropertyListInfo->wPropertyCount*propsize + sizeof(DBO_GR_PropertyListInfo));
	if (wDataSize != pPropertyListInfo->wPropertyCount*propsize + sizeof(DBO_GR_PropertyListInfo)) return false;

	//��ȡ�û�
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

//������Ϣ
bool CAttemperEngineSink::OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureInfo * pUserInsureInfo=(DBO_GR_UserInsureInfo *)pData;

	//��������
	CMD_GR_S_UserInsureInfo UserInsureInfo;
	ZeroMemory(&UserInsureInfo,sizeof(UserInsureInfo));

	//��������
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

	//��������
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_INSURE,SUB_GR_USER_INSURE_INFO,&UserInsureInfo,sizeof(UserInsureInfo));

	return true;
}

//���гɹ�
bool CAttemperEngineSink::OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserInsureSuccess * pUserInsureSuccess=(DBO_GR_UserInsureSuccess *)pData;

	//��������
	SCORE lFrozenedScore=pUserInsureSuccess->lFrozenedScore;
	SCORE lInsureRevenue=pUserInsureSuccess->lInsureRevenue;
	SCORE lVariationScore=pUserInsureSuccess->lVariationScore;
	SCORE lVariationInsure=pUserInsureSuccess->lVariationInsure;

	//�ⶳ����
	if ((lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//���в���
	if (pIServerUserItem->ModifyUserInsure(lVariationScore,lVariationInsure,lInsureRevenue)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	CMD_GR_S_UserInsureSuccess UserInsureSuccess;
	ZeroMemory(&UserInsureSuccess,sizeof(UserInsureSuccess));

	//�������
	UserInsureSuccess.cbActivityGame=pUserInsureSuccess->cbActivityGame;
	UserInsureSuccess.lUserScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	UserInsureSuccess.lUserInsure=pUserInsureSuccess->lSourceInsure+pUserInsureSuccess->lVariationInsure;
	lstrcpyn(UserInsureSuccess.szDescribeString,pUserInsureSuccess->szDescribeString,CountArray(UserInsureSuccess.szDescribeString));

	//��������
	WORD wDescribe=CountStringBuffer(UserInsureSuccess.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureSuccess)-sizeof(UserInsureSuccess.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_SUCCESS,&UserInsureSuccess,wHeadSize+wDescribe);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//���ʹ���
	DBO_GR_UserInsureFailure * pUserInsureFailure=(DBO_GR_UserInsureFailure *)pData;
	SendInsureFailure(pIServerUserItem,pUserInsureFailure->szDescribeString,pUserInsureFailure->lResultCode,pUserInsureFailure->cbActivityGame);

	//�ⶳ����
	if ((pUserInsureFailure->lFrozenedScore>0L)&&(pIServerUserItem->UnFrozenedUserScore(pUserInsureFailure->lFrozenedScore)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
	if ((pBindParameter->dwSocketID!=dwContextID)||(pBindParameter->pIServerUserItem==NULL)) return true;

	//��ȡ�û�
	ASSERT(GetBindUserItem(LOWORD(dwContextID))!=NULL);
	IServerUserItem * pIServerUserItem=GetBindUserItem(LOWORD(dwContextID));

	//��������
	DBO_GR_UserTransferUserInfo * pTransferUserInfo=(DBO_GR_UserTransferUserInfo *)pData;

	//��������
	CMD_GR_S_UserTransferUserInfo UserTransferUserInfo;
	ZeroMemory(&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	//�������
	UserTransferUserInfo.cbActivityGame=pTransferUserInfo->cbActivityGame;
	UserTransferUserInfo.dwTargetGameID=pTransferUserInfo->dwGameID;
	lstrcpyn(UserTransferUserInfo.szAccounts,pTransferUserInfo->szAccounts,CountArray(UserTransferUserInfo.szAccounts));

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_TRANSFER_USER_INFO,&UserTransferUserInfo,sizeof(UserTransferUserInfo));

	return true;
}

////���߳ɹ�
//bool CAttemperEngineSink::OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
//{
//	//��������
//	DBO_GR_S_PropertySuccess * pPropertySuccess=(DBO_GR_S_PropertySuccess *)pData;
//
//	//��ȡ�û�
//	IServerUserItem * pISourceUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwSourceUserID);
//	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pPropertySuccess->dwTargetUserID);
//
//	//�����û�
//	if (pISourceUserItem!=NULL)
//	{
//		//��������
//		SCORE lFrozenedScore=pPropertySuccess->lFrozenedScore;
//
//		//�ⶳ����
//		if ((lFrozenedScore>0L)&&(pISourceUserItem->UnFrozenedUserScore(lFrozenedScore)==false))
//		{
//			ASSERT(FALSE);
//			return false;
//		}
//	}
//
//	//��������
//	if (pISourceUserItem!=NULL)
//	{
//		pISourceUserItem->ModifyUserProperty(0,pPropertySuccess->lSendLoveLiness);
//	}
//
//	//��������
//	if(pITargetUserItem!=NULL)
//	{
//		pITargetUserItem->ModifyUserProperty(0,pPropertySuccess->lRecvLoveLiness);
//	}
//
//	//��������
//	DWORD dwCurrentTime=(DWORD)time(NULL);
//	tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();
//
//	//���ߴ���	
//	switch(pPropertySuccess->wPropertyIndex)
//	{
//	case PROPERTY_ID_VIP1_CARD: //�»�Ա
//	case PROPERTY_ID_VIP2_CARD:	//���Ȼ�Ա
//	case PROPERTY_ID_VIP3_CARD: //�����Ա
//		{
//			//�û���Ϣ
//			tagUserInfo * pUserInfo = pITargetUserItem->GetUserInfo();
//			if(pUserInfo==NULL) return true;
//
//			//���»�Ա
//			pUserInfo->cbMemberOrder=pPropertySuccess->cbMemberOrder;
//			
//			//�޸�Ȩ��
//			pITargetUserItem->ModifyUserRight(pPropertySuccess->dwUserRight,0);
//
//			//������Ϣ
//			SendPropertyEffect(pITargetUserItem);
//
//			break;
//		}
//	default:  // ȫ������
//		{			
//			break;
//		}
//	}
//
//	//���ѷ�ʽ
//	if(pPropertySuccess->cbConsumeScore==FALSE)
//	{
//		pISourceUserItem->ModifyUserInsure(0,-pPropertySuccess->lConsumeGold,0);
//	}
//
//	//��������
//	CMD_GR_S_PropertySuccess PropertySuccess;
//	ZeroMemory(&PropertySuccess,sizeof(PropertySuccess));
//
//	//���ñ���
//	PropertySuccess.cbRequestArea=pPropertySuccess->cbRequestArea;
//	PropertySuccess.wItemCount=pPropertySuccess->wItemCount;
//	PropertySuccess.wPropertyIndex=pPropertySuccess->wPropertyIndex;
//	PropertySuccess.dwSourceUserID=pPropertySuccess->dwSourceUserID;
//	PropertySuccess.dwTargetUserID=pPropertySuccess->dwTargetUserID;
//
//	//������Ϣ
//	if (pISourceUserItem!=NULL)
//	{
//		SendData(pISourceUserItem,MDM_GR_USER,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
//	}
//
//	//������Ϣ
//	if (pITargetUserItem!=NULL && pITargetUserItem!=pISourceUserItem)
//	{
//		SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_PROPERTY_SUCCESS,&PropertySuccess,sizeof(PropertySuccess));
//	}
//
//	//�㲥����
//	if(GetPropertyType(pPropertySuccess->wPropertyIndex)==PT_TYPE_PRESENT)
//		SendPropertyMessage(PropertySuccess.dwSourceUserID,PropertySuccess.dwTargetUserID,PropertySuccess.wPropertyIndex,
//		PropertySuccess.wItemCount);
//
//	return true;
//}
//
////����ʧ��
//bool CAttemperEngineSink::OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize)
//{
//	//����У��
//	ASSERT(wDataSize==sizeof(DBO_GR_PropertyFailure));
//	if(wDataSize!=sizeof(DBO_GR_PropertyFailure)) return false;
//
//	//��ȡ����
//	DBO_GR_PropertyFailure * pPropertyFailure = (DBO_GR_PropertyFailure *)pData;
//	if(pPropertyFailure==NULL) return false;
//
//	//��������
//	WORD wBindIndex=LOWORD(dwContextID);
//	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
//	if(pBindParameter==NULL) return false;
//	if(pBindParameter->pIServerUserItem==NULL) return false;
//
//	//������Ϣ
//	return SendPropertyFailure(pBindParameter->pIServerUserItem,pPropertyFailure->szDescribeString,0L,pPropertyFailure->cbRequestArea);
//}

//������
bool CAttemperEngineSink::OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	DBO_GR_PurchaseResult * pPurchaseResult=(DBO_GR_PurchaseResult *)pData;

	//����ṹ
	CMD_GR_PurchaseResult PurchaseResult;
	ZeroMemory(&PurchaseResult,sizeof(PurchaseResult));

	//���ñ���
	PurchaseResult.bSuccessed=pPurchaseResult->bSuccessed;
	PurchaseResult.lCurrScore=pPurchaseResult->lCurrScore;
	PurchaseResult.cbMemberOrder=pPurchaseResult->cbMemberOrder;
	lstrcpyn(PurchaseResult.szNotifyContent,pPurchaseResult->szNotifyContent,CountArray(PurchaseResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(PurchaseResult)-sizeof(PurchaseResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(PurchaseResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_PURCHASE_RESULT,&PurchaseResult,wSendDataSize);

	//����ж�
	if(PurchaseResult.bSuccessed==true)
	{
		//����Ȩ��
		pIServerUserItem->ModifyUserRight(pPurchaseResult->dwUserRight,0,UR_KIND_SERVER);		

		//������Ϸ��
		if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
		{
			pIServerUserItem->ModifyUserInsure(PurchaseResult.lCurrScore-pIServerUserItem->GetUserScore(),0,0);
		}
	}

	return true;
}

//�һ����
bool CAttemperEngineSink::OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	DBO_GR_ExchangeResult * pExchangeResult=(DBO_GR_ExchangeResult *)pData;

	//����ṹ
	CMD_GP_ExchangeResult ExchangeResult;
	ZeroMemory(&ExchangeResult,sizeof(ExchangeResult));

	//���ñ���
	ExchangeResult.bSuccessed=pExchangeResult->bSuccessed;
	ExchangeResult.lCurrScore=pExchangeResult->lCurrScore;
	ExchangeResult.lCurrIngot=pExchangeResult->lCurrIngot;
	lstrcpyn(ExchangeResult.szNotifyContent,pExchangeResult->szNotifyContent,CountArray(ExchangeResult.szNotifyContent));

	//��������
	WORD wSendDataSize = sizeof(ExchangeResult)-sizeof(ExchangeResult.szNotifyContent);
	wSendDataSize += CountStringBuffer(ExchangeResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_RESULT,&ExchangeResult,wSendDataSize);

	//������Ϸ��
	if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD && ExchangeResult.bSuccessed==true)
	{
		pIServerUserItem->ModifyUserInsure(ExchangeResult.lCurrScore-pIServerUserItem->GetUserScore(),0,0);
	}

	return true;
}

//�ʼ����ͳɹ�
bool CAttemperEngineSink::onDBSendEMailSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(wDataSize <= sizeof(tagSendBenefitSuc));
	if (wDataSize > sizeof(tagSendBenefitSuc)) return false;

	//��������
	tagSendBenefitSuc * pBenefitSuc = (tagSendBenefitSuc *)pData;
	IServerUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pBenefitSuc->dwUserID);
	if (pIServerUserItem != NULL)
	{
		pIServerUserItem->SetBenefitInfo(pBenefitSuc);
	}

	return true;
}

//�ڻ���������ע��
bool CAttemperEngineSink::OnDBReRegServerInRedis(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	RegServerInfoInRedis();

	return true;
}

//�����б�
bool CAttemperEngineSink::OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_TaskListInfo * pTaskListInfo=(DBO_GR_TaskListInfo *)pData;

	//��������	
	WORD wTaskCount = __min(pTaskListInfo->wTaskCount, CountArray(m_TaskParameter)-m_wTaskCount);
	CopyMemory(m_TaskParameter+m_wTaskCount, ++pTaskListInfo, sizeof(tagTaskParameter)*wTaskCount);

	//���ñ���
	m_wTaskCount += wTaskCount;

	return true;
}

//�������
bool CAttemperEngineSink::OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�Ƴ�����
	m_UserTaskManager.RemoveTaskParameter();

	//�������
	m_UserTaskManager.AddTaskParameter(m_TaskParameter, m_wTaskCount);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskInfo));
	if (wDataSize>sizeof(DBO_GR_TaskInfo)) return false;

	//��������
	DBO_GR_TaskInfo * pTaskInfo=(DBO_GR_TaskInfo *)pData;

	//��������
	WORD wTaskCount=0;
	WORD wSendSize=0,wParameterSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];
	LPBYTE pDataBuffer = cbDataBuffer;

	//��������
	pDataBuffer += sizeof(wTaskCount);
	wSendSize = sizeof(wTaskCount);

	//��������	
	POSITION Position=NULL;	
	do
	{
		tagTaskParameter * pTaskParameter=m_UserTaskManager.EnumTaskParameter(Position);			
		if(pTaskParameter!=NULL)
		{
			//Խ���ж�
			if(wSendSize+sizeof(tagTaskParameter)>SOCKET_TCP_PACKET)
			{
				//���ñ���
				pDataBuffer = cbDataBuffer;
				*(WORD*)pDataBuffer = wTaskCount; 

				//��������
				m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,pDataBuffer,wSendSize);

				//���ñ���
				wTaskCount = 0;
				pDataBuffer += sizeof(wSendSize);
				wSendSize = sizeof(wSendSize);				
			}

			//�����С
			wParameterSize = sizeof(tagTaskParameter);// -sizeof(pTaskParameter->szTaskDescribe) + CountStringBuffer(pTaskParameter->szTaskDescribe);
			*(WORD *)pDataBuffer = wParameterSize;
			pDataBuffer += sizeof(wParameterSize);
			wSendSize += sizeof(wParameterSize);

			//��������
			CopyMemory(pDataBuffer,pTaskParameter,wParameterSize);

			//�ƽ�ָ��
			wTaskCount++;
			pDataBuffer += wParameterSize;
			wSendSize += wParameterSize;
		}
	}while(Position!=NULL);

	//ʣ�෢��
	if(wTaskCount>0 && wSendSize)
	{
		//���ñ���
		pDataBuffer = cbDataBuffer;
		*(WORD *)pDataBuffer = wTaskCount; 

		//��������
		m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_LIST,pDataBuffer,wSendSize);
	}

	//����ṹ
	CMD_GR_S_TaskInfo TaskInfo;
	TaskInfo.wTaskCount = pTaskInfo->wTaskCount;
	CopyMemory(TaskInfo.TaskStatus,pTaskInfo->TaskStatus,sizeof(TaskInfo.TaskStatus[0])*pTaskInfo->wTaskCount);

	//��������
	WORD wSendDataSize = sizeof(TaskInfo)-sizeof(TaskInfo.TaskStatus);
	wSendDataSize += sizeof(TaskInfo.TaskStatus[0])*TaskInfo.wTaskCount;
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_INFO,&TaskInfo,wSendDataSize);

	return true;
}

//������
bool CAttemperEngineSink::OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	ASSERT(wDataSize<=sizeof(DBO_GR_TaskResult));
	if(wDataSize>sizeof(DBO_GR_TaskResult)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBO_GR_TaskResult * pTaskResult=(DBO_GR_TaskResult *)pData;

	//��������
	CMD_GR_S_TaskResult TaskResult;
	ZeroMemory(&TaskResult,sizeof(TaskResult));

	//�������
	TaskResult.wCommandID=pTaskResult->wCommandID;
	TaskResult.bSuccessed=pTaskResult->bSuccessed;
	TaskResult.lCurrScore=pTaskResult->lCurrScore;
	TaskResult.lCurrIngot=pTaskResult->lCurrIngot;
	lstrcpyn(TaskResult.szNotifyContent,pTaskResult->szNotifyContent,CountArray(TaskResult.szNotifyContent));

	//��������
	WORD wSendSize = sizeof(TaskResult)-sizeof(TaskResult.szNotifyContent)+CountStringBuffer(TaskResult.szNotifyContent);
	m_pITCPNetworkEngine->SendData(dwContextID,MDM_GR_TASK,SUB_GR_TASK_RESULT,&TaskResult,wSendSize);

	//��������
	if(TaskResult.bSuccessed==true && TaskResult.wCommandID==SUB_GR_TASK_TAKE)
	{
		tagTaskParameter * pTaskParameter=m_UserTaskManager.SearchTaskParameter(pTaskResult->wCurrTaskID);
		if(pTaskParameter!=NULL && pTaskParameter->wKindID==m_pGameServiceOption->wKindID)
		{
			//��������
			tagUserTaskInfo UserTaskInfo;

			//��������		
			UserTaskInfo.wTaskProgress=0;
			UserTaskInfo.cbTaskStatus=TASK_STATUS_UNFINISH;
			UserTaskInfo.wTaskID=pTaskResult->wCurrTaskID;
			UserTaskInfo.dwResidueTime=pTaskParameter->dwTimeLimit;
			UserTaskInfo.dwLastUpdateTime=(DWORD)time(NULL);

			//��������
			m_UserTaskManager.SetUserTaskInfo(pIServerUserItem->GetUserID(),&UserTaskInfo,1);
		}
	}

	return true;
}

//ϵͳ��Ϣ
bool CAttemperEngineSink::OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBR_GR_SystemMessage));
	if(wDataSize!=sizeof(DBR_GR_SystemMessage)) return false;

	//��ȡ����
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

	//�������
	tagSystemMessage  *pSendMessage=new tagSystemMessage;
	ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

	//���ñ���
	pSendMessage->dwLastTime=(DWORD)pSystemMessage->tStartTime;
	CopyMemory(&pSendMessage->SystemMessage,pSystemMessage, sizeof(DBR_GR_SystemMessage));

	//��¼��Ϣ
	m_SystemMessageActive.Add(pSendMessage);

	return true;
}

//�������
bool CAttemperEngineSink::OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//������ʱ��Ϣ
	if(m_SystemMessageBuffer.GetCount()>0)
	{
		//��������
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

//�������д�
bool CAttemperEngineSink::OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ʼ����
	if(dwContextID==0xfffe)
	{
		m_WordsFilter.ResetSensitiveWordArray();
		m_pITimerEngine->KillTimer(IDI_LOAD_SENSITIVE_WORD);
		return true;			
	}

	//�������
	if(dwContextID==0xffff)
	{
		m_WordsFilter.FinishAdd();
		return true;
	}

	//�������д�
	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.AddSensitiveWords(pWords);
	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			//ע�����
			if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
			{
				//����ṹ
				CMD_CS_C_RegisterMatch RegitsterMatch;
				ZeroMemory(&RegitsterMatch,sizeof(RegitsterMatch));

				//���ýṹ
				RegitsterMatch.wServerID = m_pGameServiceOption->wServerID;
				RegitsterMatch.dwMatchID =  m_pGameMatchOption->dwMatchID;
				RegitsterMatch.dwMatchNO =  m_pGameMatchOption->dwMatchNO;
				RegitsterMatch.cbMatchType = m_pGameMatchOption->cbMatchType;
				RegitsterMatch.lMatchFee =  m_pGameMatchOption->lMatchFee;
				RegitsterMatch.cbMatchFeeType =  m_pGameMatchOption->cbMatchFeeType;
				RegitsterMatch.cbMemberOrder =  m_pGameMatchOption->cbMemberOrder;
				RegitsterMatch.wRewardCount = m_pGameMatchOption->wRewardCount;

				//������Ϣ
				lstrcpyn(RegitsterMatch.szMatchName,m_pGameMatchOption->szMatchName,CountArray(RegitsterMatch.szMatchName));

				//��ʱ����
				if(m_pGameMatchOption->cbMatchType==MATCH_TYPE_LOCKTIME)
				{	
					RegitsterMatch.MatchEndTime = m_pGameMatchOption->MatchType.LockTime.MatchEndTime;
					RegitsterMatch.MatchStartTime = m_pGameMatchOption->MatchType.LockTime.MatchStartTime;					
					RegitsterMatch.wMatchPlayCount = m_pGameMatchOption->MatchType.LockTime.wMatchPlayCount;
				}

				//��ʱ����
				if(m_pGameMatchOption->cbMatchType==MATCH_TYPE_IMMEDIATE)
				{
					RegitsterMatch.wMatchPlayCount = m_pGameMatchOption->MatchType.Immediate.cbPlayCount;
					RegitsterMatch.wStartUserCount = m_pGameMatchOption->MatchType.Immediate.wStartUserCount;				
				}				

				//��������
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_MATCH,&RegitsterMatch,sizeof(RegitsterMatch));
			}

			return true;
		}
	case SUB_CS_S_SERVER_ONLINE:	//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerOnLine));
			if (wDataSize!=sizeof(CMD_CS_S_ServerOnLine)) return false;

			//��������
			CMD_CS_S_ServerOnLine * pServerOnLine=(CMD_CS_S_ServerOnLine *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerOnLine->wServerID);

			//��������
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount=pServerOnLine->dwOnLineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);
			}

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_S_ServerModify)) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);

			//���÷���
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
	case SUB_CS_S_SERVER_REMOVE:	//����ɾ��
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_S_ServerRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServerRemove)) return false;

			//��������
			CMD_CS_S_ServerRemove * pServerRemove=(CMD_CS_S_ServerRemove *)pData;

			//��������
			m_ServerListManager.DeleteGameServer(pServerRemove->wServerID);

			return true;
		}
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//�û�����
		{
			//��������
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//�����û�
			WORD wIndex=0;
			do
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//���ñ���
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				//UserEnter.dwGameID=pIServerUserItem->GetGameID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//������Ϣ
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

				//��������
				ASSERT(m_pITCPSocketService!=NULL);
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));

			} while (true);

			//�㱨���
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SYSTEM_MESSAGE:	//ϵͳ��Ϣ
		{
			//��Ϣ����
			SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);

			return true;
		}
	//case SUB_CS_S_PROPERTY_TRUMPET:  //������Ϣ
	//	{
	//		//��������
	//		//m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_PROPERTY_TRUMPET,pData,wDataSize,BG_COMPUTER);

	//		return true;
	//	}
	//case SUB_CS_S_PLATFORM_PARAMETER: //ƽ̨����
	//	{
	//		//�������			
	//		m_wTaskCount=0;
	//		ZeroMemory(m_TaskParameter,sizeof(m_TaskParameter));

	//		//��������
	//		m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_TASK_LOAD_LIST,0L,NULL,0L);

	//		return true;
	//	}
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_ADDPARAMETER:		//��Ӳ���
		{
			//����У��
			ASSERT(sizeof(CMD_CS_S_AddParameter)==wDataSize);
			if(sizeof(CMD_CS_S_AddParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_S_AddParameter * pAddParameter = (CMD_CS_S_AddParameter *)pData;

			//��Ӳ���
			m_AndroidUserManager.AddAndroidParameter(&pAddParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_MODIFYPARAMETER:  //�޸Ĳ���
		{
			//����У��
			ASSERT(sizeof(CMD_CS_S_ModifyParameter)==wDataSize);
			if(sizeof(CMD_CS_S_ModifyParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_S_ModifyParameter * pModifyParameter = (CMD_CS_S_ModifyParameter *)pData;

			//�޸Ĳ���
			m_AndroidUserManager.AddAndroidParameter(&pModifyParameter->AndroidParameter,1);

			return true;
		}
	case SUB_CS_S_DELETEPARAMETER:  //ɾ������
		{
			//����У��
			ASSERT(sizeof(CMD_CS_S_DeleteParameter)==wDataSize);
			if(sizeof(CMD_CS_S_DeleteParameter)!=wDataSize) return false;

			//��ȡ����
			CMD_CS_S_DeleteParameter * pDeleteParameter = (CMD_CS_S_DeleteParameter *)pData;

			//ɾ������
			m_AndroidUserManager.RemoveAndroidParameter(pDeleteParameter->dwBatchID);

			return true;
		}
	}

	return true;
}

//��վ����
bool CAttemperEngineSink::OnTCPSocketMainWebService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_BUY_PROP_FROM_WEB:
	{
		//����У��
		ASSERT(sizeof(CMD_CS_S_BuyPropFromWeb) == wDataSize);
		if (sizeof(CMD_CS_S_BuyPropFromWeb) != wDataSize) return false;

		//��ȡ����
		CMD_CS_S_BuyPropFromWeb * pBuyProp = (CMD_CS_S_BuyPropFromWeb *)pData;

		//��ѯ���
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

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (SUB_GR_LOGON_USERID == wSubCmdID)		//I D ��¼
	{
		return OnTCPNetworkSubLogonUserID(pData,wDataSize,dwSocketID);
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//�û�����
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_LOOKON:		//�û��Թ�
		{
			return OnTCPNetworkSubUserLookon(pData, wDataSize, dwSocketID);
		}
	case SUB_GR_USER_SITDOWN:		//�û�����
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_STANDUP:		//�û�����
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAT:			//�û�����
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_EXPRESSION:	//�û�����
		{
			return OnTCPNetworkSubUserExpression(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_CHAT:		//�û�˽��
		{
			return OnTCPNetworkSubWisperChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_WISPER_EXPRESSION:	//˽�ı���
		{
			return OnTCPNetworkSubWisperExpression(pData,wDataSize,dwSocketID);
		}
	//case SUB_GR_PROPERTY_BUY:		//�������
	//	{
	//		return OnTCPNetworkSubPropertyBuy(pData,wDataSize,dwSocketID);
	//	}
	//case SUB_GR_PROPERTY_TRUMPET:   //���ȵ���
	//	{
	//		return OnTCPNetwordSubSendTrumpet(pData,wDataSize,dwSocketID);
	//	}
	case SUB_GR_USER_INVITE_REQ:    //�����û�
		{
			return OnTCPNetworkSubUserInviteReq(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_REPULSE_SIT:   //�ܾ�����
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_KICK_USER:    //�߳��û�
		{
			return OnTCPNetworkSubMemberKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //�������λ��
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//���д���
bool CAttemperEngineSink::OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_ENABLE_INSURE_REQUEST:	//��ͨ����
		{
			return OnTCPNetworkSubEnableInsureRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_INSURE_INFO:		//���в�ѯ
		{
			return OnTCPNetworkSubQueryInsureInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SAVE_SCORE_REQUEST:		//�������
		{
			return OnTCPNetworkSubSaveScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TAKE_SCORE_REQUEST:		//ȡ������
		{
			return OnTCPNetworkSubTakeScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TRANSFER_SCORE_REQUEST:	//ת������
		{
			return OnTCPNetworkSubTransferScoreRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_USER_INFO_REQUEST://��ѯ�û�
		{
			return OnTCPNetworkSubQueryUserInfoRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_TASK_TAKE:				//��ȡ����
		{
			return OnTCPNetworkSubTakeTaskRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_REWARD:			//��ȡ����
		{
			return OnTCPNetworkSubTaskRewardRequest(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_TASK_LOAD_INFO:			//��������
		{
			return OnTCPNetworkSubLoadTaskInfoRequest(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//�һ�����
bool CAttemperEngineSink::OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_EXCHANGE_LOAD_INFO:	//��ѯ��Ϣ
		{
			return OnTCPNetworkSubQueryExchangeInfo(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_EXCHANGE_SCORE:		//�һ���Ϸ��
		{
			return OnTCPNetworkSubExchangeScore(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_QUERY_OPTION:		//��ѯ����
		{
			return OnTCPNetworkSubQueryOption(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_OPTION_SERVER:		//��������
		{
			return OnTCPNetworkSubOptionServer(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KILL_USER:          //�߳��û�
		{
			return OnTCPNetworkSubManagerKickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_LIMIT_USER_CHAT:	//��������
		{
			return OnTCPNetworkSubLimitUserChat(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_KICK_ALL_USER:		//�߳��û�
		{
			return OnTCPNetworkSubKickAllUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_MESSAGE:		//������Ϣ
		{
			return OnTCPNetworkSubSendMessage(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_DISMISSGAME:        //��ɢ��Ϸ
		{
			return OnTCPNetworkSubDismissGame(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_SEND_WARNING:		//�����û�
		{
			return OnTCPNetworkSubWarningUser(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//Ч��ӿ�
	ASSERT(m_pIMatchServiceManager!=NULL);
	if (m_pIMatchServiceManager==NULL) return false;

	//��Ϣ����
	return m_pIMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

//��Ϸ����
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��ܴ���
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonUserID));
	if (wDataSize<sizeof(CMD_GR_LogonUserID)) return false;

	//������Ϣ
	CMD_GR_LogonUserID * pLogonUserID=(CMD_GR_LogonUserID *)pData;
	pLogonUserID->szPassword[CountArray(pLogonUserID->szPassword)-1]=0;
	pLogonUserID->szMachineID[CountArray(pLogonUserID->szMachineID)-1]=0;

	//����Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIBindUserItem=GetBindUserItem(wBindIndex);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//�ظ��ж�
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		ASSERT(FALSE);
		return false;
	}

	////����У��
	//if((m_pGameServiceOption->wServerKind&SERVER_GENRE_PASSWD)!=0)
	//{
	//	//����Ƚ�
	//	if(lstrcmp(pLogonUserID->szServerPasswd,m_pGameServiceOption->szServerPasswd)!=0)
	//	{
	//		//����ʧ��
	//		SendLogonFailure(TEXT("��Ǹ��������ķ������벻��ȷ�����������룡"),0,dwSocketID);

	//		return true;
	//	}
	//}

	////�����ж�
	//if(pLogonUserID->wKindID != m_pGameServiceOption->wKindID)
	//{
	//	//����ʧ��
	//	SendLogonFailure(TEXT("�ܱ�Ǹ������Ϸ�����Ѿ��ر��ˣ�������������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
	//	return true;
	//}

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);
	if (pIServerUserItem!=NULL)
	{
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			SendRoomMessage(dwSocketID, TEXT("���ʺ����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��"), SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
			return true;
		}
	}

	//�����ж�
	CString strPassword = pLogonUserID->szPassword;
	strPassword.Trim();
	if(pBindParameter->dwClientAddr!=0L && strPassword.GetLength()==0)
	{
		//����ʧ��
		SendLogonFailure(TEXT("�ܱ�Ǹ�����ĵ�¼������󣬲�����������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}

	//�汾��Ϣ
	pBindParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pBindParameter->dwPlazaVersion=pLogonUserID->dwPlazaVersion;

	//if(LOWORD(dwSocketID) < INDEX_ANDROID)
	//{
	//	//�����汾
	//	DWORD dwPlazaVersion=pLogonUserID->dwPlazaVersion;
	//	DWORD dwFrameVersion=pLogonUserID->dwFrameVersion;
	//	DWORD dwClientVersion=pLogonUserID->dwProcessVersion;
	//	if (PerformCheckVersion(dwPlazaVersion,dwFrameVersion,dwClientVersion,dwSocketID)==false) return true;
	//}

	//�л��ж�
	if((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastLogonPass(pLogonUserID->szPassword)==true))
	{
		SwitchUserItemConnect(pIServerUserItem, pLogonUserID->szMachineID, dwSocketID);
		return true;
	}

	//����Ч��
	if(m_pIMatchServiceManager!=NULL&&m_pIMatchServiceManager->OnEventEnterMatch(dwSocketID, pData, pBindParameter->dwClientAddr,false))
	{
		//SendRoomMessage(dwSocketID, szPrint, SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM,(pBindParameter->dwClientAddr==0L));
		return true;
	}

	//��������
	DBR_GR_LogonUserID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID=pLogonUserID->dwUserID;
	LogonUserID.dwMatchID=m_pGameMatchOption->dwMatchID;
	LogonUserID.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID,DBR_GR_LOGON_USERID,dwSocketID,&LogonUserID,sizeof(LogonUserID));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true||m_pGameServiceOption->wServerType==GAME_GENRE_MATCH) return true;

	//��Ϣ����
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//�����־
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//��������
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//��������
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//��������
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//��ȡ����
		while (true)
		{
			//��ȡ����
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//���ݷ���
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//��������
				{
					//Ч������
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//��������
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
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UserLookon));
	if (wDataSize != sizeof(CMD_GR_UserLookon)) return false;

	//Ч������
	CMD_GR_UserLookon * pUserLookon = (CMD_GR_UserLookon *)pData;
	//if (pUserLookon->wChairID >= m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID >= (WORD)m_TableFrameArray.GetCount()) return false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem = GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//��Ϣ����
	WORD wTableID = pIServerUserItem->GetTableID();
	//�����Լ�ԭ�����Ӻţ����͸��ͻ�����������ͼת��
	WORD wChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
	if ((wTableID == pUserLookon->wTableID) && (cbUserStatus == US_LOOKON)) return true;

	//�û��ж�
	if (cbUserStatus == US_PLAYING)
	{
		//SendRequestFailure(pIServerUserItem, TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"), 0);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//��������������뿪����
	if (wTableID != INVALID_TABLE)
	{
		CTableFrame * pTableFrame = m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem) == false) return true;
	}

	//���´���
	CTableFrame * pTableFrame = m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(wChairID, pIServerUserItem);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//Ч������
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//�ظ��ж�
	if ((pUserSitDown->wTableID<m_pGameServiceOption->wTableCount)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		//SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//�������
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//�������
		InsertDistribute(pIServerUserItem);	
		
		return true;
	}

	bool isLookOn = false;
	WORD oldTableId = pIServerUserItem->GetTableID();
	WORD oldChairID = pIServerUserItem->GetChairID();

	//��ұ�������Թ�״̬������ʧ�ܺ�ָ��Թ�
	if (wTableID != INVALID_TABLE && pIServerUserItem->GetUserStatus() == US_LOOKON)
	{
		isLookOn = true;
	}

	////�뿪����
	//if (wTableID!=INVALID_TABLE)
	//{
	//	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	//	if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	//}
	//
	////������(��ʱע��)
	//if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	//{
	//	if(m_TableFrameArray[0]->EfficacyEnterTableScoreRule(0, pIServerUserItem))
	//	{
	//		InsertDistribute(pIServerUserItem);
	//	}
	//
	//	return true;
	//}

	//�������
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;
	
	// ���Ӻź����ӺŶ�Ϊ��ʼֵʱ�����������������λ
	if (wRequestTableID == INVALID_TABLE && wRequestChairID == INVALID_CHAIR)
	{
		UserImmediately(pIServerUserItem);
		return true;
	}

	//���ӵ���
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//��ʼ����
		WORD wStartTableID=0;
		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
		if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//��̬����
		bool bDynamicJoin=true;
		if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
		if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		//Ѱ��λ��
		for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
		{
			//��Ϸ״̬
			if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

			//��ȡ��λ
			WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

			//�������
			if (wNullChairID!=INVALID_CHAIR)
			{
				//���ñ���
				wRequestTableID=i;
				wRequestChairID=wNullChairID;

				break;
			}
		}

		//����ж�
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			//SendRequestFailure(pIServerUserItem,TEXT("��ǰ��Ϸ�����Ѿ�����Ϊ���ˣ���ʱû�п������������λ�ã����Ժ����ԣ�"),0);
			SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_FULL_PEOPLE);
			return true;
		}
	}

	//���ӵ���
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//Ч�����
		ASSERT(wRequestTableID<m_TableFrameArray.GetCount());
		if (wRequestTableID>=m_TableFrameArray.GetCount()) return false;

		//���ҿ�λ
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//����ж�
		if (wRequestChairID==INVALID_CHAIR)
		{
			//SendRequestFailure(pIServerUserItem,TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"),0);
			SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_CHAIR);
			return true;
		}
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	
	//�뿪����
	if (wTableID != INVALID_TABLE)
	{
		if (pTableFrame->PerformStandUpAction(pIServerUserItem) == false) return true;
	}

	//����ʧ�ܣ��ָ��Թ�
	if (pTableFrame->PerformSitDownAction(wRequestChairID, pIServerUserItem, pUserSitDown->szPassword)== false && isLookOn)
	{

		CTableFrame * pOldTableFrame = m_TableFrameArray[oldTableId];
		pOldTableFrame->PerformRecoerLookOn(oldChairID, pIServerUserItem);
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;

	//ȡ������
	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	{
		//ȡ������
		DeleteDistribute(pIServerUserItem);

		if(pUserStandUp->wTableID==INVALID_TABLE) return true;
	}

	//Ч������
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//�û��ж�
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		//SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_GR_C_UserChat * pUserChat=(CMD_GR_C_UserChat *)pData;

	//Ч�����
	ASSERT(wDataSize<=sizeof(CMD_GR_C_UserChat));
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0])));

	//Ч�����
	if (wDataSize>sizeof(CMD_GR_C_UserChat)) return false;
	if (wDataSize<(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(pUserChat->szChatString[0]))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pUserChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//������Ϣ
	CMD_GR_S_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));

	//�ַ�����
	SensitiveWordFilter(pUserChat->szChatString,UserChat.szChatString,CountArray(UserChat.szChatString));
	
	//��������
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pUserChat->dwSendUserID;
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(UserChat.szChatString[0]));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_UserExpression));
	if (wDataSize!=sizeof(CMD_GR_C_UserExpression)) return false;

	//��������
	CMD_GR_C_UserExpression * pUserExpression=(CMD_GR_C_UserExpression *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserExpression->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserExpression->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�д������Ե�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pUserExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//������Ϣ
	CMD_GR_S_UserExpression UserExpression;
	ZeroMemory(&UserExpression,sizeof(UserExpression));

	//��������
	UserExpression.wItemIndex=pUserExpression->wItemIndex;
	UserExpression.dwSendUserID=pUserExpression->dwSendUserID;
	UserExpression.dwTargetUserID=pUserExpression->dwTargetUserID;

	//ת����Ϣ
	SendData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_EXPRESSION,&UserExpression,sizeof(UserExpression));

	return true;
}

//�û�˽��
bool CAttemperEngineSink::OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_GR_C_WisperChat * pWisperChat=(CMD_GR_C_WisperChat *)pData;

	//Ч�����
	ASSERT(wDataSize>=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0])));

	//Ч�����
	if (wDataSize<(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_C_WisperChat)-sizeof(pWisperChat->szChatString)+pWisperChat->wChatLength*sizeof(pWisperChat->szChatString[0]))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperChat->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//ͬ���ж�
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//��������
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//�����ж�
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//��ʾ��Ϣ
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pWisperChat->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//��������
	CMD_GR_S_WisperChat WisperChat;
	ZeroMemory(&WisperChat,sizeof(WisperChat));

	//�ַ�����
	SensitiveWordFilter(pWisperChat->szChatString,WisperChat.szChatString,CountArray(WisperChat.szChatString));

	//��������
	WisperChat.dwChatColor=pWisperChat->dwChatColor;
	WisperChat.wChatLength=pWisperChat->wChatLength;
	WisperChat.dwSendUserID=pISendUserItem->GetUserID();
	WisperChat.dwTargetUserID=pIRecvUserItem->GetUserID();
	WisperChat.wChatLength=CountStringBuffer(WisperChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(WisperChat)-sizeof(WisperChat.szChatString);
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//ת����Ϣ
	WisperChat.dwSendUserID=pWisperChat->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_CHAT,&WisperChat,wHeadSize+WisperChat.wChatLength*sizeof(WisperChat.szChatString[0]));

	//�����ж�
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//ö���û�
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

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_WisperExpression));
	if (wDataSize!=sizeof(CMD_GR_C_WisperExpression)) return false;

	//��������
	CMD_GR_C_WisperExpression * pWisperExpression=(CMD_GR_C_WisperExpression *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pWisperExpression->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	//ͬ���ж�
	if ((CServerRule::IsForfendWisperOnGame(m_pGameServiceOption->dwServerRule)==true)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//��������
		bool bForfend=true;
		WORD wTableIDSend=pISendUserItem->GetTableID();
		WORD wTableIDRecv=pIRecvUserItem->GetTableID();

		//�����ж�
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetMasterOrder()!=0)) bForfend=false;
		if ((bForfend==true)&&(pISendUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&(pIRecvUserItem->GetUserStatus()!=US_PLAYING)) bForfend=false;
		if ((bForfend==true)&&((wTableIDSend==INVALID_TABLE)||(wTableIDSend!=wTableIDRecv))) bForfend=false;

		//��ʾ��Ϣ
		if (bForfend==true)
		{
			SendRoomMessage(pISendUserItem,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�"),SMT_EJECT|SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��"),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//Ȩ���ж�
	if (pWisperExpression->dwSendUserID!=pISendUserItem->GetUserID() && (pISendUserItem->GetMasterOrder()==0 ||
		CMasterRight::CanManagerAndroid(pISendUserItem->GetMasterRight())==false))
	{
		return false;
	}

	//��������
	CMD_GR_S_WisperExpression WisperExpression;
	ZeroMemory(&WisperExpression,sizeof(WisperExpression));

	//��������
	WisperExpression.wItemIndex=pWisperExpression->wItemIndex;
	WisperExpression.dwSendUserID=pISendUserItem->GetUserID();
	WisperExpression.dwTargetUserID=pWisperExpression->dwTargetUserID;

	//ת����Ϣ
	SendData(pISendUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//ת����Ϣ
	WisperExpression.dwSendUserID=pWisperExpression->dwSendUserID;
	SendData(pIRecvUserItem,MDM_GR_USER,SUB_GR_WISPER_EXPRESSION,&WisperExpression,sizeof(WisperExpression));

	//�����ж�
	if(pIRecvUserItem->IsAndroidUser()==true)
	{
		//ö���û�
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

////�������
//bool CAttemperEngineSink::OnTCPNetworkSubPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
//{
//	//�¼�����
//	int cbResult = OnPropertyBuy(pData, wDataSize, dwSocketID);
//
//	//����ж�
//	if(cbResult == RESULT_ERROR) return false;
//	if(cbResult == RESULT_FAIL) return true;
//	if(cbResult == RESULT_SUCCESS) return true;
//
//	return true;
//}

////�������
//int CAttemperEngineSink::OnPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID)
//{
//	//Ч�����
//	ASSERT(wDataSize==sizeof(CMD_GR_C_PropertyBuy));
//	if (wDataSize!=sizeof(CMD_GR_C_PropertyBuy)) return RESULT_ERROR;
//
//	//��������
//	CMD_GR_C_PropertyBuy * pPropertyBuy=(CMD_GR_C_PropertyBuy *)pData;
//
//	//����Ч��
//	ASSERT(pPropertyBuy->wItemCount>0);
//	if (pPropertyBuy->wItemCount==0) return RESULT_ERROR;
//
//	//��ȡ�û�
//	WORD wBindIndex=LOWORD(dwSocketID);
//	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
//
//	//�û�Ч��
//	ASSERT(pIServerUserItem!=NULL);
//	if (pIServerUserItem==NULL) return RESULT_ERROR;
//
//	//Ŀ���û�
//	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pPropertyBuy->dwTargetUserID);
//	if (pITargerUserItem==NULL) return RESULT_ERROR;
//
//	//��������
//	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
//	{
//		//������Ϣ
//		SendPropertyFailure(pIServerUserItem,TEXT("�������䲻����ʹ�ô˹��ܣ�"),0L,pPropertyBuy->cbRequestArea);
//
//		return RESULT_FAIL;
//	}
//
//	//��ϰ����
//	if (m_pGameServiceOption->wServerType==GAME_GENRE_EDUCATE)
//	{
//		SendPropertyFailure(pIServerUserItem,TEXT("��ϰ���䲻����ʹ�ô˹��ܣ�"),0L,pPropertyBuy->cbRequestArea);
//		return RESULT_FAIL;
//	}
//
//	//��ǰ�¼�
//	if(OnEventPropertyBuyPrep(pPropertyBuy->cbRequestArea,pPropertyBuy->wPropertyIndex,pIServerUserItem,pITargerUserItem)==false)
//		return RESULT_FAIL;
//
//	//��������
//	DBR_GR_PropertyRequest PropertyRequest;
//	ZeroMemory(&PropertyRequest,sizeof(PropertyRequest));
//
//	//���ҵ���
//	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(pPropertyBuy->wPropertyIndex);
//	if (pPropertyInfo==NULL)
//	{
//		SendPropertyFailure(pIServerUserItem,TEXT("������ĵ��߲����ڻ���ά���У��������Ա��ϵ��"),0L,pPropertyBuy->cbRequestArea);
//		return RESULT_FAIL;
//	}
//
//	//���ѷ�ʽ
//	if (pPropertyBuy->cbConsumeScore==TRUE)
//	{
//		//�����ж�
//		ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
//		if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return RESULT_FAIL;		
//
//		//��������
//		PropertyRequest.cbConsumeScore=TRUE;
//		PropertyRequest.lFrozenedScore=pPropertyInfo->lPropertyGold*pPropertyBuy->wItemCount;
//
//		//��Ա�ۿ�
//		if (pIServerUserItem->GetMemberOrder()>0)
//		{
//			PropertyRequest.lFrozenedScore = PropertyRequest.lFrozenedScore*pPropertyInfo->wDiscount/100L;
//		}
//
//		//��������
//		if (pIServerUserItem->FrozenedUserScore(PropertyRequest.lFrozenedScore)==false)
//		{
//			SendPropertyFailure(pIServerUserItem,TEXT("������Ϸ�����㣬���߹���ʧ�ܣ�"),0L,pPropertyBuy->cbRequestArea);
//			return RESULT_FAIL;
//		}
//	}
//	else
//	{
//		//���п۷�
//		PropertyRequest.lFrozenedScore=0L;
//		PropertyRequest.cbConsumeScore=FALSE;
//
//		//��������
//		SCORE lInsure = pIServerUserItem->GetUserInsure();
//		SCORE lConsumeScore = pPropertyInfo->lPropertyGold*pPropertyBuy->wItemCount;
//
//		//��Ա�ۿ�
//		if (pIServerUserItem->GetMemberOrder()>0)
//		{
//			lConsumeScore = lConsumeScore*pPropertyInfo->wDiscount/100L;
//		}
//
//		//����У��
//		if(lInsure < lConsumeScore)
//		{
//			SendPropertyFailure(pIServerUserItem,TEXT("�����������㣬������ٴι���"),0L,pPropertyBuy->cbRequestArea);
//			return RESULT_FAIL;
//		}		
//	}
//
//	//������Ϣ
//	PropertyRequest.cbRequestArea=pPropertyBuy->cbRequestArea;
//	PropertyRequest.wItemCount=pPropertyBuy->wItemCount;
//	PropertyRequest.wPropertyIndex=pPropertyBuy->wPropertyIndex;
//	PropertyRequest.dwSourceUserID=pIServerUserItem->GetUserID();
//	PropertyRequest.dwTargetUserID=pITargerUserItem->GetUserID();
//	PropertyRequest.dwUserRight = 0;
//
//	//ϵͳ��Ϣ
//	PropertyRequest.wTableID=INVALID_TABLE;
//	PropertyRequest.dwInoutIndex=pIServerUserItem->GetInoutIndex();
//	PropertyRequest.dwClientAddr=pIServerUserItem->GetClientAddr();
//	lstrcpyn(PropertyRequest.szMachineID,pIServerUserItem->GetMachineID(),CountArray(PropertyRequest.szMachineID));
//
//	//Ͷ������
//	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_PROPERTY_REQUEST,dwSocketID,&PropertyRequest,sizeof(PropertyRequest));
//
//	return RESULT_SUCCESS;
//}
//
////ʹ�õ���
//bool CAttemperEngineSink::OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID)
//{
//	//Ч�����
//	ASSERT(wDataSize==sizeof(CMD_GR_C_SendTrumpet));
//	if (wDataSize!=sizeof(CMD_GR_C_SendTrumpet)) return false;
//
//	//��������
//	CMD_GR_C_SendTrumpet * pSendTrumpet=(CMD_GR_C_SendTrumpet *)pData;
//
//	//��ȡ�û�
//	WORD wBindIndex=LOWORD(dwSocketID);
//	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
//	if(pIServerUserItem==NULL) return false;
//
//	//����ṹ
//	CMD_GR_C_PropertyBuy  PropertyBuy;
//	PropertyBuy.cbRequestArea=pSendTrumpet->cbRequestArea;
//	PropertyBuy.dwTargetUserID=pIServerUserItem->GetUserID();
//	PropertyBuy.wPropertyIndex=pSendTrumpet->wPropertyIndex;
//	PropertyBuy.cbConsumeScore=FALSE;
//	PropertyBuy.wItemCount=1;
//    
//	//����
//	int cbResult = OnPropertyBuy((void *)&PropertyBuy,sizeof(PropertyBuy),dwSocketID);
//
//	//����ж�
//	if(cbResult == RESULT_ERROR) return false;
//	if(cbResult != RESULT_SUCCESS) return true;
//
//	//��ȡ����
//	tagUserProperty * pUserProperty = pIServerUserItem->GetUserProperty();
//
//	//��������
//	BYTE cbIndex= 2;
//
//	//����ṹ
//	CMD_GR_S_SendTrumpet  SendTrumpet;
//	SendTrumpet.dwSendUserID=pIServerUserItem->GetUserID();
//	SendTrumpet.wPropertyIndex=pSendTrumpet->wPropertyIndex;
//	SendTrumpet.TrumpetColor=pSendTrumpet->TrumpetColor;
//	ZeroMemory(SendTrumpet.szTrumpetContent,sizeof(SendTrumpet.szTrumpetContent));
//	CopyMemory(SendTrumpet.szSendNickName,pIServerUserItem->GetNickName(),sizeof(SendTrumpet.szSendNickName));
//
//	//�ַ�����
//	SensitiveWordFilter(pSendTrumpet->szTrumpetContent,SendTrumpet.szTrumpetContent,CountArray(SendTrumpet.szTrumpetContent));
//
//    //����ת��
//	if(cbIndex==3)
//	{
//		//�㲥����
//		WORD wUserIndex=0;
//		CMD_CS_S_SendTrumpet * pSendTrumpet = (CMD_CS_S_SendTrumpet *)&SendTrumpet;
//		if(m_pITCPSocketService)
//		{
//			m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_PROPERTY_TRUMPET,pSendTrumpet,sizeof(CMD_CS_S_SendTrumpet));
//		}
//	}
//
//	//��Ϸת��
//	if(cbIndex==2)
//	{
//		//��������
//		m_pITCPNetworkEngine->SendDataBatch(MDM_GR_USER,SUB_GR_PROPERTY_TRUMPET,&SendTrumpet,sizeof(SendTrumpet),BG_COMPUTER);
//	}
//
//	return true;
//}

//�����û�
bool CAttemperEngineSink::OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//��Ϣ����
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ч��״̬
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//������Ϣ
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

	return true;
}

//�ܾ�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//��Ϣ����
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//��ȡ�û�
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//������Ϣ
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("��������������˲�����ͬ����Ϸ��"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//�������
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//�߳�����
bool CAttemperEngineSink::OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//Ȩ���ж�
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//���߹���Ա
	if(pITargetUserItem->GetMasterOrder() > 0)
	{
		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�ܱ�Ǹ���߳�����Ա�ǲ�������ģ�"),SMT_EJECT);
		return true;
	}

	//������Ϸ
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�ܱ�Ǹ��������Ϸ�������ˣ�"),SMT_EJECT);
		return true;
	}

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//��������
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("������� [ %s ] ������Ϸ��,�����ܽ����߳���Ϸ��"), pITargetUserItem->GetNickName());

		//������Ϣ
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	////�����ж�
	//if((pITargetUserItem->GetUserProperty()->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
	//{
	//	//��������
	//	DWORD dwCurrentTime=(DWORD)time(NULL);
	//	tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

	//	//ʱЧ�ж�
	//	DWORD dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
	//	if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime>dwCurrentTime)
	//	{
	//		//��������
	//		TCHAR szMessage[256]=TEXT("");
	//		_sntprintf(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ����ʹ�÷��߿�,���޷������߳���Ϸ��"),pITargetUserItem->GetNickName());

	//		//������Ϣ
	//		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);

	//		return true; 
	//	}
	//	else
	//		pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_GUARDKICK_CARD;
	//}

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//�������
		TCHAR szMessage[64]=TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("���ѱ�%s�������ӣ�"), pIServerUserItem->GetNickName());

		//������Ϣ
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//�������λ��
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�״̬
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//ʧ��
		//m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),REQUEST_FAILURE_NORMAL);
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_LEAVE);
		return true;
	}

	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	//��������
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=(i+nStartIndex)%m_pGameServiceOption->wTableCount;

		//����ͬ��
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		////�������
		//WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		//WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		////������Ϣ
		//if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		//{
		//	WORD wNewPos = (nTableIndex/wTagerDeskCount)*wTagerDeskCount;
		//	pIServerUserItem->SetMobileUserDeskPos(wNewPos);
		//}

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//��������
	nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=(i+nStartIndex)%m_pGameServiceOption->wTableCount;

		//����ͬ��
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		////�������
		//WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		//WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		////������Ϣ
		//if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		//{
		//	WORD wNewPos = (nTableIndex/wTagerDeskCount)*wTagerDeskCount;
		//	pIServerUserItem->SetMobileUserDeskPos(wNewPos);
		//}

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//����ͬ��
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//��Ч����
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//�û�����
				pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
				return true;
			}
		}
	}

	//ʧ��
	//m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_TABLE);
	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����ṹ
	CMD_GR_ExchangeParameter ExchangeParameter;
	ZeroMemory(&ExchangeParameter,sizeof(ExchangeParameter));

	//���ñ���
	ExchangeParameter.wMemberCount=m_wMemberCount;
	ExchangeParameter.wExchangeRate=0;	
	CopyMemory(ExchangeParameter.MemberParameter,m_MemberParameter,sizeof(tagMemberParameter)*m_wMemberCount);

	//�����С
	WORD wSendDataSize = sizeof(ExchangeParameter)-sizeof(ExchangeParameter.MemberParameter);
	wSendDataSize += sizeof(tagMemberParameter)*ExchangeParameter.wMemberCount;

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_GR_EXCHANGE,SUB_GR_EXCHANGE_PARAM_INFO,&ExchangeParameter,wSendDataSize);

	return true;
}

//�һ���Ϸ��
bool CAttemperEngineSink::OnTCPNetworkSubExchangeScore(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_ExchangeScore));
	if(wDataSize!=sizeof(CMD_GR_ExchangeScore)) return false;

	//����ṹ
	CMD_GR_ExchangeScore * pExchangeScore = (CMD_GR_ExchangeScore*)pData;
	pExchangeScore->szMachineID[CountArray(pExchangeScore->szMachineID)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_ExchangeScore ExchangeScore;
	ZeroMemory(&ExchangeScore,sizeof(ExchangeScore));

	//���ñ���
	ExchangeScore.dwUserID=pExchangeScore->dwUserID;
	ExchangeScore.lExchangeIngot=pExchangeScore->lExchangeIngot;
	ExchangeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(ExchangeScore.szMachineID,pExchangeScore->szMachineID,CountArray(ExchangeScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(ExchangeScore.dwUserID,DBR_GR_EXCHANGE_SCORE,dwSocketID,&ExchangeScore,sizeof(ExchangeScore));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryInsureInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryInsureInfoRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_C_QueryInsureInfoRequest * pQueryInsureInfoRequest = (CMD_GR_C_QueryInsureInfoRequest *)pData;

	//��������
	DBR_GR_QueryInsureInfo QueryInsureInfo;
	ZeroMemory(&QueryInsureInfo,sizeof(QueryInsureInfo));

	//��������
	QueryInsureInfo.cbActivityGame=pQueryInsureInfoRequest->cbActivityGame;
	QueryInsureInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryInsureInfo.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(QueryInsureInfo.szPassword,pQueryInsureInfoRequest->szInsurePass,CountArray(QueryInsureInfo.szPassword));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(QueryInsureInfo.dwUserID,DBR_GR_QUERY_INSURE_INFO,dwSocketID,&QueryInsureInfo,sizeof(QueryInsureInfo));

	return true;
}

//��ͨ����
bool CAttemperEngineSink::OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_EnableInsureRequest));
	if (wDataSize!=sizeof(CMD_GR_C_EnableInsureRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_C_EnableInsureRequest * pEnableInsureRequest = (CMD_GR_C_EnableInsureRequest *)pData;

	//��������
	DBR_GR_UserEnableInsure UserEnableInsure;
	ZeroMemory(&UserEnableInsure,sizeof(UserEnableInsure));

	//��������	
	UserEnableInsure.dwUserID=pIServerUserItem->GetUserID();
	UserEnableInsure.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserEnableInsure.cbActivityGame=pEnableInsureRequest->cbActivityGame;
	lstrcpyn(UserEnableInsure.szLogonPass,pEnableInsureRequest->szLogonPass,CountArray(UserEnableInsure.szLogonPass));
	lstrcpyn(UserEnableInsure.szInsurePass,pEnableInsureRequest->szInsurePass,CountArray(UserEnableInsure.szInsurePass));
	lstrcpyn(UserEnableInsure.szMachineID,pEnableInsureRequest->szMachineID,CountArray(UserEnableInsure.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(UserEnableInsure.dwUserID,DBR_GR_USER_ENABLE_INSURE,dwSocketID,&UserEnableInsure,sizeof(UserEnableInsure));

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_SaveScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_SaveScoreRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_SaveScoreRequest * pSaveScoreRequest=(CMD_GR_C_SaveScoreRequest *)pData;

	//Ч�����
	ASSERT(pSaveScoreRequest->lSaveScore>0L);
	if (pSaveScoreRequest->lSaveScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	if(pSaveScoreRequest->cbActivityGame == FALSE)
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ������������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//�����ж�
	if(pSaveScoreRequest->cbActivityGame == TRUE)
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸ���������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	SCORE lConsumeQuota=0L;
	SCORE lUserWholeScore=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();

	//��ȡ�޶�
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		lConsumeQuota=m_TableFrameArray[wTableID]->QueryConsumeQuota(pIServerUserItem);
	}
	else
	{
		lConsumeQuota=pIServerUserItem->GetUserScore()+pIServerUserItem->GetTrusteeScore();
	}

	//�޶��ж�
	if (pSaveScoreRequest->lSaveScore>lConsumeQuota)
	{
		if (lConsumeQuota<lUserWholeScore)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("������������Ϸ�У���Ϸ�ҿɴ�����Ϊ %I64d���������ʧ�ܣ�"), lConsumeQuota);

			//��������
			SendInsureFailure(pIServerUserItem,szDescribe,0L,pSaveScoreRequest->cbActivityGame);
		}
		else
		{
			//��������
			SendInsureFailure(pIServerUserItem,TEXT("������Ϸ�����㣬�������ʧ�ܣ�"),0L,pSaveScoreRequest->cbActivityGame);
		}

		return true;
	}

	//��������
	if (pIServerUserItem->FrozenedUserScore(pSaveScoreRequest->lSaveScore)==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	DBR_GR_UserSaveScore UserSaveScore;
	ZeroMemory(&UserSaveScore,sizeof(UserSaveScore));

	//��������
	UserSaveScore.cbActivityGame=pSaveScoreRequest->cbActivityGame;
	UserSaveScore.dwUserID=pIServerUserItem->GetUserID();
	UserSaveScore.lSaveScore=pSaveScoreRequest->lSaveScore;
	UserSaveScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserSaveScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserSaveScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_SAVE_SCORE,dwSocketID,&UserSaveScore,sizeof(UserSaveScore));

	return true;
}

//ȡ������
bool CAttemperEngineSink::OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeScoreRequest));
	if (wDataSize!=sizeof(CMD_GR_C_TakeScoreRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_TakeScoreRequest * pTakeScoreRequest=(CMD_GR_C_TakeScoreRequest *)pData;
	pTakeScoreRequest->szInsurePass[CountArray(pTakeScoreRequest->szInsurePass)-1]=0;

	//Ч�����
	ASSERT(pTakeScoreRequest->lTakeScore>0L);
	if (pTakeScoreRequest->lTakeScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	if(pTakeScoreRequest->cbActivityGame == FALSE )
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ����ȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//�����ж�
	if(pTakeScoreRequest->cbActivityGame == TRUE )
	{
		//��������
		SendInsureFailure(pIServerUserItem,TEXT("�˷����ֹ��Ϸȡ�ȡ������ʧ�ܣ�"),0L,pTakeScoreRequest->cbActivityGame);
		return true;
	}

	//��������
	DBR_GR_UserTakeScore UserTakeScore;
	ZeroMemory(&UserTakeScore,sizeof(UserTakeScore));

	//��������
	UserTakeScore.cbActivityGame=pTakeScoreRequest->cbActivityGame;
	UserTakeScore.dwUserID=pIServerUserItem->GetUserID();
	UserTakeScore.lTakeScore=pTakeScoreRequest->lTakeScore;
	UserTakeScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(UserTakeScore.szPassword,pTakeScoreRequest->szInsurePass,CountArray(UserTakeScore.szPassword));
	lstrcpyn(UserTakeScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTakeScore.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TAKE_SCORE,dwSocketID,&UserTakeScore,sizeof(UserTakeScore));

	return true;
}

//ת������
bool CAttemperEngineSink::OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_C_TransferScoreRequest));
	if (wDataSize!=sizeof(CMD_GP_C_TransferScoreRequest)) return false;

	//�����ж�
	ASSERT(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GP_C_TransferScoreRequest * pTransferScoreRequest=(CMD_GP_C_TransferScoreRequest *)pData;
	pTransferScoreRequest->szAccounts[CountArray(pTransferScoreRequest->szAccounts)-1]=0;
	pTransferScoreRequest->szInsurePass[CountArray(pTransferScoreRequest->szInsurePass)-1]=0;

	//Ч�����
	ASSERT(pTransferScoreRequest->lTransferScore>0L);
	if (pTransferScoreRequest->lTransferScore<=0L) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	DBR_GR_UserTransferScore UserTransferScore;
	ZeroMemory(&UserTransferScore,sizeof(UserTransferScore));

	//��������
	UserTransferScore.cbActivityGame=pTransferScoreRequest->cbActivityGame;
	UserTransferScore.dwUserID=pIServerUserItem->GetUserID();
	UserTransferScore.dwClientAddr=pIServerUserItem->GetClientAddr();
	UserTransferScore.lTransferScore=pTransferScoreRequest->lTransferScore;
	lstrcpyn(UserTransferScore.szAccounts,pTransferScoreRequest->szAccounts,CountArray(UserTransferScore.szAccounts));
	lstrcpyn(UserTransferScore.szMachineID,pIServerUserItem->GetMachineID(),CountArray(UserTransferScore.szMachineID));
	lstrcpyn(UserTransferScore.szPassword,pTransferScoreRequest->szInsurePass,CountArray(UserTransferScore.szPassword));
	lstrcpyn(UserTransferScore.szTransRemark,pTransferScoreRequest->szTransRemark,CountArray(UserTransferScore.szTransRemark));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_USER_TRANSFER_SCORE,dwSocketID,&UserTransferScore,sizeof(UserTransferScore));

	return true;
}

//��ѯ�û�����
bool CAttemperEngineSink::OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_C_QueryUserInfoRequest));
	if (wDataSize!=sizeof(CMD_GR_C_QueryUserInfoRequest)) return false;

	//�����ж�
	ASSERT((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0);
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0) return false;

	//��������
	CMD_GR_C_QueryUserInfoRequest * pQueryUserInfoRequest=(CMD_GR_C_QueryUserInfoRequest *)pData;
	pQueryUserInfoRequest->szAccounts[CountArray(pQueryUserInfoRequest->szAccounts)-1]=0;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//ID�ж�
	if(pQueryUserInfoRequest->cbByNickName==FALSE)
	{
		//�����ж�
		int nLen=lstrlen(pQueryUserInfoRequest->szAccounts);
		if(nLen>=8)
		{
			SendInsureFailure(pIServerUserItem,TEXT("������Ϸ������ID��"), 0, pQueryUserInfoRequest->cbActivityGame);
			return true;
		}

		//�Ϸ��ж�
		for(int i=0; i<nLen; i++)
		{
			if(pQueryUserInfoRequest->szAccounts[i] < TEXT('0') || pQueryUserInfoRequest->szAccounts[i] > TEXT('9'))
			{
				SendInsureFailure(pIServerUserItem,TEXT("������Ϸ������ID��"), 0, pQueryUserInfoRequest->cbActivityGame);
				return true;
			}
		}
	}

	//��������
	DBR_GR_QueryTransferUserInfo QueryTransferUserInfo;
	ZeroMemory(&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	//��������	
	QueryTransferUserInfo.dwUserID=pIServerUserItem->GetUserID();
	QueryTransferUserInfo.cbByNickName=pQueryUserInfoRequest->cbByNickName;
	QueryTransferUserInfo.cbActivityGame=pQueryUserInfoRequest->cbActivityGame;	
	lstrcpyn(QueryTransferUserInfo.szAccounts,pQueryUserInfoRequest->szAccounts,CountArray(QueryTransferUserInfo.szAccounts));	

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_QUERY_TRANSFER_USER_INFO,dwSocketID,&QueryTransferUserInfo,sizeof(QueryTransferUserInfo));

	return true;
}

//��ȡ����
bool CAttemperEngineSink::OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_TakeTask));
	if(wDataSize!=sizeof(CMD_GR_C_TakeTask)) return false;

	//��ȡ����
	CMD_GR_C_TakeTask * pTaskTake = (CMD_GR_C_TakeTask *)pData;
	pTaskTake->szPassword[CountArray(pTaskTake->szPassword)-1]=0;
	pTaskTake->szMachineID[CountArray(pTaskTake->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskTake->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskTake TaskTake;
	TaskTake.dwUserID = pTaskTake->dwUserID;
	TaskTake.wTaskID = pTaskTake->wTaskID;
	TaskTake.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskTake.szPassword,pTaskTake->szPassword,CountArray(TaskTake.szPassword));
	lstrcpyn(TaskTake.szMachineID,pTaskTake->szMachineID,CountArray(TaskTake.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_TAKE,dwSocketID,&TaskTake,sizeof(TaskTake));

	return true;
}

//��ȡ����
bool CAttemperEngineSink::OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_TaskReward));
	if(wDataSize!=sizeof(CMD_GR_C_TaskReward)) return false;

	//��ȡ����
	CMD_GR_C_TaskReward * pTaskReward = (CMD_GR_C_TaskReward *)pData;
	pTaskReward->szPassword[CountArray(pTaskReward->szPassword)-1]=0;
	pTaskReward->szMachineID[CountArray(pTaskReward->szMachineID)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTaskReward->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskReward TaskReward;
	TaskReward.dwUserID = pTaskReward->dwUserID;
	TaskReward.wTaskID = pTaskReward->wTaskID;
	TaskReward.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(TaskReward.szPassword,pTaskReward->szPassword,CountArray(TaskReward.szPassword));
	lstrcpyn(TaskReward.szMachineID,pTaskReward->szMachineID,CountArray(TaskReward.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_REWARD,dwSocketID,&TaskReward,sizeof(TaskReward));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_C_LoadTaskInfo));
	if(wDataSize!=sizeof(CMD_GR_C_LoadTaskInfo)) return false;

	//��ȡ����
	CMD_GR_C_LoadTaskInfo * pLoadTaskInfo = (CMD_GR_C_LoadTaskInfo *)pData;
	pLoadTaskInfo->szPassword[CountArray(pLoadTaskInfo->szPassword)-1]=0;

	//�����û�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLoadTaskInfo->dwUserID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBR_GR_TaskQueryInfo TaskQueryInfo;
	TaskQueryInfo.dwUserID = pLoadTaskInfo->dwUserID;
	lstrcpyn(TaskQueryInfo.szPassword,pLoadTaskInfo->szPassword,CountArray(TaskQueryInfo.szPassword));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_TASK_QUERY_INFO,dwSocketID,&TaskQueryInfo,sizeof(TaskQueryInfo));

	return true;
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//��������
	CMD_GR_OptionCurrent OptionCurrent;
	ZeroMemory(&OptionCurrent,sizeof(OptionCurrent));

	//�ҽ�����
	OptionCurrent.ServerOptionInfo.wKindID=m_pGameServiceOption->wKindID;
	//OptionCurrent.ServerOptionInfo.wNodeID=m_pGameServiceOption->wNodeID;
	OptionCurrent.ServerOptionInfo.wSortID=m_pGameServiceOption->wSortID;

	//��������
	OptionCurrent.ServerOptionInfo.wRevenueRatio=m_pGameServiceOption->wRevenueRatio;
	OptionCurrent.ServerOptionInfo.lServiceScore=m_pGameServiceOption->lServiceScore;
	OptionCurrent.ServerOptionInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	OptionCurrent.ServerOptionInfo.lMinTableScore=m_pGameServiceOption->lMinTableScore;
	OptionCurrent.ServerOptionInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	OptionCurrent.ServerOptionInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

	//��Ա����
	OptionCurrent.ServerOptionInfo.cbMinEnterMember=m_pGameServiceOption->cbMinEnterMember;
	OptionCurrent.ServerOptionInfo.cbMaxEnterMember=m_pGameServiceOption->cbMaxEnterMember;

	//��������
	OptionCurrent.ServerOptionInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	lstrcpyn(OptionCurrent.ServerOptionInfo.szServerName,m_pGameServiceOption->szServerName,CountArray(OptionCurrent.ServerOptionInfo.szServerName));

	//�������
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_CHAT;
	OptionCurrent.dwRuleMask|=SR_FORFEND_WISPER_ON_GAME;

	//�������
	OptionCurrent.dwRuleMask|=SR_FORFEND_ROOM_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_ENTER;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_LOOKON;

	//��������
	//OptionCurrent.dwRuleMask|=SR_RECORD_GAME_TRACK;
	OptionCurrent.dwRuleMask|=SR_FORFEND_GAME_RULE;
	OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_SIMULATE;

	//�������
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_DYNAMIC_JOIN;
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_ANDROID_ATTEND;
	//if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE) OptionCurrent.dwRuleMask|=SR_ALLOW_OFFLINE_TRUSTEE;

	//ģʽ����
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_RECORD_GAME_SCORE;
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0) OptionCurrent.dwRuleMask|=SR_IMMEDIATE_WRITE_SCORE;

	//��������
	SendData(pIServerUserItem,MDM_GR_MANAGE,SUB_GR_OPTION_CURRENT,&OptionCurrent,sizeof(OptionCurrent));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_ServerOption));
	if (wDataSize!=sizeof(CMD_GR_ServerOption)) return false;

	//��������
	CMD_GR_ServerOption * pServerOption=(CMD_GR_ServerOption *)pData;
	tagServerOptionInfo * pServerOptionInfo=&pServerOption->ServerOptionInfo;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//��������
	bool bModifyServer=false;

	////�ҽӽڵ�
	//if (m_pGameServiceOption->wNodeID!=pServerOptionInfo->wNodeID)
	//{
	//	bModifyServer=true;
	//	m_pGameServiceOption->wNodeID=pServerOptionInfo->wNodeID;
	//}

	//�ҽ�����
	if ((pServerOptionInfo->wKindID!=0)&&(m_pGameServiceOption->wKindID!=pServerOptionInfo->wKindID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wKindID=pServerOptionInfo->wKindID;
	}
	
	//�ҽ�����
	if ((pServerOptionInfo->wSortID!=0)&&(m_pGameServiceOption->wSortID!=pServerOptionInfo->wSortID))
	{
		bModifyServer=true;
		m_pGameServiceOption->wSortID=pServerOptionInfo->wSortID;
	}

	//��������
	if ((pServerOptionInfo->szServerName[0]!=0)&&(lstrcmp(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName)!=0))
	{
		bModifyServer=true;
		lstrcpyn(m_pGameServiceOption->szServerName,pServerOptionInfo->szServerName,CountArray(m_pGameServiceOption->szServerName));
	}

	//˰������
	m_pGameServiceOption->wRevenueRatio=pServerOptionInfo->wRevenueRatio;
	m_pGameServiceOption->lServiceScore=pServerOptionInfo->lServiceScore;

	//��������
	m_pGameServiceOption->lRestrictScore=pServerOptionInfo->lRestrictScore;
	m_pGameServiceOption->lMinTableScore=pServerOptionInfo->lMinTableScore;
	m_pGameServiceOption->lMinEnterScore=pServerOptionInfo->lMinEnterScore;
	m_pGameServiceOption->lMaxEnterScore=pServerOptionInfo->lMaxEnterScore;

	//��Ա����
	m_pGameServiceOption->cbMinEnterMember=pServerOptionInfo->cbMinEnterMember;
	m_pGameServiceOption->cbMaxEnterMember=pServerOptionInfo->cbMaxEnterMember;

	//�������
	CServerRule::SetForfendGameChat(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameChat(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendWisperOnGame(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendWisperOnGame(pServerOptionInfo->dwServerRule));

	//�������
	CServerRule::SetForfendRoomEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendRoomEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameEnter(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameEnter(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameLookon(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameLookon(pServerOptionInfo->dwServerRule));

	//��������
	CServerRule::SetRecordGameTrack(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameTrack(pServerOptionInfo->dwServerRule));
	CServerRule::SetForfendGameRule(m_pGameServiceOption->dwServerRule,CServerRule::IsForfendGameRule(pServerOptionInfo->dwServerRule));

	//��̬����
	if (m_pGameServiceAttrib->cbDynamicJoin==TRUE)
	{
		CServerRule::SetAllowDynamicJoin(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowDynamicJoin(pServerOptionInfo->dwServerRule));
	}

	//��������
	if (m_pGameServiceAttrib->cbAndroidUser==TRUE)
	{
		CServerRule::SetAllowAndroidAttend(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowAndroidAttend(pServerOptionInfo->dwServerRule));
	}

	//�����й�
	if (m_pGameServiceAttrib->cbOffLineTrustee==TRUE)
	{
		CServerRule::SetAllowOffLineTrustee(m_pGameServiceOption->dwServerRule,CServerRule::IsAllowOffLineTrustee(pServerOptionInfo->dwServerRule));
	}

	//��¼�ɼ�
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,CServerRule::IsRecordGameScore(pServerOptionInfo->dwServerRule));
	}

	//����д��
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))==0)
	{
		CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,CServerRule::IsImmediateWriteScore(pServerOptionInfo->dwServerRule));
	}

	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->RectifyServiceParameter();

	//�����޸�
	if (bModifyServer==true)
	{
		//��������
		CMD_CS_C_ServerModify ServerModify;
		ZeroMemory(&ServerModify,sizeof(ServerModify));

		//����˿�
		ServerModify.wServerPort=pServiceUnits->m_TCPNetworkEngine->GetCurrentPort();

		//������Ϣ
		ServerModify.wKindID=m_pGameServiceOption->wKindID;
		//ServerModify.wNodeID=m_pGameServiceOption->wNodeID;
		ServerModify.wSortID=m_pGameServiceOption->wSortID;
		ServerModify.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
		ServerModify.dwAndroidCount=m_AndroidUserManager.GetAndroidCount();
		ServerModify.dwFullCount=m_pGameServiceOption->wMaxPlayer-RESERVE_USER_COUNT;
		lstrcpyn(ServerModify.szServerName,m_pGameServiceOption->szServerName,CountArray(ServerModify.szServerName));
		lstrcpyn(ServerModify.szServerAddr,m_pInitParameter->m_ServiceAddress.szAddress,CountArray(ServerModify.szServerAddr));

		//��������
		m_pITCPSocketService->SendData(MDM_CS_SERVICE_INFO,SUB_CS_C_SERVER_MODIFY,&ServerModify,sizeof(ServerModify));
	}

	//������Ϣ
	SendRoomMessage(pIServerUserItem,TEXT("��ǰ��Ϸ����������ġ�����ֵ��״̬���������޸ĳɹ�"),SMT_CHAT|SMT_EJECT);

	//�����Ϣ
	TCHAR szBuffer[128]=TEXT("");
	_sntprintf_s(szBuffer, CountArray(szBuffer), TEXT("Զ���޸ķ�������֪ͨ ����Ա %s [ %ld ]"), pIServerUserItem->GetNickName(), pIServerUserItem->GetUserID());

	//�����Ϣ
	CTraceService::TraceString(szBuffer,TraceLevel_Info);

	return true;
}

//�߳��û�
bool CAttemperEngineSink::OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMasterOrder()>0));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMasterOrder()==0)) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//�߳���¼
	m_KickUserItemMap[pITargetUserItem->GetUserID()]=(DWORD)time(NULL);

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//������Ϣ
		SendGameMessage(pITargetUserItem,TEXT("���ѱ�����Ա�������ӣ�"),SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	//����֪ͨ
	LPCTSTR pszMessage=TEXT("���ѱ�����Ա�������Ϸ���䣡");
	SendRoomMessage(pITargetUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

	pITargetUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
	if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

	//��Ϣ����
	CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ŀ���û�
	IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
	if (pITargerUserItem==NULL) return true;

	//��������
	DWORD dwAddRight = 0, dwRemoveRight = 0;

	//��������
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
	{
		if (CMasterRight::CanLimitRoomChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_ROOM_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
	}

	//��Ϸ����
	if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
	{
		if (CMasterRight::CanLimitGameChat(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_GAME_CHAT;
		else
			dwRemoveRight |= UR_CANNOT_GAME_CHAT;
	}

	//����˽��
	if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
	{
		if (CMasterRight::CanLimitWisper(pIServerUserItem->GetMasterRight())==false) return false;

		if( pLimitUserChat->cbLimitValue == TRUE )
			dwAddRight |= UR_CANNOT_WISPER;
		else
			dwRemoveRight |= UR_CANNOT_WISPER;
	}

	//��������
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

		//����֪ͨ
		CMD_GR_ConfigUserRight cur = {0};
		cur.dwUserRight = pITargerUserItem->GetUserRight();

		SendData( pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�û�����Ȩ�����óɹ���"),SMT_CHAT);
	}
	else return false;

	return true;
}

//�߳������û�
bool CAttemperEngineSink::OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��Ϣ����
	CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;

	//Ч������
	ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
	if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
	ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
	if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

	//��ɢ������Ϸ
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	//tagBindParameter *pBindParameter = m_pNormalParameter;
	//for( INT i = 0; i < m_pGameServiceOption->wMaxPlayer; i++ )
	//{
	//	//Ŀ¼�û�
	//	IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
	//	if (pITargerUserItem==NULL || pITargerUserItem==pIServerUserItem ) 
	//	{
	//		pBindParameter++;
	//		continue;
	//	}

	//	//������Ϣ
	//	SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

	//	pBindParameter++;
	//} 

	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	while(pITargerUserItem!=NULL)
	{
		if(pITargerUserItem!=pIServerUserItem)
		{
			//������Ϣ
			SendRoomMessage(pITargerUserItem,pKillAllUser->szKickMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);
		}

		//ö���û�
		pITargerUserItem= m_ServerUserManager.EnumUserItem(wEnumIndex++);
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

	//��������
	CMD_GR_SendMessage *pSysMessage = (CMD_GR_SendMessage *)pData;

	if(pSysMessage->cbLoop==TRUE)
	{
		//���ڱ���
		tagSystemMessage  *pSendMessage=new tagSystemMessage;
		ZeroMemory(pSendMessage, sizeof(tagSystemMessage));

		//������Ϣ
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

		//��¼��Ϣ
		m_SystemMessageActive.Add(pSendMessage);
	}


	//��Ϣ����
	return SendSystemMessage((CMD_GR_SendMessage *)pData, wDataSize);
}

//��ɢ��Ϸ
bool CAttemperEngineSink::OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
	if (wDataSize!=sizeof(CMD_GR_DismissGame)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

	//��Ϣ����
	CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;

	//Ч������
	if(pDismissGame->wDismissTableNum >= m_TableFrameArray.GetCount()) return true;

	//��ɢ��Ϸ
	CTableFrame *pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
	if(pTableFrame)
	{
		if(pTableFrame->IsGameStarted()) pTableFrame->DismissGame();
		pTableFrame->SendGameMessage(TEXT("��Ǹ�������ѱ�����Ա��ɢ��Ϸ"),SMT_EJECT);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_SendWarning));
	if (wDataSize!=sizeof(CMD_GR_SendWarning)) return false;

	//��ȡ����
	CMD_GR_SendWarning * pSendWarning = (CMD_GR_SendWarning*)pData;
	ASSERT(pSendWarning!=NULL);

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pSendWarning->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//Ȩ���ж�
	ASSERT(CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==true);
	if (CMasterRight::CanSendWarning(pIServerUserItem->GetMasterRight())==false) return false;

	//Ȩ���ж�
	ASSERT(pIServerUserItem->GetMasterOrder()>pITargetUserItem->GetMasterOrder());
	if(pIServerUserItem->GetMasterOrder()<pITargetUserItem->GetMasterOrder()) return false;

	//���;���
	TCHAR szMessage[128]=TEXT("");
	_sntprintf_s(szMessage, CountArray(szMessage), TEXT("���棺%s"), pSendWarning->szWarningMessage);
	SendRoomMessage(pITargetUserItem,szMessage,SMT_EJECT);

	return true;
}

//�û���¼
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//��ȡ����
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bAndroidUser=pIServerUserItem->IsAndroidUser();
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��¼����
	//��������
	CMD_GR_LogonSuccess LogonSuccess;
	ZeroMemory(&LogonSuccess, sizeof(LogonSuccess));

	//��¼�ɹ�
	LogonSuccess.dwUserID = pIServerUserItem->GetUserID();
	LogonSuccess.dwUserRight = pIServerUserItem->GetUserRight();
	LogonSuccess.dwMasterRight = pIServerUserItem->GetMasterRight();
	SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_SUCCESS, &LogonSuccess, sizeof(LogonSuccess));

	////��������
	//CMD_GR_ConfigServer ConfigServer;
	//ZeroMemory(&ConfigServer,sizeof(ConfigServer));

	////��������
	//ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
	//ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
	//ConfigServer.wServerType=m_pGameServiceOption->wServerType;
	//ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
	//SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

	//�������
	//SendData(pBindParameter->dwSocketID,MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

	//���ͷ�������
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

	//Ⱥ����PC�û�
	//SendUserInfoPacket(pIServerUserItem, INVALID_DWORD);

	// ���͸��Լ�
	SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

	//��¼���
	SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, SUB_GR_LOGON_FINISH, NULL, 0);

	//������¼
	if (pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//����ͬһ���ϵ���Ҹ��ֻ���
		WORD wEnumIndex = 0;
		while (wEnumIndex < m_ServerUserManager.GetUserItemCount())
		{
			//�����û�
			IServerUserItem *pIUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIUserItem == NULL || pIUserItem == pIServerUserItem) continue;
			if (pIUserItem->GetTableID() != pIServerUserItem->GetTableID()) continue;

			// ���͸��Լ�
			SendUserInfoPacket(pIUserItem, pBindParameter->dwSocketID);
		}
	}

	//��ӭ��Ϣ
	if (bAndroidUser == false)
	{
		//������ʾ
		TCHAR szMessage[128] = TEXT("");
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ӭ�����롰%s����Ϸ���䣬ף����Ϸ��죡"), m_pGameServiceAttrib->szGameName);

		//������Ϣ
		SendRoomMessage(pIServerUserItem, szMessage, SMT_TABLE_ROLL);
	}

	//��������
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

	//��¼֪ͨ
	if (m_pIMatchServiceManager != NULL)
	{
		m_pIMatchServiceManager->OnEventUserLogon(pIServerUserItem);
	}

	return;
}

//�û��뿪
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//��ȡ�ɼ�
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

	//�û���Ϣ
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//������Ϣ
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	LeaveGameServer.cbMaxCardType = pIServerUserItem->GetMaxCardData(LeaveGameServer.aryCardData, MAX_CARD_DATA_COUNT);

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);

	//�ƽ�����
	tagUserTaskEntry * pUserTaskEntry = m_UserTaskManager.GetUserTaskEntry(pIServerUserItem->GetUserID(), TASK_STATUS_UNFINISH | TASK_STATUS_FAILED);
	if(pUserTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem, pUserTaskEntry, LeaveGameServer.VariationInfo, TASK_TYPE_WIN_INNINGS | TASK_TYPE_SUM_INNINGS );
	}
	
	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//���ñ���
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//������Ϣ
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//�������
	if(m_pIMatchServiceManager!=NULL) 
	{
		m_pIMatchServiceManager->OnEventUserQuitMatch(pIServerUserItem, 0);
	}

	//ɾ������
	DeleteDistribute(pIServerUserItem);

	//�Ƴ�����
	m_UserTaskManager.RemoveUserTask(pIServerUserItem->GetUserID());

	//ɾ���û�
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	//�ǳ�֪ͨ
	if (m_pIMatchServiceManager != NULL)
	{
		m_pIMatchServiceManager->OnEventUserLogout(pIServerUserItem);
	}

	return;
}

//������Ϸ��
bool CAttemperEngineSink::PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//���ñ���
	LeaveGameServer.dwUserID=dwUserID;
	LeaveGameServer.dwInoutIndex=dwInoutIndex;
	LeaveGameServer.dwLeaveReason=dwLeaveReason;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(dwUserID,DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return true;
}

//�ƽ�����
bool CAttemperEngineSink::PerformTaskProgress(IServerUserItem * pIServerUserItem, tagUserTaskEntry * pUserTaskEntry, tagVariationInfo &VariationInfo, WORD wTaskType)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL && pUserTaskEntry!=NULL);
	if(pIServerUserItem==NULL || pUserTaskEntry==NULL) return false;

	//״̬У��
	if(pUserTaskEntry->pTaskParameter==NULL) return true;
	if(pUserTaskEntry->cbTaskStatus!=TASK_STATUS_UNFINISH) return true;
	if(pUserTaskEntry->pTaskParameter->wKindID!=m_pGameServiceOption->wKindID) return true;

	//�������
	tagTaskParameter * pTaskParameter = pUserTaskEntry->pTaskParameter;

	//��ȡʱ��
	DWORD dwSpanSecond=(DWORD)time(NULL)-pUserTaskEntry->dwLastUpdateTime;
	dwSpanSecond=__min(dwSpanSecond,pUserTaskEntry->dwResidueTime);

	//����ʱ��	
	pUserTaskEntry->dwResidueTime-=dwSpanSecond;
	pUserTaskEntry->dwLastUpdateTime=(DWORD)time(NULL);

	//����ʱ
	if(pUserTaskEntry->dwResidueTime == 0)
	{		
		pUserTaskEntry->wTaskProgress=0;
		pUserTaskEntry->pTaskParameter=NULL;		
		pUserTaskEntry->cbTaskStatus=TASK_STATUS_FAILED;		
	}

	//����δ��ɣ�������ָ������
	if (pUserTaskEntry->cbTaskStatus == TASK_STATUS_UNFINISH && (pTaskParameter->wTaskType&wTaskType)>0)
	{
		//�ϴ��������
		WORD wLastTaskProgress = pUserTaskEntry->wTaskProgress;

		//��������
		switch(pTaskParameter->wTaskType)
		{
		case TASK_TYPE_WIN_INNINGS:			//Ӯ������
			{
				//����״̬
				pUserTaskEntry->wTaskProgress += VariationInfo.dwWinCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}

				break;
			};
		case TASK_TYPE_SUM_INNINGS:			//�ܾ�����
			{
				//����״̬
				pUserTaskEntry->wTaskProgress += VariationInfo.dwWinCount + VariationInfo.dwLostCount + VariationInfo.dwDrawCount;
				if(pUserTaskEntry->wTaskProgress>=pTaskParameter->wTaskObject)
				{
					pUserTaskEntry->cbTaskStatus=TASK_STATUS_SUCCESS;
				}
				break;
			};
		case TASK_TYPE_MAGIC_EXP:			//ħ������
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

		// ������ȷ����仯���������ݿ�
		if (wLastTaskProgress != pUserTaskEntry->wTaskProgress)
		{
			//��������
			DBR_GR_TaskProgress TaskProgress;
			ZeroMemory(&TaskProgress, sizeof(TaskProgress));

			//���ñ���
			TaskProgress.dwUserID = pIServerUserItem->GetUserID();
			TaskProgress.dwID = pUserTaskEntry->dwID;
			TaskProgress.wTaskProgress = pUserTaskEntry->wTaskProgress;
			TaskProgress.cbTaskStatus = pUserTaskEntry->cbTaskStatus;

			//Ͷ������
			m_pIDBCorrespondManager->PostDataBaseRequest(TaskProgress.dwUserID, DBR_GR_TASK_PROGRESS, 0L, &TaskProgress, sizeof(TaskProgress));
		}

		//�������
		if (pUserTaskEntry->cbTaskStatus == TASK_STATUS_SUCCESS)
		{
			//����ṹ
			CMD_GR_S_TaskFinish TaskFinish;
			TaskFinish.wFinishTaskID = pTaskParameter->wTaskID;
			SendData(pIServerUserItem, MDM_GR_TASK, SUB_GR_TASK_FINISH, &TaskFinish, sizeof(TaskFinish));
		}
	}

	//�ݹ����
	if(pUserTaskEntry->pNextTaskEntry!=NULL)
	{
		PerformTaskProgress(pIServerUserItem, pUserTaskEntry->pNextStatusEntry, VariationInfo,wTaskType);
	}

	return true;
}

//�л�����
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], DWORD dwSocketID, BYTE cbDeviceType)
{
	WORD wTargetIndex = LOWORD(dwSocketID);
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD)) return false;

	//�Ͽ��û�
	if (pIServerUserItem->IsOnline())
	{
		//����֪ͨ
		LPCTSTR pszMessage=TEXT("��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�������뿪��");
		SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

		//�󶨲���
		WORD wSourceIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pSourceParameter=GetBindParameter(wSourceIndex);

		//�����
		ASSERT((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem));
		if ((pSourceParameter!=NULL)&&(pSourceParameter->pIServerUserItem==pIServerUserItem)) pSourceParameter->pIServerUserItem=NULL;

		//�Ͽ��û�
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID,true);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//״̬�л�
	bool bIsOffLine=false;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
	{
		//��������
		WORD wTableID=pIServerUserItem->GetTableID();
		WORD wChairID=pIServerUserItem->GetChairID();

		//����״̬
		bIsOffLine=true;
		//˽������;
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

	//�����ж�
	LPCTSTR pszMachineID=pIServerUserItem->GetMachineID();
	bool bSameMachineID=(lstrcmp(pszMachineID,szMachineID)==0);

	//��������
	bool bAndroidUser=(wTargetIndex>=INDEX_ANDROID);
	tagBindParameter * pTargetParameter=GetBindParameter(wTargetIndex);

	//�����û�
	pTargetParameter->pIServerUserItem=pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr,wTargetIndex,szMachineID,bAndroidUser,false);

	//ȡ���й�
	if (m_pGameServiceOption->wServerType==GAME_GENRE_MATCH)
	{
		if (pIServerUserItem->IsTrusteeUser()==true && pIServerUserItem->GetUserStatus()!=US_PLAYING)
		{
			pIServerUserItem->SetTrusteeUser(false);
		}
	}

	//���¼��ض��������Ϣ
	if (bAndroidUser == false)
	{
		//���ص���
		DBR_GR_RELINE_USERID RelineUserID;
		ZeroMemory(&RelineUserID, sizeof(RelineUserID));
		RelineUserID.dwUserID = pIServerUserItem->GetUserID();
		m_pIDBCorrespondManager->PostDataBaseRequest(RelineUserID.dwUserID, DBR_GR_LOGON_RELINE, 0, &RelineUserID, sizeof(RelineUserID));
	}

	//����������¼�¼�
	OnEventUserLogon(pIServerUserItem, true);

	//��ȫ��ʾ
	if ((bAndroidUser == false) && (bIsOffLine == false) && (bSameMachineID == false))
	{
		SendRoomMessage(pIServerUserItem, TEXT("��ע�⣬�����ʺ�����һ�ط������˴���Ϸ���䣬�Է������뿪��"), SMT_EJECT | SMT_CHAT | SMT_GLOBAL);
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID)
{
	//��������
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//��������
	LogonFailure.lErrorCode=lErrorCode;
	lstrcpyn(LogonFailure.szDescribeString, CYSCharUtils::ConvertToUTF8(pszString), CountArray(LogonFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wHeadSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString);

	//��������
	SendData(dwSocketID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszString, LONG lErrorCode,BYTE cbActivityGame)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_S_UserInsureFailure UserInsureFailure;
	ZeroMemory(&UserInsureFailure,sizeof(UserInsureFailure));

	//��������
	UserInsureFailure.cbActivityGame=cbActivityGame;
	UserInsureFailure.lErrorCode=lErrorCode;
	lstrcpyn(UserInsureFailure.szDescribeString,pszString,CountArray(UserInsureFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(UserInsureFailure.szDescribeString);
	WORD wHeadSize=sizeof(UserInsureFailure)-sizeof(UserInsureFailure.szDescribeString);

	//��������
	SendData(pIServerUserItem,MDM_GR_INSURE,SUB_GR_USER_INSURE_FAILURE,&UserInsureFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//���ñ���
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea)
{
	////��������
	//CMD_GR_PropertyFailure PropertyFailure;
	//ZeroMemory(&PropertyFailure,sizeof(PropertyFailure));

	////���ñ���
	//PropertyFailure.lErrorCode=lErrorCode;
	//PropertyFailure.wRequestArea=wRequestArea;
	//lstrcpyn(PropertyFailure.szDescribeString,pszDescribe,CountArray(PropertyFailure.szDescribeString));

	////��������
	//WORD wDataSize=CountStringBuffer(PropertyFailure.szDescribeString);
	//WORD wHeadSize=sizeof(PropertyFailure)-sizeof(PropertyFailure.szDescribeString);
	//SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_PROPERTY_FAILURE,&PropertyFailure,wHeadSize+wDataSize);

	return true;
}

//�����û�
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID,bool bSendAndroidFalg)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	BYTE cbBuffer[SOCKET_TCP_PACKET] = {0};
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));

	//�û�����
	pUserInfoHead->wFaceID=pUserInfo->wFaceID;
	pUserInfoHead->dwUserID=pUserInfo->dwUserID;
	//pUserInfoHead->dwCustomID=pUserInfo->dwCustomID;
	pUserInfoHead->dwCustomFaceVer = pUserInfo->dwCustomFaceVer;

	//�û�����	
	pUserInfoHead->cbGender=pUserInfo->cbGender;
	pUserInfoHead->cbMemberOrder=pUserInfo->cbMemberOrder;

	//�û�״̬
	pUserInfoHead->wTableID=pUserInfo->wTableID;
	pUserInfoHead->wChairID=pUserInfo->wChairID;
	pUserInfoHead->cbUserStatus=pUserInfo->cbUserStatus;

	////�û�����
	//pUserInfoHead->dwWinCount=pUserInfo->dwWinCount;
	//pUserInfoHead->dwLostCount=pUserInfo->dwLostCount;
	//pUserInfoHead->dwDrawCount=pUserInfo->dwDrawCount;
	//pUserInfoHead->dwFleeCount=pUserInfo->dwFleeCount;
	//pUserInfoHead->dwExperience=pUserInfo->dwExperience;
	//pUserInfoHead->lLoveLiness=pUserInfo->lLoveLiness;

	////�û�����
	//pUserInfoHead->lDiamond = pUserInfo->lDiamond;
	//pUserInfoHead->lInsure=pUserInfo->lInsure;

	//�û��ɼ�
	pUserInfoHead->lScore=pUserInfo->lScore;
	pUserInfoHead->lHoldScore = pUserInfo->lScore + pIServerUserItem->GetTrusteeScore();
	pUserInfoHead->lHoldScore += pIServerUserItem->GetFrozenedScore();

	//�ǳ��ж�
	ASSERT(pUserInfo->szNickName != NULL);
	if (pUserInfo->szNickName[0] == 0)
	{
		_sntprintf_s(pUserInfo->szNickName, CountArray(pUserInfo->szNickName), TEXT("%d"), pUserInfo->dwUserID);
	}

	//������Ϣ
	SendPacket.AddPacket(CYSCharUtils::ConvertToUTF8(pUserInfo->szNickName), DTP_GR_NICK_NAME);
	//SendPacket.AddPacket(pUserInfo->szUnderWrite,DTP_GR_UNDER_WRITE);

	//��������
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

////�㲥����
//bool CAttemperEngineSink::SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount)
//{
//	//����ṹ
//	CMD_GR_S_PropertyMessage  PropertyMessage;
//	PropertyMessage.wPropertyIndex=wPropertyIndex;
//	PropertyMessage.dwSourceUserID=dwSourceID;
//	PropertyMessage.dwTargerUserID=dwTargerID;
//	PropertyMessage.wPropertyCount=wPropertyCount;
//
//	//�����û�
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
////����ЧӦ
//bool CAttemperEngineSink::SendPropertyEffect(IServerUserItem * pIServerUserItem)
//{
//	//����У��
//	if(pIServerUserItem==NULL) return false;
//
//	//����ṹ
//	CMD_GR_S_PropertyEffect  PropertyEffect;
//	PropertyEffect.wUserID =pIServerUserItem->GetUserID();
//	PropertyEffect.cbMemberOrder=pIServerUserItem->GetMemberOrder();
//
//	//�����û�
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
////�����¼�
//bool CAttemperEngineSink::OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem)
//{
//	//Ŀ�����
//	if ( pTargetUserItem == NULL )
//	{
//		//������Ϣ
//		SendPropertyFailure(pISourceUserItem,TEXT("����ʧ�ܣ���Ҫ���͵�����Ѿ��뿪��"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//�����ж�
//	if ( (m_pGameServiceOption->wServerType == GAME_GENRE_GOLD || m_pGameServiceOption->wServerType == GAME_GENRE_MATCH ) )
//	{
//		//������Ϣ
//		SendPropertyFailure(pISourceUserItem,TEXT("�˷��䲻����ʹ�ô˵���,����ʧ��"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//���ҵ���
//	tagPropertyInfo * pPropertyInfo=m_GamePropertyManager.SearchPropertyItem(wPropertyIndex);
//
//	//��ЧЧ��
//	if(pPropertyInfo==NULL)
//	{
//		//������Ϣ
//		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߻�δ����,����ʧ�ܣ�"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//�Լ�ʹ��
//    if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_MESELF)==0 && pISourceUserItem==pTargetUserItem) 
//	{
//		//������Ϣ
//		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲����Լ�ʹ��,����ʧ�ܣ�"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//���ʹ��
//	if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_PLAYER)==0 && pISourceUserItem!=pTargetUserItem) 
//	{
//		//������Ϣ
//		SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲������͸����,ֻ���Լ�ʹ��,����ʧ�ܣ�"), 0L,cbRequestArea);
//
//		return false;
//	}
//
//	//�Թ۷�Χ
//	if((pPropertyInfo->wIssueArea&PT_SERVICE_AREA_LOOKON)==0)  
//	{
//		//��������
//		WORD wTableID = pTargetUserItem->GetTableID();
//		if(wTableID!=INVALID_TABLE)
//		{
//			//��������
//			WORD wEnumIndex=0;
//			IServerUserItem * pIServerUserItem=NULL;
//
//			//��ȡ����
//			CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
//	
//			//ö���û�
//			do
//			{
//				//��ȡ�û�
//				pIServerUserItem=pTableFrame->EnumLookonUserItem(wEnumIndex++);
//				if( pIServerUserItem==NULL) break;
//				if( pIServerUserItem==pTargetUserItem )
//				{
//					//������Ϣ
//					SendPropertyFailure(pISourceUserItem,TEXT("�˵��߲������͸��Թ��û�,����ʧ�ܣ�"), 0L,cbRequestArea);
//
//					return false;
//				}
//			} while (true);
//		}
//	}
//
//	return true;
//}

//���û�
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//��ȡ����
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//��������
	if ((wBindIndex>=INDEX_ANDROID)&&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID)))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//��������
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

//�����û�
bool CAttemperEngineSink::PerformDistribute()
{
	//����У��
	if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinDistributeUser) return false;

	//�����û�
	while(true)
	{
		//����У��
		if(m_DistributeManage.GetCount()<m_pGameServiceOption->wMinPartakeGameUser) break;

		//��������
		CDistributeInfoArray DistributeInfoArray;

        //��ȡ�û�
		WORD wRandCount = __max(m_pGameServiceOption->wMaxPartakeGameUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
		WORD wChairCount = m_pGameServiceOption->wMinPartakeGameUser+rand()%wRandCount;
		WORD wDistributeCount = m_DistributeManage.PerformDistribute(DistributeInfoArray,wChairCount);
		if(wDistributeCount < m_pGameServiceOption->wMinPartakeGameUser) break;

		//Ѱ��λ��
		CTableFrame * pCurrTableFrame=NULL;
		for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
		{
			//��ȡ����
			ASSERT(m_TableFrameArray[i]!=NULL);
			CTableFrame * pTableFrame=m_TableFrameArray[i];

			//״̬�ж�
			if (pTableFrame->GetSitUserCount()==0)
			{
				pCurrTableFrame=pTableFrame;
				break;
			}
		}

		//�����ж�
		if(pCurrTableFrame==NULL) break;

		//�������
		bool bSitSuccess=true;
		INT_PTR nSitFailedIndex=INVALID_CHAIR;
		for(INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
		{
			//��������
			WORD wChairID=pCurrTableFrame->GetNullChairID();

			//�����û�
			if (wChairID!=INVALID_CHAIR)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=DistributeInfoArray[nIndex].pIServerUserItem;

				//�û�����
				if(pCurrTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					nSitFailedIndex=nIndex;
					break;
				}
			}
		}

		//���½��
		if(bSitSuccess)
		{
			//�Ƴ����
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
					//�û�����
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

//���û���
bool CAttemperEngineSink::InitAndroidUser()
{
	//��������
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//���ò���
	AndroidUserParameter.bServiceContinue=true;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;

	//�������
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//��������
	if(m_pGameServiceAttrib->wChairCount>=MAX_CHAIR)
	{
		AndroidUserParameter.bServiceContinue=false;
		AndroidUserParameter.dwMinSitInterval=MIN_INTERVAL_TIME;
		AndroidUserParameter.dwMaxSitInterval=MAX_INTERVAL_TIME;	
	}

	//���ö���
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//��������
bool CAttemperEngineSink::InitTableFrameArray()
{
	//���Ӳ���
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//�ں����
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//�������
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;
	TableFrameParameter.pBenefitInfo = &m_pInitParameter->m_BenefitInfo;

	//���ò���
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	//��������
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(),m_pGameServiceOption->wTableCount*sizeof(CTableFrame *));

	//��������
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		//��������
		m_TableFrameArray[i]=new CTableFrame;

		//��������
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		//������
		if(m_pIMatchServiceManager!=NULL)
		{
			m_pIMatchServiceManager->BindTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
		}
	}	

	return true;
}

//���ñ���
bool CAttemperEngineSink::InitMatchServiceManager()
{
	//����У��
	if(m_pIMatchServiceManager==NULL) return true;

	//���Ӳ���
	tagMatchManagerParameter MatchManagerParameter;
	ZeroMemory(&MatchManagerParameter,sizeof(MatchManagerParameter));

	//���ò���
	MatchManagerParameter.pGameMatchOption=m_pGameMatchOption;
	MatchManagerParameter.pGameServiceOption=m_pGameServiceOption;
	MatchManagerParameter.pGameServiceAttrib=m_pGameServiceAttrib;

	//�ں����
	MatchManagerParameter.pITimerEngine=m_pITimerEngine;
	MatchManagerParameter.pICorrespondManager=m_pIDBCorrespondManager;
	MatchManagerParameter.pTCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//�������
	MatchManagerParameter.pIMainServiceFrame=this;
	MatchManagerParameter.pIServerUserItemSink=this;
	MatchManagerParameter.pIAndroidUserManager=&m_AndroidUserManager;
	MatchManagerParameter.pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);		

	//��ʼ���ӿ�
	m_pIMatchServiceManager->InitMatchInterface(MatchManagerParameter);

	return true;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//�������
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//��������
	if(m_pIMatchServiceManager!=NULL) 
	{
		//��ȡ����
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//�������
		return m_pIMatchServiceManager->OnEventUserJoinMatch(pIServerUserItem,0,pBindParameter->dwSocketID);
	}

	//��������
	tagDistributeInfo DistributeInfo;
	ZeroMemory(&DistributeInfo,sizeof(DistributeInfo));

	//���ñ���
	DistributeInfo.pIServerUserItem=pIServerUserItem;
	DistributeInfo.wLastTableID=pIServerUserItem->GetTableID();
	DistributeInfo.pPertainNode=NULL;

	//��������
	if(m_DistributeManage.InsertDistributeNode(DistributeInfo)==false)
	{
		return pIServerUserItem->IsAndroidUser()?false:true; 
	}

	//�û�����
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		m_TableFrameArray[wTableID]->PerformStandUpAction(pIServerUserItem);
	}

	//����֪ͨ
	SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	//�����ж�
	bool bDistribute=false;
	if (m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_IMMEDIATE) bDistribute=true;

	//ִ�з���
	if (bDistribute==true)
	{
		PerformDistribute();
	}

	return true;
}

//���дʹ���
void CAttemperEngineSink::SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen)
{
	m_WordsFilter.Filtrate(pMsg,pszFiltered,nMaxLen);
}

//ɾ���û�
bool CAttemperEngineSink::DeleteDistribute(IServerUserItem * pIServerUserItem)
{
	//�Ƴ��ڵ�
	m_DistributeManage.RemoveDistributeNode(pIServerUserItem);

	return true;
}

//������¼
bool CAttemperEngineSink::UserImmediately(IServerUserItem * pIServerUserItem)
{
	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

	bool bSitFail = false;

	//�������ӣ�����ҵ����ӣ�
	INT nStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
	{
		//�������
		INT nTableIndex=(i+nStartIndex)%m_pGameServiceOption->wTableCount;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�û�����
		if (pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem) == false)
		{
			bSitFail = true;
			break;
		}

		return true;
	}

	if (!bSitFail)
	{
		//�������ӣ�����ҵ����ӣ�
		nStartIndex = rand() % m_pGameServiceOption->wTableCount;
		for (INT_PTR i = 0; i < (m_pGameServiceOption->wTableCount); i++)
		{
			//�������
			INT nTableIndex = (i + nStartIndex) % m_pGameServiceOption->wTableCount;

			//��ȡ����
			CTableFrame * pTableFrame = m_TableFrameArray[nTableIndex];
			if ((pTableFrame->IsGameStarted() == true) && (bDynamicJoin == false)) continue;
			if (pTableFrame->IsTableLocked()) continue;

			//��Ч����
			WORD wChairID = pTableFrame->GetRandNullChairID();
			if (wChairID == INVALID_CHAIR) continue;

			//�û�����
			if (pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem) == false)
			{
				break;
			}

			return true;
		}
	}

	// �Ҳ����������£������Թ�,�������������������Ϣ���ͻ��ˣ�֪ͨ�ͻ������󳡾���Ϣ
	CTableFrame * pTableFrame = m_TableFrameArray[nStartIndex];
	INT nChairId = rand() % pTableFrame->GetChairCount();
	pTableFrame->PerformLookonAction(nChairId, pIServerUserItem);

	// ������£�������ֻ���ң���������������ҷ��͸��Լ�
	if ( m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	{
		for (WORD i = 0; i < m_pGameServiceAttrib->wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem *pIUserItem = pTableFrame->GetTableUserItem(i);
			if (pIUserItem == NULL) continue;
			if (pIUserItem->GetUserID() == pIServerUserItem->GetUserID()) continue;

			SendUserInfo(pIUserItem, pIServerUserItem->GetBindIndex());
		}
	}

	// ֪ͨ�ͻ������󳡾���Ϣ
	tagBindParameter * pBindParameter = GetBindParameter(pIServerUserItem->GetBindIndex());
	if (pBindParameter == nullptr)
	{
		return false;
	}

	SendData(pBindParameter->dwSocketID, MDM_GF_FRAME, SUB_GF_GAME_OPTION_TO_USER, NULL, 0);

	//ʧ��
	//m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	//m_TableFrameArray[0]->SendMobileReqFailure(pIServerUserItem, REQUEST_FAILURE_NORMAL, EN_MOBILE_CODE::EN_MC_NO_TABLE);
	return true;
}

//���͵�ǰ���ӵ��û����ֻ���¼
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

	// �����ҷ��͸��Լ�
	SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

	return true;
}

//����ϵͳ��Ϣ
bool CAttemperEngineSink::SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize)
{
	//��Ϣ����
	ASSERT(pSendMessage!=NULL);

	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength);
	if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+sizeof(TCHAR)*pSendMessage->wChatLength) 
		return false;

	//���з���
	if(pSendMessage->cbAllRoom == TRUE)
	{
		pSendMessage->cbAllRoom=FALSE;
		m_pITCPSocketService->SendData(MDM_CS_MANAGER_SERVICE,SUB_CS_C_SYSTEM_MESSAGE,pSendMessage,wDataSize);
	}
	else
	{
		//����ϵͳ��Ϣ
		if(pSendMessage->cbGame == TRUE)
			SendGameMessage(pSendMessage->szSystemMessage,SMT_CHAT);
		if(pSendMessage->cbRoom == TRUE)
			SendRoomMessage(pSendMessage->szSystemMessage,SMT_CHAT);
	}

	return true;
}

//�Ƴ���Ϣ
void CAttemperEngineSink::RemoveSystemMessage()
{
	//������Ϣ
	m_SystemMessageBuffer.Append(m_SystemMessageActive);
	m_SystemMessageActive.RemoveAll();
}

void CAttemperEngineSink::RegServerInfoInRedis()
{
#ifdef USE_REDIS
	//���������Ϣ�����������
	CStringA strHost = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerAddr);
	CStringA strCachePASS = CYSCharUtils::ConvertToANSI(m_pGameServiceOption->szCacheServerPass);
	//������������
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
			//���û���,�ȴ����б��ٴ���HASH��
			ValueList ls;
			mRedis.lrange("privatelist", 0, -1, ls);
			list<string>::iterator it = find(ls.begin(), ls.end(), strKey);
			if (it == ls.end())
			{
				mRedis.lpush("privatelist", strKey, retval);
			}

			mRedis.hmset(strKey, vMap, retval);

			//��������Ϸ�������б�;
			string strKindID(CYSService::ToString(m_pGameServiceOption->wKindID));

			ValueList serverList;
			mRedis.lrange("serverlist", 0, -1, serverList);
			list<string>::iterator itFind = find(serverList.begin(), serverList.end(), strKindID);
			if (itFind == serverList.end())
			{
				mRedis.lpush("serverlist", strKindID, retval);
			}

			//������kindID�µ�SERVERID;
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
			//���û���,�ȴ����б��ٴ���HASH��
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
			//���û���,�ȴ����б��ٴ���HASH��
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

//�����ƿ�
bool CAttemperEngineSink::OnDBGameCardLibrary(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��ʼ����
	if (dwContextID == 0xfffe)
	{
		CTableFrame::ClearGameCardLibrary();
		return true;
	}

	//�������
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

//�ƿ�����;
bool CAttemperEngineSink::OnDBGameCardLibraryCfg(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
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
		//��ȡ����;
		ASSERT(m_TableFrameArray[i] != NULL);
		CTableFrame * pTableFrame = m_TableFrameArray[i];
		if ( NULL == pTableFrame )
		{	//�ߵ�����ط�,�϶�������;
			continue;
		}
		
		if (pTableFrame->IsGameStarted())
		{
			//����״��;
			tagTableUserInfo tableUserInfo;
			WORD wUserSitCount = pTableFrame->GetTableUserInfo(tableUserInfo);

			iGameStartCount++;
			iGameAndroidCount	+= tableUserInfo.wTableAndroidCount;
			iGameUserCount		+= tableUserInfo.wTableUserCount;
		}
	}
	CString str;
	str.Format(_T("��ǰ����%u��,���л�����%u��\n��ǰ��Ϸ%d��,���%d��,������%d��."), wOnlineCount, wAndroidCount, iGameStartCount, iGameUserCount, iGameAndroidCount);

	if ( m_pGameServiceOption->cbDistributeRule&DISTRIBUTE_IMMEDIATE )
	{
		WORD wQueueUser = m_DistributeManage.GetRealCount();
		WORD wQueueAndroid = m_DistributeManage.GetAndroidCount();
		str.AppendFormat(_T("\n��ǰ������:���%u��,������%u��"), wQueueUser, wQueueAndroid);
	}
	CTraceService::TraceString(str, TraceLevel_Info);
}

//////////////////////////////////////////////////////////////////////////////////
