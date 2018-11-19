/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDCLogFile.h
	file base:	NDCLogFile
	purpose:	record log file operation;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_C_LOG_FILE_H__
#define __SHARE_BASE_ND_C_LOG_FILE_H__

#include <stdarg.h>

#include "NDTypes.h"
#include "NDShareBaseMacros.h"


_NDSHAREBASE_BEGIN

class NDCFile;
class NDMutexLock;
class NDCLogFile
{
private:
	NDUint32		m_nTotalSize;		//缓存+已经写入文件的大小;
	NDUint32		m_nCacheSize;		//缓存大小;
	NDUint32		m_nCacheLogPos;		//缓存中的数据大小;
	char*			m_szLogCache;		//缓存内存实体;
	NDCFile*		m_pNDCFile;			//真实的文件读写指针;
	NDMutexLock*	m_pLogMutex;

public:
	NDCLogFile( const char* pFileName, const char* pOpenFlag, NDUint32 nCacheSize=DEFAULT_LOG_CACHE_SIZE );
	~NDCLogFile(void);

	NDBool		isopen() const;
	void		close();
	//将缓存日志内存数据写入文件;
	NDBool		flush();
	NDUint32	size();

	NDBool		write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... );
	NDBool		write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap );

private:
    string		getWriteContent( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap );
	
};

class NDCLogManager
{
private:
	char*					m_pszLogPath;			//log路径名;
	char*					m_pszLogBaseName;		//log文件基本名称;
	NDUint32				m_nLogMaxSize;			//每个LOG文件的最大尺寸;
	NDCLogFile*				m_pLogFile;				//log文件的指针;

public:
	NDCLogManager();
	~NDCLogManager();
	
	NDBool	init( const char* szLogPath, const char* szLogBaseName, NDUint32 nLogMaxSize=DEFAULT_LOG_FILE_MAX_SIZE );
	NDBool	write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... );
	NDBool	write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap );

	//将缓存日志内存数据写入文件;
	NDBool	flush();
	//主动关闭;
	void	close();

private:
	NDCLogManager(const NDCLogManager&);
	NDCLogManager& operator = (const NDCLogManager&);

	NDBool	initFile();
};


/* info : char* it mustn't null */
/* pLogManager : NDCLogManager* it mustn't null */
extern NDCLogManager* g_pFileLogManager;
#define SET_NDFILE_LOG_MGR(pFileLogManager) \
	g_pFileLogManager = pFileLogManager;

#define NDFILE_LOG_INFO( info, ... )\
	if(NULL != g_pFileLogManager)	\
	{ g_pFileLogManager->write( APLOG_MARK, APLOG_INFO, info, ##__VA_ARGS__ ); }

#define NDFILE_LOG_WARNING( info, ... ) \
	if(NULL != g_pFileLogManager)	\
	{ g_pFileLogManager->write( APLOG_MARK, APLOG_WARNING, info, ##__VA_ARGS__ ); }

#define NDFILE_LOG_ERROR( info, ... ) \
	if(NULL != g_pFileLogManager)\
	{ g_pFileLogManager->write( APLOG_MARK, APLOG_ERR, info, ##__VA_ARGS__ ); }

_NDSHAREBASE_END
#endif
