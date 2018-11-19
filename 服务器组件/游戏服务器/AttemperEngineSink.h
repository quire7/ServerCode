#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrame.h"
#include "InitParameter.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "DistributeManager.h"
#include "SensitiveWordsFilter.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define CLIENT_KIND_FALSH			1									//��ҳ����
#define CLIENT_KIND_MOBILE			2									//�ֻ�����
#define CLIENT_KIND_COMPUTER		3									//��������

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾

	//�û�����
	BYTE							cbClientKind;						//��������
	IServerUserItem *				pIServerUserItem;					//�û��ӿ�
};

//ϵͳ��Ϣ
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //����ʱ��
	DBR_GR_SystemMessage            SystemMessage;                     //ϵͳ��Ϣ
};

//����˵��
typedef CYSArray<CTableFrame *>		CTableFrameArray;					//��������
typedef CYSArray<tagSystemMessage *>   CSystemMessageArray;             //ϵͳ��Ϣ
typedef CYSArray<tagTaskParameter *>   CTaskParameterArray;             //��������
typedef CMap<DWORD,DWORD,DWORD,DWORD &>  CKickUserItemMap;              //����ӳ�� 
typedef CMap<WORD,WORD,tagTaskParameter *,tagTaskParameter *>  CTaskParameterMap;

//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink, public IMainServiceFrame,public IServerUserItemSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־
	bool							m_bNeekCorrespond;					//Э����־

	//���Ʊ���
protected:
	DWORD							m_dwIntervalTime;					//���ʱ��
	DWORD							m_dwLastDisposeTime;				//����ʱ��

	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ
	tagBindParameter *				m_pAndroidParameter;				//����Ϣ

	//������Ϣ
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameMatchOption *			m_pGameMatchOption;					//��������	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��������
protected:
	CMD_GR_ConfigProperty			m_DataConfigProperty;				//��������

	//��Ա����
protected:
	WORD							m_wMemberCount;						//��Ա��Ŀ
	tagMemberParameter				m_MemberParameter[10];				//��Ա����

	//�������
protected:
	WORD							m_wTaskCount;						//������Ŀ
	tagTaskParameter				m_TaskParameter[TASK_MAX_COUNT];	//�������

	//�������
protected:
	CTableFrameArray				m_TableFrameArray;					//��������
	CServerListManager				m_ServerListManager;				//�б����
	CServerUserManager				m_ServerUserManager;				//�û�����
	CAndroidUserManager				m_AndroidUserManager;				//��������
	CUserTaskManager				m_UserTaskManager;					//�������
	CGamePropertyManager			m_GamePropertyManager;				//���߹���
	CDistributeManager              m_DistributeManage;                 //�������
	CKickUserItemMap                m_KickUserItemMap;                  //���˹���
	CSystemMessageArray             m_SystemMessageActive;              //ϵͳ��Ϣ
	CSystemMessageArray				m_SystemMessageBuffer;				//��Ϣ����	
	CSensitiveWordsFilter			m_WordsFilter;						//���ֹ���

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IAttemperEngine *				m_pIAttemperEngine;					//��������
	ITCPSocketService *				m_pITCPSocketService;				//�������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	IGameServiceManager *			m_pIGameServiceManager;				//�������

	//��������
public:	
	IMatchServiceManager			* m_pIMatchServiceManager;			//��������

	//��������
public:
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��������
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//��������
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��

	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��Ϣ�ӿ�
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid);

	//����ӿ�
public:
	//��������
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//Ⱥ������
	virtual bool SendDataToTable(WORD wTableID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����û���Ϣ��ָ���û�
	virtual bool SendUserInfo(IServerUserItem * pIServerUserItem, WORD wBindIndex);

	//���ܽӿ�
public:
	//�����û�
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//ɾ���û�
	virtual bool DeleteDistribute(IServerUserItem * pIServerUserItem);
	//���дʹ���
	virtual void SensitiveWordFilter(LPCTSTR pMsg, LPTSTR pszFiltered, int nMaxLen);

	//�û��ӿ�
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem, BYTE cbReason);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight, BYTE cbRightKind){ return true; }

	//�����¼�
protected:
	//��¼�ɹ�
	bool OnDBLogonSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBLogonFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBGameAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBGamePropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�û�����
	bool OnDBUserPropertyInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBUserInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���гɹ�
	bool OnDBUserInsureSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBUserInsureFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�û���Ϣ
	bool OnDBUserInsureUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����б�
	bool OnDBUserTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBUserTaskListEnd(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBUserTaskInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������
	bool OnDBUserTaskResult(DWORD dwContextID, VOID * pData, WORD wDataSize);	
	////���߳ɹ�
	//bool OnDBPropertySuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	////����ʧ��
	//bool OnDBPropertyFailure(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������
	bool OnDBPurchaseResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ����
	bool OnDBExChangeResult(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnDBSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������д�
	bool OnDBSensitiveWords(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBSystemMessageFinish(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ʼ��������
	bool onDBSendEMailSuccess(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ڻ���������ע��
	bool OnDBReRegServerInRedis(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����ƿ�
	bool OnDBGameCardLibrary(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�ƿ�����;
	bool OnDBGameCardLibraryCfg(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���Ӵ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����¼�
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	bool OnTCPSocketMainAndroidService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��վ����
	bool OnTCPSocketMainWebService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����¼�
protected:
	//�û�����
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��¼����
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ϸ����
	bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ܴ���
	bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���д���
	bool OnTCPNetworkMainInsure(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������
	bool OnTCPNetworkMainTask(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ�����
	bool OnTCPNetworkMainExchange(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������
	bool OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�����¼�
protected:
	//I D ��¼
	bool OnTCPNetworkSubLogonUserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�û�����
protected:
	//�û�����
	bool OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û��Թ�
	bool OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�˽��
	bool OnTCPNetworkSubWisperChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubUserExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkSubWisperExpression(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	////�������
	//bool OnTCPNetworkSubPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ʹ�õ���
	//bool OnTCPNetwordSubSendTrumpet(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û�
	bool OnTCPNetworkSubUserInviteReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ܾ�����
	bool OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳�����
	bool OnTCPNetworkSubMemberKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������λ��
	bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
protected:	
	//��ѯ����
	bool OnTCPNetworkSubQueryInsureInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ͨ����
	bool OnTCPNetworkSubEnableInsureRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������
	bool OnTCPNetworkSubSaveScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ȡ������
	bool OnTCPNetworkSubTakeScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ת������
	bool OnTCPNetworkSubTransferScoreRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ�û�����
	bool OnTCPNetworkSubQueryUserInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
protected:
	//��ȡ����
	bool OnTCPNetworkSubTakeTaskRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ����
	bool OnTCPNetworkSubTaskRewardRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubLoadTaskInfoRequest(VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//�һ�����
protected:
	//��ѯ����
	bool OnTCPNetworkSubQueryExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ���Ϸ��
	bool OnTCPNetworkSubExchangeScore(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��������
protected:
	//��ѯ����
	bool OnTCPNetworkSubQueryOption(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubOptionServer(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳��û�
	bool OnTCPNetworkSubManagerKickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubLimitUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳������û�
	bool OnTCPNetworkSubKickAllUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ϣ
	bool OnTCPNetworkSubSendMessage(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ��Ϸ
	bool OnTCPNetworkSubDismissGame(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������
	bool OnTCPNetworkSubWarningUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�ڲ��¼�
protected:
	//�û���¼
	VOID OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine);
	//�û��ǳ�
	VOID OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason);

	//ִ�й���
protected:
	//�����û�
	bool PerformDistribute();	
	//������Ϸ��
	bool PerformUnlockScore(DWORD dwUserID, DWORD dwInoutIndex, DWORD dwLeaveReason);
	//�ƽ�����
	bool PerformTaskProgress(IServerUserItem * pIServerUserItem, tagUserTaskEntry * pUserTaskEntry, tagVariationInfo &VariationInfo, WORD wTaskType);
	//�л�����
	bool SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], DWORD dwSocketID, BYTE cbDeviceType = DEVICE_TYPE_PC);

	//���ͺ���
protected:
	//�û���Ϣ
	bool SendUserInfoPacket(IServerUserItem * pIServerUserItem,DWORD dwSocketID,bool bSendAndroidFalg=false);

	//��������
protected:
	////��ǰ�¼�
	//bool OnEventPropertyBuyPrep(WORD cbRequestArea,WORD wPropertyIndex,IServerUserItem *pISourceUserItem,IServerUserItem *pTargetUserItem);
	////������Ϣ
	//bool SendPropertyMessage(DWORD dwSourceID,DWORD dwTargerID,WORD wPropertyIndex,WORD wPropertyCount);
	////����ЧӦ
	//bool SendPropertyEffect(IServerUserItem * pIServerUserItem);

	//��������
protected:
	//��¼ʧ��
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID);
	//����ʧ��
	bool SendInsureFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,BYTE cbActivityGame);
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);
	//����ʧ��
	bool SendPropertyFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode,WORD wRequestArea);

	//��������
public:
	//���û�
	IServerUserItem * GetBindUserItem(WORD wBindIndex);
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//��������
	WORD GetPropertyType(WORD wPropertyIndex);

	//��������
protected:
	//���û���
	bool InitAndroidUser();
	//��������
	bool InitTableFrameArray();
	//���ñ���
	bool InitMatchServiceManager();
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//������¼
	bool UserImmediately(IServerUserItem * pIServerUserItem);
	////�������
	//int OnPropertyBuy(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����ϵͳ��Ϣ
	bool SendSystemMessage(CMD_GR_SendMessage * pSendMessage, WORD wDataSize);

	//��������
protected:
	//�Ƴ���Ϣ
	void RemoveSystemMessage();
	//�ڻ���ע���������Ϣ
	void RegServerInfoInRedis();
	//��ʾ������Ϣ;
	void ShowBurnInfo(void);
};

//////////////////////////////////////////////////////////////////////////////////

#endif