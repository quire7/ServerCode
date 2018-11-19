#ifndef PRIVATE_HEAD_FILE
#define PRIVATE_HEAD_FILE

#pragma once

//�����ļ�
#include "CTableFramePrivate.h"
#include "PrivateServiceHead.h"
#include "PrivateTableInfo.h"

///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

//ʱ�Ӷ���
#define IDI_DISMISS_WAITE_END		(IDI_PRIVATE_MODULE_START+1)					//�����ɢ����

#ifdef _DEBUG
#define DISMISS_WAITE_TIME		60					//�����ɢʱ��
#else
#define DISMISS_WAITE_TIME		60*5				//�����ɢʱ��
#endif

#define AGAIN_WAITE_TIME		30					//���¿�ʼ�ȴ�ʱ��

#define CREATE_PRIVATE_FREE_TIME		30*60		//�������䵫δ��ʼ��ʱ��

struct DBR_GR_Create_Private;

//��ʱ��
class PriaveteGame 
	:public IGameMatchItem
	,public IMatchEventSink
	,public IServerUserItemSink
{
	//��������
protected:
	tagGameServiceOption *				m_pGameServiceOption;			//��������
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//��������

	CMD_GR_Private_Info					m_kPrivateInfo;
	YSAllocationID						m_allocationID;

	CRedisInstance						mRedis;

	//�ں˽ӿ�
protected:
	PrivateTableInfo*					m_pTableInfo;				//
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
	PriaveteGame();
	//��������
	virtual ~PriaveteGame(void);

	bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	bool SendTableData(ITableFrame*	pITableFrame, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	void CreatePrivateCost(PrivateTableInfo* pTableInfo);

	bool joinPrivateRoom(IServerUserItem * pIServerUserItem,ITableFrame * pICurrTableFrame);

	bool OnEventCreatePrivate(WORD wRequestID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize,std::string kChannel);

	void sendPrivateRoomInfo(IServerUserItem * pIServerUserItem,PrivateTableInfo* pTableInfo);

	PrivateTableInfo* getTableInfoByRoomID(DWORD dwRoomID);

	PrivateTableInfo* getTableInfoByCreaterID(DWORD dwUserID);

	PrivateTableInfo* getTableInfoByTableID(WORD wTableID);

	PrivateTableInfo* getTableInfoByTableFrame(ITableFrame* pTableFrame);

	void DismissRoom(PrivateTableInfo* pTableInfo);

	void ClearRoom(PrivateTableInfo* pTableInfo);

	void SendGamePrivateEndInfo(PrivateTableInfo* pTableInfo);		// ������Ϣ����ÿ����Ϸ�ڲ�

	void gameEnd(PrivateTableInfo* pTableInfo);

private:
	void CreaterPlayerPrivateCost(PrivateTableInfo* pTableInfo);				//������֧������;
	void EveryOnePrivateCost(PrivateTableInfo* pTableInfo, bool bDivideCost);	//ÿ����Ҷ�Ҫ֧��,bDivideCost=trueʱ��ƽ�ַ���֧��;

	//�����ӿ�
public:
 	//�ͷŶ���
 	virtual VOID Release(){ delete this; }
 	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ƽӿ�
public:
	//����֪ͨ
	virtual void OnStartService();

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
	//˽�˳���Ϣ
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//����˽�˳�
	bool OnTCPNetworkSubCreatePrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//����˽�˳�
	bool OnTCPNetworkSubJoinPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	//��ɢ
	bool OnTCPNetworkSubDismissPrivate(VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);
	
	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind);

	//˽�˳��û��¼�
	virtual bool AddPrivateAction(ITableFrame* pTbableFrame,DWORD dwChairID, BYTE	bActionIdex);
	//ͳ��˽�˳���Ҿ���
	virtual bool addPrivatePlayCout(ITableFrame* pTbableFrame, WORD wCout);

	//�¼��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnEventEnterMatch(DWORD dwSocketID, VOID* pData, DWORD dwUserIP, bool bIsMobile);
	//�û�����
	virtual bool OnEventUserJoinMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, DWORD dwSocketID);
	//�û�����
	virtual bool OnEventUserQuitMatch(IServerUserItem * pIServerUserItem, BYTE cbReason, WORD *pBestRank = NULL, DWORD dwContextID = INVALID_WORD);

	 //���ܺ���
public:
	 //��Ϸ��ʼ
	 virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	 //��Ϸ����
	 virtual bool OnEventGameEnd(ITableFrame *pITableFrame,WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	 //�û��¼�
public:
	 //�û�����
	 virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem);
	 //�û�����
	 virtual bool OnActionUserSitDown(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�����
	 virtual bool OnActionUserStandUp(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	 //�û�ͬ��
	 virtual bool OnActionUserOnReady(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);
	 //�жϷ����Ƿ�
	 virtual bool OnActionUserFangKaCheck(WORD wTableID, WORD wChairID, IServerUserItem * pIServerUserItem, SCORE lUserInsure);
	 //��ҷ���
	 virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame, IServerUserItem * pIServerUserItem);

public:
	 //�û�����
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem);

	virtual bool WriteTableScore(ITableFrame* pITableFrame,tagScoreInfo ScoreInfoArray[], WORD wScoreCount,DataStream& kData);	
	//��������;
protected:
	//��redis�м��봴��˽�˳�ʱ��KEYֵ(prm%d);
	bool	AddPRMRedisKey(DWORD dwRoomNum);
	//ɾ��redis�д���˽�˳�ʱ��KEYֵ(prm%d);
	bool	DelPRMRedisKey(DWORD dwRoomNum);
	//ɾ��redis�е�KEY;
	bool	DelRedisKey(const char* szKey);

	//�������Ϣ�������roomid�ֶ�ֵ;
	bool	AddRoomIDToIDRedisKey(DWORD dwUserID, DWORD dwRoomNum);
	//ɾ�������Ϣ�е�roomid�ֶ�ֵ;
	bool	DelRoomIDFromIDRedisKey(DWORD dwUserID);
	bool	DelRoomIDFromIDRedisKey(PrivateTableInfo* pTableInfo);

	//����privateserverid��rediskey�е�wCurTable��ֵ;
	bool	IncrCurTableOfPrivateserveridKey(int nIncValue=1);
	bool	DecrCurTableOfPrivateserveridKey();
};

#endif