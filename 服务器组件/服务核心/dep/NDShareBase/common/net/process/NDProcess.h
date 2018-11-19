/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\process\NDProcess.h
	file base:	NDProcess
	purpose:	pure virtual process class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_PROCESS_H__
#define __SHARE_BASE_ND_PROCESS_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDIStream;

class NDProcess
{
public:
	virtual ~NDProcess(){}
	virtual NDBool process( NDIStream& rIStream ) = 0;
};


_NDSHAREBASE_END
#endif
