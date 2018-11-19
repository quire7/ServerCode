#ifndef __SHARE_BASE_ND_TOTAL_LOG_H__
#define __SHARE_BASE_ND_TOTAL_LOG_H__


#include "NDCLibFun.h"
#include "NDCLogFile.h"
#include "NDConsoleLog.h"
#include "NDShareMemroyLogFile.h"


#ifndef SET_NDLOG_MGR
#define SET_NDLOG_MGR( pFileLogMgr, pSMLogMgr, bConsole )\
		SET_NDCONSOLE_LOG_MGR( bConsole )\
		SET_NDFILE_LOG_MGR( pFileLogMgr )\
		SET_NDSM_LOG_MGR( pSMLogMgr )
#endif

//调用时info的变量名一定不能为szConsoleBuf,或则控制台打印出错,宏只是代码替换;

#ifndef NDLOG_INFO
#define NDLOG_INFO( info, ... ) \
	do { \
		if (g_bConsole) { \
			char szConsoleBuf[BUF_LEN_1024] = { 0 }; \
			ND_SNPRINTF( szConsoleBuf, sizeof(szConsoleBuf)-1, info, ##__VA_ARGS__ ); \
			NDCONSOLE_LOG_INFO << szConsoleBuf << NDCONSOLE_LOG_END; \
		} \
		NDFILE_LOG_INFO( info, ##__VA_ARGS__ )	\
		NDSM_LOG_INFO( info, ##__VA_ARGS__ )	\
	} while(0);
#endif

#ifndef NDLOG_WARNING
#define NDLOG_WARNING( info, ... ) \
	do { \
		if (g_bConsole) { \
			char szConsoleBuf[BUF_LEN_1024] = { 0 }; \
			ND_SNPRINTF( szConsoleBuf, sizeof(szConsoleBuf)-1, info, ##__VA_ARGS__ ); \
			NDCONSOLE_LOG_WARNING << szConsoleBuf << NDCONSOLE_LOG_END; \
		} \
		NDFILE_LOG_WARNING( info, ##__VA_ARGS__ )	\
		NDSM_LOG_WARNING( info, ##__VA_ARGS__ )	\
	} while(0);
#endif

#ifndef NDLOG_ERROR
#define NDLOG_ERROR( info, ... ) \
	do { \
		if ( g_bConsole ) { \
			char szConsoleBuf[BUF_LEN_1024] = { 0 }; \
			ND_SNPRINTF( szConsoleBuf, sizeof(szConsoleBuf)-1, info, ##__VA_ARGS__ ); \
			NDCONSOLE_LOG_ERROR << szConsoleBuf << NDCONSOLE_LOG_END; \
		} \
		NDFILE_LOG_ERROR( info, ##__VA_ARGS__ ) \
		NDSM_LOG_ERROR( info, ##__VA_ARGS__ )	\
	} while(0);
#endif

#endif

