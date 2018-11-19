#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

///////////////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

///////////////////////////////////////////////////////////////////////////////////
//牌值定义
#define MIN_ZICARD					0x31								//字牌最小值
#define MAX_ZICARD					0x34								//字牌最大值
#define MIN_HUACARD					0x35								//花牌最小值
#define MAX_HUACARD					0x48								//花牌最大值

///////////////////////////////////////////////////////////////////////////////////
//动作定义
//动作标志
#define WIK_NULL					 0									//没有类型
#define WIK_OXX						(1<<0)								//左吃类型
#define WIK_XOX						(1<<1)								//中吃类型
#define WIK_XXO						(1<<2)								//右吃类型
#define WIK_CHI						(WIK_OXX|WIK_XOX|WIK_XXO)			//吃牌类型
#define WIK_PENG					(1<<3)								//碰牌类型
#define WIK_MGANG					(1<<4)								//明杠类型
#define WIK_BGANG					(1<<5)								//补杠类型
#define WIK_AGANG					(1<<6)								//暗杠类型
#define WIK_GANG					(WIK_MGANG|WIK_BGANG|WIK_AGANG)		//杠牌类型
#define WIK_TING					(1<<7)								//听牌类型
#define WIK_ZHUOPAO					(1<<8)								//捉炮胡类型
#define WIK_ZIMO					(1<<9)								//自摸胡类型
#define WIK_BUHUA					(1<<10)								//补花类型
#define WIK_CHU						(1<<11)								//出牌类型
#define	WIK_TIAOZHAN				(1<<12)								//挑战类型
#define WIK_BAOZI					(1<<13)								//菜包子类型
#define WIK_HU						(WIK_ZHUOPAO|WIK_ZIMO|WIK_BAOZI)	//胡牌类型

///////////////////////////////////////////////////////////////////////////////////
//胡牌牌型定义
#define HU_NULL						0									//不胡
#define	HU_PINGHU					(1<<0)								//平胡
#define HU_MENQING					(1<<1)								//门清
#define HU_HUNYISE					(1<<2)								//混一色
#define HU_QINGYISE					(1<<3)								//清一色
#define HU_DUIDUIHU					(1<<4)								//对对胡
#define HU_QQDUDIAO					(1<<5)								//全球独钓
#define HU_QIDUI					(1<<6)								//七对
#define HU_SQIDUI					(1<<7)								//双七对
#define HU_WUHUAGUO					(1<<10)								//无花果
#define HU_YAJUE					(1<<11)								//压绝
#define HU_XGKAIHUA					(1<<12)								//小杠开花
#define HU_DGKAIHUA					(1<<13)								//大杠开花
#define HU_TIANHU					(1<<14)								//天胡
#define HU_DIHU						(1<<15)								//地胡
#define HU_ONLYONE					(1<<17)								//只胡一张
#define HU_BAOZI					(1<<19)								//菜包子类型
#define HU_QIANGGANGHU				(1<<20)								//抢杠胡包

/////////////////////////////////////////////////////////////////////////////////////
////包牌类型
//#define BAOPAI_NULL					0									//不包
//#define BAOPAI_3PENG				(1<<0)								//三碰包
//#define BAOPAI_QIANGGANGHU			(1<<2)								//抢杠胡包
//#define BAOPAI_GANGHOUHU			(1<<3)								//大杠杠后胡包

///////////////////////////////////////////////////////////////////////////////////
//数据结构
//类型子项
struct tagKindItem
{
	DWORD							dwWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心牌
	BYTE							cbCardIndex[3];						//牌索引
};
//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//眼牌
	DWORD							dwWeaveKind[4];						//组合类型
	BYTE							cbCenterCard[4];					//中心牌
};


/////////////////////////////////////////////////////////////////////////////////////
//数组说明
typedef CYSArray<tagAnalyseItem, tagAnalyseItem &> CAnalyseItemArray;

/////////////////////////////////////////////////////////////////////////////////////
//游戏逻辑类
class CGameLogic
{
///<变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//牌数据

///<函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

///<控制函数
public:
	//重置数据
	void ResetAllData();
	//混乱牌
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//删除牌
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除牌
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除牌
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//排序牌
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount)const;
	//提取硬花牌
	BYTE DistillHardHuaCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbHardHuaCardData[MAX_HUA_COUNT]);

///<辅助函数
public:
	//有效判断
	static bool IsValidCard(BYTE cbCardData);
	//用户判断
	bool IsValidUser(WORD wChairID) { return (wChairID>=0&&wChairID<GAME_PLAYER)?true:false; }
	//获取数值
	static BYTE GetCardValue(BYTE cbCardData) {return (cbCardData&MASK_VALUE);}
	//获取花色
	static BYTE GetCardColor(BYTE cbCardData) {return ((cbCardData&MASK_COLOR)>>4);}
	//牌数目
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//组合牌
	BYTE GetWeaveCard(DWORD dwWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[MAX_WEAVE]);
	//计算成牌花数
	DWORD CalculateFanShu(DWORD dwHuPaiKind);
	//是否是字牌
	bool IsZiCard(BYTE cbCardData) { return (cbCardData>=MIN_ZICARD&&cbCardData<=MAX_ZICARD)?true:false; }
	//是否是花牌
	bool IsHuaCard(BYTE cbCardData) { return (cbCardData>=MIN_HUACARD&&cbCardData<=MAX_HUACARD)?true:false; }
	//检测牌是否在数组中
	bool IsCardInArrary(const BYTE cbCardArrary[], const BYTE cbArraryCount, const BYTE cbCardData);

///<等级函数
public:
	//动作等级
	BYTE GetUserActionRank(DWORD dwUserAction);
	//胡牌等级
	WORD GetHuPaiActionRank(tagHuPaiResult & HuPaiResult);

///<动作判断
public:
	//吃牌分析
	DWORD AnalyseChiPai(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌分析
	DWORD AnalysePengPai(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌分析
	DWORD AnalyseGangPai(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, tagGangPaiResult &GangPaiResult, bool bZiMo);
	//听牌分析
	DWORD AnalyseTingPai(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbHuaCount, DWORD dwPreHuPaiKind, tagTingPaiResult &TingPaiResult, bool bCheckTianTing=false);
	//胡牌分析
	DWORD AnalyseHuPai(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind, tagHuPaiResult &HuPaiResult, bool bZiMo, bool bContinue=true);
	//快照分析
	//DWORD AnalyseKuaiZhao(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, DWORD dwPreHuPaiKind, tagHuPaiResult &HuPaiResult);

///<内部函数
private:
	//分析牌
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);

///<胡牌类型
public:
	//门清
	bool IsMenQing(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//混一色牌
	bool IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//清一色牌
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//对对胡
	bool IsDuiDuiHu(tagAnalyseItem *pAnalyseItem);
	//全球独钓
	bool IsQuanQiuDuDiao(const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind);
	//获取七对种类
	BYTE IsQiDui(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//无花果
	bool IsWuHuaGuo(const BYTE cbHuaCount) { return (cbHuaCount==0)?true:false; }
	//单钓将牌
	bool IsDanDiaoJiang(const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind);
	//只胡一张牌
	bool IsOnlyHuOneCard(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind);
	//是否是大胡
	bool IsBigHuPaiKind(DWORD dwHuPaiKind);
	//是否是七对胡牌型
	bool IsHuPaiQiDuiKind(DWORD dwHuPaiKind);

///<转换函数
public:
	//牌转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//牌转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//牌转换
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//牌转换
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
};

//////////////////////////////////////////////////////////////////////////

#endif