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
#define AI_CT_ERROR					0									//错误类型
#define AI_CT_SINGLE				1									//单牌类型
#define AI_CT_DOUBLE				2									//对子类型
#define AI_CT_THREE					3									//三条类型
#define AI_CT_SINGLE_LINK			4									//单连类型
#define AI_CT_DOUBLE_LINK			5									//对连类型
#define AI_CT_THREE_LINK			6									//三连类型
#define AI_CT_THREE_DOUBLE			7									//三带二型
#define AI_CT_TONG_HUA_SHUN			8									//同花顺型
#define AI_CT_BOMB					9									//炸弹类型
#define AI_CT_BOMB_TW				10									//天王炸弹
#define TYPE_COUNT					11                                  //牌类型数量（数组0~10）
#define MAX_TYPE_COUNT				254

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

//牌类型
struct CardTypeImfomation
{
	BYTE							cbLikeCardTypeCount;
	BYTE							cbLikeCardType[TYPE_COUNT];
	BYTE							cbUnLikeCardTypeCount;
	BYTE							cbUnLikeCardType[TYPE_COUNT];
};

//出牌玩家信息
struct PlayerOutImfomation
{
	WORD							wOutChairID;
	BYTE							cbOutCardCount;
	BYTE							cbOutCardData[MAX_COUNT];
};

struct OutCardImfomation
{
	WORD							wCurrentChairID;
	BYTE							cbCurPlaOutCount;
	BYTE							cbEnemyFirstCount;
	BYTE							cbEnemyOutCount;
};

//最优牌型(各个玩家)
struct tagBestOutType
{
	BYTE							cbCardType[GAME_PLAYER];
	WORD							cbCardSafeValue[GAME_PLAYER];
};

struct tagOutCardTypeResult 
{
	BYTE							cbCardType;							//扑克类型
	BYTE							cbCardTypeCount;					//牌型数目
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//每手个数
	BYTE							cbEachHandCardData[MAX_TYPE_COUNT][MAX_COUNT];//扑克数据
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CAILogic
{
	//逻辑变量
protected:
	BYTE							m_cbMainValue;						//主牌数值

	//函数定义
public:
	//构造函数
	CAILogic();
	//析构函数
	virtual ~CAILogic();

	//信息函数
public:
	//主牌数值
	VOID SetMainValue(BYTE cbMainValue) { m_cbMainValue=cbMainValue; }
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//逻辑数值
	BYTE GetCardLogicValue(BYTE cbCardData);

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//获取花色
	BYTE GetCardColor(const BYTE cbCardData[], BYTE cbCardCount);

	//控制函数
public:
	//排列扑克
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//删除扑克
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

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

	//搜索牌型
protected:
	//搜索同花顺
	bool SearchTongHuaShun(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult);
	//按变幻牌分析扑克
	VOID AnalyseMagicData(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagMagicResult &MagicResult);
	//变幻搜索同花顺
	bool SearchMagicTongHuaShun(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult);

	//机器人辅助函数
public:
	//牌型搜索
	void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT/*+1*/]);
	//出牌记录的函数总汇
	void SetOutCardImfomation(WORD wMeChairID, WORD wOutChairID, WORD wCurrentID, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCount, bool bOutCard, bool bFirstOutCard);
	//记录剩余的牌
	void RecordOutCard(BYTE cbOutCard[], BYTE cbOutCount);
	//设置初始值
	void SetInitData();
	//记录大小王
	void RecordKindCard(WORD wMeChairID, BYTE cbHandData[], BYTE cbHandCount, WORD wPassaChairID, WORD wOutChairID, BYTE cbTurnData[], BYTE cbTurnCount, PlayerOutImfomation ForOutImfo);
	//记录出牌的牌型
	void RecordOutCardType(WORD wOutChairID, BYTE cbTurnData[], BYTE cbTurnCount, bool bFirst, WORD wPassChairID, BYTE cbHandData[], BYTE cbHandCount, PlayerOutImfomation ForOutImfo);
	//获取最优牌
	void GetBestCardType(WORD wOutChairID, BYTE cbTurnCard[], BYTE cbTurnCount, WORD wPassID);
	//查找顺子
	int SerarchSingleLink(BYTE cbCardData[], BYTE cbCardCount, int iMagicCount, tagOutCardResult &OutCardResult);
	//左右各5个逻辑差之间计算四种花色的个数
	bool NearbyColorCount(BYTE cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbCardData, BYTE ColorCount[4]);
	//制定类型中剩余的最大的逻辑值
	BYTE SearchMaxLogicValue(BYTE cbType, BYTE cbHandData[], BYTE cbHandCount);
	//查找牌型
	bool SearchType(BYTE cbType, bool bLike, WORD wChairID, bool bMove);
	//获取牌型牌的逻辑值
	BYTE GetTypeCardValue(BYTE cbCardData[], BYTE cbCardCount);
	//获取牌型
	BYTE GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//变幻牌型比较
	bool CompareMagicCard(BYTE cbFirstCard[], BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//是否可以拆牌
	bool AnalyseCardPass(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbOutCard[], BYTE cbCardType, tagOutCardResult &OutCardResult);
	//分析同花顺
	bool AnalyseTongHuaShun(BYTE const cbHandCardData[], BYTE cbHandCardCount, tagOutCardTypeResult &CardTypeResult);
	//牌型搜索(除同花顺)
	void AnalyseOutCardTypeNomal(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT/*+1*/]);
	
public:
	//获取所有用户手牌信息
	void OnHandCardChange(const BYTE cbAllUserHandCardData[][MAX_COUNT], const BYTE cbAllUserHandCardCount[]);
	//设置用户手牌
	void SetUserHandCard(WORD wUserID, const BYTE cbUserCardData[], BYTE cbUserCardCount);
	//判断一个用户是否危险
	bool IsDangerous(WORD wUserChairID);
	//排除手牌中的炸弹后，手牌的牌型
	BYTE GetAfterRemoveBombsCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount);

	//验证函数
public:
	//验证要出的牌是否是指定的牌型
	bool CheckOutCardRightType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbType);
	//验证要出的牌是否大于已出牌
	bool CheckWillOutCard(WORD wOutID, WORD wMeID, BYTE cbCard[], BYTE cbCount, BYTE cbTurnCard[], BYTE cbTurnCount);
	//验证要出的牌是否在自己手牌中
	bool CheckOutCardInHand(const BYTE cbHandData[], const BYTE cbHandCount, BYTE cbOutCard[], BYTE cbOutCount);

	//对外调用
public:
	//获取进贡牌
	BYTE GetTributeCard(WORD wChariID, BYTE cbHandCardData[], BYTE cbHandCardCount);
	//获取回贡牌
	BYTE GetBackTributeCard(WORD wChariID, BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wThrowUserNum, bool bThrowToFriend);
	//查找要出的牌
	bool SearchOutCardAI(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult);
	//首家出牌
	void SearchFirstOutCard(WORD wMeChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult &OutCardResult);
	//下家接牌
	void SearchNextOutCard(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult);


private:
	BYTE					m_cbHandCardCount[GAME_PLAYER];						  	   //各玩家手牌数
	BYTE					m_cbHandCardData[GAME_PLAYER][MAX_COUNT];				   //各玩家手牌数据

public:
	int                     m_iOutCardCount[MAX_LOGIC_VALUE+1];                        //15存的是主牌，0存储可变幻的牌
	BYTE                    m_cbKindCount[GAME_PLAYER][2];                             //玩家拥有的大小王数量
	CardTypeImfomation      m_PlayerTypeImfo[GAME_PLAYER];                             //出牌类型记录    
	PlayerOutImfomation     m_ForOutImfo;                                              //前一出牌玩家
	OutCardImfomation       m_CurPlayerOutTimes;                                       //当前玩家连续出牌次数
	bool                    m_bFirstOutCard;                                           //是否是首次出牌
	BYTE                    m_cbCardNum[GAME_PLAYER];                                  //扑克数目
	tagBestOutType          m_PlayerBestOutType;                                       //最优牌型

private:
	WORD					m_wRoundOutTimes;										   //玩家连续出牌次数
};

//////////////////////////////////////////////////////////////////////////
