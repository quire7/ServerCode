#pragma once
#include "Stdafx.h"
#include "GameLogic.h"
class CRobottLogic
{
public:
	CRobottLogic(void);
	~CRobottLogic(void);
//����
	BYTE							m_cbRobotsType;							//�����˲�������
	BYTE							m_cbRobotWin;							//��Ӯ	
	BYTE							m_cbRobotsWin[GAME_PLAYER];
	BYTE							m_cbRobotCardType[GAME_PLAYER][2];		//��������
	BYTE							m_cbMyCardType[6];						//�Լ�����������ʲô�ȼ�����
	BYTE							m_cbEndCardData[GAME_PLAYER][5];

	LONG							m_lRobotAdd[INT_ROBOT_ADD];				//��ע��
	LONG							m_lRobotQuAnYa[INT_ROBOT_QUANYA];		//ȫѹ��
	LONG							m_lRobotGiveUp[INT_ROBOT_GIVEUP];		//������

	LONG							m_lBei[INT_ROBOT_BEI];					//������������������
	LONG							m_lCellScore;							//��Ԫ��ע
	LONG							m_lTurnLessScore;						//��С��ע
	LONG							m_lAddLessScore;						//����Сע	
	
	int 							m_iBalanceCount;						//ƽ�����	
	int 							m_iLastBalanceCnt;						//��һ�ֵ�ƽ�����	
	LONG							m_lRobotsScore;							//������ע��
	CGameLogic						m_GameLogic;
	class CAndroidUserItemSink		*m_pAndroidUserItemSink;					//�����˲���
public:
	//���ͻ�������Ϣ��ָ��
	int SetRobotAlgorithm(CAndroidUserItemSink* _AndroidUserItemSink);
	//
	int Robots(WORD wChairID, LONG lScore,BYTE ID);
	//
	int RobotsLostAlgorith(WORD wChairID, LONG lScore,BYTE ID);
	//����������Ƶ�����ʤ��
	BYTE RobotAlgorithm(WORD wChairID, LONG lScore,BYTE ID) ;
	//������� 
	int RobotsRate( BYTE cbValue,BYTE ID, WORD wChairID );
	//��ȡ��ǰ�ִ����Ƶȼ�
	bool SetLevel(WORD wChairID) ; 
	void OnClear();
};
