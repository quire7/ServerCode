#include "StdAfx.h"
#include "AfxTempl.h"
#include "PrivateGame.h"
#include "PrivateServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CPrivateServiceManager::CPrivateServiceManager(void)
{
	//״̬����
	m_bIsService=false;

	//���ñ���
	m_pIGamePrivatetem=NULL;

	return;
}

//��������
CPrivateServiceManager::~CPrivateServiceManager(void)
{	
	//�ͷ�ָ��
	if(m_pIGamePrivatetem!=NULL) SafeDelete(m_pIGamePrivatetem);
}

//ֹͣ����
bool CPrivateServiceManager::StopService()
{
	//״̬�ж�
	ASSERT(m_bIsService==true);

	//����״̬
	m_bIsService=false;

	//�ͷ�ָ��
	if(m_pIGamePrivatetem!=NULL) SafeRelease(m_pIGamePrivatetem);

	return true;
}

//��������
bool CPrivateServiceManager::StartService()
{
	//״̬�ж�
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//����״̬
	m_bIsService=true;

	//����֪ͨ
	if(m_pIGamePrivatetem!=NULL) m_pIGamePrivatetem->OnStartService();

	return true;
}
//�ӿڲ�ѯ
void *  CPrivateServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMatchServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IMatchServiceManager,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CPrivateServiceManager::CreateGameMatch(BYTE cbMatchType)
{
	//�ӿ��ж�
	ASSERT(m_pIGamePrivatetem==NULL);
	if(m_pIGamePrivatetem!=NULL) return false;

	try
	{
		m_pIGamePrivatetem = new PriaveteGame();
		if(m_pIGamePrivatetem==NULL) throw TEXT("˽�˷�����ʧ�ܣ�");
	}
	catch(...)
	{
		ASSERT(FALSE);
		return false;
	}

	return m_pIGamePrivatetem!=NULL;
}

//��ʼ�����ӿ��
bool CPrivateServiceManager::BindTableFrame(ITableFrame * pTableFrame,WORD wTableID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->BindTableFrame(pTableFrame,wTableID);	
	}

	return true;
}


//��ʼ���ӿ�
bool CPrivateServiceManager::InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->InitMatchInterface(MatchManagerParameter);
	}

	return true;
}

//ʱ���¼�
bool CPrivateServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventTimer(dwTimerID,dwBindParameter);	
	}

	return true;
}

//���ݿ��¼�
bool CPrivateServiceManager::OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);	
	}

	return true;
}


//������Ϣ
bool CPrivateServiceManager::OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);	
	}

	return true;
}

//�û���¼
bool CPrivateServiceManager::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserLogon(pIServerUserItem);	
	}

	return true;
}

//�û��ǳ�
bool CPrivateServiceManager::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserLogout(pIServerUserItem);	
	}

	return true;
}

//�������
bool CPrivateServiceManager::OnEventEnterMatch(DWORD dwSocketID, VOID* pData, DWORD dwUserIP, bool bIsMobile)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventEnterMatch(dwSocketID, pData, dwUserIP, bIsMobile);	
	}

	return true;
	
}

//�û�����
bool CPrivateServiceManager::OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, DWORD dwSocketID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserJoinMatch(pIServerUserItem,cbReason,dwSocketID);		
	}

	return true;
}

//�û�����
bool  CPrivateServiceManager::OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank, DWORD dwContextID)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventUserQuitMatch(pIServerUserItem,cbReason,pBestRank,dwContextID);	
	}

	return true;
}
bool CPrivateServiceManager::OnEventReqStandUP(IServerUserItem * pIServerUserItem)
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return m_pIGamePrivatetem->OnEventReqStandUP(pIServerUserItem);	
	}
	return true;
}

//�û��ӿ�
IUnknownEx * CPrivateServiceManager::GetServerUserItemSink()
{
	ASSERT(m_pIGamePrivatetem!=NULL);
	if(m_pIGamePrivatetem!=NULL)
	{
		return QUERY_OBJECT_PTR_INTERFACE(m_pIGamePrivatetem,IServerUserItemSink);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(PrivateServiceManager);

//////////////////////////////////////////////////////////////////////////
