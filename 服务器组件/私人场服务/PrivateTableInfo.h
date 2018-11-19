#ifndef PRIVATE_TABLE_INFO_FILE
#define PRIVATE_TABLE_INFO_FILE

#pragma once

//引入文件
#include "CTableFramePrivate.h"
#include "PrivateServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////////////

enum RoomType
{
	Type_Private,
	Type_Public,
};

enum emPlayCount
{
	Jiang_1,		//1将
	Jiang_2			//2将
};

enum ePrivateTableCommonRule
{
	ePrivateTableCommonRule_CardAA		= 1,		//房卡AA;
	ePrivateTableCommonRule_NOSameIPAddr= 1 << 31,	//不可以同IP地址;
};

enum ePrivateRoomScoreType
{
	ePrivateRoomScoreType_Default		= 1,			//默认类型;
	ePrivateRoomScoreType_SetOneTime	= 1 << 1,		//进入房间时设置一次;
	ePrivateRoomScoreType_EveryTime		= 1 << 2,		//每局开始时都设置一次;
};

//定时赛
class PrivateTableInfo 
{
public:
	PrivateTableInfo();
	~PrivateTableInfo();

	void restValue();
	void restAgainValue();
	void newRandChild();
	WORD getChairCout();
	void setRoomNum(DWORD RoomNum);
	void writeSocre(tagScoreInfo ScoreInfoArray[], WORD wScoreCount,DataStream& daUserDefine);

	bool IsCardAARule();
	
	bool IsEveryTimeRoomScoreType();
	bool IsSetOneTimeRoomScoreType();

	bool IsCanJoinTable(IServerUserItem * pIServerUserItem, CString& refStrMsg);

	ITableFrame*	pITableFrame;			
	DWORD			dwCreaterUserID;		// 创建者
	DWORD			dwRoomNum;				// 房间ID
	DWORD			dwPlayCout;				// 目标局数
	DWORD			dwPlayCost;				// 消耗点数
	bool			bStart;					// 是否开始
	bool			bInEnd;					// 是否结束
	float			fAgainPastTime;			// 房间结束和重新创建间隔时间
	float			fCreateTime;			// 房间创建时间

	std::string		kHttpChannel;

	BYTE			cbRoomType;				// 房间类型 私人场和普通场

	DWORD			dwStartPlayCout;		// 实际打完数量
	DWORD			dwFinishPlayCout;		// 统计打完数量(如：麻将中的连庄，实际局数中统计，完成局数中不统计)

	BYTE			bPlayCoutIdex;			//玩家局数
	BYTE			bGameTypeIdex;			//游戏类型
	DWORD			dwGameRuleIdex;			//游戏规则(默认0x0001位置是房卡AA,如果不是,这个位置为0);

	BYTE			cbEnterRoomScoreType;	//进房间时的分数类型;
	SCORE			lInitEnterRoomScore;	//进入房间时的分数;

	//SCORE			lPlayerMaxScore[MAX_CHAIR];		// 最大赢积分
	SCORE			lPlayerWinLose[MAX_CHAIR];		// 总积分
	//BYTE			lPlayerAction[MAX_CHAIR][MAX_PRIVATE_ACTION];//类型统计

	float			fDismissPastTime;
	std::vector<DWORD> kDismissChairID;
	std::vector<DWORD> kNotAgreeChairID;

	tagPrivateRandTotalRecord kTotalRecord;
};

#endif