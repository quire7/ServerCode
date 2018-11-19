#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

///////////////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

///////////////////////////////////////////////////////////////////////////////////
//��ֵ����
#define MIN_ZICARD					0x31								//������Сֵ
#define MAX_ZICARD					0x34								//�������ֵ
#define MIN_HUACARD					0x35								//������Сֵ
#define MAX_HUACARD					0x48								//�������ֵ

///////////////////////////////////////////////////////////////////////////////////
//��������
//������־
#define WIK_NULL					 0									//û������
#define WIK_OXX						(1<<0)								//�������
#define WIK_XOX						(1<<1)								//�г�����
#define WIK_XXO						(1<<2)								//�ҳ�����
#define WIK_CHI						(WIK_OXX|WIK_XOX|WIK_XXO)			//��������
#define WIK_PENG					(1<<3)								//��������
#define WIK_MGANG					(1<<4)								//��������
#define WIK_BGANG					(1<<5)								//��������
#define WIK_AGANG					(1<<6)								//��������
#define WIK_GANG					(WIK_MGANG|WIK_BGANG|WIK_AGANG)		//��������
#define WIK_TING					(1<<7)								//��������
#define WIK_ZHUOPAO					(1<<8)								//׽�ں�����
#define WIK_ZIMO					(1<<9)								//����������
#define WIK_BUHUA					(1<<10)								//��������
#define WIK_CHU						(1<<11)								//��������
#define	WIK_TIAOZHAN				(1<<12)								//��ս����
#define WIK_BAOZI					(1<<13)								//�˰�������
#define WIK_HU						(WIK_ZHUOPAO|WIK_ZIMO|WIK_BAOZI)	//��������

///////////////////////////////////////////////////////////////////////////////////
//�������Ͷ���
#define HU_NULL						0									//����
#define	HU_PINGHU					(1<<0)								//ƽ��
#define HU_MENQING					(1<<1)								//����
#define HU_HUNYISE					(1<<2)								//��һɫ
#define HU_QINGYISE					(1<<3)								//��һɫ
#define HU_DUIDUIHU					(1<<4)								//�ԶԺ�
#define HU_QQDUDIAO					(1<<5)								//ȫ�����
#define HU_QIDUI					(1<<6)								//�߶�
#define HU_SQIDUI					(1<<7)								//˫�߶�
#define HU_WUHUAGUO					(1<<10)								//�޻���
#define HU_YAJUE					(1<<11)								//ѹ��
#define HU_XGKAIHUA					(1<<12)								//С�ܿ���
#define HU_DGKAIHUA					(1<<13)								//��ܿ���
#define HU_TIANHU					(1<<14)								//���
#define HU_DIHU						(1<<15)								//�غ�
#define HU_ONLYONE					(1<<17)								//ֻ��һ��
#define HU_BAOZI					(1<<19)								//�˰�������
#define HU_QIANGGANGHU				(1<<20)								//���ܺ���

/////////////////////////////////////////////////////////////////////////////////////
////��������
//#define BAOPAI_NULL					0									//����
//#define BAOPAI_3PENG				(1<<0)								//������
//#define BAOPAI_QIANGGANGHU			(1<<2)								//���ܺ���
//#define BAOPAI_GANGHOUHU			(1<<3)								//��ܸܺ����

///////////////////////////////////////////////////////////////////////////////////
//���ݽṹ
//��������
struct tagKindItem
{
	DWORD							dwWeaveKind;						//�������
	BYTE							cbCenterCard;						//������
	BYTE							cbCardIndex[3];						//������
};
//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//����
	DWORD							dwWeaveKind[4];						//�������
	BYTE							cbCenterCard[4];					//������
};


/////////////////////////////////////////////////////////////////////////////////////
//����˵��
typedef CYSArray<tagAnalyseItem, tagAnalyseItem &> CAnalyseItemArray;

/////////////////////////////////////////////////////////////////////////////////////
//��Ϸ�߼���
class CGameLogic
{
///<��������
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//������

///<��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

///<���ƺ���
public:
	//��������
	void ResetAllData();
	//������
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//ɾ����
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//ɾ����
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//ɾ����
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//������
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount)const;
	//��ȡӲ����
	BYTE DistillHardHuaCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbHardHuaCardData[MAX_HUA_COUNT]);

///<��������
public:
	//��Ч�ж�
	static bool IsValidCard(BYTE cbCardData);
	//�û��ж�
	bool IsValidUser(WORD wChairID) { return (wChairID>=0&&wChairID<GAME_PLAYER)?true:false; }
	//��ȡ��ֵ
	static BYTE GetCardValue(BYTE cbCardData) {return (cbCardData&MASK_VALUE);}
	//��ȡ��ɫ
	static BYTE GetCardColor(BYTE cbCardData) {return ((cbCardData&MASK_COLOR)>>4);}
	//����Ŀ
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//�����
	BYTE GetWeaveCard(DWORD dwWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[MAX_WEAVE]);
	//������ƻ���
	DWORD CalculateFanShu(DWORD dwHuPaiKind);
	//�Ƿ�������
	bool IsZiCard(BYTE cbCardData) { return (cbCardData>=MIN_ZICARD&&cbCardData<=MAX_ZICARD)?true:false; }
	//�Ƿ��ǻ���
	bool IsHuaCard(BYTE cbCardData) { return (cbCardData>=MIN_HUACARD&&cbCardData<=MAX_HUACARD)?true:false; }
	//������Ƿ���������
	bool IsCardInArrary(const BYTE cbCardArrary[], const BYTE cbArraryCount, const BYTE cbCardData);

///<�ȼ�����
public:
	//�����ȼ�
	BYTE GetUserActionRank(DWORD dwUserAction);
	//���Ƶȼ�
	WORD GetHuPaiActionRank(tagHuPaiResult & HuPaiResult);

///<�����ж�
public:
	//���Ʒ���
	DWORD AnalyseChiPai(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//���Ʒ���
	DWORD AnalysePengPai(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//���Ʒ���
	DWORD AnalyseGangPai(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, tagGangPaiResult &GangPaiResult, bool bZiMo);
	//���Ʒ���
	DWORD AnalyseTingPai(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbHuaCount, DWORD dwPreHuPaiKind, tagTingPaiResult &TingPaiResult, bool bCheckTianTing=false);
	//���Ʒ���
	DWORD AnalyseHuPai(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind, tagHuPaiResult &HuPaiResult, bool bZiMo, bool bContinue=true);
	//���շ���
	//DWORD AnalyseKuaiZhao(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, DWORD dwPreHuPaiKind, tagHuPaiResult &HuPaiResult);

///<�ڲ�����
private:
	//������
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);

///<��������
public:
	//����
	bool IsMenQing(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//��һɫ��
	bool IsHunYiSe(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//��һɫ��
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//�ԶԺ�
	bool IsDuiDuiHu(tagAnalyseItem *pAnalyseItem);
	//ȫ�����
	bool IsQuanQiuDuDiao(const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind);
	//��ȡ�߶�����
	BYTE IsQiDui(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard);
	//�޻���
	bool IsWuHuaGuo(const BYTE cbHuaCount) { return (cbHuaCount==0)?true:false; }
	//��������
	bool IsDanDiaoJiang(const BYTE cbCardIndex[MAX_INDEX], tagAnalyseItem *pAnalyseItem, tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind);
	//ֻ��һ����
	bool IsOnlyHuOneCard(const BYTE cbCardIndex[MAX_INDEX], tagWeaveItem WeaveItem[], BYTE cbItemCount, BYTE cbCurrentCard, BYTE cbHuaCount, DWORD dwPreHuPaiKind);
	//�Ƿ��Ǵ��
	bool IsBigHuPaiKind(DWORD dwHuPaiKind);
	//�Ƿ����߶Ժ�����
	bool IsHuPaiQiDuiKind(DWORD dwHuPaiKind);

///<ת������
public:
	//��ת��
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//��ת��
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//��ת��
	BYTE SwitchToCardData(BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//��ת��
	BYTE SwitchToCardIndex(BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);
};

//////////////////////////////////////////////////////////////////////////

#endif