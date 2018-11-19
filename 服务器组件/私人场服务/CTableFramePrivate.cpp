#include "StdAfx.h"
#include "CTableFramePrivate.h"


//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFramePrivate::m_wPlayerCount=MAX_CHAIR;			//游戏人数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFramePrivate::CTableFramePrivate()
{
	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pPrivateEventSink=NULL;

	return;
}

//析构函数
CTableFramePrivate::~CTableFramePrivate(void)
{
}

//接口查询
void *  CTableFramePrivate::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameHook, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameHook, Guid, dwQueryVer);
	return NULL;
}

//
bool CTableFramePrivate::SetMatchEventSink(IUnknownEx * pIUnknownEx)
{
	m_pPrivateEventSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IMatchEventSink);

	return true;
}

//初始化
bool  CTableFramePrivate::InitTableFrameHook(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	return true;
}

//游戏开始
bool CTableFramePrivate::OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount)
{
	if(m_pPrivateEventSink!=NULL)
	{
		return m_pPrivateEventSink->OnEventGameStart(pITableFrame, wChairCount);
	}

	return false;
}

//游戏结束
bool  CTableFramePrivate::OnEventGameEnd(ITableFrame *pITableFrame, WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason )
{
	if(m_pPrivateEventSink!=NULL)
	{
		return m_pPrivateEventSink->OnEventGameEnd(pITableFrame,wChairID,pIServerUserItem,cbReason);
	}
	
	return false;
}
//写分
bool CTableFramePrivate::WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount,DataStream& kData)
{
	if(m_pPrivateEventSink!=NULL)
	{
		return m_pPrivateEventSink->WriteTableScore(pITableFrame,ScoreInfoArray,wScoreCount,kData);
	}

	return false;
}
bool CTableFramePrivate::AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex)
{
	if(m_pPrivateEventSink!=NULL)
	{
		return m_pPrivateEventSink->AddPrivateAction(pTbableFrame,dwChairID,bActionIdex);
	}

	return false;
	
}
bool CTableFramePrivate::addPrivatePlayCout(ITableFrame* pTbableFrame, WORD wCout)
{
	if (m_pPrivateEventSink != NULL)
	{
		return m_pPrivateEventSink->addPrivatePlayCout(pTbableFrame, wCout);
	}

	return false;
}
bool CTableFramePrivate::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if(m_pPrivateEventSink!=NULL)  
	{
		return m_pPrivateEventSink->OnActionUserOffLine(wChairID,pIServerUserItem);
	}
	return false;
}
//用户坐下
bool CTableFramePrivate::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pPrivateEventSink!=NULL)  
	{
		return m_pPrivateEventSink->OnActionUserSitDown(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//用户起来
bool CTableFramePrivate::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(m_pPrivateEventSink!=NULL) 
	{
		return m_pPrivateEventSink->OnActionUserStandUp(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem, bLookonUser);
	}

	return false;
}

//用户同意
bool CTableFramePrivate::OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if(m_pPrivateEventSink!=NULL)
	{
		return m_pPrivateEventSink->OnActionUserOnReady(m_pITableFrame->GetTableID(),wChairID, pIServerUserItem ,pData ,wDataSize);
	}

	return false;
}

//房卡数量判断
bool CTableFramePrivate::OnActionUserFangKaCheck(WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure)
{
	if (m_pPrivateEventSink != NULL)
	{
		return m_pPrivateEventSink->OnActionUserFangKaCheck(m_pITableFrame->GetTableID(), wChairID, pIServerUserItem, lUserInsure);
	}

	return false;
}

bool CTableFramePrivate::OnEventUserReturnMatch(IServerUserItem * pIServerUserItem)
{
	if (m_pPrivateEventSink != NULL)
	{
		return m_pPrivateEventSink->OnEventUserReturnMatch(m_pITableFrame, pIServerUserItem);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
