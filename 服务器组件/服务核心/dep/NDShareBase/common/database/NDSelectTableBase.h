/************************************************************************/
/*  
	NDSelectTableBase.h

	���ݿ���������(ֻ��select����);

	create by fxd;
	2012.7.20
*/
/************************************************************************/

#ifndef __SERVERSHARE_ND_SELECT_TABLE_BASE_H__
#define __SERVERSHARE_ND_SELECT_TABLE_BASE_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDMysqlQueryResult;
class NDMysqlBaseOp;
class NDSelectTableBase
{
public:
	NDSelectTableBase(){}
	virtual~NDSelectTableBase(){}

	//ִ�л�����ݲ���;
	NDBool		onSelect( NDMysqlBaseOp* pMysqlBaseOp );

	// ��ù����MYSQL��query���(ֻ��select���������);
	virtual	string		getQueryString() = 0;
	// �õ�ִ�н��;
	virtual NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult ) = 0;
};

_NDSHAREBASE_END

#endif

