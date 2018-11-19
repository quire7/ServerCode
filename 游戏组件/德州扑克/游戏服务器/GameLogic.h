#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿�����
#define CT_SINGLE					1									//��������
#define CT_ONE_LONG				    2									//��������
#define CT_TWO_LONG				    3									//��������
#define CT_THREE_TIAO				4									//��������
#define	CT_SHUN_ZI					5									//˳������
#define CT_TONG_HUA					6									//ͬ������
#define CT_HU_LU					7									//��«����
#define CT_TIE_ZHI					8									//��֧����
#define CT_TONG_HUA_SHUN			9									//ͬ��˳��
#define CT_KING_TONG_HUA_SHUN		10									//�ʼ�ͬ��˳
#define CT_MAXTYPE          		11									//��������

//�˿˴�С
#define CARD_VALUE_2				2									//��2	
#define CARD_VALUE_3				3									//��3	
#define CARD_VALUE_4				4									//��4	
#define CARD_VALUE_5				5									//��5	
#define CARD_VALUE_6				6									//��6	
#define CARD_VALUE_7				7									//��7	
#define CARD_VALUE_8				8									//��8	
#define CARD_VALUE_9				9									//��9	
#define CARD_VALUE_10				10									//��10	
#define CARD_VALUE_J				11									//��J	
#define CARD_VALUE_Q				12									//��Q	
#define CARD_VALUE_K				13									//��K	
#define CARD_VALUE_A				14									//��A	
//////////////////////////////////////////////////////////////////////////

//ʤ����Ϣ�ṹ
struct UserWinList
{
	BYTE bSameCount;
	WORD wWinerList[GAME_PLAYER];
};

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//������Ŀ
	BYTE 							cbThreeCount;						//������Ŀ
	BYTE 							cbLONGCount;						//������Ŀ
	BYTE							cbSignedCount;						//������Ŀ
	BYTE 							cbFourLogicVolue[1];				//�����б�
	BYTE 							cbThreeLogicVolue[1];				//�����б�
	BYTE 							cbLONGLogicVolue[2];				//�����б�
	BYTE 							cbSignedLogicVolue[5];				//�����б�
	BYTE							cbFourCardData[MAX_CENTERCOUNT];	//�����б�
	BYTE							cbThreeCardData[MAX_CENTERCOUNT];	//�����б�
	BYTE							cbLONGCardData[MAX_CENTERCOUNT];	//�����б�
	BYTE							cbSignedCardData[MAX_CENTERCOUNT];	//������Ŀ
};
//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼�
class CGameLogic
{
	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//��������
private:
	static BYTE						m_cbCardData[FULL_COUNT];					//�˿˶���
	static const WORD               m_wLogicCount;
public:
	static LONG						m_lGiveUp[4][INT_ROBOT_GIVEUP];				//�����˷�����
	static LONG						m_lAdd[4][INT_ROBOT_ADD];					//�����˼�ע��
	static LONG						m_lQuanYa[4][INT_ROBOT_QUANYA];				//������ȫѺ
	static CString					m_strFileName;
	static CString					m_strFileName2;

	//���ƺ���
public:
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//����
	void SortValueList(LONG cbValue[], BYTE ID[], BYTE cbCardCount);
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }
	//��ȡ�����ļ�
	static BOOL SetConfigInit(void);

	//���ܺ���
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	BYTE GetCardLogicValue1(BYTE cbCardData[],BYTE CardType[]);
	//�Ա��˿�
	BYTE CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);
	//�����˿�
	void AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//7��5
	BYTE FiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE cbLastCardCount);
	//�ж��Ƿ�����������
	BYTE GetFiveFromSeven(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE cbCenterCardData[],BYTE cbCenterCardCount,BYTE cbLastCardData[],BYTE CardType);

	//�����������
	BYTE FinishCard(BYTE cbEndCardData[],BYTE cbEndCardData1[],BYTE cbHandCardCount1,BYTE cbHandData[],BYTE Type);
	//�������
	bool SelectMaxUser(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],UserWinList &EndResult,BYTE bHandData[GAME_PLAYER][2],const LONG lm_AddScore[]);
	//�˿���������
	bool SelectCardPaiXu(BYTE bCardData[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wchair[GAME_PLAYER][MAX_CENTERCOUNT],BYTE wCount[]);
	//�Ƚ����ƴ�С
	BYTE SelectTwoCard(BYTE Vale1[],BYTE Vale2[]);
};

//////////////////////////////////////////////////////////////////////////

#endif