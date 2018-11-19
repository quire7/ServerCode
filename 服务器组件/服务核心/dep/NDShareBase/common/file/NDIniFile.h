/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDIniFile.h
	file base:	NDIniFile
	purpose:	read INI file operation(ANSI C);(要求最后一行是空行);
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
	typedef	map<char*, char*, cStrLess>				PropertyValueMap;		//属性值对;
	typedef	PropertyValueMap::iterator				PropertyValueMapIter;
	typedef map<char*, PropertyValueMap, cStrLess>	SectionValueMap;		//段值对;
	typedef SectionValueMap::iterator				SectionValueMapIter;

private:
	char*			m_szFileName;							//配置文件路径;
	char*			m_szModifyData;							//修正过后的数据;(加\0后的数据);
	NDUint32		m_nFileSize;							//文件大小;
	SectionValueMap	m_SectionValueMap;						//所有段值的map;

public:
	NDIniFile();
	~NDIniFile();

	NDBool			open( const char* szFileName );			//打开配置文件;
	NDBool			reOpen();								//重新打开配置文件;
	void			close();								//关闭配置文件;

	const char*		getValue( const char* szSection, const char* szKey ) const;							//获得字符串;
	NDBool			getInt32( const char* szSection, const char* szKey, NDInt32 &refValue ) const;
	NDBool			getUint32( const char* szSection, const char* szKey, NDUint32 &refValue ) const;
	NDBool			getFloat32( const char* szSection, const char* szKey, NDFloat32 &refValue ) const;
	NDBool			getFloat64( const char* szSection, const char* szKey, NDFloat64 &refValue ) const;

private:
	NDBool			modifyData();							//修正数据;
};

_NDSHAREBASE_END

#endif

