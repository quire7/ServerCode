#include "StdAfx.h"
#include "ServerUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
DWORD CServerUserItem::g_dwCardLibCount = 0;
BYTE CServerUserItem::g_cbCardLibEnable = 0;
DWORD CServerUserItem::g_dwLibStartDateTime = 0;
DWORD CServerUserItem::g_dwLibEndDateTime = 0;
DWORD CServerUserItem::g_dwCardLibCustomCount = 0;

//构造函数
CServerUserItem::CServerUserItem()
{
	//附加变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//托管变量
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;

	//带入设置
	m_lDefPutScore=0L;					//默认一次带入多少积分
	m_lLowPutScore=0L;					//低于多少带入积分
	m_lNextPutScore = 0L;				//下次带入积分
	//m_iExchangeRatio=0;					//兑换比例

	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//最大牌型
	m_cbMaxCardType = 0;
	ZeroMemory(m_aryCardData, sizeof(m_aryCardData));

	//客户类型
	m_bAndroidUser=false;

	//组件接口
	m_pIServerUserItemSink=NULL;

	//系统属性
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_NONE;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//比赛信息
	m_dwSignUpTime=0;
	m_cbSignUpStatus=0;
	m_pMatchData=NULL;

	//辅助变量
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));
	m_cbReUserStatus = US_NULL;

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//记录信息
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	//牌库
	m_dwCardLibIndex = 0;
	m_dwStartLibTime = 0;
	m_dwStopLibTime = 0;
	m_cbCardLibEnabled = 0;

	return;
}

//析构函数
CServerUserItem::~CServerUserItem()
{
	m_UserProperty.RemoveAll();
}

//接口查询
VOID * CServerUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserItem,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserItem,Guid,dwQueryVer);
	return NULL;
}

//玩家最大牌型
BYTE CServerUserItem::GetMaxCardData(BYTE* pData, BYTE cbCount)
{
	ASSERT(cbCount <= MAX_CARD_DATA_COUNT);
	if ( cbCount>MAX_CARD_DATA_COUNT )
	{
		return 0;
	}

	for (int i = 0; i < cbCount; i++)
	{
		pData[i] = m_aryCardData[i];
	}

	return m_cbMaxCardType;
}

//用户胜率
WORD CServerUserItem::GetUserWinRate()
{
	//计算胜率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwWinCount*10000L/dwPlayCount);

	return 0;
}

//用户输率
WORD CServerUserItem::GetUserLostRate()
{
	//计算输率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwLostCount*10000L/dwPlayCount);

	return 0;
}

//用户和率
WORD CServerUserItem::GetUserDrawRate()
{
	//计算和率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwDrawCount*10000L/dwPlayCount);

	return 0;
}

//用户逃率
WORD CServerUserItem::GetUserFleeRate()
{
	//计算逃率
	DWORD dwPlayCount=GetUserPlayCount();
	if (dwPlayCount!=0L) return (WORD)(m_UserInfo.dwFleeCount*10000L/dwPlayCount);

	return 0;
}

//对比帐号
bool CServerUserItem::ContrastNickName(LPCTSTR pszNickName)
{
	//效验参数
	ASSERT(pszNickName!=NULL);
	if (pszNickName==NULL) return false;

	//长度对比
	INT nContrastLen=lstrlen(pszNickName);
	INT nSourceLen=lstrlen(m_UserInfo.szNickName);

	//字符对比
	if (nContrastLen!=nSourceLen) return false;
	int nRet = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pszNickName, nContrastLen, m_UserInfo.szNickName, nSourceLen);
	if (nRet!=CSTR_EQUAL) return false;

	return true;
}

//对比密码
bool CServerUserItem::ContrastLogonPass(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT(pszPassword!=NULL);
	if (pszPassword==NULL) return false;

	//长度对比
	INT nTargetLen=lstrlen(pszPassword);
	INT nSourceLen=lstrlen(m_szLogonPass);

	//密码对比
	if (nTargetLen!=nSourceLen) return false;
	int nRet = CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, pszPassword, nTargetLen, m_szLogonPass, nSourceLen);
	if (nRet!=CSTR_EQUAL) return false;

	return true;
}

//设置状态
bool CServerUserItem::SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//记录信息
	WORD wOldTableID=m_UserInfo.wTableID;
	WORD wOldChairID=m_UserInfo.wChairID;

	//设置变量
	m_UserInfo.wTableID=wTableID;
	m_UserInfo.wChairID=wChairID;
	m_UserInfo.cbUserStatus=cbUserStatus;
	if (cbUserStatus == US_PLAYING)
	{
		m_UserInfo.wLastTableID = wTableID;
	}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink != NULL)
	{
 		m_pIServerUserItemSink->OnEventUserItemStatus(this, wOldTableID, wOldChairID);
	}

	return true;
}

//写入积分
bool CServerUserItem::WriteUserScore(SCORE lScore, SCORE lGrade, SCORE lRevenue, SCORE lIngot, BYTE cbScoreType, DWORD dwPlayTimeCount,DWORD dwWinExperience)
{
	//效验状态
	ASSERT((m_UserInfo.dwUserID!=0L)&&(cbScoreType!=SCORE_TYPE_NULL));
	if ((m_UserInfo.dwUserID==0L)||(cbScoreType==SCORE_TYPE_NULL)) return false;

	//修改胜负
	switch (cbScoreType)
	{
	case SCORE_TYPE_WIN:	//胜局
		{
			m_UserInfo.dwWinCount++;
			m_RecordInfo.dwWinCount++;
			m_VariationInfo.dwWinCount++;
			break;
		}
	case SCORE_TYPE_LOSE:	//输局
		{
			m_UserInfo.dwLostCount++;
			m_RecordInfo.dwLostCount++;
			m_VariationInfo.dwLostCount++;
			break;
		}
	case SCORE_TYPE_DRAW:	//和局
		{
			m_UserInfo.dwDrawCount++;
			m_RecordInfo.dwDrawCount++;
			m_VariationInfo.dwDrawCount++;
			break;
		}
	case SCORE_TYPE_FLEE:	//逃局
		{
			m_UserInfo.dwFleeCount++;
			m_RecordInfo.dwFleeCount++;
			m_VariationInfo.dwFleeCount++;
			break;
		}
	}

	//设置标志
	m_bModifyScore=true;

	//设置积分
	m_UserInfo.lScore+=lScore;
	//m_UserInfo.lGrade+=lGrade;
	//m_UserInfo.lIngot+=lIngot;
	//m_UserInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;	
	m_UserInfo.dwExperience += dwWinExperience;

	//记录信息
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lGrade+=lGrade;
	m_RecordInfo.lRevenue+=lRevenue;
	//m_RecordInfo.lIngot+=lIngot;
	m_RecordInfo.dwPlayTimeCount+=dwPlayTimeCount;
	//m_RecordInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;
	m_RecordInfo.dwExperience += dwWinExperience;

	//变更信息
	//m_VariationInfo.lIngot+=lIngot;
	m_VariationInfo.dwPlayTimeCount+=dwPlayTimeCount;
	//m_VariationInfo.dwExperience+=(dwPlayTimeCount+59L)/60L+dwWinExperience;
	m_VariationInfo.dwExperience += dwWinExperience;
	
	//分数变更
	m_VariationInfo.lScore += lScore;
	m_VariationInfo.lGrade+=lGrade;
	m_VariationInfo.lRevenue+=lRevenue;	

	//带入积分
	if (m_lNextPutScore > 0)
	{
		// 总积分
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// 总积分大于默认带入积分，带入默认积分值
		if (lTotalScore > m_lNextPutScore)
		{
			m_UserInfo.lScore = m_lNextPutScore;
			m_lTrusteeScore = (lTotalScore - m_lNextPutScore);// / m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0;
		}

		m_lNextPutScore = 0;
	}
	else
	{
		SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
		SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
		if (lLowPutScore > 0 && lPutScore > 0)
		{
			// 身上积分小于带入积分限制，做带入调整
			if (m_UserInfo.lScore < lLowPutScore)
			{
				if (IsAndroidUser() == true)
				{
					CString str;
					str.Format(TEXT("机器人 %s 带入前持有金币 %d "), m_UserInfo.szNickName, m_UserInfo.lScore);
					::OutputDebugString(str);
					str.Format(TEXT("身上总金币为 %d \n"), m_lTrusteeScore);
					::OutputDebugString(str);
				}

				// 总积分
				SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

				// 总积分大于默认带入积分，带入默认积分值
				if (lTotalScore > lPutScore)
				{
					m_UserInfo.lScore = lPutScore;
					m_lTrusteeScore = (lTotalScore - lPutScore); // m_iExchangeRatio;
				}
				else
				{
					m_UserInfo.lScore = lTotalScore;
					m_lTrusteeScore = 0;
				}

				if (IsAndroidUser() == true)
				{
					CString str;
					str.Format(TEXT("机器人 %s 带入后持有金币 %d "), m_UserInfo.szNickName, m_UserInfo.lScore);
					::OutputDebugString(str);
					str.Format(TEXT("身上总金币为 %d \n"), m_lTrusteeScore);
					::OutputDebugString(str);
				}
			}
		}
	}

	//托管积分
	//if (m_lRestrictScore>0L)
	//{
	//	//变量定义
	//	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

	//	//托管调整
	//	if (lTotalScore>m_lRestrictScore)
	//	{
	//		m_UserInfo.lScore=m_lRestrictScore;
	//		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	//	}
	//	else
	//	{
	//		m_lTrusteeScore=0L;
	//		m_UserInfo.lScore=lTotalScore;
	//	}
	//}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink != NULL)
	{
		m_pIServerUserItemSink->OnEventUserItemScore(this, TASK_TYPE_WIN_INNINGS | TASK_TYPE_SUM_INNINGS);
	}

	return true;
}

//查询记录
bool CServerUserItem::QueryRecordInfo(tagVariationInfo & RecordInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//设置变量
	RecordInfo=m_RecordInfo;

	return true;
}

//提取变更
bool CServerUserItem::DistillVariation(tagVariationInfo & VariationInfo)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//修改判断
	if (m_bModifyScore==true)
	{
		//设置变量
		VariationInfo=m_VariationInfo;
	}
	else
	{
		//设置变量
		ZeroMemory(&VariationInfo,sizeof(VariationInfo));
	}

	//还原变量
	m_bModifyScore=false;
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	return true;
}

//冻结积分
bool CServerUserItem::FrozenedUserScore(SCORE lScore)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	////效验积分
	//ASSERT((lScore>=0L)&&((m_UserInfo.lScore+m_lTrusteeScore)>=lScore));
	//if ((lScore<0L)||((m_UserInfo.lScore+m_lTrusteeScore)<lScore)) return false;

	////设置变量
	//m_lFrozenedScore+=lScore;

	////设置用户
	//if (m_lTrusteeScore<lScore)
	//{
	//	//积分锁定
	//	lScore-=m_lTrusteeScore;
	//	m_UserInfo.lScore-=lScore;
	//	m_lTrusteeScore=0L;
	//}
	//else
	//{
	//	//托管锁定
	//	m_lTrusteeScore-=lScore;
	//}

	SCORE lTrusteeScore = m_lTrusteeScore;// *m_iExchangeRatio;
	SCORE lAllScore = m_UserInfo.lScore + lTrusteeScore;
	
	//效验积分
	ASSERT((lScore >= 0L) && (lAllScore >= lScore));
	if ((lScore < 0L) || (lAllScore < lScore)) return false;

	//设置变量
	m_lFrozenedScore += lScore;

	//设置用户
	if (lTrusteeScore < lScore)
	{
		//积分锁定
		lScore -= lTrusteeScore;
		m_UserInfo.lScore -= lScore;
		m_lTrusteeScore = 0L;
	}
	else
	{
		//托管锁定
		m_lTrusteeScore -= lScore;// / m_iExchangeRatio;
	}

	return true;
}

//解冻积分
bool CServerUserItem::UnFrozenedUserScore(SCORE lScore)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//效验积分
	ASSERT((lScore>=0L)&&(m_lFrozenedScore>=lScore));
	if ((lScore<0L)||(m_lFrozenedScore<lScore)) return false;

	//设置变量
	m_lFrozenedScore -= lScore;
	m_UserInfo.lScore += lScore;

	//带入积分
	if (m_lNextPutScore > 0)
	{
		// 总积分
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// 总积分大于默认带入积分，带入默认积分值
		if (lTotalScore > m_lNextPutScore)
		{
			m_UserInfo.lScore = m_lNextPutScore;
			m_lTrusteeScore = (lTotalScore - m_lNextPutScore); // m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0;
		}

		m_lNextPutScore = 0;
	}
	else
	{
		SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
		SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
		if (lLowPutScore > 0 && lPutScore > 0)
		{
			// 身上积分小于带入积分限制，做带入调整
			if (m_UserInfo.lScore < lLowPutScore)
			{
				// 总积分
				SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

				// 总积分大于默认带入积分，带入默认积分值
				if (lTotalScore > lPutScore)
				{
					m_UserInfo.lScore = lPutScore;
					m_lTrusteeScore = (lTotalScore - lPutScore);// / m_iExchangeRatio;
				}
				else
				{
					m_UserInfo.lScore = lTotalScore;
					m_lTrusteeScore = 0;
				}
			}
		}
	}

	////托管积分
	//if (m_lRestrictScore>0L)
	//{
	//	//变量定义
	//	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

	//	//托管调整
	//	if (lTotalScore>m_lRestrictScore)
	//	{
	//		m_UserInfo.lScore=m_lRestrictScore;
	//		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	//	}
	//	else
	//	{
	//		m_lTrusteeScore=0L;
	//		m_UserInfo.lScore=lTotalScore;
	//	}
	//}

	return true;
}

//修改信息
bool CServerUserItem::ModifyUserProperty(SCORE lScore,LONG lLoveLiness)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//设置积分
	//m_UserInfo.lScore+=lScore;
	m_UserInfo.lLoveLiness+=lLoveLiness;

	//记录信息
	//m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lLoveLiness+=lLoveLiness;

	//变更信息
	if (lLoveLiness!=0L)
	{
		m_bModifyScore=true;
		m_VariationInfo.lLoveLiness+=lLoveLiness;
	}

	/**
	////带入积分
	//if (m_lNextPutScore > 0)
	//{
	//	// 总积分
	//	SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

	//	// 总积分大于默认带入积分，带入默认积分值
	//	if (lTotalScore > m_lNextPutScore)
	//	{
	//		m_UserInfo.lScore = m_lNextPutScore;
	//		m_lTrusteeScore = (lTotalScore - m_lNextPutScore); // m_iExchangeRatio;
	//	}
	//	else
	//	{
	//		m_UserInfo.lScore = lTotalScore;
	//		m_lTrusteeScore = 0;
	//	}

	//	m_lNextPutScore = 0;
	//}
	//else
	//{
	//	SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
	//	SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
	//	if (lLowPutScore > 0 && lPutScore > 0)
	//	{
	//		// 身上积分小于带入积分限制，做带入调整
	//		if (m_UserInfo.lScore < lLowPutScore)
	//		{
	//			// 总积分
	//			SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

	//			// 总积分大于默认带入积分，带入默认积分值
	//			if (lTotalScore > lPutScore)
	//			{
	//				m_UserInfo.lScore = lPutScore;
	//				m_lTrusteeScore = (lTotalScore - lPutScore); // m_iExchangeRatio;
	//			}
	//			else
	//			{
	//				m_UserInfo.lScore = lTotalScore;
	//				m_lTrusteeScore = 0;
	//			}
	//		}
	//	}
	//}

	//////托管积分
	////if ((m_lRestrictScore>0L)&&(lScore!=0L))
	////{
	////	//变量定义
	////	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;
	////
	////	//托管调整
	////	if (lTotalScore>m_lRestrictScore)
	////	{
	////		m_UserInfo.lScore=m_lRestrictScore;
	////		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	////	}
	////	else
	////	{
	////		m_lTrusteeScore=0L;
	////		m_UserInfo.lScore=lTotalScore;
	////	}
	////}

	////发送状态
	//ASSERT(m_pIServerUserItemSink!=NULL);
	////if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_PROPERTY);
	//if (m_pIServerUserItemSink != NULL)
	//{
	//	m_pIServerUserItemSink->OnEventUserItemScore(this, 0);
	//}
	*/

	return true;
}

//玩家最大牌型
bool CServerUserItem::ModifyMaxCardType(BYTE cbCurType, BYTE aryData[], BYTE cbPokerCount)
{
	ASSERT(cbPokerCount <= MAX_CARD_DATA_COUNT);
	if (cbPokerCount > MAX_CARD_DATA_COUNT)
	{
		return false;
	}

	m_cbMaxCardType = cbCurType;
	for (BYTE i = 0; i < cbPokerCount; i++)
	{
		if (i >= MAX_CARD_DATA_COUNT)
		{
			ASSERT(FALSE);
			return false;
		}

		m_aryCardData[i] = aryData[i];
	}

	return true;
}

//解除绑定
bool CServerUserItem::DetachBindStatus()
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bClientReady=false;

	//连接属性
	m_wBindIndex=INVALID_WORD;

	return true;
}

//银行操作
bool CServerUserItem::ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue)
{
	//设置积分
	m_UserInfo.lScore+=lScore;
	m_UserInfo.lInsure+=lInsure;

	//记录信息
	m_RecordInfo.lScore+=lScore;
	m_RecordInfo.lInsure+=lInsure;
	m_RecordInfo.lRevenue+=lRevenue;
	
	//带入积分
	if (m_lNextPutScore > 0)
	{
		// 总积分
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// 总积分大于默认带入积分，带入默认积分值
		if (lTotalScore > m_lNextPutScore)
		{
			m_UserInfo.lScore = m_lNextPutScore;
			m_lTrusteeScore = (lTotalScore - m_lNextPutScore); // m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0;
		}

		m_lNextPutScore = 0;
	}
	else
	{
		SCORE lPutScore = m_lDefPutScore;// *m_iExchangeRatio;
		SCORE lLowPutScore = m_lLowPutScore;// *m_iExchangeRatio;
		if (lLowPutScore > 0 && lPutScore > 0)
		{
			// 身上积分小于带入积分限制，做带入调整
			if (m_UserInfo.lScore < lLowPutScore)
			{
				// 总积分
				SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

				// 总积分大于默认带入积分，带入默认积分值
				if (lTotalScore > lPutScore)
				{
					m_UserInfo.lScore = lPutScore;
					m_lTrusteeScore = (lTotalScore - lPutScore); // m_iExchangeRatio;
				}
				else
				{
					m_UserInfo.lScore = lTotalScore;
					m_lTrusteeScore = 0;
				}
			}
		}
	}

	////托管积分
	//if (m_lRestrictScore>0L)
	//{
	//	//变量定义
	//	SCORE lTotalScore=m_UserInfo.lScore+m_lTrusteeScore;

	//	//托管调整
	//	if (lTotalScore>m_lRestrictScore)
	//	{
	//		m_UserInfo.lScore=m_lRestrictScore;
	//		m_lTrusteeScore=lTotalScore-m_lRestrictScore;
	//	}
	//	else
	//	{
	//		m_lTrusteeScore=0L;
	//		m_UserInfo.lScore=lTotalScore;
	//	}
	//}

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	//if (m_pIServerUserItemSink!=NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, SCORE_REASON_INSURE);
	if (m_pIServerUserItemSink != NULL)
	{
		m_pIServerUserItemSink->OnEventUserItemScore(this, 0);
	}

	return true;
}

//金币带入
SCORE CServerUserItem::TakeGold(SCORE lScore, bool isAtOnce)
{
	if (lScore <= 0) return lScore;

	// 立刻带入
	if (isAtOnce)
	{
		// 总积分
		SCORE lTotalScore = m_UserInfo.lScore + m_lTrusteeScore;// *m_iExchangeRatio;

		// 总积分大于默认带入积分，带入默认积分值
		if (lTotalScore > lScore)
		{
			m_UserInfo.lScore = lScore;
			m_lTrusteeScore = (lTotalScore - lScore); // m_iExchangeRatio;
		}
		else
		{
			m_UserInfo.lScore = lTotalScore;
			m_lTrusteeScore = 0L;
		}
	}
	else
	{
		m_lNextPutScore = lScore;
	}

	return m_UserInfo.lScore;
}

//其他积分消费
bool CServerUserItem::WriteOtherScore(SCORE lScore, BYTE cbType)
{
	//效验状态
	ASSERT((m_UserInfo.dwUserID != 0L));
	if (m_UserInfo.dwUserID == 0L) return false;
	
	SCORE lTrusteeScore = m_lTrusteeScore;// *m_iExchangeRatio;
	SCORE lAllScore = m_UserInfo.lScore + lTrusteeScore;

	//效验积分
	ASSERT((lScore >= 0L) && (lAllScore >= lScore));
	if ((lScore < 0L) || (lAllScore < lScore)) return false;

	//设置用户
	if (lTrusteeScore < lScore)
	{
		//积分锁定
		lScore -= lTrusteeScore;
		m_UserInfo.lScore -= lScore;
		m_lTrusteeScore = 0L;
	}
	else
	{
		//托管锁定
		m_lTrusteeScore -= lScore;// / m_iExchangeRatio;
	}

	//设置标志
	m_bModifyScore = true;

	//记录信息
	m_RecordInfo.lScore -= lScore;

	//分数变更
	m_VariationInfo.lScore -= lScore;

	//发送状态
	ASSERT(m_pIServerUserItemSink != NULL);
	if (m_pIServerUserItemSink != NULL) m_pIServerUserItemSink->OnEventUserItemScore(this, cbType);

	return true;
}

//设置参数
bool CServerUserItem::SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser, bool bClientReady)
{
	//效验状态
	ASSERT(m_UserInfo.dwUserID!=0L);
	if (m_UserInfo.dwUserID==0L) return false;

	//用户属性
	m_bAndroidUser=bAndroidUser;
	m_bClientReady=bClientReady;

	//连接属性
	m_wBindIndex=wBindIndex;
	m_dwClientAddr=dwClientAddr;
	lstrcpyn(m_szMachineID,szMachineID,CountArray(m_szMachineID));

	return true;
}

//修改权限
VOID CServerUserItem::ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind)
{
	//添加权限
	m_dwUserRight |= dwAddRight;

	//删除权限
	m_dwUserRight &= ~dwRemoveRight;

	//发送状态
	ASSERT(m_pIServerUserItemSink!=NULL);
	if (m_pIServerUserItemSink != NULL)
	{
		m_pIServerUserItemSink->OnEventUserItemRight(this, dwAddRight, dwRemoveRight, cbRightKind);
	}

	return;
}

//修改救济金信息
VOID CServerUserItem::SetBenefitInfo(tagSendBenefitSuc* pbenefit)
{
	if (pbenefit == NULL) return;

	ASSERT(pbenefit->dwUserID == m_UserInfo.dwUserID);
	m_UserInfo.cbBenefitTimes = pbenefit->cbBenefitTimes;
	CopyMemory(&m_UserInfo.LastBenefitTime, &pbenefit->LastBenefitTime, sizeof(SYSTEMTIME));

	return;
}

//设置道具
void CServerUserItem::SetUserProperty(tagUserProperty &Property)
{
	//查找现存
	if (m_UserProperty.GetCount() > 0)
	{
		//查找任务
		for (INT_PTR nIndex = 0; nIndex < m_UserProperty.GetCount(); nIndex++)
		{
			if (m_UserProperty[nIndex].wPropertyUseMark == Property.wPropertyUseMark)
			{
				m_UserProperty.RemoveAt(nIndex);
				break;
			}
		}
	}

	m_UserProperty.Add(Property);

	return;
}

//增加道具
void CServerUserItem::AddUserProperty(tagUserProperty &Property)
{
	//查找现存
	if (m_UserProperty.GetCount() > 0)
	{
		//查找任务
		for (INT_PTR nIndex = 0; nIndex < m_UserProperty.GetCount(); nIndex++)
		{
			if (m_UserProperty[nIndex].wPropertyUseMark == Property.wPropertyUseMark)
			{
				m_UserProperty[nIndex].wPropertyCount += Property.wPropertyCount;
				return;
			}
		}
	}

	m_UserProperty.Add(Property);
}

//使用道具
EN_PROPERTY_ERROR CServerUserItem::UsedProperty(CMD_GR_C_Property *pProperty, SYSTEMTIME &SystemTime, DWORD &dwPropDBID)
{
	if (pProperty == NULL) return EN_PROPERTY_ERROR::EN_PROP_NULL;

	//查找现存
	if (m_UserProperty.GetCount() <= 0) return EN_PROPERTY_ERROR::EN_PROP_NULL;

	//查找道具
	tagUserProperty* ptmpProp = NULL;

	for (INT_PTR nIndex = 0; nIndex < m_UserProperty.GetCount(); nIndex++)
	{
		if (m_UserProperty[nIndex].wPropertyUseMark == pProperty->wPropertyIndex)
		{
			ptmpProp = &m_UserProperty[nIndex];
			break;
		}
	}

	if (ptmpProp != NULL)
	{
		// 实际数量小于使用数量
		if (ptmpProp->wPropertyCount < pProperty->wItemCount) return EN_PROPERTY_ERROR::EN_PROP_NUMBER;

		//过期判断
		COleDateTime dTimeNow(SystemTime);
		COleDateTime dTimeOver(ptmpProp->OverTime);

		//有效时间值判断时间差; 无效时间表示永久可用
		if (dTimeOver.m_status == COleDateTime::valid)
		{
			COleDateTimeSpan dTimeSpan = dTimeNow - dTimeOver;
			double nSecnonSpan = dTimeSpan.GetTotalSeconds();

			//过期
			if (nSecnonSpan > 0)
			{
				return EN_PROPERTY_ERROR::EN_PROP_OVER;
			}
		}

		//正常使用
		if (pProperty->wItemCount > 0)
		{
			ptmpProp->wPropertyCount -= pProperty->wItemCount;
		}

		dwPropDBID = ptmpProp->dwDBID;

		return EN_PROPERTY_ERROR::EN_PROP_SUCC;
	}

	return EN_PROPERTY_ERROR::EN_PROP_NULL;
}

//重置数据
VOID CServerUserItem::ResetUserItem()
{
	//附加变量
	m_dwUserRight=0L;
	m_dwMasterRight=0L;

	//托管变量
	m_lTrusteeScore=0L;
	m_lRestrictScore=0L;
	m_lFrozenedScore=0L;
	m_lFrozenedScore=0L;

	//登录信息
	m_dwDBQuestID=0L;
	m_dwLogonTime=0L;
	m_dwInoutIndex=INVALID_DWORD;

	//最大牌型
	m_cbMaxCardType = 0;
	ZeroMemory(m_aryCardData, sizeof(m_aryCardData));

	//系统属性
	m_wBindIndex=INVALID_WORD;
	m_dwClientAddr=INADDR_ANY;
	ZeroMemory(m_szMachineID,sizeof(m_szMachineID));

	//客户类型
	m_bAndroidUser=false;

	//比赛信息
	m_dwSignUpTime=0;
	m_cbSignUpStatus=0;
	m_pMatchData=NULL;

	//辅助变量
	m_bTrusteeUser=false;
	m_bClientReady=false;
	m_bModifyScore=false;
	ZeroMemory(m_szLogonPass,sizeof(m_szLogonPass));

	//属性变量
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_UserRule,sizeof(m_UserRule));
	ZeroMemory(&m_ScoreFormer,sizeof(m_ScoreFormer));
	ZeroMemory(&m_UserProperty,sizeof(m_UserProperty));
	m_UserInfo.wTableID=INVALID_TABLE;
	m_UserInfo.wLastTableID=INVALID_TABLE;
	m_UserInfo.wChairID=INVALID_CHAIR;

	//记录信息
	ZeroMemory(&m_RecordInfo,sizeof(m_RecordInfo));
	ZeroMemory(&m_VariationInfo,sizeof(m_VariationInfo));

	m_UserProperty.RemoveAll();

	//接口变量
	m_pIServerUserItemSink=NULL;

	return;
}

//是否可以使用牌库
bool CServerUserItem::canUseCardLib()
{
	if (m_cbCardLibEnabled == 0)
	{
		return false;
	}

	CTime cTime = CTime::GetCurrentTime();
	DWORD dwCurTime = cTime.GetTime();
	if (dwCurTime >= m_dwStartLibTime && dwCurTime <= m_dwStopLibTime)
	{
		return true;
	}

	return false;
}

//获取牌库索引
DWORD CServerUserItem::getCardLibIndex()
{
	return m_dwCardLibIndex;
}

//设置牌库索引
void CServerUserItem::setCardLibIndex(DWORD dwCardLibIndex)
{
	m_dwCardLibIndex = dwCardLibIndex;
}

//累加牌库索引
void CServerUserItem::addCardLibIndex()
{
	//if (++m_dwCardLibIndex >= g_dwCardLibCount)
	//{
	//	m_dwCardLibIndex = 0;
	//}

	if (++m_dwCardLibIndex > g_dwCardLibCustomCount)
	{
		m_dwCardLibIndex = 1;
	}
}

//设置牌库可用性
void CServerUserItem::setCardLibEnabled(bool bEnabled)
{
	m_cbCardLibEnabled = bEnabled ? 1 : 0;
}

//设置牌库使用时间段
void CServerUserItem::setUseCardLibTime(DWORD dwStartTime, DWORD dwEndTime)
{
	m_dwStartLibTime = dwStartTime;
	m_dwStopLibTime = dwEndTime;
}


//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerUserManager::CServerUserManager()
{
	//组件接口
	m_pIServerUserItemSink=NULL;

	//设置质数
	m_UserIDMap.InitHashTable(PRIME_SERVER_USER);

	return;
}

//析构函数
CServerUserManager::~CServerUserManager()
{
	//释放用户
	for (INT_PTR i=0;i<m_UserItemStore.GetCount();i++) m_UserItemStore[i]->Release();
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++) m_UserItemArray[i]->Release();

	//删除数据
	m_UserIDMap.RemoveAll();
	m_UserItemStore.RemoveAll();
	m_UserItemArray.RemoveAll();

	return;
}

//接口查询
VOID * CServerUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServerUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IServerUserManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool CServerUserManager::SetServerUserItemSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink)!=NULL);
		m_pIServerUserItemSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IServerUserItemSink);

		//成功判断
		if (m_pIServerUserItemSink==NULL) return false;
	}
	else m_pIServerUserItemSink=NULL;

	return true;
}

//枚举用户
IServerUserItem * CServerUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemArray.GetCount()) return NULL;
	return m_UserItemArray[wEnumIndex];
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_UserIDMap[dwUserID];
}

//机器人数
DWORD CServerUserManager::GetAndroidCount()
{
	//变量定义
	DWORD dwAndroidCount=0;
	CServerUserItem * pServerUserItem=NULL;

	//搜索用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->IsAndroidUser() == true)
		{
			dwAndroidCount++;
		}
	}

	return dwAndroidCount;
}

//查找用户
IServerUserItem * CServerUserManager::SearchUserItem(LPCTSTR pszNickName)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//搜索用户
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		pServerUserItem=m_UserItemArray[i];
		if (pServerUserItem->ContrastNickName(pszNickName) == true)
		{
			return pServerUserItem;
		}
	}

	return NULL;
}

//删除用户
bool CServerUserManager::DeleteUserItem()
{
	//存储对象
	m_UserItemStore.Append(m_UserItemArray);

	//删除对象
	m_UserIDMap.RemoveAll();
	m_UserItemArray.RemoveAll();

	return true;
}

//删除用户
bool CServerUserManager::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserStatus()==US_NULL));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetUserStatus()!=US_NULL)) return false;

	TRACE2("CServerUserManager::DeleteUserItem %d Name %s", pIServerUserItem, pIServerUserItem->GetNickName());

	//变量定义
	CServerUserItem * pTempUserItem=NULL;
	DWORD dwUserID=pIServerUserItem->GetUserID();

	//寻找对象
	for (INT_PTR i=0;i<m_UserItemArray.GetCount();i++)
	{
		//获取用户
		pTempUserItem=m_UserItemArray[i];
		if (pIServerUserItem!=pTempUserItem) continue;

		//重置对象
		pTempUserItem->ResetUserItem();

		//删除对象
		m_UserItemArray.RemoveAt(i);
		m_UserIDMap.RemoveKey(dwUserID);
		m_UserItemStore.Add(pTempUserItem);

		return true;
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//插入用户
bool CServerUserManager::InsertUserItem(IServerUserItem * * pIServerUserResult, tagUserInfo & UserInfo,tagUserInfoPlus & UserInfoPlus)
{
	//变量定义
	CServerUserItem * pServerUserItem=NULL;

	//获取指针
	if (m_UserItemStore.GetCount()>0)
	{
		INT_PTR nItemPostion=m_UserItemStore.GetCount()-1;
		pServerUserItem=m_UserItemStore[nItemPostion];
		m_UserItemStore.RemoveAt(nItemPostion);

		//重置对象
		pServerUserItem->ResetUserItem();		
	}
	else
	{
		try
		{
			pServerUserItem=new CServerUserItem;
		}
		catch (...)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置接口
	pServerUserItem->m_pIServerUserItemSink=m_pIServerUserItemSink;

	//原始变量
	pServerUserItem->m_ScoreFormer.lScore=UserInfo.lScore;
	pServerUserItem->m_ScoreFormer.dwWinCount=UserInfo.dwWinCount;
	pServerUserItem->m_ScoreFormer.dwLostCount=UserInfo.dwLostCount;
	pServerUserItem->m_ScoreFormer.dwDrawCount=UserInfo.dwDrawCount;
	pServerUserItem->m_ScoreFormer.dwFleeCount=UserInfo.dwFleeCount;
	pServerUserItem->m_ScoreFormer.dwExperience=UserInfo.dwExperience;

	//属性变量
	CopyMemory(&pServerUserItem->m_UserInfo,&UserInfo,sizeof(UserInfo));
	ZeroMemory(&pServerUserItem->m_RecordInfo,sizeof(pServerUserItem->m_RecordInfo));
	ZeroMemory(&pServerUserItem->m_VariationInfo,sizeof(pServerUserItem->m_VariationInfo));

	//登录信息
	pServerUserItem->m_dwLogonTime=UserInfoPlus.dwLogonTime;
	pServerUserItem->m_dwInoutIndex=UserInfoPlus.dwInoutIndex;

	//用户权限
	pServerUserItem->m_dwUserRight=UserInfoPlus.dwUserRight;
	pServerUserItem->m_dwMasterRight=UserInfoPlus.dwMasterRight;
	pServerUserItem->m_lRestrictScore=UserInfoPlus.lRestrictScore;

	//带入设置
	pServerUserItem->m_lDefPutScore = UserInfoPlus.lDefPutScore;
	pServerUserItem->m_lLowPutScore = UserInfoPlus.lLowPutScore;
	//pServerUserItem->m_iExchangeRatio = UserInfoPlus.iExchangeRatio;

	//最大牌型
	pServerUserItem->m_cbMaxCardType = UserInfoPlus.cbMaxCardType;
	CopyMemory(pServerUserItem->m_aryCardData, UserInfoPlus.aryCardData, sizeof(UserInfoPlus.aryCardData));

	//连接信息
	pServerUserItem->m_wBindIndex=UserInfoPlus.wBindIndex;
	pServerUserItem->m_dwClientAddr=UserInfoPlus.dwClientAddr;
	lstrcpyn(pServerUserItem->m_szMachineID,UserInfoPlus.szMachineID,CountArray(pServerUserItem->m_szMachineID));

	//辅助变量
	pServerUserItem->m_bClientReady=false;
	pServerUserItem->m_bModifyScore=false;
	pServerUserItem->m_bTrusteeUser=false;
	pServerUserItem->m_bAndroidUser=UserInfoPlus.bAndroidUser;
	lstrcpyn(pServerUserItem->m_szLogonPass,UserInfoPlus.szPassword,CountArray(pServerUserItem->m_szLogonPass));

	////托管调整
	//if ((UserInfoPlus.lRestrictScore>0L)&&(UserInfo.lScore>UserInfoPlus.lRestrictScore))
	//{
	//	pServerUserItem->m_UserInfo.lScore=UserInfoPlus.lRestrictScore;
	//	pServerUserItem->m_lTrusteeScore=UserInfo.lScore-UserInfoPlus.lRestrictScore;
	//}

	//带入调整
	SCORE lputScore = UserInfoPlus.lDefPutScore * UserInfoPlus.iExchangeRatio;
	if (lputScore > 0)
	{
		ASSERT(UserInfoPlus.lDefPutScore > 0);

		if (UserInfo.lScore >= UserInfoPlus.lDefPutScore)
		{
			pServerUserItem->m_UserInfo.lScore = lputScore;
			pServerUserItem->m_lTrusteeScore = UserInfo.lScore - UserInfoPlus.lDefPutScore;
		}
		else
		{
			pServerUserItem->m_UserInfo.lScore = UserInfo.lScore * UserInfoPlus.iExchangeRatio;
			pServerUserItem->m_lTrusteeScore = 0;
		}
	}

	//插入用户
	m_UserItemArray.Add(pServerUserItem);
	m_UserIDMap[UserInfo.dwUserID]=pServerUserItem;

	//设置变量
	*pIServerUserResult=pServerUserItem;

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
