#ifndef GameLottery_h__
#define GameLottery_h__
#pragma once

#include <vector>
using namespace std;

#define LOTTERY_PRIZE_JINBI					0			//抽奖奖品金币
#define LOTTERY_PRIZE_JIANGQUAN				1			//抽奖奖品奖券
#define LOTTERY_PRIZE_SAIPAI				2			//抽奖奖品赛牌
#define LOTTERY_PRIZE_ALL					3			//抽奖奖品最大种类

#define MAX_LOTTERY_TYPE					5			//获取奖励类型最大种类
#define MAX_SUB_PRIZE_RATE					7			//奖品子比例最大种类
#define RATE_DENOMINATOR					1000		//所有比例分母

//抽奖分类
struct tagLotteryType
{
	int				nMinPoolScore;						//奖池剩余范围最小值		
	int				nMaxPoolScore;						//奖池剩余范围最大值
	int				nGetMinJinBi;						//可获取的最小金币值
	int				nGetMaxJinBi;						//可获取的最大金币值
	int				nGetMinJiangQuan;					//可获取的最小奖券值
	int				nGetMaxJiangQuan;					//可获取的最大奖券值
	int				nGetMinSaiPai;						//可获取的最小赛牌值
	int				nGetMaxSaiPai;						//可获取的最大赛牌值
};
//子奖品比例
struct tagSubPrizeRate
{
	int				nGetMinPrize;						//可获取最小奖品值
	int				nGetMaxPrize;						//可获取最大奖品值
	double			fGetPrizeRate;						//可获取奖品概率
};
//奖品比例
struct tagPrizeRate
{
	double			fTotalRate;							//各奖品可获取的总比例
	int				nExchangeRate;						//奖品兑换成金币的比例
	tagSubPrizeRate stSubPrizeRate[MAX_SUB_PRIZE_RATE];	//各奖品子概率
};
//抽奖配置
struct tagGameLottery
{
	double			fPoolRate;							//奖池占抽水比例
	tagLotteryType	stLotteryType[MAX_LOTTERY_TYPE];	//抽奖分类
	tagPrizeRate	stPrizeRate[LOTTERY_PRIZE_ALL];		//奖品概率
};

///////////////////////////////////////////////////////
class CGameLottery
{
	//成员变量
private:
	static CGameLottery		*m_spGameLottery;			//抽奖实例

	LONG					 m_lPrizePoolScore;			//奖池数目
	tagGameLottery			 m_stGameLottery;			//抽奖配置参数

	int						 m_nDummyPrizeRate;			//虚设奖励比例
	vector<tagLotteryPrizeResult> m_vecDummyLotteryPrize;//虚设奖品数组
	
	//成员函数	
private:
	//构造函数
	CGameLottery(void);
public:
	//析构函数
	~CGameLottery(void);
	//销毁抽奖实例
	void Release();
	//获取抽奖实例
	static CGameLottery *GetInstance();

public:	
	//读取抽奖配置参数
	bool ReadGameLotteryConfig();
	//添加奖池数目
	void AddScoreToPrizePool(LONG lScoreNumber);
	//玩家抽奖
	void OnUserLottery(BYTE &cbPrizeType, LONG &lPrizeCount);
	//获取虚设的奖励
	void OnGetDummyPrize(BYTE &cbPrizeType, LONG &lPrizeCount);

protected:
	//获取玩家抽奖奖品
	void GetLotteryPrize(const BYTE cbIndex, BYTE &cbPrizeType, LONG &lPrizeCount);
	//获取金币奖励数量
	LONG GetJinBiPrizeCount();
	//获取奖券奖励数量
	LONG GetJiangQuanPrizeCount();
	//获取赛牌奖励数量
	LONG GetSaiPaiPrizeCount();
};

#endif // GameLottery_h__
