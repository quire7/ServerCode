#ifndef DATA_BASE_ENGINE_HEAD_FILE
#define DATA_BASE_ENGINE_HEAD_FILE

#pragma once

//���ͷ�ļ�
#include "KernelEngineHead.h"
#include "QueueService.h"
#include "TraceService.h"
#include "QueueServiceEvent.h"

#include "../../���������/�������/dep/NDShareBase/common/mysql/mysql.h"
#include <vector>
using namespace std;
#include <queue>
using std::queue;

/* catch error record set result. */
#define ErrorSQLRS (0xFFFFFFFF)

#define MAX_16_BUF	(16777216)			//16M;
//////////////////////////////////////////////////////////////////////////

//DataBase ������
class CDataBaseError : public IDataBaseException
{
	//��������
protected:
	enSQLException					m_enErrorType;						//�������
	CString							m_strErrorDescribe;					//������Ϣ

	//��������
public:
	//���캯��
	CDataBaseError();
	//��������
	virtual ~CDataBaseError();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release(){	return ;}
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���ܽӿ�
public:
	//��������
	virtual enSQLException GetExceptionType()
	{
		return m_enErrorType;
	}
	//��������
	virtual LPCTSTR GetExceptionDescribe()
	{
		return m_strErrorDescribe;
	}

	//���ܺ���
public:
	//���ô���
	void SetErrorInfo(enSQLException enErrorType, LPCTSTR pszDescribe);
};


//////////////////////////////////////////////////////////////////////////

//���ݿ����
class CDataBase : public IDataBase
{
	//��Ϣ����
protected:
	CDataBaseError					m_dbError;							//�������
	CString							m_strConnect;						//�����ַ���
	CString							m_strErrorDescribe;					//������Ϣ

	std::string						m_strHost;
	WORD							m_wPort;
	std::string						m_strUser;
	std::string						m_strPassword;
	std::string						m_strDatabaseName;

	char							m_szBuf[100 * 1000];
	stringstream					m_strSQLBuffer;

	queue<char*>					m_bufQueue;							//16M�ڴ�����;

	//״̬����
protected:
	DWORD							m_dwConnectCount;					//���Դ���
	DWORD							m_dwConnectErrorTime;				//����ʱ��
	const DWORD						m_dwResumeConnectCount;				//�ָ�����
	const DWORD						m_dwResumeConnectTime;				//�ָ�ʱ��

	//�ں˱���
protected:
	MYSQL							m_dMYSQL;							//Mysql�Ļ�����Ϣ��
	BOOL							m_bConnected;
	BOOL							m_bNeedReconnect;
	DWORD							m_dwLastCheckTimeMS;

	//��������
public:
	//���캯��
	CDataBase();
	//��������
	virtual ~CDataBase();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//������
	virtual VOID Open();
	//�ر�����
	virtual VOID Close();
	//�Ƿ��Ծ
	virtual bool IsActive();
	//������¼��
	virtual VOID CleanupResults();
	//������Ϣ
	virtual bool SetDBInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);
	//������Ϣ
	virtual bool SetDBInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);

	//״̬�ӿ�
public:
	//�Ƿ��
	virtual BOOL IsOpened();
	//�������
	virtual BOOL CheckAndReconnect(DWORD dwCheckInterval = 1000);

	//����ӿ�
public:
	//��ʼ����
	virtual BOOL BeginTransaction();
	//ֹͣ����
	virtual BOOL CommitTransaction();
	//�ع�����
	virtual BOOL RollbackTransaction();

public:
	//�����Զ��ύģʽ
	virtual BOOL SetAutoCommit(BOOL bAutoCommit);
	//ѡ�����ݿ�
	virtual BOOL SelectDatabase(const std::string& strDatabaseName);

	//�������ӿ�
public:
	//�������
	virtual void AddParameter(bool bValue);
	//�������
	virtual void AddParameter(BYTE cbValue);
	//�������
	virtual void AddParameter(WORD wValue);
	//�������
	virtual void AddParameter(int iValue);
	//�������
	virtual void AddParameter(DWORD dwValue);
	//�������
	virtual void AddParameter(LONG lValue);
	//�������
	virtual void AddParameter(LONGLONG llValue);
	//�������
	virtual void AddParameter(SYSTEMTIME& st);
	//�������
	virtual void AddParameter(LPCTSTR lpsValue);
	//�������(nStrSize:�������Ĵ�С(�������������ʱ��Ҫ����mysql_real_escape_string���������ݽ���ת��);
	virtual void AddBinaryParameter(LPCTSTR lpsValue, int nStrSize);
	//�����������
	virtual void AddParameterOutput(const char* strOutput);
	//ɾ������
	virtual void ClearParameters();

private:
	//�������
	void AddParameter(const char* fmt, ...);

	//ִ�нӿ�
public:
	//ִ�����
	virtual BOOL ExecuteProcess(const std::string &strSQL);
	//ִ�����
	virtual BOOL ExecuteSQL(const std::string& strSQL);
	//��ȡ����������ID
	virtual __int64 GetLastInsertID();

	//ִ��û�н���������
	BOOL ExecuteSQLNonResult(const char* fmt, ...);

public:
	operator MYSQL*() {	return &m_dMYSQL; }

	MYSQL* GetMYSQL() { return &m_dMYSQL; }

	//�ڲ�����
private:

	VOID OutputError(const std::string& strSQL, bool bReconnect = false);

	void	ReleaseBufQueue();
	char*	getBuf();
	void	releaseBuf(char* pBuf);
};

//////////////////////////////////////////////////////////////////////////

//���ݿ������
class CDataBaseEngine : public IDataBaseEngine, public IQueueServiceSink
{
	//�ں˱���
protected:
	bool							m_bService;							//���б�־
	CQueueServiceEvent				m_QueueServiceEvent;				//���ж���
	//IDataBaseEngineSink				* m_pIDataBaseEngineSink;			//֪ͨ����

	vector<IDataBaseEngineSink *>   m_pIDataBaseEngineSinks;			//֪ͨ����
	//��������
public:
	//���캯��
	CDataBaseEngine(void);
	//��������
	virtual ~CDataBaseEngine(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release()
	{
		if (IsValid()) delete this;
		return /*true*/;
	}
	//�Ƿ���Ч
	virtual bool IsValid()
	{
		return AfxIsValidAddress(this, sizeof(CDataBaseEngine)) ? true : false;
	}
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();

public:
	//���渺��
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//���ýӿ�
public:
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//����ģ��
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);

	//���нӿ�
public:
	//���нӿ�
	virtual void OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//�����¼�
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif