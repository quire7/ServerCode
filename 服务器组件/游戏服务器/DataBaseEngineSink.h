#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "InitParameter.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
class CDataBaseEngineSink : public IDataBaseEngineSink, public IGameDataBaseEngine
{
	//��Ԫ����
	friend class CServiceUnits;

	//��������
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//��¼ʧ��
	DBO_GR_LogonSuccess				m_LogonSuccess;						//��¼�ɹ�

	CRedisInstance					m_RedisInstance;					//�������

	CYSArray<WORD>					m_wArrDailyTaskID;					//�ճ���������

	//�˺����ݿ�
protected:
	CDataBaseHelper					m_AccountsDBModule;					//�˺����ݿ�

	//��Ϸ�����ݿ�
protected:
	CDataBaseHelper					m_TreasureDBModule;					//��Ϸ�����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//���ñ���
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//�������
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//����ӿ�
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//�����¼�

	//��ѯ�ӿ�
protected:
	IGameDataBaseEngineSink *		m_pIGameDataBaseEngineSink;			//���ݽӿ�

	//�������
public:
	IDBCorrespondManager *          m_pIDBCorrespondManager;            //����Э��

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ò���
public:
	//�Զ�����
	virtual VOID * GetCustomRule() { return m_pGameServiceOption->cbCustomRule; };
	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//��ȡ����
public:
	//��ȡ����
	virtual VOID * GetDataBase(REFGUID Guid, DWORD dwQueryVer);
	//��ȡ����
	virtual VOID * GetDataBaseEngine(REFGUID Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//Ͷ�ݽ��
	virtual bool PostGameDataBaseResult(WORD wRequestID, VOID * pData, WORD wDataSize);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��¼����
protected:
	//I D ��¼
	bool OnRequestLogonUserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID,BOOL bMatch=false);
	//�û�����
	bool OnRequestLogonReLine(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//ϵͳ����
protected:
	//��Ϸд��
	bool OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�뿪����
	bool OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��Ϸ��¼
	bool OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ػ���
	bool OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//���ػ���
	bool OnRequestLoadAndroidParameter(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//���ص���
	bool OnRequestLoadGameProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�����û�����
	bool OnRequestLoadUserProperty(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestPropertyRequest(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ϵͳ��Ϣ
	bool OnRequestLoadSystemMessage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�˳�����
	bool OnRequestMatchUnSignup(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//������ʼ
	bool OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//������̭
	bool OnRequestMatchEliminate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�����ʼ�
	bool OnRequestSendEMail(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//ˢ����������
	bool OnRequestRefreshOnlineUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//���з���
protected:
	//������Ϸ��
	bool OnRequestUserSaveScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ȡ��Ϸ��
	bool OnRequestUserTakeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��ѯ����
	bool OnRequestQueryInsureInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�������
protected:
	//��ȡ����
	bool OnRequestTaskReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//��������
	bool OnRequestLoadTaskList(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);	
	//��ѯ����
	bool OnRequestTaskQueryInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//�����ѯ
	bool OnQueryTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess);
	//��ʼ���������
	bool InitUserTask(DWORD dwContextID, DBO_GR_LogonSuccess m_LogonSuccess);
	//�������
	bool OnRequestTaskProgress(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//�һ�����
protected:
	//�һ���Ϸ��
	bool OnRequestExchangeScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//��������
private:
	//��������
	bool UnLockAndroidUser();
	//��¼���
	VOID OnLogonDisposeResult(DWORD dwContextID, LPCTSTR lpsPassword, ValueMap& vMap);
	//��¼���
	VOID OnLogonDisposeResult(DWORD dwContextID, SQLRecordset& rs, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbDeviceType = DEVICE_TYPE_PC);
	//���н��
	VOID OnInsureDisposeResult(DWORD dwContextID, SQLRecordset& rs, DWORD dwErrorCode, SCORE lFrozenedScore, LPCTSTR pszErrorString, bool bMobileClient, BYTE cbActivityGame = FALSE);
	//���»�������
	void UpdateCachedData(DWORD dwUserID, tagVariationInfo& VariationInfo, BYTE cbCardType, LPCTSTR pszCardData);
	//��ȡ�������
	void GetUserTaskInfo(DWORD dwContextID, WORD wKindID, DBO_GR_LogonSuccess &m_LogonSuccess);
	//��ҽ��뷿��
	void UserEnterGameRoom();
	//�ӻ����л�ȡ��ҵ���
	void GetUserPropFromRedis(DWORD dwContextID, DWORD dwUserID);
	//�޸Ļ�������ҵ�������
	void ModifyUserPropNumFromRedis(DWORD dwDBID, DWORD dwUserID, WORD wPropID, int nChangeValue);

	//˽�˳�����
protected:
	bool OnRequestPrivateInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	bool OnRequestCreatePrivate(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	bool OnRequestCreatePrivateCost(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);

	//˽�˷���Ϸ��¼;
	bool OnRequestPrivateGameRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//˽�˷�������Ϸ��¼;
	bool OnRequestSinglePrivateGameRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
	//˽�˷�����;
	bool OnRequestPrivateGameEnd(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID);
};

//////////////////////////////////////////////////////////////////////////////////

#endif