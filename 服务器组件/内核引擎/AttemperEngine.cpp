#include "StdAfx.h"
#include "TraceService.h" // #include "EventService.h"
#include "AttemperEngine.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngine::CAttemperEngine(void)
{
	m_bService = false;
	m_pITCPSocketService = NULL;
	m_pTCPNetworkEngine = NULL;
	m_pIAttemperEngineSink = NULL;
	return;
}

//��������
CAttemperEngine::~CAttemperEngine(void)
{
}

//�ӿڲ�ѯ
void * CAttemperEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngine, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE(IDataBaseEngineEvent, Guid, dwQueryVer);
	QUERYINTERFACE(ITCPNetworkEngineEvent, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngine, Guid, dwQueryVer);
	return NULL;
}

//��������
bool CAttemperEngine::StartService()
{
	//�ж�״̬
	if (m_bService)
	{
		CTraceService::TraceString(TEXT("���������ظ�������������������"), TraceLevel_Warning);
		return true;
	}

	//��ҽӿ�
	if (m_pIAttemperEngineSink == NULL)
	{
		CTraceService::TraceString(TEXT("����������ҷ��񲻴���"), TraceLevel_Exception);
		return false;
	}

	//���ö���
	if ( !m_QueueService.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) )
	{
		CTraceService::TraceString(TEXT("������������з����ʧ��"), TraceLevel_Exception);
		return false;
	}

	//�������
	if ( !m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx)) )
	{
		CTraceService::TraceString(TEXT("����������ҷ�������ʧ��"), TraceLevel_Exception);
		return false;
	}

	//��������
	if ( !m_QueueService.StartService() )
	{
		CTraceService::TraceString(TEXT("����������з�������ʧ��"), TraceLevel_Exception);
		return false;
	}

	////��ջ�����
	//m_AndroidUserSocketIDArray.RemoveAll();

	//���ñ���
	m_bService = true;

	return true;
}

//ֹͣ����
bool CAttemperEngine::ConcludeService()
{
	//���ñ���
	m_bService = false;

	//ֹͣ�������
	m_QueueService.ConcludeService();

	//ֹͣ���
	if (m_pIAttemperEngineSink != NULL)
	{
		m_pIAttemperEngineSink->OnAttemperEngineConclude(QUERY_ME_INTERFACE(IUnknownEx));
		m_pIAttemperEngineSink = NULL;
	}

	////��ջ�����
	//m_AndroidUserSocketIDArray.RemoveAll();

	return true;
}

//��������
bool CAttemperEngine::SetSocketEngine(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx != NULL);
	m_pITCPSocketService = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPSocketService);
	ASSERT(m_pITCPSocketService != NULL);
	return (m_pITCPSocketService != NULL);
}

//��������
bool CAttemperEngine::SetNetworkEngine(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx != NULL);
	m_pTCPNetworkEngine = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITCPNetworkEngine);
	ASSERT(m_pTCPNetworkEngine != NULL);
	return (m_pTCPNetworkEngine != NULL);
}

//ע�ṳ��
bool CAttemperEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
{
	//Ч�����
	ASSERT(pIUnknownEx != NULL);
	ASSERT(m_pIAttemperEngineSink == NULL);
	if (pIUnknownEx == NULL) return false;
	if (m_pIAttemperEngineSink != NULL) return false;

	//��ѯ�ӿ�
	m_pIAttemperEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAttemperEngineSink);
	if (m_pIAttemperEngineSink == NULL)
	{
		CTraceService::TraceString(TEXT("����������ҷ���ӿڻ�ȡʧ�ܣ��ҽӲ���ʧ��"), TraceLevel_Exception);
		return false;
	}

	return true;
}

//��ȡ�ӿ�
void * CAttemperEngine::GetQueueService(const IID & Guid, DWORD dwQueryVer)
{
	return m_QueueService.QueryInterface(Guid, dwQueryVer);
}

//�����¼�
//////////////////////////////////////////////////////////////////////////
//�Զ��¼�
bool CAttemperEngine::OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	ASSERT(m_pIAttemperEngineSink);
	if (m_pIAttemperEngineSink == NULL) return false;

	return m_pIAttemperEngineSink->OnEventAttemperData(wRequestID, pData, wDataSize);
}

//�����¼�
bool CAttemperEngine::OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	ASSERT(m_pIAttemperEngineSink);
	if (m_pIAttemperEngineSink == NULL) return false;

	return m_pIAttemperEngineSink->OnEventControl(wControlID, pData, wDataSize);
}

//���нӿ�
void CAttemperEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	//�ں��¼�
	VERIFY_RETURN_VOID(m_pIAttemperEngineSink != NULL);
	switch (wIdentifier)
	{
	case EVENT_TIMER:			//��ʱ���¼�
		{
			//Ч�����
			ASSERT(wDataSize == sizeof(NTY_TimerEvent));
			if (wDataSize != sizeof(NTY_TimerEvent)) return;

			//������Ϣ
			NTY_TimerEvent * pTimerEvent = (NTY_TimerEvent *)pBuffer;
			try
			{
				m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->dwTimerID, pTimerEvent->dwBindParameter);
			}
			catch (...)
			{
				CTraceService::TraceStringEx(TraceLevel_Exception,TEXT("��ʱ�������쳣, TimerID:%u, Param:%ld"), pTimerEvent->dwTimerID, pTimerEvent->dwBindParameter);
			}

			return;
		}

	case EVENT_DATABASE:		//���ݿ��¼�
		{
			//Ч�����
			ASSERT(wDataSize >= sizeof(NTY_DataBaseEvent));
			if (wDataSize < sizeof(NTY_DataBaseEvent)) return;

			//������Ϣ
			NTY_DataBaseEvent * pDataBaseEvent = (NTY_DataBaseEvent *)pBuffer;
			try
			{
				m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent->wRequestID, pDataBaseEvent->dwContextID, pDataBaseEvent + 1, wDataSize - sizeof(NTY_DataBaseEvent));
			}
			catch (...)
			{
				CTraceService::TraceStringEx(TraceLevel_Exception,TEXT("���ݿ⴦���쳣, RequestID:%u"), pDataBaseEvent->wRequestID);
			}
			return;
		}

	case EVENT_TCP_NETWORK_ACCEPT:	//����Ӧ���¼�
		{
			//Ч���С
			ASSERT(wDataSize == sizeof(NTY_TCPNetworkAcceptEvent));
			if (wDataSize != sizeof(NTY_TCPNetworkAcceptEvent)) return;

			//������Ϣ
			NTY_TCPNetworkAcceptEvent * pSocketAcceptEvent = (NTY_TCPNetworkAcceptEvent *)pBuffer;
			bool bSuccess = false;
			try
			{
				bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkBind(pSocketAcceptEvent->dwClientAddr, pSocketAcceptEvent->dwSocketID);
			}
			catch (...) 
			{
				CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("OnEventTCPNetworkBind�쳣:dwClientIP=%u, dwSocketID=%u"), pSocketAcceptEvent->dwClientAddr, pSocketAcceptEvent->dwSocketID);
			}
			if ( !bSuccess )
			{
				m_pTCPNetworkEngine->CloseSocket(pSocketAcceptEvent->dwSocketID);
			}

			return;
		}

	case EVENT_TCP_NETWORK_READ:	//�����ȡ�¼�
		{
			//Ч���С
			NTY_TCPNetworkReadEvent * pSocketReadEvent = (NTY_TCPNetworkReadEvent *)pBuffer;
			ASSERT(wDataSize >= sizeof(NTY_TCPNetworkReadEvent));
			ASSERT(wDataSize == (sizeof(NTY_TCPNetworkReadEvent) + pSocketReadEvent->wDataSize));
			if (wDataSize < sizeof(NTY_TCPNetworkReadEvent)) return;
			if (wDataSize != (sizeof(NTY_TCPNetworkReadEvent) + pSocketReadEvent->wDataSize)) return;

			//������Ϣ
			bool bSuccess = false;
			try
			{
				bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkRead(pSocketReadEvent->Command, pSocketReadEvent + 1, pSocketReadEvent->wDataSize, pSocketReadEvent->dwSocketID);
				if ( !bSuccess )
				{
					CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("��Ϣ����ʧ��, MainID:%u, SubID:%u"), pSocketReadEvent->Command.wMainCmdID, pSocketReadEvent->Command.wSubCmdID);

					m_pTCPNetworkEngine->CloseSocket(pSocketReadEvent->dwSocketID);
				}
			}
			catch (...)
			{
				CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("��Ϣ�����쳣, MainID:%u, SubID:%u"), pSocketReadEvent->Command.wMainCmdID, pSocketReadEvent->Command.wSubCmdID);
				m_pTCPNetworkEngine->CloseSocket(pSocketReadEvent->dwSocketID);
			}
			return;
		}

		case EVENT_TCP_NETWORK_SHUT:	//�رն�ȡ�¼�
		{
			//Ч���С
			ASSERT(wDataSize == sizeof(NTY_TCPNetworkShutEvent));
			if (wDataSize != sizeof(NTY_TCPNetworkShutEvent)) return;

			//������Ϣ
			NTY_TCPNetworkShutEvent * pNetworkShutEvent = (NTY_TCPNetworkShutEvent *)pBuffer;

			bool bSuccess = false;
			try
			{
				bSuccess = m_pIAttemperEngineSink->OnEventTCPNetworkShut(pNetworkShutEvent->dwClientAddr, pNetworkShutEvent->dwActiveTime, pNetworkShutEvent->dwSocketID);
			}
			catch (...)	
			{
				CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("OnEventTCPNetworkShut�쳣:dwClientIP=%u, dwActiveTime=%u,dwSocketID=%u"), pNetworkShutEvent->dwClientAddr, pNetworkShutEvent->dwActiveTime, pNetworkShutEvent->dwSocketID);
			}
			if ( !bSuccess )
			{
				m_pTCPNetworkEngine->CloseSocket(pNetworkShutEvent->dwSocketID);
			}

			return;
		}

	case EVENT_TCP_SOCKET_READ:		//�����ȡ�¼�
		{
			//Ч���С
			NTY_TCPSocketReadEvent * pSocketReadEvent = (NTY_TCPSocketReadEvent *)pBuffer;
			ASSERT(wDataSize >= sizeof(NTY_TCPSocketReadEvent));
			ASSERT(wDataSize == (sizeof(NTY_TCPSocketReadEvent) + pSocketReadEvent->wDataSize));
			if (wDataSize < sizeof(NTY_TCPSocketReadEvent)) return;
			if (wDataSize != (sizeof(NTY_TCPSocketReadEvent) + pSocketReadEvent->wDataSize)) return;

			//������Ϣ
			try
			{
				if (!m_pIAttemperEngineSink->OnEventTCPSocketRead(pSocketReadEvent->wServiceID, pSocketReadEvent->Command, pSocketReadEvent + 1, pSocketReadEvent->wDataSize))
				{
					CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("��Ϣ����ʧ��, MainID:%d, SubID:%d"), pSocketReadEvent->Command.wMainCmdID, pSocketReadEvent->Command.wSubCmdID);
					m_pITCPSocketService->CloseSocket();
				}
			}
			catch (...)
			{
				CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("��Ϣ�����쳣, MainID:%d, SubID:%d"), pSocketReadEvent->Command.wMainCmdID, pSocketReadEvent->Command.wSubCmdID);
				m_pITCPSocketService->CloseSocket();
			}
			return;
		}

		case EVENT_TCP_SOCKET_SHUT:	//����ر��¼�
		{
			//Ч���С
			ASSERT(wDataSize == sizeof(NTY_TCPSocketShutEvent));
			if (wDataSize != sizeof(NTY_TCPSocketShutEvent)) return;

			//������Ϣ
			NTY_TCPSocketShutEvent * pSocketShutEvent = (NTY_TCPSocketShutEvent *)pBuffer;

			bool bSuccess = false;
			try
			{
				bSuccess = m_pIAttemperEngineSink->OnEventTCPSocketShut(pSocketShutEvent->wServiceID, pSocketShutEvent->cbShutReason);
			}
			catch (...)
			{
				CTraceService::TraceStringEx(TraceLevel_Exception, TEXT("OnEventTCPSocketShut�쳣:wServiceID=%u, cbShutReason=%u"), pSocketShutEvent->wServiceID, pSocketShutEvent->cbShutReason);
			}
			if (!bSuccess)
			{
				m_pITCPSocketService->CloseSocket();
			}
			return;
		}

		case EVENT_TCP_SOCKET_LINK:
		{
			//Ч���С
			ASSERT(wDataSize == sizeof(NTY_TCPSocketLinkEvent));
			if (wDataSize != sizeof(NTY_TCPSocketLinkEvent)) return;

			//������Ϣ
			NTY_TCPSocketLinkEvent * pSocketLinkEvent = (NTY_TCPSocketLinkEvent *)pBuffer;

			bool bSuccess = false;
			try
			{
				bSuccess = m_pIAttemperEngineSink->OnEventTCPSocketLink(pSocketLinkEvent->wServiceID, pSocketLinkEvent->nErrorCode);
			}
			catch (...)
			{
				CTraceService::TraceStringEx(TraceLevel_Exception, _T("OnEventTCPSocketLink�쳣!wServiceID=%u,nErrorCode=%u"), pSocketLinkEvent->wServiceID, pSocketLinkEvent->nErrorCode);
			}
			if (!bSuccess)
			{
				m_pITCPSocketService->CloseSocket();
			}

			return;
		}

	case EVENT_CONTROL:
		{
			//Ч���С
			ASSERT(wDataSize == sizeof(NTY_ControlEvent));
			if (wDataSize != sizeof(NTY_ControlEvent)) return;

			//������Ϣ
			//NTY_ControlEvent * pControlEvent = (NTY_ControlEvent *)pBuffer;
			try
			{
				m_pIAttemperEngineSink->OnEventControl(wIdentifier, pBuffer, wDataSize);
			}
			catch (...)
			{
				ASSERT(0);
				CTraceService::TraceStringEx(TraceLevel_Exception, _T("OnEventControl�쳣!wIdentifier=%u,wDataSize=%u"), wIdentifier, wDataSize);
			}

			return;
		}
	}

	//�����¼�
	try
	{
		m_pIAttemperEngineSink->OnEventAttemperData(wIdentifier, pBuffer, wDataSize);
	}
	catch (...)
	{
		ASSERT(0);
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("OnEventAttemperData�쳣!wIdentifier=%u,wDataSize=%u"), wIdentifier, wDataSize);
	}

	return;
}

bool CAttemperEngine::OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		if (m_pIAttemperEngineSink->OnEventDataBase(wRequestID, dwContextID, pData, wDataSize))
		{
			return true;
		}
		ASSERT(0);
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("OnEventDataBaseResultʧ��!wRequestID=%u,dwContextID=%u,wDataSize=%u"), wRequestID, dwContextID, wDataSize);
	}
	catch (...)
	{
		ASSERT(0);
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("OnEventDataBaseResult�쳣!wRequestID=%u,dwContextID=%u,wDataSize=%u"), wRequestID, dwContextID, wDataSize);
	}
	return false;
}

//����ģ��ӿ�
//////////////////////////////////////////////////////////////////////////
//Ӧ���¼�
bool CAttemperEngine::OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr)
{
	//��������
	CYSDataLocker lock(m_CriticalSection);

	try
	{
		if (m_pIAttemperEngineSink->OnEventTCPNetworkBind(dwClientAddr, dwSocketID))
		{
			return true;
		}
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("����ģ��󶨴���ʧ��!dwSocketID=%u,dwClientIP=%u"), dwSocketID, dwClientAddr);
	}
	catch (...) 
	{
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("����ģ��󶨴����쳣!dwSocketID=%u,dwClientIP=%u"), dwSocketID, dwClientAddr);
	}
	ASSERT(0);
	return false;
}

//�ر��¼�
bool CAttemperEngine::OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr, DWORD dwActiveTime)
{
	//��������
	CYSDataLocker lock(m_CriticalSection);

	try
	{
		if (m_pIAttemperEngineSink->OnEventTCPNetworkShut(dwClientAddr, dwActiveTime, dwSocketID))
		{
			return true;
		}
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("����ģ��رմ���ʧ��!dwClientIP=%u,dwActiveTime=%u,dwSocketID=%u"), dwClientAddr, dwActiveTime, dwSocketID);
	}
	catch (...)	
	{
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("����ģ��رմ����쳣!dwClientIP=%u,dwActiveTime=%u,dwSocketID=%u"), dwClientAddr, dwActiveTime, dwSocketID);
	}

	ASSERT(0);
	return false;
}

//��ȡ�¼�
bool CAttemperEngine::OnEventTCPNetworkRead(DWORD dwSocketID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//��������
	CYSDataLocker lock(m_CriticalSection);

	try
	{
		if (m_pIAttemperEngineSink->OnEventTCPNetworkRead(Command, pData, wDataSize, dwSocketID))
		{
			return true;
		}
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("����ģ���ȡ����ʧ��!MainID=%u,SubID=%u,DataSize=%u"), Command.wMainCmdID, Command.wSubCmdID, wDataSize);
	}
	catch (...)
	{
		CTraceService::TraceStringEx(TraceLevel_Exception, _T("����ģ���ȡ�����쳣!MainID=%u,SubID=%u,DataSize=%u"), Command.wMainCmdID, Command.wSubCmdID, wDataSize);
	}
	ASSERT(0);
	return false;
}

bool CAttemperEngine::GetBurthenInfo( tagBurthenInfo & BurthenInfo )
{
	return m_QueueService.GetBurthenInfo(BurthenInfo);
}

//////////////////////////////////////////////////////////////////////////
//����������
//
extern "C" __declspec(dllexport) void * CreateAttemperEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CAttemperEngine * pAttemperEngine = NULL;
	try
	{
		pAttemperEngine = new CAttemperEngine();
		if (pAttemperEngine == NULL) throw TEXT("����ʧ��");
		void * pObject = pAttemperEngine->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pAttemperEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
