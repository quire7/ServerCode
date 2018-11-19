/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\function\NDConsoleCMDManager.h
	file base:	NDConsoleCMDManager
	purpose:	design for console cmd or gm cmd dispose;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_CONSOLE_CMD_MANAGER_H__
#define __SHARE_BASE_ND_CONSOLE_CMD_MANAGER_H__

#include <vector>
using std::vector;

#include <map>
using std::map;

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDConsoleCommand
{
public:
	virtual ~NDConsoleCommand()=0;
	virtual NDBool	process( vector<string>& refStrVec ) = 0;
};


class NDConsoleCMDManager
{
public:
	NDConsoleCMDManager();
	~NDConsoleCMDManager();

	void		registerCommad( const string& strKey, NDConsoleCommand* pConsoleCMD );
	
	NDBool		process( const string& strMsg );

private:
	NDConsoleCMDManager( const NDConsoleCMDManager& other );
	NDConsoleCMDManager& operator = ( const NDConsoleCMDManager& other );

private:
	typedef	map<string, NDConsoleCommand*>		ConsoleCMDMap;
	typedef ConsoleCMDMap::iterator				ConsoleCMDMapIter;	

	ConsoleCMDMap								m_consoleCMDMap;
};

_NDSHAREBASE_END

#endif
