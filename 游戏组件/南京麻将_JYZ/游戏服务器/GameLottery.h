#ifndef GameLottery_h__
#define GameLottery_h__
#pragma once

#include <vector>
using namespace std;

#define LOTTERY_PRIZE_JINBI					0			//�齱��Ʒ���
#define LOTTERY_PRIZE_JIANGQUAN				1			//�齱��Ʒ��ȯ
#define LOTTERY_PRIZE_SAIPAI				2			//�齱��Ʒ����
#define LOTTERY_PRIZE_ALL					3			//�齱��Ʒ�������

#define MAX_LOTTERY_TYPE					5			//��ȡ���������������
#define MAX_SUB_PRIZE_RATE					7			//��Ʒ�ӱ����������
#define RATE_DENOMINATOR					1000		//���б�����ĸ

//�齱����
struct tagLotteryType
{
	int				nMinPoolScore;						//����ʣ�෶Χ��Сֵ		
	int				nMaxPoolScore;						//����ʣ�෶Χ���ֵ
	int				nGetMinJinBi;						//�ɻ�ȡ����С���ֵ
	int				nGetMaxJinBi;						//�ɻ�ȡ�������ֵ
	int				nGetMinJiangQuan;					//�ɻ�ȡ����С��ȯֵ
	int				nGetMaxJiangQuan;					//�ɻ�ȡ�����ȯֵ
	int				nGetMinSaiPai;						//�ɻ�ȡ����С����ֵ
	int				nGetMaxSaiPai;						//�ɻ�ȡ���������ֵ
};
//�ӽ�Ʒ����
struct tagSubPrizeRate
{
	int				nGetMinPrize;						//�ɻ�ȡ��С��Ʒֵ
	int				nGetMaxPrize;						//�ɻ�ȡ���Ʒֵ
	double			fGetPrizeRate;						//�ɻ�ȡ��Ʒ����
};
//��Ʒ����
struct tagPrizeRate
{
	double			fTotalRate;							//����Ʒ�ɻ�ȡ���ܱ���
	int				nExchangeRate;						//��Ʒ�һ��ɽ�ҵı���
	tagSubPrizeRate stSubPrizeRate[MAX_SUB_PRIZE_RATE];	//����Ʒ�Ӹ���
};
//�齱����
struct tagGameLottery
{
	double			fPoolRate;							//����ռ��ˮ����
	tagLotteryType	stLotteryType[MAX_LOTTERY_TYPE];	//�齱����
	tagPrizeRate	stPrizeRate[LOTTERY_PRIZE_ALL];		//��Ʒ����
};

///////////////////////////////////////////////////////
class CGameLottery
{
	//��Ա����
private:
	static CGameLottery		*m_spGameLottery;			//�齱ʵ��

	LONG					 m_lPrizePoolScore;			//������Ŀ
	tagGameLottery			 m_stGameLottery;			//�齱���ò���

	int						 m_nDummyPrizeRate;			//���轱������
	vector<tagLotteryPrizeResult> m_vecDummyLotteryPrize;//���轱Ʒ����
	
	//��Ա����	
private:
	//���캯��
	CGameLottery(void);
public:
	//��������
	~CGameLottery(void);
	//���ٳ齱ʵ��
	void Release();
	//��ȡ�齱ʵ��
	static CGameLottery *GetInstance();

public:	
	//��ȡ�齱���ò���
	bool ReadGameLotteryConfig();
	//��ӽ�����Ŀ
	void AddScoreToPrizePool(LONG lScoreNumber);
	//��ҳ齱
	void OnUserLottery(BYTE &cbPrizeType, LONG &lPrizeCount);
	//��ȡ����Ľ���
	void OnGetDummyPrize(BYTE &cbPrizeType, LONG &lPrizeCount);

protected:
	//��ȡ��ҳ齱��Ʒ
	void GetLotteryPrize(const BYTE cbIndex, BYTE &cbPrizeType, LONG &lPrizeCount);
	//��ȡ��ҽ�������
	LONG GetJinBiPrizeCount();
	//��ȡ��ȯ��������
	LONG GetJiangQuanPrizeCount();
	//��ȡ���ƽ�������
	LONG GetSaiPaiPrizeCount();
};

#endif // GameLottery_h__
