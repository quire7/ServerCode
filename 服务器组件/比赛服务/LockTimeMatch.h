#ifndef LOCKTIME_MATCH_HEAD_FILE
#define LOCKTIME_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "DistributeManager.h"

///////////////////////////////////////////////////////////////////////////////////////////

//ʱ�Ӷ���
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+1)					//��ʼʱ��
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+2)					//����ʱ��

//�Ƴ�����
#define REMOVE_BY_ENDMATCH			250											//��������

///////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//����״̬
enum emMatchStatus
{
	MatchStatus_Free=0,
	MatchStatus_Playing,
	MatchStatus_End
};

//////////////////////////////////////////////////////////////////////////////////////////

//��ʱ��
class CLockTimeMatch : public IGameMatchItem,public IMatchEventSink,IServerUserItemSink
{
	//״̬����
protected:
	emMatchStatus						m_MatchStatus;					//����״̬

	//��������
protected:
	CDistributeManager					m_DistributeManage;             //�������

	//��������
protected:
	tagGameMatchOption *				m_pMatchOption;					//��������	
	tagGameServiceOption *				m_pGameServiceOption;			//��������
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//��������

	//�ں˽ӿ�
protected:
	ITableFrame	**						m_ppITableFrame;				//��ܽӿ�
	ITimerEngine *						m_pITimerEngine;				//ʱ������
	IDBCorrespondManager *				m_pIDataBaseEngine;				//��������	
	ITCPNetworkEngineEvent *			m_pITCPNetworkEngineEvent;		//��������

	//����ӿ�
protected:
	IMainServiceFrame *					m_pIGameServiceFrame;			//���ܽӿ�
	IServerUserManager *				m_pIServerUserManager;			//�û�����
	IAndroidUserManager	*				m_pAndroidUserManager;			//��������
	IServerUserItemSink *				m_pIServerUserItemSink;			//�û��ص�

	//��������
public:
	//���캯��
	CLockTimeMatch();
	//��������
	virtual ~CLockTimeMatch(void);

	//�����ӿ�
public:
 	//�ͷŶ���
 	virtual VOID Release(){ delete this; }
 	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//����֪ͨ
	virtual void OnStartService() { return ; }

	//����ӿ�
public:
	//������
	virtual bool BindTableFrame(ITableFrame * pTableFrame,WORD wTableID);
	//��ʼ���ӿ�
	virtual bool InitMatchInterface(tagMatchManagerParameter & MatchManagerParameter);	

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,BYTE cbRightKind);

	//�¼��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnEventEnterMatch(DWORD dwSocketID ,VOID* pData,DWORD dwUserIP, bool bIsMobile);	
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason,DWORD dwSocketID);
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank=NULL, DWORD dwContextID=INVALID_WORD);

	 //���ܺ���
public:
	 //��Ϸ��ʼ
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //��Ϸ����
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	 //�û��¼�
public:
	 //�û�����
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�����
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�ͬ��
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	 //�û��¼�
public:
	//��ҷ���
	virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame, IServerUserItem * pIServerUserItem);

	//��������
protected:
	//������
	void ClearSameTableUser(DWORD dwUserID);
	//�����û�
	void InserSameTableUser(DWORD dwUserID,DWORD dwTableUserID);
	//���ͷ���
	bool SendMatchUserScore(IServerUserItem * pIServerUserItem);
	//�Ƴ�����
	bool RemoveDistribute(IServerUserItem * pIServerUserItem);
	//�����û�
	bool InsertDistribute(IServerUserItem * pIServerUserItem);		

	//У�麯��
private:
	//����У��
	bool VerifyUserEnterMatch(IServerUserItem * pIServerUserItem);
	//ʱ��У��
	bool VerifyMatchTime(LPTSTR pszMessage,WORD wMaxCount,WORD & wMessageType);		

	//��������
protected:
	//�����û�
	bool PerformDistribute();
	//��ʼ���
	DWORD GetMatchStartInterval();	
	//���ͱ�����Ϣ
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//д�뽱��
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);

	//�û�����
public:
	//
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem) { return true; };

	//��Ϸ�¼�;
public:
	//�û�����
	virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) { return true; };
	//��Ϸ��¼
	virtual bool WriteTableScore(ITableFrame* pITableFrame, tagScoreInfo ScoreInfoArray[], WORD wScoreCount, DataStream& kData) { return true; };
	//�жϷ����Ƿ�
	virtual bool OnActionUserFangKaCheck(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure) { return true; };

	//˽�˳��û��¼�
	virtual bool AddPrivateAction(ITableFrame* pTbableFrame, DWORD dwChairID, BYTE	bActionIdex) { return true; };
	//ͳ��˽�˳���Ҿ���
	virtual bool addPrivatePlayCout(ITableFrame* pTbableFrame, WORD wCout) { return true; };
};

#endif