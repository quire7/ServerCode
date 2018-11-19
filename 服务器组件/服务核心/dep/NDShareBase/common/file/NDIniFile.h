/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDIniFile.h
	file base:	NDIniFile
	purpose:	read INI file operation(ANSI C);(Ҫ�����һ���ǿ���);
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_INI_FILE_H__
#define __SHARE_BASE_ND_INI_FILE_H__

#include "NDTypes.h"
#include "stl/NDSTLFunctor.h"

#include <map>
using std::map;

_NDSHAREBASE_BEGIN


class NDIniFile
{
private:
	typedef	map<char*, char*, cStrLess>				PropertyValueMap;		//����ֵ��;
	typedef	PropertyValueMap::iterator				PropertyValueMapIter;
	typedef map<char*, PropertyValueMap, cStrLess>	SectionValueMap;		//��ֵ��;
	typedef SectionValueMap::iterator				SectionValueMapIter;

private:
	char*			m_szFileName;							//�����ļ�·��;
	char*			m_szModifyData;							//�������������;(��\0�������);
	NDUint32		m_nFileSize;							//�ļ���С;
	SectionValueMap	m_SectionValueMap;						//���ж�ֵ��map;

public:
	NDIniFile();
	~NDIniFile();

	NDBool			open( const char* szFileName );			//�������ļ�;
	NDBool			reOpen();								//���´������ļ�;
	void			close();								//�ر������ļ�;

	const char*		getValue( const char* szSection, const char* szKey ) const;							//����ַ���;
	NDBool			getInt32( const char* szSection, const char* szKey, NDInt32 &refValue ) const;
	NDBool			getUint32( const char* szSection, const char* szKey, NDUint32 &refValue ) const;
	NDBool			getFloat32( const char* szSection, const char* szKey, NDFloat32 &refValue ) const;
	NDBool			getFloat64( const char* szSection, const char* szKey, NDFloat64 &refValue ) const;

private:
	NDBool			modifyData();							//��������;
};

_NDSHAREBASE_END

#endif

