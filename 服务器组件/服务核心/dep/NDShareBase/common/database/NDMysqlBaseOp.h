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
	/* 取DB相关信息函数                                                     */
	/************************************************************************/
	string			getState();												//服务器状态;
	string			getServerInfo();										//服务器信息;
	string			getHostInfo();											//主机信息;
	string			getClientInfo();										//客户机信息;
	NDUint32		getProtocolInfo();										//协议信息;
	
	/* 函数用于创建可在SQL语句中使用的合法SQL字符串。;*/
	/*MySQL仅需要反斜杠和引号字符，用于引用转义查询中的字符串。;*/
	/*该函数能引用其他字符,从而使得它们在日志文件中具有更好的可读性.;*/
	string			escapeString( const char* pszBuf, NDUint32 nSize );

	/************************************************************************/
	/* 数据库基本操作函数;                                                  */
	/************************************************************************/
	NDBool			selectSql( const char *szSql , NDUint32 nSqlSize, MYSQL_RES* &pRefMysqlRes, NDUint32 &nRefFieldCount, NDUint32 &nRefRowCount, NDBool bChinese=NDFalse );//选择记录,返回结果集;
	NDBool			insertSql( const char *szSql , NDUint32 nSqlSize, NDUint32 &refAffectedRows, NDUint32 &refLastInsertID, NDBool bChinese=NDFalse );		//插入记录;
	NDBool			updateSql( const char *szSql , NDUint32 nSqlSize, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );									//更新记录;
	NDBool			deleteSql( const char *szSql , NDUint32 nSqlSize, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );									//删除记录;

	/************************************************************************/
	/* 数据库事务操作函数;                                                   */
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
