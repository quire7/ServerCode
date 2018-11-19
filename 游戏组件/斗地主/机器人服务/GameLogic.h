#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE
#pragma once

#include "Stdafx.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////
//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序

////////////////////////////////////////////////////////////////////////////////////////
//数目定义
#define MAX_COUNT					20									//最大数目
#define FULL_COUNT					54									//全牌数目
#define BACK_COUNT					3									//底牌数目
#define NORMAL_COUNT				17									//常规数目
#define GOOD_CARD_COUNT				38									//好牌数目

////////////////////////////////////////////////////////////////////////////////////////
//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

////////////////////////////////////////////////////////////////////////////////////////
//分析结构
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbDoubleCount;						//两张数目
	BYTE							cbSingleCount;						//单张数目
	BYTE							cbFourCardData[MAX_COUNT];			//四张扑克
	BYTE							cbThreeCardData[MAX_COUNT];			//三张扑克
	BYTE							cbDoubleCardData[MAX_COUNT];		//两张扑克
	BYTE							cbSingleCardData[MAX_COUNT];		//单张扑克
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_COUNT];				//结果扑克
};

//提示出牌结果
struct tagPromptCardResult
{
	BYTE							cbResultCount;						//提示结果数目
	BYTE							cbCardCount[MAX_COUNT];				//扑克数目
	BYTE							cbCardData[MAX_COUNT][MAX_COUNT];	//结果扑克
};

//手牌信息结构
struct tagPromptCardInfo
{
	tagPromptCardResult				stRocketResult;						//火箭
	tagPromptCardResult				stBombResult;						//炸弹
	tagPromptCardResult				stFourTwoResult;					//四带二
	tagPromptCardResult				cbPlaneResult;						//飞机
	tagPromptCardResult				stDoubleLineResult;					//姐妹对
	tagPromptCardResult				stSingleLineResult;					//顺子
};

//牌库数据
struct LandCardList
{
	std::vector<BYTE>				vecLandListCard;					
};


//////////////////////////////////////////////////////////////////////////
//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据
	static const BYTE				m_cbGoodcardData[GOOD_CARD_COUNT];	//好牌数据

public:
	static std::vector<LandCardList> m_vecLandCard;						//牌库数据

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

public:
	//加载牌库数据
	static void LoadLandCard();
	//删除所有扑克
	static bool RemoveAllCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//类型函数
public:
	//获取底牌类型
	BYTE GetBackCardType(const BYTE cbBackCardData[], bool bCheckLongTask=false);
	//获取底牌倍数
	BYTE GetBackCardMulitple(const BYTE cbBackCardType);
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], const BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//得到好牌
	void GetGoodCardData(BYTE cbGoodCardData[NORMAL_COUNT]);
	//删除好牌
	bool RemoveGoodCardData(BYTE cbGoodcardData[NORMAL_COUNT], BYTE cbGoodCardCount, BYTE cbCardData[FULL_COUNT], BYTE cbCardCount);
	//混乱缓冲区
	void RandArrayBuffer(BYTE cbBufferData[], BYTE cbBufferCount);
	//牌型缓冲区
	void SortArrayBuffer(BYTE cbBufferData[], BYTE cbBufferCount);

	//逻辑函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//有效判断
	bool IsValidCount(BYTE cbCardCount);
	//有效判断
	bool IsValidUser(WORD wChairID);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount, BYTE cbRecordType=CT_ERROR);
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagOutCardResult &OutCardResult);
	//出牌搜索
	bool SearchOutCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, const BYTE cbTurnCardData[], const BYTE cbTurnCardCount, tagPromptCardResult &OutCardResult);

	//内部函数
public:
	//分析扑克
	bool AnalysebCardData(const BYTE cbCardData[], const BYTE cbCardCount, tagAnalyseResult &AnalyseResult);
	//分析炸弹
	void GetAllBombCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbBomCardData[], BYTE &cbBomCardCount);
	//分析三条
	void GetAllThreeCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbThreeCardData[], BYTE &cbThreeCardCount);
	//分析对子
	void GetAllDoubleCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbDoubleCardData[], BYTE &cbDoubleCardCount);
	//分析单牌
	void GetAllSingleCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbSingleCardData[], BYTE &cbSingleCardCount, bool bOnlySingle);
	//分析顺子
	void GetAllLineCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, BYTE cbLineCardData[], BYTE &cbLineCardCount);
	//分析顺子
	void GetAllLineCard(const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagPromptCardInfo &PromptCardInfo);
	//提取信息
	void PromptHandCardEachType(const BYTE cbHandCardData[], const BYTE cbHandCardCount, tagPromptCardInfo &PromptCardInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif