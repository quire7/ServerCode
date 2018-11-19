/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDCFile.h
	file base:	NDCFile
	purpose:	file operation(ANSI C);
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_C_FILE_H__
#define __SHARE_BASE_ND_C_FILE_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDCFileImpl;
class NDCFile
{
private:
	NDCFileImpl*	m_pNDCFileImpl;

public:
	NDCFile(void);
	~NDCFile(void);
	NDBool		isopen() const;
	NDBool		open( const char* pFileName, const char* pOpenFlag );

	NDBool		readline( void* pReadBuf, NDUint32 nSize );

	NDUint32	read( void* pReadBuf, NDUint32 nSize );
	NDUint32	write( const void* pWriteBuf, NDUint32 nSize );
	NDUint32	size();
	void		close();
	NDInt32		flush();	//如果成功刷新,flush返回0;

};

_NDSHAREBASE_END

#endif

