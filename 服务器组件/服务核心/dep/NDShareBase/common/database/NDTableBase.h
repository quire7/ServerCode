/************************************************************************/
/*  
	NDTableBase.h

	���ݿ���������;

	create by fxd;
	2012.7.20
*/
/************************************************************************/

#ifndef __SERVERSHARE_ND_TABLE_BASE_H__
#define __SERVERSHARE_ND_TABLE_BASE_H__

#include <list>
using std::list;

#include "NDTypes.h"
#include "NDShareBaseEnums.h"

_NDSHAREBASE_BEGIN

class NDMysqlQueryResult;
class NDMysqlBaseOp;
class NDTableBase
{
public:
	NDTableBase(){}
	virtual~NDTableBase(){}

	//ִ�л�����ݲ���;
	NDBool		onSelect( NDMysqlBaseOp* pMysqlBaseOp );
	//ִ�и������ݲ���;
	NDBool		onUpdate( NDMysqlBaseOp* pMysqlBaseOp, NDUint32 &refAffectedRows );
	//ִ��ɾ�����ݲ���;
	NDBool		onDelete( NDMysqlBaseOp* pMysqlBaseOp, NDUint32 &refAffectedRows );


	//������ݲ�������;
	virtual NDUint8		getOperType() const	= 0;

	//���ݿ��ö������;
	virtual NDUint8		getTableType() const = 0;

	//��������������GUID,������ݲ������κ�һ�����,�򷵻�0;
	virtual NDUint64	getPlayerGUID() const = 0;

	// ��ù����MYSQL��query���;
	virtual	string		getQueryString( EDataSourceOperType nOperType ) = 0;
	// �õ�ִ�н��;
	virtual NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult ) = 0;
};

typedef list<NDTableBase*>				NDTableBaseList;
typedef NDTableBaseList::iterator		NDTableBaseListIter;

_NDSHAREBASE_END

#endif

