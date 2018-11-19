#include "StdAfx.h"
#include "Math.h"
#include "TraceService.h"
#include "DataBase.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseError::CDataBaseError()
{
	m_enErrorType = SQLException_None;
}

//��������
CDataBaseError::~CDataBaseError()
{
}

//�ӿڲ�ѯ
void * CDataBaseError::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseException, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseException, Guid, dwQueryVer);
	return NULL;
}

//���ô���
void CDataBaseError::SetErrorInfo(enSQLException enErrorType, LPCTSTR pszDescribe)
{
	//���ô���
	m_enErrorType = enErrorType;
	m_strErrorDescribe = pszDescribe;

	//�׳�����
	throw QUERY_ME_INTERFACE(IDataBaseException);

	return;
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBase::CDataBase() : m_dwResumeConnectCount(30L)
						, m_dwResumeConnectTime(30L)
						, m_bConnected(FALSE)
						, m_dwLastCheckTimeMS(0)
						, m_bNeedReconnect(FALSE)
{
	//״̬����
	m_dwConnectCount = 0;
	m_dwConnectErrorTime = 0L;

	//���ñ���
	memset(&m_dMYSQL, 0, sizeof(m_dMYSQL));
	m_wPort = 0;

	return;
}

//��������
CDataBase::~CDataBase()
{
	//�ر�����
	Close();

	ReleaseBufQueue();

	return;
}

//�ӿڲ�ѯ
void * CDataBase::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBase, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBase, Guid, dwQueryVer);
	return NULL;
}

//������
VOID CDataBase::Open()
{
	//�ر�����;
	Close();

	//�������ݿ�;
	do
	{
		if (!mysql_init(&m_dMYSQL))
		{
			break;
		}

		//�������ӳ�ʱ;
		unsigned int timeout = 3;
		int ret = mysql_options(&m_dMYSQL, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
		if (ret)
		{
			break;
		}

		//�������ʱ: ���Բ��öԱ��д�� lock  table tablename  write  ��killall -STOP mysqld�������в���;
		mysql_options(&m_dMYSQL, MYSQL_OPT_READ_TIMEOUT, (const char *)&timeout);

		//����д��ʱ: ���Բ��öԱ��д�� lock  table tablename  write  ��killall -STOP mysqld�������в���;
		mysql_options(&m_dMYSQL, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

		//��������;
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

	//���ñ���
	m_dwConnectCount = 0L;
	m_dwConnectErrorTime = 0L;

	if (m_bConnected==FALSE)
	{
		CString strMsg;
#ifdef UNICODE
		strMsg.Format(TEXT("�������ݿ�ʧ��! Error: %s"), CYSCharUtils::ANSIToUnicode(mysql_error(&m_dMYSQL)));
#else
		strMsg.Format(TEXT("�������ݿ�ʧ��! Error: %s"), mysql_error(&m_dMYSQL));
#endif // UNICODE

		m_dbError.SetErrorInfo(SQLException_Syntax, strMsg);
	}
}

//������¼��
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

//�ر�����
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

//������Ϣ
bool CDataBase::SetDBInfo(DWORD dwDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword)
{
	//Ч�����
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

//������Ϣ
bool CDataBase::SetDBInfo(LPCTSTR szDBAddr, WORD wPort, LPCTSTR szDBName, LPCTSTR szUser, LPCTSTR szPassword)
{
	//Ч�����
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

//�Ƿ��
BOOL CDataBase::IsOpened()
{
	return m_bConnected;
}

//�������
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

//��ʼ����
BOOL CDataBase::BeginTransaction()
{
	CleanupResults();
	return SetAutoCommit(FALSE);
}

//ֹͣ����
BOOL CDataBase::CommitTransaction()
{
	CleanupResults();
	BOOL bResult = mysql_commit(&m_dMYSQL) == 0;
	SetAutoCommit(TRUE);
	return bResult;
}

//�ع�����
BOOL CDataBase::RollbackTransaction()
{
	CleanupResults();
	BOOL bResult = mysql_rollback(&m_dMYSQL) == 0;
	SetAutoCommit(TRUE);
	return bResult;
}

//�����Զ��ύģʽ
BOOL CDataBase::SetAutoCommit(BOOL bAutoCommit)
{
	return mysql_autocommit(&m_dMYSQL, bAutoCommit) == 0;
}

//ѡ�����ݿ�
BOOL CDataBase::SelectDatabase(const std::string &strDatabaseName)
{
	return mysql_select_db(&m_dMYSQL, strDatabaseName.c_str()) == 0;
}

//ɾ������
void CDataBase::ClearParameters()
{
	m_strSQLBuffer.str("");	//stringstream����շ�ʽ;
}

//�������
void CDataBase::AddParameter(bool bValue)
{
	return AddParameter("%d", bValue?1:0);
}

//�������
void CDataBase::AddParameter(BYTE cbValue)
{
	return AddParameter("%d", cbValue);
}

//�������
void CDataBase::AddParameter(WORD wValue)
{
	return AddParameter("%d", wValue);
}

//�������
void CDataBase::AddParameter(int iValue)
{
	return AddParameter("%d", iValue);
}

//�������
void CDataBase::AddParameter(DWORD dwValue)
{
	return AddParameter("%d", dwValue);
}

//�������
void CDataBase::AddParameter(LONG lValue)
{
	return AddParameter("%d", lValue);
}

//�������
void CDataBase::AddParameter(LONGLONG llValue)
{
	return AddParameter("%ld", llValue);
}

//�������
void CDataBase::AddParameter(SYSTEMTIME& st)
{
	return AddParameter("'%d-%d-%d %d:%d:%d'", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

//�������
void CDataBase::AddParameter(LPCTSTR lpsValue)
{
	return AddParameter("'%s'", CYSCharUtils::ConvertToANSI(lpsValue));
}

#define MAX_STRING_LENGTH (2048)
//�������
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

//�����������
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

//ִ�����;
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

	// ��һ�� ֱ�� ��  ������  ������;
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
			char* pBuf = (char*)malloc(MAX_16_BUF);	//16M�ڴ�;
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

//���캯��
CDataBaseEngine::CDataBaseEngine(void)
{
	//���ñ���
	m_bService = false;
	m_pIDataBaseEngineSinks.clear();

	return;
}

//��������
CDataBaseEngine::~CDataBaseEngine(void)
{
}

//�ӿڲ�ѯ
void * CDataBaseEngine::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngine, Guid, dwQueryVer);
	QUERYINTERFACE(IQueueServiceSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngine, Guid, dwQueryVer);
	return NULL;
}

//ע��ӿ�
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
		//Ч�����
		ASSERT(pIUnknownEx != NULL);
		if (pIUnknownEx == NULL) return false;

		//��ѯ�ӿ�
		IDataBaseEngineSink * pIDataBaseEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IDataBaseEngineSink);
		if (pIDataBaseEngineSink == NULL)
		{
			CTraceService::TraceString(TEXT("���ݿ�������ҷ���ӿڻ�ȡʧ�ܣ��ҽӲ���ʧ��"), TraceLevel_Exception);
			return false;
		}
		m_pIDataBaseEngineSinks.push_back(pIDataBaseEngineSink);
	}
	return true;
}
//��������
bool CDataBaseEngine::StartService()
{
	//�ж�״̬
	if (m_bService == true)
	{
		CTraceService::TraceString(TEXT("���ݿ������ظ�������������������"), TraceLevel_Warning);
		return true;
	}

	//��ҽӿ�
	if (m_pIDataBaseEngineSinks.empty())
	{
		CTraceService::TraceString(TEXT("���ݿ�������ҷ��񲻴���"), TraceLevel_Exception);
		return false;
	}

	//���ö���
	if (m_QueueServiceEvent.SetQueueServiceSink(QUERY_ME_INTERFACE(IUnknownEx)) == false)
	{
		CTraceService::TraceString(TEXT("���ݿ���������з����ʧ��"), TraceLevel_Exception);
		return false;
	}

	//�������
	for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
	{
		//�������
		if (m_pIDataBaseEngineSinks[i]->OnDataBaseEngineStart(QUERY_ME_INTERFACE(IUnknownEx)) == false)
		{
			CTraceService::TraceString(TEXT("���ݿ�������ҷ�������ʧ��"), TraceLevel_Exception);
			return false;
		}
	}
	//���ñ���
	m_bService = true;

	return true;
}

//ֹͣ����
bool CDataBaseEngine::ConcludeService()
{
	//���ñ���
	m_bService = false;

	//ֹͣ�������
	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	//ֹͣ���
	for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
	{
		//ֹͣ���
		if (m_pIDataBaseEngineSinks[i] != NULL)
		{
			m_pIDataBaseEngineSinks[i]->OnDataBaseEngineConclude(QUERY_ME_INTERFACE(IUnknownEx));
		}
	}
	m_pIDataBaseEngineSinks.clear();


	m_QueueServiceEvent.SetQueueServiceSink(NULL);

	return true;
}

//���渺��
bool CDataBaseEngine::GetBurthenInfo( tagBurthenInfo & BurthenInfo )
{
	ASSERT(TRUE);ASSERT(FALSE);
	return true;
}

//���нӿ�
void CDataBaseEngine::OnQueueServiceSink(WORD wIdentifier, void * pBuffer, WORD wDataSize)
{
	//�ж�״̬
	if (m_bService == false) return;

	//������
	switch (wIdentifier)
	{
		case EVENT_DATABASE:
		{
			//Ч�����
			ASSERT(pBuffer != NULL);
			ASSERT(wDataSize >= sizeof(NTY_DataBaseEvent));
			if (wDataSize < sizeof(NTY_DataBaseEvent)) return;

			//��������
			NTY_DataBaseEvent * pDataBaseEvent = (NTY_DataBaseEvent *)pBuffer;
			WORD wHandleBuffer = wDataSize - sizeof(NTY_DataBaseEvent);

			//��������
			for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
			{
				ASSERT(m_pIDataBaseEngineSinks[i] != NULL);
				m_pIDataBaseEngineSinks[i]->OnDataBaseEngineRequest(pDataBaseEvent->wRequestID, pDataBaseEvent->dwContextID, pDataBaseEvent + 1, wHandleBuffer);
			}

			return;
		}
		case EVENT_TIMER://ʱ���¼�
		{
			//Ч�����
			ASSERT(pBuffer != NULL);
			ASSERT(wDataSize >= sizeof(NTY_TimerEvent));
			if (wDataSize < sizeof(NTY_TimerEvent)) return;

			//��������
			NTY_TimerEvent * pDataBaseEvent = (NTY_TimerEvent *)pBuffer;
			WORD wHandleBuffer = wDataSize - sizeof(NTY_TimerEvent);

			//��������
			for (WORD i = 0; i < m_pIDataBaseEngineSinks.size(); i++)
			{
				ASSERT(m_pIDataBaseEngineSinks[i] != NULL);
				m_pIDataBaseEngineSinks[i]->OnDataBaseEngineTimer(pDataBaseEvent->dwTimerID, pDataBaseEvent->dwBindParameter);
			}
			return;
		}
		case EVENT_CONTROL://�����¼�
		{
			//Ч�����
			ASSERT(pBuffer != NULL);
			ASSERT(wDataSize >= sizeof(NTY_ControlEvent));
			if (wDataSize < sizeof(NTY_ControlEvent)) return;

			//��������
			NTY_ControlEvent * pDataBaseEvent = (NTY_ControlEvent *)pBuffer;
			WORD wHandleBuffer = wDataSize - sizeof(NTY_ControlEvent);

			//��������
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

//�����¼�
//////////////////////////////////////////////////////////////////////////
//�����¼�
bool CDataBaseEngine::PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return m_QueueServiceEvent.PostControlEvent(wControlID, pData, wDataSize);
}

//�����¼�
bool CDataBaseEngine::PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return m_QueueServiceEvent.PostDataBaseEvent(wRequestID, dwContextID, pData, wDataSize);
}

//��������
bool CDataBaseEngine::DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	ASSERT(FALSE);
	return false;
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) void * CreateDataBaseEngine(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CDataBaseEngine * pDataBaseEngine = NULL;
	try
	{
		pDataBaseEngine = new CDataBaseEngine();
		if (pDataBaseEngine == NULL) throw TEXT("����ʧ��");
		void * pObject = pDataBaseEngine->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pDataBaseEngine);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) void * CreateDataBase(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CDataBase * pDataBase = NULL;
	try
	{
		pDataBase = new CDataBase();
		if (pDataBase == NULL) throw TEXT("����ʧ��");
		void * pObject = pDataBase->QueryInterface(Guid, dwInterfaceVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pDataBase);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
