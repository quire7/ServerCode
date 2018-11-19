#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"

#include "AILogic.h"
//////////////////////////////////////////////////////////////////////////

//��������;
class CAndroidUserItemSink : public IAndroidUserItemSink
{
	//������ʱ�����;
private:
	static BYTE						m_cbMinSetTableTime;						//��С����ʱ��
	static BYTE						m_cbMaxSetTableTime;						//�������ʱ��
	static BYTE						m_cbMinStartTime;							//��С׼��ʱ��
	static BYTE						m_cbMaxStartTime;							//���׼��ʱ��
	static BYTE						m_cbMinTributeTime;							//��С����ʱ��
	static BYTE						m_cbMaxTributeTime;							//������ʱ��
	static BYTE						m_cbMinBackTributeTime;						//��С�ع�ʱ��
	static BYTE						m_cbMaxBackTributeTime;						//���ع�ʱ��
	static BYTE						m_cbMinOutCardTime;							//��С����ʱ��
	static BYTE						m_cbMaxOutCardTime;							//������ʱ��
	static BYTE						m_cbMinSetSeries;							//���õ���С����
	static BYTE						m_cbMaxSetSeries;							//���õ������
	static bool						m_bLoadConfig;								//�Ƿ���ع�����

	//�˿˱���;
public:
	BYTE							m_cbHandCardCount;							//�˿���Ŀ
	BYTE							m_cbHandCardData[MAX_COUNT];				//�����˿�
	BYTE							m_cbTurnCardCount;							//������Ŀ
	BYTE							m_cbTurnCardData[MAX_COUNT];				//�����б�
	BYTE							m_cbAllCardCount[GAME_PLAYER];				//�����������
	BYTE							m_cbAllCardData[GAME_PLAYER][MAX_COUNT];	//�������������
	BYTE							m_cbTributeCard;							//������
	WORD							m_wFromUser;								//�����û�

	//�û�����;
	WORD							m_wCurrentUser;								//��ǰ����
	WORD							m_wOutCardUser;								//�������
	WORD							m_wRequestQuitID;							//�������

	BYTE							m_cbCurSeries;								//����
	LONG							m_lBaseScore;								//��ע

	//�ؼ�����;
protected:
	CAILogic						m_GameLogic;								//��Ϸ�߼�
	IAndroidUserItem *				m_pIAndroidUserItem;						//�û��ӿ�

	//��������;
public:
	//���캯��;
	CAndroidUserItemSink();
	//��������;
	virtual ~CAndroidUserItemSink();

	//�����ӿ�;
public:
	//�ͷŶ���;
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ;
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�;
public:
	//��ʼ�ӿ�;
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//���ýӿ�;
	virtual bool RepositionSink();

	//��Ϸ�¼�;
public:
	//ʱ����Ϣ;
	virtual bool OnEventTimer(UINT nTimerID);
	//��Ϸ��Ϣ;
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ��Ϣ;
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ;
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//�û��¼�;
public:
	//�û�����;
	virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//�û��뿪;
	virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//�û�����;
	virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//�û�״̬;
	virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}
	//�û���λ;
	virtual VOID OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser){}

	//��Ϣ����;
protected:
	//��Ϸ��ʼ;
	bool OnSubSendCard(void * pData, WORD wDataSize);
	//����֪ͨ;
	bool OnSubTributeCardNotify(void * pData, WORD wDataSize);
	//�����˿�;
	bool OnSubTributeOneCard(void * pData, WORD wDataSize);
	//��������;
	bool OnSubTributeCardResult(void * pData, WORD wDataSize);
	//�ع�;
	bool OnSubReturnTribute(void * pData, WORD wDataSize);
	//��Ϸ��ʼ;
	bool OnSubStartOutCard(void * pData, WORD wDataSize);
	//�û�����;
	bool OnSubOutCard(void * pData, WORD wDataSize);
	//�û�����;
	bool OnSubPassCard(void * pData, WORD wDataSize);
	//��Ϸ����;
	bool OnSubGameEnd(void * pData, WORD wDataSize);
	//�û������˳�;
	bool OnSubRequestQuit(void * pData, WORD wDataSize);
	//��ȡ�����������;
	bool OnSubGetAllCard(void * pData, WORD wDataSize);

	//���ܺ���;
public:
	//��ȡ����������;
	static bool ReadAndroidConfig();

protected:
	//��ȡ�����˲���ʱ��;
	UINT GetAndroidOpTime(UINT uOpType);
	//���ƺ���;
	void SearchOutCardData();
};

//////////////////////////////////////////////////////////////////////////

#endif