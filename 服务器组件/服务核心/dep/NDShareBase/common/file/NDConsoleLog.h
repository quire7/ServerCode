/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDConsoleLog.h
	file base:	NDConsoleLog
	purpose:	console log;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CONSOLE_LOG_H__
#define __SHARE_BASE_ND_CONSOLE_LOG_H__


#include <iostream>
#include "thread/NDMutex.h"

#ifdef WIN32
#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#endif



_NDSHAREBASE_BEGIN

#ifdef WIN32

#define TRED		FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN		FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE		TNORMAL | FOREGROUND_INTENSITY
#define TBLUE		FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#else

#define TRED		1
#define TGREEN		2
#define TYELLOW		3
#define TNORMAL		4
#define TWHITE		5
#define TBLUE		6

#endif


#ifndef WIN32
static const char* colorstrings[TBLUE+1] = {
		"",
		"\033[22;31m",
		"\033[22;32m",
		"\033[01;33m",
		//"\033[22;37m",
		"\033[0m",
		"\033[01;37m",
		"\033[22;34m",
};
#endif

struct NDConsoleColor 
{
	explicit NDConsoleColor(int attribute):m_color(attribute){};
	int m_color;//ÑÕÉ«Öµ;
};


#ifdef WIN32
	static HANDLE sHStdout	= ::GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	static NDSysLock* gPLogMutex = new	NDMutexLock;

template <class _Elem, class _Traits>
std::basic_ostream<_Elem,_Traits>& 
	operator<<(std::basic_ostream<_Elem,_Traits>& ostream, const NDConsoleColor& c)
{
	NDGuardLock logSysLock(*gPLogMutex);

#ifdef WIN32
	::SetConsoleTextAttribute(sHStdout, (WORD)c.m_color);
#else
	ostream << colorstrings[c.m_color];
#endif
	
	return ostream;
}

extern	NDBool	g_bConsole;
#define SET_NDCONSOLE_LOG_MGR( bConsole )\
		g_bConsole = bConsole;

#define NDCONSOLE_LOG_INFO		(std::cerr<<NDConsoleColor(TNORMAL))
#define NDCONSOLE_LOG_WARNING	(std::cerr<<NDConsoleColor(TGREEN|TRED))
#define NDCONSOLE_LOG_ERROR		(std::cerr<<NDConsoleColor(TRED))
#define NDCONSOLE_LOG_END		NDConsoleColor(TNORMAL)<<std::endl


class NDConsoleLog
{
public:
	NDConsoleLog(void);
	~NDConsoleLog(void);

	static void info( const char* info, ... );
	static void warning( const char* info, ... );
	static void error( const char* info, ... );
};

_NDSHAREBASE_END
#endif
