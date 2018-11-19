/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\function\NDWordFilter.h
	file base:	NDWordFilter
	purpose:	word filter;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_WORD_FILTER_H__
#define __SHARE_BASE_ND_WORD_FILTER_H__

#include <map>
using std::map;

#include "NDTypes.h"

//利用INT正好是4个字符的特性,使用联合;
//现在词根中找,词根中没有肯定没在词中;

_NDSHAREBASE_BEGIN

#ifndef ND_FilterWordRootStr_Max
#define ND_FilterWordRootStr_Max		(4)
#endif

class NDWordFilter
{
	enum EFilterWordRoot
	{
		EFilterWordRoot_Invalid,
		EFilterWordRoot_Continue,			//继续查找(即词根是敏感词的一部分);
		EFilterWordRoot_End,				//已经在词中;
	};

	union FilterWordRootStrUnion
	{
		NDUint32	nNumber;
		char		szBuf[ND_FilterWordRootStr_Max];
	};
	
	struct FilterWordRoot
	{
		NDInt8					nFindFlag;		//词根查询标志;
		FilterWordRootStrUnion	bufUnion;		//词根联合;

		FilterWordRoot()
		{
			memset( this, 0, sizeof(FilterWordRoot) );
		}
	};

private:
	typedef	map< NDUint32, FilterWordRoot >				FilterWordRootMap;
	typedef	map< NDUint32, FilterWordRoot >::iterator	FilterWordRootMapIter;

	typedef	map< NDUint32, string >						FilterWordMap;
	typedef	map< NDUint32, string >::iterator			FilterWordMapIter;


	FilterWordRootMap									m_FilterWordRootMap;	//词根;
	FilterWordMap										m_FilterWordMap;		//词;


public:
	NDWordFilter();
	~NDWordFilter();
	
	//加敏感词;
	NDBool	AddFilterWord( const char* pStr );

	//删除敏感词;
	NDBool	DelFilterWord( const char* pStr );

	//是否含有敏感词(完全匹配);
	NDBool	IsHaveWholeFilterWord( const char* pStr );

	//是否含有敏感词(最大完全匹配);
	NDBool	IsHaveFuzzyFilterWord( const char* pStr );

private:

	//加敏感词根;
	NDBool	AddFilterWordRoot( const char* pStr );	

	//删除敏感词根;
	NDBool	DelFilterWordRoot( const char* pStr );

	//是否有敏感词根(完全匹配);
	NDInt8	IsHaveWholeFilterWordRoot( const char* pStr, FilterWordRootStrUnion& refUnion );

	//是否含有敏感词;
	NDBool	IsHaveFilterWord( const char* pStr, NDBool bWhole=NDTrue );

private:
	NDWordFilter( const NDWordFilter& other );
	NDWordFilter& operator = ( const NDWordFilter& other );


};

////for example:
//NDWordFilter wordFilter;
////加载屏蔽词列表;
//wordFilter.AddFilterWord( "fuck" );
//
//NDBool bRet = wordFilter.IsHaveWholeFilterWord( "wofuck" );

_NDSHAREBASE_END

#endif // __SHARE_BASE_ND_WORD_FILTER_H__
