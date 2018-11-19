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

//����INT������4���ַ�������,ʹ������;
//���ڴʸ�����,�ʸ���û�п϶�û�ڴ���;

_NDSHAREBASE_BEGIN

#ifndef ND_FilterWordRootStr_Max
#define ND_FilterWordRootStr_Max		(4)
#endif

class NDWordFilter
{
	enum EFilterWordRoot
	{
		EFilterWordRoot_Invalid,
		EFilterWordRoot_Continue,			//��������(���ʸ������дʵ�һ����);
		EFilterWordRoot_End,				//�Ѿ��ڴ���;
	};

	union FilterWordRootStrUnion
	{
		NDUint32	nNumber;
		char		szBuf[ND_FilterWordRootStr_Max];
	};
	
	struct FilterWordRoot
	{
		NDInt8					nFindFlag;		//�ʸ���ѯ��־;
		FilterWordRootStrUnion	bufUnion;		//�ʸ�����;

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


	FilterWordRootMap									m_FilterWordRootMap;	//�ʸ�;
	FilterWordMap										m_FilterWordMap;		//��;


public:
	NDWordFilter();
	~NDWordFilter();
	
	//�����д�;
	NDBool	AddFilterWord( const char* pStr );

	//ɾ�����д�;
	NDBool	DelFilterWord( const char* pStr );

	//�Ƿ������д�(��ȫƥ��);
	NDBool	IsHaveWholeFilterWord( const char* pStr );

	//�Ƿ������д�(�����ȫƥ��);
	NDBool	IsHaveFuzzyFilterWord( const char* pStr );

private:

	//�����дʸ�;
	NDBool	AddFilterWordRoot( const char* pStr );	

	//ɾ�����дʸ�;
	NDBool	DelFilterWordRoot( const char* pStr );

	//�Ƿ������дʸ�(��ȫƥ��);
	NDInt8	IsHaveWholeFilterWordRoot( const char* pStr, FilterWordRootStrUnion& refUnion );

	//�Ƿ������д�;
	NDBool	IsHaveFilterWord( const char* pStr, NDBool bWhole=NDTrue );

private:
	NDWordFilter( const NDWordFilter& other );
	NDWordFilter& operator = ( const NDWordFilter& other );


};

////for example:
//NDWordFilter wordFilter;
////�������δ��б�;
//wordFilter.AddFilterWord( "fuck" );
//
//NDBool bRet = wordFilter.IsHaveWholeFilterWord( "wofuck" );

_NDSHAREBASE_END

#endif // __SHARE_BASE_ND_WORD_FILTER_H__
