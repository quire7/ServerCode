/************************************************************************/
/*  
	NDSelectTableBase.h

	数据库表操作基类(只做select操作);

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

	//执行获得数据操作;
	NDBool		onSelect( NDMysqlBaseOp* pMysqlBaseOp );

	// 获得构造的MYSQL的query语句(只是select操作的语句);
	virtual	string		getQueryString() = 0;
	// 得到执行结果;
	virtual NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult ) = 0;
};

_NDSHAREBASE_END

#endif

