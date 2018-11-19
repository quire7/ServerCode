/************************************************************************/
/*  
	NDTableBase.h

	数据库表操作基类;

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

	//执行获得数据操作;
	NDBool		onSelect( NDMysqlBaseOp* pMysqlBaseOp );
	//执行更新数据操作;
	NDBool		onUpdate( NDMysqlBaseOp* pMysqlBaseOp, NDUint32 &refAffectedRows );
	//执行删除数据操作;
	NDBool		onDelete( NDMysqlBaseOp* pMysqlBaseOp, NDUint32 &refAffectedRows );


	//获得数据操作类型;
	virtual NDUint8		getOperType() const	= 0;

	//数据库表枚举类型;
	virtual NDUint8		getTableType() const = 0;

	//获得数据所属玩家GUID,如果数据不属于任何一个玩家,则返回0;
	virtual NDUint64	getPlayerGUID() const = 0;

	// 获得构造的MYSQL的query语句;
	virtual	string		getQueryString( EDataSourceOperType nOperType ) = 0;
	// 得到执行结果;
	virtual NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult ) = 0;
};

typedef list<NDTableBase*>				NDTableBaseList;
typedef NDTableBaseList::iterator		NDTableBaseListIter;

_NDSHAREBASE_END

#endif

