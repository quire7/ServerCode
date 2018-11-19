/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\database\NDMysqlBase.h
	file base:	NDMysqlBase
	purpose:	mysql encapsulation;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SERVERSHARE_ND_MYSQL_BASE_H__
#define __SERVERSHARE_ND_MYSQL_BASE_H__


#include "NDTypes.h"


_NDSHAREBASE_BEGIN

struct NDMysqlConnParam;
class NDMysqlQueryResult;
class NDMysqlBaseOp;
class NDMysqlBase
{
public:
	NDMysqlBase();
	virtual ~NDMysqlBase();

	/* mysql initialize operation */
	NDBool			initialize( const NDMysqlConnParam& connParam, NDUint32 nConnCount );
	void			disConnectDB();

	/************************************************************************/
	/* 取DB相关信息函数                                                     */
	/************************************************************************/
	string			getState();												//服务器状态;
	string			getServerInfo();										//服务器信息;
	string			getHostInfo();											//主机信息;
	string			getClientInfo();										//客户机信息;
	NDUint32		getProtocolInfo();										//协议信息;
	

	/************************************************************************/
	/* 数据库基本操作函数                                                   */
	/************************************************************************/
	//选择记录;(pRefResult:返回结果,nRefFieldCount:影响的列数,nRefRowCount:影响的行数);
	NDBool					selectSql( NDUint32 nProtocolID, void* pProtocolReq, void* &pRefResult, NDUint32 &nRefFieldCount, NDUint32 &nRefRowCount, NDBool bChinese=NDFalse );				
	//插入记录;(refAffectedRows:返回删除影响的行数,refLastInsertID:返回最新插入行的自动ID);
	NDBool					insertSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDUint32 &refLastInsertID, NDBool bChinese=NDFalse );
	//更新记录;(refAffectedRows:返回更新影响的行数);
	NDBool					updateSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );
	//删除记录(refAffectedRows:返回删除影响的行数);
	NDBool					deleteSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );			

protected:
	
	// 构造MYSQL语句;
	virtual string			getQueryString( NDUint32 nProtocolID, void* pProtocolReq ) = 0;
	// 得到执行结果;
	virtual	NDBool			getResult( NDUint32	nProtocolID, NDMysqlQueryResult& refQueryResult, void* &pRefResult ) = 0;

private:

	NDMysqlBaseOp*	m_pMysqlBaseOp;

};


_NDSHAREBASE_END

#endif

