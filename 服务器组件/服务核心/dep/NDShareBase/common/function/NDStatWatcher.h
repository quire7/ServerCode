/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\function\NDStatWatcher.h
	file base:	NDStatWatcher
	purpose:	statistic watcher;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_STAT_WATCHER_H__
#define __SHARE_BASE_ND_STAT_WATCHER_H__

#include <iosfwd>
using std::stringstream;

#include <vector>
using std::vector;
#include <map>
using std::map;

#include "NDTypes.h"



_NDSHAREBASE_BEGIN

#ifndef ST_PROTIME
#define ST_PROTIME			"ProcTime"
#define ST_PROTIME_INDEX	0
#endif

class StatData
{
public:
	struct StatItemBaseInfo
	{
		NDUint64	nTotal;		//ͳ����ֵ;
		NDUint64	nMaxV;		//ͳ���е����ֵ;
		NDUint64	nMinV;		//ͳ���е���Сֵ;
		NDUint64	nDiff;		//ͳ���еĲ�ֵ;
	};
	struct StatItemInfo
	{
		NDUint64			nCount;				//ͳ�Ƶ�����ͳ�ƵĴ���;
		StatItemBaseInfo	bsInfoContent;		//ͳ�Ƶ�����;
		StatItemBaseInfo	bsInfoTime;			//ͳ�Ƶ�����Ҫ��ʱ��;
		StatItemInfo() { memset( this, 0, sizeof( StatItemInfo ) ); };
	};
	typedef vector<StatItemInfo>	StatItemInfoVec;	//key : items ( e.g totalbyes );

	struct StatInfo
	{
		NDUint32			nTypeID;			//����ID;
		StatItemInfoVec		itemInfoVec;		//Ҫͳ�Ƶ�����������Ϣ;
		StatInfo() : nTypeID(0)
		{
			itemInfoVec.clear();
		}
	};
	typedef map< NDUint32, StatInfo >	StatInfoMap;	//key : type ( e.g msgcode );

public:
	StatData( const char** pItemVec, NDUint32 nSize );
	~StatData();

	void			RecordConst( StatInfo* pInfo, NDUint32 nItem, NDUint64 nValue );
	void			BeginDiff( StatInfo* pInfo, NDUint32 nItem, NDUint64 nValue );
	void			EndDiff( StatInfo* pInfo, NDUint32 nItem, NDUint64 nValue );
	void			Count( StatInfo* pInfo, NDUint32 nItem );

	NDUint64		GetTotal( StatInfo* pInfo, NDUint32 nItem );
	NDUint64		GetMax( StatInfo* pInfo, NDUint32 nItem );
	NDUint64		GetMin( StatInfo* pInfo, NDUint32 nItem );
	float			GetAvg( StatInfo* pInfo, NDUint32 nItem );

	void			SaveArrayTypeLog( const char* pFileName, const char** pArrayAddr, NDUint32 nLen );
	void			SaveMapTypeLog( const char* pFileName, map< NDUint32, string >& refStrMap );

	StatInfo*		GetStatInfo( NDUint32 nTypeID );
	StatItemInfo*	GetStatItemInfo( NDUint32 nTypeID, NDUint32 nItem, NDBool bCount=NDFalse );

private:
	StatItemInfo*	GetStatItemInfo( StatInfo* pInfo, NDUint32 nItem );

	void			MakeArrayStat( stringstream& refStream, const char** pArrayAddr, NDUint32 nLen );
	void			MakeMapStdStat( stringstream& refStream, map< NDUint32, string >& refStrMap );
	void			MakeStdStatHead( stringstream& refStream );
	void			MakeStatItemInfo( stringstream& refStream, StatItemInfoVec& refItemInfoVec );
private:
	NDUint32		m_nItemSize;
	vector<string>	m_itemsStringVec;
	StatInfoMap		m_StatInfoMap;
};


class NDStatWatcher
{
public:
	NDStatWatcher( StatData* pData, NDUint32 nType ) : m_pStatData ( pData ), m_pStatInfo( NULL )
	{
		Begin( pData, nType );
	}
	~NDStatWatcher()
	{
		End();
	}
	
	//��¼��������в�����;
	void			Record( NDUint32 nItem, NDUint64 nValue );
	//��¼��������п��ܱ仯����;
	void			BeginDiff( NDUint32 nItem, NDUint64 nValue );
	void			EndDiff( NDUint32 nItem, NDUint64 nValue );

private:
	void			Begin( StatData* pData, NDUint32 nType );
	void			End();

private:
	StatData*			m_pStatData;
	StatData::StatInfo*	m_pStatInfo;
};

#ifndef __PROF_FUNC_TICK__
#define	__PROF_FUNC_TICK__	\
	static NDBool bFirst	= NDTrue; \
	static NDUint32 nHash	= 0; \
	if ( bFirst ) \
	{ \
		bFirst	= NDFalse; \
		nHash	= NDShareBaseGlobal::nd_bkdr_hash( FUNC_NAME ); \
		NDShareBaseGlobal::g_sStatFuncNameMap.insert( std::make_pair( nHash, FUNC_NAME ) ); \
	} \
	NDStatWatcher watcher( NDShareBaseGlobal::g_spStatFunc, nHash );

#endif


_NDSHAREBASE_END

#endif // __SHARE_BASE_ND_STAT_WATCHER_H__


