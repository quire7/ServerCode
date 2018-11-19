#ifndef IMMEDIATE_MATCH_HEAD_FILE
#define IMMEDIATE_MATCH_HEAD_FILE

#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "MatchServiceHead.h"
#include "MatchServiceManager.h"
#include "ImmediateGroup.h"

///////////////////////////////////////////////////////////////////////////////////////////

//ʱ�Ӷ���
#define IDI_CHECK_START_MATCH		(IDI_MATCH_MODULE_START+1)					//��ʼʱ��
#define IDI_CHECK_END_MATCH			(IDI_MATCH_MODULE_START+2)					//����ʱ��

//�Ƴ�����
#define REMOVE_BY_ENDMATCH			250											//��������

///////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//��ʱ������
struct tagMatchTimerItem
{
	//DWORD								dwTimerID;						//��ʱ�� ID
	DWORD								dwRealID;						//ʵ��ID
	DWORD								dwRepeatTimes;					//�ظ�����
	WPARAM								wBindParam;						//�󶨲���
};

//��������
struct tagMatchReward
{
	DWORD								dwGold;							//��ҽ���
	DWORD								dwMedal;						//���ƽ���
	DWORD								dwExperience;					//���齱��
};

///////////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���

//���鶨��
typedef CYSArray<BYTE> CMatchLoopTimerArray;
typedef CYSArray<CImmediateGroup *> CMatchGroupArray;
typedef CYSArray<tagMatchReward*> CMatchRewardArray;
typedef CYSArray<tagMatchTimerItem *> CTimerItemPtr;
typedef CYSArray<CTableFrameHook *> CMatchTableArray;
typedef CYSArray<IServerUserItem *> CMatchUserItemArray;

///////////////////////////////////////////////////////////////////////////////////////////

//��ʱ��
class CImmediateMatch : public IGameMatchItem,public IImmediateGroupSink,IServerUserItemSink
{
	//�洢����
protected:
	CMatchUserItemArray					m_OnMatchUserItem;				//�����û�
	CMatchLoopTimerArray				m_LoopTimer;					//ѭ����ʱ��
	CMatchTableIDArray					m_aryFreeTableID;				//��������ID

	//��������
protected:	
	CMatchGroupArray					m_MatchGroup;					//���еı�����
	CMatchGroupArray					m_OverMatchGroup;				//�����ı�����
	CImmediateGroup *					m_pCurMatchGroup;				//��ǰ������

	//��������
protected:
	CMD_GR_MatchDesc					m_MatchDesc;					//��Ϣ����

	//��������
protected:
	tagGameMatchOption *				m_pMatchOption;					//��������	
	tagGameServiceOption *				m_pGameServiceOption;			//��������
	tagGameServiceAttrib *				m_pGameServiceAttrib;			//��������
	tagGameMatchOption::unGameMatchType::tagImmediateMatch* m_pImmediate;

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
	CImmediateMatch();
	//��������
	virtual ~CImmediateMatch(void);

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
	//�����¼�
	virtual bool OnEventSocketMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID);	

	//�û��¼�
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

	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind);
	
	//�����¼�
public:
	//��ʼ����
	virtual bool OnEventMatchStart(CImmediateGroup *pMatch);
	//��������
	virtual bool OnEventMatchOver(CImmediateGroup *pMatch);
		 
	//��ʱ���ӿ�
public:
	//ɾ����ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch);
	//���ö�ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch);

	//���ͺ���
public:
	//��������
	virtual bool SendMatchInfo(IServerUserItem * pIServerUserItem);
	//������Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType);
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);	

	//���ܺ���
public:
	//��ȡ�յ�����
	virtual void GetNullTable(CImmediateGroup* pImmediateGroup, CTableFrameMananerArray& aryTableFrameInfo, WORD wTableCount);
	//��������
	virtual void RecoverTable(CTableFrameMananerArray& aryTableFrameInfo);
	//��ȡ������
	virtual IAndroidUserItem * GetFreeAndroidUserItem();	
	
	//�ڲ�����
private:
	//ɾ���û�
	bool DeleteUserItem(DWORD dwUserIndex);
	//�Ƴ������û�
	bool RemoveMatchUserItem(IServerUserItem *pIServerUserItem);
	//д�뽱��
	bool WriteUserAward(IServerUserItem *pIServerUserItem,tagMatchRankInfo * pMatchRankInfo);

	//�û�����
public:
	//
	virtual bool OnEventReqStandUP(IServerUserItem * pIServerUserItem) { return true; };
};

#endif