#ifndef PRIVATE_TABLE_INFO_FILE
#define PRIVATE_TABLE_INFO_FILE

#pragma once

//�����ļ�
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
	Jiang_1,		//1��
	Jiang_2			//2��
};

enum ePrivateTableCommonRule
{
	ePrivateTableCommonRule_CardAA		= 1,		//����AA;
	ePrivateTableCommonRule_NOSameIPAddr= 1 << 31,	//������ͬIP��ַ;
};

enum ePrivateRoomScoreType
{
	ePrivateRoomScoreType_Default		= 1,			//Ĭ������;
	ePrivateRoomScoreType_SetOneTime	= 1 << 1,		//���뷿��ʱ����һ��;
	ePrivateRoomScoreType_EveryTime		= 1 << 2,		//ÿ�ֿ�ʼʱ������һ��;
};

//��ʱ��
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
	DWORD			dwCreaterUserID;		// ������
	DWORD			dwRoomNum;				// ����ID
	DWORD			dwPlayCout;				// Ŀ�����
	DWORD			dwPlayCost;				// ���ĵ���
	bool			bStart;					// �Ƿ�ʼ
	bool			bInEnd;					// �Ƿ����
	float			fAgainPastTime;			// ������������´������ʱ��
	float			fCreateTime;			// ���䴴��ʱ��

	std::string		kHttpChannel;

	BYTE			cbRoomType;				// �������� ˽�˳�����ͨ��

	DWORD			dwStartPlayCout;		// ʵ�ʴ�������
	DWORD			dwFinishPlayCout;		// ͳ�ƴ�������(�磺�齫�е���ׯ��ʵ�ʾ�����ͳ�ƣ���ɾ����в�ͳ��)

	BYTE			bPlayCoutIdex;			//��Ҿ���
	BYTE			bGameTypeIdex;			//��Ϸ����
	DWORD			dwGameRuleIdex;			//��Ϸ����(Ĭ��0x0001λ���Ƿ���AA,�������,���λ��Ϊ0);

	BYTE			cbEnterRoomScoreType;	//������ʱ�ķ�������;
	SCORE			lInitEnterRoomScore;	//���뷿��ʱ�ķ���;

	//SCORE			lPlayerMaxScore[MAX_CHAIR];		// ���Ӯ����
	SCORE			lPlayerWinLose[MAX_CHAIR];		// �ܻ���
	//BYTE			lPlayerAction[MAX_CHAIR][MAX_PRIVATE_ACTION];//����ͳ��

	float			fDismissPastTime;
	std::vector<DWORD> kDismissChairID;
	std::vector<DWORD> kNotAgreeChairID;

	tagPrivateRandTotalRecord kTotalRecord;
};

#endif