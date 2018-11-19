#ifndef DATA_BASE_ENGINE_HEAD_FILE
#define DATA_BASE_ENGINE_HEAD_FILE

#pragma once

//组件头文件
#include "KernelEngineHead.h"
#include "QueueService.h"
#include "TraceService.h"
#include "QueueServiceEvent.h"

#include "../../服务器组件/服务核心/dep/NDShareBase/common/mysql/mysql.h"
#include <vector>
using namespace std;
#include <queue>
using std::queue;

/* catch error record set result. */
#define ErrorSQLRS (0xFFFFFFFF)

#define MAX_16_BUF	(16777216)			//16M;
//////////////////////////////////////////////////////////////////////////

//DataBase 错误类
class CDataBaseError : public IDataBaseException
{
	//变量定义
protected:
	enSQLException					m_enErrorType;						//错误代号
	CString							m_strErrorDescribe;					//错误信息

	//函数定义
public:
	//构造函数
	CDataBaseError();
	//析构函数
	virtual ~CDataBaseError();

	//基础接口
public:
	//释放对象
	virtual VOID Release(){	return ;}
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//功能接口
public:
	//错误类型
	virtual enSQLException GetExceptionType()
	{
		return m_enErrorType;
	}
	//错误描述
	virtual LPCTSTR GetExceptionDescribe()
	{
		return m_strErrorDescribe;
	}

	//功能函数
public:
	//设置错误
	void SetErrorInfo(enSQLException enErrorType, LPCTSTR pszDescribe);
};


//////////////////////////////////////////////////////////////////////////

//数据库对象
class CDataBase : public IDataBase
{
	//信息变量
protected:
	CDataBaseError					m_dbError;							//错误对象
	CString							m_strConnect;						//连接字符串
	CString							m_strErrorDescribe;					//错误信息

	std::string						m_strHost;
	WORD							m_wPort;
	std::string						m_strUser;
	std::string						m_strPassword;
	std::string						m_strDatabaseName;

	char							m_szBuf[100 * 1000];
	stringstream					m_strSQLBuffer;

	queue<char*>					m_bufQueue;							//16M内存容器;

	//状态变量
protected:
	DWORD							m_dwConnectCount;					//重试次数
	DWORD							m_dwConnectErrorTime;				//错误时间
	const DWORD						m_dwResumeConnectCount;				//恢复次数
	const DWORD						m_dwResumeConnectTime;				//恢复时间

	//内核变量
protected:
	MYSQL							m_dMYSQL;							//Mysql的基本信息。
	BOOL							m_bConnected;
	BOOL							m_bNeedReconnect;
	DWORD							m_dwLastCheckTimeMS;

	//函数定义
public:
	//构造函数
	CDataBase();
	//析构函数
	virtual ~CDataBase();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//打开连接
	virtual VOID Open();
	//关闭连接
	virtual VOID Close();
	//是否活跃
	virtual bool IsActive();
	//消除记录集
	virtual VOID CleanupResults();
	//连接信息
	virtual bool SetDBInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);
	//设置信息
	virtual bool SetDBInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword);

	//状态接口
public:
	//是否打开
	virtual BOOL IsOpened();
	//检测重连
	virtual BOOL CheckAndReconnect(DWORD dwCheckInterval = 1000);

	//事务接口
public:
	//开始事务
	virtual BOOL BeginTransaction();
	//停止事务
	virtual BOOL CommitTransaction();
	//回滚数据
	virtual BOOL RollbackTransaction();

public:
	//设置自动提交模式
	virtual BOOL SetAutoCommit(BOOL bAutoCommit);
	//选择数据库
	virtual BOOL SelectDatabase(const std::string& strDatabaseName);

	//命令对象接口
public:
	//插入参数
	virtual void AddParameter(bool bValue);
	//插入参数
	virtual void AddParameter(BYTE cbValue);
	//插入参数
	virtual void AddParameter(WORD wValue);
	//插入参数
	virtual void AddParameter(int iValue);
	//插入参数
	virtual void AddParameter(DWORD dwValue);
	//插入参数
	virtual void AddParameter(LONG lValue);
	//插入参数
	virtual void AddParameter(LONGLONG llValue);
	//插入参数
	virtual void AddParameter(SYSTEMTIME& st);
	//插入参数
	virtual void AddParameter(LPCTSTR lpsValue);
	//插入参数(nStrSize:数据流的大小(插入二进制数据时需要利用mysql_real_escape_string函数对数据进行转换);
	virtual void AddBinaryParameter(LPCTSTR lpsValue, int nStrSize);
	//插入输出参数
	virtual void AddParameterOutput(const char* strOutput);
	//删除参数
	virtual void ClearParameters();

private:
	//插入参数
	void AddParameter(const char* fmt, ...);

	//执行接口
public:
	//执行语句
	virtual BOOL ExecuteProcess(const std::string &strSQL);
	//执行语句
	virtual BOOL ExecuteSQL(const std::string& strSQL);
	//获取最后插入数据ID
	virtual __int64 GetLastInsertID();

	//执行没有结果集的语句
	BOOL ExecuteSQLNonResult(const char* fmt, ...);

public:
	operator MYSQL*() {	return &m_dMYSQL; }

	MYSQL* GetMYSQL() { return &m_dMYSQL; }

	//内部函数
private:

	VOID OutputError(const std::string& strSQL, bool bReconnect = false);

	void	ReleaseBufQueue();
	char*	getBuf();
	void	releaseBuf(char* pBuf);
};

//////////////////////////////////////////////////////////////////////////

//数据库管理类
class CDataBaseEngine : public IDataBaseEngine, public IQueueServiceSink
{
	//内核变量
protected:
	bool							m_bService;							//运行标志
	CQueueServiceEvent				m_QueueServiceEvent;				//队列对象
	//IDataBaseEngineSink				* m_pIDataBaseEngineSink;			//通知钩子

	vector<IDataBaseEngineSink *>   m_pIDataBaseEngineSinks;			//通知钩子
	//函数定义
public:
	//构造函数
	CDataBaseEngine(void);
	//析构函数
	virtual ~CDataBaseEngine(void);

	//基础接口
public:
	//释放对象
	virtual VOID Release()
	{
		if (IsValid()) delete this;
		return /*true*/;
	}
	//是否有效
	virtual bool IsValid()
	{
		return AfxIsValidAddress(this, sizeof(CDataBaseEngine)) ? true : false;
	}
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

public:
	//引擎负荷
	virtual bool GetBurthenInfo(tagBurthenInfo & BurthenInfo);

	//配置接口
public:
	//配置模块
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx);
	//配置模块
	virtual bool SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount);

	//队列接口
public:
	//队列接口
	virtual void OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize);

	//控制事件
public:
	//控制事件
	virtual bool PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
	//延期请求
	virtual bool DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif