#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////
//��������
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����

////////////////////////////////////////////////////////////////////////////////////////
//��Ŀ����
#define MAX_COUNT					20									//�����Ŀ
#define FULL_COUNT					54									//ȫ����Ŀ
#define BACK_COUNT					3									//������Ŀ
#define NORMAL_COUNT				17									//������Ŀ
#define GOOD_CARD_COUNT				38									//������Ŀ

////////////////////////////////////////////////////////////////////////////////////////
//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

////////////////////////////////////////////////////////////////////////////////////////
//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//������Ŀ
	BYTE 							cbThreeCount;						//������Ŀ
	BYTE 							cbDoubleCount;						//������Ŀ
	BYTE							cbSingleCount;						//������Ŀ
	BYTE							cbFourCardData[MAX_COUNT];			//�����˿�
	BYTE							cbThreeCardData[MAX_COUNT];			//�����˿�
	BYTE							cbDoubleCardData[MAX_COUNT];		//�����˿�
	BYTE							cbSingleCardData[MAX_COUNT];		//�����˿�
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT];				//����˿�
};

//��ʾ���ƽ��
struct tagPromptCardResult
{
	BYTE							cbResultCount;						//��ʾ�����Ŀ
	BYTE							cbCardCount[MAX_COUNT];				//�˿���Ŀ
	BYTE							cbCardData[MAX_COUNT][MAX_COUNT];	//����˿�
};

//������Ϣ�ṹ
struct tagPromptCardInfo
{
	tagPromptCardResult				stRocketResult;						//���
	tagPromptCardResult				stBombResult;						//ը��
	tagPromptCardResult				stFourTwoResult;					//�Ĵ���
	tagPromptCardResult				cbPlaneResult;						//�ɻ�
	tagPromptCardResult				stDoubleLineResult;					//���ö�
	tagPromptCardResult				stSingleLineResult;					//˳��
};

//�ƿ�����
struct LandCardList
{
	std::vector<BYTE>				vecLandListCard;					
};


//////////////////////////////////////////////////////////////////////////
//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//�˿�����
	static const BYTE				m_cbGoodcardData[GOOD_CARD_COUNT];	//��������

public:
	static std::vector<LandCardList> m_vecLandCard;						//�ƿ�����

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

public:
	//�����ƿ�����
	static void LoadLandCard();
	//ɾ�������˿�
	static bool RemoveAllCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//���ͺ���
public:
	//��ȡ��������
	BYTE GetBackCardType(const BYTE cbBackCardData[], bool bCheckLongTask=false);
	//��ȡ���Ʊ���
	BYTE GetBackCardMulitple(const BYTE cbBackCardType);
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], const BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//�õ�����
	void GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT]);
	//ɾ������
	bool RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount);
	//���һ�����
	void RandArrayBuffer(BYTE cbBufferData[], BYTE cbBufferCount);
	//���ͻ�����
	void SortArrayBuffer(BYTE cbBufferData[], BYTE cbBufferCount);

	//�߼�����
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//��Ч�ж�
	bool IsValidCount(BYTE cbCardCount);
	//��Ч�ж�
	bool IsValidUser(WORD wChairID);
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount, BYTE cbRecordType=CT_ERROR);
	//��������
	bool SearchOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagOutCardResult &OutCardResult);
	//��������
	bool SearchOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagPromptCardResult &OutCardResult);

	//�ڲ�����
public:
	//�����˿�
	bool AnalysebCardData(const BYTE cbCardData[], const BYTE cbCardCount, tagAnalyseResult &AnalyseResult);
	//����ը��
	void GetAllBombCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
	//��������
	void GetAllThreeCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount);
	//��������
	void GetAllDoubleCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount);
	//��������
	void GetAllSingleCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, bool bOnlySingle);
	//����˳��
	void GetAllLineCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount);
	//����˳��
	void GetAllLineCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagPromptCardInfo &PromptCardInfo);
	//��ȡ��Ϣ
	void PromptHandCardEachType(const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagPromptCardInfo &PromptCardInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif