#pragma once
#include "Stdafx.h"
#include "GameLogic.h"
class CRobottLogic
{
public:
	CRobottLogic(void);
	~CRobottLogic(void);
//变量
	BYTE							m_cbRobotsType;							//机器人操作类型
	BYTE							m_cbRobotWin;							//输赢	
	BYTE							m_cbRobotsWin[GAME_PLAYER];
	BYTE							m_cbRobotCardType[GAME_PLAYER][2];		//手牌类型
	BYTE							m_cbMyCardType[6];						//自己的手牌属于什么等级的牌
	BYTE							m_cbEndCardData[GAME_PLAYER][5];

	LONG							m_lRobotAdd[INT_ROBOT_ADD];				//加注率
	LONG							m_lRobotQuAnYa[INT_ROBOT_QUANYA];		//全压率
	LONG							m_lRobotGiveUp[INT_ROBOT_GIVEUP];		//放弃率

	LONG							m_lBei[INT_ROBOT_BEI];					//设置扎进花基础倍数
	LONG							m_lCellScore;							//单元下注
	LONG							m_lTurnLessScore;						//最小下注
	LONG							m_lAddLessScore;						//加最小注	
	
	int 							m_iBalanceCount;						//平衡次数	
	int 							m_iLastBalanceCnt;						//上一轮的平衡次数	
	LONG							m_lRobotsScore;							//机器人注数
	CGameLogic						m_GameLogic;
	class CAndroidUserItemSink		*m_pAndroidUserItemSink;					//机器人操作
public:
	//传送机器人消息类指针
	int SetRobotAlgorithm(CAndroidUserItemSink* _AndroidUserItemSink);
	//
	int Robots(WORD wChairID, LONG lScore,BYTE ID);
	//
	int RobotsLostAlgorith(WORD wChairID, LONG lScore,BYTE ID);
	//计算机器人牌的最终胜负
	BYTE RobotAlgorithm(WORD wChairID, LONG lScore,BYTE ID) ;
	//计算比率 
	int RobotsRate( BYTE cbValue,BYTE ID, WORD wChairID );
	//获取当前轮次手牌等级
	bool SetLevel(WORD wChairID) ; 
	void OnClear();
};
