/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDExcelFile.h
	file base:	NDExcelFile
	purpose:	read NDExcelFile file operation(ANSI C);(Ҫ����ת�������EXCEL�ļ�);
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
	//�ֶ���������;
	enum FIELD_TYPE
	{
		T_INT		= 0,	//����;
		T_FLOAT		= 1,	//������;
		T_STRING	= 2,	//�ַ���;
	};

	//���ݶθ�ʽ����;
	typedef vector< FIELD_TYPE >	FILEDS_TYPE;

	//���ݶ�;
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
	//������;
	typedef vector< FIELD >		DATA_BUF;

private:
	char*			m_szFileName;							//�����ļ�·��;
	char*			m_szModifyData;							//�������������;(��\0�������);
	NDUint32		m_nFileSize;							//�ļ���С;

	NDUint32		m_nRowNum;								//��Ч����;
	NDUint32		m_nColumnNum;							//��Ч����;
	FILEDS_TYPE		m_columnType;							//���ݶθ�ʽ����;
	DATA_BUF		m_vDataBuf;								//������;(size=m_nRowNum*m_nColumnNum);

public:
	NDExcelFile();
	~NDExcelFile();

	NDBool			open( const char* szFileName );			//�������ļ�;
	NDBool			reOpen();								//���´������ļ�;
	void			close();								//�ر������ļ�;

	NDUint32		getFileSize() const { return m_nFileSize; }
	NDUint32		getRowNum() const	{ return m_nRowNum; }
	NDUint32		getColumnNum() const{ return m_nColumnNum; }
	
	const char*		getValue( NDUint32 nRowNum, NDUint32 nColumn ) const;							//����ַ���(�±��0��ʼ);
	NDBool			getInt32( NDUint32 nRowNum, NDUint32 nColumn, NDInt32 &refValue ) const;		//���INT(�±��0��ʼ);
	NDBool			getFloat32( NDUint32 nRowNum, NDUint32 nColumn, NDFloat32 &refValue ) const;	//���FLOAT(�±��0��ʼ);

private:
	NDBool			modifyData();							//��������;
	const FIELD*	getFIELD( NDUint32 nRowNum, NDUint32 nColumn ) const;
	char*			getLineFromMemory( char* szOutBuf, NDUint32 nOutBufSize, const char* pMemory, const char* pMemoryEnd );
};

_NDSHAREBASE_END

#endif

