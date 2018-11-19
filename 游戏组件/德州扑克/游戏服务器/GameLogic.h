#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//扑克类型
#define CT_SINGLE					1									//单牌类型
#define CT_ONE_LONG				    2									//对子类型
#define CT_TWO_LONG				    3									//两对类型
#define CT_THREE_TIAO				4									//三条类型
#define	CT_SHUN_ZI					5									//顺子类型
#define CT_TONG_HUA					6									//同花类型
#define CT_HU_LU					7									//葫芦类型
#define CT_TIE_ZHI					8									//铁支类型
#define CT_TONG_HUA_SHUN			9									//同花顺型
#define CT_KING_TONG_HUA_SHUN		10									//皇家同花顺
#define CT_MAXTYPE          		11									//最大的类型

//扑克大小
#define CARD_VALUE_2				2									//牌2	
#define CARD_VALUE_3				3									//牌3	
#define CARD_VALUE_4				4									//牌4	
#define CARD_VALUE_5				5									//牌5	
#define CARD_VALUE_6				6									//牌6	
#define CARD_VALUE_7				7									//牌7	
#define CARD_VALUE_8				8									//牌8	
#define CARD_VALUE_9				9									//牌9	
#define CARD_VALUE_10				10									//牌10	
#define CARD_VALUE_J				11									//牌J	
#define CARD_VALUE_Q				12									//牌Q	
#define CARD_VALUE_K				13									//牌K	
#define CARD_VALUE_A				14									//牌A	
//////////////////////////////////////////////////////////////////////////

//胜利信息结构
struct UserWinList
{
	BYTE bSameCount;
	WORD wWinerList[GAME_PLAYER];
};

//分析结构
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//四张数目
	BYTE 							cbThreeCount;						//三张数目
	BYTE 							cbLONGCount;						//两张数目
	BYTE							cbSignedCount;						//单张数目
	BYTE 							cbFourLogicVolue[1];				//四张列表
	BYTE 							cbThreeLogicVolue[1];				//三张列表
	BYTE 							cbLONGLogicVolue[2];				//两张列表
	BYTE 							cbSignedLogicVolue[5];				//单张列表
	BYTE							cbFourCardData[MAX_CENTERCOUNT];	//四张列表
	BYTE							cbThreeCardData[MAX_CENTERCOUNT];	//三张列表
	BYTE							cbLONGCardData[MAX_CENTERCOUNT];	//两张列表
	BYTE							cbSignedCardData[MAX_CENTERCOUNT];	//单张数目
};
//////////////////////////////////////////////////////////////////////////

//游戏逻辑
class CGameLogic
{
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//变量定义
private:
	static BYTE						m_cbCardData[FULL_COUNT];					//扑克定义
	static const WORD               m_wLogicCount;
public:
	static LONG						m_lGiveUp[4][INT_ROBOT_GIVEUP];				//机器人放弃率
	static LONG						m_lAdd[4][INT_ROBOT_ADD];					//机器人加注率
	static LONG						m_lQuanYa[4][INT_ROBOT_QUANYA];				//机器人全押
	static CString					m_strFileName;
	static CString					m_strFileName2;

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//排序
	void SortValueList(LONG cbValue[], BYTE ID[], BYTE cbCardCount);
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//类型函数
public:
	//获取类型
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }
	//读取配置文件
	static BOOL SetConfigInit(void);

	//功能函数
public:
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	BYTE GetCardLogicValue1(BYTE cbCardData[],BYTE CardType[]);
	//对比扑克
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);
	//分析扑克
	void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//7返5
	BYTE FiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE cbLastCardCount);
	//判断是否大于最大牌型
	BYTE GetFiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE CardType);

	//整理最大牌型
	BYTE FinishCard(BYTE cbEndCardData[],BYTE cbEndCardData1[],BYTE cbHandCardCount1,BYTE cbHandData[],BYTE Type);
	//查找最大
	bool SelectMaxUser(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],UserWinList &EndResult,BYTE bHandData[GAME_PLAYER][2],const LONG lm_AddScore[]);
	//扑克牌型排序
	bool SelectCardPaiXu(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wchair[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wCount[]);
	//比较两牌大小
	BYTE SelectTwoCard(BYTE Vale1[],BYTE Vale2[]);
};

//////////////////////////////////////////////////////////////////////////

#endif