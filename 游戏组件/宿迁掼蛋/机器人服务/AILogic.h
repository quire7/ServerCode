#pragma once

///////////////////////////////////////////////////////////////////////////////////
//��Ŀ����
#define FULL_COUNT					108									//ȫ����Ŀ

///////////////////////////////////////////////////////////////////////////////////
//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

///////////////////////////////////////////////////////////////////////////////////
//��ֵ����
#define MIN_LOGIC_VALUE             1                                   //��С�߼�ֵ
#define MAX_LOGIC_VALUE             18                                  //����߼�ֵ

///////////////////////////////////////////////////////////////////////////////////
//��������
#define ST_ORDER					0									//��С����
#define ST_COUNT					1									//��Ŀ����
#define ST_VALUE					2									//��ֵ����
#define ST_COLOR					3									//��ɫ����
#define ST_CUSTOM					4									//�Զ�����

///////////////////////////////////////////////////////////////////////////////////
//�˿�����
#define AI_CT_ERROR					0									//��������
#define AI_CT_SINGLE				1									//��������
#define AI_CT_DOUBLE				2									//��������
#define AI_CT_THREE					3									//��������
#define AI_CT_SINGLE_LINK			4									//��������
#define AI_CT_DOUBLE_LINK			5									//��������
#define AI_CT_THREE_LINK			6									//��������
#define AI_CT_THREE_DOUBLE			7									//��������
#define AI_CT_TONG_HUA_SHUN			8									//ͬ��˳��
#define AI_CT_BOMB					9									//ը������
#define AI_CT_BOMB_TW				10									//����ը��
#define TYPE_COUNT					11                                  //����������������0~10��
#define MAX_TYPE_COUNT				254

///////////////////////////////////////////////////////////////////////////////////////
//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbBlockCount[10];					//�˿���Ŀ
	BYTE							cbCardData[10][MAX_COUNT];			//�˿�����
};

//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT];			//����˿�
};

//�ֲ���Ϣ
struct tagDistributing
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbDistributing[15][6];				//�ֲ���Ϣ
};

struct tagMagicResult
{
	BYTE							cbMagicCardCount;					//ħ������Ŀ
	BYTE							cbNormalCardCount;					//��������Ŀ
	BYTE							cbMagicCardData[2];					//ħ��������
	BYTE							cbNormalCardData[MAX_COUNT];		//����������
};

//������
struct CardTypeImfomation
{
	BYTE							cbLikeCardTypeCount;
	BYTE							cbLikeCardType[TYPE_COUNT];
	BYTE							cbUnLikeCardTypeCount;
	BYTE							cbUnLikeCardType[TYPE_COUNT];
};

//���������Ϣ
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

//��������(�������)
struct tagBestOutType
{
	BYTE							cbCardType[GAME_PLAYER];
	WORD							cbCardSafeValue[GAME_PLAYER];
};

struct tagOutCardTypeResult 
{
	BYTE							cbCardType;							//�˿�����
	BYTE							cbCardTypeCount;					//������Ŀ
	BYTE							cbEachHandCardCount[MAX_TYPE_COUNT];//ÿ�ָ���
	BYTE							cbEachHandCardData[MAX_TYPE_COUNT][MAX_COUNT];//�˿�����
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class CAILogic
{
	//�߼�����
protected:
	BYTE							m_cbMainValue;						//������ֵ

	//��������
public:
	//���캯��
	CAILogic();
	//��������
	virtual ~CAILogic();

	//��Ϣ����
public:
	//������ֵ
	VOID SetMainValue(BYTE cbMainValue) { m_cbMainValue=cbMainValue; }
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ɫ
	BYTE GetCardColor(const BYTE cbCardData[], BYTE cbCardCount);

	//���ƺ���
public:
	//�����˿�
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//�߼�����
public:
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);

	//���ܺ���
public:
	//�����˿�
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult &AnalyseResult);
	//�����ֲ�
	VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing &Distributing);
	//����˿�
	bool MagicCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbResultCard[MAX_COUNT]);

	//�ڲ�����
private:
	//�����˿�
	BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//�Ƿ�����
	bool IsStructureLink(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCellCount);
	//�Ա��˿�
	bool CompareCardByValue(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);

	//��������
protected:
	//����ͬ��˳
	bool SearchTongHuaShun(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult);
	//������Ʒ����˿�
	VOID AnalyseMagicData(const BYTE cbHandCardData[], BYTE cbHandCardCount, tagMagicResult &MagicResult);
	//�������ͬ��˳
	bool SearchMagicTongHuaShun(const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, tagOutCardResult &OutCardResult);

	//�����˸�������
public:
	//��������
	void AnalyseOutCardType(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT/*+1*/]);
	//���Ƽ�¼�ĺ����ܻ�
	void SetOutCardImfomation(WORD wMeChairID, WORD wOutChairID, WORD wCurrentID, BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbTurnCardData[], BYTE cbTurnCount, bool bOutCard, bool bFirstOutCard);
	//��¼ʣ�����
	void RecordOutCard(BYTE cbOutCard[], BYTE cbOutCount);
	//���ó�ʼֵ
	void SetInitData();
	//��¼��С��
	void RecordKindCard(WORD wMeChairID, BYTE cbHandData[], BYTE cbHandCount, WORD wPassaChairID, WORD wOutChairID, BYTE cbTurnData[], BYTE cbTurnCount, PlayerOutImfomation ForOutImfo);
	//��¼���Ƶ�����
	void RecordOutCardType(WORD wOutChairID, BYTE cbTurnData[], BYTE cbTurnCount, bool bFirst, WORD wPassChairID, BYTE cbHandData[], BYTE cbHandCount, PlayerOutImfomation ForOutImfo);
	//��ȡ������
	void GetBestCardType(WORD wOutChairID, BYTE cbTurnCard[], BYTE cbTurnCount, WORD wPassID);
	//����˳��
	int SerarchSingleLink(BYTE cbCardData[], BYTE cbCardCount, int iMagicCount, tagOutCardResult &OutCardResult);
	//���Ҹ�5���߼���֮��������ֻ�ɫ�ĸ���
	bool NearbyColorCount(BYTE cbHandCardData[MAX_COUNT], BYTE cbHandCardCount, BYTE cbCardData, BYTE ColorCount[4]);
	//�ƶ�������ʣ��������߼�ֵ
	BYTE SearchMaxLogicValue(BYTE cbType, BYTE cbHandData[], BYTE cbHandCount);
	//��������
	bool SearchType(BYTE cbType, bool bLike, WORD wChairID, bool bMove);
	//��ȡ�����Ƶ��߼�ֵ
	BYTE GetTypeCardValue(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ����
	BYTE GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//������ͱȽ�
	bool CompareMagicCard(BYTE cbFirstCard[], BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	//�Ƿ���Բ���
	bool AnalyseCardPass(BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbOutCard[], BYTE cbCardType, tagOutCardResult &OutCardResult);
	//����ͬ��˳
	bool AnalyseTongHuaShun(BYTE const cbHandCardData[], BYTE cbHandCardCount, tagOutCardTypeResult &CardTypeResult);
	//��������(��ͬ��˳)
	void AnalyseOutCardTypeNomal(BYTE const cbHandCardData[], BYTE const cbHandCardCount, tagOutCardTypeResult CardTypeResult[TYPE_COUNT/*+1*/]);
	
public:
	//��ȡ�����û�������Ϣ
	void OnHandCardChange(const BYTE cbAllUserHandCardData[][MAX_COUNT], const BYTE cbAllUserHandCardCount[]);
	//�����û�����
	void SetUserHandCard(WORD wUserID, const BYTE cbUserCardData[], BYTE cbUserCardCount);
	//�ж�һ���û��Ƿ�Σ��
	bool IsDangerous(WORD wUserChairID);
	//�ų������е�ը�������Ƶ�����
	BYTE GetAfterRemoveBombsCardType(const BYTE cbHandCardData[], const BYTE cbHandCardCount);

	//��֤����
public:
	//��֤Ҫ�������Ƿ���ָ��������
	bool CheckOutCardRightType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbType);
	//��֤Ҫ�������Ƿ�����ѳ���
	bool CheckWillOutCard(WORD wOutID, WORD wMeID, BYTE cbCard[], BYTE cbCount, BYTE cbTurnCard[], BYTE cbTurnCount);
	//��֤Ҫ�������Ƿ����Լ�������
	bool CheckOutCardInHand(const BYTE cbHandData[], const BYTE cbHandCount, BYTE cbOutCard[], BYTE cbOutCount);

	//�������
public:
	//��ȡ������
	BYTE GetTributeCard(WORD wChariID, BYTE cbHandCardData[], BYTE cbHandCardCount);
	//��ȡ�ع���
	BYTE GetBackTributeCard(WORD wChariID, BYTE cbHandCardData[], BYTE cbHandCardCount, WORD wThrowUserNum, bool bThrowToFriend);
	//����Ҫ������
	bool SearchOutCardAI(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult);
	//�׼ҳ���
	void SearchFirstOutCard(WORD wMeChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, tagOutCardResult &OutCardResult);
	//�¼ҽ���
	void SearchNextOutCard(WORD wMeChairID, WORD wOutChairID, const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult &OutCardResult);


private:
	BYTE					m_cbHandCardCount[GAME_PLAYER];						  	   //�����������
	BYTE					m_cbHandCardData[GAME_PLAYER][MAX_COUNT];				   //�������������

public:
	int                     m_iOutCardCount[MAX_LOGIC_VALUE+1];                        //15��������ƣ�0�洢�ɱ�õ���
	BYTE                    m_cbKindCount[GAME_PLAYER][2];                             //���ӵ�еĴ�С������
	CardTypeImfomation      m_PlayerTypeImfo[GAME_PLAYER];                             //�������ͼ�¼    
	PlayerOutImfomation     m_ForOutImfo;                                              //ǰһ�������
	OutCardImfomation       m_CurPlayerOutTimes;                                       //��ǰ����������ƴ���
	bool                    m_bFirstOutCard;                                           //�Ƿ����״γ���
	BYTE                    m_cbCardNum[GAME_PLAYER];                                  //�˿���Ŀ
	tagBestOutType          m_PlayerBestOutType;                                       //��������

private:
	WORD					m_wRoundOutTimes;										   //����������ƴ���
};

//////////////////////////////////////////////////////////////////////////
