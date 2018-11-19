#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "robottlogic.h"
//////////////////////////////////////////////////////////////////////////
//����ʱ��
#define TIME_LESS					1									//����ʱ��

#define IDI_ANDROID_ITEM_SINK		100

//��Ϸʱ��
#define IDI_START_GAME				(IDI_ANDROID_ITEM_SINK+0)			//��ʼ��ʱ��
#define IDI_USER_SCORE_END			(IDI_ANDROID_ITEM_SINK+1)			//�������
#define IDI_USER_FAN_END			(IDI_ANDROID_ITEM_SINK+2)			//���ƶ�������
#define IDI_USER_ENDPEICARD			(IDI_ANDROID_ITEM_SINK+3)			//������������ϲ���
#define IDI_USER_PEICARD			(IDI_ANDROID_ITEM_SINK+4)			//���Ʋ���
#define IDI_SET_BASESCORE           (IDI_ANDROID_ITEM_SINK+6)			//���õ�ע
#define IDI_REQUEST					(IDI_ANDROID_ITEM_SINK+5)			//������ǿ��
#define IDI_USER_ROBOTDELAY         (IDI_ANDROID_ITEM_SINK+7)			//�����˲����ӳ�
#define IDI_USER_ADD_SCORE          (IDI_ANDROID_ITEM_SINK+8)			//�����˸��ӷ��������ӳ�
#define IDI_ADDTAKEGOLD             (IDI_ANDROID_ITEM_SINK+9)           //���Ӵ�����

//ʱ���ʶ
#define TIME_START_GAME				6									//��ʼ��ʱ��
#define TIME_USER_ADD_SCORE			3									//��ע��ʱ��
#define	TIME_USER_COMPARE_CARD		3									//���ƶ�ʱ��
//��Ϸ�Ի���
class CAndroidUserItemSink : public IAndroidUserItemSink
{
public:
	LONG							m_lCellScore;							//��Ԫ��ע	
	LONG							m_lTableScore[GAME_PLAYER];				//��ע��Ŀ	
	LONG							m_lTurnMaxScore[GAME_PLAYER];			//ʣ��������ע
	LONG							m_lUserMaxScore[GAME_PLAYER];			//Я�������ע
	LONG                            m_lTurnLessScore;                       //��С��ע
	LONG							m_lAddLessScore;                        //��С��ע
	BYTE							m_cbCenterCardData[MAX_CENTERCOUNT];	//�����˿�
	BYTE							m_cbPlayStatus[GAME_PLAYER];			//��Ϸ״̬	
	WORD                            m_wCurrentUser;                         //��ǰ�û�
	int								m_iChipPoolCount;                       //

	BYTE                            m_bSetBaseScore;
	BYTE							m_cbBalanceCount;						//ƽ�����
	CRobottLogic                    m_RobottLogic;
    LONG                            m_lAddTakeGoldMin;        				//��������С����
	LONG                            m_lAddTakeGoldMax;        				//�������
	LONG                            m_lSelfTakeGold;          				//����Я�����ٽ��
	//�ؼ�����
public:
	IAndroidUserItem *				m_pIAndroidUserItem;					//�û��ӿ�	
	BYTE							m_cbHandCardData[GAME_PLAYER][2];       //�����˿�,0С�ƣ�1���ƣ�2����
	//��������
public:
	//���캯��
	CAndroidUserItemSink();
	//��������
	virtual ~CAndroidUserItemSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CAndroidUserItemSink))?true:false; }
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
	//��Ϸ��ʼ
	bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
	//����������
	bool OnGameRobot(const void * pBuffer, WORD wDataSize);
    //������Ӵ�����
	bool OnUserAddTakeGold(const void * pBuffer, WORD wDataSize);
	//�û�����
	bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
	//�û������ã���ע
	bool OnSubAddScore(const void * pBuffer, WORD wDataSize);
	//һ�ֽ���
	bool OnSubTurnOver(const void * pBuffer, WORD wDataSize);
	//�����˿�
	bool OnSubSendCard(const void * pBuffer, WORD wDataSize);	
	//��Ϸ����
	bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
	//���õ�ע
	bool OnSubSetBaseScore(const void * pBuffer, WORD wDataSize);
	//��ձ���
	void OnSubClear();
	//���ʹ���
	bool OnSubDaShang(void * pData, WORD wDataSize);
	//��������
	bool RobotGameMessage(WORD wChairID, LONG lScore,BYTE ID);
};

//////////////////////////////////////////////////////////////////////////

#endif
