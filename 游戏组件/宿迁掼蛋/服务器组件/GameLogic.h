#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

///////////////////////////////////////////////////////////////////////////////////
//数目定义
#define FULL_COUNT					108									//全牌数目
///////////////////////////////////////////////////////////////////////////////////
//数值掩码
#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

///////////////////////////////////////////////////////////////////////////////////
//牌值定义
#define MIN_LOGIC_VALUE             1                                   //最小逻辑值
#define MAX_LOGIC_VALUE             18                                  //最大逻辑值

///////////////////////////////////////////////////////////////////////////////////
//排序类型
#define ST_ORDER					0									//大小排序
#define ST_COUNT					1									//数目排序
#define ST_VALUE					2									//数值排序
#define ST_COLOR					3									//花色排序
#define ST_CUSTOM					4									//自定排序

///////////////////////////////////////////////////////////////////////////////////
//扑克类型
#define CT_ERROR					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对子类型
#define CT_THREE					3									//三条类型
#define CT_SINGLE_LINK				4									//单连类型
#define CT_DOUBLE_LINK				5									//对连类型
#define CT_THREE_LINK				6									//三连类型
#define CT_THREE_DOUBLE				7									//三带二型
#define CT_TONG_HUA_SHUN			8									//同花顺型
#define CT_BOMB						9									//炸弹类型
#define CT_BOMB_TW					10									//天王炸弹
#define TYPE_COUNT					11                                  //牌类型数量（数组0~10）

///////////////////////////////////////////////////////////////////////////////////////
//分析结构
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[10];					//扑克数目
	BYTE							cbCardData[10][MAX_COUNT];			//扑克数据
};

//出牌结果
struct tagOutCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbResultCard[MAX_COUNT];			//结果扑克
};

//分布信息
struct tagDistributing
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbDistributing[15][6];				//分布信息
};

struct tagMagicResult
{
	BYTE							cbMagicCardCount;					//魔术牌数目
	BYTE							cbNormalCardCount;					//常规牌数目
	BYTE							cbMagicCardData[2];					//魔术牌数据
	BYTE							cbNormalCardData[MAX_COUNT];		//常规牌数据
};

//////////////////////////////////////////////////////////////////////////
//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//扑克数据
	//逻辑变量
	BYTE							m_cbMainValue;						//主牌数值
	
	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//信息函数
public:
	//主牌数值
	VOID SetMainValue(BYTE cbMainValue) { m_cbMainValue = cbMainValue; }
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);
	//获取进贡牌
	BYTE GetJingongCard(const BYTE cbCardData[]);

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取花色
	BYTE GetCardColor(const BYTE cbCardData[], BYTE cbCardCount);
	 //获取牌型
	BYTE GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount);

	//控制函数
public:
	//混乱扑克
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//添加扑克
	bool Add(BYTE bAddCard, BYTE  cbCardData[], BYTE cbCardCount = 27);

	//逻辑函数
public:
	//对比扑克
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);

	//功能函数
public:
	//分析扑克
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult);
	//分析分布
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing &Distributing);
	//变幻扑克
	bool MagicCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT]);

	//内部函数
private:
	//构造扑克
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//是否连牌
	bool IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount);
	//对比扑克
	bool CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
};

//////////////////////////////////////////////////////////////////////////

#endif