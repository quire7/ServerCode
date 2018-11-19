/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\database\NDField.h
	file base:	NDField
	purpose:	mysql field;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SERVERSHARE_ND_FIELD_H__
#define __SERVERSHARE_ND_FIELD_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

enum NDFieldType
{
	FIELD_BOOL,
	FIELD_I8,
	FIELD_I16,
	FIELD_I32,
	FIELD_I64,
	FIELD_UI8,
	FIELD_UI16,
	FIELD_UI32,
	FIELD_UI64,
	FIELD_FLOAT,
	FIELD_DOUBLE,
	FIELD_STRING,
	FIELD_BIN_DATA,
	FIELD_NONE,
};

class NDField
{
public:
	NDField();
	~NDField();

	void	setValue( char* pValue );

	const char* getString();

	NDInt8		getInt8();
	NDUint8		getUint8();
	NDInt16		getInt16();
	NDUint16	getUint16();
	NDInt32		getInt32();
	NDUint32	getUint32();
	NDInt64		getInt64();
	NDUint64	getUint64();

	NDFloat32	getFloat();
	NDFloat64	getDouble();

	NDBool		getBool();

private:
	char*	m_pValue;
};


struct NDSelectResultType
{
	NDUint8		m_nIndex;				//数据表列的下标;
	NDUint8		m_nFieldType;			//数据表列的数据类型;
	NDInt32		m_nExtend;				//为了补充(数据表列的数据类型)的不足;
	NDUint64	m_nAddrOffset;			//数据结构中与首地址的偏移;
};

_NDSHAREBASE_END

#endif

