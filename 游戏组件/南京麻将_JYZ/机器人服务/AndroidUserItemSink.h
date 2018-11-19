#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
//#include <algorithm>
//////////////////////////////////////////////////////////////////////////

//��������
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//������ʱ�����
public:
	static BYTE						m_cbMinReadyTime;						//��С׼��ʱ��
	static BYTE						m_cbMaxReadyTime;						//���׼��ʱ��
	static BYTE						m_cbMinOperateCardTime;					//��С����ʱ��
	static BYTE						m_cbMaxOperateCardTime;					//������ʱ��
	static BYTE						m_cbMinBaseScoreTime;					//��С���õ�עʱ��
	static BYTE						m_cbMaxBaseScoreTime;					//������õ�עʱ��

	//�û�����
protected:
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	LONG                            m_lBaseScore;							//�����ע
	BYTE                            m_cbSetBaseScoreStatus;					//���õ�ע״̬

	//״̬����
protected:
	bool							m_bHaveGameEnd;							//��Ϸ�Ƿ����
	bool							m_bTingStatus;							//����״̬
	DWORD							m_dwActionMask;							//��������
	BYTE							m_cbActionCard;							//������	

	tagTingPaiResult                m_TingResult;							//���ƽ��
	tagGangPaiResult                m_GangResult;							//���ƽ��
	//��
protected:
	BYTE							m_cbLeftCardCount;						//ʣ����Ŀ
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//������
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//�����

	//�����
protected:
	BYTE							m_cbWeaveCount[GAME_PLAYER];			//�����Ŀ
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][4];		//�����

	//������Ϣ
protected:
	WORD							m_wOutCardUser;							//�����û�
	BYTE							m_cbOutCardData;						//������
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//������Ŀ
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];		//������¼
	BYTE							m_cbSendCardData;						//������

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;					//�û��ӿ�


	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	typedef bool (CAndroidUserItemSink::*pSortFun)(BYTE cbCardLeft,BYTE cbCardRight);
	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//��ʼ�ӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�
	virtual bool RepositionSink();

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize);

	//�û��¼�
public:
	//�û�����
	virtual void OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û��뿪
	virtual void OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�����
	virtual void OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û�״̬
	virtual void OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
	//�û���λ
	virtual void OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

	//��Ϣ����
protected:
	//�������õ�ע��Ϣ
	bool OnSubSetBaseScore(void *pData, WORD wDataSize);
	//��������Ϣ
	bool OnSubShootDice(const void *pBuffer, WORD wDataSize);
	//ׯ����Ϣ
	bool OnSubGameStart(void * pData, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameEnd(void * pData, WORD wDataSize);
	//�û�����
	bool OnSubOutCard(const void * pBuffer, WORD wDataSize);
	//������Ϣ
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
	//����
	bool OnSubUserBuHua(const void *pBuffer, WORD wDataSize);
	//������ʾ
	bool OnSubOperateNotify(const void * pBuffer, WORD wDataSize);
	//�������
	bool OnSubOperateResult(const void * pBuffer, WORD wDataSize);
	//У�����
	bool VerdictOutCard(BYTE cbPreCardData);
	//������
protected:
	//����
	void ReInit();
	//�Ƿ�����
	bool IsYaoCard( BYTE cbCardData );
	//�����ж�
	bool OnOutCard();
	//�����������ж�
	bool OnOperateCard();
	//���Ʋ���
	void DoOutCard( BYTE cbOutCard );
	//��������������
	void DoOperateCard( DWORD dwOperateCode,BYTE cbOperateCard );
	//�������
	BYTE CalIsolatedCard(BYTE cbCalCards[],BYTE cbIsolateCard[],bool bGetAllIsolate = true);
	//�������������Ƶ�����
	BYTE CalNearCardNum(BYTE cbCard);
	//���Ų���
	DWORD GetTheBestOperate(DWORD dwOperateCode,BYTE cbOperateCard);
	//��������������ֶ����� �����Լ�����
	BYTE CalCardCountInTable(BYTE cbCardData);
	//���������Լ������ж�����
	BYTE CalCardCountInHand(BYTE cbCardData);
	//�ӿ��Դ�������м���ѡ���ų���
	BYTE ChooseIsolatedCard(IN BYTE cbCanOutCard[],IN BYTE cbCount);
	//���ָ����������
	BYTE GetCardFromCount(OUT BYTE cbCard[],IN const int nCardCount);
	//�ܷ����˳�� ���˳������
	DWORD GetCardShunType(BYTE cbCard);
	//�ж����ƶ�������
	BYTE GetDuiZiNum();
	//��ȱ�� 
	bool OutCardSameColor();
	//���߶�
	bool OutCardQiDui( IN BYTE cbCanOutCard[],IN BYTE cbCount);
	//��ȡ��ͬ��ɫ��
	BYTE GetSameColorCard(OUT BYTE cbSameColorCard[],IN BYTE cbColor);
	//��������������ʣ������������Ŀ��Դ����
	BYTE GetBestTingPutCard(const tagTingPaiResult & TingPaiData);
	//������
	void RandCardData( BYTE cbCardData[], BYTE cbMaxCount);
	//��������
	void SortCardHeap(BYTE cbCardData[],BYTE cbMaxCount, pSortFun pFun);
	//�����Ѿ�����������Ƿ����۾����� �۾�����
	bool CompareCard( BYTE cbCard1, BYTE cbCard2 );

public:
	//��ȡ����������
	static bool ReadAndroidConfig();
	//��ȡ�����˲���ʱ��
	static UINT GetAndroidOpTime( UINT uOpType );
	//����Ŀ¼
	static bool GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount);
};

//////////////////////////////////////////////////////////////////////////

#endif