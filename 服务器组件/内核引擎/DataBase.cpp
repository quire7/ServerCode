#include "StdAfx.h"
#include "Math.h"
#include "TraceService.h"
#include "DataBase.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseError::CDataBaseError()
{
	m_enErrorType = SQLException_None;
}

//析构函数
CDataBaseError::~CDataBaseError()
{
}

//接口查询
void * CDataBaseError::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseException, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseException, Guid, dwQueryVer);
	return NULL;
}

//设置错误
void CDataBaseError::SetErrorInfo(enSQLException enErrorType, LPCTSTR pszDescribe)
{
	//设置错误
	m_enErrorType = enErrorType;
	m_strErrorDescribe = pszDescribe;

	//抛出错误
	throw QUERY_ME_INTERFACE(IDataBaseException);

	return;
}


//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBase::CDataBase() : m_dwResumeConnectCount(30L)
						, m_dwResumeConnectTime(30L)
						, m_bConnected(FALSE)
						, m_dwLastCheckTimeMS(0)
						, m_bNeedReconnect(FALSE)
{
	//状态变量
	m_dwConnectCount = 0;
	m_dwConnectErrorTime = 0L;

	//设置变量
	memset(&m_dMYSQL, 0, sizeof(m_dMYSQL));
	m_wPort = 0;

	return;
}

//析构函数
CDataBase::~CDataBase()
{
	//关闭连接
	Close();

	ReleaseBufQueue();

	return;
}

//接口查询
void * CDataBase::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBase, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBase, Guid, dwQueryVer);
	return NULL;
}

//打开连接
VOID CDataBase::Open()
{
	//关闭连接;
	Close();

	//连接数据库;
	do
	{
		if (!mysql_init(&m_dMYSQL))
		{
			break;
		}

		//设置连接超时;
		unsigned int timeout = 3;
		int ret = mysql_options(&m_dMYSQL, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
		if (ret)
		{
			break;
		}

		//避免读超时: 可以采用对表加写锁 lock  table tablename  write  或killall -STOP mysqld等来进行测试;
		mysql_options(&m_dMYSQL, MYSQL_OPT_READ_TIMEOUT, (const char *)&timeout);

		//避免写超时: 可以采用对表加写锁 lock  table tablename  write  或killall -STOP mysqld等来进行测试;
		mysql_options(&m_dMYSQL, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

		//设置重连;
		char cReconnect = 1;
		if (mysql_options(&m_dMYSQL, MYSQL_OPT_RECONNECT, &cReconnect) != 0)
		{
			break;
		}

		if (!mysql_real_connect(&m_dMYSQL, m_strHost.c_str(), m_strUser.c_str(), m_strPassword.c_str(),
			m_strDatabaseName.c_str(), m_wPort, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS | CLIENT_TRANSACTIONS))
		{
			break;
		}

		if (mysql_set_character_set(&m_dMYSQL, "utf8mb4") != 0)
		{
			OutputError("mysql_set_character_set", true);
		}

		ExecuteSQL("SET NAMES utf8mb4");

		m_bConnected = TRUE;
	} while (0);

	//设置变量
	m_dwConnectCount = 0L;
	m_dwConnectErrorTime = 0L;

	if (m_bConnected==FALSE)
	{
		CString strMsg;
#ifdef UNICODE
		strMsg.Format(TEXT("连接数据库失败! Error: %s"), CYSCharUtils::ANSIToUnicode(mysql_error(&m_dMYSQL)));
#else
		strMsg.Format(TEXT("连接数据库失败! Error: %s"), mysql_error(&m_dMYSQL));
#endif // UNICODE

		m_dbError.SetErrorInfo(SQLException_Syntax, strMsg);
	}
}

//消除记录集
VOID CDataBase::CleanupResults()
{
	do
	{
		MYSQL_RES* pRes = mysql_store_result(&m_dMYSQL);
		if (pRes)
		{
			mysql_free_result(pRes);
		}
	} while (mysql_next_result(&m_dMYSQL) == 0);
}

//关闭连接
VOID CDataBase::Close()
{
	if (m_bConnected)
	{
		mysql_close(&m_dMYSQL);
		m_bConnected = FALSE;
	}
}

bool CDataBase::IsActive()
{
	if (!m_bConnected) return false;

	int ret = mysql_ping(&m_dMYSQL);
	switch (ret)
	{
	case 0:
		return true;

		//        case CR_COMMANDS_OUT_OF_SYNC: 
		//        case CR_SERVER_GONE_ERROR: 
		//        case CR_UNKNOWN_ERROR: 
		//            return false;

	default:
		return false;
	}
}

//连接信息
bool CDataBase::SetDBInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword)
{
	//效验参数
	ASSERT(dwDBAddr != 0);
	ASSERT(szDBName != NULL);
	ASSERT(szUser != NULL);
	ASSERT(szPassword != NULL);

	BYTE a = (BYTE)((dwDBAddr >> 24) & 0xFF);
	BYTE b = (BYTE)((dwDBAddr >> 16) & 0xFF);
	BYTE c = (BYTE)((dwDBAddr >> 8) & 0xFF);
	BYTE d = (BYTE)(dwDBAddr & 0xFF);

	CString strAddr;
	strAddr.Format(_T("%d.%d.%d.%d"), a, b, c, d);

	return SetDBInfo(strAddr, wPort, szDBName, szUser, szPassword);
}

//设置信息
bool CDataBase::SetDBInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword)
{
	//效验参数
	ASSERT(szDBAddr != NULL);
	ASSERT(szDBName != NULL);
	ASSERT(szUser != NULL);
	ASSERT(szPassword != NULL);

	m_wPort = wPort;

	m_strHost = CYSCharUtils::ConvertToANSI(szDBAddr);
	m_strUser = CYSCharUtils::ConvertToANSI(szUser);
	m_strPassword = CYSCharUtils::ConvertToANSI(szPassword);
	m_strDatabaseName = CYSCharUtils::ConvertToANSI(szDBName);

	return true;
}

//是否打开
BOOL CDataBase::IsOpened()
{
	return m_bConnected;
}

//检测重连
BOOL CDataBase::CheckAndReconnect(DWORD dwCheckInterval)
{
	if (m_bNeedReconnect)
	{
		Close();

		m_bNeedReconnect = FALSE;

		Open();
	}

	DWORD dwCurrent = GetTickCount();
	if (m_dwLastCheckTimeMS == 0 || (dwCurrent - m_dwLastCheckTimeMS) > dwCheckInterval)
	{
		m_dwLastCheckTimeMS = dwCurrent;
		return mysql_ping(&m_dMYSQL) == 0;
	}
	return TRUE;
}

//开始事务
BOOL CDataBase::BeginTransaction()
{
	CleanupResults();
	return SetAutoCommit(FALSE);
}

//停止事务
BOOL CDataBase::CommitTransaction()
{
	CleanupResults();
	BOOL bResult = mysql_commit(&m_dMYSQL) == 0;
	SetAutoCommit(TRUE);
	return bResult;
}

//回滚数据
BOOL CDataBase::RollbackTransaction()
{
	CleanupResults();
	BOOL bResult = mysql_rollback(&m_dMYSQL) == 0;
	SetAutoCommit(TRUE);
	return bResult;
}

//设置自动提交模式
BOOL CDataBase::SetAutoCommit(BOOL bAutoCommit)
{
	return mysql_autocommit(&m_dMYSQL, bAutoCommit) == 0;
}

//选择数据库
BOOL CDataBase::SelectDatabase(const std::string &strDatabaseName)
{
	return mysql_select_db(&m_dMYSQL, strDatabaseName.c_str()) == 0;
}

//删除参数
void CDataBase::ClearParameters()
{
	m_strSQLBuffer.str("");	//stringstream的清空方式;
}

//插入参数
void CDataBase::AddParameter(bool bValue)
{
	return AddParameter("%d", bValue?1:0);
}

//插入参数
void CDataBase::AddParameter(BYTE cbValue)
{
	return AddParameter("%d", cbValue);
}

//插入参数
void CDataBase::AddParameter(WORD wValue)
{
	return AddParameter("%d", wValue);
}

//插入参数
void CDataBase::AddParameter(int iValue)
{
	return AddParameter("%d", iValue);
}

//插入参数
void CDataBase::AddParameter(DWORD dwValue)
{
	return AddParameter("%d", dwValue);
}

//插入参数
void CDataBase::AddParameter(LONG lValue)
{
	return AddParameter("%d", lValue);
}

//插入参数
void CDataBase::AddParameter(LONGLONG llValue)
{
	return AddParameter("%ld", llValue);
}

//插入参数
void CDataBase::AddParameter(SYSTEMTIME& st)
{
	return AddParameter("'%d-%d-%d %d:%d:%d'", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

//插入参数
void CDataBase::AddParameter(LPCTSTR lpsValue)
{
	return AddParameter("'%s'", CYSCharUtils::ConvertToANSI(lpsValue));
}

#define MAX_STRING_LENGTH (2048)
//插入参数
void CDataBase::AddParameter(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	char szBuf[MAX_STRING_LENGTH] = {0};
	vsnprintf_s(szBuf, MAX_STRING_LENGTH, MAX_STRING_LENGTH, fmt, ap);
	va_end(ap);

	std::stringbuf *pbuf = m_strSQLBuffer.rdbuf();
	std::streamsize rdSize = pbuf->in_avail();

	if (rdSize > 0)
	{
		m_strSQLBuffer << ",";
	}
	m_strSQLBuffer << szBuf;
}

void CDataBase::AddBinaryParameter(LPCTSTR lpsValue, int nStrSize)
{
	char* pEscapeBuf = getBuf();

	mysql_real_escape_string(&m_dMYSQL, pEscapeBuf, (char *)lpsValue, nStrSize);

	std::stringbuf *pbuf = m_strSQLBuffer.rdbuf();
	std::streamsize rdSize = pbuf->in_avail();

	if (rdSize > 0)
	{
		m_strSQLBuffer << ",";
	}
	m_strSQLBuffer << "'";
	m_strSQLBuffer << pEscapeBuf;
	m_strSQLBuffer << "'";

	releaseBuf(pEscapeBuf);
}

//插入输出参数
void CDataBase::AddParameterOutput(const char* strOutput)
{
	std::stringbuf *pbuf = m_strSQLBuffer.rdbuf();
	std::streamsize rdSize = pbuf->in_avail();

	if (rdSize > 0)
	{
		m_strSQLBuffer << ",";
	}

	m_strSQLBuffer << strOutput;
}

//执行语句;
BOOL CDataBase::ExecuteProcess(const std::string &strSQL)
{
	std::stringbuf *pbuf = m_strSQLBuffer.rdbuf();
	std::streamsize rdSize = pbuf->in_avail();

	int nReserveSize = 16 + strSQL.length() + rdSize;

	std::string strProcess;
	strProcess.reserve(nReserveSize);
	strProcess.append("CALL ");
	strProcess.append(strSQL);

	if (rdSize > 0)
	{
		strProcess.append("(");
		strProcess.append(m_strSQLBuffer.str());
		strProcess.append(")");
	}
	else
	{
		strProcess.append("()");
	}

	return ExecuteSQL(strProcess);
}

BOOL CDataBase::ExecuteSQL(const std::string& strSQL)
{
	if (!IsOpened())
		return FALSE;

	// 这一步 直接 把  多结果集  屏蔽了;
	CleanupResults();

	if (mysql_real_query(&m_dMYSQL, strSQL.c_str(), strSQL.length()) != 0)
	{
		OutputError(strSQL);
		return FALSE;
	}

	//CTraceService::TraceString(CYSCharUtils::ANSIToUnicode(strSQL.c_str()), TraceLevel_Exception);
	//TRACE(CYSCharUtils::ANSIToUnicode(strSQL.c_str()));

	return TRUE;
}

__int64 CDataBase::GetLastInsertID()
{
	return mysql_insert_id(&m_dMYSQL);
}

BOOL CDataBase::ExecuteSQLNonResult(const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	_vsnprintf_s(m_szBuf, _countof(m_szBuf), fmt, vl);
	va_end(vl);

	return ExecuteSQL(m_szBuf);
}

VOID CDataBase::OutputError(const std::string &sql, bool bReconnect/* = false*/)
{
	char msg[8192] = "";
	if (sql.length() < 7000)
	{
		_snprintf_s(msg, sizeof(msg), "SQLError(%d): %s Informatin:%s", mysql_errno(&m_dMYSQL), sql.c_str(), mysql_error(&m_dMYSQL));
	}
	else
	{
		string str(sql.c_str(), sql.c_str() + 7000);
		_snprintf_s(msg, sizeof(msg), "SQLError(%d): %s Informatin:%s", mysql_errno(&m_dMYSQL), str.c_str(), mysql_error(&m_dMYSQL));
	}

#ifdef UNICODE
	CTraceService::TraceString(CYSCharUtils::ANSIToUnicode(msg), TraceLevel_Exception);
	TRACE(CYSCharUtils::ANSIToUnicode(msg));
#else
	CTraceService::TraceString(msg, TraceLevel_Exception);
	TRACE(msg);
#endif // UNICODE

	if (bReconnect)
	{
		m_bNeedReconnect = TRUE;
	}
}

void CDataBase::ReleaseBufQueue()
{
	while (!m_bufQueue.empty())
	{
		char* pBuf = m_bufQueue.front();
		m_bufQueue.pop();

		free(pBuf);
		pBuf = NULL;
	}
}

char* CDataBase::getBuf()
{
	char* pRet = NULL;

	while (m_bufQueue.empty())
	{
		for (int i = 0; i < 5; ++i)
		{
			char* pBuf = (char*)malloc(MAX_16_BUF);	//16M内存;
			m_bufQueue.push(pBuf);
		}
	}

	pRet = m_bufQueue.front();
	m_bufQueue.pop();

	memset(pRet, 0, MAX_16_BUF);

	return pRet;
}

void CDataBase::releaseBuf(char* pBuf)
{
	m_bufQueue.push(pBuf);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngine::CDataBaseEngine(void)
{
	//设置变量
	m_bService = false;
	m_pIDataBaseEngineSinks.clear();

	return;
}

//析构函数
CDataBaseEngine::~CDataBaseEngine(void)
{
}

//接口查询
void * CDataBaseEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngine, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngine, Guid, dwQueryVer);
	return NULL;
}

//注册接口
bool  CDataBaseEngine::SetDataBaseEngineSink(IUnknownEx * pIUnknownEx)
{
	SetDataBaseEngineSink(&pIUnknownEx, 1);
	return true;
}

bool  CDataBaseEngine::SetDataBaseEngineSink(IUnknownEx * pIUnknownExs[], WORD wSinkCount)
{
	m_pIDataBaseEngineSinks.clear();
	wSinkCount = 1;
	for (WORD i = 0; i < wSinkCount; i++)
	{
		IUnknownEx *pIUnknownEx = pIUnknownExs[i];
		//效验参数
		ASSERT(pIUnknownEx != NULL);
		if (pIUnknownEx == NULL) return false;

		//查询接口
		IDataBaseEngineSink * pIDataBaseEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IDataBaseEngineSink);
		if (pIDataBaseEngineSink == NULL)
		{
			CTraceService::TraceString(TEXT("数据库引擎外挂服务接口获取失败，挂接操作失败"), TraceLevel_Exception);
			return false;
		}
		m_pIDataBaseEngineSinks.push_back(pIDataBaseEngineSink);
	}
	return true;
}
//启动服务
bool CDataBaseEngine::StartService()
{
	//判断状态
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("数据库引擎重复启动，启动操作忽略"), TraceLevel_Warning);
		return true;
	}

	//外挂接口
	if (m_pIDataBaseEngineSinks.empty())
	{
		CTraceService::TraceString(TEXT("数据库引擎外挂服务不存在"), TraceLevel_Exception);
		return false;
	}

	//设置队列
	if (m_QueueServiceEvent.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
	{
		CTraceService::TraceString(TEXT("数据库引擎与队列服务绑定失败"), TraceLevel_Exception);
		return false;
	}

	//启动外挂
	for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
	{
		//启动外挂
		if (m_pIDataBaseEngineSinks[i]->OnDataBaseEngineStart(QUERY_ME_INTERFACE(IUnknownEx)) == false)
		{
			CTraceService::TraceString(TEXT("数据库引擎外挂服务启动失败"), TraceLevel_Exception);
			return false;
		}
	}
	//设置变量
	m_bService = true;

	return true;
}

//停止服务
bool CDataBaseEngine::ConcludeService()
{
	//设置变量
	m_bService = false;

	//停止请求队列
	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	//停止外挂
	for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
	{
		//停止外挂
		if (m_pIDataBaseEngineSinks[i] != NULL)
		{
			m_pIDataBaseEngineSinks[i]->OnDataBaseEngineConclude(QUERY_ME_INTERFACE(IUnknownEx));
		}
	}
	m_pIDataBaseEngineSinks.clear();


	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	return true;
}

//引擎负荷
bool CDataBaseEngine::GetBurthenInfo( tagBurthenInfo & BurthenInfo )
{
	ASSERT(TRUE);ASSERT(FALSE);
	return true;
}

//队列接口
void CDataBaseEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	//判断状态
	if (m_bService == false) return;

	//请求处理
	switch (wIdentifier)
	{
		case EVENT_DATABASE:
		{
			//效验参数
			ASSERT(pBuffer != NULL);
			ASSERT(wDataSize >= sizeof(NTY_DataBaseEvent));
			if (wDataSize < sizeof(NTY_DataBaseEvent)) return;

			//变量定义
			NTY_DataBaseEvent * pDataBaseEvent = (NTY_DataBaseEvent *)pBuffer;
			WORD wHandleBuffer = wDataSize - sizeof(NTY_DataBaseEvent);

			//处理数据
			for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
			{
				ASSERT(m_pIDataBaseEngineSinks[i] != NULL);
				m_pIDataBaseEngineSinks[i]->OnDataBaseEngineRequest(pDataBaseEvent->wRequestID, pDataBaseEvent->dwContextID, pDataBaseEvent + 1, wHandleBuffer);
			}

			return;
		}
		case EVENT_TIMER://时间事件
		{
			//效验参数
			ASSERT(pBuffer != NULL);
			ASSERT(wDataSize >= sizeof(NTY_TimerEvent));
			if (wDataSize < sizeof(NTY_TimerEvent)) return;

			//变量定义
			NTY_TimerEvent * pDataBaseEvent = (NTY_TimerEvent *)pBuffer;
			WORD wHandleBuffer = wDataSize - sizeof(NTY_TimerEvent);

			//处理数据
			for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
			{
				ASSERT(m_pIDataBaseEngineSinks[i] != NULL);
				m_pIDataBaseEngineSinks[i]->OnDataBaseEngineTimer(pDataBaseEvent->dwTimerID, pDataBaseEvent->dwBindParameter);
			}
			return;
		}
		case EVENT_CONTROL://控制事件
		{
			//效验参数
			ASSERT(pBuffer != NULL);
			ASSERT(wDataSize >= sizeof(NTY_ControlEvent));
			if (wDataSize < sizeof(NTY_ControlEvent)) return;

			//变量定义
			NTY_ControlEvent * pDataBaseEvent = (NTY_ControlEvent *)pBuffer;
			WORD wHandleBuffer = wDataSize - sizeof(NTY_ControlEvent);

			//处理数据
			for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
			{
				ASSERT(m_pIDataBaseEngineSinks[i] != NULL);
				m_pIDataBaseEngineSinks[i]->OnDataBaseEngineControl(pDataBaseEvent->wControlID, pDataBaseEvent + 1, wHandleBuffer);
			}
			return;
		}
		default:
		{
			ASSERT(false);
		}
	}

	return;
}

//控制事件
//////////////////////////////////////////////////////////////////////////
//控制事件
bool CDataBaseEngine::PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return m_QueueServiceEvent.PostControlEvent(wControlID, pData, wDataSize);
}

//请求事件
bool CDataBaseEngine::PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return m_QueueServiceEvent.PostDataBaseEvent(wRequestID, dwContextID, pData, wDataSize);
}

//延期请求
bool CDataBaseEngine::DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(FALSE);
	return false;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * CreateDataBaseEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CDataBaseEngine * pDataBaseEngine = NULL;
	try
	{
		pDataBaseEngine = new CDataBaseEngine();
		if (pDataBaseEngine == NULL) throw TEXT("创建失败");
		void * pObject = pDataBaseEngine->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pDataBaseEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * CreateDataBase(const GUID & Guid, DWORD dwInterfaceVer)
{
	//建立对象
	CDataBase * pDataBase = NULL;
	try
	{
		pDataBase = new CDataBase();
		if (pDataBase == NULL) throw TEXT("创建失败");
		void * pObject = pDataBase->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pDataBase);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
