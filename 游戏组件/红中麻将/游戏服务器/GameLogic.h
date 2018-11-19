#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�߼�����

#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//////////////////////////////////////////////////////////////////////////
//��������

//������־
#define WIK_NULL					0x00								//û������
#define WIK_LEFT					0x01								//�������
#define WIK_CENTER					0x02								//�г�����
#define WIK_RIGHT					0x04								//�ҳ�����
#define WIK_PENG					0x08								//��������
#define WIK_GANG					0x10								//��������
#define WIK_XIAO_HU					0x20								//С��
#define WIK_CHI_HU					0x40								//�Ժ�����
#define WIK_ZI_MO					0x80								//����;
#define WIK_TING					0x100								//��������;
#define WIK_MING_GANG				0X200								//��������;
#define WIK_AN_GANG					0X400								//��������;

//////////////////////////////////////////////////////////////////////////
//���ƶ���

//����
#define CHK_NULL					0x00										//�Ǻ�����
#define CHK_CHI_HU					0x01										//������

// ���
#define CHR_PENGPENG_HU				0x00000001									//������
#define CHR_JIANGJIANG_HU			0x00000002									//������
#define CHR_QING_YI_SE				0x00000004									//��һɫ
#define CHR_HAI_DI_LAO				0x00000008									//������
#define CHR_HAI_DI_PAO				0x00000010									//������
#define CHR_QI_XIAO_DUI				0x00000020									//��С��
#define CHR_HAOHUA_QI_XIAO_DUI		0x00000040									//������С��
#define CHR_GANG_KAI				0x00000080									//���Ͽ���
#define CHR_QIANG_GANG_HU			0x00000100									//���ܺ�
#define CHR_GANG_SHANG_PAO			0x00000200									//������
#define CHR_QUAN_QIU_REN			0x00000400									//ȫ����

//С��
#define CHR_XIAO_DA_SI_XI			0x00004000									//����ϲ
#define CHR_XIAO_BAN_BAN_HU			0x00008000									//����
#define CHR_XIAO_QUE_YI_SE			0x00010000									//ȱһɫ
#define CHR_XIAO_LIU_LIU_SHUN		0x00020000									//����˳


#define CHR_ZI_MO					0x01000000									//����
#define CHR_SHU_FAN					0x02000000									//�ط�
//////////////////////////////////////////////////////////////////////////

#define ZI_PAI_COUNT	7

//��������
struct tagKindItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbCardIndex[3];						//�˿�����
	BYTE							cbValidIndex[3];					//ʵ���˿�����
};

//�������
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//�������
	BYTE							cbCenterCard;						//�����˿�
	BYTE							cbPublicCard;						//������־
	WORD							wProvideUser;						//��Ӧ�û�
};

//���ƽ��
struct tagGangCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbCardData[4];						//�˿�����
};

//��������
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//�����˿�
	bool                            bMagicEye;                          //�����Ƿ�������
	BYTE							cbWeaveKind[4];						//�������
	BYTE							cbCenterCard[4];					//�����˿�
	BYTE                            cbCardData[4][4];                   //ʵ���˿�
};

//////////////////////////////////////////////////////////////////////////

#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	Ȩλ�ࡣ
//  ע�⣬�ڲ�����λʱ���ֻ��������Ȩλ.����
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)������������޶���ġ�
//  ֻ�ܵ�������:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//��̬����
private:
	static bool						m_bInit;
	static DWORD					m_dwRightMask[MAX_RIGHT_COUNT];

	//Ȩλ����
private:
	DWORD							m_dwRight[MAX_RIGHT_COUNT];

public:
	//���캯��
	CChiHuRight();

	//���������
public:
	//��ֵ��
	CChiHuRight & operator = ( DWORD dwRight );

	//�����
	CChiHuRight & operator &= ( DWORD dwRight );
	//�����
	CChiHuRight & operator |= ( DWORD dwRight );

	//��
	CChiHuRight operator & ( DWORD dwRight );
	CChiHuRight operator & ( DWORD dwRight ) const;

	//��
	CChiHuRight operator | ( DWORD dwRight );
	CChiHuRight operator | ( DWORD dwRight ) const;

	//���ܺ���
public:
	//�Ƿ�ȨλΪ��
	bool IsEmpty();

	//����ȨλΪ��
	void SetEmpty();

	//��ȡȨλ��ֵ
	BYTE GetRightData( DWORD dwRight[], BYTE cbMaxCount );

	//����Ȩλ��ֵ
	bool SetRightData( const DWORD dwRight[], BYTE cbRightCount );

private:
	//���Ȩλ�Ƿ���ȷ
	bool IsValidRight( DWORD dwRight );
};


//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CYSArray<tagAnalyseItem, tagAnalyseItem &> CAnalyseItemArray;

//��Ϸ�߼���
class CGameLogic
{
	//��������
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//�˿�����
	static const BYTE				m_cbCardDataArray_HZ[MAX_REPERTORY_HZ];	//�˿�����
	BYTE							m_cbMagicIndex;						//��������

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ƺ���
public:
	//�����˿�
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//ɾ���˿�
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//��������
	void SetMagicIndex( BYTE cbMagicIndex ) { m_cbMagicIndex = cbMagicIndex; }
	//����
	bool IsMagicCard( BYTE cbCardData );

	//��������
public:
	//��Ч�ж�
	bool IsValidCard(BYTE cbCardData);
	//�˿���Ŀ
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//����˿�
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);

	//�ȼ�����
public:
	//�����ȼ�
	BYTE GetUserActionRank(DWORD cbUserAction);
	//���Ƶȼ�
	WORD GetChiHuActionRank(const CChiHuRight & ChiHuRight);
	WORD GetChiHuActionRank_ZZ(const CChiHuRight & ChiHuRight);
	WORD GetChiHuActionRank_CS(const CChiHuRight & ChiHuRight);

	//�����ж�
public:
	//�����ж�
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//�����ж�
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//�����ж�
public:
	//���Ʒ���,(�޸Ĺ��ܱ����һʱ���);
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult, BYTE cbSendCardData);
	//�Ժ�����
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);
	//���Ʒ���
	DWORD AnalyseTingCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagTingPaiResult &TingPaiResult, bool bCheckTianTing=false);
	//�Ƿ�����
	bool IsTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );
	//�Ƿ���
	bool IsHuaZhu( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );

	//ת������
public:
	//�˿�ת��
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//�˿�ת��
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//�˿�ת��
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//�˿�ת��
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);

	//��������
public:
	//�����
	bool IsPengPeng( const tagAnalyseItem *pAnalyseItem );
	//��һɫ��
	bool IsQingYiSe(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], const BYTE cbItemCount,const BYTE cbCurrentCard);
	//��С����
	bool IsQiXiaoDui(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount,const BYTE cbCurrentCard,int& nGenCount);

	//����
	bool IsDaiYao( const tagAnalyseItem *pAnalyseItem );
	//����
	bool IsJiangDui( const tagAnalyseItem *pAnalyseItem );
	//������
	bool IsJiangJiangHu(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[],const  BYTE cbWeaveCount,const BYTE cbCurrentCard);
	bool IsDanDiao(const BYTE cbCardIndex[MAX_INDEX],const BYTE cbCurrentCard);

	//�����˿�
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//����,������ֵ����
	bool SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount );

	BYTE GetPickNiaoCount( BYTE cbCardData[MAX_NIAO_CARD],BYTE cbCardNum);
};

//////////////////////////////////////////////////////////////////////////
#pragma pack()
#endif
