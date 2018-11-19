#ifndef AndroidLogic_h__
#define AndroidLogic_h__
#pragma once

#include "GameLogic.h"

#define MAX_TYPE_COUNT				254

////////////////////////////////////////////////////////////////////////////////////////////
//�������ͽ��
struct tagOutCardTypeResult 
{
	BYTE							cbCardType;								//�˿�����
	BYTE							cbCardTypeCount;						//������Ŀ
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];	//ÿ�ָ���
	BYTE							cbEachHandCardData[MAX_TYPE_COUNT][MAX_COUNT];	///ÿ������
};

////////////////////////////////////////////////////////////////////////////////////////////
class CAndroidLogic : public CGameLogic
{
	//AI����
public:
	WORD							m_wBankerUser;								//�������
	LONG							m_lBankerOutCardCount;						//���ƴ���
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];	//�����˿�
	BYTE							m_cbHandCardCount[GAME_PLAYER];				//�˿���Ŀ
	BYTE							m_cbBackCardData[BACK_COUNT];				//����
	BYTE							m_cbAllCardOutCount[18];					//���Ƽ�¼ 0,1,2�±겻��

public:
	//���캯��
	CAndroidLogic(void);
	//��������
	~CAndroidLogic(void);
	
public:
	//���ñ���
	void ResetAllData(bool bConstruct=false);
	//����ׯ��
	void SetBankerUser(const WORD wBankerUser);
	//�����˿�
	void SetUserCard(const WORD wChairID, const BYTE cbCardData[], const BYTE cbCardCount);
	//���õ���
	void SetBackCard(const BYTE cbCardData[], const BYTE cbCardCount);
	//���õ���
	void SetBackCard(const WORD wChairID, const BYTE cbCardData[], const BYTE cbCardCount);
	//ɾ���˿�
	void RemoveUserCard(const WORD wChairID, const BYTE cbRemoveCardData[], const BYTE cbRemoveCardCount);
	//�з��ж�
	BYTE LandScore(const WORD wMeChairID, const BYTE cbCurrentLandScore);

	//��������
public:
	//����㷨
	void Combination(BYTE cbCombineCardData[], BYTE cbResComLen, BYTE cbResultCardData[254][5], BYTE &cbResCardLen, BYTE cbSrcCardData[], BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	//�����㷨
	void Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len);
	//��������ж�
	bool IsLargestCard(const WORD wChairID, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount);
	//�ų�ը����һ�����������
	bool IsLargestCardWithOutBomb(const WORD wMeChairID, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount);
	//�����Ĵ�������
	void Analyse4D2CardType(const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbHandCardType, tagOutCardResult &OutCardResult);
	//����ֻʣһ����ʱ�ĳ��Ʋ���
	void AnalyseEnemyLastOneCard(const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const tagOutCardTypeResult CardTypeResult[13], tagOutCardResult &OutCardResult);
	//����ֻʣ������ʱ�ĳ��Ʋ���
	void AnalyseEnemyLastTwoCard(const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const tagOutCardTypeResult CardTypeResult[13], tagOutCardResult &OutCardResult);

	//��Ҫ����
protected:
	//�������ͣ�����Ƶ��ã�
	void AnalyseOutCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagOutCardTypeResult OutCardTypeResult[12+1]);
	//�������ƣ��ȳ��Ƶ��ã�
	void AnalyseOutCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagOutCardTypeResult OutCardTypeResult[12+1]);
	//�������Ƹ���
	BYTE AnalyseSinleCardCount(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount, BYTE cbSingleCardData[]=NULL);

	//���ƺ���
public:	
	//��������
	bool SearchOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
	//�������ƣ��ȳ��ƣ�
	void BankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult);
	//�������ƣ�����ƣ�
	void BankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
	//�����ϼң��ȳ��ƣ�
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult);
	//�����ϼң�����ƣ�
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
	//�����¼ң��ȳ��ƣ�
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult);
	//�����¼ң�����ƣ�
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
};

#endif // AndroidLogic_h__