/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\database\NDMysqlBaseOp.h
	file base:	NDMysqlBaseOp
	purpose:	mysql base operation;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SERVERSHARE_ND_MYSQL_BASE_OPERATION_H__
#define __SERVERSHARE_ND_MYSQL_BASE_OPERATION_H__


#include "NDTypes.h"


#ifndef MYSQL_RES
typedef struct st_mysql_res MYSQL_RES;
#endif

#ifndef MYSQL
typedef struct st_mysql	MYSQL;
#endif

_NDSHAREBASE_BEGIN

struct NDSelectResultType;
class NDField;
class NDMysqlQueryResult
{
public:
	NDMysqlQueryResult( MYSQL_RES* pResult, NDUint32 nFieldCount, NDUint32 nRowCount );
	~NDMysqlQueryResult();

	NDBool			nextRow();

	NDField*		fetchRecord()		  { return m_pCurrentRow; }
	NDUint32		getFieldCount() const { return m_nFieldCount; }
	NDUint32		getRowCount()	const { return m_nRowCount; }

	static NDBool	ParseSelectResultToStruct( void* pStructAddr, const NDSelectResultType* pSelectResultType, NDUint8 nMemberNum, NDField* pField );

private:
	MYSQL_RES*		m_pQueryResult;
	NDField*		m_pCurrentRow;
	NDUint32		m_nFieldCount;
	NDUint32		m_nRowCount;
};

struct NDMysqlConnParam;
class NDSysLock;
class NDMysqlBaseOp
{
private:
	struct NDMysqlConnHandle
	{
		NDUint32	m_nIndex;
		NDBool		m_bBusy;
		MYSQL*		m_pMysql;
	};

private:
	NDUint32			m_nConnCount;
	NDSysLock*			m_pHanldeMutex;
	NDMysqlConnHandle*	m_pConnHandle;
	NDMysqlConnParam*	m_pConnParam;

public:
	NDMysqlBaseOp();
	~NDMysqlBaseOp();

	/* mysql initialize operation */
	NDBool			initialize( const NDMysqlConnParam& connParam, NDUint32 nConnCount );
	void			release();
	NDBool			checkConnections();
	void			disConnectDB();

	/************************************************************************/
	/* ȡDB�����Ϣ����                                                     */
	/************************************************************************/
	string			getState();												//������״̬;
	string			getServerInfo();										//��������Ϣ;
	string			getHostInfo();											//������Ϣ;
	string			getClientInfo();										//�ͻ�����Ϣ;
	NDUint32		getProtocolInfo();										//Э����Ϣ;
	
	/* �������ڴ�������SQL�����ʹ�õĺϷ�SQL�ַ�����;*/
	/*MySQL����Ҫ��б�ܺ������ַ�����������ת���ѯ�е��ַ�����;*/
	/*�ú��������������ַ�,�Ӷ�ʹ����������־�ļ��о��и��õĿɶ���.;*/
	string			escapeString( const char* pszBuf, NDUint32 nSize );

	/************************************************************************/
	/* ���ݿ������������;                                                  */
	/************************************************************************/
	NDBool			selectSql( const char *szSql , NDUint32 nSqlSize, MYSQL_RES* &pRefMysqlRes, NDUint32 &nRefFieldCount, NDUint32 &nRefRowCount, NDBool bChinese=NDFalse );//ѡ���¼,���ؽ����;
	NDBool			insertSql( const char *szSql , NDUint32 nSqlSize, NDUint32 &refAffectedRows, NDUint32 &refLastInsertID, NDBool bChinese=NDFalse );		//�����¼;
	NDBool			updateSql( const char *szSql , NDUint32 nSqlSize, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );									//���¼�¼;
	NDBool			deleteSql( const char *szSql , NDUint32 nSqlSize, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );									//ɾ����¼;

	/************************************************************************/
	/* ���ݿ������������;                                                   */
	/************************************************************************/
	void			transBegin();
	void			transCommit();
	void			transRollback();

private:

	NDBool				connectDB( NDMysqlConnHandle* pConn );
	void				disConnectDB( NDMysqlConnHandle* pConn );

	NDMysqlConnHandle*	getIdleHandle();
	void				setIdleHandle( NDMysqlConnHandle* pConn );

	string				escapeString( NDMysqlConnHandle* pConn, const char* pszBuf, NDUint32 nSize );

	// set chinese font ;
	NDBool				setChineseFont( NDMysqlConnHandle* pConn );

	void				freeStoredResults( MYSQL* pMysql );
};

_NDSHAREBASE_END


#endif
