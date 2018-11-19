#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

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
#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE					3									//��������
#define CT_SINGLE_LINK				4									//��������
#define CT_DOUBLE_LINK				5									//��������
#define CT_THREE_LINK				6									//��������
#define CT_THREE_DOUBLE				7									//��������
#define CT_TONG_HUA_SHUN			8									//ͬ��˳��
#define CT_BOMB						9									//ը������
#define CT_BOMB_TW					10									//����ը��
#define TYPE_COUNT					11                                  //����������������0~10��

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

//////////////////////////////////////////////////////////////////////////
//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT];			//�˿�����
	//�߼�����
	BYTE							m_cbMainValue;						//������ֵ
	
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//��Ϣ����
public:
	//������ֵ
	VOID SetMainValue(BYTE cbMainValue) { m_cbMainValue = cbMainValue; }
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//��ȡ������
	BYTE GetJingongCard(const BYTE cbCardData[]);

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ɫ
	BYTE GetCardColor(const BYTE cbCardData[], BYTE cbCardCount);
	 //��ȡ����
	BYTE GetMagicCardType(const BYTE cbCardData[], BYTE cbCardCount);

	//���ƺ���
public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//����˿�
	bool Add(BYTE bAddCard, BYTE  cbCardData[], BYTE cbCardCount = 27);

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
};

//////////////////////////////////////////////////////////////////////////

#endif