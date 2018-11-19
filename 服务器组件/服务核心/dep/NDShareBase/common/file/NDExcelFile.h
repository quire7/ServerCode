/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDExcelFile.h
	file base:	NDExcelFile
	purpose:	read NDExcelFile file operation(ANSI C);(要求是转换过后的EXCEL文件);
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_EXCEL_FILE_H__
#define __SHARE_BASE_ND_EXCEL_FILE_H__

#include "NDTypes.h"

#include <vector>
using std::vector;

_NDSHAREBASE_BEGIN

class NDExcelFile
{
private:
	//字段数据类型;
	enum FIELD_TYPE
	{
		T_INT		= 0,	//整数;
		T_FLOAT		= 1,	//浮点数;
		T_STRING	= 2,	//字符串;
	};

	//数据段格式描述;
	typedef vector< FIELD_TYPE >	FILEDS_TYPE;

	//数据段;
	union FIELD
	{
		NDFloat32	fValue;
		NDInt32		iValue;
		const char*	pString;	// Just for runtime!

		//Construct;
		FIELD() {}
		FIELD(NDInt32 value)		{ iValue	= value; }
		FIELD(NDFloat32 value)		{ fValue	= value; }
		FIELD(const char* value)	{ pString	= value; }
	};
	//数据区;
	typedef vector< FIELD >		DATA_BUF;

private:
	char*			m_szFileName;							//配置文件路径;
	char*			m_szModifyData;							//修正过后的数据;(加\0后的数据);
	NDUint32		m_nFileSize;							//文件大小;

	NDUint32		m_nRowNum;								//有效行数;
	NDUint32		m_nColumnNum;							//有效列数;
	FILEDS_TYPE		m_columnType;							//数据段格式描述;
	DATA_BUF		m_vDataBuf;								//数据区;(size=m_nRowNum*m_nColumnNum);

public:
	NDExcelFile();
	~NDExcelFile();

	NDBool			open( const char* szFileName );			//打开配置文件;
	NDBool			reOpen();								//重新打开配置文件;
	void			close();								//关闭配置文件;

	NDUint32		getFileSize() const { return m_nFileSize; }
	NDUint32		getRowNum() const	{ return m_nRowNum; }
	NDUint32		getColumnNum() const{ return m_nColumnNum; }
	
	const char*		getValue( NDUint32 nRowNum, NDUint32 nColumn ) const;							//获得字符串(下标从0开始);
	NDBool			getInt32( NDUint32 nRowNum, NDUint32 nColumn, NDInt32 &refValue ) const;		//获得INT(下标从0开始);
	NDBool			getFloat32( NDUint32 nRowNum, NDUint32 nColumn, NDFloat32 &refValue ) const;	//获得FLOAT(下标从0开始);

private:
	NDBool			modifyData();							//修正数据;
	const FIELD*	getFIELD( NDUint32 nRowNum, NDUint32 nColumn ) const;
	char*			getLineFromMemory( char* szOutBuf, NDUint32 nOutBufSize, const char* pMemory, const char* pMemoryEnd );
};

_NDSHAREBASE_END

#endif

