#ifndef IMMEDIATE_GROUP_HEAD_FILE
#define IMMEDIATE_GROUP_HEAD_FILE
#pragma once

//�����ļ�
#include "TableFrameHook.h"
#include "MatchServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���
#define IDI_DELETE_OVER_MATCH		(IDI_MATCH_MODULE_START+1)					//ɾ�������ı�����
#define IDI_CHECK_OVER_MATCH		(IDI_MATCH_MODULE_START+2)					//���һ������ 
#define IDI_GROUP_TIMER_START		(IDI_MATCH_MODULE_START+10)					//�������ڵĶ�ʱ��

//ʱ�Ӷ���
#define IDI_CHECK_MATCH_GROUP		(IDI_GROUP_TIMER_START+1)					//��Ѳ���п��������� 
#define IDI_SWITCH_WAIT_TIME		(IDI_GROUP_TIMER_START+2)					//�����ȴ�ʱ�� 
#define IDI_LASTMATCHRESULT			(IDI_GROUP_TIMER_START+3)					//��������

//ʱ�Ӷ���
#define IDI_CONTINUE_GAME			1											//������Ϸ
#define IDI_LEAVE_TABLE				2											//�뿪����
#define IDI_CHECK_TABLE_START		3											//�����������Ƿ�ʼ(����п���׼���󲻿�ʼ���)
#define IDI_START_TABLE_GAME		4

///////////////////////////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//����״̬
enum enMatchStatus
{
	MatchStatus_Null=0,										//��״̬
	MatchStatus_Signup,										//������
	MatchStatus_Wait,										//�ȴ�����
	MatchStatus_FirstRound									//���ֱ���
};

//������ʱ��
struct tagMatchTimer
{
	DWORD					dwTimerID;						//��ʱ��ID
	int						iElapse;						//ִ�м����	
	WPARAM					wParam;							//���Ӳ���
	LPARAM					lParam;							//���Ӳ���
};

//��������
struct tagMatchScore
{
	DWORD					dwUserID;						//�û�I D
	SCORE					lScore;							//�û��ɼ�	
	IServerUserItem *		pUserItem;						//�û��ӿ�
};

//��������
struct tagMatchRanking
{
	DWORD					dwUserID;						//�û�I D
	SCORE					lScore;							//�û�����
	LONG					lExpand;						//��չֵ
	IServerUserItem *		pUserItem;						//�û��ӿ�
};

//����������
struct tagTableFrameInfo 
{
	ITableFrame		*		pTableFrame;					//���ӽӿ�
	WORD					wTableID;						//���ӱ��
	DWORD					dwBaseScore;					//��������	
	BYTE					cbPlayCount;					//��Ϸ����
	bool					bRoundTableFinish;				//��ɱ�ʶ
	bool					bSwtichTableFinish;				//�������
	bool					bWaitForNextInnings;			//�ȴ��¾�
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//���Ͷ���

//���鶨��
typedef CYSArray<WORD>				CMatchTableIDArray;
typedef CYSArray<tagMatchTimer *>	CUserMatchTimerArray;
typedef CYSArray<tagMatchScore *>	CUserMatchScoreArray;
typedef CYSArray<IServerUserItem *>	CMatchUserItemArray;
typedef CYSArray<tagTableFrameInfo *>	CTableFrameMananerArray;

//�ֵ䶨��
typedef CMap<DWORD,DWORD,IServerUserItem *, IServerUserItem *>		CUserItemMap;
typedef CMap<IServerUserItem *, IServerUserItem *, DWORD, DWORD>	CUserSeatMap;

///////////////////////////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���

//��������
class CImmediateGroup;

//����ص�
interface IImmediateGroupSink
{
	//�����¼�
public:
	//��ʼ����
	virtual bool OnEventMatchStart(CImmediateGroup *pMatch)=NULL;
	//��������
	virtual bool OnEventMatchOver(CImmediateGroup *pMatch)=NULL;

	//��ʱ���ӿ�
public:
	//ɾ����ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID,CImmediateGroup *pMatch)=NULL;
	//���ö�ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter,CImmediateGroup *pMatch)=NULL;	

	//���ͺ���
public:
	//������Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	

	//���ܺ���
public:
	//��ȡ�յ�����
	virtual void GetNullTable(CImmediateGroup* pImmediateGroup, CTableFrameMananerArray& aryTableFrameInfo, WORD wTableCount) = NULL;
	//��������
	virtual void RecoverTable(CTableFrameMananerArray& aryTableFrameInfo) = NULL;
	//��ȡ������
	virtual IAndroidUserItem * GetFreeAndroidUserItem()=NULL;	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
//��������
class CImmediateGroup : public IMatchEventSink
{
	friend class CImmediateMatch;

	//״̬��Ϣ
protected:
	DWORD							m_dwCurBase;					//��ǰ����
	DWORD							m_dwMatchNO;					//�������
	enMatchStatus					m_enMatchStatus;				//�����׶�
	BYTE							m_LoopTimer;					//ѭ����ʱ��
	WORD							m_wAndroidUserCount;			//�����˸���
	SYSTEMTIME						m_MatchStartTime;				//������ʼ

	//ָ�����
protected:
	tagGameMatchOption *			m_pMatchOption;					//��������
	IImmediateGroupSink *			m_pMatchSink;					//����ص�
	tagGameMatchOption::unGameMatchType::tagImmediateMatch* m_pMatchInfo;

	//�洢����
protected:
	CUserItemMap					m_OnMatchUserMap;				//�����û�
	CMatchUserItemArray				m_FreeUserArray;				//�����û�
	CTableFrameMananerArray			m_MatchTableArray;				//Ԥ����������
	CUserMatchTimerArray			m_MatchTimerArray;				//����ר�ö�ʱ��	

	//��̬����
protected:
	static WORD						m_wChairCount;					//������Ŀ

	//��������
public:
	//���캯��
	CImmediateGroup(DWORD dwMatchNO, tagGameMatchOption * pGameMatchOption, IImmediateGroupSink *pIImmediateGroupSink);
	//��������
	virtual ~CImmediateGroup();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release(){}
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ܺ���
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart(ITableFrame *pITableFrame, WORD wChairCount);
	//��Ϸ����
	virtual bool OnEventGameEnd(ITableFrame *pITableFrame, WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//������Ϣ
	virtual bool SendData(IServerUserItem *pUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����û�����
	virtual bool SendUserScore(IServerUserItem * pIUserItem);
	//������Ϣ
	virtual bool SendUserMessage(IServerUserItem * pIUserItem, LPCTSTR pStrMessage);
	//����״̬
	virtual bool SendUserStatus(IServerUserItem * pIUserItem);

	//���ͱ�����Ϣ
	virtual void SendMatchInfo(IServerUserItem *pUserItem);

	//�û���̭
	virtual bool OnUserOut(IServerUserItem *pUserItem, LPCTSTR szReason, WORD wLen, WORD wRank);

	//�û��¼�
public:
	//��ҷ���
	virtual bool OnEventUserReturnMatch(ITableFrame *pITableFrame, IServerUserItem * pIServerUserItem);

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
	//�û�����
	bool OnUserSignUpMatch(IServerUserItem *pUserItem);
	//�û�����
	bool OnUserQuitMatch(IServerUserItem *pUserItem);

	//��������
protected:
	//��������
	void MatchOver();
	//Ч�������ʼ
	void EfficacyStartMatch();
	//����������
	bool CheckMatchUser();
	//�������
	WORD SortMapUser(tagMatchScore score[]);
	//��ʱ����Ϣ
	bool OnTimeMessage(DWORD dwTimerID, WPARAM dwBindParameter);
	//�������
	bool UserSitDown(IServerUserItem *pUserItem);
	//�������
	bool UserStandUp(IServerUserItem *pUserItem);
	//��ȡ����
	WORD GetUserRank(IServerUserItem *pUserItem, ITableFrame *pITableFrame = NULL);
	//������ʾ
	VOID SendWaitTip(ITableFrame *pTableFrame, WORD wChairID = INVALID_CHAIR);
	//��ȡ����ר�ýӿ�
	tagTableFrameInfo* GetTableInterface(ITableFrame* ITable);
	//����һ����Ϣ
	void SendGroupBox(LPCTSTR pStrMessage);
	//���������û��뿪
	void AllUserExitTable(ITableFrame *pITableFrame);
	//����������Ϸ
	void ContinueGame(ITableFrame *pITableFrame);
	//��ȡ���������û�
	WORD GetPrecedeSitUser(DWORD dwCurTime);
	//ʵ�ָú����ɱ�֤Free��Ψһ
	void InsertFreeGroup(IServerUserItem *pUserItem);
	//���ͱ�����Ϣ
	void SendTableUserMatchInfo(ITableFrame *pITableFrame, WORD wChairID);
	//����������Ϸ����
	void FirstRoundRuleInningsGameEnd(ITableFrame *pITableFrame);

	//��Ǻ���
protected:
	//�����
	bool CheckRoundTableFinish();
	//�����
	bool CheckSwitchTableFinish();
	//���ñ��
	void SetRoundTableFinish(ITableFrame *pITableFrame);
	//���ñ��
	void SetSwitchTableFinish(ITableFrame *pITableFrame);

	//��ʱ������
protected:
	//����ʱ��
	void CaptureMatchTimer();
	//ɱ����ʱ��
	void AllKillMatchTimer();
	//ɱ����ʱ��
	bool KillMatchTimer(INT_PTR dwIndexID);
	//ɱ����ʱ��
	bool KillMatchTimer(DWORD dwTimerID, WPARAM wParam);
	//Ͷ�ݶ�ʱ��
	void PostMatchTimer(DWORD dwTimerID, int iElapse, WPARAM wParam = 0, LPARAM lParam = 0);
	//ɱ����ʱ��
	void KillMatchGameTimer(DWORD dwTimerID);
	//�趨��ʱ��
	void SetMatchGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);

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