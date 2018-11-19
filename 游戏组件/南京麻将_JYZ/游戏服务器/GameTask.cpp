#include "StdAfx.h"
#include "GameTask.h"
#include "GameLogic.h"

CGameTask::CGameTask(void)
{
	ResetAllData();
}

CGameTask::~CGameTask(void)
{

}

//任务是否完成
bool CGameTask::IsTaskCompleted( WORD wChairID, DWORD dwTaskID, IN OUT DWORD &dwRecord )
{
	ASSERT(dwTaskID>0);
	if (dwTaskID==0) return false;

	//短线任务
	if (dwTaskID>=STask_Begin && dwTaskID<STask_End)
	{
		return m_bShortGameTaskStatus[wChairID][dwTaskID-STask_Begin];
	}
	//长线任务
	else if (dwTaskID>=LTask_Begin && dwTaskID<LTask_End)
	{
		return m_bLongGameTaskStatus[wChairID][dwTaskID-LTask_Begin];
	}
	ASSERT(false);
	return false;
}

//重置变量
void CGameTask::ResetAllData()
{
	memset(m_bShortGameTaskStatus, false, sizeof(m_bShortGameTaskStatus));
	memset(m_bLongGameTaskStatus, false, sizeof(m_bLongGameTaskStatus));
	memset(m_cbGangCardCount, 0, sizeof(m_cbGangCardCount));
}

//起手抓到的牌
void CGameTask::GetCardCount( WORD wChairID, BYTE cbCardValue, BYTE cbCardCount )
{
	ASSERT(wChairID>=0&&wChairID<GAME_PLAYER);
	if (cbCardCount == 1)
	{
		m_bShortGameTaskStatus[wChairID][STask_1W_1-STask_Begin+cbCardValue] = true;
	}
	else if(cbCardCount == 2)
	{
		if (cbCardValue < MAX_NORMAL)
		{
			m_bShortGameTaskStatus[wChairID][STask_1W_2-STask_Begin+cbCardValue] = true;
		}
	}
	else if (cbCardCount == 3)
	{
		if (cbCardValue < MAX_NORMAL)
		{
			m_bShortGameTaskStatus[wChairID][STask_1W_3-STask_Begin+cbCardValue] = true;
		}
	}
	else if (cbCardCount == 4)
	{
		if (cbCardValue < MAX_NORMAL)
		{
			m_bShortGameTaskStatus[wChairID][STask_1W_4-STask_Begin+cbCardValue] = true;
		}		
	}
	else
	{
		ASSERT(FALSE);
	}
}

//起手抓花牌
void CGameTask::GetHuaCardCount( WORD wChairID, BYTE cbCardCount )
{
	ASSERT(wChairID>=0&&wChairID<GAME_PLAYER);
	if (cbCardCount == 2)
	{
		m_bShortGameTaskStatus[wChairID][STask_H_2-STask_Begin];
	}
	else if (cbCardCount == 3)
	{
		m_bShortGameTaskStatus[wChairID][STask_H_3-STask_Begin];
	}
	else if (cbCardCount == 4)
	{
		m_bShortGameTaskStatus[wChairID][STask_H_4-STask_Begin];
	}
	else if (cbCardCount == 5)
	{
		m_bShortGameTaskStatus[wChairID][STask_H_5-STask_Begin];
	}
	else if (cbCardCount == 6)
	{
		m_bShortGameTaskStatus[wChairID][STask_H_6-STask_Begin];
	}
	else if (cbCardCount == 7)
	{
		m_bShortGameTaskStatus[wChairID][STask_H_7-STask_Begin];
	}
	else if (cbCardCount == 8)
	{
		m_bLongGameTaskStatus[wChairID][LTask_H_8-LTask_Begin];
	}
}

//设置杠牌次数
void CGameTask::GetGangCardCount( WORD wChairID )
{
	ASSERT(wChairID>=0&&wChairID<GAME_PLAYER);
	m_cbGangCardCount[wChairID]++;
	if (m_cbGangCardCount[wChairID] == 1)
	{
		m_bShortGameTaskStatus[wChairID][STask_Gang_1-STask_Begin];
	}
	else if (m_cbGangCardCount[wChairID] == 2)
	{
		m_bShortGameTaskStatus[wChairID][STask_Gang_2-STask_Begin];
	}
}

//设置玩家胡牌结果
void CGameTask::SetUserHuPaiKind( WORD wChairID, const DWORD dwHuPaiKind )
{
	/*ASSERT(wChairID>=0&&wChairID<GAME_PLAYER);
	if ((dwHuPaiKind&HU_MENQING) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_MenQing-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_PINGHU) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_XiaoHu-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_HUNYISE) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_HunYiSe-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_QINGYISE) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_QingYiSe-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_DUIDUIHU) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_QiDui-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_QQDUDIAO) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_QQDuDiao-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_QIDUI) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_QiDui-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_SQIDUI) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_HHQiDui-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_YAJUE) != 0)
	{
		m_bShortGameTaskStatus[wChairID][STask_Hu_YaJue-STask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_HHSQIDUI) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_CHHQiDui-LTask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_SQIDUIJNH) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_CCHHQiDui-LTask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_WUHUAGUO) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_WuHuaGuo-LTask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_DGKAIHUA) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_DGKaiHua-LTask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_XGKAIHUA) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_XGKaiHua-LTask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_TIANHU) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_TianHu-LTask_Begin] = true;
	}
	if ((dwHuPaiKind&HU_QIDUI) != 0)
	{
		m_bShortGameTaskStatus[wChairID][LTask_Hu_DiHu-LTask_Begin] = true;
	}*/
	//if ((dwHuPaiKind&HU_HAIDILAOYUE) != 0)
	//{
	//	m_bShortGameTaskStatus[wChairID][LTask_Hu_HDLY-LTask_Begin] = true;
	//}
}

//打印状态
void CGameTask::WriteGameTaskInfo(WORD wChairID)
{
	 //for (int i=STask_Begin; i<STask_End; i++)
	 //{
		// MyLog(_T("玩家ID: %d,  任务ID: %d, 完成状态: %d"), wChairID, i, m_bShortGameTaskStatus[wChairID][i-STask_Begin]);
	 //}

	 //for (int i=LTask_Begin; i<LTask_End; i++)
	 //{
		// MyLog(_T("玩家ID: %d,  任务ID: %d, 完成状态: %d"), wChairID, i, m_bLongGameTaskStatus[wChairID][i-LTask_Begin]);
	 //}
}
