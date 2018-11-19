#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////
//��Ϸ������;
class CTableFrameSink : public ITableFrameSink,public ITableUserAction
{
	//���Ʊ���;
private:
	WORD							m_wFriendFirst;						//�ѷ�����;
	BYTE							m_cbUserTrustee[GAME_PLAYER];		//�û��й� 0:NO  1:User 2:System;
	bool							m_bUserOffline[GAME_PLAYER];		//��ҵ���;

	//��Ϸ����;
protected:
	WORD							m_wBankerUser;						//ׯ���û�;
	WORD							m_wCurrentUser;						//��ǰ���;
	WORD							m_wLastOutUser;						//�ϼҳ������;
	bool                            m_bGameEnd;							//��Ϸ����;

	//������Ϣ;
protected:
	WORD							m_wTurnWiner;						//ʤ�����;
	BYTE							m_cbTurnCardType;					//�˿�����
	BYTE							m_cbTurnCardCount;					//������Ŀ;
	BYTE							m_cbTurnCardData[27];				//�����б�;
	BYTE							m_cbMagicCardData[MAX_COUNT];		//����˿�

	//��Ϸ״̬;
	Series                          m_stSeries;                         //��Ϸ����;
	BYTE                            m_cbRanking[4];                     //��Ϸ����;
	BYTE							m_cbUserCurRank[4];					//��Ҿּ�ʱ����;
	BYTE                            m_cbMark;                           //��������;
	BYTE                            m_cbGameStatus;						//ʤ������;
	BYTE							m_cbPayTributeStatus;				//�����ع�״̬;
	BYTE                            m_cbPayTribute[4];                  //�Ƿ����;

	BYTE							m_cbRequestTimes[4];				//�����뿪����;
	list<WORD>						m_lstRequestUserID;					//�����˳����ID�б�;
	BYTE							m_cbResponseStatus[4];				//�ش�״̬;
	bool							m_bHaveSendRequestQuit;				//�Ѿ�����;

	//��������;
protected:
    BYTE                            m_cbTributeCard[4];					//�����˿�;
	bool							m_bTributeStatus[4];				//����״̬;
	WORD							m_wTributeBigID;					//����������;
	WORD							m_aryRetTributeTarget[4];			//�ع�����;

	//�˿���Ϣ;
protected:
	BYTE							m_cbHandCardCount[4];				//�˿���Ŀ;
	BYTE							m_cbHandCardData[4][28];			//�����˿�;

	//��������;
protected:
	LONG							m_lBaseScore;						//��Ϸ��ע;

	//�������;
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�;

	ITableFrame						* m_pITableFrame;					//��ܽӿ�;
	tagCustomRule					* m_pGameCustomRule;				//�Զ�����;
	const tagGameServiceOption		* m_pGameServiceOption;				//���ò���;

	//���Ա���;
protected:
	static const WORD				m_wPlayerCount;						//��Ϸ����;

	//��������;
public:
	//���캯��;
	CTableFrameSink();
	//��������;
	virtual ~CTableFrameSink();

	//�����ӿ�;
public:
	//�ͷŶ���;
	virtual void Release() { if (IsValid()) delete this; return ; }
	//�Ƿ���Ч;
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ;
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�;
public:
	//��ʼ��;
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����;
	virtual VOID RepositionSink();

	//��Ϸ�¼�;
public:
	//��Ϸ��ʼ;
	virtual bool OnEventGameStart();
	//��Ϸ����;
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���;
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);


	//�¼��ӿ�;
public:
	//��ʱ���¼�;
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam);
	//�����¼�;
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//�����¼�;
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return true; }

	//�����ӿ�;
public:
	//���û���;
	virtual void SetGameBaseScore(LONG lBaseScore){};

	virtual void SetPrivateInfo(BYTE bGameTypeIdex, DWORD bGameRuleIdex){ ; }
	//����˽�˳�������Ϣ;
	virtual void ResetPrivateEndInfo();
	//���˽�˳�������Ϣ��;
	virtual void GetPrivateEndInfo(DataStream &kDataStream, bool bSend);
	//�ж�˽�˳��Ƿ����;
	virtual	bool IsPrivateEnd();

	//��ѯ�ӿ�;
public:
	//��ѯ�޶�;
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) { return 0; }
	//���ٻ���;
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) { return 0; }
	//��ѯ�Ƿ�۷����;
	virtual bool QueryBuckleServiceCharge(WORD wChairID) { return true; };

	//����ӿ�;
public:
	//��Ϸ��Ϣ����;
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����;
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//�û��¼�;
public:
	//�û�����;
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����;
	virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem);
	//�û�����;
	virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����;
	virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��;
	virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) { return true; }
	//���������ж�;
	virtual bool OnActionUserFangKaCheck(WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure) { return true; }

	//��Ϸ�¼�;
protected:

	//�û�����;
	bool OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount);
	//�û�����;
	bool OnSubPassCard(WORD wChairID);
    //�û�����;
	bool OnSubPayTribute(WORD wChairID, const void * pDataBuffer);
	//�û��ع�;
	bool OnSubReturnTribute(WORD wChairID, const void * pDataBuffer);
	//�û��й�;
	bool OnSubTrustee(WORD wChairID, const void* pDataBuffer);

	//�û������˳�;
	bool OnSubRequestQuit(const void * pDataBuffer);
	//�ظ��û��˳�����;
	bool OnSubResponseQuit(WORD wMeChiarID, const void * pDatabuffer);
	
	//���ܺ���;
public:
	//��Ϸ��ʼ;
	bool OnGameStart();
	//��(��)����ʱ;
	bool OnTributeTimeOut(BYTE cbTributeType);
	//���Ƴ�ʱ;
	bool OnOutCardTimeOut();
	//���������˳���Ϣ;
	void SendRequestQuitMsg();
	//�ظ�������;
	bool OnDoResponseQuit(WORD wMeChairID, WORD wRequestID, BYTE cbResponseQuitType);


	//˽�˳�������Ϣ;
public:
	CMD_S_Private_End_Info			m_PrivateEndInfo;
};

//////////////////////////////////////////////////////////////////////////

#endif