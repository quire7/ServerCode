#ifndef AndroidLogic_h__
#define AndroidLogic_h__
#pragma once

#include "GameLogic.h"

#define MAX_TYPE_COUNT				254

////////////////////////////////////////////////////////////////////////////////////////////
//出牌类型结果
struct tagOutCardTypeResult 
{
	BYTE							cbCardType;								//扑克类型
	BYTE							cbCardTypeCount;						//牌型数目
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];	//每手个数
	BYTE							cbEachHandCardData[MAX_TYPE_COUNT][MAX_COUNT];	///每手数据
};

////////////////////////////////////////////////////////////////////////////////////////////
class CAndroidLogic : public CGameLogic
{
	//AI变量
public:
	WORD							m_wBankerUser;								//地主玩家
	LONG							m_lBankerOutCardCount;						//出牌次数
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];	//所有扑克
	BYTE							m_cbHandCardCount[GAME_PLAYER];				//扑克数目
	BYTE							m_cbBackCardData[BACK_COUNT];				//底牌
	BYTE							m_cbAllCardOutCount[18];					//出牌记录 0,1,2下标不用

public:
	//构造函数
	CAndroidLogic(void);
	//析构函数
	~CAndroidLogic(void);
	
public:
	//重置变量
	void ResetAllData(bool bConstruct=false);
	//设置庄家
	void SetBankerUser(const WORD wBankerUser);
	//设置扑克
	void SetUserCard(const WORD wChairID, const BYTE cbCardData[], const BYTE cbCardCount);
	//设置底牌
	void SetBackCard(const BYTE cbCardData[], const BYTE cbCardCount);
	//设置底牌
	void SetBackCard(const WORD wChairID, const BYTE cbCardData[], const BYTE cbCardCount);
	//删除扑克
	void RemoveUserCard(const WORD wChairID, const BYTE cbRemoveCardData[], const BYTE cbRemoveCardCount);
	//叫分判断
	BYTE LandScore(const WORD wMeChairID, const BYTE cbCurrentLandScore);

	//辅助函数
public:
	//组合算法
	void Combination(BYTE cbCombineCardData[], BYTE cbResComLen, BYTE cbResultCardData[254][5], BYTE &cbResCardLen, BYTE cbSrcCardData[], BYTE cbCombineLen1, BYTE cbSrcLen, const BYTE cbCombineLen2);
	//排列算法
	void Permutation(BYTE *list, int m, int n, BYTE result[][4], BYTE &len);
	//最大牌型判断
	bool IsLargestCard(const WORD wChairID, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount);
	//排除炸弹，一定是最大牌型
	bool IsLargestCardWithOutBomb(const WORD wMeChairID, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount);
	//分析四带二牌型
	void Analyse4D2CardType(const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbHandCardType, tagOutCardResult &OutCardResult);
	//分析只剩一张牌时的出牌策略
	void AnalyseEnemyLastOneCard(const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const tagOutCardTypeResult CardTypeResult[13], tagOutCardResult &OutCardResult);
	//分析只剩两张牌时的出牌策略
	void AnalyseEnemyLastTwoCard(const WORD wMeChairID, const BYTE cbHandCardData[], const BYTE cbHandCardCount, const tagOutCardTypeResult CardTypeResult[13], tagOutCardResult &OutCardResult);

	//主要函数
protected:
	//分析牌型（后出牌调用）
	void AnalyseOutCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagOutCardTypeResult OutCardTypeResult[12+1]);
	//分析牌牌（先出牌调用）
	void AnalyseOutCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagOutCardTypeResult OutCardTypeResult[12+1]);
	//分析单牌个数
	BYTE AnalyseSinleCardCount(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbWantOutCardData[], const BYTE cbWantOutCardCount, BYTE cbSingleCardData[]=NULL);

	//出牌函数
public:	
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
	//地主出牌（先出牌）
	void BankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult);
	//地主出牌（后出牌）
	void BankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
	//地主上家（先出牌）
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult);
	//地主上家（后出牌）
	void UpsideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
	//地主下家（先出牌）
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const WORD wMeChairID, tagOutCardResult &OutCardResult);
	//地主下家（后出牌）
	void UndersideOfBankerOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, const WORD wMeChairID, const WORD wOutCardUser, tagOutCardResult &OutCardResult);
};

#endif // AndroidLogic_h__