#include "StdAfx.h"
#include "TableFrameHook.h"


//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameHook::m_wPlayerCount=MAX_CHAIR;			//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameHook::CTableFrameHook()
{
	//�������
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pMatchEventSink=NULL;

	return;
}

//��������
CTableFrameHook::~CTableFrameHook(void)
{
}

//�ӿڲ�ѯ
void *  CTableFrameHook::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameHook,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameHook,Guid,dwQueryVer);
	return NULL;
}

//
bool CTableFrameHook::SetMatchEventSink(IUnknownEx * pIUnknownEx)
{
	m_pMatchEventSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchEventSink);

	return true;
}

//��ʼ��
bool  CTableFrameHook::InitTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//��Ϸ��ʼ
bool CTableFrameHook::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventGameStart(pITableFrame, wChairCount);
	}

	return false;
}

//��Ϸ����
bool  CTableFrameHook::OnEventGameEnd(ITableFrame *pITableFrame, WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnEventGameEnd(pITableFrame,wChairID,pIServerUserItem,cbReason);
	}
	
	return false;
}

//�û�����
bool CTableFrameHook::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pMatchEventSink!=NULL)  
	{
		return m_pMatchEventSink->OnActionUserSitDown(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//�û�����
bool CTableFrameHook::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pMatchEventSink!=NULL) 
	{
		return m_pMatchEventSink->OnActionUserStandUp(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//�û�ͬ��
bool CTableFrameHook::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if(m_pMatchEventSink!=NULL)
	{
		return m_pMatchEventSink->OnActionUserOnReady(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem ,pData ,wDataSize);
	}

	return false;
}

bool CTableFrameHook::addPrivatePlayCout(ITableFrame* pITableFrame, WORD wCout)
{
	if (m_pMatchEventSink != NULL)
	{
		return m_pMatchEventSink->addPrivatePlayCout(pITableFrame, wCout);
	}

	return false;
}

bool CTableFrameHook::OnEventUserReturnMatch(IServerUserItem * pIServerUserItem)
{
	if (m_pMatchEventSink != NULL)
	{
		return m_pMatchEventSink->OnEventUserReturnMatch(m_pITableFrame, pIServerUserItem);
	}

	return false;
}

bool CTableFrameHook::WriteTableScore(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData)
{
	if (m_pMatchEventSink != NULL)
	{
		return m_pMatchEventSink->WriteTableScore(pITableFrame, ScoreInfoArray, wScoreCount, kData);
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
