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
	NDUint32		m_nTotalSize;		//����+�Ѿ�д���ļ��Ĵ�С;
	NDUint32		m_nCacheSize;		//�����С;
	NDUint32		m_nCacheLogPos;		//�����е����ݴ�С;
	char*			m_szLogCache;		//�����ڴ�ʵ��;
	NDCFile*		m_pNDCFile;			//��ʵ���ļ���дָ��;
	NDMutexLock*	m_pLogMutex;

public:
	NDCLogFile( const char* pFileName, const char* pOpenFlag, NDUint32 nCacheSize=DEFAULT_LOG_CACHE_SIZE );
	~NDCLogFile(void);

	NDBool		isopen() const;
	void		close();
	//��������־�ڴ�����д���ļ�;
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
	char*					m_pszLogPath;			//log·����;
	char*					m_pszLogBaseName;		//log�ļ���������;
	NDUint32				m_nLogMaxSize;			//ÿ��LOG�ļ������ߴ�;
	NDCLogFile*				m_pLogFile;				//log�ļ���ָ��;

public:
	NDCLogManager();
	~NDCLogManager();
	
	NDBool	init( const char* szLogPath, const char* szLogBaseName, NDUint32 nLogMaxSize=DEFAULT_LOG_FILE_MAX_SIZE );
	NDBool	write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... );
	NDBool	write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap );

	//��������־�ڴ�����д���ļ�;
	NDBool	flush();
	//�����ر�;
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
