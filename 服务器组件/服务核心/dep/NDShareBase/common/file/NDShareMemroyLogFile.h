#ifndef __SHARE_BASE_ND_SHARE_MEMORY_LOG_FILE_H__
#define __SHARE_BASE_ND_SHARE_MEMORY_LOG_FILE_H__

#include <vector>
using std::vector;

#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "sharememory/NDShareMemoryBase.h"

_NDSHAREBASE_BEGIN

class NDShareLogCacheSMU
{
private:
	NDSMUHead			m_NDSMUHead;
	NDUint32			m_nLogDataSize;						//m_szLogBuf中有效数据大小;
	NDUint32			m_nLogNameHash;						//log name's hash value;
	char				m_szLogName[MAX_PATH_LEN];			//log name path;
	char				m_szLogBuf[DEFAULT_LOG_CACHE_SIZE];	//log data;
public:
	NDShareLogCacheSMU()	{ clear(); }
	~NDShareLogCacheSMU()	{ clear(); }

	void		clear()
	{
		m_nLogDataSize = 0;
		m_nLogNameHash = 0;
		memset( &m_NDSMUHead, 0, sizeof(m_NDSMUHead) );
		memset( m_szLogName, 0, sizeof(m_szLogName) );
		memset( m_szLogBuf, 0, sizeof(m_szLogBuf) );
	}

	//锁定这个单元为nLockType拥有;
	NDBool		tryLock( NDUint16 nLockType );
	//解锁nUnlockType拥有这个单元;
	NDBool		tryUnlock( NDUint16 nUnlockType );

	//设置和获得锁定这个单元的nOwnType类型(设置函数只能特定进程调用);
	void		setOwnType( NDUint16 nOwnType )			{ m_NDSMUHead.m_nOwnType = nOwnType; }
	NDUint16	getOwnType() const						{ return m_NDSMUHead.m_nOwnType; }

	void		setPoolID( NDUint32 nPoolID )			{ m_NDSMUHead.m_nPoolID = nPoolID; }
	NDUint32	getPoolID() const						{ return m_NDSMUHead.m_nPoolID; }
	
	void		setUseStatus( NDUint8 nUseStatus )		{ m_NDSMUHead.m_nUseStatus = nUseStatus; }
	NDUint8		getUseStatus() const					{ return m_NDSMUHead.m_nUseStatus; }

	void		setSaveTime( NDTime nSeconds )			{ m_NDSMUHead.m_nSaveTime = nSeconds; }
	NDTime		getSaveTime() const						{ return m_NDSMUHead.m_nSaveTime; }

	void		setLogDataSize( NDUint32 nSize )		{ m_nLogDataSize = nSize; }
	NDUint32	getLogDataSize() const					{ return m_nLogDataSize; }

	NDUint32	getLogBufSize() const					{ return sizeof(m_szLogBuf); }

	void		setLogNameHash( NDUint32 nHash )		{ m_nLogNameHash = nHash; }
	NDUint32	getLogNameHash() const					{ return m_nLogNameHash; }

	NDBool		setLogName( const char* szLogName );
	const char* getLogName() const						{ return m_szLogName; }

	NDBool		writeLog( const char* szBuf, NDUint32 nBufSize );
	const char* getLogBuf()								{ return m_szLogBuf; }
};

class NDMutexLock;
class NDShareMemoryLogFile
{
private:
	NDUint32				m_nTotalSize;				//缓存+已经写入文件的大小;
	NDUint32				m_nCacheSize;				//缓存大小;
	NDShareLogCacheSMU*		m_pNDShareLogCacheSMU;
	NDMutexLock*			m_pLogMutex;
public:
	NDShareMemoryLogFile();
	~NDShareMemoryLogFile();

	NDBool		init( NDShareLogCacheSMU* pNDShareLogCacheSMU );
	void		release();

	//log文件内容的大小;
	NDUint32	size() const			{ return m_nTotalSize;	}
	//log cache size;
	NDUint32	cacheSize() const		{ return m_nCacheSize; }
	//log cache data size;
	NDUint32	cacheDataSize() const	
	{ 
		if ( NULL == m_pNDShareLogCacheSMU ) return (NDUint32)ND_INVALID_ID;
		return  m_pNDShareLogCacheSMU->getLogDataSize();
	}
	//log cache space size;
	NDUint32	cacheSpaceSize() const
	{
		if ( NULL == m_pNDShareLogCacheSMU ) return (NDUint32)ND_INVALID_ID;

		return ( m_nCacheSize - m_pNDShareLogCacheSMU->getLogDataSize() );
	}

	NDBool		write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... );
	NDBool		write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap );

	NDShareLogCacheSMU* getShareLogCacheSMU() { return m_pNDShareLogCacheSMU; }
};


template<typename T>
class NDShareMemoryUnitPool;

class NDShareMemoryLogManager
{
private:
	char*					m_pszLogPath;					//log路径名;
	char*					m_pszLogBaseName;				//log文件基本名称;
	NDUint32				m_nLogMaxSize;					//每个LOG文件的最大尺寸;
	NDShareLogCacheSMU*		m_pStandbyShareLogCacheSMU;		//备用的共享内存指针;
	NDShareMemoryLogFile*	m_pNDShareMemoryLogFile;

	NDShareMemoryUnitPool<NDShareLogCacheSMU>	*m_pNDShareLogCacheSMUPool;

public:
	NDShareMemoryLogManager();
	~NDShareMemoryLogManager();

	NDBool		init(  NDSM_KEY nKey, NDUint32 nUnitMax, NDUint16 nPoolType, const char* szLogPath, const char* szLogBaseName, NDUint32 nLogMaxSize=DEFAULT_LOG_FILE_MAX_SIZE );
	NDBool		write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... );
	NDBool		write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap );

private:
	NDShareMemoryLogManager(const NDShareMemoryLogManager&);
	NDShareMemoryLogManager& operator = (const NDShareMemoryLogManager&);

	NDBool		initFile( NDShareLogCacheSMU* pNDShareLogCacheSMU );
	NDBool		setLogNameInfo( NDShareLogCacheSMU* pNDShareLogCacheSMU );
	NDBool		getStandbyShareLogCacheSMU();
};


extern NDShareMemoryLogManager* g_pSMLogManager;

#define SET_NDSM_LOG_MGR(pSMLogManager) \
		g_pSMLogManager = pSMLogManager;

#define NDSM_LOG_INFO( info, ... ) \
	if( NULL != g_pSMLogManager )\
	{ g_pSMLogManager->write( APLOG_MARK, APLOG_INFO, info, ##__VA_ARGS__ ); }

#define NDSM_LOG_WARNING( info, ... ) \
	if( NULL != g_pSMLogManager ) \
	{ g_pSMLogManager->write( APLOG_MARK, APLOG_WARNING, info, ##__VA_ARGS__ ); }

#define NDSM_LOG_ERROR( info, ... ) \
	if( NULL != g_pSMLogManager )\
	{ g_pSMLogManager->write( APLOG_MARK, APLOG_ERR, info, ##__VA_ARGS__ ); }

_NDSHAREBASE_END

#endif

