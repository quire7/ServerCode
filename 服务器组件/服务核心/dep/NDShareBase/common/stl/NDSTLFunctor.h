/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\stl\NDSTLFunctor.h
	file base:	NDSTLFunctor
	purpose:	stl functor;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_STL_FUNCTOR_H__
#define __SHARE_BASE_ND_STL_FUNCTOR_H__

#include <string.h>

_NDSHAREBASE_BEGIN

struct cStrLess
{
	bool operator()( const char* lh, const char* rh ) const
	{
		return ( strcmp( lh, rh) < 0 );
	}
};



_NDSHAREBASE_END

#endif

