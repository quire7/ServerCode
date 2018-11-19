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
	/* ȡDB�����Ϣ����                                                     */
	/************************************************************************/
	string			getState();												//������״̬;
	string			getServerInfo();										//��������Ϣ;
	string			getHostInfo();											//������Ϣ;
	string			getClientInfo();										//�ͻ�����Ϣ;
	NDUint32		getProtocolInfo();										//Э����Ϣ;
	

	/************************************************************************/
	/* ���ݿ������������                                                   */
	/************************************************************************/
	//ѡ���¼;(pRefResult:���ؽ��,nRefFieldCount:Ӱ�������,nRefRowCount:Ӱ�������);
	NDBool					selectSql( NDUint32 nProtocolID, void* pProtocolReq, void* &pRefResult, NDUint32 &nRefFieldCount, NDUint32 &nRefRowCount, NDBool bChinese=NDFalse );				
	//�����¼;(refAffectedRows:����ɾ��Ӱ�������,refLastInsertID:�������²����е��Զ�ID);
	NDBool					insertSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDUint32 &refLastInsertID, NDBool bChinese=NDFalse );
	//���¼�¼;(refAffectedRows:���ظ���Ӱ�������);
	NDBool					updateSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );
	//ɾ����¼(refAffectedRows:����ɾ��Ӱ�������);
	NDBool					deleteSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDBool bChinese=NDFalse );			

protected:
	
	// ����MYSQL���;
	virtual string			getQueryString( NDUint32 nProtocolID, void* pProtocolReq ) = 0;
	// �õ�ִ�н��;
	virtual	NDBool			getResult( NDUint32	nProtocolID, NDMysqlQueryResult& refQueryResult, void* &pRefResult ) = 0;

private:

	NDMysqlBaseOp*	m_pMysqlBaseOp;

};


_NDSHAREBASE_END

#endif

