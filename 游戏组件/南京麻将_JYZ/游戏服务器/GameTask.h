#pragma once

////////////////////////////////////////////////////////////////////////////////////////
//短线任务
enum SGameTask{ STask_Begin=1,STask_1W_1=1,STask_2W_1,STask_3W_1,STask_4W_1,STask_5W_1,STask_6W_1,STask_7W_1,STask_8W_1,STask_9W_1,STask_1S_1,STask_2S_1,STask_3S_1,STask_4S_1,STask_5S_1, \
STask_6S_1,STask_7S_1,STask_8S_1,STask_9S_1,STask_1T_1,STask_2T_1,STask_3T_1,STask_4T_1,STask_5T_1,STask_6T_1,STask_7T_1,STask_8T_1,STask_9T_1,STask_ZD_1,STask_ZN_1,STask_ZX_1,STask_ZB_1, \
STask_JZ_1,STask_JF_1,STask_JB_1,STask_HC_1,STask_HX_1,STask_HQ_1,STask_HD_1,STask_HM_1,STask_HL_1,STask_HZ_1,STask_HJ_1,STask_1W_2,STask_2W_2,STask_3W_2,STask_4W_2,STask_5W_2,STask_6W_2, \
STask_7W_2,STask_8W_2,STask_9W_2,STask_1S_2,STask_2S_2,STask_3S_2,STask_4S_2,STask_5S_2,STask_6S_2,STask_7S_2,STask_8S_2,STask_9S_2,STask_1T_2,STask_2T_2,STask_3T_2,STask_4T_2,STask_5T_2, \
STask_6T_2,STask_7T_2,STask_8T_2,STask_9T_2,STask_ZD_2,STask_ZN_2,STask_ZX_2,STask_ZB_2,STask_JZ_2,STask_JF_2,STask_JB_2,STask_H_2,STask_1W_3,STask_2W_3,STask_3W_3,STask_4W_3,STask_5W_3, \
STask_6W_3,STask_7W_3,STask_8W_3,STask_9W_3,STask_1S_3,STask_2S_3,STask_3S_3,STask_4S_3,STask_5S_3,STask_6S_3,STask_7S_3,STask_8S_3,STask_9S_3,STask_1T_3,STask_2T_3,STask_3T_3,STask_4T_3, \
STask_5T_3,STask_6T_3,STask_7T_3,STask_8T_3,STask_9T_3,STask_ZD_3,STask_ZN_3,STask_ZX_3,STask_ZB_3,STask_JZ_3,STask_JF_3,STask_JB_3,STask_H_3,STask_1W_4,STask_2W_4,STask_3W_4,STask_4W_4, \
STask_5W_4,STask_6W_4,STask_7W_4,STask_8W_4,STask_9W_4,STask_1S_4,STask_2S_4,STask_3S_4,STask_4S_4,STask_5S_4,STask_6S_4,STask_7S_4,STask_8S_4,STask_9S_4,STask_1T_4,STask_2T_4,STask_3T_4, \
STask_4T_4,STask_5T_4,STask_6T_4,STask_7T_4,STask_8T_4,STask_9T_4,STask_ZD_4,STask_ZN_4,STask_ZX_4,STask_ZB_4,STask_JZ_4,STask_JF_4,STask_JB_4,STask_H_4,STask_H_5,STask_H_6,STask_H_7, \
STask_Gang_1,STask_Gang_2,STask_Hu_MenQing,STask_Hu_XiaoHu,STask_Hu_HunYiSe,STask_Hu_QingYiSe,STask_Hu_DuiDuiHu,STask_Hu_QQDuDiao,STask_Hu_QiDui,STask_Hu_HHQiDui,STask_Hu_YaJue,STask_End };
//长线任务
enum LGameTask{ LTask_Begin=1001,LTask_H_8=1001,LTask_Hu_CHHQiDui,LTask_Hu_CCHHQiDui,LTask_Hu_WuHuaGuo,LTask_Hu_DGKaiHua,LTask_Hu_XGKaiHua,LTask_Hu_TianHu,LTask_Hu_DiHu,LTask_Hu_HDLY,LTask_End };

class CGameTask
{
private:
	bool					m_bShortGameTaskStatus[GAME_PLAYER][STask_End-STask_Begin];	//短线任务完成状态
	bool					m_bLongGameTaskStatus[GAME_PLAYER][LTask_End-LTask_Begin];	//长线任务完成状态
	BYTE					m_cbGangCardCount[GAME_PLAYER];								//玩家杠牌次数

public:
	CGameTask(void);
	~CGameTask(void);

///<任务接口
public:
	//任务是否完成
	bool IsTaskCompleted( WORD wChairID, DWORD dwTaskID, IN OUT DWORD &dwRecord );

///<功能函数
public:
	//重置变量
	void ResetAllData();
	//起手抓到的牌
	void GetCardCount(WORD wChairID, BYTE cbCardValue, BYTE cbCardCount);
	//起手抓花牌
	void GetHuaCardCount(WORD wChairID, BYTE cbCardCount);
	//设置杠牌次数
	void GetGangCardCount(WORD wChairID);
	//设置玩家胡牌结果
	void SetUserHuPaiKind(WORD wChairID, const DWORD dwHuPaiKind);


	//打印任务状态
	void WriteGameTaskInfo(WORD wChairID);
};
